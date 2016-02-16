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
	uniform mat4	ciModelViewProjection;
	uniform mat4	ciModelView;
	uniform mat4	ciViewMatrix;
	uniform mat3	ciNormalMatrix;
	uniform mat4	ciModelMatrix;

	uniform mat4	uShadowMvp;
	uniform vec4	uLightPos;
	uniform float	uFogNearDist;
	uniform float	uFogFarDist;
	uniform float	uFogPower;
} ciBlock0;

layout(location = 0) in vec4        ciPosition;
layout(location = 1) in vec3        ciNormal;
layout(location = 2) in vec2		ciTexCoord0;

layout(location = 0) out vec4		Position;
layout(location = 1) out vec3		Normal;
layout(location = 2) out vec2		TexCoord;
layout(location = 3) out vec3		EyeDir;
layout(location = 4) out vec3		LightDir;
layout(location = 5) out float		FogPer;
layout(location = 6) out vec4		ShadowCoord;

void main()
{
	Normal				= normalize( ciBlock0.ciNormalMatrix * ciNormal ); 
	Position			= ciBlock0.ciModelView * ciPosition;
	TexCoord			= ciPosition.xz;
	
	EyeDir				= normalize( -Position.xyz );
	vec3 viewLightPos	= vec3( ciBlock0.ciViewMatrix * ciBlock0.uLightPos );
	LightDir			= normalize( viewLightPos - ( Position.xyz * ciBlock0.uLightPos.w ) );
	
	FogPer				= pow( 1.0 - min( max( length( Position.xyz ) - ciBlock0.uFogNearDist, 0.0 ) / ciBlock0.uFogFarDist, 1.0 ), ciBlock0.uFogPower );
	
	gl_Position			= ciBlock0.ciModelViewProjection * ciPosition;
	ShadowCoord			= ( ciBlock0.uShadowMvp * ciBlock0.ciModelMatrix ) * ciPosition;
}
