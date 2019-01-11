#version 330 core

layout (location = 0) in vec4 vertexPos;
layout (location = 1) in vec4 vertexColour;
layout (location = 3) in vec3 textureColour;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 VertexColour;

void main()
{
	gl_Position = ((projection * view) * model) * vertexPos;
	VertexColour = vertexColour;
}