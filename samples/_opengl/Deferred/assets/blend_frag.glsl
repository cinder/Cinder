#version 430 core

uniform float		uBlend;
uniform sampler2D	uTexture0;
uniform sampler2D	uTexture1;

in Vertex
{
	vec2 	uv;
} vertex;

out vec4 	oColor;

void main( void )
{
	vec4 color0	= texture( uTexture0, vertex.uv ) * vec4( 1.0 - uBlend );
	vec4 color1	= texture( uTexture1, vertex.uv ) * vec4( uBlend );
	oColor		= color0 + color1;
}
