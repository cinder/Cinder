#version 400

uniform mat4 ciModelViewProjection;

in vec4 ciPosition;
in vec4 ciColor;

out vec4 VsPosition;
out vec4 VsColor;

void main ()
{
	VsPosition = ciModelViewProjection * ciPosition; // control points out == vertex points in
	VsColor = ciColor;
}