//#version 410
#version 150
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable


layout(location = 0) in vec4 vColor;
layout(location = 1) in vec4 vPosition;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec4 vModelPosition;
layout(location = 4) in vec3 vModelNormal;
layout(location = 5) in vec4 vShadowCoord;

layout(std140, binding = 1) uniform ciBlock1T {
	uniform vec4 	uLightPos;
	uniform vec4	uColor;

	uniform float	ciElapsedSeconds;
	uniform bool	uIsTeapot;
	uniform int		uShadowTechnique;
	uniform float	uDepthBias;	
	uniform bool	uOnlyShadowmap;
	uniform float	uRandomOffset;
	uniform bool	uEnableNormSlopeOffset;
	uniform int		uNumRandomSamples;
} ciBlock1;

//layout(binding = 2) uniform sampler2DShadow uShadowMap;
layout(binding = 2) uniform sampler2D uShadowMap;

layout(location = 0) out vec4 fragColor;

float rand( vec2 seed ) {
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

float ciTextureProj( vec4 P, vec2 off, float bias )
{
	float shadow = 1.0;
	vec4 shadowCoord = P / P.w;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) 
	{
		float dist = texture( uShadowMap, shadowCoord.st + off ).r;
		shadowCoord.z += bias;
		if ( shadowCoord.w > 0.0 && dist < shadowCoord.z ) {
			shadow = 0.0;
		}
	}
	return shadow;
}

float sampleBasic( vec4 sc, float bias ) {
	//return textureProj( uShadowMap, sc );
	return ciTextureProj( sc, vec2( 0 ), bias );
}

float samplePCF3x3( vec4 sc, float bias )
{
	ivec2 size = textureSize( uShadowMap, 0 );
	float s = 2.0;
	float dx = s*1.0/float(size.x);
	float dy = s*1.0/float(size.y);

	float shadow = 0.0;
	shadow += ciTextureProj( sc, vec2(-dx,-dy), bias );
	shadow += ciTextureProj( sc, vec2(-dx,  0), bias );
	shadow += ciTextureProj( sc, vec2(-dx, dy), bias );
	shadow += ciTextureProj( sc, vec2(  0,-dy), bias );
	shadow += ciTextureProj( sc, vec2(  0,  0), bias );
	shadow += ciTextureProj( sc, vec2(  0, dy), bias );
	shadow += ciTextureProj( sc, vec2( dx,-dy), bias );
	shadow += ciTextureProj( sc, vec2( dx, 0 ), bias );
	shadow += ciTextureProj( sc, vec2( dx, dy), bias );
	return shadow/9.0;

/*
	const int s = 2;
	
	float shadow = 0.0;
	shadow += textureProjOffset( uShadowMap, sc, ivec2(-s,-s) );
	shadow += textureProjOffset( uShadowMap, sc, ivec2(-s, 0) );
	shadow += textureProjOffset( uShadowMap, sc, ivec2(-s, s) );
	shadow += textureProjOffset( uShadowMap, sc, ivec2( 0,-s) );
	shadow += textureProjOffset( uShadowMap, sc, ivec2( 0, 0) );
	shadow += textureProjOffset( uShadowMap, sc, ivec2( 0, s) );
	shadow += textureProjOffset( uShadowMap, sc, ivec2( s,-s) );
	shadow += textureProjOffset( uShadowMap, sc, ivec2( s, 0) );
	shadow += textureProjOffset( uShadowMap, sc, ivec2( s, s) );
	return shadow/9.0;
*/
}

