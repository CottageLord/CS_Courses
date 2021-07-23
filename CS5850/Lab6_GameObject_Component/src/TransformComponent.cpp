#include "TransformComponent.hpp"
#include <iostream>

TransformComponent::TransformComponent(){
	screenPosX = 0;
	screenPosY = 0;
	worldPosX = 0;
	worldPosY = 0;
	int scale = 1;
}

TransformComponent::~TransformComponent(){
}

void TransformComponent::SetController(ControllerComponent* newControl) {
	controller = newControl;
}

void TransformComponent::Update(){
	if (!controller)
	{
		return;
	}
	switch(controller->currKey){
		case controller->MoveKey::Left :
			worldPosX -= PLAYER_SPEED;
			break;
		case controller->MoveKey::Right :
			worldPosX += PLAYER_SPEED;
			break;
		case controller->MoveKey::Up :
			worldPosY -= PLAYER_SPEED;
			break;
		case controller->MoveKey::Down :
			worldPosY += PLAYER_SPEED;
			break;
		default : // MoveKey::None
			break;
	}
	std::cout << "current world pos: [" << worldPosX << ", " << worldPosY << std::endl;
    // prevent out of bound
    if (worldPosX >= LEVEL_WIDTH - TILE_WIDTH) worldPosX = LEVEL_WIDTH - TILE_WIDTH;

    if (worldPosY >= LEVEL_HEIGHT - TILE_HEIGHT) worldPosY = LEVEL_HEIGHT - TILE_HEIGHT;

    //int tileMapMidPointX = (float)TILE_ON_WINDOW_W / 2;
    //int tileMapMidPointY = (float)TILE_ON_WINDOW_H / 2;

    if (worldPosX <= 0) worldPosX = 0;
    if (worldPosY <= 0) worldPosY = 0;
    
}

std::pair<int, int> TransformComponent::GetWorldPosition() {
	return std::make_pair(worldPosX, worldPosY);
}

std::pair<int, int> TransformComponent::GetScreenPosition() {
	// adjust screen pos according to world pos
	if (worldPosX > WINDOW_W_HALF && worldPosX < LEVEL_WIDTH - WINDOW_W_HALF) {
		screenPosX = WINDOW_W_HALF;
	} else if (worldPosX <= WINDOW_W_HALF) {
		screenPosX = worldPosX;
	} else if (worldPosX >= LEVEL_WIDTH - WINDOW_W_HALF) {
		screenPosX = worldPosX - (LEVEL_WIDTH - WINDOW_WIDTH);
	}

	if (worldPosY > WINDOW_H_HALF && worldPosY < LEVEL_HEIGHT - WINDOW_H_HALF) {
		screenPosY = WINDOW_H_HALF;
	} else if (worldPosY <= WINDOW_H_HALF) {
		screenPosY = worldPosY;
	} else if (worldPosY >= LEVEL_HEIGHT - WINDOW_H_HALF){
		screenPosY = worldPosY - (LEVEL_HEIGHT - WINDOW_HEIGHT);
	}
	return std::make_pair(screenPosX, screenPosY);
}

std::pair<int, int> TransformComponent::GetDistance(int x, int y) {
	return std::make_pair(worldPosX - x, worldPosY - y);
}

void TransformComponent::Move(int x, int y) {
	worldPosX += x;
	worldPosY += y;
}
///@brief empty render
void TransformComponent::Render(SDL_Renderer* ren){
}