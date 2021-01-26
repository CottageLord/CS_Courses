#include "SDLGraphicsProgram.h"


// Try toggling this number!
#define BLOCKS 500

// Just a cheap little class to give a visual effect that stresses the system.
// This should also be useful for helping you get started with your breakout code!
// Move this to its own .h and .cpp files for best practice!
class Rectangle{
public:
    Rectangle(){
        // Empty constructor! We haev complete control!
    }

    // Okay, but do not forget to call this!
    void init(int screenWidth, int screenHeight){
        x = rand()%screenWidth;
        y = rand()%screenHeight;
        w = rand()%100;
        h = rand()%100;
        speed = rand()%2+1; 
    }

    // Arguments here are a little redundant--do we need them?
    // (Perhaps if the screen resizes? Hmm!)
    void update(int screenWidth, int screenHeight){
        if(up){
            y+=speed;
            if(y>screenHeight){
                up = !up;
            }
        }

        if(!up){
            y-=speed;
            if(y<0){
                up = !up;
            }
        }
        if(left){
            x+=speed;
            if(x>screenWidth){
                left = !left;
            }
        }

        if(!left){
            x-=speed;
            if(x<0){
                left = !left;
            }
        }
    }

    // Okay, render our rectangles!
    void render(SDL_Renderer* gRenderer){
        SDL_Rect fillRect = {x,y,w,h};
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderDrawRect(gRenderer, &fillRect); 
    }

private:
     
    int x{100};
    int y{100};
    int w{100};
    int h{100};
    int speed{1};
    bool up{true};
    bool left{true};

};


// Create our list of rectangles!
Rectangle r[BLOCKS];


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
    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event e;
    // Enable text input
    SDL_StartTextInput();
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
	  // Update our scene
	  update();
      // Render using OpenGL
      render();
      //Update screen of our specified window
      // SDL_GL_SwapWindow(getSDLWindow());
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
