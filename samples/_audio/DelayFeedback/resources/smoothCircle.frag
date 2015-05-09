#version 110

uniform float uRadius;


void main()
{
	const float stepLength = 0.25;

	float stepRadius = uRadius - stepLength;
	float d = length( gl_TexCoord[0].st );

	if( d > uRadius )
		discard;
	else if( d > stepRadius ) {
		vec4 color = gl_Color;
		color.a *= smoothstep( uRadius, stepRadius, d );
		gl_FragColor = color;
	}
	else
		gl_FragColor = gl_Color;
}