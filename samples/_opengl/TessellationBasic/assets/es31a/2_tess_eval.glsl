#version 310 es
#extension GL_ANDROID_extension_pack_es31a : require

layout( triangles, equal_spacing, ccw ) in;

in vec4 TcsColor[];

out vec4 TesColor;

uniform mat4 ciModelViewProjection;

void main()
{
	vec3 p0 = gl_TessCoord.x * gl_in[0].gl_Position.xyz; // x is one corner
	vec3 p1 = gl_TessCoord.y * gl_in[1].gl_Position.xyz; // y is the 2nd corner
	vec3 p2 = gl_TessCoord.z * gl_in[2].gl_Position.xyz; // z is the 3rd corner (ignored when using quads)
	gl_Position = ciModelViewProjection * vec4( p0 + p1 + p2, 1.0 );

	vec4 c0 = gl_TessCoord.x * TcsColor[0];
	vec4 c1 = gl_TessCoord.y * TcsColor[1];
	vec4 c2 = gl_TessCoord.z * TcsColor[2];
	TesColor = c0 + c1 + c2;
}
