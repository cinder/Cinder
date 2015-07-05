#include "../common/vertex_in.glsl"
#include "../common/offset.glsl"

uniform sampler2D uSamplerDepth;
uniform sampler2D uSamplerColor;

const vec3	kColorNear	= vec3( 1.0, 0.95, 0.9 );
const vec3	kColorFar	= vec3( 1.0, 0.8, 0.95 );
const float	kDensity 	= 0.333;
const float	kFalloff 	= 2000.0;

layout (location = 0) out vec4 oColor;

void main( void )
{
	vec2 uv		= calcTexCoordFromUv( vertex.uv );
	float depth	= texture( uSamplerDepth, uv ).x;
	vec3 color	= mix( kColorNear, kColorFar, depth );
	color		*= clamp( pow( depth, kFalloff ) * kDensity, 0.0, 1.0 );
	oColor.rgb	= texture( uSamplerColor, vertex.uv ).rgb + ( depth < 1.0 ? color : vec3( 0.0 ) );
}
 