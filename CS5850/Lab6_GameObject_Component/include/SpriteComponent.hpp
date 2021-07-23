#ifndef SPRITE_H
#define SPRITE_H

#include <string>

#include "GraphicsEngineRenderer.hpp"

/**
 * A small class to demonstrate loading sprites.
 * Sprite sheets are often used for loading characters,
 * environments, icons, or other images in a game.
 */
#include "Component.hpp"
#include "TransformComponent.hpp"

class SpriteComponent : public Component{
public:
    /**
     * Constructor
     */
    SpriteComponent();
    /**
     * Constructor
     */
    ~SpriteComponent();
    /**
     * Initialize the sprite
     */
    void SetPosition(int x, int y);
    /**
     * Update the sprites position and frame
     */
    void Update();
    /**
     * Render the sprite 
     */
    void Render(SDL_Renderer* ren);
    /**
     * Load a sprite
     */
    void LoadImage(std::string filePath,SDL_Renderer* ren);
    /**
     * Add a transform to follow
     */
    void SetTransform(TransformComponent *newPlayerTrans);

private:
	int m_xPos;
    int m_yPos;
    int frame_x;
    int frame_y;

    TransformComponent *playerTrans;
    // An SDL Surface contains pixel data to draw an image
	SDL_Surface* m_spriteSheet;
	SDL_Texture* m_texture;

	SDL_Rect m_src;
	SDL_Rect m_dest;
};

#endif
