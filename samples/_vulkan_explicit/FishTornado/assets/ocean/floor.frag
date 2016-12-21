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

#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(std140, binding = 1) uniform ciBlock1T {
	uniform float		ciElapsedSeconds;
	uniform float		uTime;
	uniform float		uDepthBias;
	uniform vec3		uFogColor;
} ciBlock1;

layout(binding = 2) uniform sampler2D	uDiffuseTex;
layout(binding = 3) uniform sampler2D	uNormalsTex;
layout(binding = 4) uniform sampler2D	uCausticsTex;
layout(binding = 5) uniform sampler2D	uShadowMap;

layout(location = 0) in vec4		Position;
layout(location = 1) in vec3		Normal;
layout(location = 2) in vec2		TexCoord;
layout(location = 3) in vec3		EyeDir;
layout(location = 4) in vec3		LightDir;
layout(location = 5) in float		FogPer;
layout(location = 6) in vec4		ShadowCoord;

layout(location = 0) out vec4		FragColor;

vec2 getNormSlopeBias( vec3 N, vec3 L ) {
	
	float cos_alpha = clamp(dot(N, L), 0.0, 1.0 );
	float offset_scale_N = sqrt(1 - cos_alpha * cos_alpha); // sin(acos(L·N))
	float offset_scale_L = offset_scale_N / cos_alpha;    // tan(acos(L·N))
	return vec2(offset_scale_N, min(2.0, offset_scale_L));
}

float sampleBasic( vec4 sc )
{
	// No need to check depth since the floor never casts shadows on anything above it. 
	vec4 shadowCoord = sc / sc.w;
	float shadow = 1.0 - texture( uShadowMap, shadowCoord.st ).r;
	shadow = clamp( shadow, 0.0, 1.0 );
	return shadow;	
}

void main()
{
	vec2 tc				= TexCoord * 0.04;
	vec3 diffuseRgb		= texture( uDiffuseTex, tc * 0.1 ).rgb;
	vec3 normalsRgb		= texture( uNormalsTex, tc * 0.1 ).rgb * 2.0 - 1.0;
	vec3 causticsRgb	= texture( uCausticsTex, tc * 0.035 + vec2( ciBlock1.uTime, ciBlock1.uTime ) * 0.005 ).rgb;
	vec3 caustics2Rgb	= texture( uCausticsTex, tc * 0.025 + vec2( ciBlock1.uTime, ciBlock1.uTime ) * -0.01 ).rgb;
	vec3 caustics3Rgb	= texture( uCausticsTex, tc * 0.01 + vec2( ciBlock1.uTime, ciBlock1.uTime ) * -0.002 ).rgb;
	
	vec3 N				= normalize( Normal + normalsRgb * 0.2 );
	vec3 L				= normalize( LightDir );
	vec3 C				= normalize( -Position.xyz );
	vec3 R				= normalize( -reflect( L, N ) );
	
	vec3 A				= vec3( 0.05 );
	float NdotL			= max( dot( N, L ), 0.0 );
	vec3 D				= vec3( NdotL );
	vec3 S				= pow( max( dot( R, C ), 0.0 ), 150.0 ) * vec3(1.0);
	
	float shadow = 1.0f;
	vec4 sc = ShadowCoord;
	sc.z   += ciBlock1.uDepthBias;
	vec2 offset			= mix( vec2( 70.0 ), getNormSlopeBias( N, L ), 0.0 );
	shadow				= sampleBasic( sc )*0.70 + 0.30;
	
	float eyeDiff		= max( dot( C, N ), 0.0 );
	float eyeRim		= pow( 1.0 - eyeDiff, 4.0 );
	
	vec3 fogColor		= ciBlock1.uFogColor + vec3( 0.1, 0.3, 0.0 ) * sin( pow( FogPer, 2.0 ) * 3.14159 );
	vec3 finalColor		= ( diffuseRgb * 0.7 * ( shadow * 0.75 + 0.25 ) ) + ( causticsRgb * caustics2Rgb * 0.5 + caustics3Rgb * 0.4 ) * shadow + eyeRim * 0.25;
	
	FragColor.rgb		= mix( fogColor, finalColor, FogPer );
	FragColor.a			= 1.0;
}


