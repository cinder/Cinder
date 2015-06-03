#include "../common/light.glsl"

uniform mat4	ciModelViewProjection;

in vec4			ciPosition;

flat out int	vInstanceId;

void main( void ) 
{
	vInstanceId	= gl_InstanceID;
	Light light = uLights[ vInstanceId ];
	
	vec3 p		= ciPosition.xyz * light.volume + light.position;
	
	gl_Position = ciModelViewProjection * vec4( p, 1.0 );
}
