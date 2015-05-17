#version 150

uniform mat4 ciModelViewProjection;

in vec4 ciPosition;
in vec2 ciTexCoord0;

out vec2 vertTexCoord0;

void main()
{
	// Simply pass the texture coordinate and the position.
	vertTexCoord0 = ciTexCoord0;
	gl_Position = ciModelViewProjection * ciPosition;
}