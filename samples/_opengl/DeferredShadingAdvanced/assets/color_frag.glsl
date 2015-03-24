#version 330 core

uniform float		uBlend;
uniform float		uColorOffset;
uniform float		uContrast;
uniform float		uMultiply;
uniform vec4		uSaturation;
uniform sampler2D	uSampler;

in Vertex
{
	vec2 	uv;
} vertex;

out vec4 	oColor;

void main( void )
{
	vec4 color = vec4( vec3( 0.0 ), 1.0 );
	if ( uColorOffset != 0.0 ) {
		color.r = texture( uSampler, vec2( vertex.uv.x + uColorOffset,	vertex.uv.y ) ).r;
		color.g = texture( uSampler, vec2( vertex.uv.x + 0.0,			vertex.uv.y ) ).g;
		color.b = texture( uSampler, vec2( vertex.uv.x - uColorOffset,	vertex.uv.y ) ).b;
	}
	color *= uSaturation;
	if ( uContrast != 1.0 ) {
		color = clamp( color * 0.5 + 0.5 * color * color * uContrast, 0.0, 1.0 );
	}
	if ( uMultiply != 1.0 ) {
		color *= 0.5 + 0.5 * uMultiply * vertex.uv.x * vertex.uv.y * ( 1.0 - vertex.uv.x ) * ( 1.0 - vertex.uv.y );
	}
	if ( uBlend < 1.0 ) {
		color = mix( texture( uSampler, vertex.uv ), color, uBlend );
	}
	oColor = color;
}
