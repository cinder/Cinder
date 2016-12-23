#version 310 es
#extension GL_ANDROID_extension_pack_es31a : require

layout(vertices = 3) out;

in vec4 VsColor[];
out vec4 TcsColor[];

uniform float uTessLevelInner;
uniform float uTessLevelOuter;

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	TcsColor[gl_InvocationID] = VsColor[gl_InvocationID];

	gl_TessLevelInner[0] = uTessLevelInner; // number of nested primitives to generate
	gl_TessLevelOuter[0] = uTessLevelOuter; // times to subdivide first side
	gl_TessLevelOuter[1] = uTessLevelOuter; // times to subdivide second side
	gl_TessLevelOuter[2] = uTessLevelOuter; // times to subdivide third side
}
