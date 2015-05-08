#include "../common/vertex_in.glsl"

const float	kAttenuation	= 1.7;
const float	kScale			= 0.012;

uniform vec2		uAxis;
uniform sampler2D	uSampler;

layout (location = 0) out vec4 oColor;

void main( void )
{
	vec2 sz = uAxis * kScale;

	vec4 sum = vec4( 0.0 );
	sum += texture( uSampler, vertex.uv + sz * -1.0 ) * 0.009167927656011385;
	sum += texture( uSampler, vertex.uv + sz * -0.8 ) * 0.020595286319257878;
	sum += texture( uSampler, vertex.uv + sz * -0.6 ) * 0.038650411513543079;
	sum += texture( uSampler, vertex.uv + sz * -0.4 ) * 0.060594058578763078;
	sum += texture( uSampler, vertex.uv + sz * -0.2 ) * 0.079358891804948081;
	sum += texture( uSampler, vertex.uv + sz *  0.0 ) * 0.086826196862124602;
	sum += texture( uSampler, vertex.uv + sz *  0.2 ) * 0.079358891804948081;
	sum += texture( uSampler, vertex.uv + sz *  0.4 ) * 0.060594058578763078;
	sum += texture( uSampler, vertex.uv + sz *  0.6 ) * 0.038650411513543079;
	sum += texture( uSampler, vertex.uv + sz *  0.8 ) * 0.020595286319257878;
	sum += texture( uSampler, vertex.uv + sz *  1.0 ) * 0.009167927656011385;
	
	oColor = kAttenuation * sum;
}
