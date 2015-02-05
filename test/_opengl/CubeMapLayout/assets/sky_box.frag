#version 150

uniform samplerCube uCubeMapTex;

in vec3	NormalWorldSpace;

out vec4 	oColor;

void main( void )
{
	oColor = texture( uCubeMapTex, NormalWorldSpace );
}