#include "vertex_out.glsl"

uniform mat4 	ciModelViewProjection;

in vec4 		ciPosition;

void main( void ) 
{
	vec4 v 		= sign( ciModelViewProjection * ciPosition );
	vertex.uv 	= ( v.xy + 1.0 ) * 0.5;

	gl_Position = v;
}
