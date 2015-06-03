#version 300 es
uniform mat4	ciModelViewProjection;

in vec4		ciPosition;
in vec4		ciColor;
in vec4		pickingColor;

out highp vec4 	Color;
out highp vec4 	PickingColor;

void main( void )
{
	gl_Position  = ciModelViewProjection * ciPosition;
	gl_PointSize  = 6.0;
	Color 		 = ciColor;
	PickingColor = pickingColor;
}
