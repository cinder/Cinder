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
	highp vec3 Ambient		= vec3( 0.2 );

	highp vec4 ShadowCoord	= vShadowCoord / vShadowCoord.w;
	highp float Shadow		= 1.0;

	if ( ShadowCoord.z > -1.0 && ShadowCoord.z < 1.0 ) {
		// On Android and Linux GL_DEPTH_COMPONENT16 does not
		// expand out into all 4 channels. So we have to
		// use the .r/.x channel to sample.
		highp float Dist = texture2D( uShadowMap, ShadowCoord.st ).x;

		ShadowCoord.z -= 0.0005;
		if ( ShadowCoord.w > 0.0 && Dist < ShadowCoord.z ) {
			Shadow = 0.0;
		}
	}

	gl_FragColor.rgb	= ( Diffuse * Shadow + Ambient ) * vColor.rgb;
	gl_FragColor.a		= 1.0;
}
