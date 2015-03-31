#version 330 core

const int MATERIAL_COUNT = 3;

uniform sampler2D	uSamplerAlbedo;
uniform isampler2D	uSamplerMaterial;

struct Material
{
	vec4	ambient;
	vec4	diffuse;
	vec4	emissive;
	vec4	specular;
	float	shininess;
	uint	pad0;
	uint	pad1;
	uint	pad2;
};

layout (std140) uniform Materials
{
	Material uMaterials[ MATERIAL_COUNT ];
};

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