float samplePCF4x4( vec4 sc, float bias )
{
	ivec2 size = textureSize( uShadowMap, 0 );
	float r = 2.0;
	float dx2 = r*1.0/float(size.x);
	float dy2 = r*1.0/float(size.y);
	float s = 2.0*r;
	float dx4 = r*1.0/float(size.x);
	float dy4 = r*1.0/float(size.y);

	float shadow = 0.0;
	shadow += ciTextureProj( sc, vec2(-dx4,-dx4), bias );
	shadow += ciTextureProj( sc, vec2(-dx2,-dx4), bias );
	shadow += ciTextureProj( sc, vec2( dx2,-dx4), bias );
	shadow += ciTextureProj( sc, vec2( dx4,-dx4), bias );
	
	shadow += ciTextureProj( sc, vec2(-dx4,-dx2), bias );
	shadow += ciTextureProj( sc, vec2(-dx2,-dx2), bias );
	shadow += ciTextureProj( sc, vec2( dx2,-dx2), bias );
	shadow += ciTextureProj( sc, vec2( dx4,-dx2), bias );
	
	shadow += ciTextureProj( sc, vec2(-dx4, dx2), bias );
	shadow += ciTextureProj( sc, vec2(-dx2, dx2), bias );
	shadow += ciTextureProj( sc, vec2( dx2, dx2), bias );
	shadow += ciTextureProj( sc, vec2( dx4, dx2), bias );
	
	shadow += ciTextureProj( sc, vec2(-dx4, dx4), bias );
	shadow += ciTextureProj( sc, vec2(-dx2, dx4), bias );
	shadow += ciTextureProj( sc, vec2( dx2, dx4), bias );
	shadow += ciTextureProj( sc, vec2( dx4, dx4), bias );
		
	return shadow/16.0;


/*
	const int r = 2;
	const int s = 2 * r;
	
	float shadow = 0.0;
	shadow += textureProjOffset( uShadowMap,  sc, ivec2(-s,-s) );
	shadow += textureProjOffset( uShadowMap,  sc, ivec2(-r,-s) );
	shadow += textureProjOffset( uShadowMap,  sc, ivec2( r,-s) );
	shadow += textureProjOffset( uShadowMap,  sc, ivec2( s,-s) );
	
	shadow += textureProjOffset( uShadowMap,  sc, ivec2(-s,-r) );
	shadow += textureProjOffset( uShadowMap,  sc, ivec2(-r,-r) );
	shadow += textureProjOffset( uShadowMap,  sc, ivec2( r,-r) );
	shadow += textureProjOffset( uShadowMap,  sc, ivec2( s,-r) );
	
	shadow += textureProjOffset( uShadowMap,  sc, ivec2(-s, r) );
	shadow += textureProjOffset( uShadowMap,  sc, ivec2(-r, r) );
	shadow += textureProjOffset( uShadowMap,  sc, ivec2( r, r) );
	shadow += textureProjOffset( uShadowMap,  sc, ivec2( s, r) );
	
	shadow += textureProjOffset( uShadowMap,  sc, ivec2(-s, s) );
	shadow += textureProjOffset( uShadowMap,  sc, ivec2(-r, s) );
	shadow += textureProjOffset( uShadowMap,  sc, ivec2( r, s) );
	shadow += textureProjOffset( uShadowMap,  sc, ivec2( s, s) );
		
	return shadow/16.0;
*/
}

/*
float sampleRandom( vec4 sc, vec2 normSlopeBias )
{
	float shadow = 0.0;
	for( int i = 0; i< ciBlock1.uNumRandomSamples; i++ ) {
		vec4 off = getRandomOffset( i );
		off.xy *= normSlopeBias;
		shadow += textureProj( uShadowMap, sc + off );
	}
	return shadow / float( ciBlock1.uNumRandomSamples );
}
*/

void main()
{
	// Normal in view space
	vec3	N = normalize( vNormal );
	// Light direction
	vec3	L = normalize( vec3(ciBlock1.uLightPos) - vPosition.xyz );
	// To camera vector
	vec3	C = normalize( -vPosition.xyz );
	// Surface reflection vector
	vec3	R = normalize( -reflect( L, N ) );
	
	// Modulated ambient (with fake red indirect lighting coming from the sphere)
	vec3	sphereGlow = vec3( 0.6, 0.15, 0.15 );
	vec3	indirectGlow = vec3( clamp( dot( 0.8 * normalize(vModelNormal), -normalize(vModelPosition.xyz) ), 0.0, 0.55 ), 0.0, 0.0 );
	vec3	A = mix( sphereGlow, indirectGlow, float(ciBlock1.uIsTeapot) ) + vec3( 0.07, 0.05, 0.1 );
	// Diffuse factor
	float NdotL = max( dot( N, L ), 0.0 );
	vec3	D = vec3( NdotL );
	// Specular factor
	vec3	S = pow( max( dot( R, C ), 0.0 ), 50.0 ) * vec3(1.0);

	// Sample the shadowmap to compute the shadow value
	float shadow = 1.0;
	vec4 sc = vShadowCoord;
	//sc.z += ciBlock1.uDepthBias;
	
	shadow = sampleBasic( sc, ciBlock1.uDepthBias );
	//shadow = samplePCF3x3( sc, ciBlock1.uDepthBias );
	//shadow = samplePCF4x4( sc, ciBlock1.uDepthBias );

/*
	if( ciBlock1.uShadowTechnique == 0 ) {
		//shadow = sampleBasic( sc, ciBlock1.uDepthBias );
		shadow = sampleBasic( sc, ciBlock1.uDepthBias );
	}
	else if( ciBlock1.uShadowTechnique == 1 ) {
		shadow = samplePCF3x3( sc, ciBlock1.uDepthBias );
	}
	else if ( ciBlock1.uShadowTechnique == 2 ) {
		shadow = samplePCF4x4( sc );
	}
	else if ( ciBlock1.uShadowTechnique == 3 ) {
		vec2 offset	= mix( vec2(ciBlock1.uRandomOffset), getNormSlopeBias( N, L ), float(ciBlock1.uEnableNormSlopeOffset) );
		shadow = sampleRandom( sc, offset );
	}
*/

	fragColor.rgb = mix( ( ( D + S ) * shadow + A ) * ciBlock1.uColor.rgb, vec3(shadow), float(ciBlock1.uOnlyShadowmap) );
	fragColor.a = 1.0;
}
