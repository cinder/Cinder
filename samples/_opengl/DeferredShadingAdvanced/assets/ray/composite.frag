#include "../common/vertex_in.glsl"

uniform vec2		uPixel;
uniform sampler2D	uSamplerRay;
uniform sampler2D	uSamplerColor;

layout (location = 0) out vec4 oColor;

void main( void )
{
	vec2 o		= uPixel * 0.5;
	vec3 sum	= vec3( 0.0 );
	sum			+= texture( uSamplerRay, vertex.uv + vec2( -o.x,  o.y ) ).rgb;
	sum			+= texture( uSamplerRay, vertex.uv + vec2( -o.x, -o.y ) ).rgb;
	sum			+= texture( uSamplerRay, vertex.uv + vec2(  o.x,  o.y ) ).rgb;
	sum			+= texture( uSamplerRay, vertex.uv + vec2(  o.x, -o.y ) ).rgb;

	oColor		= texture( uSamplerColor, vertex.uv );
	oColor.rgb	+= sum * 0.25;
}
