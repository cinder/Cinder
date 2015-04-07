#include "vertex_in.glsl"
#include "decode.glsl"

// Horizon-Based Ambient Occlusion
// http://rdimitrov.twistedsanity.net/HBAO_SIGGRAPH08.pdf

uniform sampler2D uSamplerNormal;

const int	kNumSampleDirections	= 8;
const int	kNumSampleSteps			= 4;
const float	kPi						= 3.1415926535897932384626433832795;
const float	kPiTwo					= 2.0 * kPi;
const float	kPiHalf					= kPi * 0.5;
const float	kSampleRadius			= 0.5;
const float kSampleStep				= 0.0005;
const float kStepAngle				= kPiTwo / kNumSampleDirections;
const float kStrength				= 1.3;
const float kTangentBias			= 0.05;

out vec4 	oColor;

void main( void )
{
    vec3 position		= decodePosition( vertex.uv ).xyz;
    vec3 normal			= decodeNormal( texture( uSamplerNormal, vertex.uv ).xy );
    float sum			= 0.0;
	float t				= 0.0;
    for ( int i = 0; i < kNumSampleDirections; ++i, t += kStepAngle ) {
        vec2 a			= vec2( cos( t ), sin( t ) );
        float tangent	= acos( dot( vec3( a, 0.0 ), normal ) ) - kPiHalf + kTangentBias;
        float h			= tangent;
        vec3 d1		= vec3( 0.0 );
        for ( int j = 0; j < kNumSampleSteps; ++j ) {
            vec3 d0 = decodePosition( vertex.uv + ( float( j + 1 ) * kSampleStep * a ) ).xyz - position.xyz;
            if ( length( d0 ) < kSampleRadius ) {
                h		= max( h, atan( d0.z / length( d0.xy ) ) );
                d1		= d0;
            }
        }
        sum				+= 1.0 - clamp( ( 1.0 / ( 1.0 + length( d1 ) ) ) * ( sin( h ) - sin( tangent ) ), 0.0, 1.0 ) * kStrength;
    }
    oColor				= vec4( vec3( sum / kNumSampleDirections ), 1.0 );
}
 