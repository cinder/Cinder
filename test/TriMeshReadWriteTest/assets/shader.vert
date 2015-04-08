#version 150

uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;

in vec4		ciPosition;
in vec2		ciTexCoord0;
in vec3		ciNormal;
in vec4		ciColor;

out highp vec3	vNormal;
out highp vec2	vTexCoord0;

void main( void )
{
	gl_Position	= ciModelViewProjection * ciPosition;
	vNormal		= ciNormalMatrix * ciNormal;
	vTexCoord0	= ciTexCoord0;
}
