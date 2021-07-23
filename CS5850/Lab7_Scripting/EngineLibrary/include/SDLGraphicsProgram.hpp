#ifndef SDLGRAPHICSPROGRAM_H
#define SDLGRAPHICSPROGRAM_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include "config.hpp"
#include "Ball.hpp"
#include "Paddle.hpp"
#include "collision.hpp"

// Purpose:
// This class sets up a full graphics program using SDL
//
//
//
class SDLGraphicsProgram{
public:

    // Constructor
    SDLGraphicsProgram(int w, int h);
    // Destructor
    ~SDLGraphicsProgram();
    // Setup OpenGL
    bool initGL();
    // loop that runs forever
    void loop();
    // the Update() helper function that provide a frame stablizer
    void update_with_timer(std::chrono::steady_clock::time_point &previous_time,
                    double &elapsed_time_total, int &frame_counter, 
                    double &lag, double mcs_per_update);

    bool handle_event();
    void update(double elapsed_time);
    void render();
    void delay(int milliseconds);
    Contact check_paddle_collision(Ball const& ball, Paddle const& paddle);
    Contact check_wall_collision(Ball const& ball);
    void StartTextInput();
    void StopTextInput();
    void resolve_collision(Ball &ball, Paddle &paddle_1, Paddle &paddle_2);
    void response_to_input(Paddle &paddle_1, Paddle &paddle_2);
    // Flips to new buffer
    void flip();
private:
    // Screen dimension constants
    int screenHeight;
    int screenWidth;
    // The window we'll be rendering to
    SDL_Window* gWindow ;
    // Our renderer
    SDL_Renderer* gRenderer;
};



// ======================SDL essentials ===================//

//The window we'll be rendering to
SDL_Window* g_window {NULL};
//SDL renderer



// ================== frame stablizer vars ================//

const int frame_rate {60};
// I used microseconds instead of miliseconds for better precision
// using miliseconds (larger gap) would sometimes ignore time elapsed if
// the last update/render loop runs too fast
const int mcs_per_second {1000000};
// as we use micro seconds, the time stamp of calculating update()
// could be too large
// this is for constraining the update loop time recorder to make sure
// classes like paddle() could accept a more reasonable number
const int time_cast {1500};
// switch between ideal frame rate and crazy frame rate
const bool stable_frame {true};


// ====================== game objects ====================//

// Create the ball at the center of the screen
Ball ball(Vec2((SCREEN_WIDTH / 2.0f) - (BALL_WIDTH / 2.0f),
    (SCREEN_HEIGHT / 2.0f) - (BALL_WIDTH / 2.0f)),
    Vec2(BALL_SPEED, 0.0f));

// Create the paddles
Paddle paddle_1(Vec2(50.0f, 
    (SCREEN_HEIGHT / 2.0f) - (PADDLE_HEIGHT / 2.0f)),
    Vec2(0.0f, 0.0f));

Paddle paddle_2(Vec2(SCREEN_WIDTH - 50.0f, 
    (SCREEN_HEIGHT / 2.0f) - (PADDLE_HEIGHT / 2.0f)),
    Vec2(0.0f, 0.0f));

bool pause = false;

#endif
