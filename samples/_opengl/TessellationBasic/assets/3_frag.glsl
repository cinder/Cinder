#version 400

out vec4 oColor;

in vec4 TesColor;

void main ()
{
	oColor = TesColor;
}