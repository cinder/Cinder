#version 330 core

// Reworked from Alex Fraser's sweet tutorial here:
// http://phatcore.com/alex/?p=227

const float	kBias		= 0.01;
const float	kDepthPlane	= 1.0;
const float	kOpacity	= 1.0;

uniform float		uAspect;
uniform float		uNear;
uniform sampler2D	uSampler;
uniform sampler2D	uSamplerDepth;

in Vertex
{
	vec2 	uv;
} vertex;

out vec4 	oColor;

vec4 bokeh( float depth, float focalDepth, vec2 offset, inout float influence ) 
{
	float iDepth	= 0.0;
	float contrib	= 0.0;
	vec4 color		= texture( uSampler, vertex.uv + offset );

	iDepth = texture( uSamplerDepth, vertex.uv + offset ).r;
	if ( iDepth < depth ) {
		contrib = distance( iDepth, focalDepth ) / 1.0;
	} else {
		contrib = 1.0;
	}

	influence += contrib;
	return color * contrib;
}

void main( void )
{
	float focalDepth	= 1.0 - uNear / kDepthPlane;
	float depth			= texture( uSamplerDepth, vertex.uv ).r;
	vec2 sz				= vec2( kBias * distance( depth, kDepthPlane ) / 1.0 ) * vec2( 1.0, uAspect );
	float influence		= 0.000001;
	vec4 sum			= vec4( 0.0 );

	sum += bokeh( depth, focalDepth, vec2(  0.158509, -0.884836 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2(  0.475528, -0.654508 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2(  0.792547, -0.424181 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2(  0.890511, -0.122678 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2(  0.769421,  0.250000 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2(  0.648330,  0.622678 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2(  0.391857,  0.809017 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2( -0.000000,  0.809017 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2( -0.391857,  0.809017 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2( -0.648331,  0.622678 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2( -0.769421,  0.250000 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2( -0.890511, -0.122678 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2( -0.158509, -0.884836 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2( -0.475528, -0.654509 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2( -0.792547, -0.424181 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2(  0.000000, -1.000000 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2(  0.951056, -0.309017 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2(  0.587785,  0.809017 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2( -0.587785,  0.809017 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2( -0.951057, -0.309017 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2(  0.317019, -0.769672 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2(  0.634038, -0.539345 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2(  0.829966,  0.063661 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2(  0.708876,  0.436339 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2(  0.195928,  0.809017 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2( -0.195929,  0.809017 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2( -0.708876,  0.436339 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2( -0.829966,  0.063661 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2( -0.317019, -0.769672 ) * sz, influence );
    sum += bokeh( depth, focalDepth, vec2( -0.634038, -0.539345 ) * sz, influence );
   
	oColor = mix( texture( uSampler, vertex.uv ), sum / influence, vec4( vec3( kOpacity ), 1.0 ) );
}
 