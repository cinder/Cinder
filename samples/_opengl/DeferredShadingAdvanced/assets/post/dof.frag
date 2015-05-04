#include "../common/vertex_in.glsl"
#include "../common/offset.glsl"

// Reworked from Alex Fraser's sweet tutorial here:
// http://phatcore.com/alex/?p=227

const float	kBias = 2.0;

uniform float		uAspect;
uniform float		uFocalDepth;
uniform float		uNear;
uniform sampler2D	uSamplerColor;
uniform sampler2D	uSamplerDepth;

layout (location = 0) out vec4 oColor;

vec3 bokeh( in float depth, in float depthPlane, in vec2 depthUv, in vec2 uv, in vec2 offset, inout float influence )
{
	float iDepth	= 0.0;
	float contrib	= 0.0;
	vec3 color		= texture( uSamplerColor, uv + offset ).rgb;

	if ( color == vec3( 0.0 ) ) {
		contrib = 0.2;
	} else {
		iDepth = texture( uSamplerDepth, depthUv + offset * ( uv / depthUv ) ).x;
		if ( iDepth < depth ) {
			contrib = distance( iDepth, depthPlane ) / depthPlane;
		} else {
			contrib = 1.0;
		}
	}

	influence += contrib;
	return color * contrib;
}

void main( void )
{
	float depthPlane	= 1.0f - uNear / uFocalDepth;
	vec2 depthUv		= calcTexCoordFromUv( vertex.uv );
	float depth			= texture( uSamplerDepth, depthUv ).x;
	vec2 sz				= vec2( kBias * distance( depth, depthPlane ) / depthPlane ) * vec2( 1.0, uAspect );
	float influence		= 0.000001;
	vec3 sum			= vec3( 0.0 );

	sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2(  0.158509, -0.884836 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2(  0.475528, -0.654508 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2(  0.792547, -0.424181 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2(  0.890511, -0.122678 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2(  0.769421,  0.250000 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2(  0.648330,  0.622678 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2(  0.391857,  0.809017 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2( -0.000000,  0.809017 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2( -0.391857,  0.809017 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2( -0.648331,  0.622678 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2( -0.769421,  0.250000 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2( -0.890511, -0.122678 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2( -0.158509, -0.884836 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2( -0.475528, -0.654509 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2( -0.792547, -0.424181 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2(  0.000000, -1.000000 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2(  0.951056, -0.309017 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2(  0.587785,  0.809017 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2( -0.587785,  0.809017 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2( -0.951057, -0.309017 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2(  0.317019, -0.769672 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2(  0.634038, -0.539345 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2(  0.829966,  0.063661 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2(  0.708876,  0.436339 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2(  0.195928,  0.809017 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2( -0.195929,  0.809017 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2( -0.708876,  0.436339 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2( -0.829966,  0.063661 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2( -0.317019, -0.769672 ) * sz, influence );
    sum += bokeh( depth, depthPlane, depthUv, vertex.uv, vec2( -0.634038, -0.539345 ) * sz, influence );

	oColor = vec4( sum / influence, 1.0 );
}
 