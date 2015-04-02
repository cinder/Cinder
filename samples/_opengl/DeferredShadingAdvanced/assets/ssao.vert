#version 330 core

uniform mat4 	ciModelViewProjection;

in vec4 		ciPosition;

out Vertex
{
	vec2 	uv;
} vertex;

void main( void ) 
{
	vec4 v 		= sign( ciModelViewProjection * ciPosition );
	vertex.uv 	= ( v.xy + 1.0 ) * 0.5;

	gl_Position = v;
}
