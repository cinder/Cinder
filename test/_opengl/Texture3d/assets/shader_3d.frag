#version 150

uniform float uDepth;
uniform sampler3D uTex0;

in vec2	vTexCoord;
out vec4 oColor;

void main( void )
{
	oColor = texture( uTex0, vec3( vTexCoord, uDepth ) );
}