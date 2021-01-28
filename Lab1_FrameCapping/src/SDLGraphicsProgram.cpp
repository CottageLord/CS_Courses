#include "SDLGraphicsProgram.h"
#include "rectangle.h"

#define FRAME_RATE      60
// I used microseconds instead of miliseconds for better precision
// using miliseconds (larger gap) would sometimes ignore time elapsed if
// the last update/render loop runs too fast
#define MCS_PER_SECOND  1000000
// switch between ideal frame rate and crazy frame rate
#define STABLE_FRAME    true

// Initialization function
// Returns a true or false value based on successful completion of setup.
// Takes in dimensions of window.
SDLGraphicsProgram::SDLGraphicsProgram(int w, int h):screenWidth(w),screenHeight(h){
  // Initialize random number generation.
   srand(time(NULL));
    // Setup blocks
    for(int i=0; i < BLOCKS; ++i){
        r[i].init(screenWidth,screenHeight);
    }


  // Initialization flag
    bool success = true;
  // String to hold any errors that occur.
  std::stringstream errorStream;
  // The window we'll be rendering to
  gWindow = NULL;
  // Render flag

	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO)< 0){
		errorStream << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		success = false;
	}
	else{

		//Create window
		gWindow = SDL_CreateWindow( "Lab",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                screenWidth,
                                screenHeight,
                                SDL_WINDOW_SHOWN );

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

  // If initialization did not work, then print out a list of errors in the constructor.
  if(!success){
    errorStream << "Failed to initialize!\n";
    std::string errors=errorStream.str();
    std::cout << errors << "\n";
  }else{
    std::cout << "No SDL, or OpenGL, errors Detected\n\n";
  }
}




// Proper shutdown and destroy initialized objects
SDLGraphicsProgram::~SDLGraphicsProgram(){
    // Destroy Renderer
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
	// Nothing yet!
    SDL_SetRenderDrawColor(gRenderer, 0x22,0x22,0x22,0xFF);
    SDL_RenderClear(gRenderer);  
    for(int i =0; i < BLOCKS; i++){
        r[i].update(screenWidth,screenHeight);
    }
}


// Render
// The render function gets called once per loop
void SDLGraphicsProgram::render(){

    for(int i =0; i < BLOCKS; i++){
        r[i].render(getSDLRenderer());
    }

    SDL_RenderPresent(gRenderer);
}



//Loops forever!
void SDLGraphicsProgram::loop(){
    // Main loop flag
    // If this is quit = 'true' then the program terminates.
    bool quit = false;
    // define microseconds per update
    double mcs_per_update = MCS_PER_SECOND / FRAME_RATE;

    // elapsed_time - measure how much time spent after last update/render
    // elapsed_time_total - measure total time elapsed
    // lag - accumulate elapsed time for determining when to update to ensure steady frame rate
    double lag, elapsed_time, elapsed_time_total = 0.0;
    // frame_counter - measure the real frame rate
    int    frame_counter     = 0;
    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event e;
    // Enable text input
    SDL_StartTextInput();
    // for measuring times while running
    std::chrono::steady_clock::time_point previous_time, current_time, duration;
    // record the initial time
    previous_time = std::chrono::steady_clock::now();
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
        current_time  = std::chrono::steady_clock::now();
        elapsed_time  = (double)std::chrono::duration_cast<std::chrono::microseconds>
                          (current_time - previous_time).count();
        // renew time record
        previous_time = current_time;
        // record overall time elapsed (in microseconds)
        elapsed_time_total += elapsed_time;

        if(STABLE_FRAME) {
            // record how much time has been spent since last loop (in microseconds)
            lag += elapsed_time;
            //std::cout << lag;
            while(lag >= mcs_per_update) {
                // Update our scene
                update();
                lag -= mcs_per_update;
                frame_counter ++;
            }
        } else {
            update();
            frame_counter ++;
        }
        // for every 1 second, report frame rate and re-initialize counters
        if (elapsed_time_total > MCS_PER_SECOND)
        {
            std::cout << "current frame rate: " << frame_counter << std::endl;
            frame_counter = 0;
            elapsed_time_total = 0.0;
        }
        // Render using OpenGL
        render();
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
