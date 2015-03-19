#version 430 core

const uint MATERIAL_COUNT = 4;

layout (binding = 0) uniform sampler2D			uTextureAlbedo;
layout (binding = 1) uniform usampler2D			uTextureMaterial;
layout (binding = 2) uniform sampler2D			uTextureNormalDepth;
layout (binding = 3) uniform sampler2D			uTexturePosition;
layout (binding = 4) uniform sampler2DShadow 	uTextureShadowMap;

uniform vec4	uLightColorAmbient;
uniform vec4	uLightColorDiffuse;
uniform vec4	uLightColorSpecular;
uniform float	uLightIntensity;
uniform vec3	uLightPosition;
uniform float	uLightRadius;

uniform float	uShadowBias;
uniform float	uShadowBlurSize;
uniform mat4 	uShadowMatrix;
uniform vec2	uShadowPixel;
uniform float	uShadowSamples;

uniform mat4 	uViewMatrixInverse;
uniform vec2	uWindowSize;

struct Material
{
	vec4	ambient;
	vec4	diffuse;
	vec4	emissive;
	vec4	specular;
	float	shininess;
	float	twoSided;
	uint 	pad0; // std140
	uint 	pad1; // std140
};

layout (std140, binding = 0) uniform Materials
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
	vec2 uv				= gl_FragCoord.xy / uWindowSize;

	vec4 position 		= texture( uTexturePosition, uv );
	vec3 L 				= uLightPosition - position.xyz;
	float d 			= length( L );
	if ( d > uLightRadius ) {
		discard;
	}
	L 					/= d;
	
	vec4 albedo 		= texture( uTextureAlbedo, 		uv );
	uint materialId 	= texture( uTextureMaterial, 	uv ).r;
	Material material 	= uMaterials[ materialId ];

	vec4 normalDepth 	= texture( uTextureNormalDepth, uv );
	vec3 N 				= normalize( normalDepth.xyz );
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

	float shadow 		= 2.0;
	vec4 shadowClip 	= uShadowMatrix * uViewMatrixInverse * position;
	vec3 shadowCoord 	= shadowClip.xyz / shadowClip.w;
	shadowCoord 		= shadowCoord * 0.5 + 0.5;
	if ( shadowCoord.z > uShadowBias && 
		 shadowCoord.x >= 0.0 && shadowCoord.x <= 1.0 && 
		 shadowCoord.y >= 0.0 && shadowCoord.y <= 1.0 ) {
		shadow = texture( uTextureShadowMap, shadowCoord ).r;
		if ( shadow < shadowCoord.z + uShadowBias ) {
			if ( uShadowBlurSize > 0.0 ) {
				float d		= uShadowBlurSize / uShadowSamples;
				float c		= 1.0;
				for ( float i = 1.0; i < uShadowBlurSize; i += d, c += 4.0 ) {
					shadow 	+= texture( uTextureShadowMap, shadowCoord + vec3( uShadowPixel.x, 0.0, 0.0 )	* i ).r;
					shadow 	+= texture( uTextureShadowMap, shadowCoord + vec3( 0.0, uShadowPixel.y, 0.0 )	* i ).r;
					shadow 	+= texture( uTextureShadowMap, shadowCoord + vec3( -uShadowPixel.x, 0.0, 0.0 )	* i ).r;
					shadow 	+= texture( uTextureShadowMap, shadowCoord + vec3( 0.0, -uShadowPixel.y, 0.0 )	* i ).r;
				}
				shadow		/= c;
			}
			shadow			+= 1.0;
		} else {
		shadow = 2.0;
		}
	}

	oColor 				= Ia + att * ( Id + Is ) + Ie;
	oColor				*= shadow;
	oColor				*= uLightIntensity;
}
