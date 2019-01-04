#include "Object.h"
#include "Renderer.h"
#include "Physics.h"

Object::Object(vec3 startPos, vec3 startRot, vec3 startSca)
{
	//Set start variables
	position = startPos;
	rotation = startRot;
	scale = startSca;
}
Object::~Object(void) { delete renderer, physics; }

void Object::Update(float deltaTime)
{
	if (renderer) renderer->Compute(deltaTime);
	else cout << "WARNING: object does not have a valid renderer attached\n";

	//if (physics) physics->Compute(deltaTime);
	//else cout << "WARNING: object does not have a valid physics computer attached\n";
}

vec3 Object::GetPosition(void) { return position; }
vec3 Object::GetRotation(void) { return rotation; }
vec3 Object::GetScale(void) { return scale; }

void Object::SetPosition(vec3 nPosition) { position = nPosition; }
void Object::SetRotation(vec3 nRotation) { rotation = nRotation; }
void Object::SetScale(vec3 newScale) { scale = newScale; }