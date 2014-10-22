#version 150

uniform int uCheckerFrequency;

in vec4		Color;
in vec3		Normal;
in vec2		TexCoord;

out vec4 	oColor;

// This is due to GPU Gems Ch. 25: Fast Filter-Width Estimates with Texture Maps
float checker( vec2 uv, int freq )
{
	uv *= vec2( freq );
	vec2 fw = fwidth( uv );
	float width = max( fw.s, fw.t );
	vec2 p0 = vec2( uv - 0.5 * width ), p1 = vec2( uv + 0.5 * width );
	vec2 ip0 = floor(p0 / 2.0) + 2.0 * max( p0 / 2.0 - floor( p0 / 2.0 ) - 0.5f, 0.f);
	vec2 ip1 = floor(p1 / 2.0) + 2.0 * max( p1 / 2.0 - floor( p1 / 2.0 ) - 0.5f, 0.f);
	vec2 i = (ip1 - ip0) / width;
	return i.x * i.y + (1 - i.x) * (1 - i.y);
}

void main( void )
{
	vec3 normal = normalize( -Normal );
	float diffuse = max( dot( normal, vec3( 0, 0, -1 ) ), 0 );
	oColor = vec4( vec3( abs(diffuse) ) * vec3( checker( TexCoord.xy, uCheckerFrequency ) ), 1.0 );
}