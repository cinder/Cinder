uniform mat4	ciModelViewProjection;

in vec4			ciPosition;

#if defined( TEX_COORD )
#include "common/vertex_out.glsl"
in vec2			ciTexCoord0;
#endif

#if defined( INSTANCED_MODEL )
in mat4			vInstanceModelMatrix;
#endif

void main( void )
{
#if defined( TEX_COORD )
	vertex.uv 	= ciTexCoord0;
#endif
	mat4 m		= ciModelViewProjection;
	vec4 p		= ciPosition;
#if defined( INSTANCED_MODEL )
	m			= m * vInstanceModelMatrix;
#endif
	gl_Position = m * p;
}
