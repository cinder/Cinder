	
	#include "common.glsl"

uniform float ciElapsedSeconds;

in vec2			vTexCoord0;
out vec4		oFragColor;

void main( void )
{
	float t = ciElapsedSeconds * 0.4;
	float gray = noise( vTexCoord0 * 25.0 + t ) * 0.5;
	gray += noise( vTexCoord0 * 25.0 - t ) * 0.5;
	oFragColor = vec4( gray );
}