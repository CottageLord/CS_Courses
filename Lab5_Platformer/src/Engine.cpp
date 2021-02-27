#include "Engine.hpp"
#include "Sprite.hpp"
#include "TileMap.hpp"
// I recommend a map for filling in the resource manager
#include <map>
#include <string>
#include <memory>
#include <iterator>

// Try toggling this number!
#define CHARACTERS 1

// Global array to create our characters
Sprite characters[CHARACTERS];

// Create a TileMap
TileMap* myTileMap;

// Initialization function
// Returns a true or false value based on successful completion of setup.
// Takes in dimensions of window.
Engine::Engine(){
}


// Proper shutdown and destroy initialized objects
Engine::~Engine(){
}

// Return Input
void Engine::Input(bool *quit){
    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event event;
    // Enable text input
    SDL_StartTextInput();
    //Handle events on queue
    while(SDL_PollEvent( &event ) != 0){
      // User posts an event to quit
      // An example is hitting the "x" in the corner of the window.
      if(event.type == SDL_QUIT){
        *quit = true;
      } else if (event.type == SDL_KEYDOWN) { // respond to various keyboard inputs
        if (event.key.keysym.sym == SDLK_LEFT)      myTileMap->SetCameraOffset(-1, 0);
        else if (event.key.keysym.sym == SDLK_RIGHT)myTileMap->SetCameraOffset(1, 0);
        else if (event.key.keysym.sym == SDLK_UP)   myTileMap->SetCameraOffset(0, -1);
        else if (event.key.keysym.sym == SDLK_DOWN) myTileMap->SetCameraOffset(0, 1);
      }
       
    }
}

// Update SDL
void Engine::Update()
{
    static int frame_x = 0;
    static int frame_y = 0;
    // proceed to next image
    if(frame_x > BMP_ROW){
      frame_x = 0;
      frame_y++;
    }
    if (frame_y > BMP_COLOMN)
    {
      frame_y = 0;
    }
    // if reach the black, restart from beginning
    if (frame_y == BMP_BLK_Y && frame_x == BMP_BLK_X)
    {
      frame_x = 0;
      frame_y = 0;
    }
    // Nothing yet!
    // iterate through each of our characters
    for(int i =0; i < CHARACTERS; i++){
        characters[i].Update(frame_x,frame_y);
    }
    frame_x++;
}


// Render
// The render function gets called once per loop
void Engine::Render(){
    // Set the color of the empty framebuffer
    m_renderer->SetRenderDrawColor(110, 130,170,0xFF);
    // Clear the screen to the color of the empty framebuffer
    m_renderer->RenderClear();

    // Render each of the character(s)
    for(int i =0; i < CHARACTERS; i++){
        characters[i].Render(m_renderer->GetRenderer());
    }

    // Render the tile map
    myTileMap->Render(m_renderer->GetRenderer());

    // Flip the buffer to render
    m_renderer->RenderPresent();
}



//Loops forever!
void Engine::MainGameLoop(){
    // Main loop flag
    // If this is quit = 'true' then the program terminates.
    bool quit = false;

    // While application is running
    while(!quit){
      // Get user input
      Input(&quit);
      // If you have time, implement your frame capping code here
      // Otherwise, this is a cheap hack for this lab.
      SDL_Delay(100);
      // Update our scene
      Update();
      // Render using OpenGL
      Render();
      //Update screen of our specified window
    }
    //Disable text input
    SDL_StopTextInput();
}

void Engine::Start(){
    // Report which subsystems are being initialized
    if(m_renderer!=nullptr){
        std::cout << "Initializing Graphics Subsystem\n";
    }else{
        std::cout << "No Graphics Subsystem initialized\n";
    }

    // Move Sprite to initial position
    characters[0].SetPosition(128,508);
    // Load an image for our character
    characters[0].LoadImage(SPRITE_FILE, m_renderer->GetRenderer());

    // Setup our TileMap
    // This tile map is 20x11 in our game
    // It is using a 'reference' tilemap with 8x8 tiles
    // that are each 64x64 pixels.
    //int tile_width = WINDOW_WIDTH / TILEMAP_WIDTH;
    //int tile_height = WINDOW_HEIGHT / TILEMAP_HEIGHT;

    myTileMap = new TileMap(TILE_FILE, TILE_FILE_ROW, TILE_FILE_COL, 
      TILE_WIDTH, TILE_HEIGHT, TILEMAP_COL, TILEMAP_ROW, m_renderer->GetRenderer());
    // Generate a a simple tilemap
    myTileMap->GenerateSimpleMap();
    // Print out the map to the console
    // so we can see what was created.
    myTileMap->PrintMap();

}

void Engine::Shutdown(){
    // Shut down our Tile Systems
    if(nullptr!=m_renderer){
        delete m_renderer;
    } 

    // Destroy our tilemap
    if(nullptr!=myTileMap){
        delete myTileMap;
    }
}

void Engine::InitializeGraphicsSubSystem(){
    // Setup our Renderer
    m_renderer = new GraphicsEngineRenderer(WINDOW_WIDTH,WINDOW_HEIGHT);
    if(nullptr == m_renderer){
        exit(1); // Terminate program if renderer 
                 // cannot be created.
                 // (Optional) TODO:   Could put an error 
                 //                    messeage, or try to 
                 //                    reinitialize the engine 
                 //                    with a different render
    }
}
