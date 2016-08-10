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

varying lowp vec4 vColor;
varying lowp vec3 vNormal;
varying lowp vec4 vPosition;
varying highp vec4 vShadowCoord;

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
	gl_Position		= ciModelViewProjection * ciPosition;
}