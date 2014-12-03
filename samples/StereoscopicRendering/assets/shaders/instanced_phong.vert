#version 130

uniform mat4	ciModelView;
uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;

in vec4			ciPosition;
in vec3			ciNormal;
in vec4			ciColor;
in vec2			ciTexCoord0;

in vec3			vInstancePosition;
in vec3			vInstanceColor;

out vec4		vVertex;
out vec3		vNormal;
out vec2		vTexCoord0;
out vec4		vColor;

void main()
{
	vec4 position = ciPosition;
	position.xyz += vInstancePosition;
	
	vVertex = ciModelView * position;       
	vNormal = ciNormalMatrix * ciNormal;

	vTexCoord0 = ciTexCoord0;
	vColor = vec4( vInstanceColor, 1 );
	
	// vertex shader must always pass projection space position
	gl_Position = ciModelViewProjection * position;
}
