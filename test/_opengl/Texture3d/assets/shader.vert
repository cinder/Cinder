#version 150

uniform mat4	ciModelViewProjection;

in vec4		ciPosition;
in vec2		ciTexCoord0;

out vec2	vTexCoord;

void main( void )
{
	vTexCoord = ciTexCoord0;
	gl_Position	= ciModelViewProjection * ciPosition;
}
