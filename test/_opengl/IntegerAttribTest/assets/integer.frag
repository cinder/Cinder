#version 330 core

in vec3 oColor;

out vec4 Color;

void main() {
	Color = vec4( oColor, 1.0 );
}