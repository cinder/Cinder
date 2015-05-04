#include "precision.glsl"

const int MODE_ALBEDO	= 0;
const int MODE_NORMAL	= 1;
const int MODE_POSITION	= 2;
const int MODE_EMISSIVE	= 3;

uniform sampler2D uSamplerAlbedo;
uniform sampler2D uSamplerNormalEmissive;
uniform sampler2D uSamplerPosition;

uniform int uMode;

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
	case MODE_NORMAL:
		color 	= vec4( texture( uSamplerNormalEmissive, vertex.uv ).xyz, 1.0 );
		break;
	case MODE_POSITION:
		color 	= texture( uSamplerPosition, vertex.uv );
		break;
	case MODE_EMISSIVE:
		color 	= vec4( vec3( texture( uSamplerNormalEmissive, vertex.uv ).a ), 1.0 );
		break;
	}
	oColor 		= color;
}
