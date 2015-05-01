#version 150

uniform int uCheckerFrequency;

in vec4		Color;
in vec3		Normal;
in vec2		TexCoord;

out vec4 	oColor;

// Based on OpenGL Programming Guide (8th edition), p 457-459.
float checkered( in vec2 uv, in int freq )
{
	vec2 checker = fract( uv * freq );
	vec2 edge = fwidth( uv ) * freq;
	float mx = max( edge.x, edge.y );

	vec2 pattern = smoothstep( vec2(0.5), vec2(0.5) + edge, checker );
	pattern += 1.0 - smoothstep( vec2(0.0), edge, checker );

	float factor = pattern.x * pattern.y + ( 1.0 - pattern.x ) * ( 1.0 - pattern.y );
	return mix( factor, 0.5, smoothstep( 0.0, 0.75, mx ) );
}

void main( void )
{
	vec3 normal = normalize( -Normal );
	float diffuse = max( dot( normal, vec3( 0, 0, -1 ) ), 0 );
	oColor = vec4( vec3( abs(diffuse) ) * vec3( checkered( TexCoord.xy, uCheckerFrequency ) ), 1.0 );
}