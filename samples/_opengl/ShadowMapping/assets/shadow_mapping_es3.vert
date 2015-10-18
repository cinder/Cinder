#version 300 es

in vec4 ciPosition;
in vec3 ciNormal;
in vec4 ciColor;

uniform mat4 ciModelMatrix;
uniform mat4 ciModelView;
uniform mat3 ciNormalMatrix;
uniform mat4 ciModelViewProjection;

// ShadowMatrix converts from modeling coordinates to shadow map coordinates.
uniform mat4 uShadowMatrix;

out vec4	vColor;
out vec4	vPosition;
out vec3	vNormal;
out vec4	vModelPosition;
out vec3	vModelNormal;
out vec2	vTexCoord0;
// Coordinate to be used for shadow map lookup
out vec4	vShadowCoord;

/* Bias matrix alters the clip coordinates so that x & y
 * lie between 0.0 and 1.0 for texture sampling. */
const mat4 biasMat  = mat4(	0.5, 0.0, 0.0, 0.0,
							0.0, 0.5, 0.0, 0.0,
							0.0, 0.0, 0.5, 0.0,
							0.5, 0.5, 0.5, 1.0 );
void main() {
	vColor			= ciColor;
	vPosition		= ciModelView * ciPosition;
	vModelPosition	= ciModelMatrix * ciPosition;
	vModelNormal	= (ciModelMatrix * vec4(ciNormal, 0.0)).xyz;
	vNormal			= normalize( ciNormalMatrix * ciNormal );
	
	vShadowCoord	= (biasMat * uShadowMatrix * ciModelMatrix) * ciPosition;
	gl_Position		= ciModelViewProjection * ciPosition;
}