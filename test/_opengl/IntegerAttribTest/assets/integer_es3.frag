#version 300 es

in mediump vec3 oColor;

out mediump vec4 Color;

void main() {
	Color = vec4( oColor, 1.0 );
}