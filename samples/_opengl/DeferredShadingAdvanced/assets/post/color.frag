#include "../common/vertex_in.glsl"

const float	kBlend					= 0.8;
const float	kBrightness				= 0.45;
const float	kChromaticAberration	= 0.0015;
const float	kContrast				= 0.2;
const float	kExposure				= 1.5;
const vec3	kSaturation				= vec3( 1.15, 1.0, 0.953 );

uniform sampler2D uSampler;

layout (location = 0) out vec4 oColor;

void main( void )
{
	vec3 color	= vec3( 0.0 );
	if ( kChromaticAberration != 0.0 ) {
		color.r = texture( uSampler, vec2( vertex.uv.x + kChromaticAberration,	vertex.uv.y ) ).r;
		color.g = texture( uSampler, vec2( vertex.uv.x + 0.0,					vertex.uv.y ) ).g;
		color.b	= texture( uSampler, vec2( vertex.uv.x - kChromaticAberration,	vertex.uv.y ) ).b;
	}
	color		*= kSaturation;
	color.rgb	+= vec3( kBrightness );
	color		= clamp( color * 0.5 + 0.5 * color * color * kContrast, 0.0, 1.0 );
	color		= pow( color, vec3( kExposure ) );
	if ( kBlend < 1.0 ) {
		color	= mix( texture( uSampler, vertex.uv ).rgb, color, kBlend );
	}
	oColor		= vec4( color, 1.0 );
}
