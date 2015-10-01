#version 100

precision highp float;

varying lowp vec4		Color;
varying highp vec3		Normal;

uniform sampler2D uTex0;
varying highp vec2	TexCoord;
void main( void )
{
	vec3 normal = normalize( -Normal );
	float diffuse = max( dot( normal, vec3( 0, 0, -1 ) ), 0.0 );
	gl_FragColor = texture2D( uTex0, TexCoord.st ) * Color * diffuse;
}