/** @file lab.hpp
@author yang hu
@version 1.0
@brief The interface file describing the game's main logic loop.
@details The game implements a conventional input-update-render loop, 
and this file holds the main logic of that game process. All objects'
Update() and Render() functions will be called here.
@date Monday, Feburuary 22, 2021
*/

#include "config.hpp"
#include "Ball.hpp"
#include "Paddle.hpp"
#include "Brick.hpp"
#include "tiny_math.hpp"
#include "Resource_manager.hpp"
#include "Text_display.hpp"
// ========================= Classes =======================

/// @brief Starts up SDL, creates window, and initializes OpenGL
bool init_window();

/// @brief Starts up text display, audio player and singleton resource manager
bool init_components();

/// @brief A little helper function that checks if things like language selection have been done
bool game_running();

/// @brief renders a language selection text before the game starts
void language_selection();

/// @brief The first function in each loop. Read user input and update game state accordingly
void handle_event(bool &quit);

/// @brief The second function in each loop. Update all game objects' properties like position
void update();

/// @brief The third function in each loop. Render all game objects.
void render();

/// @brief Triggers the game's main while-loop.
void loop();

/// @brief Properly shutdown and destroy initialized objects
void close();
/**
 * @brief An update() wrapper function that implements a frame stablizer
 * @param previous_time The time stamp recorded one loop earlier
 * @param elapsed_time_total Total time spent since last second, in microseconds
 * @param frame_counter The total frame rendered in the current second
 * @param lag Total time spent since last render
 * @param mcs_per_update A calculated constant that specifies how many microseconds we have for rendering 1 frame
 */
// the Update() helper function that provide a frame stablizer
void update_with_timer(std::chrono::steady_clock::time_point &previous_time,
                    double &elapsed_time_total, int &frame_counter, 
                    double &lag, double &mcs_per_update);

// initialize global objects
Ball ball(Vec2((SCREEN_WIDTH / 2.0f) - (BALL_WIDTH / 2.0f),
	(SCREEN_HEIGHT / 2.0f) - (BALL_WIDTH / 2.0f)),
	Vec2(0.0f, BALL_SPEED)); ///< initialize the ball at the center of the screen

// Create the paddles
Paddle paddle_1(Vec2(50.0f, 
	SCREEN_HEIGHT - PADDLE_HEIGHT * 2.0f),
	Vec2(0.0f, 0.0f)); ///< initialize the paddle at the mid-bottom of the screen
