#version 150

uniform mat4	ciModelViewProjection;
uniform mat4	ciModelView;
uniform mat3	ciNormalMatrix;

in vec4		ciPosition;
in vec3		ciNormal;

out vec3 PositionEyeSpace;
out vec3 Normal;

void main()
{
	PositionEyeSpace = vec3( ciModelView * ciPosition );
	Normal = normalize( ciNormalMatrix * ciNormal );
	gl_Position = ciModelViewProjection * ciPosition;
}
