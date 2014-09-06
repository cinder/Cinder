#version 150

uniform samplerCube uCubeMapTex;

in vec3	NormalWorldSpace;
in vec3 EyeDirWorldSpace;

out vec4 	oColor;

void main( void )
{
	// reflect the eye vector about the surface normal (all in world space)
	vec3 reflectedEyeWorldSpace = reflect( EyeDirWorldSpace, normalize(NormalWorldSpace) );
	oColor = texture( uCubeMapTex, reflectedEyeWorldSpace );
}