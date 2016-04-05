/*
 Copyright (c) 2014, Robert Hodgin, All rights reserved.
 Copyright (c) 2016, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.

 Copyright 2016 Google Inc.
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#version 150
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(std140, binding = 1) uniform ciBlock1T {
	uniform float		ciElapsedSeconds;

	uniform float		uTime;

	uniform vec4		uLightPos;
	uniform float		uDepthBias;
	uniform vec3		uFishColor;
	uniform vec3		uFogColor;
	uniform vec3		uFloorColor;
} ciBlock1;

layout(binding = 2) uniform sampler2D	uDiffuseTex;
layout(binding = 3) uniform sampler2D	uNormalsTex;
layout(binding = 4) uniform sampler2D	uCausticsTex;

//uniform sampler2DShadow	uShadowMap;
layout(binding = 5) uniform sampler2D 	uShadowMap;

layout(location =  0) in vec4	Position;
layout(location =  1) in vec4	WorldPos;
layout(location =  2) in vec4	ViewPos;
layout(location =  3) in vec3	Normal;
layout(location =  4) in vec3	ViewNormal;
layout(location =  5) in vec2	TexCoord;
layout(location =  6) in vec3	EyeDir;
layout(location =  7) in vec3	LightPos;
layout(location =  8) in vec3	LightDir;
layout(location =  9) in float	FogPer;
layout(location = 10) in vec4	ShadowCoord;

layout(location =  0)out vec4	FragColor;

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

float ciTextureProj( vec4 P, float bias )
{
	float shadow = 1.0;
	vec4 shadowCoord = P / P.w;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) 
	{
		float dist = texture( uShadowMap, shadowCoord.st ).r;
		shadowCoord.z += bias;
		if ( shadowCoord.w > 0.0 && dist < shadowCoord.z ) {
			shadow = 0.0;
		}
	}
	return shadow;
}

float sampleBasic( vec4 sc )
{
	//return textureProj( uShadowMap, sc );
	return ciTextureProj( sc, -0.00005);
}

/*
float sampleRandom( vec4 sc, vec2 normSlopeBias )
{
	float shadow = 0.0;
	int numSamples = 20;
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
	vec2 PosCoords		= ViewPos.xz * 0.005;
	
	vec3 diffuseRgb		= texture( uDiffuseTex, TexCoord ).rgb;
	vec3 normalsRgb		= texture( uNormalsTex, TexCoord ).rbg * 2.0 - 1.0;
	vec3 causticsRgb	= texture( uCausticsTex, PosCoords + vec2( ciBlock1.uTime, ciBlock1.uTime ) * 0.001 ).rgb;
	
	vec3 N				= normalize( Normal + 0*normalsRgb );
	vec3 L				= normalize( LightDir );
	vec3 C				= normalize( -Position.xyz );
	vec3 R				= normalize( -reflect( L, N ) );
	
	float A				= 0.5;
	float NdotL			= max( dot( N, L ), 0.0 );
	float D				= NdotL;
	
	float shadow		= 1.0f;
	vec4 sc				= ShadowCoord;
	sc.z				+= ciBlock1.uDepthBias;
	vec2 offset			= mix( vec2( 25.0 ), getNormSlopeBias( N, L ), 0.0 );
	shadow				= sampleBasic( sc )*0.75 + 0.25; //sampleRandom( sc, offset ) * 0.75 + 0.25;
	
	// eye/cam lighting
	float eyeDiff, eyeRim;
	setEyeLighting( N, eyeDiff, eyeRim );
	float eyeSpec		= pow( eyeDiff, 20.0 );
	
	vec3 caustics		= causticsRgb * max( N.y, 0.0 );
	
	float upAmt			= max( ViewNormal.y, 0.0 );
	float downAmt		= max( -ViewNormal.y, 0.0 );
	float depthDarkness = clamp( ( ViewPos.y + 50.0 )/600.0, 0.0, 1.0 );
//	float depthDarkness = Position.y/200.0;
	
	vec3 causticsFinal	= caustics * shadow;
	
	vec3 ambientFinal	= A * ciBlock1.uFishColor;
	vec3 diffuseFinal	= vec3( D * 0.5 ) * shadow;
	vec3 floorReflect	= ciBlock1.uFloorColor * downAmt * max( 1.0 - depthDarkness, 0.0 ) * 0.5;
	vec3 surfaceReflect	= ciBlock1.uFogColor * upAmt * shadow;
	
//	vec3 finalDiffuse	= D * uColor * shadow + lightCol;
//	vec3 finalSpecular	= S * shadow;
	
	vec3 finalColor		= ( ambientFinal + diffuseFinal + floorReflect + surfaceReflect ) * 0.82*diffuseRgb + causticsFinal;
	
	FragColor.rgb		= mix( ciBlock1.uFogColor, finalColor, FogPer );
	FragColor.a			= 1.0;
}


