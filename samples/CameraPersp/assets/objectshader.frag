#version 150 core

uniform samplerCube uCubeMapTex;

in vec3		NormalWorldSpace;
in vec3		EyeDirWorldSpace;

in vec4		vPosition;
in vec3		vNormal;

out vec4 	oColor;

void main( void )
{
	vec3 LightPos		= vec3( 0.0, 5.0, 1.0 );

	vec3 Normal			= normalize( vNormal );
	vec3 LightVec		= normalize( LightPos - vPosition.xyz );
	float NdotL			= max( dot( vNormal, LightVec ), 0.0 );
	
	vec3 Diffuse		= vec3( NdotL ) + vec3 ( 0.2, 0.3, 0.5 );
	
	float Specular		= pow( NdotL, 100.0 );
	
	vec3 reflectedEyeWorldSpace = reflect( EyeDirWorldSpace, normalize(NormalWorldSpace) );
	oColor.rgb = vec3 ( texture( uCubeMapTex, reflectedEyeWorldSpace ) ) * Diffuse + Specular;
	oColor.a = 1.0;
}