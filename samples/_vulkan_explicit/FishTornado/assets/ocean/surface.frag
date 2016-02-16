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
	uniform vec4		uLightPos;
	uniform vec3		uFogColor;
	uniform float		uTime;
} ciBlock1;

layout(binding = 2) uniform sampler2D uNormalsTex;

layout(location = 0) in vec4 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 ViewNormal;
layout(location = 3) in vec2 TexCoord;
layout(location = 4) in vec3 EyeDir;
layout(location = 5) in vec3 LightPos;
layout(location = 6) in vec3 LightDir;
layout(location = 7) in float FogPer;

layout(location = 0) out vec4 FragColor;

const vec3 SURFACE_COL = vec3( 159.0, 224.0, 230.0 )/255.0;

void main()
{	
	vec3 n1				= texture( uNormalsTex, TexCoord * 0.5 - ciBlock1.uTime * 0.07 ).rbg * 2.0 - 1.0;
	vec3 n2				= texture( uNormalsTex, TexCoord * 1.4 - ciBlock1.uTime * 0.1 ).rbg * 2.0 - 1.0;
	vec3 n3				= texture( uNormalsTex, TexCoord * 2.5 + ciBlock1.uTime * 0.08 ).rbg * 2.0 - 1.0;
	
	vec3 N				= normalize( ViewNormal.rbg + n1 + n2 + n3 );
	vec3 L				= normalize( LightDir );
	vec3 C				= normalize( -Position.xyz );
	vec3 R				= normalize( -reflect( L, N ) );
	
	float A				= 0.05;
	float NdotL			= max( dot( -N, L ), 0.0 );
	float D				= NdotL;
	float S				= pow( max( dot( R, -C ), 0.0 ), 10.0 );

	float eyeDiff		= max( dot( N, EyeDir ), 0.0 );
	float eyeRim		= pow( 1.0 - eyeDiff, 10.0 );
	
	vec3 finalColor		= vec3( S * 5.0 + pow( eyeDiff, 10.0 ) + eyeRim * 10.0 * SURFACE_COL ) * 4.0;
	vec3 finalWithFog	= mix( ciBlock1.uFogColor, finalColor, FogPer );
	
	
	FragColor.rgb		= finalWithFog;
	FragColor.a			= 1.0;
}


