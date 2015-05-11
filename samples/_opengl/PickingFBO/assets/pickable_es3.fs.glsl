#version 300 es

in highp vec4		Color;
in highp vec4		PickingColor;

layout(location = 0 ) out highp vec4 	oColor;
layout(location = 1 ) out highp vec4 	oPickingColor;

void main( void )
{
	oColor = Color;
	oPickingColor = PickingColor;
}