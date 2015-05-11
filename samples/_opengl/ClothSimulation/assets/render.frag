#version 330 core

layout (location = 0) out vec4 color;

in vec3 oColor;

void main(void)
{
	vec3 colorSpectrum = clamp( normalize( abs( oColor ) ), vec3( 0.1 ), vec3( 1.0 ) );
	color = vec4( 1.0 - colorSpectrum, 1.0 );
}
