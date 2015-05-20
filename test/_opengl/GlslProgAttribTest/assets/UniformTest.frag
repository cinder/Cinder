precision highp float;

uniform vec4 uColors[3];

in vec2 TexCoord0;
out vec4 oColor;

void main() {
	oColor = uColors[int(TexCoord0.s)];
}