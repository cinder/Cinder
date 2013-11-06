#version 110

varying vec4		vVertex;
varying vec3		vNormal;
varying vec3		vTangent;
varying vec3		vBiTangent;

uniform	sampler2D	uDiffuseMap;
uniform	sampler2D	uSpecularMap;
uniform	sampler2D	uNormalMap;

uniform bool		bShowNormals;

uniform bool		bUseDiffuseMap;
uniform bool		bUseSpecularMap;
uniform bool		bUseNormalMap;

void main()
{
	// calculate view space surface normal and modify it using information from the normal map
	vec3	vMappedNormal = texture2D(uNormalMap, gl_TexCoord[0].st).rgb * 2.0 - 1.0;
	vec3	vSurfaceNormal = bUseNormalMap ? normalize((vTangent * vMappedNormal.x) + (vBiTangent * vMappedNormal.y) + (vNormal * vMappedNormal.z)) : vNormal;	

	// calculate view space light vectors
	vec3	vToLight = normalize(gl_LightSource[0].position.xyz - vVertex.xyz);   
	vec3	vToCamera = normalize(-vVertex.xyz); 
	vec3	vReflect = normalize(-reflect(vToLight,vSurfaceNormal));

	// calculate ambient term
	float	fAmbient = bUseDiffuseMap ? 0.5 : 0.0;

	// calculate diffuse term
	float	fDiffuse = max(dot(vSurfaceNormal,vToLight), 0.0);
	fDiffuse = clamp(fDiffuse, fAmbient, 1.0);  

	// calculate specular term
	float	fSpecularPower = 100.0;
	float	fSpecular = pow( max( dot(vReflect, vToCamera), 0.0), fSpecularPower );
	fSpecular = clamp(fSpecular, 0.0, 1.0);

	// calculate final colors
	vec4	vDiffuseColor	= gl_LightSource[0].diffuse * fDiffuse; 
	vec4	vSpecularColor	= gl_LightSource[0].specular * fSpecular; 

	if(bUseDiffuseMap)
		vDiffuseColor *= texture2D(uDiffuseMap, gl_TexCoord[0].st);

	if(bUseSpecularMap)
		vSpecularColor *= texture2D(uSpecularMap, gl_TexCoord[0].st);

	// output colors to buffer
	gl_FragColor.rgb = bShowNormals ? vSurfaceNormal : (vDiffuseColor + vSpecularColor).rgb;
	gl_FragColor.a = 1.0;
}