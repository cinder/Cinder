/*
 Copyright (c) 2014, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

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

#include "cinder/Export.h"
#include "cinder/audio/Buffer.h"

#include <list>
#include <atomic>
#include <functional>
#include <string>

namespace cinder { namespace audio {

typedef std::shared_ptr<class Context>		ContextRef;
typedef std::shared_ptr<class Node>			NodeRef;

//! A Reference to Event's returned by the ramping methods. \see applyRamp() \see appendRamp()
typedef std::shared_ptr<class Event>			EventRef;
//! Ramping function that determines the curvature of a ramp.
typedef std::function<void ( float *, size_t, double, double, float, float )>	RampFn;

//! Array-based linear ramping function.
void CI_API rampLinear( float *array, size_t count, double t, double tIncr, float valueBegin, float valueEnd );
//! Array-based quadradic (t^2) ease-in ramping function.
void CI_API rampInQuad( float *array, size_t count, double t, double tIncr, float valueBegin, float valueEnd );
//! Array-based quadradic (t^2) ease-out ramping function.
void CI_API rampOutQuad( float *array, size_t count, double t, double tIncr, float valueBegin, float valueEnd );

//! Class representing a sample-accurate parameter control instruction. \see Param::applyRamp(), Param::appendRamp()
class CI_API Event {
  public:
	double getTimeBegin()		const	{ return mTimeBegin; }
	double getTimeEnd()			const	{ return mTimeEnd; }
	double getDuration()		const	{ return mDuration; }
	//! \note if getCopyValueOnBegin() is true then the begin value may not be known until the Event begins to be processed.
	float getValueBegin()		const	{ return mValueBegin; }
	float getValueEnd()			const	{ return mValueEnd; }
	const RampFn& getRampFn()	const	{ return mRampFn; }
	//! Returns whether the Param's current value will be copied when this Event begins or not.
	bool getCopyValueOnBegin()  const	{ return mCopyValueOnBegin; }
	//! Sets the value that will be used when this Event begins.
	void setValueBegin( float value )	{ mValueBegin = value; mCopyValueOnBegin = false; }

	void cancel()				{ mIsCanceled = true; }
	bool isComplete() const		{ return mIsComplete; }

	//! Returns the label that was assigned when the Event was created, or an empty string. Useful when debugging.
	const std::string&	getLabel() const	{ return mLabel; }

  private:
	Event( double timeBegin, double timeEnd, float valueBegin, float valueEnd, bool copyValueOnBegin, const RampFn &rampFn );

	double				mTimeBegin, mTimeEnd, mTimeCancel, mDuration;
	float				mValueBegin, mValueEnd;
	std::atomic<bool>	mIsComplete, mIsCanceled;
	bool				mCopyValueOnBegin;
	std::string			mLabel;
	RampFn				mRampFn;

	friend class Param;
};

//! \brief Allows an audio parameter to be controlled over time with sample accurate curves.

//! The Param class enables expressive control over variables that control audio::Node's with a Timeline-esque API.
//! It supports appending 'ramps' (audio animations) and maintains these in an internal timeline that is evaluated from the audio thread.
//! You can also set a Node as the 'processor' with Param::setProcessor(), enabling you to control it with an arbitrary signal.
//!
//! A Param is owned by a parent Node, from which it gains access to the current Context.  This is a necessary step in making it sample
//! accurate yet still controllable in a thread-safe manager on the user thread.
//!
//! \note Ramp Events should not overlap, or you may get discontinuities in the evaluated curve. This could potentially happen when
//! using multiple appendRamp() calls. Instead, use applyRamp() and set Options::beginTime() accordingly, which will remove any
//! Events that would otherwise be overlapping.
class CI_API Param {
  public:

	//! Optional parameters when applying or appending ramps. \see applyRamp() \see appendRamp()
	struct Options {
		Options() : mDelay( 0 ), mBeginTime( -1 ), mRampFn( rampLinear ) {}

		//! Specifies a delay of \a delay in seconds.
		Options& delay( double delay )				{ mDelay = delay; return *this; }
		//! Specifies the begin time in seconds. If this is value is greater or equal to zero, delay() is ignored.
		Options& beginTime( double time )			{ mBeginTime = time; return *this; }
		//! Specifies the ramping function used during evaluation.
		Options& rampFn( const RampFn &rampFn )		{ mRampFn = rampFn; return *this; }
		//! Sets a label that will be assigned to the Event. Useful when debugging.
		Options& label( const std::string &label )	{ mLabel = label; return *this; }

		//! Returns the delay specified in seconds.
		double				getDelay() const		{ return mDelay; }
		//! Returns the begin time specified in seconds.
		double				getBeginTime() const	{ return mBeginTime; }
		//! Returns the ramping function that will be used during evaluation.
		const RampFn&		getRampFn() const		{ return mRampFn; }
		//! Returns a label that will be assigned to the Event. Useful when debugging.
		const std::string&	getLabel() const		{ return mLabel; }

	  private:
		double	mDelay, mBeginTime;
		RampFn	mRampFn;
		std::string	mLabel;
	};

	//! Constructs a Param with a pointer (weak reference) to the owning parent Node and an optional \a initialValue (default = 0).
	Param( Node *parentNode, float initialValue = 0 );

	//! Sets the value of the Param, blowing away any scheduled Event's or processing Node. \note Must be called from a non-audio thread.
	void	setValue( float value );
	//! Returns the current value of the Param.
	float	getValue() const	{ return mValue; }
	//! Returns a pointer to the buffer used when evaluating a Param that is varying over the current processing block, of equal size to the owning Context's frames per block.
	//! \note If not varying (eval() returns false), the returned pointer will be invalid.

	//! Returns an array of values, time varying if `Event`s were processed, or filled with the a constant value if there were no `Event`s this block. \note Must call eval() once per block before using this method.
	const float*	getValueArray();

	//! Apply a ramp Event from the current value to \a valueEnd over \a rampSeconds, replacing any existing Events when this one begins. Any existing processing Node is disconnected. \see Options.
	EventRef applyRamp( float valueEnd, double rampSeconds, const Options &options = Options() );
	//! Apply a ramp Event from t\a valueBegin to \a valueEnd over \a rampSeconds, replacing any existing Events when this one begins. Any existing processing Node is disconnected. \see Options.
	EventRef applyRamp( float valueBegin, float valueEnd, double rampSeconds, const Options &options = Options() );
	//! Appends a ramp Event onto the end of the last scheduled Event (or the current time) to \a valueEnd over \a rampSeconds, according to \a options. Any existing processing Node is disconnected.
	EventRef appendRamp( float valueEnd, double rampSeconds, const Options &options = Options() );
	//! Appends an ramp Event onto the end of the last scheduled Event (or the current time), from \a valueBegin to \a valueEnd over \a rampSeconds, according to \a options. Any existing processing Node is disconnected.
	EventRef appendRamp( float valueBegin, float valueEnd, double rampSeconds, const Options &options = Options() );

	//! Sets this Param's input to be the processing performed by \a node. Any existing Event's are discarded. \note Forces \a node to be mono.
	void	setProcessor( const NodeRef &node );
	//! Returns this Param's processing Node, or an empty NodeRef if none is set.
	NodeRef	getProcessor() const	{ return mProcessor; }

	//! Resets Param, blowing away any Event's or processing Node. \note Must be called from a non-audio thread.
	void reset();
	//! Returns the number of Event's that are currently scheduled.
	size_t getNumEvents() const;

	//! Evaluates the Param for the current processing block, with current time determined from the parent Node's Context.
	//! \return true if the Param is varying this block (there are Event's or a processing Node) and getValueArray() should be used, or false if the Param's value is constant for this block (use getValue()).
	//! \note Safe to call on the audio thread.
	bool	eval();
	//! Evaluates the Param from \a timeBegin for \a arrayLength samples at \a sampleRate.
	//! \return true if the Param is varying this block (there are Event's or a processing Node) and getValueArray() should be used, or false if the Param's value is constant for this block (use getValue()).
	//! \note Safe to call on the audio thread.
	bool	eval( double timeBegin, float *array, size_t arrayLength, size_t sampleRate );

	//! Returns the total duration of any scheduled Event's, including delay, or 0 if none are scheduled.
	float					findDuration() const;
	//! Returns the end time and value of the latest scheduled Event, or [0, getValue()] if none are scheduled.
	std::pair<double, float> findEndTimeAndValue() const;

  protected:

	// non-locking protected methods
	void		initInternalBuffer();
	void		resetImpl();
	void		removeEventsAt( double time );
	ContextRef	getContext() const;

	std::list<EventRef>	mEvents;
	std::atomic<float>	mValue;
	bool				mIsVaryingThisBlock;
	Node*				mParentNode;
	NodeRef				mProcessor;
	BufferDynamic		mInternalBuffer;
};

} } // namespace cinder::audio
