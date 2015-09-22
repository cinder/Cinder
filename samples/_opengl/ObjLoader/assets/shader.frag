#version 150

uniform sampler2D	uTex0;

in vec4		Color;
in vec3		Normal;
in vec2		TexCoord0;

out vec4 	oColor;

void main( void )
{
	vec3 normal = normalize( -Normal );
	float diffuse = max( dot( normal, vec3( 0, 0, -1 ) ), 0 );
	vec3 color = texture( uTex0, TexCoord0 ).rgb * diffuse;
	oColor = vec4( color, 1.0 );
}