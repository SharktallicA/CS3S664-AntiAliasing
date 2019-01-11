#pragma once

#include "Effect.h"

class NewtonianGravity : public Effect
{
public:
	NewtonianGravity(void) {}

	void compute(Object* obj, Object* source, Constants constants)
	{
		//work out direction between both objects
		vec3 dir = obj->getPosition() - source->getPosition();

		//get distance from direction
		float dist = sqrt(pow(dir.x, 2) + pow(dir.y, 2) + pow(dir.z, 2));

		//calculate magnitude of the gravitational force
		double mag = (constants.G * obj->getMass() * source->getMass()) / pow(dist, 2);

		//convert calculation into workable force and apply resultant force
		vec3 Fg = normalize(dir) * (float)mag;
		//cout << glm::to_string(dir) << endl;
		obj->addForce(Fg);
	}
};