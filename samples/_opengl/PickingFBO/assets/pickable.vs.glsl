#version 150

uniform mat4	ciModelViewProjection;

in vec4		ciPosition;
in vec4		ciColor;
in vec4		pickingColor;

out vec4	Color;
out vec4	PickingColor;

void main( void )
{
	gl_Position  = ciModelViewProjection * ciPosition;
	Color 		 = ciColor;
	PickingColor = pickingColor;
}
