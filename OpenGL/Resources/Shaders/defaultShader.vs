#version 330 core

layout (location = 0) in vec4 vertexPos;
layout (location = 1) in vec4 vertexColour;
layout (location = 2) in vec3 vertexOffset;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float scalar;

out vec4 VertexColour;

void main()
{
	//vec4 D = vertexPos;
	//D.xyz += vertexOffset;

	gl_Position = projection * view * model  * vertexPos;
	VertexColour = vertexColour;
}