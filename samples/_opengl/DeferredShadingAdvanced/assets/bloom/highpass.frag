#include "../common/vertex_in.glsl"

const float	kExposure	= 6.0;
const float kLuminosity	= 0.333;
const float	kThreshold	= 0.05;
const float	kEdge		= 0.55;

uniform sampler2D	uSampler;

layout (location = 0) out vec4 oColor;

void main( void )
{
	oColor		= texture( uSampler, vertex.uv );
	oColor.rgb	*= pow( smoothstep( kThreshold, kThreshold + kEdge, dot( oColor.rgb, vec3( kLuminosity ) ) ), kExposure );
}
