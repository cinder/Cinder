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

#include "cinder/Tween.h"
#include "cinder/Timeline.h"

#include <algorithm>

using namespace std;

namespace cinder {

TweenBase::TweenBase( void *target, bool copyStartValue, float startTime, float duration, EaseFn easeFunction )
	: TimelineItem( 0, target, startTime, duration ), mCopyStartValue( copyStartValue ), mEaseFunction( easeFunction )
{
}

/*TweenScope::~TweenScope()
{
	for( list<weak_ptr<TimelineItem> >::iterator itemIt = mItems.begin(); itemIt != mItems.end(); ++itemIt ) {
		TimelineItemRef item = itemIt->lock();
		if( item )
			item->removeSelf();
	}
}

TweenScope& TweenScope::operator+=( TimelineItemRef item )
{
	add( item );
	return *this;
}

void TweenScope::add( TimelineItemRef item )
{
	mItems.push_back( item );
}*/

AnimBase::AnimBase( const AnimBase &rhs, void *voidPtr )
	: mVoidPtr( voidPtr )
{
	mParentTimeline = rhs.mParentTimeline;
	if( mParentTimeline ) {
		mParentTimeline->cloneAndReplaceTarget( rhs.mVoidPtr, mVoidPtr );
	}	
}

AnimBase::~AnimBase()
{
	if( mParentTimeline )
		mParentTimeline->removeTarget( mVoidPtr );
}

void AnimBase::set( const AnimBase &rhs )
{
	setParentTimeline( rhs.mParentTimeline );
	if( mParentTimeline ) {
		mParentTimeline->cloneAndReplaceTarget( rhs.mVoidPtr, mVoidPtr );
	}	
}

// Implements move semantics
void AnimBase::setReplace( const AnimBase &rhs )
{
	setParentTimeline( rhs.mParentTimeline );
	if( mParentTimeline ) {
		mParentTimeline->replaceTarget( rhs.mVoidPtr, mVoidPtr );
	}	
}

bool AnimBase::isComplete() const
{
	if( ! mParentTimeline )
		return true;
	else {
		TimelineItemRef lastTween = mParentTimeline->findLastEnd( mVoidPtr );
		if( lastTween )
			return lastTween->isComplete();
		else
			return true;
	}
}

void AnimBase::stop()
{
	if( mParentTimeline )
		mParentTimeline->removeTarget( mVoidPtr );
	mParentTimeline.reset();
}

void AnimBase::setParentTimeline( TimelineRef parentTimeline )
{
	if( mParentTimeline && ( parentTimeline != mParentTimeline ) ) {
		mParentTimeline->removeTarget( mVoidPtr );
	}
	mParentTimeline = parentTimeline;  		
}

// these are to provide a compiler firewall to use Timeline from the TweenOptions
void TweenBase::Options::appendTo( TweenBase &tweenBase, void *endTarget, float offset )
{
	tweenBase.setStartTime( mTimeline->findEndTimeOf( endTarget ) + offset );	
}

void TweenBase::Options::timelineEnd( TweenBase &tweenBase, float offset )
{
	tweenBase.setStartTime( std::max( mTimeline->getDuration(), mTimeline->getCurrentTime() ) + offset );
}

} // namespace cinder