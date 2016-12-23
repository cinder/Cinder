uniform mat4 	ciModelView;
uniform mat3 	ciNormalMatrix;
uniform mat4 	ciModelViewProjection;

in vec4 		ciColor;
in vec3 		ciNormal;
in vec4 		ciPosition;

out vec4 		color;
out vec3 		normal;
out vec4 		position;

void main()
{
	color       = ciColor;
	position    = ciModelView * ciPosition;
	vec3 n		= ciNormal;
	normal      = ciNormalMatrix * n;

	gl_Position 	= ciModelViewProjection * ciPosition;
}
