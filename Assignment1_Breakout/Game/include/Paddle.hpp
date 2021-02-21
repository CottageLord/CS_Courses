#ifndef PADDLE_CLASS
#define PADDLE_CLASS

#include "config.hpp"
#include "Vec2.hpp"
#include "Collision_obj.hpp"

class Collision_obj;

const int PADDLE_WIDTH  = 100;
const int PADDLE_HEIGHT = 30;
// class for playable paddles

class Paddle : public Collision_obj {
public:
	// similar to Ball.hpp
	Paddle(Vec2 position, Vec2 velocity)
		: Collision_obj(position, velocity)
	{
		rect.w = PADDLE_WIDTH;
		rect.h = PADDLE_HEIGHT;
	}

	// move the paddle by position = dt * v
	void Update(float dt)
	{
		position += velocity * dt;

		if (position.y < 0)
		{
			// Restrict to top of the screen
			position.y = 0;
		}
		else if (position.y > (SCREEN_HEIGHT - PADDLE_HEIGHT))
		{
			// Restrict to bottom of the screen
			position.y = SCREEN_HEIGHT - PADDLE_HEIGHT;
		}
		
		if (position.x < 0)
		{
			// Restrict to left of the screen
			position.x = 0;
		}
		else if (position.x > (SCREEN_WIDTH - PADDLE_WIDTH))
		{
			// Restrict to right of the screen
			position.x = SCREEN_WIDTH - PADDLE_WIDTH;
		}
	}

	void Draw(SDL_Renderer* renderer)
	{
		rect.y = static_cast<int>(position.y);
		rect.x = static_cast<int>(position.x);
		SDL_RenderFillRect(renderer, &rect);
	}
};
// paddles for external reference
extern Paddle paddle_1, paddle_2;

#endif