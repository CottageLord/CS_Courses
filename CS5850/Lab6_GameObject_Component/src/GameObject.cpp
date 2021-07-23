#include "GameObject.hpp"

GameObject::GameObject(){
}

GameObject::~GameObject(){
}

void GameObject::Update(){
    for (auto myPair : m_components) {
        myPair.second->Update();
    }
}

void GameObject::Render(SDL_Renderer* ren){
	for (auto myPair : m_components) {
        myPair.second->Render(ren);
    }
}

void GameObject::AddComponent(int id, Component *comp){
	m_components.emplace(id, comp);
}

void GameObject::RemoveComponent(int id){
	if(m_components.find(id) != m_components.end()) {
		delete m_components[id]; // clean memory
		m_components.erase (id); // erasing by key
	}
}

Component *GameObject::GetComponent(int id) {
	if(m_components.find(id) != m_components.end()) return m_components[id];
	return nullptr;
}
