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
#include "cinder/CinderMath.h"
#include "cinder/Easing.h"
#include "cinder/Quaternion.h"

#include <list>

namespace cinder {

class Timeline;
typedef std::shared_ptr<Timeline>		TimelineRef;

template<typename T>
class Tween;
typedef std::function<float (float)> EaseFn;

template<typename T>
class Anim;

template<typename T>
T tweenLerp( const T &start, const T &end, float time )
{
	return start * ( 1 - time ) + end * time;
}

// Specialization of tweenLerp for Quaternions to use slerping
template<>
inline quat tweenLerp( const quat &start, const quat &end, float time )
{
	quat val = glm::slerp( start, end, time );
	glm::vec3 axis = glm::axis( val );
	if( std::isfinite( axis.x ) && std::isfinite( axis.y ) && std::isfinite( axis.z ) )
		return val;
	else
		return quat();
}

template<>
inline dquat tweenLerp( const dquat &start, const dquat &end, float time )
{
	dquat val = glm::slerp( start, end, (double)time );
	glm::dvec3 axis = glm::axis( val );
	if( std::isfinite( axis.x ) && std::isfinite( axis.y ) && std::isfinite( axis.z ) )
		return val;
	else
		return dquat();
}


class CI_API TweenBase : public TimelineItem {
  public:
	typedef std::function<void ()>		StartFn;
	typedef std::function<void ()>		FinishFn;
	typedef std::function<void ()>		UpdateFn;

	TweenBase( void *target, bool copyStartValue, float startTime, float duration, EaseFn easeFunction = easeNone );
	virtual ~TweenBase() {}

	//! change how the tween moves through time
	void	setEaseFn( EaseFn easeFunction ) { mEaseFunction = easeFunction; }
	EaseFn	getEaseFn() const { return mEaseFunction; }

	void			setStartFn( StartFn startFunction ) { mStartFunction = startFunction; }
	StartFn			getStartFn() const { return mStartFunction; }

	void			setReverseStartFn( StartFn reverseStartFunction ) { mReverseStartFunction = reverseStartFunction; }
	StartFn			getReverseStartFn() const { return mReverseStartFunction; }
	
	void			setUpdateFn( UpdateFn updateFunction ) { mUpdateFunction = updateFunction; }									
	UpdateFn		getUpdateFn() const { return mUpdateFunction; }
																																					
	void			setFinishFn( FinishFn finishFn ) { mFinishFunction = finishFn; }
	FinishFn		getFinishFn() const { return mFinishFunction; }

	void			setReverseFinishFn( FinishFn reverseFinishFn ) { mReverseFinishFunction = reverseFinishFn; }
	FinishFn		getReverseFinishFn() const { return mReverseFinishFunction; }
	
	class CI_API Options {
	  protected:
		Options( TimelineRef timeline )
			: mTimeline( timeline )
		{}

		void	appendTo( TweenBase &tweenBase, void *target, float offset );
		void	timelineEnd( TweenBase &tweenBase, float offset );
	  
		TimelineRef		mTimeline;
	};
	
  protected:
	virtual void reset( bool unsetStarted )
	{
		TimelineItem::reset( unsetStarted );
	}

	virtual void complete( bool reverse )
	{
		if( reverse && mReverseFinishFunction )
			mReverseFinishFunction();
		else if( ( ! reverse ) && mFinishFunction )
			mFinishFunction();
	}

  
	StartFn			mStartFunction, mReverseStartFunction;
	UpdateFn		mUpdateFunction;	
	FinishFn		mFinishFunction, mReverseFinishFunction;
  
	EaseFn		mEaseFunction;
	float		mDuration;
	bool		mCopyStartValue;
};

template<typename T>
class TweenRef : public std::shared_ptr<Tween<T> > {
  public:
	TweenRef( const std::shared_ptr<Tween<T> > &sp )
		: std::shared_ptr<Tween<T> >( sp )
	{}
	TweenRef( Tween<T> *tween )
		: std::shared_ptr<Tween<T> >( tween )
	{}
	TweenRef()
		: std::shared_ptr<Tween<T> >()
	{}
};
		
template<typename T>
class Tween : public TweenBase {
  public:
	typedef std::function<T (const T&, const T&, float)>	LerpFn;

	// build a tween with a target, target value, duration, and optional ease function
	Tween( T *target, T endValue, float startTime, float duration,
			EaseFn easeFunction = easeNone, LerpFn lerpFunction = &tweenLerp<T> )
		: TweenBase( target, true, startTime, duration, easeFunction ), mStartValue( *target ), mEndValue( endValue ), mLerpFunction( lerpFunction )
	{
	}
	
