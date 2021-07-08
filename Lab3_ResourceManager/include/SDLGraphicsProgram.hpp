#ifndef SDLGRAPHICSPROGRAM_HPP
#define SDLGRAPHICSPROGRAM_HPP

// ==================== Libraries ==================
// Depending on the operating system we use
// The paths to SDL are actually different.
// The #define statement should be passed in
// when compiling using the -D argument.
// This gives an example of how a programmer
// may support multiple platforms with different
// dependencies.
//
// Note that your path may be different depending on where you intalled things
//
//
#if defined(LINUX) || defined(MINGW)
    #include <SDL2/SDL.h>
#else // This works for Mac
    #include <SDL.h>
#endif

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <chrono>

//const char* SPRITE_PATH = "./sprite.bmp";
// This class sets up a full graphics program
class SDLGraphicsProgram{
public:

    // Constructor
    SDLGraphicsProgram(int w, int h);
    // Desctructor
    ~SDLGraphicsProgram();
    // Per frame update
    void update();
    // the Update() helper function that provide a frame stablizer
    void update_with_timer(std::chrono::steady_clock::time_point &previous_time,
                    double &elapsed_time_total, int &frame_counter, 
                    double &lag, double mcs_per_update);
    // Renders shapes to the screen
    void render();
    // loop that runs forever
    void loop();
    void destroy();
    // Get Pointer to Window
    SDL_Window* getSDLWindow();
    // Get Pointer to Renderer
    SDL_Renderer* getSDLRenderer();

private:
    // Screen dimension constants
    int screenHeight;
    int screenWidth;
    // The window we'll be rendering to
    SDL_Window* gWindow ;
    // SDL Renderer
    SDL_Renderer* gRenderer = NULL;
};

//const int frame_rate {30};
// I used microseconds instead of miliseconds for better precision
// using miliseconds (larger gap) would sometimes ignore time elapsed if
// the last update/render loop runs too fast
const int mcs_per_second {1000000};
const bool stable_frame {true};
// this prevent the sprites with less grid to move too fast
const double sprite_stablizer {1.1};
#endif
