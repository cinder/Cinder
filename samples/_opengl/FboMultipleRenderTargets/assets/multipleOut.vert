#version 330 core

uniform mat4  ciModelViewProjection;
uniform mat3	ciNormalMatrix;

in vec4			ciPosition;
in vec3			ciNormal;
in vec2			ciTexCoord0;
in vec2			ciTexCoord1;

out vec2		vTexCoord0;
out vec2		vTexCoord1;
out vec3		vNormal;

void main()
{
	vTexCoord0	= ciTexCoord0;
	vTexCoord1	= ciTexCoord1;
	vNormal		= ciNormalMatrix * ciNormal;
	gl_Position	= ciModelViewProjection * ciPosition;
}
