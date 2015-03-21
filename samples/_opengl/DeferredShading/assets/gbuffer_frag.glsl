#version 400 core

uniform float uEmissive;

in Vertex
{
	vec4	color;
	float	depth;
	vec3 	normal;
	vec3 	position;
	vec2 	uv;
} vertex;

layout (location = 0) out vec4	oAlbedo;
layout (location = 1) out vec4	oNormalDepth;
layout (location = 2) out vec4	oPosition;

void main( void )
{
	oAlbedo 		= vertex.color;
	oNormalDepth	= vec4( vertex.normal, vertex.depth );
	oPosition 		= vec4( vertex.position, uEmissive );
}
