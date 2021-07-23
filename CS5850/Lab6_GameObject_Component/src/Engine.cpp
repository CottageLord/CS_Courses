#include "Engine.hpp"
// Global array to create our characters
//Sprite characters;

// Create a TileMap
//TileMap* myTileMap;

// Initialization function
// Returns a true or false value based on successful completion of setup.
// Takes in dimensions of window.
Engine::Engine(){
}


// Proper shutdown and destroy initialized objects
Engine::~Engine(){
}

void Engine::Start(){
    // Report which subsystems are being initialized
    if(m_renderer!=nullptr){
        std::cout << "Initializing Graphics Subsystem\n";
    }else{
        std::cout << "No Graphics Subsystem initialized\n";
    }
    SDL_StartTextInput();
    bool quitApp = false;

    // =================== Generate a player ================= //
    GameObject *player = new GameObject();

    TransformComponent *playerTransform = new TransformComponent();
    ControllerComponent *playerControl = new ControllerComponent();

    playerTransform->SetController(playerControl);
    playerControl->AddEventListener(&SDLEvent);
    // initializa sprite and make the sprite stick to player pos
    SpriteComponent *playerSprite = new SpriteComponent();
    playerSprite->LoadImage(SPRITE_FILE, m_renderer->GetRenderer());
    playerSprite->SetTransform(playerTransform);
    // register all components
    player->AddComponent(TRANSFORM_ID, playerTransform);
    player->AddComponent(CONTROLLER_ID, playerControl);
    player->AddComponent(SPRITE_ID, playerSprite);

    // =================== Generate a TileMap ================= //
    TileMap *myTileMap = new TileMap(TILE_FILE, TILE_FILE_ROW, TILE_FILE_COL, 
      TILE_WIDTH, TILE_HEIGHT, TILEMAP_COL, TILEMAP_ROW, m_renderer->GetRenderer());
    // add player as the tile map reference for moving along with player
    myTileMap->AddPlayer(player);
    // Generate a a simple tilemap
    myTileMap->GenerateSimpleMap();

    m_objects.push_back(player);
    m_objects.push_back(myTileMap);

}
// Return Input
void Engine::Input(){
    // Event handler that handles various events in SDL
    // that are related to input and output
    
    //Handle events on queue
    while(SDL_PollEvent( &SDLEvent ) != 0){
      // User posts an event to quitApp
      // An example is hitting the "x" in the corner of the window.
      if(SDLEvent.type == SDL_QUIT){
        quitApp = true;
      } else if (SDLEvent.type == SDL_KEYDOWN) { // respond to various keyboard inputs
        if (SDLEvent.key.keysym.sym == SDLK_q) quitApp = true;
      }
    }
}

// Update SDL
void Engine::Update()
{
  for (GameObject* obj : m_objects)
  {
    obj->Update();
  }

}


// Render
// The render function gets called once per loop
void Engine::Render(){
    // Set the color of the empty framebuffer
    m_renderer->SetRenderDrawColor(110, 130,170,0xFF);
    // Clear the screen to the color of the empty framebuffer
    m_renderer->RenderClear();
    for (GameObject* obj : m_objects)
    {
      obj->Render(m_renderer->GetRenderer());
    }

    // Render the tile map
    //myTileMap->Render(m_renderer->GetRenderer());

    // Render each of the character(s)
    //characters.Render(m_renderer->GetRenderer());

    // Flip the buffer to render
    m_renderer->RenderPresent();
}

void Engine::update_with_timer(std::chrono::steady_clock::time_point &previous_time, 
  double &elapsed_time_total, int &frame_counter, double &lag, double mcs_per_update) {
  // time recorders
    //std::cout << "prev time: " << std::chrono::duration_cast<std::chrono::minutes>(previous_time).count();
    std::chrono::steady_clock::time_point current_time;
    // calculate how much time has elapsed since last record (usually 1 render loop earlier)
    current_time  = std::chrono::steady_clock::now();

    double elapsed_time;
    elapsed_time  = (double)std::chrono::duration_cast<std::chrono::microseconds>
                      (current_time - previous_time).count();
    // renew time record
    previous_time = current_time;
    // record overall time spend (in microseconds)
    elapsed_time_total += elapsed_time;
    // stablizer switch
    if(STABLE_FRAME) {
        // if the last update/render loop spent more than fps limit (16.67ms for 60 fps)
        // we update untill the game progress catches up
        lag += elapsed_time;
        while(lag >= mcs_per_update) {
            // Update our scene
            Update();
            lag -= mcs_per_update;
            frame_counter++;
        }
    } else {
        Update();
        frame_counter++;
    }
    // for every 1 second, report frame rate and re-initialize counters
    if (elapsed_time_total > MCS_PER_SECOND)
    {
        std::cout << "current frame rate: " << frame_counter << std::endl;
        frame_counter = 0;
        elapsed_time_total = 0.0;
    }
}

//Loops forever!
void Engine::MainGameLoop(){
    // Main loop flag
    // If this is quitApp = 'true' then the program terminates.

    // define microseconds per update
    // ===============================================================
    // use MAGIC NUMBER to make sure sprite with less grids run slower
    // ===============================================================
    int frame_rate = BMP_ROW * BMP_COLOMN * SPRITE_STABLIZER;

    double mcs_per_update = MCS_PER_SECOND / frame_rate;
    // elapsed_time_total - measure total time elapsed
    // lag - accumulate elapsed time for determining when to update to ensure steady frame rate
    double lag = 0, elapsed_time_total = 0.0;
    // frame_counter - measure the real frame rate
    int frame_counter = 0;
    // chrono::steadyclock for measuring times accurately while rendering
    std::chrono::steady_clock::time_point previous_time;
    // record the initial time
    previous_time = std::chrono::steady_clock::now();
    // While application is running

    // While application is running
    while(!quitApp){
      // Get user input
      Input();
      // If you have time, implement your frame capping code here
      // Otherwise, this is a cheap hack for this lab.
      SDL_Delay(100);
      // Update our scene
      // update with a frame stablizer
      update_with_timer(previous_time, elapsed_time_total, frame_counter, lag, mcs_per_update);

      // Render using OpenGL
      Render();
      //Update screen of our specified window
    }
    //Disable text input
    SDL_StopTextInput();
}



void Engine::Shutdown(){
    // Shut down our Tile Systems
    if(nullptr!=m_renderer){
        delete m_renderer;
    } 
    for (GameObject* obj : m_objects)
    {
      delete obj;
    }
    /*
    // Destroy our tilemap
    if(nullptr!=myTileMap){
        delete myTileMap;
    }*/
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
