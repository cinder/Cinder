#version 120
#extension GL_EXT_bindable_uniform: enable
#extension GL_EXT_draw_instanced : enable

bindable uniform mat4	model_matrix[1024];

varying vec4	V;
varying vec3	N;

void main()
{
	// based on the instance ID, determine the position of this vertex 
	V = gl_ModelViewMatrix * model_matrix[gl_InstanceID] * gl_Vertex;

	// do the same for the normal vector (note: this calculation is only correct if your model is NOT scaled!)
	N = normalize( gl_NormalMatrix * vec3( model_matrix[gl_InstanceID] * vec4( gl_Normal, 0.0 ) ) );
	
	gl_Position = gl_ProjectionMatrix * V;
}
