#version 150

uniform mat4	ciModelViewProjection;

in vec4			ciPosition;

void main() {
	// pass screen space position
	gl_Position = ciModelViewProjection * ciPosition;
}