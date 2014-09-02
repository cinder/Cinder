#version 150

in VertexData	{
	vec4 position;
	vec3 normal;
} vVertexIn;

uniform vec3 cAmbient;
uniform vec3 cDiffuse;
uniform vec3 cSpecular;
uniform float shininess;

out vec4 oColor;

void main(void) {
	// light properties in view space
	vec3 vLightPosition = vec3(0.0, 0.0, 0.0);

	// lighting calculations
	vec3 vVertex = vVertexIn.position.xyz;
	vec3 vNormal = normalize( vVertexIn.normal );
	vec3 vToLight = normalize( vLightPosition - vVertex );
	vec3 vToEye = normalize( -vVertex );
	vec3 vReflect = normalize( -reflect(vToLight, vNormal) );

	// diffuse coefficient
	vec3 diffuse = max( dot( vNormal, vToLight ), 0.0 ) * cDiffuse;

	// specular coefficient with energy conservation
	float coeff = (2.0 + shininess) / (2.0 * 3.14159265);
	vec3 specular = pow( max( dot( vReflect, vToEye ), 0.0 ), shininess ) * coeff * cSpecular;

	// to conserve energy, diffuse and specular colors should not exceed one
	float maxDiffuse = max(diffuse.r, max(diffuse.g, diffuse.b));
	float maxSpecular = max(specular.r, max(specular.g, specular.b));
	float fConserve = 1.0 / max(1.0, maxDiffuse + maxSpecular);

	// final color
	oColor.rgb = cAmbient + ( (diffuse + specular) * fConserve );
	oColor.a = 1.0;
}