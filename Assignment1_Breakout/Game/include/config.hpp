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
	#include <SDL2/SDL_ttf.h>
#else
	// Windows and Mac use a different path
	// If you have compilation errors, change this as needed.
	#include <SDL.h>
	#include <SDL_ttf.h>
#endif
// ==================== C and C++ Libraries ==================
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <cmath>
//Screen dimension constants
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

// ======================== Game Rule== ===================//

const float PADDLE_SPEED = 0.01f;
const float BALL_SPEED = 0.01f;

int player_1_score = 0;
int player_2_score = 0;

const int SCORE_TO_WIN = 3;

bool player_1_win = false;
//====================== data structure ===================//

enum Buttons
{
	paddle_1_left = 0,
	paddle_1_right,
	paddle_1_up,
	paddle_1_down,
};

bool buttons[4] = {};

enum class CollisionType
{
	None,
	Top,
	Middle,
	Bottom,
	Left,
	Right
};

enum class CollisionSide
{
	None,
	Top,
	Bottom,
	Left,
	Right
};

struct Contact
{
	CollisionType type;
	CollisionSide side;
	float penetration;
};


#endif