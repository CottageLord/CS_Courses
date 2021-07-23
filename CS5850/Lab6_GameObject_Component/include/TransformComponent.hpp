#ifndef TRANSFORMCOMPONENT_HPP
#define TRANSFORMCOMPONENT_HPP

#include <utility>
#include "Component.hpp"
#include "ControllerComponent.hpp"

class TransformComponent : public Component{
    public:
        TransformComponent();
        ~TransformComponent();
        void Update();
        void Render(SDL_Renderer* ren);
        void Move(int x, int y);
        void SetController(ControllerComponent* newControl);
        std::pair<int, int> GetDistance(int x, int y);
        std::pair<int, int> GetWorldPosition();
		std::pair<int, int> GetScreenPosition();

    private:
    	int screenPosX;
    	int screenPosY;
    	int worldPosX;
    	int worldPosY;
    	int scale;
    	ControllerComponent *controller;
};


#endif
