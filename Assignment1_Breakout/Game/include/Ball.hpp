#ifndef BALL_CLASS
#define BALL_CLASS

#include "config.hpp"
#include "Vec2.hpp"
#include "level_manager.hpp"

const int BALL_WIDTH  = 15;
const int BALL_HEIGHT = 15;

class Ball
{
public:
	Vec2 position;
	Vec2 velocity;
	SDL_Rect rect{};
	// A structure that contains the definition of a rectangle, 
	// with the origin at the upper left (x,y).

	// initialize ball with given position
	Ball(Vec2 position, Vec2 velocity)
		: position(position), velocity(velocity)
	{
		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
		rect.w = BALL_WIDTH;
		rect.h = BALL_HEIGHT;
	}

	void Update(float dt)
	{
		position += velocity * dt;
	}

	void collide_with_paddle(Contact const& contact)
	{
		// if collide up/bottom
		if (contact.side == Collision_side::Top ||
			contact.side == Collision_side::Bottom) {
			position.y += contact.penetration;
			velocity.y = -velocity.y;
			if (contact.type == Collision_type::Left) 
				velocity.x = -.75f * BALL_SPEED;
			else if (contact.type == Collision_type::Right) 
				velocity.x = .75f * BALL_SPEED;
		// if collide left/right
		} else if (contact.side == Collision_side::Left ||
			contact.side == Collision_side::Right) {
			position.x += contact.penetration;
			velocity.x = -velocity.x;
			// for this game only, only reflect upwards
			velocity.y = -.75f * BALL_SPEED;
			/*
			if (contact.type == Collision_type::Top) 
				velocity.y = -.75f * BALL_SPEED;
			else if (contact.type == Collision_type::Bottom) 
				velocity.y = .75f * BALL_SPEED;*/
		}
	}

	void collide_with_brick(Contact const& contact) {
		// reflect the ball
		if (contact.side == Collision_side::Top ||
			contact.side == Collision_side::Bottom) {
			position.y += contact.penetration;
			velocity.y = -velocity.y;
		} else if (contact.side == Collision_side::Left ||
			contact.side == Collision_side::Right) {
			position.x += contact.penetration;
			velocity.x = -velocity.x;
		}
	}
	void collide_with_wall(Contact const& contact)
	{
		if (contact.type == Collision_type::Right
		    || contact.type == Collision_type::Left)
		{
			position.x += contact.penetration;
			velocity.x = -velocity.x;
		}
		else if (contact.type == Collision_type::Top)
		{
			position.y += contact.penetration;
			velocity.y = -velocity.y;
		}
		// if the ball falls
		else if (contact.type == Collision_type::Bottom)
		{
			ball_with_paddle = true;
			velocity.x = 0.0f;
			velocity.y = BALL_SPEED;//0.75f * BALL_SPEED;
			// reduce player life
			player_life--;
		}
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