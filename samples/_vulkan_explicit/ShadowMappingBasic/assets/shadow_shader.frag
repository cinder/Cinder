#version 150
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(std140, binding = 1) uniform ciBlock1T{
	uniform vec4 uLightPos;
	uniform vec4 uColor;
} ciBlock1;

//layout(binding = 2) uniform sampler2DShadow uShadowMap;
layout(binding = 2) uniform sampler2D uShadowMap;

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec4 vShadowCoord;

layout(location = 0) out vec4 Color;

void main( void )
{
	vec3 Normal			= normalize( vNormal );
	vec3 LightVec		= normalize( vec3(ciBlock1.uLightPos) - vPosition.xyz );
	float NdotL			= max( dot( vNormal, LightVec ), 0.0 );
	
	vec3 Diffuse		= vec3( NdotL );
	vec3 Ambient		= vec3( 0.3 );

	vec4 ShadowCoord	= vShadowCoord / vShadowCoord.w;
	float Shadow		= 1.0;

// NOTE: Something is up with textureProj - so revert to the old method for now.
/*
	if ( ShadowCoord.z > -1 && ShadowCoord.z < 1 ) {
		Shadow = textureProj( uShadowMap, ShadowCoord, -0.00005 );
	}

	Color.rgb = ( Diffuse * Shadow + Ambient ) * ciBlock1.uColor.rgb;
	Color.a	= 1.0;
*/


	if ( ShadowCoord.z > -1.0 && ShadowCoord.z < 1.0 ) {
		float Dist = texture( uShadowMap, ShadowCoord.st ).r;
		ShadowCoord.z -= 0.0005;
		if ( ShadowCoord.w > 0.0 && Dist < ShadowCoord.z ) {
			Shadow = 0.0;
		}
	}


// NOTE: This method almost works - produces shadow acne because there isn't the
//       equivalent of glPolyonoffset figured out yet.
/*
	if ( ShadowCoord.z > -1.0 && ShadowCoord.z < 1.0 ) {
		float Dist = texture( uShadowMap, ShadowCoord.xyz, -0.000005 );
		if ( ShadowCoord.w > 0.0 && Dist < ShadowCoord.z ) {
			Shadow = 0.0;
		}
	}
*/

	Color.rgb = ( Diffuse * Shadow + Ambient ) * ciBlock1.uColor.rgb;
	Color.a	= 1.0;
}
