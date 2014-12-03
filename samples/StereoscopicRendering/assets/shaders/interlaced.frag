#version 130

uniform sampler2D	tex0;

uniform vec2		window_origin;
uniform vec2		window_size;

in vec4 vColor;
in vec2 vTexCoord0;

out vec4 oColor;

// find the correct half of the texture to sample (over-under FBO)
const vec4	kOffset = vec4(1.0, 0.5, 0.0, 0.5);

void main()
{	
	// find the actual screen coordinate of this fragment
	float y = window_size.y - gl_FragCoord.y + window_origin.y;

	// does the fragment sit on an odd or even line?
	float isOdd = mod( floor(y), 2.0 );

	// sample the correct half of the texture
	oColor = texture( tex0, vTexCoord0 * kOffset.xy + ( isOdd * kOffset.zw ) );
}