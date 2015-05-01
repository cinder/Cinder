#version 300 es
precision highp float;

uniform int uCheckerFrequency;

in highp vec4		Color;
in highp vec3		Normal;
in highp vec2		TexCoord;

out highp vec4 	oColor;

// Based on OpenGL Programming Guide (8th edition), p 457-459.
highp float checkered( in vec2 uv, in highp float freq )
{
	highp vec2 checker = fract( uv * freq );
	highp vec2 edge = fwidth( uv ) * freq;
	highp float mx = max( edge.x, edge.y );

	highp vec2 pattern = smoothstep( vec2(0.5), vec2(0.5) + edge, checker );
	pattern += 1.0 - smoothstep( vec2(0.0), edge, checker );

	highp float factor = pattern.x * pattern.y + ( 1.0 - pattern.x ) * ( 1.0 - pattern.y );
	return mix( factor, 0.5, smoothstep( 0.0, 0.75, mx ) );
}

void main( void )
{
	highp vec3 normal = normalize( -Normal );
	highp float diffuse = max( dot( normal, vec3( 0, 0, -1.0 ) ), 0.0 );
	oColor = vec4( vec3( abs(diffuse) ) * vec3( checkered( TexCoord.xy, float(uCheckerFrequency) ) ), 1.0 );
}