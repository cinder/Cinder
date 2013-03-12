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

#include "TuioProfileBase.h"
#include "cinder/app/TouchEvent.h"

namespace cinder { namespace tuio {
	
class Cursor : public ProfileBase {
  public:
	Cursor() : ProfileBase() {}

	Cursor( std::string source, int32_t sessionId, Vec2f pos, Vec2f speed = Vec2f::zero(), float motionAccel = 0 )
		: ProfileBase(source,sessionId), mPos( pos ), mSpeed( speed ), mPrevPos( pos )
	{}

	Vec2f	getPos() const { return mPos; }
	Vec2f	getPrevPos() const { return mPrevPos; }
	Vec2f	getSpeed() const { return mSpeed; }
	float	getMotionAccel() const { return mMotionAccel; }

	// Create from an OSC message
	static Cursor createFromSetMessage( const osc::Message &message ) {
		std::string source = message.getRemoteIp();
		if ( message.getAddress() != "/tuio/2Dcur" ) {
			// should really return a bad cursor or throw an exception
			return Cursor( source, -1, Vec2f(0.0,0.0), Vec2f(0.0,0.0), 0.0 );
		}
		int32_t sessionId = message.getArgAsInt32( 1 );
		Vec2f pos( message.getArgAsFloat( 2 ), message.getArgAsFloat( 3 ) );
		Vec2f speed( message.getArgAsFloat( 4 ), message.getArgAsFloat( 5 ) );
		float motionAccel =  message.getArgAsFloat( 6 );
		return Cursor( source, sessionId, pos, speed, motionAccel );
	}

  protected:
	Vec2f		mPos, mPrevPos;
	Vec2f		mSpeed;
	float		mMotionAccel;
};
	
class Cursor25d : public ProfileBase {
  public:
	Cursor25d() : ProfileBase() {}

	Cursor25d( std::string source, int32_t sessionId, Vec3f pos, Vec3f speed = Vec3f::zero(), float motionAccel = 0 )
		: ProfileBase(source,sessionId), mPos25( pos ), mSpeed25( speed ), mPrevPos25( pos )
	{
	}

	Vec2f	getPos() const { return Vec2f(mPos25.x,mPos25.y); }
	Vec2f	getPrevPos() const { return Vec2f(mPrevPos25.x,mPrevPos25.y); }
	Vec2f	getSpeed() const { return Vec2f(mSpeed25.x,mSpeed25.y); }

	Vec3f	getPos25() const { return mPos25; }
	Vec3f	getSpeed25() const { return mSpeed25; }

	// Create from an OSC message
	static Cursor25d createFromSetMessage( const osc::Message &message ) {
		std::string source = message.getRemoteIp();
		if ( message.getAddress() != "/tuio/25Dcur" ) {
			// should really return a bad cursor or throw an exception
			return Cursor25d( source, -1, Vec3f(0.0,0.0,0.0), Vec3f(0.0,0.0,0.0), 0.0 );
		}
		int32_t sessionId = message.getArgAsInt32( 1 );
		Vec3f pos( message.getArgAsFloat( 2 ), message.getArgAsFloat( 3 ), message.getArgAsFloat( 4 ) );
		Vec3f speed( message.getArgAsFloat( 5 ), message.getArgAsFloat( 6 ), message.getArgAsFloat( 7 ) );
		float motionAccel =  message.getArgAsFloat( 8 );
		return Cursor25d( source, sessionId, pos, speed, motionAccel );
	}

private:
	Vec3f mPos25;
	Vec3f mPrevPos25;
	Vec3f mSpeed25;
	float mMotionAccel;
};
	
} } // namespace cinder::tuio

