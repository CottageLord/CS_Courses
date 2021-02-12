#include "SDLGraphicsProgram.hpp"
#include "ResourceManager.hpp"

// Try toggling this number!
#define CHARACTERS 50


// Global variable to store character render info
ResourceManager *characters;

// Initialization function
// Returns a true or false value based on successful completion of setup.
// Takes in dimensions of window.
SDLGraphicsProgram::SDLGraphicsProgram(int w, int h):screenWidth(w),screenHeight(h){
  	// Initialize random number generation.
   	srand(time(NULL));

	 // Initialization flag
	 bool success = true;
	 // String to hold any errors that occur.
	 std::stringstream errorStream;
	 // The window we'll be rendering to
	 gWindow = NULL;
	 // Render flag

	// Initialize SDL
	if(SDL_Init(SDL_INIT_EVERYTHING)< 0){
		errorStream << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		success = false;
	}
	else{
		//Create window
		gWindow = SDL_CreateWindow( "Lab", 100, 100, screenWidth, screenHeight, SDL_WINDOW_SHOWN );

		// Check if Window did not create.
		if( gWindow == NULL ){
			errorStream << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		//Create a Renderer to draw on
		gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
		// Check if Renderer did not create.
		if( gRenderer == NULL ){
			errorStream << "Renderer could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}
	}
    // Setup our characters
    // Remember, this can only be done after SDL has been
    // successfully initialized!
    // Here I am just building a little grid of characters

    characters =  characters->get_instance();
    // initialize with the empty maps
    characters->init();

    unsigned int xRow = 0;
    unsigned int yColumn = 0;
    constexpr int offset = 128;

    for(int i=0; i < CHARACTERS; ++i){
        characters->load_resource(FILE_NAME, xRow, yColumn, getSDLRenderer());
        xRow += offset;
        if (xRow > screenWidth){
            yColumn+=offset;
            xRow = 0;
        }
    }

  // If initialization did not work, then print out a list of errors in the constructor.
  if(!success){
    	errorStream << "Failed to initialize!\n";
    	std::string errors=errorStream.str();
    	std::cout << errors << "\n";
  }else{
    	std::cout << "No SDL errors Detected in during init\n\n";
  }
}


SDLGraphicsProgram::~SDLGraphicsProgram(){}

// Proper shutdown and destroy initialized objects
void SDLGraphicsProgram::destroy(){
    // Destroy Renderer
    characters->destroy();

    SDL_DestroyRenderer(gRenderer);
    //Destroy window
    SDL_DestroyWindow( gWindow );
    // Point gWindow to NULL to ensure it points to nothing.
    gRenderer = NULL;
    gWindow = NULL;
    //Quit SDL subsystems
    SDL_Quit();
}



// Update OpenGL
void SDLGraphicsProgram::update()
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
    characters->update(frame_x,frame_y);
    frame_x++;
}

// the Update() helper function that provide a frame stablizer
// adapted from my lab 1
void SDLGraphicsProgram::update_with_timer(std::chrono::steady_clock::time_point &previous_time, double &elapsed_time_total, int &frame_counter, double &lag, double mcs_per_update) {
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
    if(stable_frame) {
        // if the last update/render loop spent more than fps limit (16.67ms for 60 fps)
        // we update untill the game progress catches up
        lag += elapsed_time;
        while(lag >= mcs_per_update) {
            // Update our scene
            update();
            lag -= mcs_per_update;
            frame_counter++;
        }
    } else {
        update();
        frame_counter++;
    }
    // for every 1 second, report frame rate and re-initialize counters
    if (elapsed_time_total > mcs_per_second)
    {
        std::cout << "current frame rate: " << frame_counter << std::endl;
        frame_counter = 0;
        elapsed_time_total = 0.0;
    }
}

// Render
// The render function gets called once per loop
void SDLGraphicsProgram::render(){

    SDL_SetRenderDrawColor(gRenderer, 0x22,0x22,0x22,0xFF);
    SDL_RenderClear(gRenderer);  

    characters->render(getSDLRenderer());

    SDL_RenderPresent(gRenderer);
}



//Loops forever!
void SDLGraphicsProgram::loop(){
    // Main loop flag
    // If this is quit = 'true' then the program terminates.
    bool quit = false;
    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event e;
    // Enable text input
    SDL_StartTextInput();

    // define microseconds per update
    // ===============================================================
    // use MAGIC NUMBER to make sure sprite with less grids run slower
    // ===============================================================
    int frame_rate = BMP_ROW * BMP_COLOMN * sprite_stablizer;

    double mcs_per_update = mcs_per_second / frame_rate;
    double ms_per_update = mcs_per_update / 1000;
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
    while(!quit){
      //Handle events on queue
      while(SDL_PollEvent( &e ) != 0){
        // User posts an event to quit
        // An example is hitting the "x" in the corner of the window.
        if(e.type == SDL_QUIT){
          quit = true;
        }		
      }

      // If you have time, implement your frame capping code here
      // Otherwise, this is a cheap hack for this lab.
      SDL_Delay(100);

      // Update our scene
      // update with a frame stablizer
      update_with_timer(previous_time, elapsed_time_total, frame_counter, lag, mcs_per_update);
      // Render using OpenGL
      render();
      //Update screen of our specified window
    }

    //Disable text input
    SDL_StopTextInput();
}

// Get Pointer to Window
SDL_Window* SDLGraphicsProgram::getSDLWindow(){
  return gWindow;
}

// Get Pointer to Renderer
SDL_Renderer* SDLGraphicsProgram::getSDLRenderer(){
  return gRenderer;
}
