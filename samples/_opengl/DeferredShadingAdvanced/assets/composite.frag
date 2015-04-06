#include "vertex_in.glsl"

uniform sampler2D uSamplerLBuffer;
uniform sampler2D uSamplerSsao;

out vec3 	oColor;

void main( void )
{
	vec3 color	= texture( uSamplerLBuffer, vertex.uv ).rgb;
	color		-= vec3( 1.0 - texture( uSamplerSsao, vertex.uv ).r );
	oColor		= color;
}
