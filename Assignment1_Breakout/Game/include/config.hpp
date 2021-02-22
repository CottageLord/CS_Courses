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
	#include <SDL2/SDL_mixer.h>
#else
	// Windows and Mac use a different path
	// If you have compilation errors, change this as needed.
	#include <SDL.h>
	#include <SDL_ttf.h>
	#include <SDL_mixer.h>
#endif
// ==================== C and C++ Libraries ==================
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <vector>
#include <cmath>
//Screen dimension constants
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

const float BRICK_SIDE_PADDING = 50;
const float BRICK_TOP_PADDING  = 50;
const float BRICK_AREA = 240;
const float BRICK_GAP = 1;
const int   PLAYER_LIFE = 3;
// ======================== Game Rule== ===================//
// as I am using microseconds, the time step number will be large
// so I used slow speed as compensate
const float PADDLE_SPEED = 0.001f;
const float BALL_SPEED = 0.0005f;
/*
int player_1_score = 0;
int player_2_score = 0;*/

int player_life = PLAYER_LIFE;
int bricks_remained = 0;

const int SCORE_TO_WIN = 3;

bool player_1_win = false;
bool player_1_lose = false;
bool ball_with_paddle = true;
//====================== data structure ===================//

const char* ENGLISH_FILE ="../Assets/English.txt";
const char* FRENCH_FILE ="../Assets/French.txt";
bool language_selected = false;
std::string language_file = ENGLISH_FILE;
//const std::string LEVEL_FILE = "Assets/level_1";
const char* WALL_SOUND_FILE = "../Assets/wall_hit.wav";
const char* PADDLE_SOUND_FILE = "../Assets/paddle_hit.wav";
const char* BGM_FILE = "../Assets/PaketPhoenixIndiHome.mp3";
const char* FONT_FILE = "../Assets/DejaVuSansMono.ttf";

const std::vector<std::string> LEVEL_FILES = 
	{"../Assets/level_0", "../Assets/level_1", "../Assets/level_2"};
const int LEVEL_NUM = 3;
int current_level = 0;

// multi-language support
enum class Text_order
{
	Win_next = 0,
	Win_end,
	Lose_end,
	Launch_ball,
	Lifes,
	Bricks
};

enum Buttons
{
	paddle_1_left = 0,
	paddle_1_right,
	paddle_1_up,
	paddle_1_down
};

bool buttons[4] = {};

enum class Collision_type
{
	None,
	Top,
	Middle,
	Bottom,
	Left,
	Right
};

enum class Collision_side
{
	None,
	Top,
	Bottom,
	Left,
	Right
};

enum class Brick_type
{
	None,
	Exist
};

struct Contact
{
	Collision_type type;
	Collision_side side;
	float penetration;
};

// ================== frame stablizer vars ================//

const int frame_rate {60};
const int mcs_per_second {1000000};

// switch between ideal frame rate and crazy frame rate
const bool stable_frame {true};

#endif