
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
#include <iostream>

#include "ResourceManager.hpp"

// initialize the singleton pointer field
ResourceManager* ResourceManager::instance = nullptr;

ResourceManager::ResourceManager(){}


ResourceManager::~ResourceManager(){}

void ResourceManager::destroy() {
	std::map<position, render_info>::iterator to_be_rendered_it = to_be_rendered.begin();
	// free all objects
	// Iterate over the map using Iterator till end.
	SDL_FreeSurface(to_be_rendered_it->second.sprite_sheet);
	SDL_DestroyTexture(to_be_rendered_it->second.texture);
	// as we only have 1 sprite_sheet and 1 texture
	// we only free up the pointer variable
	while (to_be_rendered_it != to_be_rendered.end())
    {
    	to_be_rendered_it->second.sprite_sheet = nullptr;
		to_be_rendered_it->second.texture = nullptr;
        to_be_rendered_it++;
    }
    delete &loaded_resources;
    //delete &to_be_rendered;
    //delete instance;
    // free all resources
	// std::map<std::string, render_obj>::iterator loaded_resources_it = loaded_resources.begin();
    
}

ResourceManager *ResourceManager::get_instance() {
	if (!instance) instance = new ResourceManager;
	return instance;
}

void ResourceManager::init() {
	// initialize the maps
	std::map<std::string, render_obj> loaded_resources;
	std::map<position, render_info> to_be_rendered;
}

void ResourceManager::load_resource(const char* resource_path, int x, int y, SDL_Renderer* ren) {
	//std::string resource_path = "./sprite.bmp";
	// if the resource is a new one, load it to a map
	SDL_Surface *sprite_sheet;
	SDL_Texture *texture;
	// if new resource
	if(loaded_resources.find(resource_path) == loaded_resources.end()) {
		sprite_sheet = SDL_LoadBMP(resource_path);
		//SDL_Log("new resource!");
		if(sprite_sheet==NULL){
	    	SDL_Log("Failed to allocate surface");
		}else{
		    SDL_Log("Allocated a bunch of memory to create identical game character");
			// Create a texture from our surface
		    // Textures run faster and take advantage of hardware acceleration
	    	texture = SDL_CreateTextureFromSurface(ren, sprite_sheet);
		}
		loaded_resources[resource_path] = std::make_pair(sprite_sheet, texture);
	}
	// load the info into render obj
	Render_Info new_sprite;
	// record where to draw
	new_sprite.Dest.x = x;
	new_sprite.Dest.y = y;
	new_sprite.sprite_sheet = loaded_resources[resource_path].first;
	new_sprite.texture = loaded_resources[resource_path].second;
	to_be_rendered[std::make_pair(x, y)] = new_sprite;
}


void ResourceManager::update(int frame_x, int frame_y){
	// The part of the image that we want to render

	std::map<position, render_info>::iterator to_be_rendered_it = to_be_rendered.begin();
	// free all objects
	// Iterate over the map using Iterator till end.
	while (to_be_rendered_it != to_be_rendered.end())
    {
    	// Here I am selecting which frame I want to draw
		// from our sprite sheet. Think of this as just
		// using a mouse to draw a rectangle around the
		// sprite that we want to draw.
		to_be_rendered_it->second.Src.x = frame_x * BMP_IMG_W;
		to_be_rendered_it->second.Src.y = frame_y * BMP_IMG_H;
		to_be_rendered_it->second.Src.w = BMP_IMG_W;
		to_be_rendered_it->second.Src.h = BMP_IMG_H;

		// Where we want the rectangle to be rendered at.
		// This is an actual 'quad' that will draw our
		// source image on top of.	
		//to_be_rendered_it->second.Dest.x = 20;
		//to_be_rendered_it->second.Dest.y = 20;
		to_be_rendered_it->second.Dest.w = 128;
		to_be_rendered_it->second.Dest.h = 128;

        to_be_rendered_it++;
    }
}

void ResourceManager::render(SDL_Renderer* ren){
	std::map<position, render_info>::iterator to_be_rendered_it = to_be_rendered.begin();
	// Iterate over the map using Iterator till end.
	while (to_be_rendered_it != to_be_rendered.end())
    {
		SDL_RenderCopy(ren, 
			to_be_rendered_it->second.texture, 
			&(to_be_rendered_it->second.Src), 
			&(to_be_rendered_it->second.Dest));
        to_be_rendered_it++;
    }
    
}
