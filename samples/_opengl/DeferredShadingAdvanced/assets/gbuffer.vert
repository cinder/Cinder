uniform mat4 	ciModelView;
uniform mat3 	ciNormalMatrix;
uniform mat4 	ciModelViewProjection;

in vec4 		ciPosition;
in vec3 		ciNormal;
in vec4 		ciColor;

out Vertex
{
	vec3 		color;
	vec3 		normal;
} vertex;

void main()
{
	vertex.color 	= ciColor.rgb;
	vec3 n			= ciNormal;
	vertex.normal 	= normalize( ciNormalMatrix * n );

	gl_Position 	= ciModelViewProjection * ciPosition;
}
 