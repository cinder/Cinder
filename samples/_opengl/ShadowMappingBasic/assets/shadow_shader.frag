#version 150 core

uniform vec3 uLightPos;
uniform sampler2DShadow uShadowMap;

in vec4 vColor;
in vec4 vPosition;
in vec3 vNormal;
in vec4 vShadowCoord;

out vec4 Color;

void main( void )
{
	vec3 Normal			= normalize( vNormal );
	vec3 LightVec		= normalize( uLightPos - vPosition.xyz );
	float NdotL			= max( dot( vNormal, LightVec ), 0.0 );
	
	vec3 Diffuse		= vec3( NdotL );
	vec3 Ambient		= vec3( 0.3 );
	
	vec4 ShadowCoord	= vShadowCoord / vShadowCoord.w;
	float Shadow		= 1.0;
	
	if ( ShadowCoord.z > -1 && ShadowCoord.z < 1 ) {
		Shadow = textureProj( uShadowMap, ShadowCoord, -0.00005 );
	}

	Color.rgb = ( Diffuse * Shadow + Ambient ) * vColor.rgb;
	Color.a	= 1.0;
}