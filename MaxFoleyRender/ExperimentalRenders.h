//
//  ExperimentalRenders.h
//  MaxFoleyRender
//
//  Created by Maxwell Foley on 12/7/20.
//  Copyright © 2020 Maxwell Foley. All rights reserved.
//

#ifndef ExperimentalRenders_h
#define ExperimentalRenders_h

#include <stdio.h>
#include "common.h"
#include "Raycaster.h"
#include <thread>

using namespace MFR;




void GenerateRays(int begin, int end, Ray * rayBuffer, std::shared_ptr<Camera> camera, RaycasterOptions options)
{
    for(int i = begin; i < end; i++)
    {
        int x = i % options.width;
        int y = i / options.width;
        
        //calculate ray direction and origin based on camera position
        const float side = -2.0f * tan(    camera->fov  / 2.0f);
        
        Point p = Point( camera->focalLength * (float(x) / float(options.width) - 0.5f) * side * float(options.width) / float(options.height), camera->focalLength * -(float(y) / float(options.height) - 0.5f) * side, camera->focalLength)  + camera->position;
        
        Vector dir = p-camera->position;
        dir = dir.unit();
        Ray ray = Ray(p,dir);
        //TODO: rotate ray if camera is rotated
        
        rayBuffer[i] = ray;
    }
    
}

void IntersectRays(int begin, int end, Ray * rayBuffer, std::shared_ptr<Surfel> * surfelBuffer, Color * colorBuffer, std::shared_ptr<TriTree> * tt)
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
        
        TriResult result = (*tt)->intersectRay(ray, b, distance);
        
        auto t2 = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
        if(duration > longestDuration) {
            longestDuration = duration;
            logLevel = 2;
            (*tt)->intersectRay(ray, b, distance);
            std::cout << i << std::endl;
            logLevel = 1;
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
        std::cout << (*tt)->mostRecentNodesTouched << std::endl;
        colorBuffer[i] = Color(float((*tt)->mostRecentNodesTouched)/84.0,0,0);
        
        ///
        
    }
    averageDuration /= (end-begin);
    std::cout << "Longest dur" << longestDuration << "\n";
    std::cout << "Average dur" << averageDuration << "\n";
    std::cout << "Average nodes hit" << (*tt)->averageNodesTouched() << "\n";
}



void VisualizeTimeToRender(std::shared_ptr<Scene> scene, Color * colorBuffer, RaycasterOptions options)
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
    std::shared_ptr<TriTree> * tt = new std::shared_ptr<TriTree>(std::make_shared<TriTree>(scene->posedMesh));
    
    std::vector<std::shared_ptr<Light>> lights = scene->lights;
        
    std::cout << "generating rays" << std::endl;
    GenerateRays(0,numPix,rayBuffer,scene->camera,options);
    
    std::cout << "intersecting rays" << std::endl;
    IntersectRays(0,numPix,rayBuffer, surfelBuffer, colorBuffer, tt);
    
    
}

#endif /* ExperimentalRenders_h */
