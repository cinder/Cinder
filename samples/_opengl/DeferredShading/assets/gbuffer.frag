#include "precision.glsl"

uniform float uEmissive;

in Vertex
{
	vec4	color;
	vec3 	normal;
	vec4 	position;
} vertex;

layout (location = 0) out vec4 oAlbedo;
layout (location = 1) out vec4 oNormalEmissive;
layout (location = 2) out vec4 oPosition;

void main( void )
{
	oAlbedo 		= vertex.color;
	oNormalEmissive	= vec4( normalize( vertex.normal ), uEmissive );
	oPosition 		= vertex.position;
}
