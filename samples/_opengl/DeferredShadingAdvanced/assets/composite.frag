#version 330 core

uniform sampler2D uSamplerLBuffer;
uniform sampler2D uSamplerSsao;

in Vertex
{
	vec2 	uv;
} vertex;

out vec3 	oColor;

void main( void )
{
	vec3 color	= texture( uSamplerLBuffer, vertex.uv ).rgb;
	color		-= vec3( 1.0 - texture( uSamplerSsao, vertex.uv ).r );
	oColor		= color;
}
