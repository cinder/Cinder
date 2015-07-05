#include "../common/unpack.glsl"
#include "../common/offset.glsl"

const float kBias		= 0.99914;
const float kInfluence	= 0.0625;
const float	kBlurSize	= 0.004;
const float kOpacity	= 0.5;

uniform sampler2DShadow uSampler;
uniform mat4			uProjView;
uniform mat4 			uViewMatrixInverse;

layout (location = 0) out vec4 oColor;

void main( void )
{
	vec2 uv				= calcTexCoordFromFrag( gl_FragCoord.xy );
	vec4 position		= unpackPosition( uv );
	position			= uProjView * uViewMatrixInverse * position;
	vec3 shadowCoord 	= ( position.xyz / position.w ) * 0.5 + 0.5;
	
	float v 			= 1.0;
	float e				= 0.0625;
	if ( position.w > 1.0 ) {
		float d = shadowCoord.z - kBias;
		if ( texture( uSampler, shadowCoord + vec3( vec2( -0.06095261, -0.1337204 )   * kBlurSize, 0.0 ) ) > d ) {
			v -= kInfluence;
		}
		if ( texture( uSampler, shadowCoord + vec3( vec2(  0.4983526,   0.233555 )    * kBlurSize, 0.0 ) ) > d ) {
			v -= kInfluence;
		}
		if ( texture( uSampler, shadowCoord + vec3( vec2( -0.2842098,  -0.5506849 )   * kBlurSize, 0.0 ) ) > d ) {
			v -= kInfluence;
		}
		if ( texture( uSampler, shadowCoord + vec3( vec2(  0.05801121,  0.6332615 )   * kBlurSize, 0.0 ) ) > d ) {
			v -= kInfluence;
		}
		if ( texture( uSampler, shadowCoord + vec3( vec2( -0.5088959,  -0.003537838 ) * kBlurSize, 0.0 ) ) > d ) {
			v -= kInfluence;
		}
		if ( texture( uSampler, shadowCoord + vec3( vec2(  0.4832182,  -0.2853011 )   * kBlurSize, 0.0 ) ) > d ) {
			v -= kInfluence;
		}
		if ( texture( uSampler, shadowCoord + vec3( vec2( -0.8192781,  -0.2787592 )   * kBlurSize, 0.0 ) ) > d ) {
			v -= kInfluence;
		}
		if ( texture( uSampler, shadowCoord + vec3( vec2(  0.1339615,  -0.6042675 )   * kBlurSize, 0.0 ) ) > d ) {
			v -= kInfluence;
		}
		if ( texture( uSampler, shadowCoord + vec3( vec2(  0.5493031,  -0.8009133 )   * kBlurSize, 0.0 ) ) > d ) {
			v -= kInfluence;
		}
		if ( texture( uSampler, shadowCoord + vec3( vec2(  0.9285686,   0.146349 )    * kBlurSize, 0.0 ) ) > d ) {
			v -= kInfluence;
		}
		if ( texture( uSampler, shadowCoord + vec3( vec2( -0.2837186,  -0.9508537 )   * kBlurSize, 0.0 ) ) > d ) {
			v -= kInfluence;
		}
		if ( texture( uSampler, shadowCoord + vec3( vec2(  0.5228189,   0.8005553 )   * kBlurSize, 0.0 ) ) > d ) {
			v -= kInfluence;
		}
		if ( texture( uSampler, shadowCoord + vec3( vec2( -0.4011278,   0.5258422 )   * kBlurSize, 0.0 ) ) > d ) {
			v -= kInfluence;
		}
		if ( texture( uSampler, shadowCoord + vec3( vec2( -0.2490727,   0.9233519 )   * kBlurSize, 0.0 ) ) > d ) {
			v -= kInfluence;
		}
		if ( texture( uSampler, shadowCoord + vec3( vec2( -0.8024328,   0.3718062 )   * kBlurSize, 0.0 ) ) > d ) {
			v -= kInfluence;
		}
		if ( texture( uSampler, shadowCoord + vec3( vec2( -0.6656654,  -0.7041242 )   * kBlurSize, 0.0 ) ) > d ) {
			v -= kInfluence;
		}
		oColor	= vec4( vec3( 0.0 ), v * kOpacity );
	} else {
		oColor	= vec4( 0.0 );
	}
}
