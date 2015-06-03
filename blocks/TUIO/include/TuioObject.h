/*
 Copyright (c) 2010, The Cinder Project: http://libcinder.org
 All rights reserved.

 Portions Copyright (c) 2010, Hector Sanchez-Pajares
 Aer Studio http://www.aerstudio.com
 All rights reserved.

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
*/

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Vector.h"

#include "TuioClient.h"
#include "TuioProfileBase.h"

namespace cinder { namespace tuio {
	
class Object : public ProfileBase {
  public:
	Object() : ProfileBase() {}

	Object( std::string source, int32_t sessionId, int32_t fiducialId, vec2 pos, float angle, vec2 speed, float rotationSpeed, float motionAccel, float rotationAccel )
		: ProfileBase(source,sessionId), mPos( pos ), mSpeed( speed ), mPrevPos( pos ),
		mFiducialId( fiducialId ), mAngle( angle ), mRotationSpeed( rotationSpeed ), mRotationAccel( rotationAccel )
	{}

	// Create from a '2dobj' 'set' message
	static Object createFromSetMessage( const osc::Message &message )
	{
		std::string source = message.getRemoteIp();
		int32_t sessionId = message.getArgAsInt32( 1 );
		int32_t fiducialId = message.getArgAsInt32( 2 );
		vec2 pos = vec2( message.getArgAsFloat( 3 ), message.getArgAsFloat( 4 ) );
		float angle = message.getArgAsFloat( 5 );
		vec2 speed = vec2( message.getArgAsFloat( 6 ), message.getArgAsFloat( 7 ) );
		float rotationSpeed = message.getArgAsFloat( 8 );
		float motionAccel = message.getArgAsFloat( 9 );
		float rotationAccel = message.getArgAsFloat( 10 );

		return Object( source, sessionId, fiducialId, pos, angle, speed, rotationSpeed, motionAccel, rotationAccel );
	}

	vec2	getPos() const { return mPos; }
	vec2	getPrevPos() const { return mPrevPos; }
	vec2	getSpeed() const { return mSpeed; }
	float	getMotionAccel() const { return mMotionAccel; }

	int32_t getFiducialId() const {  return mFiducialId; };
	//! Returns the angle of the object measured in radians
	float getAngle() const { return mAngle; }
	float getRotationSpeed() const { return mRotationSpeed; }
	float getRotationAccel() const {  return mRotationAccel; }

  protected:	
	int32_t		mFiducialId;
	float		mAngle;
	float		mRotationSpeed, mRotationAccel;
	vec2		mPos, mPrevPos;
	vec2		mSpeed;
	float		mMotionAccel;
};

} } // namespace cinder::tuio
