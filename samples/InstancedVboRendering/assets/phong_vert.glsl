#version 120

varying vec4	V;
varying vec3	N;

void main()
{
	// pass the vertex position to the fragment shader
	V = gl_ModelViewMatrix * gl_Vertex;

	// pass the normal to the fragment shader      
	N = normalize(gl_NormalMatrix * gl_Normal);
	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
