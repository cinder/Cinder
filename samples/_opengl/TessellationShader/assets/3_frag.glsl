#version 400

out vec4 fragcolour;

in vec4 tes_color;

void main () {
	fragcolour = tes_color;
}