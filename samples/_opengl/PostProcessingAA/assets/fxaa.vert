#version 130

uniform mat4	ciModelViewProjection;

in vec4			ciPosition;
in vec4			ciColor;
in vec2			ciTexCoord0;

out vec4		vColor;
out vec2		vTexCoord0;

void main()
{
	vColor = ciColor;
	vTexCoord0 = ciTexCoord0;
	
	gl_Position = ciModelViewProjection * ciPosition;
}
