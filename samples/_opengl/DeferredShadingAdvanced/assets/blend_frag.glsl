#version 330 core

uniform float		uBlend;
uniform sampler2D	uSampler0;
uniform sampler2D	uSampler1;

in Vertex
{
	vec2 	uv;
} vertex;

out vec4 	oColor;

void main( void )
{
	vec4 color0	= texture( uSampler0, vertex.uv ) * vec4( 1.0 - uBlend );
	vec4 color1	= texture( uSampler1, vertex.uv ) * vec4( uBlend );
	oColor		= color0 + color1;
}
