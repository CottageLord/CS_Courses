#pragma once
#include "Behavior.h"

class BehaviorTree {
protected:
	Behavior* m_pRoot;
public:
	void tick();
};