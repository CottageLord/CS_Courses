#include "config.hpp"
#include "Ball.hpp"
#include "Paddle.hpp"
#include "collision.hpp"
#include "PlayerScore.hpp"
#include "ScoreDisplay.hpp"
// ========================= Classes =======================

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
                    double &lag, double &mcs_per_update);

// ======================SDL essentials ===================//

//The window we'll be rendering to
SDL_Window* g_window {NULL};
//SDL renderer
SDL_Renderer* g_renderer {NULL};

// Initialize the font
TTF_Font* score_font;

// ================== frame stablizer vars ================//

const int frame_rate {60};
const int mcs_per_second {1000000};

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
// Create the player score text fields
ScoreDisplay score_display(0,0);