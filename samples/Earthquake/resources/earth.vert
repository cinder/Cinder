#version 150

uniform mat4 ciModelViewProjection;
uniform mat3 ciNormalMatrix;

in vec4 ciPosition;
in vec3 ciNormal;
in vec2 ciTexCoord0;

out vec3 vertPosition;
out vec3 vertNormal;
out vec2 vertTexCoord0;

void main()
{
	vertPosition = ciPosition.xyz;
	vertNormal = ciNormalMatrix * ciNormal;
	vertTexCoord0 = ciTexCoord0;

	gl_Position = ciModelViewProjection * ciPosition;
}
