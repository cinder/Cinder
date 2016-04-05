#version 150
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (std140, binding=0) uniform ciBlock0T {
	uniform mat4 ciModelViewProjection;
} ciBlock0;

layout (location = 0) in vec4 ciPosition;

layout (location = 0) out vec3 NormalWorldSpace;

void main( void )
{
	NormalWorldSpace = vec3( ciPosition );
	gl_Position = ciBlock0.ciModelViewProjection * ciPosition;
}
