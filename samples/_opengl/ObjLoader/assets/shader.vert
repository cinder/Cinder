#version 150

uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;

in vec4		ciPosition;
in vec2		ciTexCoord0;
in vec3		ciNormal;

out lowp vec4	Color;
out highp vec3	Normal;
out highp vec2	TexCoord0;

void main( void )
{
	gl_Position	= ciModelViewProjection * ciPosition;
	TexCoord0	= ciTexCoord0;
	Normal		= ciNormalMatrix * ciNormal;
}
