#version 330 core

const float	kBlend			= 0.8;
const float	kColorOffset	= 0.0015;
const float	kContrast		= 0.5;
const float	kMultiply		= 24.0;
const vec4	kSaturation		= vec4( 0.992, 1.0, 0.953, 1.0 );

uniform sampler2D	uSampler;

in Vertex
{
	vec2 	uv;
} vertex;

out vec4 	oColor;

void main( void )
{
	vec4 color = vec4( vec3( 0.0 ), 1.0 );
	if ( kColorOffset != 0.0 ) {
		color.r = texture( uSampler, vec2( vertex.uv.x + kColorOffset,	vertex.uv.y ) ).r;
		color.g = texture( uSampler, vec2( vertex.uv.x + 0.0,			vertex.uv.y ) ).g;
		color.b = texture( uSampler, vec2( vertex.uv.x - kColorOffset,	vertex.uv.y ) ).b;
	}
	color *= kSaturation;
	if ( kContrast != 1.0 ) {
		color = clamp( color * 0.5 + 0.5 * color * color * kContrast, 0.0, 1.0 );
	}
	if ( kMultiply != 1.0 ) {
		color *= 0.5 + 0.5 * kMultiply * vertex.uv.x * vertex.uv.y * ( 1.0 - vertex.uv.x ) * ( 1.0 - vertex.uv.y );
	}
	if ( kBlend < 1.0 ) {
		color = mix( texture( uSampler, vertex.uv ), color, kBlend );
	}
	oColor = color;
}
