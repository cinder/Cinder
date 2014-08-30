#version 150

uniform mat4	uModelViewProjection;
uniform mat3	uNormalMatrix;

in vec4		vPosition;
in vec2		vTexCoord0;
in vec3		vNormal;
out highp vec2	TexCoord;
in vec4		vColor;
out lowp vec4	Color;
out highp vec3	Normal;

void main( void )
{
	gl_Position	= uModelViewProjection * vPosition;
	Color 		= vColor;
	TexCoord	= vTexCoord0;
	Normal		= uNormalMatrix * vNormal;
}
