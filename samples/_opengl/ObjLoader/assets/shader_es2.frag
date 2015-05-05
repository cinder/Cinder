#version 100

precision highp float;

uniform sampler2D 		uTex0;

varying highp vec2		TexCoord0;
varying highp vec3		Normal;

void main( void )
{
	vec3 normal = normalize( -Normal );
	float diffuse = max( dot( normal, vec3( 0, 0, -1 ) ), 0.0 );
	gl_FragColor = texture2D( uTex0, TexCoord0 ) * diffuse;
}