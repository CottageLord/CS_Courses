
// ==================== Libraries ==================
// Depending on the operating system we use
// The paths to SDL are actually different.
// The #define statement should be passed in
// when compiling using the -D argument.
// This gives an example of how a programmer
// may support multiple platforms with different
// dependencies.
#if defined(LINUX) || defined(MINGW)
	#include <SDL2/SDL.h>
#else
	// Windows and Mac use a different path
	// If you have compilation errors, change this as needed.
	#include <SDL.h>
#endif
// ==================== C and C++ Libraries ==================
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>

// ==================== Libraries ==================

//Screen dimension constants
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

//Starts up SDL, creates window, and initializes OpenGL
bool init();

//Per frame update
void update();

//Renders quad to the screen
void render();

// main game loop
void loop();

// Proper shutdown and destroy initialized objects
void close();

// the Update() helper function that provide a frame stablizer
void update_with_timer(std::chrono::steady_clock::time_point &previous_time,
                    double &elapsed_time_total, int &frame_counter, 
                    double &lag, double mcs_per_update);

int frame_rate {60};
// I used microseconds instead of miliseconds for better precision
// using miliseconds (larger gap) would sometimes ignore time elapsed if
// the last update/render loop runs too fast
int mcs_per_second {1000000};
// switch between ideal frame rate and crazy frame rate
bool stable_frame {true};

//The window we'll be rendering to
SDL_Window* g_window {NULL};
//SDL renderer
SDL_Renderer* g_renderer {NULL};
