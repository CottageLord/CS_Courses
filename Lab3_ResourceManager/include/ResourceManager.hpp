#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

// I recommend a map for filling in the resource manager
#include <map>
#include <string>
#include <memory>
#include <iterator>

// Try toggling this number!
#define CHARACTERS 50


// Just a cheap little class to demonstrate loading characters.
class ResourceManager{
public:

	ResourceManager();

	~ResourceManager();

	void init(int x, int y, SDL_Renderer* ren);

	void update(int x, int y, int frame);

	void render(SDL_Renderer* ren);

private:
	int xPos, yPos;
	unsigned int currentFrame{0};
	unsigned int LastFrame{7};
    	// An SDL Surface contains pixel data to draw an image
	SDL_Surface *spriteSheet;
	SDL_Texture *texture;

	SDL_Rect Src;
	SDL_Rect Dest;
};

#endif
