#version 150

uniform mat4	ciModelViewProjection;
in vec4		ciPosition;
in vec4		ciColor;
in vec2		ciTexCoord0;

out VertexData {
	vec4 color;
	vec2 texcoord;
} vVertexOut;

void main()
{
	vVertexOut.color = ciColor;
	vVertexOut.texcoord = ciTexCoord0;
	gl_Position = ciModelViewProjection * ciPosition;
}
