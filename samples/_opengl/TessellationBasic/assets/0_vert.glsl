#version 400

in vec4 ciPosition;
in vec4 ciColor;

uniform float uRadius;

out vec3 ControlPointWorld;
out vec4 VsColor;

void main () {
	ControlPointWorld = ciPosition.xyz * uRadius; // control points out == vertex points in
	VsColor = ciColor;
}