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
#include "cinder/TimelineItem.h"
#include "cinder/Easing.h"
#include "cinder/Tween.h"

#include <vector>
#include <list>
#include <map>
#include <algorithm>

namespace cinder {

typedef std::shared_ptr<class Cue>			CueRef;
typedef std::shared_ptr<class Timeline>		TimelineRef;
	
class CI_API Timeline : public TimelineItem {		
  public:
	//! Creates a new timeline, defaulted to infinite
	static TimelineRef	create() { TimelineRef result( new Timeline() ); result->setInfinite( true ); return result; }

	//! Advances time a specified amount and evaluates items
	void	step( float timestep );
	//! Goes to a specific time and evaluates items.
	void	stepTo( float absoluteTime );
	
	//! Returns the timeline's most recent current time
	float	getCurrentTime() const { return mCurrentTime; }
	
	//! Replaces any existing tweens on the \a target with a new tween at the timeline's current time
	template<typename T>
	typename Tween<T>::Options apply( Anim<T> *target, T endValue, float duration, EaseFn easeFunction = easeNone, typename Tween<T>::LerpFn lerpFunction = &tweenLerp<T> )
	{
		target->setParentTimeline( thisRef() );
		return applyPtr( target->ptr(), endValue, duration, easeFunction, lerpFunction );
	}

	//! Replaces any existing tweens on the \a target with a new tween at the timeline's current time
	template<typename T>
	typename Tween<T>::Options apply( Anim<T> *target, T startValue, T endValue, float duration, EaseFn easeFunction = easeNone, typename Tween<T>::LerpFn lerpFunction = &tweenLerp<T> )
	{
		target->setParentTimeline( thisRef() );
		return applyPtr( target->ptr(), startValue, endValue, duration, easeFunction, lerpFunction );
	}

	//! Creates a new tween and adds it to the end of the last tween on \a target, or if no existing tween matches the target, the current time.
	template<typename T>
	typename Tween<T>::Options appendTo( Anim<T> *target, T endValue, float duration, EaseFn easeFunction = easeNone, typename Tween<T>::LerpFn lerpFunction = &tweenLerp<T> )
	{
		target->setParentTimeline( thisRef() );
		return appendToPtr( target->ptr(), endValue, duration, easeFunction, lerpFunction );
	}

	//! Creates a new tween and adds it to the end of the last tween on \a target, or if no existing tween matches the target, the current time.
	template<typename T>
	typename Tween<T>::Options appendTo( Anim<T> *target, T startValue, T endValue, float duration, EaseFn easeFunction = easeNone, typename Tween<T>::LerpFn lerpFunction = &tweenLerp<T> )
	{
		target->setParentTimeline( thisRef() );
		return appendToPtr( target->ptr(), startValue, endValue, duration, easeFunction, lerpFunction );
	}

	//! Replaces any existing tweens on the \a target with a new tween at the timeline's current time. Consider the apply( Anim<T>* ) variant unless you have an advanced use case.
	template<typename T>
	typename Tween<T>::Options applyPtr( T *target, T endValue, float duration, EaseFn easeFunction = easeNone, typename Tween<T>::LerpFn lerpFunction = &tweenLerp<T> )
	{
		TweenRef<T> newTween( new Tween<T>( target, endValue, mCurrentTime, duration, easeFunction, lerpFunction ) );
		newTween->setAutoRemove( mDefaultAutoRemove );
		apply( newTween );
		return typename Tween<T>::Options( newTween, thisRef() );
	}
	
	//! Replaces any existing tweens on the \a target with a new tween at the timeline's current time. Consider the apply( Anim<T>* ) variant unless you have an advanced use case.
	template<typename T>
	typename Tween<T>::Options applyPtr( T *target, T startValue, T endValue, float duration, EaseFn easeFunction = easeNone, typename Tween<T>::LerpFn lerpFunction = &tweenLerp<T> )
	{
		TweenRef<T> newTween( new Tween<T>( target, startValue, endValue, mCurrentTime, duration, easeFunction, lerpFunction ) );
		newTween->setAutoRemove( mDefaultAutoRemove );
		apply( newTween );
		return typename Tween<T>::Options( newTween, thisRef() );
	}

	//! Creates a new tween and adds it to the end of the last tween on \a target, or if no existing tween matches the target, the current time. Consider the appendTo( Anim<T>* ) variant unless you have an advanced use case.
	template<typename T>
	typename Tween<T>::Options appendToPtr( T *target, T endValue, float duration, EaseFn easeFunction = easeNone, typename Tween<T>::LerpFn lerpFunction = &tweenLerp<T> )
	{
		float startTime = findEndTimeOf( target );
		TweenRef<T> newTween( new Tween<T>( target, endValue, std::max( mCurrentTime, startTime ), duration, easeFunction, lerpFunction ) );
		newTween->setAutoRemove( mDefaultAutoRemove );
		insert( newTween );
		return typename Tween<T>::Options( newTween, thisRef() );
	}
	
	//! Creates a new tween and adds it to the end of the last tween on \a target, or if no existing tween matches the target, the current time. Consider the appendTo( Anim<T>* ) variant unless you have an advanced use case.
	template<typename T>
	typename Tween<T>::Options appendToPtr( T *target, T startValue, T endValue, float duration, EaseFn easeFunction = easeNone, typename Tween<T>::LerpFn lerpFunction = &tweenLerp<T> )
	{
		float startTime = findEndTimeOf( target );
		TweenRef<T> newTween( new Tween<T>( target, startValue, endValue, std::max( mCurrentTime, startTime ), duration, easeFunction, lerpFunction ) );
		newTween->setAutoRemove( mDefaultAutoRemove );
		insert( newTween );
		return typename Tween<T>::Options( newTween, thisRef() );
	}

