#include "../common/vertex_in.glsl"
#include "../common/offset.glsl"

uniform sampler2D uSampler;

const vec3	kColorNear	= vec3( 1.0, 0.95, 0.9 );
const vec3	kColorFar	= vec3( 1.0, 0.8, 0.95 );
const float	kDensity 	= 0.3;
const float	kFalloff 	= 8.0;

layout (location = 0) out vec4 oColor;

void main( void )
{
	vec2 uv		= calcTexCoordFromUv( vertex.uv );
	float depth	= texture( uSampler, uv ).x;
	vec3 color	= mix( kColorNear, kColorFar, depth );
	
	oColor		= vec4( color, depth < 1.0 ? clamp( pow( depth, kFalloff ) * kDensity, 0.0, 1.0 ) : 0.0 );
}
 