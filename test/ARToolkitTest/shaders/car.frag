#version 110

uniform sampler2D tex0;
uniform vec3 color;
varying vec3 normal;
varying vec3 worldPosition;
varying vec3 viewDir;


void main()
{
	vec3 ppNormal = normalize( normal );
	vec3 L = normalize( vec3( 0.0, 0.0, 1.0 ) );
	vec3 eye = vec3( 0.0, 0.0, 1.0 );	
	vec3 R = ppNormal * 2.0 * ( dot( L, ppNormal ) ) - L;
	vec3 V = normalize( viewDir );
	

//	float ppDiffuse = max( dot( V, ppNormal ), 0.0 );
	float ppDiffuse = abs( dot( V, ppNormal ) );
	float ppSpecular = pow( ppDiffuse, 32.0 ) * 1.0;

	gl_FragColor.rgb = ppDiffuse * color;
	gl_FragColor.a = 1.0;
}
