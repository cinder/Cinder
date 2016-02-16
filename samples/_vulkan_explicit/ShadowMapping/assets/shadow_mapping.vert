//#version 410
#version 150
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec4 ciPosition;
layout(location = 1) in vec3 ciNormal;
layout(location = 2) in vec4 ciColor;

layout(std140, binding = 0) uniform ciBlock0T {
	uniform mat4 ciModelMatrix;
	uniform mat4 ciModelView;
	uniform mat3 ciNormalMatrix;
	uniform mat4 ciModelViewProjection;
	// ShadowMatrix converts from modeling coordinates to shadow map coordinates.
	uniform mat4 uShadowMatrix;
} ciBlock0;

layout(location = 0) out vec4	vColor;
layout(location = 1) out vec4	vPosition;
layout(location = 2) out vec3	vNormal;
layout(location = 3) out vec4	vModelPosition;
layout(location = 4) out vec3	vModelNormal;
// Coordinate to be used for shadow map lookup
layout(location = 5) out vec4	vShadowCoord;

/* Bias matrix alters the clip coordinates so that x & y
 * lie between 0.0 and 1.0 for texture sampling. */
const mat4 biasMat  = mat4(	0.5, 0.0, 0.0, 0.0,
							0.0, 0.5, 0.0, 0.0,
							0.0, 0.0, 1.0, 0.0,
							0.5, 0.5, 0.0, 1.0 );
void main() {
	vColor			= ciColor;
	vPosition		= ciBlock0.ciModelView * ciPosition;
	vModelPosition	= ciBlock0.ciModelMatrix * ciPosition;
	vModelNormal	= (ciBlock0.ciModelMatrix * vec4(ciNormal, 0.0)).xyz;
	vNormal			= normalize( ciBlock0.ciNormalMatrix * ciNormal );
	
	vShadowCoord	= ( biasMat * ciBlock0.uShadowMatrix * ciBlock0.ciModelMatrix) * ciPosition;
	gl_Position		= ciBlock0.ciModelViewProjection * ciPosition;
}
