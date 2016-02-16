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

const float			kCARed				= 0.9975;
const float			kCAGreen			= 0.9925;
const float			kCABlue				= 0.985;
const vec3			kSaturation			= vec3( 0.95, 1.0, 1.0 );
const float			kVignetteRadius		= 0.75;
const float			kVignetteSoftness	= 0.35;

layout(std140, binding = 1) uniform ciBlock1T {
	uniform float		uTime;
	uniform vec2		uResolution;
	uniform float		uContrast;
	uniform float		uExposure;
	uniform float		uBrightness;
	uniform float		uBlend;
	uniform float		uDistort;
} ciBlock1;

layout(binding = 2) uniform sampler2D uNoiseNormalsTex;
layout(binding = 3) uniform sampler2D uFboTex;

layout(location = 0) in vec2 TexCoord;
layout(location = 1) out vec4 FragColor;

void main( void )
{
	vec3 normals	= texture( uNoiseNormalsTex, TexCoord + vec2( 0.0, ciBlock1.uTime * 0.025 ) ).rgb * 2.0 - 1.0;
	
	float chromaDistort = ( 1.0 - normals.b ) * 0.1 * ciBlock1.uDistort;
	// vignette
	vec2 position	= ( gl_FragCoord.xy / ciBlock1.uResolution.xy ) - vec2( 0.5 );
	float len		= length( position );
	float vignette	= smoothstep( kVignetteRadius, kVignetteRadius - kVignetteSoftness, len ) * 0.7 + 0.3;
	
	vec2 distortionOffset = normals.rg * ciBlock1.uDistort;
	vec3 color	= vec3( 0.0 );
	if ( kCABlue != 0.0 ) {
		color.r = texture( uFboTex, ( TexCoord - vec2( 0.5 ) ) * ( kCARed - chromaDistort ) + vec2( 0.5 ) + distortionOffset ).r;
		color.g = texture( uFboTex, ( TexCoord - vec2( 0.5 ) ) * ( kCAGreen - chromaDistort ) + vec2( 0.5 ) + distortionOffset ).g;
		color.b	= texture( uFboTex, ( TexCoord - vec2( 0.5 ) ) * ( kCABlue - chromaDistort ) + vec2( 0.5 ) + distortionOffset ).b;
	}
	vec3 fboCol		= color;
	
//	color		*= kSaturation;
//	color.rgb	+= vec3( ciBlock1.uBrightness );
//	color		= clamp( color * 0.5 + 0.5 * color * color * ciBlock1.uContrast, 0.0, 1.0 );
//	color		= pow( color, vec3( ciBlock1.uExposure ) );
//	if ( ciBlock1.uBlend < 1.0 ) {
//		color	= mix( fboCol, color, ciBlock1.uBlend );
//	}
	FragColor.rgb	= color * vignette;
	FragColor.a		= 1.0;
}
