#version 400

in vec4 ciPosition;
in vec4 ciColor;

out vec3 controlpoint_world;
out vec4 vs_color;

void main () {
	controlpoint_world = ciPosition.xyz; // control points out == vertex points in
	vs_color = ciColor;
}

