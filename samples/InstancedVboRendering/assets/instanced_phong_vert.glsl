#version 120
#extension GL_EXT_bindable_uniform: enable
#extension GL_EXT_draw_instanced : enable

bindable uniform mat4	offset[1024];

varying vec3	V;
varying vec3	N;

void main()
{
	// based on the instance ID, determine the position of this hexagon 
	vec4 position = offset[gl_InstanceID] * gl_Vertex;

	// pass the vertex position to the fragment shader
	V = vec3(gl_ModelViewMatrix * position);
	// pass the normal to the fragment shader      
	N = normalize(gl_NormalMatrix * gl_Normal);
	
	gl_Position = gl_ModelViewProjectionMatrix * position;
}
