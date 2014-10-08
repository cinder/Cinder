#version 400

layout( triangles, equal_spacing, ccw ) in;

in vec3 EvaluationPointWorld[];
in vec4 TcsColor[];

out vec4 TesColor;

uniform mat4 ciModelViewProjection;

// gl_TessCoord is location within the patch
// (barycentric for triangles, UV for quads)
void main ()
{
	vec3 p0 = gl_TessCoord.x * EvaluationPointWorld[0]; // x is one corner
	vec3 p1 = gl_TessCoord.y * EvaluationPointWorld[1]; // y is the 2nd corner
	vec3 p2 = gl_TessCoord.z * EvaluationPointWorld[2]; // z is the 3rd corner (ignored when using quads)
	gl_Position = ciModelViewProjection * vec4( p0 + p1 + p2, 1.0 );
	
	vec4 c0 = gl_TessCoord.x * TcsColor[0];
	vec4 c1 = gl_TessCoord.y * TcsColor[1];
	vec4 c2 = gl_TessCoord.z * TcsColor[2];
	TesColor = c0 + c1 + c2;
}