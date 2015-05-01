#version 150

uniform sampler2D	uTex0;

uniform vec2		uWindowOrigin;
uniform vec2		uWindowSize;

in vec2             vertTexCoord0;
out vec4            fragColor;

// find the correct half of the texture to sample (side-by-side FBO)
const vec4	kOffset = vec4(0.5, 1.0, 0.5, 0.0);

void main()
{	
	// find the actual screen coordinate of this fragment
	float y = uWindowSize.y - gl_FragCoord.y + uWindowOrigin.y;

	// does the fragment sit on an odd or even line?
	float isOdd = mod( floor(y), 2.0 );

	// sample the correct half of the texture
	fragColor = texture( uTex0, vertTexCoord0 * kOffset.xy + ( isOdd * kOffset.zw ) );
}