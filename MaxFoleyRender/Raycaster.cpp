//
//  Raycaster.cpp
//  MaxFoleyRender
//
//  Created by Maxwell Foley on 1/28/18.
//  Copyright © 2018 Maxwell Foley. All rights reserved.
//

#include <stdio.h>
#include "common.h"
#include "Raycaster.h"
#include <thread>

using namespace MFR;


void Raycaster::RenderImage(std::shared_ptr<Scene> scene, Color * colorBuffer, RaycasterOptions options)
{
	int width = options.width;
	int height = options.height;
	
	int numPix = width*height;
	Color * modulationBuffer = new Color[numPix];

	Ray * rayBuffer = new Ray[numPix];
	std::shared_ptr<Surfel> * surfelBuffer = new std::shared_ptr<Surfel>[numPix];
	Color * biradianceBuffer = new Color[numPix];
	Ray * shadowRayBuffer = new Ray[numPix];
	bool * lightShadowedBuffer = new bool[numPix];


	scene->poseMesh();
	TriTree tt = scene->posedMesh;
	
	std::vector<std::shared_ptr<Light>> lights = scene->lights;
    
	for(int i = 0; i < options.numRays; i++)
	{
		std::cout << "generating rays" << std::endl;
		multithread(GenerateRays,numPix,rayBuffer,scene->camera,options);
		
		//initialize the modulationBuffer to Color3(1 / paths per pixel)
		for (int i2 = 0; i2 < numPix; ++i2) {
			modulationBuffer[i2] = Color(1.0/options.numRays,1.0/options.numRays,1.0/options.numRays);
		}

        std::cout << "intersecting rays" << std::endl;
        IntersectRays(0,numPix, rayBuffer, surfelBuffer, tt);
       // multithread(IntersectRays,numPix,rayBuffer, surfelBuffer, tt);
        
        
        std::cout << "adding emissive terms" << std::endl;
        multithread(AddEmissiveTerms,numPix,rayBuffer, colorBuffer, surfelBuffer, modulationBuffer);

        
        if(lights.size() > 0)
        {
            //in this function to the next, use lightshadowedbuffer to transmit whether the light casts shadows at all
            std::cout << "getting light info" << std::endl;
            multithread(GetLightInfo, numPix, lights, surfelBuffer, biradianceBuffer, shadowRayBuffer, lightShadowedBuffer);
            
            std::cout << "shadow tests" << std::endl;
            ShadowTests(0, numPix, shadowRayBuffer, surfelBuffer, lightShadowedBuffer, tt);
         //   multithread(ShadowTests, numPix, shadowRayBuffer, surfelBuffer, lightShadowedBuffer, tt);
            
            std::cout << "shading pixels" << std::endl;
            multithread(ShadePixels, numPix, surfelBuffer, shadowRayBuffer, colorBuffer, biradianceBuffer, modulationBuffer, lightShadowedBuffer);
        }
        
        if(i < options.numScatteringEvents-1)
        {
            std::cout << "scattering rays" << std::endl;
            multithread(ScatterRays, numPix, rayBuffer, surfelBuffer, modulationBuffer, options);

        }

		std::cout << "finished!" << std::endl;

	}
	delete[] modulationBuffer;
	//delete[] rayBuffer;
	delete[] surfelBuffer;
	delete[] biradianceBuffer;
	delete[] shadowRayBuffer;
	delete[] lightShadowedBuffer;
}
template <class _Fp, class ..._Args>
void Raycaster::multithread(_Fp&& __f, int num, _Args&&... __args){
    int threads = 1;//std::thread::hardware_concurrency();
	
	std::thread *t = new std::thread[threads];
	for(int i = 0; i < threads; i++)
	{
		int begin = (num/threads)*i;
		int end = (num/threads)*(i+1);
		if(end > num) { end = num;}
		t[i] = std::thread(__f,begin,end, (__args)...);
	}
	KillThreads(t,threads);
	delete [] t;

}


