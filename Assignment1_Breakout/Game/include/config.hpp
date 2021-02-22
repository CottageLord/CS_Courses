/** @file config.hpp
@author yang hu
@version 1.0
@brief The fundamental libraries, macros, data structures and enums for the game
@date Monday, Feburuary 22, 2021
*/

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
// ======================= Basics =========================//
//Screen dimension constants

const int SCREEN_WIDTH = 1080; ///< The game window width
const int SCREEN_HEIGHT = 720; ///< The game window height

const float BRICK_SIDE_PADDING = 50; ///< The gap between bricks and the side wall
const float BRICK_TOP_PADDING  = 50; ///< The gap between bricks and the top wall
const float BRICK_AREA = 240; ///< The y-range that bricks could be aligned
const float BRICK_GAP = 1; ///< The gap between every brick
const int   PLAYER_LIFE = 3; ///< After falling 3 times, the player lose

const int PADDLE_WIDTH  = 100; ///< paddle's width
const int PADDLE_HEIGHT = 30; ///< paddle's height

const int BALL_WIDTH  = 15; ///< ball's width
const int BALL_HEIGHT = 15; ///< ball's height
// ======================== Game Rule== ===================//
// as I am using microseconds, the time step number will be large
// so I used slow speed as compensate
const float PADDLE_SPEED = 0.001f; ///< The speed of paddle, relates to our render time interval (16666 microsends per frame)
const float BALL_SPEED = 0.0005f; ///< The speed of ball, relates to our render time interval (16666 microsends per frame)

int player_life = PLAYER_LIFE; ///< initialize how many lives the player still have
int bricks_remained = 0; ///< initialize how many bricks remained

bool player_1_win = false; ///< holds the game result for game state checking
bool player_1_lose = false; ///< holds the game result for game state checking
bool ball_with_paddle = true; ///< tell game to pause when game starts/player falls/game win or lose


// ======================SDL essentials ===================//


SDL_Window* g_window {NULL}; ///< The SDL window we'll be rendering to

SDL_Renderer* g_renderer {NULL}; ///< The SDL renderer we will use


// ====================== game objects ====================//



bool pause = false; ///< pause the game for language selection, etc.
bool pre_game = true; ///< determine if the preperation has been done or not
bool language_selected = false; ///< tells if the player has chose a language
//====================== data structure ===================//

const char* ENGLISH_FILE ="../Assets/English.txt"; ///< file path for English text
const char* FRENCH_FILE ="../Assets/French.txt"; ///< file path for French text

std::string language_file = ENGLISH_FILE; ///< initialize the game language to English

const char* WALL_SOUND_FILE = "../Assets/wall_hit.wav"; ///< sound file path, played when the ball hit the wall
const char* PADDLE_SOUND_FILE = "../Assets/paddle_hit.wav"; ///< sound file path, played when the ball hit the paddle/bricks
const char* BGM_FILE = "../Assets/Frontier.mp3"; ///< sound file path, play background music
const char* FONT_FILE = "../Assets/DejaVuSansMono.ttf"; ///< font file path, used for displaying all texts

const std::vector<std::string> LEVEL_FILES = 
	{"../Assets/level_0", "../Assets/level_1", "../Assets/level_2"}; ///< level file path, used for generating multiple levels
const int LEVEL_NUM = 3; ///< record how many level remained
int current_level = 0; ///< record which level is the player playing

const char EMPTY_BRICK = '0'; ///< char represents a non-exist brick
const char REAL_BRICK  = '1'; ///< char represents an exist brick

// multi-language support
/**@enum Text_order
 * @brief indicates meanings of each line in localization text file
 */
enum class Text_order {
	Win_next = 0, ///< enum Win_next: displayed when player passes one level
	Win_end, ///< enum Win_end: displayed when player passes all levels
	Lose_end, ///< enum Lose_end: displayed when player loses
	Launch_ball, ///< enum Launch_ball: displayed when the ball is on the paddle
	Lifes, ///< enum Lifes: descriptive word for how many lives remaining
	Bricks ///< enum Bricks: descriptive word for how many bricks remaining
};

/**@enum Buttons
 * @brief Buttons that user has pressed
 */
enum class Buttons {
	paddle_1_left = 0, ///< enum paddle_1_left: a
	paddle_1_right, ///< enum paddle_1_right: d
	paddle_1_up, ///< enum paddle_1_up: w
	paddle_1_down ///< enum paddle_1_down: s
};

bool buttons[4] = {}; ///< which buttons are activated after pressing

/**@enum Collision_type
 * @brief which part of the collision surface does the collision happen
 */
enum class Collision_type { 
	None, ///< enum None: no collision happen
	Top, ///< enum Top: collision happens on vertical surface and hit the upper part
	Middle, ///< enum Middle: collision happens on either surface and hit the middle part
	Bottom, ///< enum Bottom: collision happens on vertical surface and hit the lower part
	Left, ///< enum Left: collision happens on horizontal surface and hit the left part
	Right ///< enum Right: collision happens on horizontal surface and hit the right part
};

/**@enum Collision_side
 * @brief which side of the object is being collided
 */
enum class Collision_side {
	None,
	Top,
	Bottom,
	Left,
	Right
};

/**@enum Brick_type
 * @brief describes whether the brick exists
 */
enum class Brick_type {
	None,
	Exist
};
///  @brief holds collision data like which side and which part of the side the collision happens
struct Contact { 
	Collision_type type; ///< which part of the side does collision happen
	Collision_side side; ///< which side does collision happen
	float penetration; ///< for compensating how much has the object penetrate the other one when collision detected
};

// ================== frame stablizer vars ================//

const int frame_rate {60}; ///< frame rate we want to keep
const int mcs_per_second {1000000}; ///< numbers of microseconds in one second, for calculation

// switch between ideal frame rate and crazy frame rate
const bool stable_frame {true}; ///< specifies if we need a fixed frame rate

#endif