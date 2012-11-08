#version 110

uniform sampler2D	tex0;
uniform vec4		clr_left;
uniform vec4		clr_right;

// find the correct half of the texture to sample (side-by-side FBO)
const vec2			halfOffset = vec2(0.5, 1.0);
const vec2			offset = vec2(0.5, 0.0);

void main()
{	
	vec2 st = gl_TexCoord[0].st * halfOffset;

	gl_FragColor = clr_left * texture2D( tex0, st ) + clr_right * texture2D( tex0, st + offset );
}