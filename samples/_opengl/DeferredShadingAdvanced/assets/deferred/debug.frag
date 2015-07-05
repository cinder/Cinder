#include "../common/vertex_in.glsl"
#include "../common/material.glsl"
#include "../common/unpack.glsl"

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
const int MODE_ACCUM		= 10;
const int MODE_AO			= 11;
const int MODE_RAY			= 12;

uniform sampler2D uSamplerAccum;
uniform sampler2D uSamplerAlbedo;
uniform sampler2D uSamplerAo;
uniform sampler2D uSamplerNormal;
uniform sampler2D uSamplerRay;

uniform float	uFar;
uniform int		uMode;

layout (location = 0) out vec4 oColor;

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
		color 	= unpackNormal( texture( uSamplerNormal, vertex.uv ).rg );
		break;
	case MODE_POSITION:
		color 	= unpackPosition( vertex.uv ).xyz;
		break;
	case MODE_DEPTH:
		color 	= vec3( pow( texture( uSamplerDepth, vertex.uv ).r, uFar ) );
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
		color	= vec3( float( texture( uSamplerMaterial, vertex.uv ).r ) / float( NUM_MATERIALS ), 0.0, 0.0 );
		break;
	case MODE_ACCUM:
		color 	= texture( uSamplerAccum, vertex.uv ).rgb;
		break;
	case MODE_AO:
		color 	= texture( uSamplerAo, vertex.uv ).rrr;
		break;
	case MODE_RAY:
		color 	= texture( uSamplerRay, vertex.uv ).rgb;
		break;
	}
	oColor 		= vec4( color, 1.0 );
}
 