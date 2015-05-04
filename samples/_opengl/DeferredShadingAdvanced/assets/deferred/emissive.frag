#include "../common/vertex_in.glsl"
#include "../common/material.glsl"
#include "../common/offset.glsl"

uniform sampler2D uSamplerAlbedo;

layout (location = 0) out vec4 oColor;

void main( void )
{
	vec2 uv		= calcTexCoordFromUv( vertex.uv );
	int id		= int( texture( uSamplerMaterial, uv ).r );
	oColor		= texture( uSamplerAlbedo, uv );
	oColor.a	= 0.5;
	oColor		*= uMaterials[ id ].emissive;
}
