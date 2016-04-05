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

layout(std140, binding = 0) uniform ciBlock0T {
	uniform mat4		ciModelViewProjection;
	uniform mat4		ciModelView;
	uniform mat4		ciViewMatrix;
	uniform mat4		ciViewMatrixInverse;
	uniform mat3		ciNormalMatrix;
	uniform mat4		ciModelMatrix;
	uniform mat4		ciModelMatrixInverse;
	uniform float		uTime;
	uniform int			uFboRes;
} ciBlock0;

layout(binding = 1) uniform sampler2D uPrevPosTex;
layout(binding = 2) uniform sampler2D uCurrPosTex;
layout(binding = 3) uniform sampler2D uCurrVelTex;

layout(location = 0) in vec4 ciPosition;
layout(location = 1) in vec2 ciTexCoord0;
layout(location = 2) in vec3 ciNormal;
layout(location = 3) in vec3 vInstancePosition; // per-instance position variable

layout(location = 0) out vec4 Position;

float getSinVal( float z, float randPer ){
	float t			= ( ciBlock0.uTime * 0.6 + randPer * 26.0 );
	float s			= sin( z + t ) * z * 1.0;
	return s;
}

void main()
{
	float scale			= 1.65;
	Position			= ciBlock0.ciModelView * scale*ciPosition;
	vec4 VertPos		= scale*ciPosition;

	// texture data
	vec2 samplePos		= vec2( ( gl_InstanceIndex % ciBlock0.uFboRes ), ( gl_InstanceIndex / ciBlock0.uFboRes ) );
	vec4 currPos		= textureLod( uCurrPosTex, samplePos, 0 );
	vec4 prevPos		= textureLod( uPrevPosTex, samplePos, 0 );
	vec4 currVel		= textureLod( uCurrVelTex, samplePos, 0 );
	float leader		= currPos.a;
	
	float currZ			= VertPos.z;
	float prevZ			= currZ + 1.0;
	float currVal		= getSinVal( currZ * 0.2, leader );
	float prevVal		= getSinVal( prevZ * 0.2, leader );
	
	vec3 currCenter		= vec3( currVal, 0.0, currZ );
	vec3 prevCenter		= vec3( prevVal, 0.0, prevZ );
	vec3 pathDir		= normalize( currCenter - prevCenter );
	vec3 pathUp			= vec3( 0.0, 1.0, 0.0 );
	vec3 pathNormal		= normalize( cross( pathDir, pathUp ) );
	
	mat3 basis			= mat3( pathNormal, pathUp, pathDir );
	vec3 at				= vec3( basis[0][0], basis[1][0], basis[2][0] );
	vec3 bt				= vec3( basis[0][1], basis[1][1], basis[2][1] );
	vec3 ct				= vec3( basis[0][2], basis[1][2], basis[2][2] );
	mat3 trans			= mat3( at, bt, -ct );
	
	vec3 spoke			= vec3( VertPos.x, VertPos.y, 0.0 );
	VertPos.xyz			= currCenter + basis * spoke;
	
	// calculate matrix
	vec3 dir			= normalize( currVel.xyz ); //normalize( currPos.xyz - prevPos.xyz );
	vec3 right			= normalize( cross( vec3( 0.0, 1.0, 0.0 ), dir ) );
	vec3 up				= normalize( cross( dir, right ) );
	mat3 m				= mat3( right, up, dir );
	
	// set final vertex
	vec3 vertexPos		= m * VertPos.xyz;
	VertPos				= vec4( vertexPos + currPos.xyz, ciPosition.w );
	
	gl_Position			= ciBlock0.ciModelViewProjection * VertPos;
}

