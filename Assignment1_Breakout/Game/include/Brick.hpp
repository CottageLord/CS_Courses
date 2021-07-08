/** @file Brick.hpp
@author yang hu
@version 1.0
@brief Define the brick
@date Monday, Feburuary 22, 2021
*/

#ifndef BRICK_CLASS
#define BRICK_CLASS

#include "config.hpp"
#include "Vec2.hpp"
#include "Collision_obj.hpp"

/**@class Brick
 *@brief A collidable, but unmoveable box object
 *@details Defines the draw of a brick
*/
class Brick : public Collision_obj {
public:
	Brick_type status; ///< defines if a brick exists in game
	/// @brief initialize brick with given sizes
	Brick(Vec2 position, Vec2 velocity)
		: Collision_obj(position, velocity) {}

	/// @brief draw the brick
	void Draw(SDL_Renderer* renderer)
	{
		SDL_RenderFillRect(renderer, &rect);
	}
};

#endif