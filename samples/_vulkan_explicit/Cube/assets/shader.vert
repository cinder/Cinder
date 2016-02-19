#version 150
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (std140, binding=0) uniform ciBlock0T {
	uniform mat4	ciModelViewProjection;
	uniform mat3	ciNormalMatrix;
} ciBlock0;

layout (location = 0) in vec4 ciPosition;
layout (location = 1) in vec2 ciTexCoord0;
layout (location = 2) in vec3 ciNormal;
layout (location = 3) in vec4 ciColor;

layout (location = 0) out vec4 Color;
layout (location = 1) out vec3 Normal;
layout (location = 2) out vec2 TexCoord;

void main( void )
{
	gl_Position	  = ciBlock0.ciModelViewProjection * ciPosition;
	//gl_Position.y = -gl_Position.y;
	Color 		  = ciColor;
	TexCoord	  = ciTexCoord0;
	Normal		  = ciBlock0.ciNormalMatrix * ciNormal;
}

/*
layout (std140, binding=0) uniform ciBlock0T {
	uniform mat4	ciModelViewProjection;
	uniform mat4	ciModelView;
} ciBlock0;

layout (location = 0) in vec4 ciPosition;
layout (location = 1) in vec2 ciTexCoord0;
layout (location = 2) in vec3 ciNormal;
layout (location = 3) in vec4 ciColor;

layout (location = 0) out vec4 Color;
layout (location = 1) out vec3 Normal;
layout (location = 2) out vec2 TexCoord;

void main( void )
{
	gl_Position	  = ciBlock0.ciModelViewProjection * ciPosition;
	gl_Position.y = -gl_Position.y;
	Color 		  = ciColor;
	TexCoord	  = ciTexCoord0;
	Normal		  = transpose(inverse(mat3(ciBlock0.ciModelView))) * ciNormal;
}
*/
