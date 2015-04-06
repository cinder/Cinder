#version 330 core

out float oColor;

void main( void )
{
	oColor = gl_FragCoord.z;
}
 