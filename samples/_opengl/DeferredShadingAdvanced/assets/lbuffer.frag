#include "material.glsl"
#include "vertex_in.glsl"

const float kShadowBias		= 0.8655;
const float	kShadowBlurSize = 0.005;
const float kShadowOpacity	= 0.3;

uniform sampler2D		uSamplerAlbedo;
uniform sampler2D		uSamplerDepth;
uniform sampler2D		uSamplerNormal;
uniform sampler2DShadow uSamplerShadowMap;

uniform vec4	uLightColorAmbient;
uniform vec4	uLightColorDiffuse;
uniform vec4	uLightColorSpecular;
uniform float	uLightIntensity;
uniform vec3	uLightPosition;
uniform float	uLightRadius;

uniform bool	uShadowEnabled;
uniform mat4 	uShadowMatrix;

uniform vec2    uProjectionParams;
uniform mat4    uProjMatrixInverse;
uniform mat4 	uViewMatrixInverse;
uniform vec2	uWindowSize;

out vec4 	oColor;

float shadow( in vec4 position )
{
	vec4 shadowClip 	= uShadowMatrix * uViewMatrixInverse * position;
	vec3 shadowCoord 	= shadowClip.xyz / shadowClip.w;
	shadowCoord 		= shadowCoord * 0.5 + 0.5;

	float v 			= 0.0;
	float e				= 0.0625;
	if ( shadowCoord.x >= 0.0 && shadowCoord.x <= 1.0 && shadowCoord.y >= 0.0 && shadowCoord.y <= 1.0 ) {
		float d = shadowCoord.z - kShadowBias;
		if ( texture( uSamplerShadowMap, shadowCoord + vec3( vec2( -0.06095261, -0.1337204 )   * kShadowBlurSize, 0.0 ) ) > d ) {
			v += e;
		}
		if ( texture( uSamplerShadowMap, shadowCoord + vec3( vec2(  0.4983526,   0.233555 )    * kShadowBlurSize, 0.0 ) ) > d ) {
			v += e;
		}
		if ( texture( uSamplerShadowMap, shadowCoord + vec3( vec2( -0.2842098,  -0.5506849 )   * kShadowBlurSize, 0.0 ) ) > d ) {
			v += e;
		}
		if ( texture( uSamplerShadowMap, shadowCoord + vec3( vec2(  0.05801121,  0.6332615 )   * kShadowBlurSize, 0.0 ) ) > d ) {
			v += e;
		}
		if ( texture( uSamplerShadowMap, shadowCoord + vec3( vec2( -0.5088959,  -0.003537838 ) * kShadowBlurSize, 0.0 ) ) > d ) {
			v += e;
		}
		if ( texture( uSamplerShadowMap, shadowCoord + vec3( vec2(  0.4832182,  -0.2853011 )   * kShadowBlurSize, 0.0 ) ) > d ) {
			v += e;
		}
		if ( texture( uSamplerShadowMap, shadowCoord + vec3( vec2( -0.8192781,  -0.2787592 )   * kShadowBlurSize, 0.0 ) ) > d ) {
			v += e;
		}
		if ( texture( uSamplerShadowMap, shadowCoord + vec3( vec2(  0.1339615,  -0.6042675 )   * kShadowBlurSize, 0.0 ) ) > d ) {
			v += e;
		}
		if ( texture( uSamplerShadowMap, shadowCoord + vec3( vec2(  0.5493031,  -0.8009133 )   * kShadowBlurSize, 0.0 ) ) > d ) {
			v += e;
		}
		if ( texture( uSamplerShadowMap, shadowCoord + vec3( vec2(  0.9285686,   0.146349 )    * kShadowBlurSize, 0.0 ) ) > d ) {
			v += e;
		}
		if ( texture( uSamplerShadowMap, shadowCoord + vec3( vec2( -0.2837186,  -0.9508537 )   * kShadowBlurSize, 0.0 ) ) > d ) {
			v += e;
		}
		if ( texture( uSamplerShadowMap, shadowCoord + vec3( vec2(  0.5228189,   0.8005553 )   * kShadowBlurSize, 0.0 ) ) > d ) {
			v += e;
		}
		if ( texture( uSamplerShadowMap, shadowCoord + vec3( vec2( -0.4011278,   0.5258422 )   * kShadowBlurSize, 0.0 ) ) > d ) {
			v += e;
		}
		if ( texture( uSamplerShadowMap, shadowCoord + vec3( vec2( -0.2490727,   0.9233519 )   * kShadowBlurSize, 0.0 ) ) > d ) {
			v += e;
		}
		if ( texture( uSamplerShadowMap, shadowCoord + vec3( vec2( -0.8024328,   0.3718062 )   * kShadowBlurSize, 0.0 ) ) > d ) {
			v += e;
		}
		if ( texture( uSamplerShadowMap, shadowCoord + vec3( vec2( -0.6656654,  -0.7041242 )   * kShadowBlurSize, 0.0 ) ) > d ) {
			v += e;
		}
	}
	return v;
}

vec3 decodeNormal( in vec2 enc )
{
    vec4 n				= vec4( enc.xy, 0.0, 0.0 ) * vec4( 2.0, 2.0, 0.0, 0.0 ) + vec4( -1.0, -1.0, 1.0, -1.0 );
    float l				= dot( n.xyz, -n.xyw );
    n.z					= l;
    n.xy				*= sqrt( l );
    return n.xyz * 2.0 + vec3( 0.0, 0.0, -1.0 );
}

vec4 getPosition( in vec2 uv )
{
    float depth			= texture( uSamplerDepth, uv ).x;
    float linearDepth 	= uProjectionParams.y / ( depth - uProjectionParams.x );
    vec4 posProj		= vec4( ( uv.x - 0.5 ) * 2.0, ( uv.y - 0.5 ) * 2.0, 0.0, 1.0 );
    vec4 viewRay		= uProjMatrixInverse * posProj;
    return vec4( viewRay.xyz * linearDepth, 1.0 );
}

void main( void )
{
	vec2 uv				= gl_FragCoord.xy / uWindowSize;

	vec4 position 		= getPosition( uv );
	vec3 L 				= uLightPosition - position.xyz;
	float d 			= length( L );
	if ( d > uLightRadius ) {
		discard;
	}
	L 					/= d;
	
	vec4 albedo 		= texture( uSamplerAlbedo, uv );
	int materialId		= int( texture( uSamplerMaterial, uv ).r );
	Material material 	= uMaterials[ materialId ];

	vec3 N 				= decodeNormal( texture( uSamplerNormal, uv ).rg );
	vec3 V 				= normalize( -position.xyz );
	vec3 H 				= normalize( L + V );
	float NdotL 		= max( 0.0, dot( N, L ) );
	float HdotN 		= max( 0.0, dot( H, N ) );
	float Ks		 	= pow( HdotN, material.shininess );

	vec4 Ia 			= uLightColorAmbient * material.ambient;
	vec4 Id 			= NdotL * uLightColorDiffuse * albedo * material.diffuse;
	vec4 Ie 			= material.emissive;
	vec4 Is 			= Ks * uLightColorSpecular * material.specular;

	float att			= 1.0 / pow( ( d / ( 1.0 - pow( d / uLightRadius, 2.0 ) ) ) / uLightRadius + 1.0, 2.0 ); 

	oColor 				= Ia + att * ( Id + Is ) + Ie;
	if ( uShadowEnabled ) {
		oColor			= oColor * ( 1.0 - kShadowOpacity ) + ( oColor * shadow( position ) * kShadowOpacity );
	}
	oColor				*= uLightIntensity;
	oColor.a			= 1.0;
}
 