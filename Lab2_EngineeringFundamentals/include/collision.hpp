#ifndef COLLISION_CLASS
#define COLLISION_CLASS
// check collision with Separating Axis Theorem
#include "config.hpp"
#include "Ball.hpp"
#include "Paddle.hpp"

// check if the ball hit the paddle
Contact check_paddle_collision(Ball const& ball, Paddle const& paddle)
{
	float ball_left 	= ball.position.x;
	float ball_right 	= ball.position.x + ball.rect.w;
	float ball_top 		= ball.position.y;
	float ball_bottom 	= ball.position.y + ball.rect.h;

	float paddle_left 	= paddle.position.x;
	float paddle_right 	= paddle.position.x + paddle.rect.w;
	float paddle_top 	= paddle.position.y;
	float paddle_bottom = paddle.position.y + paddle.rect.h;

	Contact contact{};

	// if collision occurs (a simple Separating Axis Theorem system)
	if (ball_left 	>= paddle_right) 	return contact;
	if (ball_right 	<= paddle_left) 	return contact;
	if (ball_top 	>= paddle_bottom) 	return contact;
	if (ball_bottom <= paddle_top) 		return contact;

	// ball launches with diff speed when hitting diff paddle speed

	float paddle_range_upper  = paddle_bottom - (2.0f * paddle.rect.h / 3.0f);
	float paddle_range_middle = paddle_bottom - (paddle.rect.h / 3.0f);

	if (ball.velocity.x < 0)
	{
		// Left paddle
		contact.penetration = paddle_right - ball_left;
	}
	else if (ball.velocity.x > 0)
	{
		// Right paddle
		contact.penetration = paddle_left - ball_right;
	}

	if ((ball_bottom > paddle_top)
	    && (ball_bottom < paddle_range_upper))
	{
		contact.type = CollisionType::Top;
	}
	else if ((ball_bottom > paddle_range_upper)
	     && (ball_bottom < paddle_range_middle))
	{
		contact.type = CollisionType::Middle;
	}
	else
	{
		contact.type = CollisionType::Bottom;
	}

	return contact;
}

// check if the ball hit the wall
Contact check_wall_collision(Ball const& ball) {

	float ball_left 	= ball.position.x;
	float ball_right 	= ball.position.x + ball.rect.w;
	float ball_top 		= ball.position.y;
	float ball_bottom 	= ball.position.y + ball.rect.h;

	Contact contact{};

	if (ball_left < 0.0f)
	{
		contact.type = CollisionType::Left;
	}
	else if (ball_right > SCREEN_WIDTH)
	{
		contact.type = CollisionType::Right;
	}
	else if (ball_top < 0.0f)
	{
		contact.type = CollisionType::Top;
		contact.penetration = -ball_top;
	}
	else if (ball_bottom > SCREEN_HEIGHT)
	{
		contact.type = CollisionType::Bottom;
		contact.penetration = SCREEN_HEIGHT - ball_bottom;
	}

	return contact;
}

#endif