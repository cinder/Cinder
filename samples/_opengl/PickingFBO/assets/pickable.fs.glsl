#version 150

in vec4		Color;
in vec4		PickingColor;

out vec4 	oColor;
out vec4 	oPickingColor;

void main( void )
{
	oColor = Color;
	oPickingColor = PickingColor;
}