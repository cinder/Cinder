#version 150

uniform mat4	ciModelViewProjection;

in vec4         ciPosition;
in vec2         ciTexCoord0;

out vec2        vertTexCoord0;

void main()
{
	vertTexCoord0 = ciTexCoord0;
	
	// vertex shader must always pass projection space position
	gl_Position = ciModelViewProjection * ciPosition;
}
