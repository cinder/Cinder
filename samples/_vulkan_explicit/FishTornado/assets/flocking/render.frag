#version 330 
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(std140, binding = 4) uniform ciBlock1T {
	uniform vec3	uFogColor;
	uniform vec3	uFishColor;
	uniform vec3	uFloorColor;
	uniform float	uTime;
	uniform float	uDepthBias;
	uniform int		uRenderType;
	uniform float 	ciElapsedSeconds;
} ciBlock1;

layout(binding = 5) uniform sampler2D	uDiffuseTex;
layout(binding = 6) uniform sampler2D	uNormalsTex;
layout(binding = 7) uniform sampler2D	uSpecularTex;
layout(binding = 8) uniform sampler2D	uCausticsTex;

//layout(binding = 9) uniform sampler2DShadow	uShadowMap;
layout(binding = 9) uniform sampler2D	uShadowMap;

layout(location =  0) in vec2	PosCoords;
layout(location =  1) in vec4	Position;
layout(location =  2) in vec4	WorldPos;
layout(location =  3) in vec4	ViewPos;
layout(location =  4) in vec2	TexCoord;
layout(location =  5) in vec3	ViewNormal;
layout(location =  6) in vec3	Normal;
layout(location =  7) in vec3	LightPos;  
layout(location =  8) in vec3	LightDir;
layout(location =  9) in float	FogPer;
layout(location = 10) in vec4	ShadowCoord;
layout(location = 11) in float	vCrowd;

layout(location = 0) out vec4 FragColor;

//const vec3 SURFACE_COL = vec3( 189.0, 234.0, 255.0 )/255.0;
const vec3 SURFACE_COL = vec3( 189.0, 190.0, 192.0 )/255.0;

float rand( vec2 seed ) 
{
	return fract( sin(dot(seed.xy ,vec2( 12.9898, 78.233 ))) * 43758.5453 );
}

float rand( vec4 seed )
{
	float dot_product = dot(seed, vec4(12.9898,78.233, 45.164, 94.673));
	return fract(sin(dot_product) * 43758.5453);
}

vec4 getRandomOffset( int i ) {
	float indexA = rand(vec4(gl_FragCoord.xyx, ciBlock1.ciElapsedSeconds*i)) - 0.5;
	float indexB = rand(vec4(gl_FragCoord.yxy * i, ciBlock1.ciElapsedSeconds*i)) - 0.5;
	return vec4( vec2(indexA, indexB), 0, 0);
}

vec2 getNormSlopeBias( vec3 N, vec3 L ) {
	
	float cos_alpha = clamp(dot(N, L), 0.0, 1.0 );
	float offset_scale_N = sqrt(1 - cos_alpha * cos_alpha); // sin(acos(L·N))
	float offset_scale_L = offset_scale_N / cos_alpha;    // tan(acos(L·N))
	return vec2(offset_scale_N, min(2.0, offset_scale_L));
}

float ciTextureProj( vec4 P, float bias )
{
	float shadow = 1.0;
	vec4 shadowCoord = P / P.w;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) 
	{
		float dist = texture( uShadowMap, shadowCoord.st ).r;
		shadowCoord.z += bias;
		if ( shadowCoord.w > 0.0 && dist < shadowCoord.z ) {
			//shadow = 0.0;
			shadow = dist;
		}
	}
	return shadow;
}

float sampleBasic( vec4 sc )
{
	//return textureProj( uShadowMap, sc );
	return ciTextureProj( sc, ciBlock1.uDepthBias );
}

/*
float samplePCF4x4( vec4 sc, sampler2DShadow shadowMap )
{
	float shadow = 1.0;
	if( sc.w > 1.0 ) {
		float sum = 0.0;
		const int r = 1;
		const int s = 2 * r;
		sum += textureProjOffset( shadowMap,  sc, ivec2(-s,-s) );
		sum += textureProjOffset( shadowMap,  sc, ivec2(-r,-s) );
		sum += textureProjOffset( shadowMap,  sc, ivec2( r,-s) );
		sum += textureProjOffset( shadowMap,  sc, ivec2( s,-s) );
		
		sum += textureProjOffset( shadowMap,  sc, ivec2(-s,-r) );
		sum += textureProjOffset( shadowMap,  sc, ivec2(-r,-r) );
		sum += textureProjOffset( shadowMap,  sc, ivec2( r,-r) );
		sum += textureProjOffset( shadowMap,  sc, ivec2( s,-r) );
		
		sum += textureProjOffset( shadowMap,  sc, ivec2(-s, r) );
		sum += textureProjOffset( shadowMap,  sc, ivec2(-r, r) );
		sum += textureProjOffset( shadowMap,  sc, ivec2( r, r) );
		sum += textureProjOffset( shadowMap,  sc, ivec2( s, r) );
		
		sum += textureProjOffset( shadowMap,  sc, ivec2(-s, s) );
		sum += textureProjOffset( shadowMap,  sc, ivec2(-r, s) );
		sum += textureProjOffset( shadowMap,  sc, ivec2( r, s) );
		sum += textureProjOffset( shadowMap,  sc, ivec2( s, s) );
		
		shadow = sum/16.0;
	}
	return shadow;
}

float sampleRandom( vec4 sc, vec2 normSlopeBias )
{
	float shadow = 0.0;
	int numSamples = 10;
	for( int i = 0; i<numSamples; i++ ) {
		vec4 off = getRandomOffset( i );
		off.xy *= normSlopeBias;
		shadow += textureProj( uShadowMap, sc + off );
	}
	return shadow / float( numSamples );
}
*/

