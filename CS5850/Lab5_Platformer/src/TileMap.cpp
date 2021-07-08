#include <iostream>
#include <iomanip>
#include <fstream>
#include "TileMap.hpp"

// Creates a new tile map.
// rows and cols are how many different tiles there are in the sprite sheet
// 
// _TileWidth and _TileHeight are the size of each individual
// tile in the sprite sheet.
// (Typically this works best if they are square for this implementation.)
//
// _mapX, and _mapY are the size of the tilemap. This is the actual
// number of tiles in the game that the player sees, not how many tiles
// are in the actual sprite sheet file loaded.
TileMap::TileMap(std::string tileSheetFileName, int rows, int cols, int _TileWidth, int _TileHeight, int _mapX, int _mapY, SDL_Renderer* ren){
    if(nullptr==ren){
        SDL_Log("No valid renderer found");
    }

    // Setup variables
    m_Rows = rows;
    m_Cols = cols;
    m_TileWidth = _TileWidth;
    m_TileHeight = _TileHeight;
    m_MapX = _mapX;
    m_MapY = _mapY;
    m_CameraXOffset = 0;
    m_CameraYOffset = 0;
    // Load the TileMap Image
    // This is the image that will get
    // sliced into smaller subsections of individual tiles.
    m_TileSpriteSheet = SDL_LoadBMP(tileSheetFileName.c_str());
	
    if(nullptr == m_TileSpriteSheet){
           SDL_Log("Failed to allocate surface");
    }else{
        // Create a texture from our surface
        // Textures run faster and take advantage of
        //  hardware acceleration
        m_Texture = SDL_CreateTextureFromSurface(ren, m_TileSpriteSheet);
    }

    // Setup the TileMap array
    // This sets each tile to '0'
    m_Tiles = new int[m_MapX*m_MapY];
    for(int i=0; i < m_MapX*m_MapY; i++){
        m_Tiles[i] = -1; // Default value is no tile.
    }
}

// Destructor
TileMap::~TileMap(){
    SDL_DestroyTexture(m_Texture);
    // Remove our TileMap
    delete[] m_Tiles;
}

// Helper function to gegenerate a simlpe map from a file
void TileMap::GenerateSimpleMap(){
    
    // open the file
    std::ifstream level_data(LEVEL_FILE);

    if (!level_data.is_open()) {
        // Print an error and exit
        std::cerr << "Cannot open the level data" << std::endl;
        return;
    }
    int tile_index;
    for(int y= 0; y < m_MapY; y++){
        for(int x= 0; x < m_MapX; x++){
            if (level_data >> tile_index)
            {
                //std::cout << tile_index << std::endl;
                SetTile(x,y,tile_index);
            }
       }
    }
}

// Helper function to print out the tile map
// to the console
void TileMap::PrintMap(){
    for(int y= 0; y < m_MapY; y++){
        for(int x= 0; x < m_MapX; x++){
            std::cout << std::setw(3) << GetTileType(x,y);
       }
         std::cout << "\n";
    }
}

// Sets a tile a certain type
void TileMap::SetTile(int x, int y, int type){
    m_Tiles[y * m_MapX + x] = type;
}


// Returns what the tile is at a specific position.
int TileMap::GetTileType(int x, int y){
    return m_Tiles[y * m_MapX + x];
}

void TileMap::SetCameraOffset(int x, int y) {
    m_CameraXOffset += x;
    m_CameraYOffset += y;
    // prevent out of bound
    if (m_CameraXOffset >= TILEMAP_COL - TILE_ON_WINDOW_W) 
        m_CameraXOffset = TILEMAP_COL - TILE_ON_WINDOW_W;

    if (m_CameraYOffset >= TILEMAP_ROW - TILE_ON_WINDOW_H) 
        m_CameraYOffset = TILEMAP_ROW - TILE_ON_WINDOW_H;

    if (m_CameraXOffset <= 0) m_CameraXOffset = 0;
    if (m_CameraYOffset <= 0) m_CameraYOffset = 0;
}
// render TileMap
void TileMap::Render(SDL_Renderer* ren){
    if(nullptr==ren){
        SDL_Log("No valid renderer found");
    }
    
    SDL_Rect Src, Dest;
    for(int y = m_CameraYOffset; y < TILEMAP_ROW; y++){
        for(int x = m_CameraXOffset; x < TILEMAP_COL; x++){
            // Select our Tile
            int currentTile = GetTileType(x,y);
            if(currentTile > -1 ){
                // Reverse lookup, given the tile type
                // and then figuring out how to select it
                // from the texture atlas.
                Src.x = (currentTile % m_Cols) * m_TileWidth;
                Src.y = (currentTile / m_Rows) * m_TileHeight;
                Src.w = m_TileWidth; 
                Src.h = m_TileHeight; 
                // Render our Tile at this location
                Dest.x = (x - m_CameraXOffset) * m_TileWidth;
                Dest.y = (y - m_CameraYOffset)* m_TileHeight;
                Dest.w = m_TileWidth;
                Dest.h = m_TileHeight;
                SDL_RenderCopy(ren, m_Texture, &Src, &Dest);
            }
        }
    }
}
