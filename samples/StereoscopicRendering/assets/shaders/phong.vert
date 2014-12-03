#version 130

uniform mat4	ciModelView;
uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;

in vec4			ciPosition;
in vec3			ciNormal;
in vec4			ciColor;
in vec2			ciTexCoord0;

out vec4		vVertex;
out vec3		vNormal;
out vec2		vTexCoord0;
out vec4		vColor;

void main()
{
	vVertex = ciModelView * ciPosition;       
	vNormal = ciNormalMatrix * ciNormal;

	vTexCoord0 = ciTexCoord0;
	vColor = ciColor;
	
	// vertex shader must always pass projection space position
	gl_Position = ciModelViewProjection * ciPosition;
}
