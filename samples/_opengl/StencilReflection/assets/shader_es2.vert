#version 100

uniform mat4	uModelViewProjection;
uniform mat3	uNormalMatrix;

attribute vec4		vPosition;
attribute vec2		vTexCoord0;
attribute vec3		vNormal;
varying highp vec2	TexCoord;
attribute vec4		vColor;
varying lowp vec4	Color;
varying highp vec3	Normal;

void main( void )
{
	gl_Position	= uModelViewProjection * vPosition;
	Color 		= vColor;
	TexCoord	= vTexCoord0;
	Normal		= uNormalMatrix * vNormal;
}
