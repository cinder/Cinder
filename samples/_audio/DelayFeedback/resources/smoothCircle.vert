#version 150

uniform mat4	ciModelViewProjection;

in vec4		ciPosition;
in vec4		ciColor;
in vec2		ciTexCoord0;

out vec4	ioColor;
out vec2	ioTexCoord;

void main()
{
	ioColor 	= ciColor;
	ioTexCoord	= ciTexCoord0;

	gl_Position	= ciModelViewProjection * ciPosition;
}

