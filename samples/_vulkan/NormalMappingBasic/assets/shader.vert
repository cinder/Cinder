#version 150
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(std140, binding=0) uniform ciBlock0T {
	uniform mat4 ciModelViewProjection;
	uniform mat3 ciNormalMatrix;
	uniform mat4 ciModelView;
} ciBlock0;

layout(location=0) in vec4 ciPosition;
layout(location=1) in vec3 ciNormal;
layout(location=2) in vec3 ciTangent;
layout(location=3) in vec2 ciTexCoord0;

layout(location=0) out vec4 VertexViewSpace;
layout(location=1) out vec3 NormalViewSpace;
layout(location=2) out vec3 TangentViewSpace;
layout(location=3) out vec3 BitangentViewSpace;
layout(location=4) out vec2 TexCoord0;

void main()
{	
	// calculate view space position (required for lighting)
	VertexViewSpace = ciBlock0.ciModelView * ciPosition;

	// calculate view space normal (required for lighting & normal mapping)
	NormalViewSpace = normalize(ciBlock0.ciNormalMatrix * ciNormal);

	// calculate tangent and construct the bitangent (required for normal mapping)
	TangentViewSpace = normalize( ciBlock0.ciNormalMatrix * ciTangent );
	BitangentViewSpace = normalize( cross( TangentViewSpace, NormalViewSpace ) );

	// pass texture coordinates
	TexCoord0 = ciTexCoord0;

	// vertex shader must always pass projection space position
	gl_Position = ciBlock0.ciModelViewProjection * ciPosition;
}
