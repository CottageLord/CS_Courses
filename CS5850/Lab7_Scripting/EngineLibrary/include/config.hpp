#ifndef COMMON_H_
#define COMMON_H_

// ==================== SDL Libraries ==================
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
	#include <SDL2/SDL.h>
#endif
// ==================== C and C++ Libraries ==================
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>

//Screen dimension constants
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

const int BALL_WIDTH  = 15;
const int BALL_HEIGHT = 15;

const int PADDLE_WIDTH  = 15;
const int PADDLE_HEIGHT = 100;
// ======================== Game Rule== ===================//

const float PADDLE_SPEED = 1.0f;
const float BALL_SPEED = 1.0f;

int player_1_score = 0;
int player_2_score = 0;

const int SCORE_TO_WIN = 3;

bool player_1_win = false;
//====================== data structure ===================//

bool paddle_1_up = false;
bool paddle_1_down = false;
bool paddle_2_up = false;
bool paddle_2_down = false;

enum class CollisionType
{
	None,
	Top,
	Middle,
	Bottom,
	Left,
	Right
};

struct Contact
{
	CollisionType type;
	float penetration;
};

SDL_Renderer* g_renderer {NULL};

#endif