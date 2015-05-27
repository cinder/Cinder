#version 150 core

uniform mat4 ciModelViewProjection;
uniform mat4 ciModelMatrix;
uniform mat3 ciNormalMatrix;
uniform mat4 ciModelView;
uniform mat4 uShadowMatrix;

in vec4	ciColor;
in vec4	ciPosition;
in vec3	ciNormal;

out vec4 vColor;
out vec3 vNormal;
out vec4 vPosition;
out vec4 vShadowCoord;

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