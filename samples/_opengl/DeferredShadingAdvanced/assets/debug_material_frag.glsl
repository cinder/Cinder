#version 330 core

const int MATERIAL_COUNT	= 3;

const int MODE_AMBIENT		= 0;
const int MODE_DIFFUSE		= 1;
const int MODE_EMISSIVE		= 2;
const int MODE_SPECULAR		= 3;
const int MODE_SHININESS	= 4;
const int MODE_MATERIAL_ID	= 5;

struct Material
{
	vec4	ambient;
	vec4	diffuse;
	vec4	emissive;
	vec4	specular;
	float	shininess;
	vec3	pad;
};

layout (std140) uniform Materials
{
	Material uMaterials[ MATERIAL_COUNT ];
};

uniform usampler2D uSampler;

uniform int uMode;

in Vertex
{
	vec2 uv;
} vertex;

out vec4 oColor;

void main( void )
{
	vec4 color			= vec4( 0.0 );
	int id				= int( texture( uSampler, vertex.uv ).r );
	Material material	= uMaterials[ id ];

	switch ( uMode ) {
	case MODE_AMBIENT:
		color = material.ambient;
		break;
	case MODE_DIFFUSE:
		color = material.diffuse;
		break;
	case MODE_EMISSIVE:
		color = material.emissive;
		break;
	case MODE_SPECULAR:
		color = material.specular;
		break;
	case MODE_SHININESS:
		color = vec4( vec3( material.shininess ) / 128.0, 1.0 );
		break;
	case MODE_MATERIAL_ID:
		color = vec4( texture( uSampler, vertex.uv ).r, 0.0, 0.0, 1.0 );
		break;
	}

	oColor = color;
}
