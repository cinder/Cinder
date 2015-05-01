#include "vertex_in.glsl"

uniform float		uAttenuation;
uniform vec2		uSize;
uniform sampler2D	uSampler;

out vec4 	oColor;

void main( void )
{
	vec4 sum = vec4( 0.0 );
	sum += texture( uSampler, vertex.uv + -10.0 * uSize ) * 0.009167927656011385;
	sum += texture( uSampler, vertex.uv +  -9.0 * uSize ) * 0.014053461291849008;
	sum += texture( uSampler, vertex.uv +  -8.0 * uSize ) * 0.020595286319257878;
	sum += texture( uSampler, vertex.uv +  -7.0 * uSize ) * 0.028855245532226279;
	sum += texture( uSampler, vertex.uv +  -6.0 * uSize ) * 0.038650411513543079;
	sum += texture( uSampler, vertex.uv +  -5.0 * uSize ) * 0.049494378859311142;
	sum += texture( uSampler, vertex.uv +  -4.0 * uSize ) * 0.060594058578763078;
	sum += texture( uSampler, vertex.uv +  -3.0 * uSize ) * 0.070921288047096992;
	sum += texture( uSampler, vertex.uv +  -2.0 * uSize ) * 0.079358891804948081;
	sum += texture( uSampler, vertex.uv +  -1.0 * uSize ) * 0.084895951965930902;
	sum += texture( uSampler, vertex.uv +   0.0 * uSize ) * 0.086826196862124602;
	sum += texture( uSampler, vertex.uv +   1.0 * uSize ) * 0.084895951965930902;
	sum += texture( uSampler, vertex.uv +   2.0 * uSize ) * 0.079358891804948081;
	sum += texture( uSampler, vertex.uv +   3.0 * uSize ) * 0.070921288047096992;
	sum += texture( uSampler, vertex.uv +   4.0 * uSize ) * 0.060594058578763078;
	sum += texture( uSampler, vertex.uv +   5.0 * uSize ) * 0.049494378859311142;
	sum += texture( uSampler, vertex.uv +   6.0 * uSize ) * 0.038650411513543079;
	sum += texture( uSampler, vertex.uv +   7.0 * uSize ) * 0.028855245532226279;
	sum += texture( uSampler, vertex.uv +   8.0 * uSize ) * 0.020595286319257878;
	sum += texture( uSampler, vertex.uv +   9.0 * uSize ) * 0.014053461291849008;
	sum += texture( uSampler, vertex.uv +  10.0 * uSize ) * 0.009167927656011385;

	oColor = uAttenuation * sum;
}
