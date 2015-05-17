#version 150

uniform vec3 lightDir;

in vec3 vertPosition;
in vec3 vertNormal;
in vec2 vertTexCoord0;

out vec4 fragColor;

void main()
{
	vec3 ppNormal			= normalize( vertNormal );

	float ppDiffuse			= abs( dot( ppNormal, lightDir ) );
	float ppFresnel			= pow( ( 1.0 - ppDiffuse ), 2.0 );
	float ppSpecular		= pow( ppDiffuse, 15.0 );
	float ppSpecularBright	= pow( ppDiffuse, 80.0 );

	fragColor.rgb		= vec3( 1 );// ( vec3( .25, 0, 0 ) * ppDiffuse + vec3( 1, 0, 0 ) * ppSpecular + ppSpecularBright );
	fragColor.a			= 1.0;
}