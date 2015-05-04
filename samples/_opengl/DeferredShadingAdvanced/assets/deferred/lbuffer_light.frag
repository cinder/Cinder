#include "../common/unpack.glsl"
#include "../common/offset.glsl"
#include "../common/material.glsl"
#include "../common/light.glsl"

const float kScatter = 0.07;

uniform sampler2D	uSamplerAlbedo;
uniform sampler2D	uSamplerNormal;
uniform mat4		uViewMatrix;

flat in int vInstanceId;

layout (location = 0) out vec4 oColor;

void main( void )
{
	vec2 uv				= calcTexCoordFromFrag( gl_FragCoord.xy );

	Light light			= uLights[ vInstanceId ];
	
	vec4 position 		= unpackPosition( uv );
	
	vec3 L 				= ( uViewMatrix * vec4( light.position, 1.0 ) ).xyz - position.xyz;
	float d 			= length( L );
	if ( d > light.volume ) {
		discard;
	}
	L 					/= d;
	
	vec4 albedo 		= texture( uSamplerAlbedo, uv );
	int materialId		= int( texture( uSamplerMaterial, uv ).r );
	Material material 	= uMaterials[ materialId ];

	vec3 N 				= unpackNormal( texture( uSamplerNormal, uv ).rg );
	vec3 V 				= normalize( -position.xyz );
	vec3 H 				= normalize( L + V );
	float NdotL 		= max( 0.0, dot( N, L ) );
	float HdotN 		= max( 0.0, dot( H, N ) );
	float Ks		 	= pow( HdotN, material.shininess );

	vec4 Ia 			= light.ambient * material.ambient;
	vec4 Id 			= NdotL * light.diffuse * albedo * material.diffuse;
	vec4 Ie 			= material.emissive;
	vec4 Is 			= Ks * light.specular * material.specular;
	float att			= 1.0 / pow( ( d / ( 1.0 - pow( d / light.volume, 2.0 ) ) ) / light.volume + 1.0, 2.0 );

	vec3 dir			= position.xyz;
	float l				= length( dir );
	dir					/= l;
	vec3 q				= -L;
	float b				= dot( dir, q );
	float c				= dot( q, q );
	float s				= 1.0f / sqrt( c - b * b );
	s					= smoothstep( 0.005, 1.0, s * (atan( (s + b) * s) - atan( b * s ) ) );
	
	oColor 				= ( ( Ia + att * ( Id + Is ) + Ie ) * light.intensity );
	oColor.rgb			*= ( vec3( 1.0 - kScatter ) + vec3( 2.5 * light.diffuse.rgb * vec3( s )) * kScatter );
	oColor.a			= 1.0;
}
 