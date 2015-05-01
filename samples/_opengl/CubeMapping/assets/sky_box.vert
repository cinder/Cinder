#version 150

uniform mat4	ciModelViewProjection;

in vec4			ciPosition;

out highp vec3	NormalWorldSpace;

void main( void )
{
	NormalWorldSpace = vec3( ciPosition );
	gl_Position = ciModelViewProjection * ciPosition;
}
