#version 100

uniform samplerCube uCubeMapTex;

varying highp vec3	NormalWorldSpace;

void main( void )
{
	gl_FragColor = textureCube( uCubeMapTex, NormalWorldSpace );
}