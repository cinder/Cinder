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

#include "cinder/audio/Param.h"
#include "cinder/audio/Context.h"
#include "cinder/audio/dsp/Dsp.h"

#include "cinder/CinderMath.h"

using namespace std;

namespace cinder { namespace audio {

void rampLinear( float *array, size_t count, double t, double tIncr, float valueBegin, float valueEnd )
{
	for( size_t i = 0; i < count; i++ ) {
		auto factor = float( t );
		array[i] = lerp( valueBegin, valueEnd, factor );
		t += tIncr;
	}
}

void rampInQuad( float *array, size_t count, double t, double tIncr, float valueBegin, float valueEnd )
{
	for( size_t i = 0; i < count; i++ ) {
		auto factor = static_cast<float>( t * t );
		array[i] = lerp( valueBegin, valueEnd, factor );
		t += tIncr;
	}
}

void rampOutQuad( float *array, size_t count, double t, double tIncr, float valueBegin, float valueEnd )
{
	for( size_t i = 0; i < count; i++ ) {
		auto factor = float( -t * ( t - 2 ) );
		array[i] = lerp( valueBegin, valueEnd, factor );
		t += tIncr;
	}
}

Event::Event( double timeBegin, double timeEnd, float valueBegin, float valueEnd, bool copyValueOnBegin, const RampFn &rampFn )
	: mTimeBegin( timeBegin ), mTimeEnd( timeEnd ), mDuration( timeEnd - timeBegin ), mCopyValueOnBegin( copyValueOnBegin ),
		mValueBegin( valueBegin ), mValueEnd( valueEnd ), mRampFn( rampFn ), mIsComplete( false ), mIsCanceled( false ), mTimeCancel( -1 )
{
}

Param::Param( Node *parentNode, float initialValue )
	: mParentNode( parentNode ), mValue( initialValue ), mIsVaryingThisBlock( false )
{
}

void Param::setValue( float value )
{
	lock_guard<mutex> lock( getContext()->getMutex() );
	resetImpl();
	mValue = value;
}

EventRef Param::applyRamp( float valueEnd, double rampSeconds, const Options &options )
{
	initInternalBuffer();

	auto ctx = getContext();
	double timeBegin = ( options.getBeginTime() >= 0 ? options.getBeginTime() : ctx->getNumProcessedSeconds() + options.getDelay() );
	double timeEnd = timeBegin + rampSeconds;

	EventRef event( new Event( timeBegin, timeEnd, mValue, valueEnd, true, options.getRampFn() ) );

	if( ! options.getLabel().empty() )
		event->mLabel = options.getLabel();

	lock_guard<mutex> lock( ctx->getMutex() );

	removeEventsAt( timeBegin );
	if( mProcessor )
		mProcessor.reset();

	mEvents.push_back( event );
	return event;
}

EventRef Param::applyRamp( float valueBegin, float valueEnd, double rampSeconds, const Options &options )
{
	initInternalBuffer();

	auto ctx = getContext();
	double timeBegin = ( options.getBeginTime() >= 0 ? options.getBeginTime() : ctx->getNumProcessedSeconds() + options.getDelay() );
	double timeEnd = timeBegin + rampSeconds;

	EventRef event( new Event( timeBegin, timeEnd, valueBegin, valueEnd, false, options.getRampFn() ) );

	if( ! options.getLabel().empty() )
		event->mLabel = options.getLabel();

	lock_guard<mutex> lock( ctx->getMutex() );

	removeEventsAt( timeBegin );
	if( mProcessor )
		mProcessor.reset();

	mEvents.push_back( event );
	return event;
}

EventRef Param::appendRamp( float valueEnd, double rampSeconds, const Options &options )
{
	initInternalBuffer();

	auto ctx = getContext();
	auto endTimeAndValue = findEndTimeAndValue();
	double timeBegin = ( options.getBeginTime() >= 0 ? options.getBeginTime() : endTimeAndValue.first + options.getDelay() );
	double timeEnd = timeBegin + rampSeconds;

	EventRef event( new Event( timeBegin, timeEnd, endTimeAndValue.second, valueEnd, true, options.getRampFn() ) );

	if( ! options.getLabel().empty() )
		event->mLabel = options.getLabel();

	lock_guard<mutex> lock( ctx->getMutex() );
	mEvents.push_back( event );
	return event;
}

EventRef Param::appendRamp( float valueBegin, float valueEnd, double rampSeconds, const Options &options )
{
	initInternalBuffer();

	auto ctx = getContext();
	auto endTimeAndValue = findEndTimeAndValue();
	double timeBegin = ( options.getBeginTime() >= 0 ? options.getBeginTime() : endTimeAndValue.first + options.getDelay() );
	double timeEnd = timeBegin + rampSeconds;

	EventRef event( new Event( timeBegin, timeEnd, valueBegin, valueEnd, false, options.getRampFn() ) );

	if( ! options.getLabel().empty() )
		event->mLabel = options.getLabel();

	lock_guard<mutex> lock( ctx->getMutex() );
	mEvents.push_back( event );
	return event;
}

void Param::setProcessor( const NodeRef &node )
{
	if( ! node )
		return;

	initInternalBuffer();

	lock_guard<mutex> lock( getContext()->getMutex() );

	resetImpl();

	// force node to be mono and initialize it
	node->setNumChannels( 1 );
	node->initializeImpl();

	mProcessor = node;
	mIsVaryingThisBlock = true; // stays true until there is no more processor and eval() sets this to false.
}

void Param::reset()
{
	lock_guard<mutex> lock( getContext()->getMutex() );
	resetImpl();
}


size_t Param::getNumEvents() const
{
	lock_guard<mutex> lock( getContext()->getMutex() );
	return mEvents.size();
}

float Param::findDuration() const
{
	auto ctx = getContext();
	lock_guard<mutex> lock( ctx->getMutex() );

	if( mEvents.empty() )
		return 0;
	else {
		const EventRef &event = mEvents.back();
		return static_cast<float>(event->mTimeEnd - ctx->getNumProcessedSeconds());
	}
}

pair<double, float> Param::findEndTimeAndValue() const
{
	auto ctx = getContext();
	lock_guard<mutex> lock( ctx->getMutex() );

	if( mEvents.empty() )
		return make_pair( ctx->getNumProcessedSeconds(), mValue.load() );
	else {
		const EventRef &event = mEvents.back();
		return make_pair( event->mTimeEnd, event->mValueEnd );
	}
}

const float* Param::getValueArray()
{
	if( ! mIsVaryingThisBlock ) {
		initInternalBuffer();
		dsp::fill( mValue, mInternalBuffer.getData(), mInternalBuffer.getSize() );
	}

	return mInternalBuffer.getData();
}

bool Param::eval()
{
	if( mProcessor ) {
		mProcessor->pullInputs( &mInternalBuffer );
		mValue = mInternalBuffer[mInternalBuffer.getNumFrames() - 1];
		return true;
	}
	else {
		auto ctx = getContext();
		mIsVaryingThisBlock = eval( ctx->getNumProcessedSeconds(), mInternalBuffer.getData(), mInternalBuffer.getSize(), ctx->getSampleRate() );
		return mIsVaryingThisBlock;
	}
}

bool Param::eval( double timeBegin, float *array, size_t arrayLength, size_t sampleRate )
{
	const double samplePeriod = 1.0 / (double)sampleRate;
	const double secondsPerBlock = (double)arrayLength * samplePeriod;
	size_t samplesWritten = 0;

	for( auto eventIt = mEvents.begin(); eventIt != mEvents.end(); /* */ ) {
		Event &event = **eventIt;

		// first remove dead events
		const bool cancelled = event.mIsCanceled;
		if( event.mTimeEnd <= timeBegin || cancelled ) {
			// if we skipped over the last event, record its end value before erasing.
			if( mEvents.size() == 1 && ! cancelled )
				mValue = event.mValueEnd;

			eventIt = mEvents.erase( eventIt );
			continue;
		}

		const double timeEnd = timeBegin + secondsPerBlock;

		if( event.mTimeBegin < timeEnd && event.mTimeEnd > timeBegin ) {
			size_t startIndex = timeBegin >= event.mTimeBegin ? 0 : size_t( ( event.mTimeBegin - timeBegin ) * sampleRate );
			size_t endIndex = timeEnd < event.mTimeEnd ? arrayLength : size_t( ( event.mTimeEnd - timeBegin ) * sampleRate );

			CI_ASSERT( startIndex <= arrayLength && endIndex <= arrayLength );
			CI_ASSERT( event.mTimeEnd >= event.mTimeBegin );

			if( startIndex > 0 && samplesWritten == 0 )
				dsp::fill( mValue, array, startIndex );

			size_t count = size_t( endIndex - startIndex );
			double timeBeginNormalized = ( timeBegin - event.mTimeBegin + startIndex * samplePeriod ) / event.mDuration;
			double timeEndNormalized = ( timeBegin - event.mTimeBegin + endIndex * samplePeriod ) / event.mDuration;
			double timeIncr = ( timeEndNormalized - timeBeginNormalized ) / (double)count;

			// If the event has a cancel time, adjust the count if needed, but all other ramp values remain the same
			if( event.mTimeCancel > 0 ) {
				if( event.mTimeCancel < timeBegin ) {
					// event should already be over
					event.cancel();
					eventIt = mEvents.erase( eventIt );
					continue;
				}

				size_t endIndexModified = timeEnd < event.mTimeCancel ? arrayLength : size_t( ( event.mTimeCancel - timeBegin ) * sampleRate );
				if( endIndexModified != endIndex ) {
					count = endIndexModified - startIndex;
					event.cancel(); // cancel but still process. This Event will be removed from the container next block.
				}
			}

			if( event.getCopyValueOnBegin() )
				event.setValueBegin( mValue ); // this is only copied the first block the Event is processed, as next block getCopyValueOnBegin() is false.

			event.mRampFn( array + startIndex, count, timeBeginNormalized, timeIncr, event.mValueBegin, event.mValueEnd );
			samplesWritten += count;

			// if this ramp ended with the current processing block, update mValue then remove event
			if( endIndex < arrayLength ) {
				event.mIsComplete = true;
				mValue = event.mValueEnd;
				eventIt = mEvents.erase( eventIt );
			}
			else if( samplesWritten == arrayLength ) {
				// the array was filled, store the last calculated samples in mValue and finish evaluating
				mValue = array[arrayLength - 1];
				break;
			}
			else
				++eventIt;
		}
		else
			++eventIt;
	}

	if( ! samplesWritten )
		return false;
	else if( samplesWritten < arrayLength )
		dsp::fill( mValue, array + (size_t)samplesWritten, size_t( arrayLength - samplesWritten ) );

	return true;
}

// ----------------------------------------------------------------------------------------------------
// Protected
// ----------------------------------------------------------------------------------------------------

void Param::resetImpl()
{
	if( ! mEvents.empty() ) {
		for( auto &event : mEvents )
			event->cancel();

		mEvents.clear();
	}

	mProcessor.reset();
}

void Param::removeEventsAt( double time )
{
	auto context = mParentNode->getContext();
	bool contextDisabled = ! context || ! context->isEnabled();
	for( auto eventIt = mEvents.begin(); eventIt != mEvents.end(); /* */ ) {
		Event &event = **eventIt;
		if( event.getTimeBegin() >= time ) {
			if( contextDisabled ) {
				eventIt = mEvents.erase( eventIt );
			}
			else {
				event.cancel();
			}
		}
		else if( event.getTimeEnd() >= time ) {
			// Handle cancel later to allow the ramp to continue until the cancel point. Only reset cancel time if it is newer than a previous setting.
			if( event.mTimeCancel > 0 ) {
				event.mTimeCancel = min( event.mTimeCancel, time );
			}
			else
				event.mTimeCancel = time;
		}
	}
}

void Param::initInternalBuffer()
{
	if( mInternalBuffer.isEmpty() )
		mInternalBuffer.setNumFrames( getContext()->getFramesPerBlock() );
}

ContextRef Param::getContext() const
{
	return mParentNode->getContext();
}

} } // namespace cinder::audio
