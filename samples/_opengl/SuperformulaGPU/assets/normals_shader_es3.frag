#version 300 es

uniform sampler2D uTex0;

in highp vec4	Color;
out highp vec4 	oColor;

void main( void )
{
	oColor = Color;
}