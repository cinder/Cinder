#include "vertex_out.glsl"

uniform mat4 	ciModelViewProjection;

in vec4 		ciPosition;
in vec4 		ciTexCoord0;

void main()
{
    vertex.uv	= ciTexCoord0.st;
    gl_Position = ciModelViewProjection * ciPosition;
}