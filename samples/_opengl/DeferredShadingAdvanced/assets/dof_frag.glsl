#version 400 core

uniform float		uBias;
uniform float		uFocalDepth;
uniform float		uRange;
uniform sampler2D	uSampler;
uniform sampler2D	uSamplerDepth;
uniform vec2		uSize;

in Vertex
{
	vec2 	uv;
} vertex;

out vec4 	oColor;

void main( void ) 
{
	float d		= sqrt( pow( uFocalDepth, 2.0 ) + pow( texture( uSamplerDepth, vertex.uv ).a, 2.0 ) );
	float blur	= log( d / uRange ) * uBias * uRange;
	vec2 sz		= vec2( clamp( blur, -uSize.x, uSize.x ), clamp( blur, -uSize.y, uSize.y ) );
	vec4 sum	= vec4( 0.0 );

	sum += texture( uSampler, vertex.uv + -10.0 * sz ) * 0.009167927656011385;
	sum += texture( uSampler, vertex.uv +  -9.0 * sz ) * 0.014053461291849008;
	sum += texture( uSampler, vertex.uv +  -8.0 * sz ) * 0.020595286319257878;
	sum += texture( uSampler, vertex.uv +  -7.0 * sz ) * 0.028855245532226279;
	sum += texture( uSampler, vertex.uv +  -6.0 * sz ) * 0.038650411513543079;
	sum += texture( uSampler, vertex.uv +  -5.0 * sz ) * 0.049494378859311142;
	sum += texture( uSampler, vertex.uv +  -4.0 * sz ) * 0.060594058578763078;
	sum += texture( uSampler, vertex.uv +  -3.0 * sz ) * 0.070921288047096992;
	sum += texture( uSampler, vertex.uv +  -2.0 * sz ) * 0.079358891804948081;
	sum += texture( uSampler, vertex.uv +  -1.0 * sz ) * 0.084895951965930902;
	sum += texture( uSampler, vertex.uv +	0.0 * sz ) * 0.086826196862124602;
	sum += texture( uSampler, vertex.uv +   1.0 * sz ) * 0.084895951965930902;
	sum += texture( uSampler, vertex.uv +   2.0 * sz ) * 0.079358891804948081;
	sum += texture( uSampler, vertex.uv +   3.0 * sz ) * 0.070921288047096992;
	sum += texture( uSampler, vertex.uv +   4.0 * sz ) * 0.060594058578763078;
	sum += texture( uSampler, vertex.uv +   5.0 * sz ) * 0.049494378859311142;
	sum += texture( uSampler, vertex.uv +   6.0 * sz ) * 0.038650411513543079;
	sum += texture( uSampler, vertex.uv +   7.0 * sz ) * 0.028855245532226279;
	sum += texture( uSampler, vertex.uv +   8.0 * sz ) * 0.020595286319257878;
	sum += texture( uSampler, vertex.uv +   9.0 * sz ) * 0.014053461291849008;
	sum += texture( uSampler, vertex.uv +  10.0 * sz ) * 0.009167927656011385;
			
	oColor = sum;
} 
