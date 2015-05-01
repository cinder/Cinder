#version 150

uniform mat4	ciModelView;
uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;

in vec4			ciPosition;
in vec3			ciNormal;
in vec3			ciTangent;
in vec2			ciTexCoord0;

out vec4		VertexViewSpace;
out vec3		NormalViewSpace;
out vec3		TangentViewSpace;
out vec3		BitangentViewSpace;
out vec2		TexCoord0;

void main()
{	
	// calculate view space position (required for lighting)
	VertexViewSpace = ciModelView * ciPosition;

	// calculate view space normal (required for lighting & normal mapping)
	NormalViewSpace = normalize(ciNormalMatrix * ciNormal);

	// calculate tangent and construct the bitangent (required for normal mapping)
	TangentViewSpace = normalize( ciNormalMatrix * ciTangent );
	BitangentViewSpace = normalize( cross( TangentViewSpace, NormalViewSpace ) );

	// pass texture coordinates
	TexCoord0 = ciTexCoord0;

	// vertex shader must always pass projection space position
	gl_Position = ciModelViewProjection * ciPosition;
}
