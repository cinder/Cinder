#version 150

uniform mat4	ciModelViewProjection;
uniform mat4	ciModelView;
uniform mat3	ciNormalMatrix;

in vec4		ciPosition;
in vec3		ciNormal;
in vec4		ciColor;
in vec2		ciTexCoord0;

out VertexData {
	vec4 position;
	vec3 normal;
	vec4 color;
	vec2 texCoord;
} vVertexOut;

void main()
{
	vVertexOut.position = ciModelView * ciPosition;
	vVertexOut.normal = ciNormalMatrix * ciNormal;
	vVertexOut.color = ciColor;
	vVertexOut.texCoord = ciTexCoord0;

	gl_Position = ciModelViewProjection * ciPosition;
}
