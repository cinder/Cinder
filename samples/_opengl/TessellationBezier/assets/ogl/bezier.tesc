#version 400

layout(vertices = 4) out;

uniform int uSubdivisions;

in vec4 VsPosition[];
in vec4 VsColor[];

out vec4 TcsColor[];
out vec4 TcsPosition[];

void main ()
{
	TcsPosition[gl_InvocationID] = VsPosition[gl_InvocationID];
	TcsColor[gl_InvocationID] = VsColor[gl_InvocationID];
	
	gl_TessLevelOuter[0] = 1;
	gl_TessLevelOuter[1] = uSubdivisions;
}