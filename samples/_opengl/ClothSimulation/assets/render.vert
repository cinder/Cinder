#version 330 core

layout (location = 0) in vec3 position;	// POSITION_INDEX
layout (location = 1) in vec3 color;	// VELOCITY_INDEX

uniform mat4 ciModelViewProjection;

out vec3 oColor;

void main(void)
{
	gl_Position = ciModelViewProjection * vec4(position, 1.0);
	oColor = color;
}
