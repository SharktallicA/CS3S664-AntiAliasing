#pragma once

using namespace std;
#include "Object.h"

//Ayy
class Component
{
protected:
	//Reference to parent object
	Object* parent;

public:
	Component() {}

	//Per-frame tasks
	virtual void Compute(float deltaTime) {}
};