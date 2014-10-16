#version 400

// number of control points in patch
layout (vertices = 3) out;

in vec3 ControlPointWorld[];
in vec4 VsColor[];
out vec4 TcsColor[];

// To evaluation shader. will be used to guide positioning of generated points
out vec3 EvaluationPointWorld[];

uniform float uTessLevelInner = 4.0;
uniform float uTessLevelOuter = 4.0;

void main ()
{
	EvaluationPointWorld[gl_InvocationID] = ControlPointWorld[gl_InvocationID];
	TcsColor[gl_InvocationID] = VsColor[gl_InvocationID];
	
	// Calculate the tessellation levels
	gl_TessLevelInner[0] = uTessLevelInner; // number of nested primitives to generate
	gl_TessLevelOuter[0] = uTessLevelOuter; // times to subdivide first side
	gl_TessLevelOuter[1] = uTessLevelOuter; // times to subdivide second side
	gl_TessLevelOuter[2] = uTessLevelOuter; // times to subdivide third side
}