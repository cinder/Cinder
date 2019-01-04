
uniform mat4	ciModelView;
uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;

in vec4			ciPosition;
in vec3			ciNormal;
in vec3			ciTangent;
in vec2			ciTexCoord0;

out vec4		vVertex;
out vec3		vNormal;
out vec3		vTangent;
out vec3		vBiTangent;
out vec2		vTexCoord0;

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
