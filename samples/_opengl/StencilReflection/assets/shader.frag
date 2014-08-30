#version 150

out vec4 			oColor;
in vec4				Color;
in vec3				Normal;

uniform sampler2D uTex0;
in vec2	TexCoord;
void main( void )
{
	vec3 normal = normalize( -Normal );
	float diffuse = max( dot( normal, vec3( 0, 0, -1 ) ), 0.0 );
	oColor = texture( uTex0, TexCoord.st ) * Color * diffuse;
}