#version 150 core

uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;
uniform mat4	ciModelView;
uniform mat4	ciViewMatrix;
uniform mat4	ciViewMatrixInverse;

in vec4		ciPosition;
in vec3		ciNormal;
in vec2		ciTexCoord0;

out highp vec3	NormalWorldSpace;
out highp vec3  EyeDirWorldSpace;

out highp vec4	vPosition;
out highp vec3	vNormal;

void main( void )
{
	vec4 positionViewSpace	= ciModelView * ciPosition;
	vec4 eyeDirViewSpace	= positionViewSpace - vec4( 0, 0, 0, 1 ); // eye is always at 0,0,0 in view space
	EyeDirWorldSpace		= vec3( ciViewMatrixInverse * eyeDirViewSpace );
	vec3 normalViewSpace	= ciNormalMatrix * ciNormal;
	NormalWorldSpace		= normalize( vec3( vec4( normalViewSpace, 0 ) * ciViewMatrix ) );

	vPosition		= ciModelView * ciPosition;
	vNormal			= normalize( ciNormalMatrix * ciNormal );
	gl_Position		= ciModelViewProjection * ciPosition;
}