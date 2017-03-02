  #include "hash.glsl"

float noise( in vec2 x )
{
	vec2 p = floor( x );
	vec2 f = fract( x );
	f = f * f * ( 3.0 - 2.0 * f );
	float n = p.x + p.y * 57.0;
	float res = mix( mix( hash( n + 0.0 ), hash( n + 1.0 ), f.x ), mix( hash( n + 57.0 ), hash( n + 58.0), f.x ), f.y );
	return res;
}