// returns diffuse and rim values for eye-positioned light
void setEyeLighting( vec3 normal, inout float eyeDiff, inout float eyeRim )
{
	eyeDiff		= abs( dot( normal, normalize( WorldPos.xyz ) ) );
	eyeRim		= 1.0 - eyeDiff;
}

void main()
{	
	vec3 diffuseRgb		= texture( uDiffuseTex, TexCoord ).rgb;
//	vec3 normalsRgb		= texture( uNormalsTex, TexCoord ).rgb * 2.0 - 1.0;
	vec3 specularRgb  	= texture( uSpecularTex, TexCoord ).rgb;
	float specularVal	= specularRgb.r;
	vec3 causticsRgb	= texture( uCausticsTex, PosCoords + vec2( ciBlock1.uTime, ciBlock1.uTime ) * 0.002 ).rgb;
	
	vec3 N				= normalize( Normal );//+ normalsRgb );
	vec3 L				= normalize( LightDir );
	vec3 C				= normalize( -WorldPos.xyz );
	vec3 R				= normalize( -reflect( L, N ) );
	
	float A				= 0.48 * vCrowd;
	float NdotL			= max( dot( N, L ), 0.0 );
	float D				= NdotL;
	float S				= pow( max( dot( R, C ), 0.0 ), 12.0 );
	
	float shadow		= 1.0f;
	vec4 sc				= ShadowCoord;
	sc.z				+= ciBlock1.uDepthBias;
//	shadow				= sampleBasic( sc ) * 0.75 + 0.25;
//	shadow				= samplePCF4x4( sc, uShadowMap ) * 0.75 + 0.25;
	vec2 offset			= mix( vec2( 10.0 ), getNormSlopeBias( N, L ), 1.0 );

	//shadow			= sampleRandom( sc, offset ) * 0.75 + 0.25;
	shadow				= sampleBasic( sc ) * 0.80 + 0.20;
	
	// eye/cam lighting
	float eyeDiff, eyeRim;
	setEyeLighting( N, eyeDiff, eyeRim );
	float eyeSpec		= pow( eyeDiff, 20.0 );
	
	float upAmt			= max( ViewNormal.y, 0.0 );
	float downAmt		= max( -ViewNormal.y, 0.0 );
	float depthDarkness = clamp( (ViewPos.y + 50.0 )/600.0, 0.0, 1.0 );
	
	vec3 caustics		= causticsRgb * upAmt;
	
	vec3 ambientFinal	= A * ciBlock1.uFishColor;
	vec3 diffuseFinal	= vec3( 0.9*D * ciBlock1.uFishColor * shadow );//D * shadow;
	float specularFinal	= S * 0.5;
	//vec3 alphaGlowFinal	= ( specularVal ) * ciBlock1.uFogColor * 2.0 * vCrowd;
	vec3 alphaGlowFinal	= 0.75 * vec3( 1.25*specularVal*S + 0.2*vCrowd )*ciBlock1.uFogColor ;
	vec3 causticsFinal	= caustics * shadow;
	//vec3 floorReflect	= ciBlock1.uFloorColor * ( downAmt ) * pow( max( 1.0 - depthDarkness, 0.0 ), 6.0 ) * vCrowd * 3.0;
	vec3 floorReflect	= 1.25 * ciBlock1.uFloorColor * ( downAmt ) * 0.75*pow( 15.0, -1.1*depthDarkness ) * 3.0 * vCrowd + 0.35*specularRgb;
	vec3 surfaceReflect	= ciBlock1.uFogColor * ( upAmt ) * vCrowd * depthDarkness * 2.1;
	vec3 eyeDiffFinal	= ciBlock1.uFishColor * eyeDiff * FogPer * 0.5;

	vec3 finalColor		= vec3( ( ambientFinal + 0.25*diffuseFinal + specularFinal ) + floorReflect + surfaceReflect + causticsFinal + eyeDiffFinal ) * diffuseRgb + alphaGlowFinal;

	FragColor.rgb		= mix( ciBlock1.uFogColor, finalColor, FogPer );
	FragColor.a			= 1.0;

	//FragColor.rgb = vec3( diffuseFinal );
}


