/*
 Copyright (c) 2011, The Cinder Project, All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Based on the sc-Choreograph CinderBlock by David Wicks: http://sansumbrella.com/

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

#include "cinder/TimelineItem.h"
#include "cinder/Timeline.h"
#include "cinder/CinderMath.h"

#include <algorithm>

namespace cinder {

TimelineItem::TimelineItem( class Timeline *parent )
	: mParent( parent ), mTarget( 0 ), mStartTime( 0 ), mDirtyDuration( false ), mDuration( 0 ), mInvDuration( 0 ), mHasStarted( false ), mHasReverseStarted( false ),
		mComplete( false ), mReverseComplete( false ), mMarkedForRemoval( false ), mAutoRemove( true ),
		mInfinite( false ), mLoop( false ), mPingPong( false ), mLastLoopIteration( -1 ), mUseAbsoluteTime( false )
{
}

TimelineItem::TimelineItem( Timeline *parent, void *target, float startTime, float duration )
	: mParent( parent ), mTarget( target ), mStartTime( startTime ), mDirtyDuration( false ), mDuration( std::max( duration, 0.0f ) ), mInvDuration( duration <= 0 ? 0 : (1 / duration) ),
		mHasStarted( false ), mHasReverseStarted( false ), mComplete( false ), mReverseComplete( false ), mMarkedForRemoval( false ), mAutoRemove( true ),
		mInfinite( false ), mLoop( false ), mPingPong( false ), mLastLoopIteration( -1 ), mUseAbsoluteTime( false )
{
}

void TimelineItem::removeSelf()
{
	mMarkedForRemoval = true;
}

void TimelineItem::stepTo( float newTime, bool reverse )
{
	if( mMarkedForRemoval )
		return;

	updateDuration();

	const float absTime = newTime - mStartTime;
	const float endTime = mStartTime + mDuration;

	if( ( ! mHasReverseStarted ) && reverse && ( newTime < mStartTime ) ) {
		// first update the current time to be the start time
		update( ( mUseAbsoluteTime ) ? mStartTime : 0 );
		// then issue reverse start
		mHasReverseStarted = true;
		mHasStarted = false;
		start( true );
	}	
	else if( newTime >= mStartTime ) {
		float relTime;
		if( mPingPong ) {
			relTime = math<float>::fmod( absTime * mInvDuration, 2 ); // varies from 0-2
			if( relTime > 1 )
				relTime = ( 2 - relTime );
		}
		else if( mLoop ) {
			relTime = math<float>::fmod( absTime * mInvDuration, 1 );
		}
		else
			relTime = math<float>::min( absTime * mInvDuration, 1 );
		
		if( ( ! mHasStarted ) && ( ! reverse ) ) {
			mHasStarted = true;
			mHasReverseStarted = false;
			mLastLoopIteration = 0;
			loopStart();
			start( false );
		}
		
		float time = ( mUseAbsoluteTime ) ? absTime : relTime;

		// accommodate a tween with a duration <= 0
		if( ( ! mUseAbsoluteTime ) && ( mInvDuration <= 0 ) )
			time = 1.0f;

		if( mLoop || mPingPong ) {
			int32_t loopIteration = static_cast<int32_t>( ( newTime - mStartTime ) * mInvDuration );
			if( loopIteration != mLastLoopIteration ) {
				mLastLoopIteration = loopIteration;
				loopStart();
				update( time );
			}
			else
				update( time );
		}
		else
			update( time );
	}

	if( newTime < endTime ) {
		if( ( ! mReverseComplete ) && reverse ) {
			mReverseComplete = true;
			mComplete = false;
			complete( true );
		}
	}
	else if( ( ! mLoop ) && ( ! mInfinite ) ) { // newTime >= endTime
		if( ( ! mComplete ) && ( ! reverse ) ) {
			mComplete = true;
			mReverseComplete = false;
			complete( false );
		}
	}
}

void TimelineItem::setStartTime( float time )
{
	mStartTime = time;
	if( mParent )
		mParent->itemTimeChanged( this );
}

void TimelineItem::updateDuration() const
{
	if( mDirtyDuration ) {
		mDuration = std::max( calcDuration(), 0.0f );
		mInvDuration = ( mDuration == 0 ) ? 1.0f : ( 1.0f / mDuration );
		mDirtyDuration = false;
	}
}

void TimelineItem::setDuration( float duration )
{
	mDuration = duration;
	mInvDuration = duration == 0 ? 1 : ( 1 / duration );
	if( mParent )
		mParent->itemTimeChanged( this );
}

float TimelineItem::loopTime( float absTime )
{
	float result = absTime;
	
	if( mPingPong ) {
		result = math<float>::fmod( result * mInvDuration, 2 ); // varies from 0-2
		if( result <= 1 )
			result *= mDuration;
		else
			result = ( 2 - result ) * mDuration;
	}
	else if( mLoop ) {
		result = math<float>::fmod( result * mInvDuration, 1 );
		result *= mDuration;
	}

	return result;
}

} // namespace cinder
