#version 150

uniform mat4	ciModelViewProjection;
uniform mat4	ciModelView;
uniform mat3	ciNormalMatrix;

in vec4		ciPosition;
in vec3		ciNormal;

out VertexData {
	vec4 position;
	vec3 normal;
} vVertexOut;

void main(void) {
	vVertexOut.position = ciModelView * ciPosition;
	vVertexOut.normal = ciNormalMatrix * ciNormal;
	gl_Position = ciModelViewProjection * ciPosition;
}
