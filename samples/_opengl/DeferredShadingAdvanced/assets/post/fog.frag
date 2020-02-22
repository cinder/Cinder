#include "../common/vertex_in.glsl"
#include "../common/offset.glsl"

uniform sampler2D uSamplerDepth;
uniform sampler2D uSamplerColor;

const vec3	kColorNear	= vec3( 1.0, 0.95, 0.9 );
const vec3	kColorFar	= vec3( 1.0, 0.8, 0.95 );
const float	kDensity 	= 0.125;
const float	kFalloff 	= 1.5;

layout (location = 0) out vec4 oColor;

void main( void )
{
	vec2 uv		= calcTexCoordFromUv( vertex.uv );
	float depth	= texture( uSamplerDepth, uv ).x;
	vec3 color	= mix( kColorNear, kColorFar, depth * 0.5 );
	color		*= clamp( pow( depth, kFalloff ) * kDensity, 0.0, 1.0 );
	oColor		= max( vec4( color, 1.0 ), texture( uSamplerColor, vertex.uv ) * ( depth > 1.0 ? vec4( color, 1.0 ) : vec4( 1.0 ) ) );
}
 