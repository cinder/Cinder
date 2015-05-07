#include "../common/vertex_in.glsl"
#include "../common/unpack.glsl"

const float kBias = 0.0001;

uniform sampler2D uSamplerLightDepth;

layout (location = 0) out vec4 oColor;

void main( void )
{
	float depth			= texture( uSamplerDepth, vertex.uv ).x;
	float lightDepth	= texture( uSamplerLightDepth, vertex.uv ).x;

	if ( depth > lightDepth - kBias ) {
		discard;
	}

	oColor = vec4( vec3( 0.0 ), 1.0 );
}
 