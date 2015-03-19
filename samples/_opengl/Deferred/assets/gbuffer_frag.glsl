#version 430 core

layout (binding = 0) uniform sampler2D	uTexture;

uniform int			uMaterialId;
uniform float		uTextureMix;

in Vertex
{
	vec4	color;
	float	depth;
	vec3 	normal;
	vec3 	position;
	vec2 	uv;
} vertex;

layout (location = 0) out vec4 oAlbedo;
layout (location = 1) out uvec4 oMaterial;
layout (location = 2) out vec4 oNormalDepth;
layout (location = 3) out vec4 oPosition;

void main( void )
{
	oAlbedo 		= vertex.color - uTextureMix + texture2D( uTexture, vertex.uv ) * vertex.color * uTextureMix;
	oMaterial 		= uvec4( uint( uMaterialId ), 0, 0, 255 );
	oNormalDepth	= vec4( vertex.normal, vertex.depth );
	oPosition 		= vec4( vertex.position, 1.0 );
}
