#version 150
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 1) uniform samplerCube uCubeMapTex;

layout(location = 0) in vec3 NormalWorldSpace;
layout(location = 1) in vec3 EyeDirWorldSpace;

layout(location = 0) out vec4 oColor;

void main( void )
{
	// reflect the eye vector about the surface normal (all in world space)
	vec3 reflectedEyeWorldSpace = reflect( EyeDirWorldSpace, normalize(NormalWorldSpace) );
	oColor = texture( uCubeMapTex, reflectedEyeWorldSpace );
}
