#version 330 core

uniform sampler2D uTex0;

smooth in vec2 vTexCoord;
smooth in vec4 vColor;

out vec4 oColor;

void main(void) {
	vec4 color = texture( uTex0, vTexCoord );
	oColor = color * vColor;
}