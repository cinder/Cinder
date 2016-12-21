#version 150

uniform mat4 ciModelMatrix;
uniform mat4 ciViewMatrix;
uniform mat3 ciNormalMatrix;
uniform mat4 ciProjectionMatrix;

in vec4 ciPosition;
in vec3 ciNormal;
in vec3 InstancePosition;
in float InstanceLod;

out vec4 Position;
out vec3 Normal;
flat out float Lod;

void main()
{
	vec4 worldSpacePosition = ciModelMatrix * ciPosition + vec4( InstancePosition, 0 );

	Position = ciViewMatrix * worldSpacePosition;
	Normal = ciNormalMatrix * ciNormal;
	Lod = InstanceLod;

	gl_Position = ciProjectionMatrix * Position;
}
