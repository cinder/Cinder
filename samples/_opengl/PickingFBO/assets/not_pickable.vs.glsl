#version 150

uniform mat4	ciModelViewProjection;

in vec4		ciPosition;
in vec4		ciColor;

out vec4	Color;
out vec4	PickingColor;

void main( void )
{
	gl_Position  = ciModelViewProjection * ciPosition;
	Color 		 = ciColor;
	PickingColor = vec4( 0.0, 0.0, 0.0, 0.0 );
}
