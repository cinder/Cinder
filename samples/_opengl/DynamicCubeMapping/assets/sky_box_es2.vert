#version 100

uniform mat4	ciModelViewProjection;

attribute vec4		ciPosition;

varying highp vec3	NormalWorldSpace;

void main( void )
{
	NormalWorldSpace = vec3( ciPosition );
	gl_Position = ciModelViewProjection * ciPosition;
}
