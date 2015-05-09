#version 150

uniform sampler2D uTex0;

in vec4		Color;
out vec4 	oColor;

void main( void )
{
	oColor = Color;
}