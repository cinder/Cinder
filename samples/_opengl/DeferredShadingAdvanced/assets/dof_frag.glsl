// Reworked from Alex Fraser's sweet tutorial here:
// http://phatcore.com/alex/?p=227

#version 400 core

uniform float		uAspect;
uniform float		uBias;
uniform float		uOpacity;
uniform sampler2D	uSampler;
uniform sampler2D	uSamplerDepth;

in Vertex
{
	vec2 	uv;
} vertex;

out vec4 	oColor;

vec4 bokeh( float depth, vec2 offset, inout float influence ) 
{
	float iDepth	= 0.0;
	float contrib	= 0.0;
	vec4 color		= texture( uSampler, vertex.uv + offset );

	if ( color.rgb == vec3( 0.0 ) ) {
		return vec4( 0.0 );
	}

	iDepth = texture( uSamplerDepth, vertex.uv + offset ).r;
	if ( iDepth < depth ) {
		contrib = abs( ( iDepth - 1.0f ) / 1.0 );
	} else {
		contrib = 1.0;
	}

	influence += contrib;
	return color * contrib;
}

void main( void )
{
	float depth		= texture( uSamplerDepth, vertex.uv ).r;
	vec2 sz			= vec2( abs( ( depth - 1.0f ) / 1.0 ) ) * vec2( 1.0, uAspect ) * uBias;
	float influence = 0.000001;

	vec4 sum		= vec4( 0.0 );
	sum				+= bokeh( depth, vec2(  0.158509, -0.884836) * sz, influence );
	sum				+= bokeh( depth, vec2(  0.792547, -0.424181) * sz, influence );
	sum 			+= bokeh( depth, vec2(  0.769421,  0.250000) * sz, influence );
	sum 			+= bokeh( depth, vec2(  0.391857,  0.809017) * sz, influence );
	sum 			+= bokeh( depth, vec2( -0.391857,  0.809017) * sz, influence );
	sum 			+= bokeh( depth, vec2( -0.769421,  0.250000) * sz, influence );
	sum 			+= bokeh( depth, vec2( -0.158509, -0.884836) * sz, influence );
	sum 			+= bokeh( depth, vec2( -0.792547, -0.424181) * sz, influence );
	sum 			+= bokeh( depth, vec2(  0.951056, -0.309017) * sz, influence );
	sum 			+= bokeh( depth, vec2( -0.587785,  0.809017) * sz, influence );
	sum 			+= bokeh( depth, vec2(  0.317019, -0.769672) * sz, influence );
	sum 			+= bokeh( depth, vec2(  0.829966,  0.063661) * sz, influence );
	sum 			+= bokeh( depth, vec2(  0.195928,  0.809017) * sz, influence );
	sum 			+= bokeh( depth, vec2( -0.708876,  0.436339) * sz, influence );
	sum 			+= bokeh( depth, vec2( -0.317019, -0.769672) * sz, influence );
   
	oColor			= mix( texture( uSampler, vertex.uv ), sum / influence, vec4( uOpacity ) );
}
 