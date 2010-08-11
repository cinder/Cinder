#version 120

void main()
{
	// set the first color attachment to green
	gl_FragData[0] = vec4( 0.0, 1.0, 0.0, 1.0 );
	// set the second to blue
	gl_FragData[1] = vec4( 0.0, 0.0, 1.0, 1.0 );
}
