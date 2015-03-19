#version 430 core

uniform sampler2D	uTextureLBuffer;
uniform sampler2D	uTextureSsao;

in Vertex
{
	vec2 	uv;
} vertex;

out vec4 	oColor;

void main( void )
{
	vec4 color	= texture( uTextureLBuffer, vertex.uv );
	color		-= vec4( vec3( 1.0 - texture( uTextureSsao, vertex.uv ).r ), 0.0 );
	oColor		= color;
}
