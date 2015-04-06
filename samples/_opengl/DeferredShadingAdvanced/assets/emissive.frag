#version 330 core

#include "material.glsl"

uniform sampler2D	uSamplerAlbedo;
uniform isampler2D	uSamplerMaterial;

in Vertex
{
	vec2 	uv;
} vertex;

out vec4 	oColor;

void main( void )
{
	int id		= int( texture( uSamplerMaterial, vertex.uv ).r );
	oColor		= texture( uSamplerAlbedo, vertex.uv );
	oColor.a	= 0.5;
	oColor		*= uMaterials[ id ].emissive;
}
