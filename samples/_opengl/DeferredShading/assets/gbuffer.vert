#version 330 core

uniform mat4 	ciModelView;
uniform mat3 	ciNormalMatrix;
uniform mat4 	ciModelViewProjection;

in vec4 		ciPosition;
in vec3 		ciNormal;
in vec2 		ciTexCoord0;
in vec4 		ciColor;

out Vertex
{
	vec4 		color;
	vec3 		normal;
	vec4 		position;
	vec2 		uv;
} vertex;

void main()
{
	vertex.color 	= ciColor;
	vertex.uv 		= ciTexCoord0;
	vertex.position = ( ciModelView * ciPosition );
	vec3 n			= ciNormal;
	vertex.normal 	= normalize( ciNormalMatrix * n );

	gl_Position 	= ciModelViewProjection * ciPosition;
}
