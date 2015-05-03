#version 100

uniform samplerCube uCubeMapTex;

varying highp vec3 NormalWorldSpace;
varying highp vec3 EyeDirWorldSpace;

void main( void )
{
	// reflect the eye vector about the surface normal (all in world space)
	highp vec3 reflectedEyeWorldSpace = reflect( EyeDirWorldSpace, normalize(NormalWorldSpace) );
	gl_FragColor = textureCube( uCubeMapTex, reflectedEyeWorldSpace );
}