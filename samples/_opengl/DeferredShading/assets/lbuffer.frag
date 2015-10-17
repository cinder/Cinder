#if __VERSION__ == 300
	precision highp float;
	precision highp sampler2DShadow;
#endif

uniform sampler2D		uSamplerAlbedo;
uniform sampler2D		uSamplerNormalEmissive;
uniform sampler2D		uSamplerPosition;
uniform sampler2DShadow uSamplerShadowMap;

uniform vec4	uLightColorAmbient;
uniform vec4	uLightColorDiffuse;
uniform vec4	uLightColorSpecular;
uniform float	uLightIntensity;
uniform vec3	uLightPosition;
uniform float	uLightRadius;

uniform bool	uShadowEnabled;
uniform mat4 	uShadowMatrix;

uniform mat4 	uViewMatrixInverse;
uniform vec2	uWindowSize;

in vec2     uv;
out vec4 	oColor;

void main( void )
{
	vec2 uv					= gl_FragCoord.xy / uWindowSize;
	vec4 position			= texture( uSamplerPosition, uv );
	
	// Do not draw background
	if ( length( position.xyz ) <= 0.0 ) {
		discard;
	}
	
	vec3 L 					= uLightPosition - position.xyz;
	float d 				= length( L );
	
	// Only draw fragment if it occurs inside the light volume
	if ( d > uLightRadius ) {
		discard;
	}
	L 						/= d;

	// Calculate lighting
	vec4 normalEmissive		= texture( uSamplerNormalEmissive, uv );
	vec3 N					= normalize( normalEmissive.xyz );
	vec3 V 					= normalize( -position.xyz );
	vec3 H 					= normalize( L + V );
	float NdotL	 			= max( 0.0, dot( N, L ) );
	float HdotN				= max( 0.0, dot( H, N ) );
	float Ks				= pow( HdotN, 100.0 );
	float att				= 1.0 / pow( ( d / ( 1.0 - pow( d / uLightRadius, 2.0 ) ) ) / uLightRadius + 1.0, 2.0 ); 

	// Calculate color
	vec4 Ia					= uLightColorAmbient;
	vec4 Id 				= NdotL * uLightColorDiffuse * texture( uSamplerAlbedo, uv );
	vec4 Is 				= Ks * uLightColorSpecular;
	vec4 Ie					= vec4( vec3( normalEmissive.w ), 1.0 );

	oColor 					= Ia + att * ( Id + Is );
	oColor					*= uLightIntensity;

	// Calculate shadow
	if ( uShadowEnabled ) {
		const float bias	= 0.9986;
		vec4 shadowClip		= uShadowMatrix * uViewMatrixInverse * position;
		vec3 shadowCoord	= shadowClip.xyz / shadowClip.w;
		shadowCoord 		= shadowCoord * 0.5 + 0.5;

		float s				= 0.0;
		if ( shadowCoord.x >= 0.0 && shadowCoord.x <= 1.0 && shadowCoord.y >= 0.0 && shadowCoord.y <= 1.0 ) {
			float depth	= texture( uSamplerShadowMap, shadowCoord );
			if ( depth < shadowCoord.z - bias ) {
				oColor		*= 0.2;
			}
		}
	}

	// Add emissive
	oColor					+= Ie;
	oColor.a				= 1.0;
}
