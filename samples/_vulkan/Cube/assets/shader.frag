#version 150
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(set=1, binding=0) uniform sampler2D uTex0;

layout (location = 0) in vec4 Color;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

layout (location = 0) out vec4 oColor;

void main( void )
{
	vec3 normal = normalize( -Normal );
	float diffuse = max( dot( normal, vec3( 0, 0, -1 ) ), 0 );
	oColor = texture( uTex0, TexCoord.st ) * Color * diffuse;
}
