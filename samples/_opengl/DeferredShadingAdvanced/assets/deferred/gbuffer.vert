#if defined( INSTANCED_LIGHT_SOURCE )
#include "../common/light.glsl"
#endif

uniform mat4	ciModelViewProjection;
#if !defined( INSTANCED_MODEL )
uniform mat3	ciNormalMatrix;
#endif

in vec4 	ciPosition;
in vec3 	ciNormal;
in vec4 	ciColor;
#if defined( INSTANCED_MODEL )
in mat3		vInstanceNormalMatrix;
in mat4		vInstanceModelMatrix;
#endif

out Vertex
{
	vec3 color;
	vec3 normal;
} vertex;

void main( void )
{
	vertex.color		= ciColor.rgb;

#if defined( INSTANCED_MODEL )
	mat3 normalMatrix	= vInstanceNormalMatrix;
#else
	mat3 normalMatrix	= ciNormalMatrix;
#endif
	vec3 n				= ciNormal;
#if defined( INVERT_NORMAL )
	n					= -n;
#endif
	vertex.normal 		= normalMatrix * n;

	vec4 p				= ciPosition;
#if defined( INSTANCED_LIGHT_SOURCE )
	Light light			= uLights[ gl_InstanceID ];
	p.xyz				*= light.radius;
	p.xyz				+= light.position;
	p.w					= 1.0;
	vertex.color		*= light.diffuse.rgb;
#endif
	
#if defined( INSTANCED_MODEL )
	p					= vInstanceModelMatrix * p;
#endif
	gl_Position			= ciModelViewProjection * p;
}
 