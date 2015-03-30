#version 330 core

out vec4 oColor;

void main( void )
{
	oColor = vec4( gl_FragCoord.zzz, 1.0 );
}
 