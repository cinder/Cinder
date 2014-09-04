#version 150

in vec4 ciPosition;
in vec3 ciNormal;
in vec2 ciTexCoord0;

uniform mat4 ciModelView;
uniform mat4 ciModelViewProjection;
uniform mat3 ciNormalMatrix;

out vec4 vVertex;
out vec3 vNormal;
out vec2 vTexCoord0;

void main() {
	vVertex		= ciModelView * ciPosition;
	vNormal		= ciNormalMatrix *  ciNormal;
	vTexCoord0	= ciTexCoord0;
	gl_Position	= ciModelViewProjection * ciPosition;
}
