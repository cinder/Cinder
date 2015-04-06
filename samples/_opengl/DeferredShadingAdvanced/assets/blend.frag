#include "vertex_in.glsl"

uniform float		uBlend;
uniform sampler2D	uSampler0;
uniform sampler2D	uSampler1;

out vec3 	oColor;

void main( void )
{
	vec3 color0	= texture( uSampler0, vertex.uv ).rgb * vec3( 1.0 - uBlend );
	vec3 color1	= texture( uSampler1, vertex.uv ).rgb * vec3( uBlend );
	oColor		= color0 + color1;
}
