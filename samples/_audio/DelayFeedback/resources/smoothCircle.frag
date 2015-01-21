#version 150

uniform float uRadius;

in vec4		ioColor;
in vec2		ioTexCoord;

out vec4	oFragColor;

void main()
{
	const float stepLength = 0.25;

	float stepRadius = uRadius - stepLength;
	float d = length( ioTexCoord.st );

	if( d > uRadius )
		discard;
	else if( d > stepRadius ) {
		vec4 color = ioColor;
		color.a *= smoothstep( uRadius, stepRadius, d );
		oFragColor = color;
	}
	else
		oFragColor = ioColor;
}