// Support Code written by Michael D. Shah
// This is modified from a tutorial from: http://lazyfoo.net/tutorials/SDL/51_SDL_and_modern_opengl/index.php
//
// You can modify this could however you like in order to implement your 'pong'
// By default this code just outputs a simple 'red' screen.
//
// If this code does NOT work, try the source from this online example: http://lazyfoo.net/tutorials/SDL/50_SDL_and_opengl_2/index.php
// (Please alert the instructor if this is the case)
//
//
// Last Updated: 2/1/21
// Please do not redistribute without asking permission.

#include "lab.hpp"

// Initialization function
bool init_window() {
	// Initialization flag
    bool success = true;
    // String to hold any errors that occur.
    std::stringstream errorStream;
    // The window we'll be rendering to
    g_window = NULL;
    // Render flag

	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO)< 0){
		errorStream << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		success = false;
	}
	else{
		//Create window
		g_window = SDL_CreateWindow( "Lab",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                SCREEN_WIDTH,
                                SCREEN_HEIGHT,
                                SDL_WINDOW_SHOWN );
        // Check if Window did not create.
        if( g_window == NULL ){
			errorStream << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}
		//Create a Renderer to draw on
        g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
        // Check if Renderer did not create.
        if( g_renderer == NULL ){
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
    return success;
}

bool init_components() {
	TTF_Init();
	// Initialize the font
	scoreFont = TTF_OpenFont("DejaVuSansMono.ttf", 40);
	
}
// This is where we do work in our graphics applications
// that is constantly refreshed.
void update() {
	// set the window to black

}

// This function draws images.
void render() {
	//============= Clear the window to black ==============//

	SDL_SetRenderDrawColor(g_renderer, 0x0, 0x0, 0x0, 0xFF);
	SDL_RenderClear(g_renderer);

	//==================== Draw the net ====================//

	// Set the draw color to be white
	SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	// Draw the net
	for (int y = 0; y < SCREEN_HEIGHT; ++y)
	{
		if (y % 5)
		{
			SDL_RenderDrawPoint(g_renderer, SCREEN_WIDTH / 2, y);
		}
	}

	//==================== Draw the ball ==================//
	//ball.x = ;
	//ball.y = ;
	ball.Draw(g_renderer);
	paddle_1.Draw(g_renderer);
	paddle_2.Draw(g_renderer);

	player_1_score_text.Draw();
	player_2_score_text.Draw();
	//================= Render all elements ================//
	SDL_RenderPresent(g_renderer);
}

// Proper shutdown and destroy initialized objects
void close() {

	// close the font
	TTF_CloseFont(scoreFont);
	TTF_Quit();

    // Destroy Renderer
    SDL_DestroyRenderer(g_renderer);
	//Destroy window
	SDL_DestroyWindow( g_window );

    // Point g_window to NULL to ensure it points to nothing.
	g_renderer = NULL;
    g_window = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

// the Update() helper function that provide a frame stablizer
// adapted from my lab 1
void update_with_timer(std::chrono::steady_clock::time_point &previous_time, double &elapsed_time_total, int &frame_counter, double &lag, double mcs_per_update) {
    // time recorders
    std::chrono::steady_clock::time_point current_time;
    double elapsed_time;
    // calculate how much time has elapsed since last record (usually 1 render loop earlier)
    current_time  = std::chrono::steady_clock::now();
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

// execute main game loop
void loop() {
	// If this is quit = 'true' then the program terminates.
    bool quit = false;
    // define microseconds per update
    double mcs_per_update = mcs_per_second / frame_rate;
    // elapsed_time_total - measure total time elapsed
    // lag - accumulate elapsed time for determining when to update to ensure steady frame rate
    double lag, elapsed_time_total = 0.0;
    // frame_counter - measure the real frame rate
    int frame_counter = 0;
    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event e;
    // Enable text input
    SDL_StartTextInput();
    // chrono::steadyclock for measuring times accurately while rendering
    std::chrono::steady_clock::time_point previous_time;
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
        // update with a frame stablizer
        update_with_timer(previous_time, elapsed_time_total, frame_counter, lag, mcs_per_update);
        // Render using SDL
        render();
    } 
    //Disable text input
    SDL_StopTextInput();
}
// Our entry point into our program.
// Remember, C++ programs have exactly one entry point
// where the program starts.
int main(int argc, char* args[])
{
	// Start up SDL and create window
	if (init_window()) loop();
	close();
	return 0;
}
