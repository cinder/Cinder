#version 330

in vec4		Color;
in vec4		PickingColor;

layout( location = 0 ) out vec4 	oColor;
layout( location = 1 ) out vec4 	oPickingColor;

void main( void )
{
	oColor = Color;
	oPickingColor = PickingColor;
}