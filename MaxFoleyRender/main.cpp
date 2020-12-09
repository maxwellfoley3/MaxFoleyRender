//
//  main.c
//  MaxFoleyRender
//
//  Created by Maxwell Foley on 11/26/17.
//  Copyright © 2017 Maxwell Foley. All rights reserved.
//

#include <stdio.h>
#include <algorithm>

#include "ExperimentalRenders.h"
#include "Vector.h"
#include "Color.h"
#include "Raycaster.h"
#include "Scene.h"
#include "SceneObject.h"
#include "MakeDefaultScenes.h"
#include "Test.h"
#include "SDL2/SDL.h"
#include <pngwriter.h>

void calculateImage(MFR::Color * buf, int width, int height)
{
	for(int i = 0; i < width*height; i++)
	{
		buf[i] = MFR::Color(1.0,0.0,0.0);
	}
}

void writeImageToFile(MFR::Color * buf, char * filename, int width, int height)
{
   pngwriter png(width,height,0,filename);
   for(int i = 0; i < width*height; i++)
	{
     	png.plot(i%width,height-i/width, buf[i].r, buf[i].g, buf[i].b);
	}
   png.close();
}

SDL_Texture * getImageTexture(char * filename, SDL_Window * win, SDL_Renderer * ren)
{
	SDL_Surface *bmp = SDL_LoadBMP(filename);
	if (bmp == nullptr){
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return nullptr;
	}
	
	SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, bmp);
	SDL_FreeSurface(bmp);
	if (tex == nullptr){
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return nullptr;
	}
	return tex;
}

SDL_Texture * getBlankTexture(SDL_Window * win, SDL_Renderer * ren, int width, int height)
{

 	//The final texture
	SDL_Texture* tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);

	if (tex == nullptr){
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return nullptr;
	}
	return tex;
}

int clamp(int v, int lo, int hi)
{
	if(v<lo) return lo;
	if(v>hi) return hi;
	return v;
}
void writeColorBufferToTexture(SDL_Texture * tex, MFR::Color * buf)
{

	Uint32* pixels = nullptr;
	int pitch = 0;
	Uint32 format;
	
	// Get the size of the texture.
	int w, h;
	SDL_QueryTexture(tex, &format, nullptr, &w, &h);
	
	// Now let's make our "pixels" pointer point to the texture data.
	//will return zero if success, another number on failure
	if (SDL_LockTexture(tex, nullptr, (void**)&pixels, &pitch))
	{
		std::cout << "Locking texture failed: " << SDL_GetError() << std::endl;
	}
	
	MFR::Color * ptr = buf;
	//copy over the pixel buffer
	for(int i = 0; i < w*h; i++)
	{
		//std::cout << "buf i " << *ptr << std::endl;
		Uint32 color = 0;
		/*
		Uint32 r = clamp(buf[i].r,0,1)*255;
		Uint32 g = clamp(buf[i].g,0,1)*255;
		Uint32 b = clamp(buf[i].b,0,1)*255;*/
		Uint32 r = ptr->r*255;
		Uint32 g = ptr->g*255;
		Uint32 b = ptr->b*255;
		
		color = (r << 24) + (g << 16) + (b << 8) + 255;
		//color = (255 << 24) + (0 << 16) + (255 << 8) + 255;

		pixels[i] =color;
		ptr++;
	}
	
	SDL_UnlockTexture(tex);

}


int main(int argc, const char * argv[]) {
	std::cout << "threads: " << std::thread::hardware_concurrency() << std::endl;
	
	//ok time to do some unit tests of my ray triangle intersection rn
    rayTriangleIntersectionUnitTests();
	rayBoxIntersectionUnitTests();
    triTreeIntersectionUnitTests();
	
	int width = 500;
	int height = 500;

	/* RENDER IMAGE TO SCREEN */
	
	//Start SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}
	
	//Set up screen
	SDL_Window *win = SDL_CreateWindow("Max Foley Render!", 100, 100, width, height, SDL_WINDOW_SHOWN);
	if (win == nullptr){
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
	
	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr){
		SDL_DestroyWindow(win);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}
	
	//initialize scene
	std::shared_ptr<MFR::Scene> cornellBox = makeSingleCubeScene();
	cornellBox->poseMesh();	

	//initialize pixel buffer
	MFR::Color * pixels = new MFR::Color[width*height];
	MFR::RaycasterOptions options;
	options.width = width;
	options.height = height;
	options.numRays = 1;
	options.numScatteringEvents = 0;
    auto t1 = std::chrono::high_resolution_clock::now();
    MFR::Raycaster::RenderImage(cornellBox, pixels, options);
    auto t2 = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::seconds>( t2 - t1 ).count();
    std::cout << "Render time " << duration << std::endl;
    //VisualizeTimeToRender(cornellBox, pixels, options);
    
	SDL_Texture * tex = getBlankTexture(win, ren, width, height);
	writeColorBufferToTexture(tex,pixels);

	writeImageToFile(pixels, (char *)"test.png", width, height);
	
	
	
	//SDL_Texture * unusedTex = getImageTexture("hello_world.bmp", win, ren);

	
	//Main loop flag
	bool quit = false;
	
	//Event handler
	SDL_Event e;
	
	while(!quit)
	{
		//Handle events on queue
		while( SDL_PollEvent( &e ) != 0 )
		{
			//User requests quit
			if( e.type == SDL_QUIT )
			{
				quit = true;
			}
		}
			
		//First clear the renderer
		SDL_RenderClear(ren);
		//Draw the texture
		SDL_RenderCopy(ren, tex, NULL, NULL);
		//Update the screen
		SDL_RenderPresent(ren);
		//Take a quick break after all that hard work
		SDL_Delay(1000);
		
	}
	
	delete[] pixels;
	
	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	
	//http://www.willusher.io/sdl2%20tutorials/2013/08/17/lesson-1-hello-world
	//Quit SDL
	SDL_Quit();

    return 0;
}

/*TODO:

figure out why it isnt faster
 intersecting rays phase
    Problem is probably use of shared_ptr for Tris
 shadow tests phase
figure out why cube2.obj doesnt work
figure out why lighting isnt working
    - having 2 lights totally fucks it up
*/