	//! add a cue to the Timeline add the start-time \a atTime
	CueRef add( const std::function<void ()> &action, float atTime );

	template<typename T>
	FnTweenRef<T> applyFn( const std::function<void (T)> &fn, T startValue, T endValue, float duration, const EaseFn &easeFunction = easeNone, const typename Tween<T>::LerpFn &lerpFunction = &tweenLerp<T> )
	{
		FnTweenRef<T> newTween( new FnTween<T>( fn, startValue, endValue, mCurrentTime, duration, easeFunction, lerpFunction ) );
		newTween->setAutoRemove( mDefaultAutoRemove );
		apply( newTween );
		return newTween;
	}

	//! Appends to the end of the timeline mirror images of all items
	void appendPingPong();

	//! Replaces any existing TimelineItems that match \a item's target and adds \a item to the timeline. Safe to use from callback fn's.
	void apply( TimelineItemRef item );
	
	//! add an item to the timeline at the current time. Safe to use from callback fn's.
	void add( TimelineItemRef item );
	//! adds an item to the timeline. Its start time is not modified. Safe to use from callback fn's.
	void insert( TimelineItemRef item );
	//! adds an item to the timeline, setting its startTime to be at \a atTime. Safe to use from callback fn's.
	void insert( TimelineItemRef item, float atTime ) { item->mStartTime = atTime; insert( item ); }

	//! Returns the number of items in the Timeline
	size_t				getNumItems() const { return mItems.size(); }
	//! Returns true if there are no items in the Timeline
	bool				empty() const { return mItems.empty(); }
	//! Returns the first item in the timeline the target of which matches \a target
	TimelineItemRef		find( void *target ) const;
	//! Returns the latest-starting item in the timeline the target of which matches \a target
	TimelineItemRef		findLast( void *target ) const;
	//! Returns the latest-end item in the timeline the target of which matches \a target
	TimelineItemRef		findLastEnd( void *target ) const;
	//! Returns the end of the latest-ending item in the timeline the target of which matches \a target, or the current time if it's not found. \a found can store whether a related item was found.
	float				findEndTimeOf( void *target, bool *found = NULL ) const;
	//! Removes the TimelineItem \a item from the Timeline. Safe to use from callback fn's.
	void				remove( TimelineItemRef item );
	//! Removes all TimelineItems whose target matches \a target
	void				removeTarget( void *target );
	//! Clones all TimelineItems whose target matches \a target, but replacing their target with \a replacementTarget
	void				cloneAndReplaceTarget( void *target, void *replacementTarget );
	//! Replaces the target of all TimelineItems whose target matches \a target, with \a replacementTarget
	void				replaceTarget( void *target, void *replacementTarget );
	
	//! Remove all tweens from the Timeline. Do not call from callback fn's.
	void clear();
	//! Sets the time to zero, marks all tweens as not completed, and if \a unsetStarted, marks the tweens as not started. Do not call from callback fn's.
	void reset( bool unsetStarted = false );

	//! Sets the default \a autoRemove value for all future TimelineItems added to the Timeline
	void	setDefaultAutoRemove( bool defaultAutoRemove ) { mDefaultAutoRemove = defaultAutoRemove; }
	//! Returns the default \a autoRemove value for all future TimelineItems added to the Timeline
	bool	getDefaultAutoRemove() const { return mDefaultAutoRemove; }

	//! Call this to notify the Timeline if the \a item's start-time or duration has changed. Advanced use cases only.
	void	itemTimeChanged( TimelineItem *item );

	TimelineRef	thisRef()
	{
		TimelineItemRef thisTimelineItem = TimelineItem::thisRef();
		TimelineRef result = std::static_pointer_cast<Timeline>( thisTimelineItem );
		return result;
	}

	//! \cond
	virtual void	stepTo( float absoluteTime, bool /*reverse*/ ) { stepTo( absoluteTime ); }
	//! \endcond
	
  protected:  
  	Timeline();
	
	virtual void reverse();
	virtual TimelineItemRef cloneReverse() const;
	virtual TimelineItemRef clone() const;
	virtual void start( bool /*reverse*/ ) {} // no-op
	virtual void loopStart();
	virtual void update( float absTime );
	virtual void complete( bool /*reverse*/ ) {} // no-op

	void						eraseMarked();
	virtual float				calcDuration() const;

	bool						mDefaultAutoRemove;
	float						mCurrentTime;
	
	std::multimap<void*,TimelineItemRef>		mItems;
	
  private:
	Timeline( const Timeline &rhs ); // private to prevent copying; use clone() method instead
	Timeline& operator=( const Timeline &rhs ); // not defined to prevent copying
};

class CI_API Cue : public TimelineItem {
  public:
	Cue( const std::function<void ()> &fn, float atTime = 0 );

	CueRef	create( const std::function<void ()> &fn, float atTime = 0 ) { return CueRef( new Cue( fn, atTime ) ); }

	void					setFn( const std::function<void ()> &fn ) { mFunction = fn; }
	std::function<void ()>	getFn() const { return mFunction; }
	
  protected:
	virtual void				reverse() { /* no-op */ }
	virtual TimelineItemRef	cloneReverse() const;
	virtual TimelineItemRef clone() const;

	virtual void start( bool /*reverse*/ ) {} // starting is a no-op for Cues
	virtual void loopStart();
	virtual void update( float /*relativeTime*/ ) {} // update is a no-op for Cues
	virtual void complete( bool /*reverse*/ ) {} // completion is a no-op for Cues
	virtual bool updateAtLoopStart() { return true; }
  
	std::function<void ()>		mFunction;
};

} // namespace cinder