#ifndef PADDLE_CLASS
#define PADDLE_CLASS

#include "config.hpp"
#include "Vec2.hpp"

const int PADDLE_WIDTH  = 15;
const int PADDLE_HEIGHT = 100;
// class for playable paddles
class Paddle
{
public:
	// similar to Ball.hpp
	Paddle(Vec2 position) : position(position)
	{
		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
		rect.w = PADDLE_WIDTH;
		rect.h = PADDLE_HEIGHT;
	}

	void Draw(SDL_Renderer* renderer)
	{
		rect.y = static_cast<int>(position.y);

		SDL_RenderFillRect(renderer, &rect);
	}

	Vec2 position;
	SDL_Rect rect{};
};
// 2 paddles for external reference
extern Paddle paddle_1, paddle_2;

#endif