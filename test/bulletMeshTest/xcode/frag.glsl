varying vec3 normal, lightDir, eyeVec;

void main (void)
{
	vec4 final_color = 
	(gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient) + 
	(gl_LightSource[0].ambient * gl_FrontMaterial.ambient);
	
	vec3 N = normalize(normal);
	vec3 L = normalize(lightDir);
	
	float lambertTerm = dot(N,L);
	
	if(lambertTerm > 0.0)
	{
		final_color += gl_LightSource[0].diffuse * 
		               gl_FrontMaterial.diffuse * 
					   lambertTerm;
	} else {
		final_color += gl_LightSource[0].diffuse * 
		               gl_FrontMaterial.diffuse * 
					   -lambertTerm;
	}
		
	vec3 E = normalize(eyeVec);
	vec3 R = reflect(-L, N);
	float specular = pow( max(dot(R, E), 0.0), 
					 gl_FrontMaterial.shininess );
	final_color += gl_LightSource[0].specular * 
				   gl_FrontMaterial.specular * 
				   specular;

	gl_FragColor = final_color;			
}