#version 150
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec4 Color;

layout(location = 0) out vec4 oColor;

void main( void )
{
	oColor =  Color;
}