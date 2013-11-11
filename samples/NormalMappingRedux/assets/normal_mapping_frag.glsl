#version 110

varying vec4		vVertex;
varying vec3		vNormal;
varying vec3		vTangent;
varying vec3		vBiTangent;

uniform	sampler2D	uDiffuseMap;
uniform	sampler2D	uSpecularMap;
uniform	sampler2D	uNormalMap;
uniform	sampler2D	uEmmisiveMap;

uniform bool		bShowNormals;

uniform bool		bUseDiffuseMap;
uniform bool		bUseSpecularMap;
uniform bool		bUseNormalMap;
uniform bool		bUseEmmisiveMap;

void main()
{
	// fetch the normal from the normal map and modify it using the normal (and tangents) from the 3D mesh
	vec3	vMappedNormal = texture2D(uNormalMap, gl_TexCoord[0].st).rgb * 2.0 - 1.0;
	vec3	vSurfaceNormal = bUseNormalMap ? normalize((vTangent * vMappedNormal.x) + (vBiTangent * vMappedNormal.y) + (vNormal * vMappedNormal.z)) : vNormal;	
 
	vec3	vToCamera = normalize(-vVertex.xyz); 

	// apply each of our light sources
	vec4	vDiffuseColor	= bUseEmmisiveMap ? texture2D(uEmmisiveMap, gl_TexCoord[0].st) : vec4(0, 0, 0, 1);
	vec4	vSpecularColor	= vec4(0, 0, 0, 1);

	for(int i=0; i<2; ++i)
	{
		// calculate view space light vectors (for directional light source)
		vec3	vToLight = normalize(-gl_LightSource[i].position.xyz); 
		vec3	vReflect = normalize(-reflect(vToLight,vSurfaceNormal));

		// calculate diffuse term
		float	fDiffuse = max(dot(vSurfaceNormal,vToLight), 0.0);
		fDiffuse = clamp(fDiffuse, 0.1, 1.0);  

		// calculate specular term
		float	fSpecularPower = 100.0;
		float	fSpecular = pow( max( dot(vReflect, vToCamera), 0.0), fSpecularPower );
		fSpecular = clamp(fSpecular, 0.0, 1.0);

		// calculate final colors
		if(bUseDiffuseMap)
			vDiffuseColor += texture2D(uDiffuseMap, gl_TexCoord[0].st) * gl_LightSource[i].diffuse * fDiffuse;
		else
			vDiffuseColor += gl_LightSource[i].diffuse * fDiffuse; 

		if(bUseSpecularMap)
			vSpecularColor += texture2D(uSpecularMap, gl_TexCoord[0].st) * gl_LightSource[i].specular * fSpecular;
		else
			vSpecularColor += gl_LightSource[i].specular * fSpecular; 
	}

	// output colors to buffer
	gl_FragColor.rgb = bShowNormals ? vSurfaceNormal : (vDiffuseColor + vSpecularColor).rgb;
	gl_FragColor.a = 1.0;
}