#version 100

// inputs passed from the vertex shader
varying highp vec4			vVertex;
varying highp vec3			vNormal;
varying highp vec3			vTangent;
varying highp vec3			vBiTangent;
varying highp vec2			vTexCoord0;

// textures and settings
uniform	sampler2D	uDiffuseMap;
uniform	sampler2D	uSpecularMap;
uniform	sampler2D	uNormalMap;
uniform	sampler2D	uEmmisiveMap;

uniform bool		bShowNormals;
uniform bool		bUseDiffuseMap;
uniform bool		bUseSpecularMap;
uniform bool		bUseNormalMap;
uniform bool		bUseEmmisiveMap;

// light source information
#define MAX_NUM_LIGHTS 16

struct LightSource
{
	highp vec4 position;
	highp vec4 diffuse;
	highp vec4 specular;
};

uniform LightSource	uLights[MAX_NUM_LIGHTS];
uniform int			uNumOfLights;

void main()
{
	// fetch the normal from the normal map
	highp vec3	vMappedNormal = texture2D(uNormalMap, vTexCoord0.st).rgb * 2.0 - 1.0;

	// modify it using the normal & tangents from the 3D mesh (normal mapping)
	highp vec3	vSurfaceNormal = bUseNormalMap ? normalize((vTangent * vMappedNormal.x) + (vBiTangent * vMappedNormal.y) + (vNormal * vMappedNormal.z)) : vNormal;
  
	// apply each of our light sources
	highp vec4	vDiffuseColor	= bUseEmmisiveMap ? texture2D(uEmmisiveMap, vTexCoord0.st) : vec4(0, 0, 0, 1);
	highp vec4	vSpecularColor	= vec4(0, 0, 0, 1);

	highp vec3	vToCamera = normalize(-vVertex.xyz);

	// Use a constant on ES2 since some GPUs do not support
	// loops using uniforms in conditionals
	//for(int i=0; i<uNumOfLights; ++i)
	for(int i=0; i<2; ++i)
	{
		// calculate view space light vectors
		highp vec3	vToLight = normalize(uLights[i].position.xyz - vVertex.xyz); 
		highp vec3	vReflect = normalize(-reflect(vToLight, vSurfaceNormal));

		// calculate diffuse term
		highp float	fDiffuse = max(dot(vSurfaceNormal, vToLight), 0.0);
		fDiffuse = clamp(fDiffuse, 0.1, 1.0);  

		// calculate specular term
		highp float	fSpecularPower = 100.0;
		highp float	fSpecular = pow( max( dot(vReflect, vToCamera), 0.0), fSpecularPower );
		fSpecular = clamp(fSpecular, 0.0, 1.0);

		// calculate final colors
		if(bUseDiffuseMap)
			vDiffuseColor += texture2D(uDiffuseMap, vTexCoord0.st) * uLights[i].diffuse * fDiffuse;
		else
			vDiffuseColor += uLights[i].diffuse * fDiffuse; 

		if(bUseSpecularMap)
			vSpecularColor += texture2D(uSpecularMap, vTexCoord0.st) * uLights[i].specular * fSpecular;
		else
			vSpecularColor += uLights[i].specular * fSpecular; 
	}

	// output colors to buffer
	gl_FragColor.rgb = bShowNormals ? vSurfaceNormal : (vDiffuseColor + vSpecularColor).rgb;
	gl_FragColor.a = 1.0;
}
