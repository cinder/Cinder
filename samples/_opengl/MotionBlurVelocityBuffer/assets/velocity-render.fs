#version 150 core

uniform sampler2D uTex0;

out vec4 oColor;

smooth in vec2 vTexCoord;

void main()
{
	vec4 color = vec4(1.0);
	color.rg = texture( uTex0, vTexCoord ).rg * 255.0 + vec2(0.5);

	oColor = color;
	// oColor = texture( uTex0, vTexCoord ).rgba;
}
