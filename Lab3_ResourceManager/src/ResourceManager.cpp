
#if defined(LINUX) || defined(MINGW)
    #include <SDL2/SDL.h>
#else // This works for Mac
    #include <SDL.h>
#endif

// I recommend a map for filling in the resource manager
#include <map>
#include <string>
#include <memory>
#include <iterator>

#include "ResourceManager.hpp"

ResourceManager::ResourceManager(){
}


ResourceManager::~ResourceManager(){
// TODO:    Make sure spritesheet and texture are destroyed
//          but is this the right place?
//          Consider adding a 'destory' member function instead.
	SDL_FreeSurface(spriteSheet);
	spriteSheet=NULL;
	SDL_DestroyTexture(texture);
}

// TODO: YUCK! Fix this common resource consumption error.
void ResourceManager::init(int x, int y, SDL_Renderer* ren){
	xPos = x;
	yPos = y;

	spriteSheet = SDL_LoadBMP("./sprite.bmp");
	if(spriteSheet==NULL){
    	SDL_Log("Failed to allocate surface");
	}else{
	    SDL_Log("Allocated a bunch of memory to create identical game character");
		// Create a texture from our surface
	    // Textures run faster and take advantage of hardware acceleration
    	texture = SDL_CreateTextureFromSurface(ren, spriteSheet);
	}
}


void ResourceManager::update(int x, int y, int frame){
	// The part of the image that we want to render
	currentFrame = frame;
	if(currentFrame>6){
		currentFrame=0;
	}        

	// Here I am selecting which frame I want to draw
	// from our sprite sheet. Think of this as just
	// using a mouse to draw a rectangle around the
	// sprite that we want to draw.
	Src.x = currentFrame*75;
	Src.y = 0;
	Src.w = 75;
	Src.h = 87;

	// Where we want the rectangle to be rendered at.
	// This is an actual 'quad' that will draw our
	// source image on top of.	
	Dest.x = xPos;
	Dest.y = yPos;
	Dest.w = 128;
	Dest.h = 128;
}

void ResourceManager::render(SDL_Renderer* ren){
    SDL_RenderCopy(ren, texture, &Src, &Dest);
}
