#version 150
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(std140, binding = 0) uniform ciBlock0T {
	uniform mat4 ciModelViewProjection;
	uniform mat4 ciModelMatrix;
	uniform mat3 ciNormalMatrix;
	uniform mat4 ciModelView;
	uniform mat4 uShadowMatrix;
} ciBlock0;

layout(location = 0) in vec4 ciPosition;
layout(location = 1) in vec4 ciColor;
layout(location = 2) in vec3 ciNormal;

layout(location = 0) out vec4 vPosition;
layout(location = 1) out vec4 vColor;
layout(location = 2) out vec3 vNormal;
layout(location = 3) out vec4 vShadowCoord;

const mat4 biasMatrix = mat4( 0.5, 0.0, 0.0, 0.0,
							  0.0, 0.5, 0.0, 0.0,
							  0.0, 0.0, 1.0, 0.0,
							  0.5, 0.5, 0.0, 1.0 );
void main( void )
{
	vColor			= ciColor;
	vPosition		= ciBlock0.ciModelView * ciPosition;
	vNormal			= normalize( ciBlock0.ciNormalMatrix * ciNormal );
	vShadowCoord	= ( biasMatrix * ciBlock0.uShadowMatrix * ciBlock0.ciModelMatrix ) * ciPosition;
	gl_Position		= ciBlock0.ciModelViewProjection * ciPosition;
}
