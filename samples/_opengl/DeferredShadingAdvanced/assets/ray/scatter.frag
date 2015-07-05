#include "../common/vertex_in.glsl"
#include "../common/offset.glsl"

// Volumetric light scattering: http://http.developer.nvidia.com/GPUGems3/gpugems3_ch13.html

const float	kDecay		= 1.0;
const float	kDensity	= 1.0;
const float	kExposure	= 0.002;
const int	kNumSamples	= 100;
const float kWeight		= 5.65;

uniform vec2		uLightPosition;
uniform sampler2D	uSampler;

layout (location = 0) out vec4 oColor;

void main( void )
{
	oColor		= vec4( 0.0 );

	vec2 uv		= calcTexCoordFromUv( vertex.uv );
	vec2 d		= vec2( uv - uLightPosition );
	d			*= 1.0 / float( kNumSamples ) * kDensity;
	float decay	= 1.0;

    for ( int i = 0; i < kNumSamples; ++i ) {
		uv		-= d;
		oColor	+= texture( uSampler, uv ) * decay * kWeight;
		decay	*= kDecay;
	}
	oColor		*= kExposure;
}
 