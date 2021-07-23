#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <map>

#include "Component.hpp"
#include "GraphicsEngineRenderer.hpp"

class GameObject{
    public:
        GameObject();
        ~GameObject();
        virtual void Update();
        virtual void Render(SDL_Renderer* ren);
        void AddComponent(int id, Component *comp);
		void RemoveComponent(int id);
    	Component *GetComponent(int id);
    private:
        std::map<int, Component*> m_components;
};


#endif
