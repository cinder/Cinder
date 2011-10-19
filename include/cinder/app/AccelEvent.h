/*
 Copyright (c) 2010, The Cinder Project: http://libcinder.org
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
#include "cinder/app/Event.h"
#include "cinder/Vector.h"
#include "cinder/Quaternion.h"

namespace cinder { namespace app {

//! Represents a single acceleration value
class AccelEvent : public Event {
  public:
	AccelEvent( const Vec3f &data, const Vec3f &rawData, const Vec3f &prevData, const Vec3f &prevRawData )
		: Event(), mData( data ), mRawData( rawData ), mPrevData( prevData ), mPrevRawData( prevRawData )
	{
	}
	
	//! Returns the filtered data for this event as a Vec3f representing the forces applied to the device
	Vec3f	getData() const { return mData; }
	//! Returns the unfiltered data for this event as a Vec3f representing the forces applied to the device
	Vec3f	getRawData() const { return mRawData; }
	//! Returns the filtered data for the previous event as a Vec3f representing the forces applied to the device
	Vec3f	getPrevData() const { return mPrevData; }
	//! Returns the raw data for the previous event as a Vec3f representing the forces applied to the device
	Vec3f	getPrevRawData() const { return mPrevRawData; }

	//! Returns whether this acceleration constitutes a shake, as defined by an acceleration of magnitude >= \a shakeDelta
	bool	isShake( float shakeDelta = 2.2f ) const
	{ return (mRawData - mPrevRawData).lengthSquared() > shakeDelta * shakeDelta; }
	
	Vec2f getPolarPlaneVector() const // (r, theta), theta is in degrees
	{ 
		float r = math<float>::sin( M_PI*(mData.z+1) / 2 );
		float theta = toDegrees( math<float>::asin( mData.y / math<float>::sqrt( mData.x*mData.x + mData.y*mData.y )) );

		if( mData.x < 0 ) {		
			theta = -theta + 180.0f;
		}
		else {
			if( mData.y < 0 ) {
				theta = theta + 360.0f;
			}
		}
		
		return Vec2f( r, theta );			
	}

	Vec2f getPlaneVector() const
	{
		Vec2f v = getPolarPlaneVector();
		return Vec2f( -v.x * math<float>::cos( toRadians( v.y ) ), v.x * math<float>::sin( toRadians( v.y ) ) );
	}

	//! Returns a matrix representing a transformation from an upright orientation (0,-1,0) to the current orientation
	Matrix44f getMatrix() const
	{
		return Quatf( Vec3f( 0, -1, 0 ), mData.normalized() ).toMatrix44();
	}
	
  private:
	Vec3f		mData, mPrevData;
	Vec3f		mRawData, mPrevRawData;
};

// For convenience only
inline std::ostream& operator<<( std::ostream &out, const AccelEvent &event )
{
	out << event.getData() << " raw: " << event.getRawData();
	return out;
}


} } // namespace cinder::app
