#ifndef BALL_CLASS
#define BALL_CLASS

#include "config.hpp"
#include "Vec2.hpp"

const int BALL_WIDTH  = 15;
const int BALL_HEIGHT = 15;

class Ball
{
	Vec2 position;
	SDL_Rect rect{};
	// A structure that contains the definition of a rectangle, 
	// with the origin at the upper left (x,y).
public:
	// initialize ball with given position
	Ball(Vec2 position) : position(position)
	{
		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
		rect.w = BALL_WIDTH;
		rect.h = BALL_HEIGHT;
	}

	// send the draw command to renderer
	void Draw(SDL_Renderer* renderer)
	{
		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);

		SDL_RenderFillRect(renderer, &rect);
	}
};

extern Ball ball;

#endif