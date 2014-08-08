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
#include "cinder/audio/Debug.h"

#include "cinder/CinderMath.h"

using namespace std;

namespace cinder { namespace audio {

void rampLinear( float *array, size_t count, float t, float tIncr, const std::pair<float, float> &valueRange )
{
	for( size_t i = 0; i < count; i++ ) {
		float factor = t;
		array[i] = lerp( valueRange.first, valueRange.second, factor );
		t += tIncr;
	}
}

void rampInQuad( float *array, size_t count, float t, float tIncr, const std::pair<float, float> &valueRange )
{
	for( size_t i = 0; i < count; i++ ) {
		float factor = t * t;
		array[i] = lerp( valueRange.first, valueRange.second, factor );
		t += tIncr;
	}
}

void rampOutQuad( float *array, size_t count, float t, float tIncr, const std::pair<float, float> &valueRange )
{
	for( size_t i = 0; i < count; i++ ) {
		float factor = -t * ( t - 2 );
		array[i] = lerp( valueRange.first, valueRange.second, factor );
		t += tIncr;
	}
}

Event::Event( float timeBegin, float timeEnd, float valueBegin, float valueEnd, const RampFn &rampFn )
	: mTimeBegin( timeBegin ), mTimeEnd( timeEnd ), mDuration( timeEnd - timeBegin ),
		mValueBegin( valueBegin ), mValueEnd( valueEnd ), mRampFn( rampFn ), mIsComplete( false ), mIsCanceled( false )
{
}

Param::Param( Node *parentNode, float initialValue )
	: mParentNode( parentNode ), mValue( initialValue )
{
}

void Param::setValue( float value )
{
	lock_guard<mutex> lock( getContext()->getMutex() );

	resetImpl();
	mValue = value;
}

EventRef Param::applyRamp( float valueEnd, float rampSeconds, const Options &options )
{
	return applyRamp( mValue, valueEnd, rampSeconds, options );
}

EventRef Param::applyRamp( float valueBegin, float valueEnd, float rampSeconds, const Options &options )
{
	initInternalBuffer();

	auto ctx = getContext();
	float timeBegin = (float)ctx->getNumProcessedSeconds() + options.getDelay();
	float timeEnd = timeBegin + rampSeconds;

	EventRef event( new Event( timeBegin, timeEnd, valueBegin, valueEnd, options.getRampFn() ) );

	lock_guard<mutex> lock( ctx->getMutex() );
	resetImpl();
	mEvents.push_back( event );

	return event;
}

EventRef Param::appendRamp( float valueEnd, float rampSeconds, const Options &options )
{
	initInternalBuffer();

	auto ctx = getContext();
	auto endTimeAndValue = findEndTimeAndValue();

	float timeBegin = endTimeAndValue.first + options.getDelay();
	float timeEnd = timeBegin + rampSeconds;

	EventRef event( new Event( timeBegin, timeEnd, endTimeAndValue.second, valueEnd, options.getRampFn() ) );

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
		return event->mTimeEnd - (float)ctx->getNumProcessedSeconds();
	}
}

pair<float, float> Param::findEndTimeAndValue() const
{
	auto ctx = getContext();
	lock_guard<mutex> lock( ctx->getMutex() );

	if( mEvents.empty() )
		return make_pair( (float)ctx->getNumProcessedSeconds(), mValue.load() );
	else {
		const EventRef &event = mEvents.back();
		return make_pair( event->mTimeEnd, event->mValueEnd );
	}
}

const float* Param::getValueArray() const
{
	CI_ASSERT( ! mInternalBuffer.isEmpty() );

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
		return eval( (float)ctx->getNumProcessedSeconds(), mInternalBuffer.getData(), mInternalBuffer.getSize(), ctx->getSampleRate() );
	}
}

bool Param::eval( float timeBegin, float *array, size_t arrayLength, size_t sampleRate )
{
	size_t samplesWritten = 0;
	const float samplePeriod = 1.0f / (float)sampleRate;

	for( auto eventIt = mEvents.begin(); eventIt != mEvents.end(); /* */ ) {
		EventRef &event = *eventIt;

		// first remove dead events
		if( event->mTimeEnd <= timeBegin || event->mIsCanceled ) {
			// if this is the last event, record its end value before erasing.
			if( mEvents.size() == 1 )
				mValue = event->mValueEnd;
			
			eventIt = mEvents.erase( eventIt );
			continue;
		}

		const float timeEnd = timeBegin + arrayLength * samplePeriod;

		if( event->mTimeBegin < timeEnd && event->mTimeEnd > timeBegin ) {
			size_t startIndex = timeBegin >= event->mTimeBegin ? 0 : size_t( ( event->mTimeBegin - timeBegin ) * sampleRate );
			size_t endIndex = timeEnd < event->mTimeEnd ? arrayLength : size_t( ( event->mTimeEnd - timeBegin ) * sampleRate );

			CI_ASSERT( startIndex <= arrayLength && endIndex <= arrayLength );

			if( startIndex > 0 && samplesWritten == 0 )
				dsp::fill( mValue, array, startIndex );

			size_t count = size_t( endIndex - startIndex );
			float timeBeginNormalized = float( timeBegin - event->mTimeBegin + startIndex * samplePeriod ) / event->mDuration;
			float timeEndNormalized = float( timeBegin - event->mTimeBegin + endIndex * samplePeriod ) / event->mDuration;
			float timeIncr = ( timeEndNormalized - timeBeginNormalized ) / (float)count;

			event->mRampFn( array + startIndex, count, timeBeginNormalized, timeIncr, make_pair( event->mValueBegin, event->mValueEnd ) );
			samplesWritten += count;

			// if this ramp ended with the current processing block, update mValue then remove event
			if( endIndex < arrayLength ) {
				event->mIsComplete = true;
				mValue = event->mValueEnd;
				eventIt = mEvents.erase( eventIt );
			}
			else if( samplesWritten == arrayLength ) {
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
// MARK: - Protected
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

void Param::initInternalBuffer()
{
	if( mInternalBuffer.isEmpty() )
		mInternalBuffer.setNumFrames( getContext()->getFramesPerBlock() );
}

ContextRef Param::getContext() const
{
	return	mParentNode->getContext();
}

} } // namespace cinder::audio
