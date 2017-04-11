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

#pragma once

#include "cinder/Cinder.h"

namespace cinder
{
typedef std::shared_ptr<class TimelineItem>	TimelineItemRef;

//! Base interface for anything that can go on a Timeline
class CI_API TimelineItem : public std::enable_shared_from_this<TimelineItem>
{
  public:
	TimelineItem( class Timeline *parent = 0 );
	TimelineItem( class Timeline *parent, void *target, float startTime, float duration );
	virtual ~TimelineItem() {}
	
	//! Returns the item's target pointer
	void* getTarget() const { return mTarget; }

	//! Returns the item's start time
	float			getStartTime() const { return mStartTime; }
	//! Set the items's start time to \a newTime
	void			setStartTime( float newTime );

	//! Returns the item's duration
	float			getDuration() const { updateDuration(); return mDuration; }
	//! Sets the item's duration to \a newDuration.
	void			setDuration( float newDuration );

	//! Returns whether the item starts over when it is complete
	bool			getLoop() const { return mLoop; }
	//! Sets whether the item starts over when it is complete
	void			setLoop( bool doLoop = true ) { mLoop = doLoop; }

	//! Returns whether the item alternates between forward and reverse. Overrides loop when true.
	bool			getPingPong() const { return mPingPong; }
	//! Sets whether the item alternates between forward and reverse. Overrides loop when true.
	void			setPingPong( bool pingPong = true ) { mPingPong = pingPong; }

	//! Returns whether the item ever is marked as complete
	bool			getInfinite() const { return mLoop; }
	//! Sets whether the item ever is marked as complete
	void			setInfinite( bool infinite = true ) { mInfinite = infinite; }

	//! Returns the time of the item's competion, equivalent to getStartTime() + getDuration().
	float			getEndTime() const { return mStartTime + getDuration(); }

	//! Returns a pointer to the item's parent Timeline
	class Timeline*		getParent() const { return mParent; }
	//! Removes the item from its parent Timeline
	void removeSelf();
	//! Marks the item as not completed, and if \a unsetStarted, marks the item as not started
	virtual void reset( bool unsetStarted = false ) { if( unsetStarted ) mHasStarted = false; mComplete = false; }
	
	//! Returns whether the item has started
	bool hasStarted() const { return mHasStarted; }			
	//! Returns whether the item has completed
	bool isComplete() { return mComplete; }
	
	//! Should the item remove itself from the Timeline when it is complete
	bool	getAutoRemove() const { return mAutoRemove; }
	//! Sets whether the item will remove itself from the Timeline when it is complete
	void	setAutoRemove( bool autoRemove = true ) { mAutoRemove = autoRemove; }
	
	virtual void start( bool reverse ) = 0;
	virtual void loopStart() {}
	virtual void update( float relativeTime ) = 0;
	virtual void complete( bool reverse ) = 0;
	//! Call update() only at the beginning of each loop (for example Cues exhibit require this behavior)
	virtual bool 	updateAtLoopStart() { return false; }
	virtual float	calcDuration() const { return mDuration; }
	virtual void	reverse() = 0;
	//! Creates a clone of the item
	virtual TimelineItemRef		clone() const = 0;
	//! Creates a cloned item which runs in reverse relative to a timeline of duration \a timelineDuration
	virtual TimelineItemRef		cloneReverse() const = 0;
	//! go to a specific time, generally called by the parent Timeline only. If \a reverse then playhead is interpreted as retreating rather than advancing.
	void stepTo( float time, bool reverse );
	
	TimelineItemRef thisRef() { return shared_from_this(); }
	
  protected:
	void	setDurationDirty() { mDirtyDuration = true; }
	void	updateDuration() const;
	//! Converts time from absolute to absolute based on item's looping attributes
	float	loopTime( float absTime );
	void	setTarget( void *target ) { mTarget = target; }

	class Timeline	*mParent;

	void	*mTarget;
	float	mStartTime;
	bool	mHasStarted, mHasReverseStarted;
	bool	mComplete, mReverseComplete;
	bool	mMarkedForRemoval;
	bool	mInfinite;
	bool	mLoop, mPingPong;
	bool	mUseAbsoluteTime;
	bool	mAutoRemove;
	int32_t	mLastLoopIteration;
	
	friend class Timeline;
  private:
	mutable float	mDuration, mInvDuration;
	mutable bool	mDirtyDuration; // marked if the virtual calcDuration() needs to be calculated
};

} // namespace cinder