	Tween( T *target, T startValue, T endValue, float startTime, float duration,
			EaseFn easeFunction = easeNone, LerpFn lerpFunction = &tweenLerp<T> )
		: TweenBase( target, false, startTime, duration, easeFunction ), mStartValue( startValue ), mEndValue( endValue ), mLerpFunction( lerpFunction )
	{
	}
	
	virtual ~Tween() {}
	
	//! Returns the starting value for the tween. If the tween will copy its target's value upon starting (isCopyStartValue()) and the tween has not started, this returns the value of its target when the tween was created
	T	getStartValue() const { return mStartValue; }
	T	getEndValue() const { return mEndValue; }			
	T*	getTarget() const { return reinterpret_cast<T*>( mTarget ); }
	
	//! Returns whether the tween will copy its target's value upon starting
	bool	isCopyStartValue() { return mCopyStartValue; }

	void	setLerpFn( const LerpFn &lerpFn ) { mLerpFunction = lerpFn; }

	//! Returns a TweenRef<T> to \a this
	TweenRef<T>		getThisRef(){ return TweenRef<T>( std::static_pointer_cast<Tween<T> >( shared_from_this() ) ); }


	class Options : public TweenBase::Options {
	  public:
		Options&	startFn( const TweenBase::StartFn &startFn ) { mTweenRef->setStartFn( startFn ); return *this; }
		Options&	reverseStartFn( const TweenBase::StartFn &reverseStartFn ) { mTweenRef->setReverseStartFn( reverseStartFn ); return *this; }
		Options&	updateFn( const TweenBase::UpdateFn &updateFn ) { mTweenRef->setUpdateFn( updateFn ); return *this; }
		Options&	finishFn( const TweenBase::FinishFn &finishFn ) { mTweenRef->setFinishFn( finishFn ); return *this; }
		Options&	reverseFinishFn( const TweenBase::FinishFn &reverseFinishFn ) { mTweenRef->setReverseFinishFn( reverseFinishFn ); return *this; }
		Options&	easeFn( const EaseFn &easeFunc ) { mTweenRef->setEaseFn( easeFunc ); return *this; }
		Options&	delay( float delayAmt ) { mTweenRef->setStartTime( mTweenRef->getStartTime() + delayAmt ); return *this; }
		Options&	startTime( float time ) { mTweenRef->setStartTime( time ); return *this; }
		Options&	autoRemove( bool remove = true ) { mTweenRef->setAutoRemove( remove ); return *this; }
		Options&	loop( bool doLoop = true ) { mTweenRef->setLoop( doLoop ); return *this; }
		Options&	pingPong( bool doPingPong = true ) { mTweenRef->setPingPong( doPingPong ); return *this; }
		Options&	infinite( bool doInfinite = true ) { mTweenRef->setInfinite( doInfinite ); return *this; }
		Options&	timelineEnd( float offset = 0 ) { TweenBase::Options::timelineEnd( *mTweenRef, offset ); return *this; }
		template<typename Y>
		Options&	appendTo( Anim<Y> *endTarget, float offset = 0 ) { TweenBase::Options::appendTo( *mTweenRef, endTarget->ptr(), offset ); return *this; }	
		Options&	appendTo( void *endTarget, float offset = 0 ) { TweenBase::Options::appendTo( *mTweenRef, endTarget, offset ); return *this; }	
		Options&	lerpFn( const typename Tween<T>::LerpFn &lerpFn ) { mTweenRef->setLerpFn( lerpFn ); return *this; }
		
		operator TweenRef<T>() { return mTweenRef; }

	  protected:
		Options( TweenRef<T> tweenRef, TimelineRef timeline )
			: TweenBase::Options( timeline ), mTweenRef( tweenRef )
		{}
				
		TweenRef<T>		mTweenRef;
		
		friend class Timeline;
	};


  protected:
	virtual void reverse()
	{
		std::swap( mStartValue, mEndValue );
	}

	virtual TimelineItemRef	clone() const
	{
		std::shared_ptr<Tween<T> > result( new Tween<T>( *this ) );
		result->mCopyStartValue = false;
		return result;
	}
	
	virtual TimelineItemRef	cloneReverse() const
	{
		std::shared_ptr<Tween<T> > result( new Tween<T>( *this ) );
		std::swap( result->mStartValue, result->mEndValue );
		result->mCopyStartValue = false;
		return result;
	}
	
