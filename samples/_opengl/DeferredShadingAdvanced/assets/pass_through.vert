#include "vertex_out.glsl"

uniform mat4 ciModelViewProjection;

in vec4 	ciPosition;
in vec2 	ciTexCoord0;

void main( void ) 
{
	vertex.uv 	= ciTexCoord0;
	gl_Position = ciModelViewProjection * ciPosition;
}
