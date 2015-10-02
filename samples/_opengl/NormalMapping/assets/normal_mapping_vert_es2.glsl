#version 100

uniform mat4	ciModelView;
uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;

attribute vec4			ciPosition;
attribute vec3			ciNormal;
attribute vec3			ciTangent;
attribute vec2			ciTexCoord0;

varying vec4		vVertex;
varying vec3		vNormal;
varying vec3		vTangent;
varying vec3		vBiTangent;
varying vec2		vTexCoord0;

void main()
{	
	// calculate view space position (required for lighting)
	vVertex = ciModelView * ciPosition; 

	// calculate view space normal (required for lighting & normal mapping)
	vNormal = normalize(ciNormalMatrix * ciNormal);

	// calculate tangent and construct the bitangent (required for normal mapping)
	vTangent = normalize(ciNormalMatrix * ciTangent); 
	vBiTangent = normalize(cross(vNormal, vTangent));

	// pass texture coordinates
	vTexCoord0 = ciTexCoord0;

	// vertex shader must always pass projection space position
	gl_Position = ciModelViewProjection * ciPosition;
}
