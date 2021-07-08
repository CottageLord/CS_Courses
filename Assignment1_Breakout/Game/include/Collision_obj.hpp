/** @file Collision_obj.hpp
@author yang hu
@version 1.0
@brief Define the root object for all collideable objects
@date Monday, Feburuary 22, 2021
*/
#ifndef COLLISION_OBJ_CLASS
#define COLLISION_OBJ_CLASS
#include "config.hpp"

/**@class Collision_obj
 *@brief Root object for all collideable objects
*/
class Collision_obj {
	public:
	Vec2 position; ///< object's position, at the top left corner (close to (0,0))
	Vec2 velocity; ///< object's velocity pn x and y direction
	SDL_Rect rect{}; ///< object's box area

	/// @brief initialize the object with position
	Collision_obj(Vec2 position, Vec2 velocity)
		: position(position), velocity(velocity)
	{
		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
	}

};

#endif