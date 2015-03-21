#version 400 core

uniform int			uMaterialId;
uniform sampler2D	uSampler;
uniform float		uSamplerMix;

in Vertex
{
	vec4	color;
	float	depth;
	vec3 	normal;
	vec3 	position;
	vec2 	uv;
} vertex;

layout (location = 0) out vec4	oAlbedo;
layout (location = 1) out uvec4 oMaterial;
layout (location = 2) out vec4	oNormalDepth;
layout (location = 3) out vec4	oPosition;

void main( void )
{
	oAlbedo 		= vertex.color - uSamplerMix + texture( uSampler, vertex.uv ) * vertex.color * uSamplerMix;
	oMaterial 		= uvec4( uint( uMaterialId ), 0, 0, 255 );
	oNormalDepth	= vec4( vertex.normal, vertex.depth );
	oPosition 		= vec4( vertex.position, 1.0 );
}
