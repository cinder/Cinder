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
	uniform vec2 res;
	uniform vec2 axis;
	uniform float pass0;
	uniform float pass1;
} ciBlock1;

layout(binding = 2) uniform sampler2D uTex;

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec2 vTexCoord;

layout(location = 0) out vec4 FragColor;

vec4 readSample( sampler2D tex, vec2 uv ) 
{
	float r = texture( tex, uv ).r;
	float r0 = r >= 1.0 ? 0.0 : pow( 1.4, -1.05*r );
	float r1 = r;
	r = r0*ciBlock1.pass0 + r1*ciBlock1.pass1;
	return vec4( r );
}

void main()
{
	vec2 dv = 1.0/ciBlock1.res * ciBlock1.axis;
	float offsets[4] = float[]( 0.644342, 2.37885, 4.29111, 6.21661 );
	float weights[4] = float[]( 0.249615, 0.192463, 0.0514763, 0.00644572 );	
	FragColor = vec4( 0 );
	for( int i = 0; i < 4; i++ ) {
		vec2 texCoordOffset = offsets[i] * dv;
		vec4 samp = readSample( uTex, vTexCoord + texCoordOffset ) + readSample( uTex, vTexCoord - texCoordOffset );
		FragColor += weights[i] * samp;
	}	
}

