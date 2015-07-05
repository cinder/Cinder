#include "../common/vertex_in.glsl"

const float	kAttenuation	= 1.05;
const float	kExposure		= 1.8;
const float	kGamma			= 1.0;
const float	kTheta			= 0.45;

uniform vec2		uPixel;

uniform sampler2D	uSamplerBloom;
uniform sampler2D	uSamplerColor;

layout (location = 0) out vec4 oColor;

void main( void )
{
	vec2 o		= uPixel * 0.5;
	vec4 sum	= vec4( 0.0 );
	sum			+= texture( uSamplerBloom, vertex.uv + vec2( -o.x,  o.y ) );
	sum			+= texture( uSamplerBloom, vertex.uv + vec2( -o.x, -o.y ) );
	sum			+= texture( uSamplerBloom, vertex.uv + vec2(  o.x,  o.y ) );
	sum			+= texture( uSamplerBloom, vertex.uv + vec2(  o.x, -o.y ) );
	vec4 bloom	= sum * 0.25 * kAttenuation;

	vec4 color	= texture( uSamplerColor, vertex.uv );
	oColor		= color + kTheta * ( bloom - color );
	oColor		*= kExposure;
	oColor		= pow( oColor, vec4( kGamma ) );
}
