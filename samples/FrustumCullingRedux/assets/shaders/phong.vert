#version 150

uniform mat4	ciModelViewProjection;
uniform mat4	ciModelView;
uniform mat3	ciNormalMatrix;

in vec4	ciPosition;
in vec3	ciNormal;

out vec4 vertPosition;
out vec3 vertNormal;

void main()
{
	vertPosition = ciModelView * ciPosition;
	vertNormal = ciNormalMatrix * ciNormal;
	gl_Position = ciModelViewProjection * ciPosition;
}
