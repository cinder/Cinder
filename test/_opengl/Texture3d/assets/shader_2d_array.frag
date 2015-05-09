#version 150

uniform vec3	uTexCoord;
uniform sampler2DArray uTex0;

out vec4 oColor;

void main( void )
{
	oColor = texture( uTex0, uTexCoord.xyz );
}