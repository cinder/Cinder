#version 150
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (std140, binding=0) uniform ciBlock0T {
	uniform mat4 ciModelView;
	uniform mat4 ciModelViewProjection;
	uniform mat3 ciNormalMatrix;
	uniform mat4 ciViewMatrix;
	uniform mat4 ciViewMatrixInverse;
} ciBlock0;

layout(location = 0) in vec4 ciPosition;
layout(location = 1) in vec3 ciNormal;

layout(location = 0) out vec3 NormalWorldSpace;
layout(location = 1) out vec3  EyeDirWorldSpace;

void main( void )
{
	vec4 positionViewSpace = ciBlock0.ciModelView * ciPosition;
	vec4 eyeDirViewSpace = positionViewSpace - vec4( 0, 0, 0, 1 ); // eye is always at 0,0,0 in view space
	EyeDirWorldSpace = vec3( ciBlock0.ciViewMatrixInverse * eyeDirViewSpace );
	vec3 normalViewSpace = ciBlock0.ciNormalMatrix * ciNormal;
	NormalWorldSpace = normalize( vec3( vec4( normalViewSpace, 0 ) * ciBlock0.ciViewMatrix ) );
	gl_Position = ciBlock0.ciModelViewProjection * ciPosition;
}