void Raycaster::GenerateRays(int begin, int end, Ray * rayBuffer, std::shared_ptr<Camera> camera, RaycasterOptions options)
{
	for(int i = begin; i < end; i++)
	{
		int x = i % options.width;
		int y = i / options.width;
		
		//calculate ray direction and origin based on camera position
		const float side = -2.0f * tan(	camera->fov  / 2.0f);
		
		Point p = Point( camera->focalLength * (float(x) / float(options.width) - 0.5f) * side * float(options.width) / float(options.height), camera->focalLength * -(float(y) / float(options.height) - 0.5f) * side, camera->focalLength)  + camera->position;
		
		Vector dir = p-camera->position;
		dir = dir.unit();
		Ray ray = Ray(p,dir);
		//TODO: rotate ray if camera is rotated
		
		rayBuffer[i] = ray;
	}

}

void Raycaster::AddEmissiveTerms(int begin, int end, Ray * rayBuffer, Color * colorBuffer, std::shared_ptr<Surfel> * surfelBuffer, Color * modulationBuffer)
{
	for(int i = begin; i < end; i++)
	{
		if(surfelBuffer[i])
		{
			//debugPrintf("modulation %f %f %f",modulationBuffer[img->width()*p.y + p.x].r,modulationBuffer[img->width()*p.y + p.x].g,modulationBuffer[img->width()*p.y + p.x].b);
			Vector wo = -rayBuffer[i].direction;
			Color addTo = surfelBuffer[i]->material->ambient*modulationBuffer[i];
			
			colorBuffer[i] += addTo;
		}
	}
}
void Raycaster::IntersectRays(int begin, int end, Ray * rayBuffer, std::shared_ptr<Surfel> * surfelBuffer, TriTree & tt)
{
    auto longestDuration = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now() - std::chrono::high_resolution_clock::now()
    ).count();
    
    double averageDuration = 0.0;
    
	for(int i = begin; i < end; i++)
	{

        ///
        
		Ray ray = rayBuffer[i];
		
		Point P = ray.origin;
		Vector w = ray.direction;
		
		float b[3] = {0,0,0};
		float distance = 10000000;
        auto t1 = std::chrono::high_resolution_clock::now();

		TriResult result = tt.intersectRay(ray, b, distance);
		
        auto t2 = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
        if(duration > longestDuration) {
            longestDuration = duration;
          //  logLevel = 2;
            tt.intersectRay(ray, b, distance);
            //std::cout << i << std::endl;
           // logLevel = 1;
        }
        averageDuration+=duration;

		if(result.found)
		{
            Tri foundTri = result.tri;
			Vector normal = -(foundTri.points[1] - foundTri.points[0]).cross(foundTri.points[2] - foundTri.points[1]).unit();
			std::shared_ptr<Surfel> surfel = std::make_shared<Surfel>(ray.origin + distance*ray.direction,normal,foundTri.material);
			surfelBuffer[i] = surfel;
		}
		else {
			surfelBuffer[i] = nullptr;
		}
        
        ///
        
	}
    averageDuration /= (end-begin);
    std::cout << "Longest dur" << longestDuration << "\n";
    std::cout << "Average dur" << averageDuration << "\n";
    std::cout << "Average nodes hit" << tt.averageNodesTouched() << "\n";



}

