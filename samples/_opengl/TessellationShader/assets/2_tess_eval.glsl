#version 400

// triangles, quads, or isolines
layout (triangles, equal_spacing, ccw) in;

in vec3 evaluationpoint_world[];
in vec4 tcs_color[];

out vec4 tes_color;

// could use a displacement map here

uniform mat4 ciModelViewProjection;

// gl_TessCoord is location within the patch
// (barycentric for triangles, UV for quads)

void main () {
	vec3 p0 = gl_TessCoord.x * evaluationpoint_world[0]; // x is one corner
	vec3 p1 = gl_TessCoord.y * evaluationpoint_world[1]; // y is the 2nd corner
	vec3 p2 = gl_TessCoord.z * evaluationpoint_world[2]; // z is the 3rd corner (ignore when using quads)
	gl_Position = ciModelViewProjection * vec4(p0 + p1 + p2, 1.0);
	
	vec4 c0 = gl_TessCoord.x * tcs_color[0];
	vec4 c1 = gl_TessCoord.y * tcs_color[1];
	vec4 c2 = gl_TessCoord.z * tcs_color[2];
	tes_color = c0 + c1 + c2;
}