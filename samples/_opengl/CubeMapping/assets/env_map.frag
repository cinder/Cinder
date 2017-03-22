#version 150

uniform samplerCube uCubeMapTex;

in vec3	NormalWorldSpace;
in vec3 EyeDirWorldSpace;

out vec4 	oColor;

void main()
{
#if 1
	// reflect the eye ray about the surface normal (all in world space)
	vec3 lookup = reflect( EyeDirWorldSpace, normalize( NormalWorldSpace ) );
#else
	// refract the eye ray upong entering the surface (we're ignoring the second refraction when the ray exits the object).
	float refractiveIndex = 1.0 / 1.08;
	vec3 lookup = refract( normalize( EyeDirWorldSpace ), normalize( NormalWorldSpace ), refractiveIndex );
#endif

	oColor = texture( uCubeMapTex, lookup );
}