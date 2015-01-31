#version 150

uniform sampler2D	uDiffuseMap, uNormalMap;
uniform vec3		uLightLocViewSpace;

// inputs passed from the vertex shader
in vec4		VertexViewSpace;
in vec3		NormalViewSpace, TangentViewSpace, BitangentViewSpace;
in vec2		TexCoord0;

// output a single color
out vec4			oColor;

void main()
{
	// fetch the normal from the normal map
	vec3 vMappedNormal = texture(uNormalMap, TexCoord0.st).rgb * 2.0 - 1.0;

	// modify it using the normal & tangents from the 3D mesh (normal mapping)
	vec3 normal = normalize((TangentViewSpace * vMappedNormal.x) + (BitangentViewSpace * vMappedNormal.y) + (NormalViewSpace * vMappedNormal.z));

	vec3 vToCamera = normalize( -VertexViewSpace.xyz );
	vec3 light = normalize( uLightLocViewSpace - VertexViewSpace.xyz );
	vec3 reflect = normalize(-reflect(light, normal));

	// calculate diffuse term
	float fDiffuse = max( dot(normal, light), 0.0 );
	fDiffuse = clamp( fDiffuse, 0.1, 1.0 );

	// calculate specular term
	float fSpecularPower = 20.0;
	float fSpecular = pow( max( dot(reflect, vToCamera), 0.0), fSpecularPower );
	fSpecular = clamp(fSpecular, 0.0, 1.0);

	vec3 vDiffuseColor = vec3( fDiffuse ) * texture( uDiffuseMap, TexCoord0 ).rgb;

	vec3 vSpecularColor = vec3( fSpecular );

	// output colors to buffer
	oColor.rgb = (vDiffuseColor + vSpecularColor).rgb;
	oColor.a = 1.0;
}