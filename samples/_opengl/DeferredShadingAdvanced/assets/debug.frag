#include "material.glsl"
#include "vertex_in.glsl"
#include "decode.glsl"

const int MODE_ALBEDO		= 0;
const int MODE_NORMAL		= 1;
const int MODE_POSITION		= 2;
const int MODE_DEPTH		= 3;
const int MODE_AMBIENT		= 4;
const int MODE_DIFFUSE		= 5;
const int MODE_EMISSIVE		= 6;
const int MODE_SPECULAR		= 7;
const int MODE_SHININESS	= 8;
const int MODE_MATERIAL_ID	= 9;

uniform sampler2D 	uSamplerAlbedo;
uniform sampler2D	uSamplerNormal;

uniform int uMode = 0;

out vec3 oColor;

int getId()
{
	return int( texture( uSamplerMaterial, vertex.uv ).r );
}

void main( void )
{
	vec3 color 	= vec3( 1.0 );
	switch ( uMode ) {
	case MODE_ALBEDO:
		color 	= texture( uSamplerAlbedo, vertex.uv ).rgb;
		break;
	case MODE_NORMAL:
		color 	= decodeNormal( texture( uSamplerNormal, vertex.uv ).rg );
		break;
	case MODE_POSITION:
		color 	= decodePosition( vertex.uv ).xyz;
		break;
	case MODE_DEPTH:
		color 	= vec3( 1.0 - texture( uSamplerDepth, vertex.uv ).r );
		break;
	case MODE_AMBIENT:
		color	= uMaterials[ getId() ].ambient.rgb;
		break;
	case MODE_DIFFUSE:
		color	= uMaterials[ getId() ].diffuse.rgb;
		break;
	case MODE_EMISSIVE:
		color	= uMaterials[ getId() ].emissive.rgb;
		break;
	case MODE_SPECULAR:
		color	= uMaterials[ getId() ].specular.rgb;
		break;
	case MODE_SHININESS:
		color	= vec3( uMaterials[ getId() ].shininess ) / 128.0;
		break;
	case MODE_MATERIAL_ID:
		color	= vec3( texture( uSamplerMaterial, vertex.uv ).r, 0.0, 0.0 );
		break;
	}
	oColor 		= color;
}
 