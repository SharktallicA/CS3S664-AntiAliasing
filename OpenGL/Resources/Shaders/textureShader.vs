#version 330

uniform mat4 mvpMatrix;
uniform float resolutionX;
uniform float resolutionY;
uniform int enableSSAA;

layout (location=0) in vec4 vertexPos;
layout (location=2) in vec2 vertexTexCoord;

out float resX;
out float resY;
out float eblSSAA;
out vec2 texCoord;

void main(void) 
{
	resX = resolutionX;
	resY = resolutionY;
	eblSSAA = enableSSAA;
	texCoord = vertexTexCoord;
	gl_Position = mvpMatrix * vertexPos;
}