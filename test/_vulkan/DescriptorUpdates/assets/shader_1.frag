#version 150
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(set=0, binding=1) uniform sampler2D uTex0;

layout (location = 0) in vec2 TexCoord;

layout (location = 0) out vec4 oColor;

void main( void )
{
	oColor = texture( uTex0, TexCoord );
}
