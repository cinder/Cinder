#version 300 es

uniform mat4	ciModelViewProjection;

in vec4			ciPosition;
in vec4			ciColor;
out lowp vec4	Color;

void main( void )
{
	gl_Position	= ciModelViewProjection * ciPosition;
	gl_PointSize = 1.0;
	Color 		= ciColor;
}
