#version 100

uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;

attribute vec4		ciPosition;
attribute vec2		ciTexCoord0;
attribute vec3		ciNormal;
attribute vec4		ciColor;
attribute vec3		vInstancePosition; // per-instance position variable
varying highp vec2	TexCoord;
varying lowp vec4	Color;
varying highp vec3	Normal;

void main( void )
{
	gl_Position	= ciModelViewProjection * ( 4.0 * ciPosition + vec4( vInstancePosition, 0 ) );
	Color 		= ciColor;
	TexCoord	= ciTexCoord0;
	Normal		= ciNormalMatrix * ciNormal;
}
