#version 110

varying vec3 normal;
uniform vec3 lightDir;
void main()
{
	vec3 ppNormal			= normalize( normal );

	float ppDiffuse			= abs( dot( ppNormal, lightDir ) );
	float ppFresnel			= pow( ( 1.0 - ppDiffuse ), 2.0 );
	float ppSpecular		= pow( ppDiffuse, 15.0 );
	float ppSpecularBright	= pow( ppDiffuse, 80.0 );

	gl_FragColor.rgb		= ( vec3( .25, 0, 0 ) * ppDiffuse + vec3( 1, 0, 0 ) * ppSpecular + ppSpecularBright );
	gl_FragColor.a			= 1.0;
}