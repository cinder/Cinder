#version 120

uniform sampler2D tex0;
uniform vec2 sampleDivisor;

void main()
{
	vec2 samplePos = gl_TexCoord[0].st - mod( gl_TexCoord[0].st, sampleDivisor );
	gl_FragColor = texture2D( tex0, samplePos );
}