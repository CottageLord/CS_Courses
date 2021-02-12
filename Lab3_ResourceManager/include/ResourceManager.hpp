#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

// I recommend a map for filling in the resource manager
#include <map>
#include <string>
#include <memory>
#include <iterator>


#define FILE_NAME "./sprite.bmp"
// data for sprite.bmp
#define BMP_ROW    6
#define BMP_COLOMN 3
#define BMP_IMG_W  75
#define BMP_IMG_H  87
#define BMP_BLK_X  6
#define BMP_BLK_Y  3



/*

#define FILE_NAME "./Sprite-Sheet.bmp"
// data for Sprite-Sheet.bmp
#define BMP_ROW    4
#define BMP_COLOMN 3
#define BMP_IMG_W  132
#define BMP_IMG_H  140
#define BMP_BLK_X  6
#define BMP_BLK_Y  6

*/

/*
#define FILE_NAME "./spritesheet-caveman.bmp"
// data for spritesheet-caveman.bmp

#define BMP_ROW    3
#define BMP_COLOMN 3
#define BMP_IMG_W  32
#define BMP_IMG_H  32
#define BMP_BLK_X  6
#define BMP_BLK_Y  6

*/

typedef std::pair<SDL_Surface*,SDL_Texture*> render_obj;
typedef std::pair<int, int> position;

typedef struct Render_Info {
	int x;
	int y;
	SDL_Rect Src;
	SDL_Rect Dest;
	SDL_Surface *sprite_sheet;
	SDL_Texture *texture;
} render_info;

// Just a cheap little class to demonstrate loading characters.
class ResourceManager{

	ResourceManager();
public:
	// locate the loaded resources by file name
	// could use better key like a resource id
	// use static to avoid re-loaded
	std::map<std::string, render_obj> loaded_resources;
	// For this proj, I use (x, y) as the key for each object
	// this is not a good idea beyond this assignment
	// The real engine should use an id-ish key
	std::map<position, render_info> to_be_rendered;


	~ResourceManager();

	static ResourceManager *get_instance();

	void init();

	void load_resource(const char* resource_path, int x, int y, SDL_Renderer* ren);

	void destroy();

	void update(int frame_x, int frame_y);

	void render(SDL_Renderer* ren);


private:

	static ResourceManager *instance;
};

#endif
