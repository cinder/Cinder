#include "material.glsl"
#include "vertex_in.glsl"

uniform sampler2D	uSamplerAlbedo;

out vec4 	oColor;

void main( void )
{
	int id		= int( texture( uSamplerMaterial, vertex.uv ).r );
	oColor		= texture( uSamplerAlbedo, vertex.uv );
	oColor.a	= 0.5;
	oColor		*= uMaterials[ id ].emissive;
}
