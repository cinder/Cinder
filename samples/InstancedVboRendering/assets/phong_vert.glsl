#version 120
#extension GL_EXT_draw_instanced : enable

uniform mat4 offset_matrix;

varying vec3 V;
varying vec3 N;

void main()
{
	// based on the instance ID, determine the position of this hexagon 
	float x = mod( gl_InstanceID, 100.0 );
	float y = floor( float(gl_InstanceID) / 100.0 );
	vec4 position = gl_Vertex + offset_matrix * vec4( x, y, mod( y, 2.0 ), 0.0 );

	// pass the vertex position to the fragment shader
	V = vec3(gl_ModelViewMatrix * position);
	// pass the normal to the fragment shader      
	N = normalize(gl_NormalMatrix * gl_Normal);
	
	gl_Position = gl_ModelViewProjectionMatrix * position;
}
