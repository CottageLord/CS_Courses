#ifndef SPRITE_H
#define SPRITE_H

#include <string>

#include "GraphicsEngineRenderer.hpp"

/**
 * A small class to demonstrate loading sprites.
 * Sprite sheets are often used for loading characters,
 * environments, icons, or other images in a game.
 */
class Sprite{
public:

    /**
     * Constructor
     */
    Sprite();
    /**
     * Constructor
     */
    ~Sprite();
    /**
     * Initialize the sprite
     */
    void SetPosition(int x, int y);
    /**
     * Update the sprites position and frame
     */
    void Update(int frame_x, int frame_y);
    /**
     * Render the sprite 
     */
    void Render(SDL_Renderer* ren);
    /**
     * Load a sprite
     */
    void LoadImage(std::string filePath,SDL_Renderer* ren);
    

private:
	int m_xPos;
    int m_yPos;
	unsigned int m_currentFrame{0};
	unsigned int m_lastFrame{7};
    // An SDL Surface contains pixel data to draw an image
	SDL_Surface* m_spriteSheet;
	SDL_Texture* m_texture;

	SDL_Rect m_src;
	SDL_Rect m_dest;
};

#endif
