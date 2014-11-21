#version 300 es

uniform int uCheckerFrequency;

in highp vec4		Color;
in highp vec3		Normal;
in highp vec2		TexCoord;

out highp vec4 	oColor;

// This is due to GPU Gems Ch. 25: Fast Filter-Width Estimates with Texture Maps
highp float checker( highp vec2 uv, int freq )
{
	uv *= vec2( freq );
	highp vec2 fw = fwidth( uv );
	highp float width = max( fw.s, fw.t );
	highp vec2 p0 = vec2( uv - 0.5 * width ), p1 = vec2( uv + 0.5 * width );
	highp vec2 ip0 = floor(p0 / 2.0) + 2.0 * max( p0 / 2.0 - floor( p0 / 2.0 ) - 0.5, 0.0 );
	highp vec2 ip1 = floor(p1 / 2.0) + 2.0 * max( p1 / 2.0 - floor( p1 / 2.0 ) - 0.5, 0.0 );
	highp vec2 i = (ip1 - ip0) / width;
	return i.x * i.y + (1.0 - i.x) * (1.0 - i.y);
}

void main( void )
{
	highp vec3 normal = normalize( -Normal );
	highp float diffuse = max( dot( normal, vec3( 0, 0, -1.0 ) ), 0.0 );
	oColor = vec4( vec3( abs(diffuse) ) * vec3( checker( TexCoord.xy, uCheckerFrequency ) ), 1.0 );
}