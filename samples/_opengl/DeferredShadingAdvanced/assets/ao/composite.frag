#include "../common/vertex_in.glsl"
#include "../common/offset.glsl"

uniform sampler2D uSampler;
uniform sampler2D uSamplerAo;

layout (location = 0) out vec4 oColor;

void main( void )
{
	vec3 color	= texture( uSampler, vertex.uv ).rgb;
	color		-= 1.0 - texture( uSamplerAo, calcTexCoordFromUv( vertex.uv ) ).r;
	oColor		= vec4( color, 1.0 );
}
