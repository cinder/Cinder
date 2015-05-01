#version 150

uniform sampler2D uTex0;

in vec4	Color;
in vec3	Normal;
in vec2	TexCoord;

out vec4 			oColor;

void main( void )
{
	vec3 normal = normalize( -Normal );
	float diffuse = max( dot( normal, vec3( 0, 0, -1 ) ), 0 );
	oColor = texture( uTex0, TexCoord.st ) * Color * diffuse;
}