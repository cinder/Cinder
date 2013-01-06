#version 110

uniform sampler2D	tex0;

uniform vec2		window_origin;
uniform vec2		window_size;

// find the correct half of the texture to sample (over-under FBO)
const vec2			halfOffset = vec2(1.0, 0.5);
const vec2			offset = vec2(0.0, 0.5);

void main()
{	
	// find the actual screen coordinate of this fragment
	float y = window_size.y - gl_FragCoord.y + window_origin.y;

	// does the fragment sit on an odd or even line?
	float isOdd = mod( floor(y), 2.0 );

	// sample the correct half of the texture
	gl_FragColor = texture2D( tex0, gl_TexCoord[0].st * halfOffset + ( isOdd * offset ) );
}