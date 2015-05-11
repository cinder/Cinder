#version 100

uniform sampler2D uTex0;

varying highp vec4	Color;
varying highp vec3	Normal;
varying highp vec2	TexCoord;

void main( void )
{
	highp vec3 normal = normalize( -Normal );
	highp float diffuse = max( dot( normal, vec3( 0, 0, -1 ) ), 0.0 );
	gl_FragColor = texture2D( uTex0, TexCoord.st ) * Color * diffuse;
}