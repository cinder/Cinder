#version 330 core

const int MODE_ALBEDO		= 0;
const int MODE_NORMAL		= 1;
const int MODE_POSITION		= 2;
const int MODE_DEPTH		= 3;

uniform sampler2D 	uSamplerAlbedo;
uniform sampler2D 	uSamplerDepth;
uniform sampler2D	uSamplerNormal;

uniform mat4        uProjMatrixInverse;
uniform vec2        uProjectionParams;

uniform int uMode = 0;

in Vertex
{
	vec2 	uv;
} vertex;

out vec4 oColor;

vec3 decodeNormal( vec2 enc )
{
    vec4 n	= vec4( enc.xy, 0.0, 0.0 ) * vec4( 2.0, 2.0, 0.0, 0.0 ) + vec4( -1.0, -1.0, 1.0, -1.0 );
    float l = dot( n.xyz, -n.xyw );
    n.z		= l;
    n.xy	*= sqrt( l );
    return n.xyz * 2.0 + vec3( 0.0, 0.0, -1.0 );
}

vec4 getPosition( vec2 uv )
{
    float depth			= texture( uSamplerDepth, uv ).x;
    float linearDepth 	= uProjectionParams.y / ( depth - uProjectionParams.x );
    vec4 posProj		= vec4( ( uv.x - 0.5 ) * 2.0, ( uv.y - 0.5 ) * 2.0, 0.0, 1.0 );
    vec4 viewRay		= uProjMatrixInverse * posProj;
    return vec4( viewRay.xyz * linearDepth, 1.0 );
}

void main( void )
{
	vec4 color 	= vec4( 1.0 );
	switch ( uMode ) {
	case MODE_ALBEDO:
		color 	= texture( uSamplerAlbedo, vertex.uv );
		break;
	case MODE_NORMAL:
		color 	= vec4( decodeNormal( texture( uSamplerNormal, vertex.uv ).rg ), 1.0 );
		break;
	case MODE_POSITION:
		color 	= getPosition( vertex.uv );
		break;
	case MODE_DEPTH:
		color 	= vec4( vec3( 1.0 - texture( uSamplerDepth, vertex.uv ).r ), 1.0 );
		break;
	}
	oColor 		= color;
}
 