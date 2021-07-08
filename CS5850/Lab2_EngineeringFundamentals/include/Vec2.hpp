#ifndef VEC_2
#define VEC_2

#include "config.hpp"

// to represent position and velocity.
class Vec2
{
public:
	Vec2() : x(0.0f), y(0.0f) {} // default initializer

	Vec2(float x, float y) : x(x), y(y) {} // specified initializer

	// specify 2-d vector addition
	Vec2 operator+(Vec2 const& rhs)
	{
		return Vec2(x + rhs.x, y + rhs.y);
	}
	// specify 2-d vector incrementation
	Vec2& operator+=(Vec2 const& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
	// specify 2-d vector scaling by multipling with a number
	Vec2 operator*(float rhs)
	{
		return Vec2(x * rhs, y * rhs);
	}
	float x, y;
};

#endif