#version 330 core

uniform sampler2D	uSamplerLBuffer;
uniform sampler2D	uSamplerSsao;

in Vertex
{
	vec2 	uv;
} vertex;

out vec4 	oColor;

void main( void )
{
	vec4 color	= texture( uSamplerLBuffer, vertex.uv );
	color		-= vec4( vec3( 1.0 - texture( uSamplerSsao, vertex.uv ).r ), 0.0 );
	oColor		= color;
}
