#ifndef CONTROLLERCOMPONENT_HPP 
#define CONTROLLERCOMPONENT_HPP

#include "GraphicsEngineRenderer.hpp"
#include "Component.hpp"

class ControllerComponent : public Component{
    public:
    	enum class MoveKey { None, Up, Down, Left, Right };
    	MoveKey currKey;
        ControllerComponent();
        ~ControllerComponent();
        void AddEventListener(SDL_Event *newEvent);
        void Update();
        void Render(SDL_Renderer* ren);

    private:
    	SDL_Event *event;
};	


#endif
