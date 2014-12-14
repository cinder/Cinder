#version 300 es

uniform sampler2D uTex0;

in highp vec4	Color;
in highp vec3	Normal;
in highp vec2	TexCoord;

out highp vec4 			oColor;

void main( void )
{
	highp vec3 normal = normalize( -Normal );
	highp float diffuse = max( dot( normal, vec3( 0, 0, -1 ) ), 0.0 );
	oColor = texture( uTex0, TexCoord.st ) * Color * diffuse;
}