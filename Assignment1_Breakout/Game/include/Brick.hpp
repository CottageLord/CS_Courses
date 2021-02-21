#ifndef BRICK_CLASS
#define BRICK_CLASS

#include "config.hpp"
#include "Vec2.hpp"
#include "Collision_obj.hpp"

class Collision_obj;

// class for playable paddles

class Brick : public Collision_obj {
public:
	Brick_type status;
	// 
	Brick(Vec2 position, Vec2 velocity)
		: Collision_obj(position, velocity) {}

	// move the paddle by position = dt * v
	

	void Draw(SDL_Renderer* renderer)
	{
		SDL_RenderFillRect(renderer, &rect);
	}
};

#endif