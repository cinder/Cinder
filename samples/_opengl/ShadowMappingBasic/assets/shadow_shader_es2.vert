#version 100

uniform mat4 ciModelViewProjection;
uniform mat4 ciModelMatrix;
uniform mat3 ciNormalMatrix;
uniform mat4 ciModelView;
uniform mat4 uShadowMatrix;

attribute vec4	ciColor;
attribute vec4	ciPosition;
attribute vec3	ciNormal;
attribute vec2  ciTexCoord0;

varying vec4 vColor;
varying vec3 vNormal;
varying vec4 vPosition;
varying vec4 vShadowCoord;
varying vec2 vTexCoord0;

const mat4 biasMatrix = mat4( 0.5, 0.0, 0.0, 0.0,
							  0.0, 0.5, 0.0, 0.0,
							  0.0, 0.0, 0.5, 0.0,
							  0.5, 0.5, 0.5, 1.0 );

void main( void )
{
	vColor			= ciColor;
	vPosition		= ciModelView * ciPosition;
	vNormal			= normalize( ciNormalMatrix * ciNormal );
	vShadowCoord	= ( biasMatrix * uShadowMatrix * ciModelMatrix ) * ciPosition;
	vTexCoord0		= ciTexCoord0;
	gl_Position		= ciModelViewProjection * ciPosition;
}