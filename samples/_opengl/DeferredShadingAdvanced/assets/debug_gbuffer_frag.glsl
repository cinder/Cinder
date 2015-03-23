#version 400 core

const int MODE_ALBEDO	= 0;
const int MODE_MATERIAL	= 1;
const int MODE_NORMAL	= 2;
const int MODE_POSITION	= 3;
const int MODE_DEPTH	= 4;

uniform sampler2D 	uSamplerAlbedo;
uniform isampler2D	uSamplerMaterial;
uniform sampler2D 	uSamplerNormal;
uniform sampler2D 	uSamplerPosition;

uniform int uMode = 0;

in Vertex
{
	vec2 	uv;
} vertex;

out vec4 oColor;

void main( void )
{
	vec4 color 	= vec4( 1.0 );
	switch ( uMode ) {
	case MODE_ALBEDO:
		color 	= texture( uSamplerAlbedo, vertex.uv );
		break;
	case MODE_MATERIAL:
		ivec4 m	= texture( uSamplerMaterial, vertex.uv );
		color 	= vec4( float( m.r ) / 4.0, 0.0, 0.0, 1.0 );
		break;
	case MODE_NORMAL:
		color 	= texture( uSamplerNormal, vertex.uv );
		break;
	case MODE_POSITION:
		color 	= texture( uSamplerPosition, vertex.uv );
		break;
	case MODE_DEPTH:
		color 	= vec4( vec3( 1.0 - texture( uSamplerNormal, vertex.uv ).a ), 1.0 );
		break;
	}
	oColor 		= color;
}
