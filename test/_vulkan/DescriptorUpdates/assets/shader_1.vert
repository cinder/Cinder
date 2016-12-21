#version 150
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (std140, set=0, binding=0) uniform ciBlock0T {
	uniform mat4	ciModelViewProjection;
} ciBlock0;

layout (location = 0) in vec4 ciPosition;
layout (location = 1) in vec2 ciTexCoord0;

layout (location = 0) out vec2 TexCoord;

void main( void )
{
	gl_Position	  = ciBlock0.ciModelViewProjection * ciPosition;
	TexCoord	  = ciTexCoord0;
}
