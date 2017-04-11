#version 400

#include "commonSimple.glsl"

in vec2			vTexCoord0;
out vec4		oFragColor;

void main( void )
{
	float x = 0.4a;
	oFragColor = vec4( 0, x, 0, 1 );
}
