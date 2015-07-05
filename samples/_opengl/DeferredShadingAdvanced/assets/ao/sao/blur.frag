#include "../../common/vertex_in.glsl"

const float kBase			= 0.01;
const float kEdgeSharpness	= 1.5;
const float kEpsilon		= 0.001;
const int kScale			= 2;

uniform ivec2       uAxis;
uniform sampler2D   uSampler;

out vec4            oColor;

float calcWeight( in ivec2 ss, in float k, in float g, inout float t )
{
	vec4 v			= texelFetch( uSampler, ss, 0 ) ;
	float w			= ( kBase + g ) * max( kEpsilon, 1.0 - kEdgeSharpness * abs( v.g - k ) );
	t				+= w;
	return v.r * w;
}

void main( void )
{
	oColor			= vec4( vec3( 0.0 ), 1.0 );
	ivec2 ss		= ivec2( gl_FragCoord.xy );
	
	vec4 v			= texelFetch( uSampler, ss, 0 );

	float k			= v.g;
	float r			= v.r;
	
	oColor.g		= k;
	if ( k == 1.0 ) {
		oColor.r	= r;
		return;
	}
	
	ivec2 sz		= uAxis * kScale;
	float t			= kBase + 0.111220;
	r				*= t;
	r				+= calcWeight( ss + sz * -6, k, 0.036108, t );
	r				+= calcWeight( ss + sz * -5, k, 0.050920, t );
	r				+= calcWeight( ss + sz * -4, k, 0.067458, t );
	r				+= calcWeight( ss + sz * -3, k, 0.083953, t );
	r				+= calcWeight( ss + sz * -2, k, 0.098151, t );
	r				+= calcWeight( ss + sz * -1, k, 0.107798, t );
	r				+= calcWeight( ss + sz *  1, k, 0.107798, t );
	r				+= calcWeight( ss + sz *  2, k, 0.098151, t );
	r				+= calcWeight( ss + sz *  3, k, 0.083953, t );
	r				+= calcWeight( ss + sz *  4, k, 0.067458, t );
	r				+= calcWeight( ss + sz *  5, k, 0.050920, t );
	r				+= calcWeight( ss + sz *  6, k, 0.036108, t );
	
	oColor.r		= r / t;
}
