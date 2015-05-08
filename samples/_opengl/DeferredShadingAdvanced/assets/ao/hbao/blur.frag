#include "../../common/vertex_in.glsl"

const float kEdgeSharpness	= 2.0;
const vec4	kGaussian		= vec4( 0.121569, 0.219963, 0.147465, 0.071788 );
const float	kScale			= 0.002;

uniform vec2		uAxis;
uniform float		uNear;
uniform sampler2D	uSampler;

layout (location = 0) out vec4 oColor;

float calcWeight( vec2 offset, float c, float g, inout float t )
{
	vec4 v		= texture( uSampler, vertex.uv + offset );
	float z		= uNear / ( 1.0 - v.g );
	float w		= g * max( 0.01, 1.0 - kEdgeSharpness * abs( z - c ) );
	t			+= w;
	return v.r * w;
}

void main( void )
{
	oColor		= vec4( vec3( 0.0 ), 1.0 );
	float depth	= texture( uSampler, vertex.uv ).g;
	float z		= uNear / ( 1.0 - depth );
	float t		= 0.0;
	float r		= 0.0;
	r			+= calcWeight( uAxis * kScale * -5.3896,	z, kGaussian.w, t );
	r			+= calcWeight( uAxis * kScale * -3.42905,	z, kGaussian.z, t );
	r			+= calcWeight( uAxis * kScale * -1.46943,	z, kGaussian.y, t );
	r			+= calcWeight( uAxis * kScale *  0.0,		z, kGaussian.x, t );
	r			+= calcWeight( uAxis * kScale *  1.46943,	z, kGaussian.y, t );
	r			+= calcWeight( uAxis * kScale *  3.42905,	z, kGaussian.z, t );
	r			+= calcWeight( uAxis * kScale *  5.3896,	z, kGaussian.w, t );
	oColor.g	= depth;
	oColor.r	= r / t;
}
 