#ifndef PADDLE_CLASS
#define PADDLE_CLASS

#include "config.hpp"
#include "Vec2.hpp"


// class for playable paddles

class Paddle
{
public:
	Vec2 position;
	Vec2 velocity;
	SDL_Rect rect{};
	// similar to Ball.hpp
	Paddle(Vec2 position, Vec2 velocity)
		: position(position), velocity(velocity)
	{
		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
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
	}

	void Draw()
	{
		rect.y = static_cast<int>(position.y);
		SDL_RenderFillRect(g_renderer, &rect);
	}
};
// 2 paddles for external reference
extern Paddle paddle_1, paddle_2;

#endif