#version 150
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

// layout(location=0) inputs passed from the vertex shader
layout(location=0) in vec4 vVertex;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec3 vTangent;
layout(location=3) in vec3 vBiTangent;
layout(location=4) in vec2 vTexCoord0;

// output a single color
layout(location=0) out vec4 oColor;

// textures and settings
layout(binding=1) uniform sampler2D uDiffuseMap;
layout(binding=2) uniform sampler2D uSpecularMap;
layout(binding=3) uniform sampler2D uNormalMap;
layout(binding=4) uniform sampler2D uEmmisiveMap;

layout(std140, binding=5) uniform ciBlock1T {
	uniform bool bShowNormals;
	uniform bool bUseDiffuseMap;
	uniform bool bUseSpecularMap;
	uniform bool bUseNormalMap;
	uniform bool bUseEmmisiveMap;
} ciBlock1;

// light source information
#define MAX_NUM_LIGHTS 16

struct LightSource
{
	vec4 position;
	vec4 diffuse;
	vec4 specular;
};

/*
layout(std140, binding=6) uniform ciBlock2T {
	uniform LightSource	uLights[MAX_NUM_LIGHTS];
	uniform int			uNumOfLights;
} ciBlock2;
*/
layout(std140, binding=6) uniform ciBlock2T {
	vec4 lightSource0_position;
	vec4 lightSource0_diffuse;
	vec4 lightSource0_specular;	
	vec4 lightSource1_position;
	vec4 lightSource1_diffuse;
	vec4 lightSource1_specular;	
} ciBlock2;


void main()
{
	// fetch the normal from the normal map
	vec3	vMappedNormal = texture(uNormalMap, vTexCoord0.st).rgb * 2.0 - 1.0;

	// modify it using the normal & tangents from the 3D mesh (normal mapping)
	vec3	vSurfaceNormal = ciBlock1.bUseNormalMap ? normalize((vTangent * vMappedNormal.x) + (vBiTangent * vMappedNormal.y) + (vNormal * vMappedNormal.z)) : vNormal;
  
	// apply each of our light sources
	vec4	vDiffuseColor	= ciBlock1.bUseEmmisiveMap ? texture(uEmmisiveMap, vTexCoord0.st) : vec4(0, 0, 0, 1);
	vec4	vSpecularColor	= vec4(0, 0, 0, 1);

	vec3	vToCamera = normalize(-vVertex.xyz);
	
/*
	for(int i=0; i<ciBlock2.uNumOfLights; ++i)
	{
		// calculate view space light vectors
		vec3	vToLight = normalize(ciBlock2.uLights[i].position.xyz - vVertex.xyz); 
		vec3	vReflect = normalize(-reflect(vToLight, vSurfaceNormal));

		// calculate diffuse term
		float	fDiffuse = max(dot(vSurfaceNormal, vToLight), 0.0);
		fDiffuse = clamp(fDiffuse, 0.1, 1.0);  

		// calculate specular term
		float	fSpecularPower = 100.0;
		float	fSpecular = pow( max( dot(vReflect, vToCamera), 0.0), fSpecularPower );
		fSpecular = clamp(fSpecular, 0.0, 1.0);

		// calculate final colors
		if(ciBlock1.bUseDiffuseMap)
			vDiffuseColor += texture(uDiffuseMap, vTexCoord0.st) * ciBlock2.uLights[i].diffuse * fDiffuse;
		else
			vDiffuseColor += ciBlock2.uLights[i].diffuse * fDiffuse; 

		if(ciBlock1.bUseSpecularMap)
			vSpecularColor += texture(uSpecularMap, vTexCoord0.st) * ciBlock2.uLights[i].specular * fSpecular;
		else
			vSpecularColor += ciBlock2.uLights[i].specular * fSpecular; 
	}
*/

	LightSource	uLights[MAX_NUM_LIGHTS];
	uLights[0].position = ciBlock2.lightSource0_position;
	uLights[0].diffuse  = ciBlock2.lightSource0_diffuse;
	uLights[0].specular = ciBlock2.lightSource0_specular;
	uLights[1].position = ciBlock2.lightSource1_position;
	uLights[1].diffuse  = ciBlock2.lightSource1_diffuse;
	uLights[1].specular = ciBlock2.lightSource1_specular;
	int uNumOfLights = 2;	
	for(int i=0; i<uNumOfLights; ++i)
	{
		// calculate view space light vectors
		vec3	vToLight = normalize(uLights[i].position.xyz - vVertex.xyz); 
		vec3	vReflect = normalize(-reflect(vToLight, vSurfaceNormal));

		// calculate diffuse term
		float	fDiffuse = max(dot(vSurfaceNormal, vToLight), 0.0);
		fDiffuse = clamp(fDiffuse, 0.1, 1.0);  

		// calculate specular term
		float	fSpecularPower = 100.0;
		float	fSpecular = pow( max( dot(vReflect, vToCamera), 0.0), fSpecularPower );
		fSpecular = clamp(fSpecular, 0.0, 1.0);

		// calculate final colors
		if(ciBlock1.bUseDiffuseMap)
			vDiffuseColor += texture(uDiffuseMap, vTexCoord0.st) * uLights[i].diffuse * fDiffuse;
		else
			vDiffuseColor += uLights[i].diffuse * fDiffuse; 

		//if(ciBlock1.bUseSpecularMap)
		if(ciBlock1.bUseSpecularMap)
			vSpecularColor += texture(uSpecularMap, vTexCoord0.st) * uLights[i].specular * fSpecular;
		else
			vSpecularColor += uLights[i].specular * fSpecular; 
	}

	// output colors to buffer
	oColor.rgb = ciBlock1.bShowNormals ? vSurfaceNormal : (vDiffuseColor + vSpecularColor).rgb;
	oColor.a = 1.0;
}
