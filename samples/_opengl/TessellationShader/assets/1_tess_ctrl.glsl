#version 400

// number of control points in patch
layout (vertices = 3) out;

in vec3 controlpoint_world[];
in  vec4 vs_color[];
out vec4 tcs_color[];

// To evaluation shader. will be used to guide positioning of generated points
out vec3 evaluationpoint_world[];

uniform float tessLevelInner = 4.0; // controlled by keyboard buttons
uniform float tessLevelOuter = 4.0; // controlled by keyboard buttons

void main () {
	evaluationpoint_world[gl_InvocationID] = controlpoint_world[gl_InvocationID];
	tcs_color[gl_InvocationID] = vs_color [gl_InvocationID];
	
	// Calculate the tessellation levels
	gl_TessLevelInner[0] = tessLevelInner; // number of nested primitives to generate
	gl_TessLevelOuter[0] = tessLevelOuter; // times to subdivide first side
	gl_TessLevelOuter[1] = tessLevelOuter; // times to subdivide second side
	gl_TessLevelOuter[2] = tessLevelOuter; // times to subdivide third side
}