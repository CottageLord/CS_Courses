/** @file Ball.hpp
@author yang hu
@version 1.0
@brief Define the ball movement, collision behaviour and draw
@date Monday, Feburuary 22, 2021
*/

#ifndef BALL_CLASS
#define BALL_CLASS

#include "config.hpp"
#include "Vec2.hpp"
#include "Resource_manager.hpp"

/**@class Ball
 *@brief A collidable, moveable box object
 *@details Defines various movement behaviour when colliding with other objects
*/
class Ball : public Collision_obj {
public:

	/// @brief initialize ball with given position
	Ball(Vec2 position, Vec2 velocity)
		: Collision_obj(position, velocity) 
	{
		rect.w = BALL_WIDTH;
		rect.h = BALL_HEIGHT;
	}

	/// @brief update the ball movement with given delta time
	void Update(float dt)
	{
		position += velocity * dt;
	}

	/**
	 * @brief define ball behaviour when colliding with a paddle
	 * @details bouncing to left and right according to which part 
	 * of the paddle the ball collide on
	 * @param contact stores contact info like which side, which part of that
	 * side does the collision happens
	 */
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

	/**
	 * @brief define ball behaviour when colliding with a brick
	 * @details reverse x or y velocity according to which side collide on
	 * @param contact stores contact info like which side, which part of that
	 * side does the collision happens
	 */
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

	/**
	 * @brief define ball behaviour when colliding with a wall
	 * @details reverse x or y velocity according to which side collide on
	 * @param contact stores contact info like which side, which part of that
	 * side does the collision happens
	 */
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
	/// @brief send the draw command to renderer
	void Draw(SDL_Renderer* renderer)
	{
		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);

		SDL_RenderFillRect(renderer, &rect);
	}
};

extern Ball ball; ///< for outside object reference

#endif