#version 150
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding=1) uniform sampler2D uDiffuseMap;
layout(binding=2) uniform sampler2D uNormalMap;

layout(std140, binding=3) uniform ciBlock1T {
	uniform vec3 uLightLocViewSpace;
} ciBlock1;

// inputs passed from the vertex shader
layout(location=0) in vec4 VertexViewSpace;
layout(location=1) in vec3 NormalViewSpace;
layout(location=2) in vec3 TangentViewSpace;
layout(location=3) in vec3 BitangentViewSpace;
layout(location=4) in vec2 TexCoord0;

// output a single color
layout(location=0) out vec4 oColor;

void main()
{
	// fetch the normal from the normal map
	vec3 vMappedNormal = texture(uNormalMap, TexCoord0.st).rgb * 2.0 - 1.0;

	// modify it using the normal & tangents from the 3D mesh (normal mapping)
	vec3 normal = normalize((TangentViewSpace * vMappedNormal.x) + (BitangentViewSpace * vMappedNormal.y) + (NormalViewSpace * vMappedNormal.z));

	vec3 vToCamera = normalize( -VertexViewSpace.xyz );
	vec3 light = normalize( ciBlock1.uLightLocViewSpace - VertexViewSpace.xyz );
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
