#version 100

uniform mat4	ciModelView;
uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;

attribute vec4			ciPosition;
attribute vec3			ciNormal;
attribute vec3			ciTangent;
attribute vec2			ciTexCoord0;

varying highp vec4		VertexViewSpace;
varying highp vec3		NormalViewSpace;
varying highp vec3		TangentViewSpace;
varying highp vec3		BitangentViewSpace;
varying highp vec2		TexCoord0;

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
