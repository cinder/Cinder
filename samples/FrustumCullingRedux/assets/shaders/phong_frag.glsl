#version 110

uniform sampler2D tex0;	// diffuse map (disabled in this demo)
uniform sampler2D tex1;	// normal map (disabled in this demo)
uniform sampler2D tex2;	// specular map (disabled in this demo)

uniform int numLights;

varying vec3 v;
varying vec3 N;

void main()
{		
	vec2 uv = gl_TexCoord[0].xy;
	vec4 clr = vec4(0.0, 0.0, 0.0, 1.0);
	
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	
	// if using a normal map, use the following lines of code:
	//vec3 normal = (texture2D(tex1, uv).rgb - vec3(0.5)) * vec3(2.0);
	//normal = normalize(N - normal);

	// otherwise:
	vec3 normal = normalize(N);
	
	for(int i=0;i<numLights;++i) {		
		//
		vec3 L = normalize(gl_LightSource[i].position.xyz - v);   
		vec3 E = normalize(-v); 
		vec3 R = normalize(reflect(-L,normal)); 
		
		// ambient term 
		ambient = gl_FrontLightProduct[i].ambient;// * texture2D(tex0, uv); 
		//clr += ambient;   

		// diffuse term		
		diffuse = gl_FrontLightProduct[i].diffuse;// * texture2D(tex0, uv); 
		diffuse *= max(dot(normal,L), 0.0);
		//diffuse = clamp(diffuse, 0.0, 1.0); 
		clr += diffuse;  

		// specular term
		specular = gl_FrontLightProduct[i].specular;// * texture2D(tex2, uv); 
		specular *= pow(max(dot(R,E),0.0), gl_FrontMaterial.shininess);
		specular = clamp(specular, 0.0, 1.0);
		clr += specular;
	}

	// final color
	gl_FragColor = clr;
}