void Raycaster::GetLightInfo(int begin, int end, std::vector<std::shared_ptr<Light>> lights, std::shared_ptr<Surfel> * surfelBuffer, Color * biradianceBuffer, Ray* shadowRayBuffer, bool* lightShadowedBuffer)
{
	for(int i = begin; i < end; i++)
	{
		//make sure surfel was found
		if(surfelBuffer[i] != NULL)
		{
			std::shared_ptr<Light> light;
			float weight = 1.0;
			std::shared_ptr<Surfel> surfel = surfelBuffer[i];
			
            //TODO, this code is clearly flawed
			if(lights.size()> 1)
			{
               // light = lights[1];
               // weight = 1.0;
				//calculate total biradiance
				float totalBiradiance = 0.0;
				for(int j = 0; j < lights.size(); j++)
				{
					Color b = lights[j]->biradiance(surfel->position);
					totalBiradiance += b.r + b.g + b.b;
				}
				
				float randomCounter = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/totalBiradiance);

				light = lights[0];
				
				for(int j = 0; randomCounter > 0; j++)
				{
					light = lights[j];
					Color b = light->biradiance(surfel->position);
					randomCounter -= b.r+b.g+b.b;
					weight = totalBiradiance/(b.r+b.g+b.b);
				}
                              
			}
			//if theres just one light, just choose it
			//else
			//{
				light = lights[0];
				weight = 1.0;
			//}
			
			Point surfelPos = surfel->position;
            biradianceBuffer[i] = light->biradiance(surfelPos) * weight;

            Vector pos = light->position;
			Vector lightDir = pos - surfelPos;
			lightDir = lightDir.unit();
			//for now put in lightShadowedBuffer simply whether the light casts shadows at all
			lightShadowedBuffer[i] = light->shadowsEnabled;
			shadowRayBuffer[i] = Ray(pos, -lightDir);
			
		}
	}
}
void Raycaster::ShadowTests(int begin, int end, Ray * shadowRayBuffer, std::shared_ptr<Surfel> * surfelBuffer, bool* lightShadowedBuffer, TriTree & tt) {
	for(int i = begin; i < end; i++)
	{
		if(surfelBuffer[i])
		{
			//if light casts shadows, test it, otherwise keep the value at false
			if(lightShadowedBuffer[i])
			{
				float b[3] = {0,0,0};
				float dist;
				
				TriResult result = tt.intersectRay(shadowRayBuffer[i],b,dist);
				if(result.found != NULL && dist < (shadowRayBuffer[i].origin - surfelBuffer[i]->position).length() -.1)
				{
					lightShadowedBuffer[i] = true;
				}
				else
				{
					lightShadowedBuffer[i] = false;
				}
			}
			
		}
	}
}

void Raycaster::ShadePixels(int begin, int end, std::shared_ptr<Surfel> * surfelBuffer, Ray * shadowRayBuffer, Color * colorBuffer, Color * biradianceBuffer, Color * modulationBuffer, bool * lightShadowedBuffer) {
	
	for(int i = begin; i < end; i++)
	{
		if(!lightShadowedBuffer[i] && surfelBuffer[i])		{
			std::shared_ptr<Surfel> surfel = surfelBuffer[i];
			Vector lightDir = -shadowRayBuffer[i].direction;
			float multiplier = std::abs(lightDir.dot(surfel->normal));

			//shade the pixel

			Color addTo = surfel->material->diffuse * multiplier * biradianceBuffer[i]; //* multiplier
			if(addTo.r < 0.0){ addTo.r = 0.0;}
			if(addTo.g < 0.0){ addTo.g = 0.0;}
			if(addTo.b < 0.0){ addTo.b = 0.0;}

            colorBuffer[i] += addTo;
		}
		
		//make it gray if the ray missed the scene
		if(!surfelBuffer[i] )
		{
			colorBuffer[i] += Color(0.5f, 0.5f, 0.5f)*modulationBuffer[i];
		}
		
	}
}
void Raycaster::ScatterRays(int begin, int end, Ray * rayBuffer, std::shared_ptr<Surfel> * surfelBuffer, Color * modulationBuffer, RaycasterOptions options)
{
	for(int i = begin; i < end; i++)
	{
		if(surfelBuffer[i])
		{
			std::shared_ptr<Surfel> surfel = surfelBuffer[i];

			Vector w_before = -1 * rayBuffer[i].direction;
			
			Vector w_after;
			Color weight;
			
			surfel->scatter(w_before, weight, w_after);
			modulationBuffer[i]*=weight;
			
			float dotSign = w_after.dot(surfel->normal);
			dotSign = dotSign / std::abs(dotSign);
			rayBuffer[i] = Ray(surfel->position +  surfel->normal * .1 * dotSign, w_after);
		}
	}
}

void Raycaster::KillThreads(std::thread * t, int numThreads)
{
        for (int i = 0; i < numThreads; ++i)
		{
             t[i].join();
		}
}
