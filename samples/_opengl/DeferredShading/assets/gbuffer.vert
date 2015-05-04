uniform mat4 	ciModelView;
uniform mat3 	ciNormalMatrix;
uniform mat4 	ciModelViewProjection;

in vec4 		ciColor;
in vec3 		ciNormal;
in vec4 		ciPosition;

out Vertex
{
	vec4 		color;
	vec3 		normal;
	vec4 		position;
} vertex;

void main()
{
	vertex.color 	= ciColor;
	vertex.position = ciModelView * ciPosition;
	vec3 n			= ciNormal;
	vertex.normal 	= ciNormalMatrix * n;

	gl_Position 	= ciModelViewProjection * ciPosition;
}
