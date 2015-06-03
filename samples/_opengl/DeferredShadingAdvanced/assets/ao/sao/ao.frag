#include "../../common/pi.glsl"

// SAO http://graphics.cs.williams.edu/papers/SAOHPG12/

uniform vec4		uProj;
uniform float		uProjScale;
uniform sampler2D	uSampler;

const float	kRadius			= 1.75;
const float kRadiusSquared	= kRadius * kRadius;

const float kBias			= 0.2;
const float kEpsilon		= 0.01;
const int	kNumSamples		= 9; //31; // Use these higher values on fast GPUs
const int	kNumSpiralTurns	= 5; //11; // Use prime numbers so the same texel is never sampled twice
const float	kStrength		= 1.25;

layout (location = 0) out vec4 oColor;

vec3 unpackPosition( vec2 ss, float z )
{
	return vec3( ( ss.xy * uProj.xy + uProj.zw ) * z, z );
}

void main( void )
{
	oColor			= vec4( vec3( 0.0 ), 1.0 );
	ivec2 ss		= ivec2( gl_FragCoord.xy ) * ivec2( 2 );
	float a			= float( ( 3 * ss.x ^ ss.y + ss.x * ss.y ) * 10 );
	vec3 position	= unpackPosition( vec2( ss ) + vec2( 0.5 ), texelFetch( uSampler, ss, 0 ).r );
	oColor.g		= position.z;
	vec3 N			= normalize( cross( dFdy( position ), dFdx( position ) ) );
	float radius	= -uProjScale * kRadius / position.z;

	float sum		= 0.0;
	for ( int i = 0; i < kNumSamples; ++i ) {
		float r		= float( float( i ) + 0.5 ) * ( 1.0 / float( kNumSamples ) );
		float angle	= r * ( float( kNumSpiralTurns ) * kPiTwo ) + a;
		vec2 offset	= vec2( cos( angle ), sin( angle ) );
		r			*= radius;
		int level	= clamp( int( floor( log2( r ) ) ) - 3, 0, 5 );
		ivec2 ssOff = ivec2( r * offset ) + ss;
		ivec2 mip	= clamp( ssOff >> level, ivec2( 0 ), textureSize( uSampler, level ) - ivec2( 1 ) );
		vec3 occ	= vec3( 0.0, 0.0, texelFetch( uSampler, mip, level ).r );
		occ			= unpackPosition( vec2( ssOff ) + vec2( 0.5 ), occ.z );
		vec3 v		= occ - position;
		float vv	= dot( v, v );
		float vn	= dot( v, N );
		float f		= max( kRadiusSquared - vv, 0.0 );
		sum			+= f * f * f * max( ( vn - kBias ) / ( kEpsilon + vv ), 0.0 );
	}
	
	float v			= max( 0.0, 1.0 - sum * ( kRadius / pow( kRadius, 6.0f ) ) * ( kRadius / float( kNumSamples ) ) );
	if ( abs( dFdx( position.z ) ) < 0.02 ) {
		v			-= dFdx( v ) * ( float( ss.x & 1 ) - 0.5 );
	}
	if ( abs( dFdy( position.z ) ) < 0.02 ) {
		v			-= dFdy( v ) * ( float( ss.y & 1 ) - 0.5 );
	}

	oColor.r		= v * kStrength + 1.0 - kStrength;
}
