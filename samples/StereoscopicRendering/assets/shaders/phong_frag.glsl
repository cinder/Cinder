#version 110

uniform sampler2D	tex0;

varying vec3 v;
varying vec3 N;
varying vec3 r;

void main()
{	
	const vec4	ambient = vec4(0.1, 0.1, 0.1, 1);
	const vec4	specular = vec4(0.7, 0.7, 0.4, 1);
	const float shinyness = 50.0;
	
	vec3 L = normalize(gl_LightSource[0].position.xyz - v);   
	vec3 E = normalize(-v); 
	vec3 R = normalize(-reflect(L,N));  

	// ambient term 
	vec4 Iamb = ambient;    

	// diffuse term
	vec4 Idiff = gl_Color; // * texture2D( tex0, gl_TexCoord[0].st);
	Idiff *= max(dot(N,L), 0.0);
	Idiff = clamp(Idiff, 0.0, 1.0);     

	// specular term
	vec4 Ispec = specular; 
	Ispec *= pow(max(dot(R,E),0.0), shinyness);
	Ispec = clamp(Ispec, 0.0, 1.0); 

	// reflection term (fake ground reflection)
	float m = 2.0 * sqrt(r.x * r.x + r.y * r.y + (r.z + 1.0) * (r.z + 1.0) );
	float f = pow(0.5 - r.y / m, 5.0);
	vec4 Irefl = vec4(f, f, f, 1.0) * specular;

	// final color 
	gl_FragColor = Iamb + Irefl + Idiff + Ispec;	
}