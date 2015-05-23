#version 100

uniform lowp vec3 uLightPos;
uniform sampler2D uShadowMap;

varying lowp vec4 vColor;
varying lowp vec4 vPosition;
varying lowp vec3 vNormal;
varying highp vec4 vShadowCoord;

void main( void )
{

	highp vec3 Normal		= normalize( vNormal );
	highp vec3 LightVec		= normalize( uLightPos - vPosition.xyz );
	highp float NdotL		= max( dot( vNormal, LightVec ), 0.0 );
	
	highp vec3 Diffuse		= vec3( NdotL );
	highp vec3 Ambient		= vec3( 0.3 );

	highp vec4 ShadowCoord	= vShadowCoord / vShadowCoord.w;
	highp float Shadow		= 1.0;
	
	if ( ShadowCoord.z > -1.0 && ShadowCoord.z < 1.0 ) {
		ShadowCoord.z -= 0.0005;
		highp float Dist = texture2D( uShadowMap, ShadowCoord.st ).z;
		
		if ( ShadowCoord.w > 0.0 && Dist < ShadowCoord.z ) {
			Shadow = 0.0;
		}
	}
	
	gl_FragColor.rgb	= ( Diffuse * Shadow + Ambient ) * vColor.rgb;
	gl_FragColor.a		= 1.0;
}