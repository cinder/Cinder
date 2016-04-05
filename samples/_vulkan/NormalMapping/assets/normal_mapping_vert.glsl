#version 150
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (std140, binding=0) uniform ciBlock0T {
	uniform mat4 ciModelView;
	uniform mat4 ciModelViewProjection;
	uniform mat3 ciNormalMatrix;
} ciBlock0;

layout(location=0) in vec4 ciPosition;
layout(location=1) in vec3 ciNormal;
layout(location=2) in vec3 ciTangent;
layout(location=3) in vec2 ciTexCoord0;

layout(location=0) out vec4 vVertex;
layout(location=1) out vec3 vNormal;
layout(location=2) out vec3 vTangent;
layout(location=3) out vec3 vBiTangent;
layout(location=4) out vec2 vTexCoord0;

void main()
{	
	// calculate view space position (required for lighting)
	vVertex = ciBlock0.ciModelView * ciPosition; 

	// calculate view space normal (required for lighting & normal mapping)
	vNormal = normalize(mat3(ciBlock0.ciNormalMatrix) * ciNormal);

	// calculate tangent and construct the bitangent (required for normal mapping)
	vTangent = normalize(mat3(ciBlock0.ciNormalMatrix) * ciTangent); 
	vBiTangent = normalize(cross(vNormal, vTangent));

	// pass texture coordinates
	vTexCoord0 = ciTexCoord0;

	// vertex shader must always pass projection space position
	gl_Position = ciBlock0.ciModelViewProjection * ciPosition;
}
