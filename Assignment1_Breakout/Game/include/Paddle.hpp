/** @file Paddle.hpp
@author yang hu
@version 1.0
@brief Define the paddle movement and draw
@date Monday, Feburuary 22, 2021
*/
#ifndef PADDLE_CLASS
#define PADDLE_CLASS

#include "config.hpp"
#include "Vec2.hpp"
#include "Collision_obj.hpp"

/**@class Paddle
 *@brief A collidable, moveable box object
 *@details Defines the movement and draw of a paddle
*/
class Paddle : public Collision_obj {
public:
	/// @brief initialize paddle with given sizes
	Paddle(Vec2 position, Vec2 velocity)
		: Collision_obj(position, velocity)
	{
		rect.w = PADDLE_WIDTH;
		rect.h = PADDLE_HEIGHT;
	}

	/// @brief move the paddle by position = dt * v, bound paddle within window area
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

	/// @brief Draw the paddle 
	void Draw(SDL_Renderer* renderer)
	{
		rect.y = static_cast<int>(position.y);
		rect.x = static_cast<int>(position.x);
		SDL_RenderFillRect(renderer, &rect);
	}
};

extern Paddle paddle_1; ///< paddles for external reference

#endif