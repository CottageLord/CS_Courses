#include "config.hpp"
#include "Ball.hpp"
#include "Paddle.hpp"
#include "PlayerScore.hpp"
// ========================= Classes =======================

//Screen dimension constants
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

//Starts up SDL, creates window, and initializes OpenGL
bool init_window();

bool init_components();
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

// ================== frame stablizer vars ================//

const int frame_rate {60};
// I used microseconds instead of miliseconds for better precision
// using miliseconds (larger gap) would sometimes ignore time elapsed if
// the last update/render loop runs too fast
const int mcs_per_second {1000000};
// switch between ideal frame rate and crazy frame rate
const bool stable_frame {true};

// ====================== game objects ====================//

// Create the ball at the center of the screen
// extern from Ball.hpp hence the only reference
Ball ball(Vec2((SCREEN_WIDTH / 2.0f) - (BALL_WIDTH / 2.0f),
	(SCREEN_HEIGHT / 2.0f) - (BALL_WIDTH / 2.0f)));

// Create the paddles
Paddle paddle_1(Vec2(50.0f, 
	(SCREEN_HEIGHT / 2.0f) - (PADDLE_HEIGHT / 2.0f)));

Paddle paddle_2(Vec2(SCREEN_WIDTH - 50.0f, 
	(SCREEN_HEIGHT / 2.0f) - (PADDLE_HEIGHT / 2.0f)));

// Create the player score text fields
// player 1 at 1/4 width pos
PlayerScore player_1_score_text(Vec2(WINDOW_WIDTH / 4, 20), renderer, scoreFont, scoreColor);
// player 2 at 3/4 width pos
PlayerScore player_2_score_text(Vec2(3 * WINDOW_WIDTH / 4, 20), renderer, scoreFont, scoreColor);

// ======================SDL essentials ===================//

//The window we'll be rendering to
SDL_Window* g_window {NULL};
//SDL renderer
SDL_Renderer* g_renderer {NULL};

TTF_Font* scoreFont;
