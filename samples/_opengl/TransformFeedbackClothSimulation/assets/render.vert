#version 150 core
#extension all : warn

in vec3 position;

void main(void)
{
	gl_Position = vec4(position * 0.03, 1.0);
}
