#version 150

uniform mat4	ciModelView;
uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;

in vec4			ciPosition;
in vec3			ciNormal;
in vec4			ciColor;

in vec3			aInstancePosition;
in vec3			aInstanceColor;

out vec4		vertPosition;
out vec3		vertNormal;
out vec4		vertColor;
out vec4        vertUp;

void main()
{
	vec4 position = ciPosition;
	position.xyz += aInstancePosition;
	
	vertPosition = ciModelView * position;       
	vertNormal = ciNormalMatrix * ciNormal;
	vertColor = vec4( aInstanceColor, 1 );
	vertUp = ciModelView * vec4(0, 1, 0, 0);
	
	// vertex shader must always pass projection space position
	gl_Position = ciModelViewProjection * position;
}
