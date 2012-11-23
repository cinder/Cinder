#version 120
#extension GL_EXT_draw_instanced : enable

attribute mat4	model_matrix;

varying vec4	V;
varying vec3	N;

void main()
{
	// based on the instance ID, determine the position of this vertex 
	V = gl_ModelViewMatrix * model_matrix * gl_Vertex;

	// do the same for the normal vector (note: this calculation is only correct if your model is NOT scaled!)
	N = normalize( gl_NormalMatrix * vec3( model_matrix * vec4( gl_Normal, 0.0 ) ) );
	
	gl_Position = gl_ProjectionMatrix * V;
}
