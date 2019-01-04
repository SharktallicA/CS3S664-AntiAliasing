#pragma once

#include "Includes.h"
#include "Component.h"
#include "Renderer.h"

using namespace std;
using namespace glm;

//Object's Physics Computer component: handles the basis physics calculations needed per-frame and provides a core Newtonian physics interface
class Physics : public Component
{
private:
	//Cached Object velocity
	vec3 velocity;
	//Object's current mass
	double mass;
	//Cached Object position in last frame
	vec3 lastPosition;
	//
	vec3 overallForce;
	//
	vec3 overallTorque;
	//
	float overallTorqueAngle;

public:
	Physics(Object* parentRef, double startMass = 0.0f)
	{
		parent = parentRef;
		mass = startMass;
	}
	~Physics(void) { parent = nullptr; }

	//Per-frame tasks
	void Compute(float deltaTime)
	{
		//Update locational data
		velocity = (lastPosition - parent->GetPosition()) / deltaTime;
		lastPosition = parent->GetPosition();

		//Apply overall forces
		parent->renderer->matrix *= translate(mat4(1.0), overallForce);
		parent->renderer->matrix *= rotate(mat4(1.0), overallTorqueAngle, overallTorque);
	}

	//Returns the Object's velocity
	vec3 GetVelocity(void) { return velocity; }
	//Returns the Object's speed 
	double GetSpeed(void) { return sqrt(pow(velocity.x, 2) + pow(velocity.y, 2) + pow(velocity.z, 2)); }
	//Returns the Object's mass
	double GetMass(void) { return mass; }

	//
	void AddForce(vec3 incomingForce) { overallForce += incomingForce; }
	//
	void AddTorque(vec3 incomingTorque, float angle)
	{
		overallTorque += incomingTorque;

		if (overallTorqueAngle + angle <= 0)
		{
			angle = 360 - fmod(angle, 1);
		}
		else if (overallTorqueAngle + angle >= 360)
		{
			angle = 0 + fmod(angle, 1);
		}
		else overallTorqueAngle += angle;
	}
	//
	void AffectMass(double incomingMass) { mass += incomingMass; }

	//
	void ResetForce(void) { overallForce = vec3(0, 0, 0); }
	//
	void ResetTorque(void)
	{
		overallTorque = vec3(0, 0, 0);
		overallTorqueAngle = 0.0f;
	}
};