/** @file Vec2.hpp
@author yang hu
@version 1.0
@brief Define behaviours and operations for a simple 2d vector
@details to represent position and velocity.
@date Monday, Feburuary 22, 2021
*/
#ifndef VEC_2
#define VEC_2

#include "config.hpp"

/// @brief 2d vector object to represent position and velocity.
class Vec2
{
public:
	/// @brief default initializer
	Vec2() : x(0.0f), y(0.0f) {}

	/// @brief specified initializer
	Vec2(float x, float y) : x(x), y(y) {}

	/// @brief specify 2-d vector addition
	Vec2 operator+(Vec2 const& rhs)
	{
		return Vec2(x + rhs.x, y + rhs.y);
	}
	/// @brief specify 2-d vector incrementation
	Vec2& operator+=(Vec2 const& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
	/// @brief specify 2-d vector scaling by multipling with a number
	Vec2 operator*(float rhs)
	{
		return Vec2(x * rhs, y * rhs);
	}
	float x, y; ///< the x and y values
};

#endif