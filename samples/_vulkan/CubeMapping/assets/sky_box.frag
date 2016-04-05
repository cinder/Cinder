#version 150
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 1) uniform samplerCube uCubeMapTex;

layout(location = 0) in vec3 NormalWorldSpace;

layout(location = 0) out vec4 oColor;

void main( void )
{
	oColor = texture( uCubeMapTex, NormalWorldSpace );
}
