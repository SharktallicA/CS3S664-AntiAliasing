#pragma once
/*
	Relativistic Physics Engine
	Khalid Ali 2018
	http://khalidali.co.uk/
*/

#include "Object.h"
#include "Effects/NewtonianGravity.h"
#include <vector>

using namespace std;

//Relativistic Physics Engine
class PhysicsEngine
{
private:
    // Scale for controlling the effect of universal constants
	float scale = 1.0f;

    // Vector of all effect processors
	vector<Effect*> processors;
 
public:
	PhysicsEngine(void) { processors.push_back(new NewtonianGravity()); }
	~PhysicsEngine(void) { for (Effect* e : processors) delete e;}

    // Per-frame computation maintainer
	void update(Object* current, vector<Object*> bodies)
	{
		for (Object* y : bodies) if (current != y)
			for (Effect* e : processors)
				e->compute(current, y, Constants());
	}
};