#include "SpriteComponent.hpp"
#include <iostream>

SpriteComponent::SpriteComponent(){
    frame_x = 0;
    frame_y = 0;
}

SpriteComponent::~SpriteComponent(){
// TODO: Make sure spritesheet and m_texture are destroyed
// but is this the right place?
    SDL_FreeSurface(m_spriteSheet);
    m_spriteSheet = nullptr;
    SDL_DestroyTexture(m_texture);
}

// Set the sprite position
void SpriteComponent::SetPosition(int x, int y){
    m_xPos = x;
    m_yPos = y;
}
void SpriteComponent::SetTransform(TransformComponent *newPlayerTrans) {
    playerTrans = newPlayerTrans;
}

void SpriteComponent::Update(){
    // place this x++ either at the beginning or the end of this function
    frame_x++;
    // proceed to next image
    if(frame_x > BMP_COLOMN){
      frame_x = 0;
      frame_y++;
    }
    if (frame_y > BMP_ROW)
    {
      frame_y = 0;
    }
    // if reach the black, restart from beginning
    if (frame_y >= BMP_BLK_Y && frame_x >= BMP_BLK_X)
    {
      frame_x = 0;
      frame_y = 0;
    }
    
    // Here I am selecting which frame I want to draw
    // from our sprite sheet. Think of this as just
    // using a mouse to draw a rectangle around the
    // sprite that we want to draw.
    m_src.x = frame_x * BMP_IMG_W;
    m_src.y = frame_y * BMP_IMG_H;
    m_src.w = BMP_IMG_W;
    m_src.h = BMP_IMG_H;

    // Where we want the rectangle to be rendered at.
    // This is an actual 'quad' that will draw our
    // source image on top of.
    if (playerTrans)
    {
        m_dest.x = playerTrans->GetScreenPosition().first;
        m_dest.y = playerTrans->GetScreenPosition().second;

    } else {
        m_dest.x = BMP_IMG_W;
        m_dest.y = BMP_IMG_H;
    }
    
    m_dest.w = BMP_IMG_W;
    m_dest.h = BMP_IMG_H;
}

void SpriteComponent::Render(SDL_Renderer* ren){
    SDL_RenderCopy(ren, m_texture, &m_src, &m_dest);
}


void SpriteComponent::LoadImage(std::string filePath, SDL_Renderer* ren){
    m_spriteSheet = SDL_LoadBMP(filePath.c_str());
    if(nullptr == m_spriteSheet){
           SDL_Log("Failed to allocate surface");
    }else{
        SDL_Log("Allocated a bunch of memory to create identical game character");
        // Create a texture from our surface
        // Textures run faster and take advantage 
        // of hardware acceleration
        m_texture = SDL_CreateTextureFromSurface(ren, m_spriteSheet);
    }
}
