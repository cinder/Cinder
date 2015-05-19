#version 150

uniform mat4 ciViewMatrix;
uniform mat4 ciProjectionMatrix;

in vec4 ciPosition;
in vec3 ciNormal;
in vec2 ciTexCoord0;

in mat4 iModelMatrix;

out vec3 vertPosition;
out vec3 vertNormal;
out vec2 vertTexCoord0;

void main()
{
	vertPosition = ciPosition.xyz;
	vertTexCoord0 = ciTexCoord0;

	// We can safely use the model matrix to convert it
	// to view space, because we make sure to scale it homogeneously.
	vertNormal = vec3( ciViewMatrix * iModelMatrix * vec4( ciNormal, 0 ) );

	gl_Position = ciProjectionMatrix * ciViewMatrix * iModelMatrix * ciPosition;
}
