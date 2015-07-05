#version 100

uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;

attribute vec4		ciPosition;
attribute vec2		ciTexCoord0;
attribute vec3		ciNormal;
varying highp vec2	TexCoord;
attribute vec4		ciColor;
varying lowp vec4	Color;
varying highp vec3	Normal;

void main( void )
{
	gl_Position	= ciModelViewProjection * ciPosition;
	Color 		= ciColor;
	TexCoord	= ciTexCoord0;
	Normal		= ciNormalMatrix * ciNormal;
}
