#ifndef BALL_CLASS
#define BALL_CLASS

#include "config.hpp"
#include "Vec2.hpp"


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
		position.x += contact.penetration;
		velocity.x = -velocity.x;

		if (contact.type == CollisionType::Top)
		{
			velocity.y = -.75f * BALL_SPEED;
		}
		else if (contact.type == CollisionType::Bottom)
		{
			velocity.y = 0.75f * BALL_SPEED;
		}
	}

	void collide_with_wall(Contact const& contact)
	{
		if ((contact.type == CollisionType::Top)
		    || (contact.type == CollisionType::Bottom))
		{
			position.y += contact.penetration;
			velocity.y = -velocity.y;
		}
		// if the ball falls, reset the ball pos
		else if (contact.type == CollisionType::Left)
		{
			position.x = SCREEN_WIDTH / 2.0f;
			position.y = SCREEN_HEIGHT / 2.0f;
			velocity.x = BALL_SPEED;
			velocity.y = 0.f;//0.75f * BALL_SPEED;
		}
		else if (contact.type == CollisionType::Right)
		{
			position.x = SCREEN_WIDTH / 2.0f;
			position.y = SCREEN_HEIGHT / 2.0f;
			velocity.x = -BALL_SPEED;
			velocity.y = 0.f;//0.75f * BALL_SPEED;
		}
	}
	// send the draw command to renderer
	void Draw()
	{
		//std::cout << "Drawing ball at: (" << rect.x << ", " << rect.y << ")" << std::endl;
		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);

		SDL_RenderFillRect(g_renderer, &rect);
	}
};

extern Ball ball;

#endif