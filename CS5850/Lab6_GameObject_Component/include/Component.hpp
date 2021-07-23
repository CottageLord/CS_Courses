#ifndef COMPONENT_HPP 
#define COMPONENT_HPP

#include "GraphicsEngineRenderer.hpp"

class Component{
    public:
        Component();
        ~Component();
        virtual void Update();
        virtual void Render(SDL_Renderer* ren);

    private:

};


#endif
