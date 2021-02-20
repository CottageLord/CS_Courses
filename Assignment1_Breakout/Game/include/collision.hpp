#ifndef COLLISION_CLASS
#define COLLISION_CLASS
// check collision with Separating Axis Theorem
#include "config.hpp"
#include "Ball.hpp"
#include "Paddle.hpp"

// check if the ball hit the obj
Contact check_obj_collision(Ball const& ball, Collision_obj const& obj)
{
	float ball_left 	= ball.position.x;
	float ball_right 	= ball.position.x + ball.rect.w;
	float ball_top 		= ball.position.y;
	float ball_bottom 	= ball.position.y + ball.rect.h;

	float obj_left 	= obj.position.x;
	float obj_right 	= obj.position.x + obj.rect.w;
	float obj_top 	= obj.position.y;
	float obj_bottom = obj.position.y + obj.rect.h;

	Contact contact{};

	// (a simple Separating Axis Theorem system)

	// if collision doesn't occur, return none
	if (ball_left 	>= obj_right) 	return contact; // x| o
	if (ball_right 	<= obj_left) 	return contact; // o |x
	if (ball_top 	>= obj_bottom) 	return contact;
	if (ball_bottom <= obj_top) 		return contact;

	//std::cout << "collide";
	// ball launches with diff speed when hitting diff obj speed
	float obj_range_left  = obj_right - (2.0f * obj.rect.w / 3.0f);
	float obj_range_mid_w = obj_right - (obj.rect.w / 3.0f);
	float obj_range_top   = obj_bottom - (2.0f * obj.rect.h / 3.0f);
	float obj_range_mid_h = obj_bottom - (obj.rect.h / 3.0f);

	// used to determine which side the ball collide with the obj
	// see doc for more detail
	
	float ball_center_x 	= ball.position.x + ball.rect.w / 2;
	float ball_center_y 	= ball.position.y + ball.rect.h / 2;
	float obj_center_x 	= obj.position.x + obj.rect.w / 2;
	float obj_center_y 	= obj.position.y + obj.rect.h / 2;

	float delta_x = obj_center_x - ball_center_x;
	float delta_y = obj_center_y - ball_center_y;

	float obj_h_w_ratio 	= obj.rect.h / obj.rect.w;
	float ball_obj_ratio = std::abs(delta_y / delta_x);
	// if the ball hits top/bottom of the object
	if (ball_obj_ratio > obj_h_w_ratio) {
		// if ball is moving down, enter the top of the object
		// negative complement, push ball upward 
		if (ball.velocity.y > 0) {
			contact.side = CollisionSide::Top;
			contact.penetration = obj_top - ball_bottom;
		}
		// if ball is moving up, enter the bottom of the object
		// positive complement, push ball downward  
		else {
			contact.side = CollisionSide::Bottom;
			contact.penetration = obj_bottom - ball_top;
		}
		// change y move direction
		if (ball_right < obj_range_left) contact.type = CollisionType::Left;
		else if (ball_right < obj_range_mid_w)  contact.type = CollisionType::Middle;
		else contact.type = CollisionType::Right;
	}
	// if the ball hit the left/right side of the object
	else {
		//std::cout << "hit lr!" << std::endl;
		// if ball is moving right, enter the left of the object
		// negative complement, push ball left 
		if (ball.velocity.x > 0)
		{
			contact.side = CollisionSide::Left;
			contact.penetration = obj_left - ball_right;
		// if ball is moving left, enter the right of the object
		// positive complement, push ball right 
		} else {
			contact.side = CollisionSide::Right;
			contact.penetration = obj_right - ball_left;
		}
		if (ball_bottom < obj_range_top) contact.type = CollisionType::Top;
		else if (ball_bottom < obj_range_mid_h)  contact.type = CollisionType::Middle;
		else contact.type = CollisionType::Bottom;
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
		contact.penetration = -ball_left;
	}
	else if (ball_right > SCREEN_WIDTH)
	{
		contact.type = CollisionType::Right;
		contact.penetration = SCREEN_WIDTH - ball_right;
	}
	else if (ball_top < 0.0f)
	{
		contact.type = CollisionType::Top;
		contact.penetration = -ball_top;
	}
	// falls
	else if (ball_bottom > SCREEN_HEIGHT)
	{
		contact.penetration = SCREEN_HEIGHT - ball_bottom;
		contact.type = CollisionType::Bottom;
	}

	return contact;
}

#endif