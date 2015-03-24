#version 400 core

uniform int			uMaterialId;
uniform sampler2D	uSampler;
uniform float		uSamplerMix;

in Vertex
{
	vec4	color;
	vec3 	normal;
	vec2 	uv;
} vertex;

layout (location = 0) out vec4	oAlbedo;
layout (location = 1) out uvec4 oMaterial;
layout (location = 2) out vec3	oNormal;

void main( void )
{
	oAlbedo 	= vertex.color - uSamplerMix + texture( uSampler, vertex.uv ) * vertex.color * uSamplerMix;
	oMaterial 	= uvec4( uint( uMaterialId ), 0, 0, 255 );
	oNormal		= vertex.normal;
}