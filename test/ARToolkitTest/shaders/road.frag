#version 110

uniform sampler2D tex0;
varying vec3 normal;


void main()
{
	vec3 sample = texture2D( tex0, gl_TexCoord[0].st ).rgb;
	vec3 ppNormal = normalize( normal );
	vec3 eye = vec3( 0.0, 0.0, 1.0 );	

	float ppDiffuse = max( dot( eye, ppNormal ), 0.0 );

	gl_FragColor.rgb = ppDiffuse * sample;
	gl_FragColor.a = 1.0;
}
