#version 110

uniform float counter;

uniform sampler2D texDiffuse;
uniform sampler2D texNormal;
uniform sampler2D texMask;
uniform vec3 lightDir;

varying vec3 normal;
varying vec3 position;

void main()
{
	vec2 texCoord			= vec2( gl_TexCoord[0].s, gl_TexCoord[0].t );
	vec3 diffuseSample		= texture2D( texDiffuse, texCoord ).rgb;
	vec3 normalSample		= ( texture2D( texNormal, texCoord ).rgb - vec3( 0.5, 0.5, 0.5 ) ) * 2.0;
	vec3 texSample			= texture2D( texMask, texCoord ).rgb;
	
	
	// use green channel for land elevation data
	float landValue			= texSample.g;

	// use blue channel for ocean elevation data
	float oceanValue		= texSample.b;

	
	vec3 ppNormal			= normalize( normal + normalSample );
	float ppDiffuse			= abs( dot( ppNormal, lightDir ) );
	float ppFresnel			= pow( ( 1.0 - ppDiffuse ), 3.0 );
	float ppSpecular		= pow( ppDiffuse, 10.0 );
	float ppSpecularBright	= pow( ppDiffuse, 120.0 );
	
	
	// use red channel for nighttime city lights
	float electrictyValue	= ( texSample.r ) * ( 1.0 - ppDiffuse );

	vec3 landFinal			= diffuseSample * landValue + ppSpecularBright * landValue;
	vec3 oceanFinal			= diffuseSample * ppSpecular * oceanValue + oceanValue * ppSpecularBright + oceanValue * ppFresnel * 15.0;
	
	float r					= ( 1.0 - ppNormal.r ) * oceanValue * 0.5;
	gl_FragColor.rgb		= landFinal + oceanFinal;// + vec3( r*r, r * 0.25, 0 ) * oceanValue;
	gl_FragColor.a			= 1.0;
}

