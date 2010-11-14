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

	Cursor( int32_t sessionId, Vec2f pos, Vec2f speed = Vec2f::zero(), float motionAccel = 0 )
		: ProfileBase( sessionId, pos, speed, motionAccel )
	{}

	// Create from a '2dcur' 'set' message
	static Cursor createFromSetMessage( const osc::Message &message )
	{
		int32_t sessionId = message.getArgAsInt32( 1 );
		Vec2f pos( message.getArgAsFloat( 2 ), message.getArgAsFloat( 3 ) );
		Vec2f speed( message.getArgAsFloat( 4 ), message.getArgAsFloat( 5 ) );
		float motionAccel =  message.getArgAsFloat( 6 );
		return Cursor( sessionId, pos, speed, motionAccel );
	}
};
	
} } // namespace cinder::tuio

