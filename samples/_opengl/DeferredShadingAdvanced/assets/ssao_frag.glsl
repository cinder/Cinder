#version 330 core

uniform float		uFalloff;
uniform float		uOffset;
uniform float		uRadius;
uniform float		uStrength;
uniform sampler2D	uSamplerDepth;
uniform sampler2D	uSamplerNoise;
uniform sampler2D	uSamplerNormal;

in Vertex
{
	vec2 	uv;
} vertex;

out vec4 	oColor;

const float kInvSamples = -0.5 / 20.0;

float rand( vec2 v )
{
    return fract( sin( dot( v, vec2( 12.9898, 78.233 ) ) ) * 43758.5453 );
}

vec3 decodeNormal( vec2 enc )
{
    vec4 n	= vec4( enc.xy, 0.0, 0.0 ) * vec4( 2.0, 2.0, 0.0, 0.0 ) + vec4( -1.0, -1.0, 1.0, -1.0 );
    float l = dot( n.xyz, -n.xyw );
    n.z		= l;
    n.xy	*= sqrt( l );
    return n.xyz * 2.0 + vec3( 0.0, 0.0, -1.0 );
}

void main( void )
{
	float bl		= 0.0;
	float d			= 0.0;
	vec3 N			= decodeNormal( texture( uSamplerNormal, vertex.uv ).rg );
    float depth		= texture( uSamplerDepth, vertex.uv ).r;
	vec4 F			= vec4( 0.0 );
	vec3 fresnel	= normalize( ( texture( uSamplerNoise, rand( vertex.uv ) * uOffset * vertex.uv ).xyz * 2.0 ) - vec3( 1.0 ) );
	vec3 R			= vec3( 0.0 );
	
	vec3 unitSphere[ 10 ];
	unitSphere[ 0 ]	= vec3(  0.13790712,   0.24864247,   0.44301823 );
	unitSphere[ 1 ]	= vec3(  0.33715037,   0.56794053,  -0.005789503 );
	unitSphere[ 2 ]	= vec3(  0.06896307,  -0.15983082,  -0.85477847 );
	unitSphere[ 3 ]	= vec3( -0.014653638,  0.14027752,   0.0762037 );
	unitSphere[ 4 ]	= vec3(  0.010019933, -0.1924225,   -0.034443386 );
	unitSphere[ 5 ]	= vec3( -0.35775623,  -0.5301969,   -0.43581226 );
	unitSphere[ 6 ]	= vec3( -0.3169221,    0.106360726,  0.015860917 );
	unitSphere[ 7 ]	= vec3(  0.010350345, -0.58698344,   0.0046293875 );
	unitSphere[ 8 ]	= vec3( -0.053382345,  0.059675813, -0.5411899 );
	unitSphere[ 9 ]	= vec3(  0.035267662, -0.063188605,  0.54602677 );
    
    for ( int i = 0; i < 10; ++i ) {
		R			= uRadius * reflect( unitSphere[ i ], fresnel );
		vec2 uv		= vertex.uv + sign( dot( R, N ) ) * R.xy;
		d			= depth - texture( uSamplerDepth, uv ).x;
		bl			+= step( uFalloff, d ) * ( 1.0 - dot( N, N ) ) * ( 1.0 - smoothstep( uFalloff, uStrength, d ) );
    }

    oColor			= vec4( vec3( 1.0 + bl * kInvSamples ), 1.0 );
}
