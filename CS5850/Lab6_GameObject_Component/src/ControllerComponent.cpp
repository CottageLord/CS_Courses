#include "ControllerComponent.hpp"
#include "Config.hpp"
ControllerComponent::ControllerComponent(){
	bool quitApp = false;
}

ControllerComponent::~ControllerComponent(){
}
void ControllerComponent::AddEventListener(SDL_Event *newEvent) {
	event = newEvent;
}

void ControllerComponent::Update() {
	if (nullptr == event)
	{
		return;
	}
    //Handle events on queue
    while(SDL_PollEvent( event ) != 0){
    	if(event->type == SDL_QUIT) quitApp = true;
	    if (event->type == SDL_KEYDOWN) { // respond to various keyboard inputs
			if (event->key.keysym.sym == SDLK_a)		currKey = MoveKey::Left;
			else if (event->key.keysym.sym == SDLK_d)	currKey = MoveKey::Right;
			else if (event->key.keysym.sym == SDLK_w)	currKey = MoveKey::Up;
			else if (event->key.keysym.sym == SDLK_s)	currKey = MoveKey::Down;
			else if (event->key.keysym.sym == SDLK_q)   quitApp = true;
		} else if (event->type == SDL_KEYUP) {
			if (event->key.keysym.sym == SDLK_a)		currKey = MoveKey::None;
			else if (event->key.keysym.sym == SDLK_d)	currKey = MoveKey::None;
			else if (event->key.keysym.sym == SDLK_w)	currKey = MoveKey::None;
			else if (event->key.keysym.sym == SDLK_s)	currKey = MoveKey::None;
		}
    }
}
///@brief empty render
void ControllerComponent::Render(SDL_Renderer* ren) {
}