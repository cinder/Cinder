#version 130

uniform sampler2D	tex0;
uniform vec4		clr_left;
uniform vec4		clr_right;

in vec4 vColor;
in vec2 vTexCoord0;

out vec4 oColor;

// find the correct half of the texture to sample (side-by-side FBO)
const vec4	kOffset = vec4(0.5, 1.0, 0.5, 0.0);

void main()
{
	oColor = clr_left * texture( tex0, vTexCoord0 * kOffset.xy ) 
		+ clr_right * texture( tex0, vTexCoord0 * kOffset.xy + kOffset.zw );
}