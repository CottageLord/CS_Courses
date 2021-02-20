#ifndef COLLISION_OBJ_CLASS
#define COLLISION_OBJ_CLASS
#include "config.hpp"

class Collision_obj {
	public:
	Vec2 position;
	Vec2 velocity;
	SDL_Rect rect{};

	Collision_obj(Vec2 position, Vec2 velocity)
		: position(position), velocity(velocity)
	{
		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
	}

};

#endif