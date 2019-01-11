#pragma once
#include "Includes.h"

using namespace std;
using namespace glm;

class Renderer;
class Physics;

// Scene object class
class Object
{
private:
	//Object's position in scene
	vec3 position;
	//Object's rotation in scene
	vec3 rotation;
	//Object's render scale
	vec3 scale;

public:
	//Object's Renderer component
	Renderer* renderer;
	//Object's Physics Computer component
	Physics* physics;

	Object(vec3 startPos, vec3 startRot, vec3 startSca);
	~Object(void);

	//Per-frame tasks
	void Update(float deltaTime);

	//Returns Object's position in scene space
	vec3 GetPosition(void);
	//Returns Object's rotation in scene space
	vec3 GetRotation(void);
	//Returns Object's scale in scene space
	vec3 GetScale(void);

	//Allows new position to be specified (bypassing Physics control)
	void SetPosition(vec3 nPosition);
	//Allows new rotation to be specified (bypassing Physics control)
	void SetRotation(vec3 nRotation);
	//Allows new scale to be specified
	void SetScale(vec3 newScale);

	//Attachs/overwrites Renderer component
	void AttachRenderer(Renderer* nRenderer) { renderer = nRenderer; }
	//Attachs/overwrites Physics Computer component
	void AttachPhysics(Physics* nPhysics) { physics = nPhysics; }
};