	virtual void start( bool reverse )
	{
		if( mCopyStartValue )
			mStartValue = *(reinterpret_cast<T*>( mTarget ) );
		if( reverse && mReverseStartFunction )
			mReverseStartFunction();
		else if( ( ! reverse ) && mStartFunction )
			mStartFunction();
	}
	
	virtual void update( float relativeTime )
	{
		*reinterpret_cast<T*>(mTarget) = mLerpFunction( mStartValue, mEndValue, mEaseFunction( relativeTime ) );
		if( mUpdateFunction )
			mUpdateFunction();
	}
	

	T	mStartValue, mEndValue;	
	
	LerpFn				mLerpFunction;
};

template<typename T>
class FnTween : public Tween<T> {
  public:
	FnTween( std::function<void (T)> fn, T startValue, T endValue, float startTime, float duration, EaseFn easeFunction = easeNone, typename Tween<T>::LerpFn lerpFunction = &tweenLerp<T> )
		: Tween<T>( &mValue, startValue, endValue, startTime, duration, easeFunction, lerpFunction ), mFn( fn ), mValue( startValue )
	{
	}
	
	virtual void update( float relativeTime )
	{
		Tween<T>::update( relativeTime );
		if( mFn )
			mFn( mValue );
	}	
	
	std::function<void (T)>		mFn;
	T							mValue;
};

template<typename T>
class FnTweenRef : public TweenRef<T> {
  public:
	FnTweenRef( const std::shared_ptr<FnTween<T> > &sp )
		: TweenRef<T>( sp )
	{}
	FnTweenRef( FnTween<T> *fnTween )
		: TweenRef<T>( fnTween )
	{}
	FnTweenRef()
		: TweenRef<T>()
	{}
};

class CI_API AnimBase {
  public:
  	//! removes self from Timeline
	void 	stop();

	//! returns false if any tweens are active on 'this', otherwise true
	bool isComplete() const;
	
	//! returns the parent timeline for the Anim<> or NULL if there is none
	TimelineRef	getParent() const { return mParentTimeline; }

  protected:
	AnimBase( void *voidPtr ) : mVoidPtr( voidPtr ) {}
	AnimBase( const AnimBase &rhs, void *voidPtr );
	~AnimBase();
	
	void 	set( const AnimBase &rhs );
	void 	setReplace( const AnimBase &rhs );
	
	void		setParentTimeline( TimelineRef parentTimeline );

	void			*mVoidPtr;	
	TimelineRef		mParentTimeline;
};

template<typename T>
class Anim : public AnimBase {
  public:
	Anim()
		: AnimBase( &mValue )
	{}
  	Anim( T value ) 
		: AnimBase( &mValue), mValue( value )
	{}
  	Anim( const Anim<T> &rhs ) // normal copy constructor
		: AnimBase( rhs, &mValue ), mValue( rhs.mValue )
  	{}
  	
	const T&	operator()() const { return mValue; }
	T&			operator()() { return mValue; }	
	
	operator const T&() const { return mValue; }	
	Anim<T>& operator=( const Anim &rhs ) { // copy assignment
		if( this != &rhs ) {
			set( rhs );
			mValue = rhs.mValue;
		}
		return *this;
  	}

	Anim( Anim &&rhs ) // move constructor
		: AnimBase( &mValue )
	{
		setReplace( rhs );
		rhs.mParentTimeline.reset(); // blow away rhs's tweens due to move semantics
		mValue = rhs.mValue;
	}
	Anim<T>& operator=( Anim &&rhs ) { // move assignment
		if( this != &rhs ) {
			setReplace( rhs );
			rhs.mParentTimeline.reset(); // blow away rhs's tweens due to move semantics
			mValue = rhs.mValue;
		}
		return *this;
	}

	Anim<T>& operator=( T value ) { mValue = value; return *this; }

	const T&	value() const { return mValue; }
	T&			value() { return mValue; }
  	
  	const T*		ptr() const { return &mValue; }
  	T*				ptr() { return &mValue; }

  protected:

	friend class Timeline;

	T				mValue;
};

//typedef boost::instrusive_ptr<TweenBase>	TweenBaseRef;

/*class TweenScope {
  public:
	TweenScope() {}
	TweenScope( const TweenScope &rhs ) {}	// do nothing for copy; these are our tweens alone
	TweenScope& operator=( const TweenScope &rhs ) { return *this; }	// do nothing for copy; these are our tweens alone	
	~TweenScope();
	
	TweenScope& operator+=( TimelineItemRef item );
	void add( TimelineItemRef item );

  private:
	std::list<std::weak_ptr<TimelineItem> >		mItems;
};*/

} //namespace cinder