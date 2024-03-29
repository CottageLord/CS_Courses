#include "Sprite.hpp"

Sprite::Sprite(){
}

Sprite::~Sprite(){
// TODO: Make sure spritesheet and m_texture are destroyed
// but is this the right place?
    SDL_FreeSurface(m_spriteSheet);
    m_spriteSheet = nullptr;
    SDL_DestroyTexture(m_texture);
}

// Set the sprite position
void Sprite::SetPosition(int x, int y){
    m_xPos = x;
    m_yPos = y;
}

void Sprite::Update(int frame_x, int frame_y){
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
    m_dest.x = 128;
    m_dest.y = 508;
    m_dest.w = 128;
    m_dest.h = 128;
}

void Sprite::Render(SDL_Renderer* ren){
    SDL_RenderCopy(ren, m_texture, &m_src, &m_dest);
}


void Sprite::LoadImage(std::string filePath, SDL_Renderer* ren){
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
