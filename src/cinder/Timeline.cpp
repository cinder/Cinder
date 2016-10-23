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

#include "cinder/Timeline.h"

#include <algorithm>
#include <vector>

using namespace std;

namespace cinder {

////////////////////////////////////////////////////////////////////////////////////////
// Timeline
typedef std::multimap<void*,TimelineItemRef>::iterator s_iter;
typedef std::multimap<void*,TimelineItemRef>::const_iterator s_const_iter;

Timeline::Timeline()
	: TimelineItem( 0, 0, 0, 0 ), mDefaultAutoRemove( true ), mCurrentTime( 0 )
{
	mUseAbsoluteTime = true;
}

Timeline::Timeline( const Timeline &rhs )
	: TimelineItem( rhs ), mDefaultAutoRemove( rhs.mDefaultAutoRemove ), mCurrentTime( rhs.mCurrentTime )
{
	for( s_const_iter iter = rhs.mItems.begin(); iter != rhs.mItems.end(); ++iter ) {
		mItems.insert( make_pair( iter->first, iter->second->clone() ) );
	}
}

void Timeline::step( float timestep )
{
	mCurrentTime += timestep;
	stepTo( mCurrentTime, timestep < 0 );
}

void Timeline::stepTo( float absoluteTime )
{	
	bool reverse = mCurrentTime > absoluteTime;
	mCurrentTime = absoluteTime;
	
	eraseMarked();
	
	// we need to cache the end(). If a tween's update() fn or similar were to manipulate
	// the list of items by adding new ones, we'll have invalidated our iterator.
	// Deleted items are never removed immediately, but are marked for deletion.
	s_iter endItem = mItems.end();
	for( s_iter iter = mItems.begin(); iter != endItem; ++iter ) {
		iter->second->stepTo( mCurrentTime, reverse );
		if( iter->second->isComplete() && iter->second->getAutoRemove() )
			iter->second->mMarkedForRemoval = true;
	}
	
	eraseMarked();	
}

CueRef Timeline::add( const std::function<void ()> &action, float atTime )
{
	CueRef newCue( new Cue( action, atTime ) );
	newCue->setAutoRemove( mDefaultAutoRemove );
	insert( newCue );
	return newCue;
}

void Timeline::clear()
{
	mItems.clear();	
}

void Timeline::appendPingPong()
{
	vector<TimelineItemRef> toAppend;
	
	float duration = mDuration;
	for( s_iter iter = mItems.begin(); iter != mItems.end(); ++iter ) {
		TimelineItemRef cloned = iter->second->cloneReverse();
		cloned->mStartTime = duration + ( duration - ( cloned->mStartTime + cloned->mDuration ) );
		toAppend.push_back( cloned );
	}
	
	for( vector<TimelineItemRef>::const_iterator appIt = toAppend.begin(); appIt != toAppend.end(); ++appIt ) {
		mItems.insert( make_pair( (*appIt)->mTarget, *appIt ) );
	}
	
	setDurationDirty();
}

void Timeline::apply( TimelineItemRef item )
{
	if( item->getTarget() )
		removeTarget( item->getTarget() );
	insert( item );
}

void Timeline::add( TimelineItemRef item )
{
	item->mParent = this;
	item->mStartTime = mCurrentTime;
	mItems.insert( make_pair( item->mTarget, item ) );
	setDurationDirty();
}

void Timeline::insert( TimelineItemRef item )
{
	item->mParent = this;
	mItems.insert( make_pair( item->mTarget, item ) );
	setDurationDirty();
}

// remove all items which have been marked for removal
void Timeline::eraseMarked()
{
	bool needRecalc = false;
	for( s_iter iter = mItems.begin(); iter != mItems.end(); ) {
		if( iter->second->mMarkedForRemoval ) {
			mItems.erase( iter++ );
			needRecalc = true;
		}
		else
			++iter;
	}
	
	if( needRecalc )
		setDurationDirty();
}	


float Timeline::calcDuration() const
{
	float duration = 0;
	for( s_const_iter iter = mItems.begin(); iter != mItems.end(); ++iter ) {
		duration = std::max( iter->second->getEndTime(), duration );
	}
	
	return duration;
}

TimelineItemRef Timeline::find( void *target ) const
{
	s_const_iter iter = mItems.begin();
	while( iter != mItems.end() ) {
		if( iter->second->getTarget() == target )
			return iter->second;
		++iter;
	}
	
	return TimelineItemRef(); // failed returns null tween
}

TimelineItemRef Timeline::findLast( void *target ) const
{
	s_const_iter result = mItems.end();
	for( s_const_iter iter = mItems.begin(); iter != mItems.end(); ++iter ) {
		if( iter->second->getTarget() == target && ( ! iter->second->mMarkedForRemoval ) ) {
			if( result == mItems.end() )
				result = iter;
			else if( iter->second->getStartTime() > result->second->getStartTime() )
				result = iter;
		}
	}
	
	return (result == mItems.end() ) ? TimelineItemRef() : result->second;
}

TimelineItemRef Timeline::findLastEnd( void *target ) const
{
	pair<s_const_iter,s_const_iter> range = mItems.equal_range( target );

	s_const_iter result = mItems.end();
	for( s_const_iter iter = range.first; iter != range.second; ++iter ) {
		if( iter->second->getTarget() == target && ( ! iter->second->mMarkedForRemoval ) ) {
			if( result == mItems.end() )
				result = iter;
			else if( iter->second->getEndTime() > result->second->getEndTime() )
				result = iter;
		}
	}
	
	if( result != mItems.end() )
		return result->second;
	else
		return TimelineItemRef();
}

float Timeline::findEndTimeOf( void *target, bool *found ) const
{
	pair<s_const_iter,s_const_iter> range = mItems.equal_range( target );

	s_const_iter result = mItems.end();
	for( s_const_iter iter = range.first; iter != range.second; ++iter ) {
		if( iter->second->getTarget() == target && ( ! iter->second->mMarkedForRemoval ) ) {
			if( result == mItems.end() )
				result = iter;
			else if( iter->second->getEndTime() > result->second->getEndTime() )
				result = iter;
		}
	}
	
	if( result != mItems.end() ) {
		if( found )
			*found = true;
		return result->second->getEndTime();
	}
	else {
		if( found )
			*found = false;
		return getCurrentTime();
	}
}

void Timeline::remove( TimelineItemRef item )
{
	for( s_iter iter = mItems.begin(); iter != mItems.end(); ++iter ) {
		if( iter->second == item ) {
			iter->second->mMarkedForRemoval = true;
			break;
		}
	}
}

void Timeline::removeTarget( void *target )
{
	if( target == 0 )
		return;
		
	pair<s_iter,s_iter> range = mItems.equal_range( target );
	for( s_iter iter = range.first; iter != range.second; ++iter )
		iter->second->mMarkedForRemoval = true;

	setDurationDirty();
}

void Timeline::cloneAndReplaceTarget( void *target, void *replacementTarget )
{
	if( target == 0 )
		return;

	pair<s_iter,s_iter> range = mItems.equal_range( target );

	vector<TimelineItemRef> newItems;
	newItems.reserve( std::distance( range.first, range.second ) );
	for( s_iter iter = range.first; iter != range.second; ++iter ) {
		newItems.push_back( iter->second->clone() );
		newItems.back()->setTarget( replacementTarget );
	}

	for( vector<TimelineItemRef>::iterator newItemIt = newItems.begin(); newItemIt != newItems.end(); ++newItemIt )
		mItems.insert( make_pair( replacementTarget, *newItemIt ) );

	setDurationDirty();
}

void Timeline::replaceTarget( void *target, void *replacementTarget )
{
	if( target == nullptr )
		return;

	s_iter iter = mItems.find( target );
	while( ( iter != mItems.end() ) && ( iter->first == target ) ) {
		iter->second->setTarget( replacementTarget );
		mItems.insert( make_pair( replacementTarget, iter->second ) );
		iter = mItems.erase( iter );
	}
}

void Timeline::reset( bool unsetStarted )
{
	TimelineItem::reset( unsetStarted );
	
	for( s_iter iter = mItems.begin(); iter != mItems.end(); ++iter )
		iter->second->reset( unsetStarted );
}


void Timeline::loopStart()
{
	reset( false );
}

void Timeline::reverse()
{
	for( s_iter iter = mItems.begin(); iter != mItems.end(); ++iter )
		iter->second->reverse();
}

TimelineItemRef Timeline::clone() const
{
	return TimelineItemRef( new Timeline( *this ) );
}

TimelineItemRef Timeline::cloneReverse() const
{
	Timeline *result = new Timeline( *this );
	for( s_iter iter = result->mItems.begin(); iter != result->mItems.end(); ++iter ) {
		iter->second->reverse();
		iter->second->mStartTime = mDuration + ( mDuration - ( iter->second->mStartTime + iter->second->mDuration ) );		
	}
	return TimelineItemRef( result );
}

void Timeline::update( float absTime )
{
	absTime = loopTime( absTime );
	stepTo( absTime );
}

void Timeline::itemTimeChanged( TimelineItem * /*item*/ )
{
	setDurationDirty();
}

////////////////////////////////////////////////////////////////////////////////////////
// Cue
Cue::Cue( const std::function<void ()> &fn, float atTime )
	: TimelineItem( 0, 0, atTime, 0 ), mFunction( fn )
{
}

void Cue::loopStart()
{
	if( mFunction )
		mFunction();
}

TimelineItemRef Cue::clone() const
{
	return TimelineItemRef( new Cue( *this ) );
}

TimelineItemRef Cue::cloneReverse() const
{
	return TimelineItemRef( new Cue( *this ) );
}

} // namespace cinder