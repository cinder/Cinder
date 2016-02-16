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

// Do not modify the line below
//<FISH_CONSTANT>

layout(std140, binding = 1) uniform ciBlock1T {
	uniform float	uMinThresh;
	uniform float	uMaxThresh;
	uniform float	uMinSpeed;
	uniform float	uMaxSpeed;
	uniform vec3	uPredPos;
	uniform vec3	uCamPos;
	uniform float	uTimeDelta;
	uniform float	uTime;
	uniform float	uZoneRadius;
} ciBlock1;

layout(binding = 2) uniform sampler2DRect uPosition;
layout(binding = 3) uniform sampler2DRect uVelocity;

layout(location = 1) in vec2 vTexCoord;

layout(location = 0) out vec4 FragColor;

void main()
{	
	vec4 vPos			= texture( uPosition, vTexCoord * vec2( FBO_RES ) );
	vec3 myPos			= vPos.xyz;
	float leadership	= vPos.a;
	float accMulti		= leadership * ciBlock1.uTimeDelta * 0.5;
	
	vec4 vVel			= texture( uVelocity, vTexCoord * vec2( FBO_RES ) );
	vec3 myVel			= vVel.xyz;
	float myCrowd		= vVel.a;
	
	vec3 acc			= vec3( 0.0 );
	
	float zoneRadSqrd	= ciBlock1.uZoneRadius * ciBlock1.uZoneRadius;//pow( uZoneRadius - myCrowd * 0.2, 2.0 );
	
	int myX				= int( vTexCoord.s * FBO_RES );
	int myY				= int( vTexCoord.t * FBO_RES );
	float crowded		= 5.0;

	float threshDelta0 	= ciBlock1.uMaxThresh - ciBlock1.uMinThresh;
	float threshDelta1	= 1.0 - ciBlock1.uMaxThresh;

	// APPLY THE ATTRACTIVE, ALIGNING, AND REPULSIVE FORCES
	for( int y = 0; y < FBO_RES; ++y )
	{
		for( int x = 0; x < FBO_RES; ++x )
		{
			float s 		= abs( x - myX ) + abs( y - myY );			
			vec2 tc			= vec2( x, y );
			vec3 pos		= texture( uPosition, tc ).xyz;
			vec3 vel 		= texture( uVelocity, tc ).xyz;
			vec3 dir		= myPos - pos;
			float distSqrd	= dot( dir, dir );			
						
			if( ( s > 0.0 ) && ( distSqrd < zoneRadSqrd ) ) 
			//if( ( x != myX ) && ( y != myY ) && ( distSqrd < zoneRadSqrd ) )
			{						
				vec3 dirNorm	= normalize( dir );
				float percent	= distSqrd/zoneRadSqrd;
				float crowdPer	= 1.0 - percent;
				
				if( percent < ciBlock1.uMinThresh )
				{
					float F					= ( ciBlock1.uMinThresh / percent - 1.0 ) * accMulti;
					acc						+= dirNorm * F;
					crowded					+= crowdPer; //  * 2.0
				} 
				else if( percent < ciBlock1.uMaxThresh )
				{
					float threshDelta		= ciBlock1.uMaxThresh - ciBlock1.uMinThresh;
					float adjustedPercent	= ( percent - ciBlock1.uMinThresh )/threshDelta;
					float F					= ( 1.0 - ( cos( adjustedPercent * 6.28318 ) * -0.5 + 0.5 ) ) * accMulti;
					acc						+= normalize( vel ) * F;
					crowded					+= crowdPer * 0.5;
				}
				else 
				{
					float threshDelta		= 1.0 - ciBlock1.uMaxThresh;
					float adjustedPercent	= ( percent - ciBlock1.uMaxThresh )/threshDelta;
					float F					= ( 1.0 - ( cos( adjustedPercent * 6.28318 ) * -0.5 + 0.5 ) ) * accMulti;				
					acc						+= -dirNorm * F;
					crowded					+=  crowdPer * 0.25;
				}
				
			}

		}
	}
	
	acc.y *= 0.96;
	acc.y -= 0.005;	


	// avoid predator
	vec3 dirToPred			= myPos - ciBlock1.uPredPos;
	float distToPred		= length( dirToPred );
	float distPredPer		= max( 1.0 - ( distToPred/200.0 ), 0.0 );
	crowded					+= distPredPer * 10.0;
	acc						+= ( normalize( dirToPred ) * vec3( 1.0, 0.25, 1.0 ) ) * distPredPer * ciBlock1.uTimeDelta * 25.0;

	// avoid camera
	vec3 dirToCam			= myPos - ciBlock1.uCamPos;
	float distToCam			= length( dirToCam );
	float distCamPer		= max( 1.0 - ( distToCam/60.0 ), 0.0 );
	crowded					+= distCamPer * 50.0;
	acc						+= ( normalize( dirToCam ) * vec3( 1.0, 0.25, 1.0 ) ) * distCamPer * ciBlock1.uTimeDelta * 5.0;
	
	// pull to center line
	vec2 centerLine = vec2( sin( ciBlock1.uTime * 0.0002 + myPos.y * 0.01 ), sin( ciBlock1.uTime * 0.0002 - 1.5 + myPos.y * 0.01 ) ) * 80.0;
	myVel.xz -= ( myPos.xz - centerLine ) * ciBlock1.uTimeDelta * 0.005;
	
	// pull to center point
	myVel -= normalize( myPos - vec3( 0.0, 150.0, 0.0 ) ) * ciBlock1.uTimeDelta * 0.2;
	
	myVel += acc * ciBlock1.uTimeDelta;
	myCrowd -= ( myCrowd - crowded ) * ( ciBlock1.uTimeDelta );
	
	vec3 tempNewPos		= myPos + myVel * ciBlock1.uTimeDelta;
	float tempNewDist	= length( tempNewPos );
	vec3 tempNewDir		= normalize( tempNewPos );
	
	// set speed limit
	float newMaxSpeed = clamp( ciBlock1.uMaxSpeed + myCrowd * 0.001, ciBlock1.uMinSpeed, 5.0 );
	float velLength = length( myVel );
	vec3 velNorm	= normalize( myVel );
	
	if( velLength < ciBlock1.uMinSpeed ) {
		myVel = velNorm * ciBlock1.uMinSpeed;
	} 
	else if( velLength > newMaxSpeed ) {
		myVel = velNorm * newMaxSpeed;
	}
	
	vec3 nextPos		= myPos + myVel * ciBlock1.uTimeDelta;
	
	// Avoid floor and sky planes
	if( nextPos.y > 470.0 ) {
		myVel.y -= ( myPos.y - 470.0 ) * 0.01; 
	}

	if( nextPos.y < 50.0 ) {
		myVel.y -= ( myPos.y - 50.0 ) * 0.01; 
	}
	
	myVel.y				*= 0.999;
	
	FragColor.rgb	= myVel;
	FragColor.a		= myCrowd;

	//FragColor = vec4( vTexCoord, 0, 1 );
}
