#version 300 es

uniform mat4	ciModelViewProjection;

in highp vec4		ciPosition;
in highp vec4		ciColor;

out highp vec4	Color;
out highp vec4	PickingColor;

void main( void )
{
	gl_Position  = ciModelViewProjection * ciPosition;
	gl_PointSize = 6.0;
	Color 		 = ciColor;
	PickingColor = vec4( 0.0, 0.0, 0.0, 0.0 );
}
