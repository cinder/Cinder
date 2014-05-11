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

Ramp::Ramp( float timeBegin, float timeEnd, float valueBegin, float valueEnd, const RampFn &rampFn )
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

RampRef Param::applyRamp( float valueEnd, float rampSeconds, const Options &options )
{
	return applyRamp( mValue, valueEnd, rampSeconds, options );
}

RampRef Param::applyRamp( float valueBegin, float valueEnd, float rampSeconds, const Options &options )
{
	initInternalBuffer();

	auto ctx = getContext();
	float timeBegin = (float)ctx->getNumProcessedSeconds() + options.getDelay();
	float timeEnd = timeBegin + rampSeconds;

	RampRef ramp( new Ramp( timeBegin, timeEnd, valueBegin, valueEnd, options.getRampFn() ) );

	lock_guard<mutex> lock( ctx->getMutex() );
	resetImpl();
	mRamps.push_back( ramp );

	return ramp;
}

RampRef Param::appendRamp( float valueEnd, float rampSeconds, const Options &options )
{
	initInternalBuffer();

	auto ctx = getContext();
	auto endTimeAndValue = findEndTimeAndValue();

	float timeBegin = endTimeAndValue.first + options.getDelay();
	float timeEnd = timeBegin + rampSeconds;

	RampRef ramp( new Ramp( timeBegin, timeEnd, endTimeAndValue.second, valueEnd, options.getRampFn() ) );

	lock_guard<mutex> lock( ctx->getMutex() );
	mRamps.push_back( ramp );

	return ramp;
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

	CI_LOG_V( "set processing Node to: " << mProcessor->getName() );
}

void Param::reset()
{
	lock_guard<mutex> lock( getContext()->getMutex() );
	resetImpl();
}


size_t Param::getNumRamps() const
{
	lock_guard<mutex> lock( getContext()->getMutex() );
	return mRamps.size();
}

float Param::findDuration() const
{
	auto ctx = getContext();
	lock_guard<mutex> lock( ctx->getMutex() );

	if( mRamps.empty() )
		return 0;
	else {
		const RampRef &ramp = mRamps.back();
		return ramp->mTimeEnd - (float)ctx->getNumProcessedSeconds();
	}
}

pair<float, float> Param::findEndTimeAndValue() const
{
	auto ctx = getContext();
	lock_guard<mutex> lock( ctx->getMutex() );

	if( mRamps.empty() )
		return make_pair( (float)ctx->getNumProcessedSeconds(), mValue.load() );
	else {
		const RampRef &ramp = mRamps.back();
		return make_pair( ramp->mTimeEnd, ramp->mValueEnd );
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

	for( auto rampIt = mRamps.begin(); rampIt != mRamps.end(); /* */ ) {
		RampRef &ramp = *rampIt;

		// first remove dead ramps
		if( ramp->mTimeEnd < timeBegin || ramp->mIsCanceled ) {
			rampIt = mRamps.erase( rampIt );
			continue;
		}

		const float timeEnd = timeBegin + arrayLength * samplePeriod;

		if( ramp->mTimeBegin < timeEnd && ramp->mTimeEnd > timeBegin ) {
			size_t startIndex = timeBegin >= ramp->mTimeBegin ? 0 : size_t( ( ramp->mTimeBegin - timeBegin ) * sampleRate );
			size_t endIndex = timeEnd < ramp->mTimeEnd ? arrayLength : size_t( ( ramp->mTimeEnd - timeBegin ) * sampleRate );

			CI_ASSERT( startIndex <= arrayLength && endIndex <= arrayLength );

			if( startIndex > 0 && samplesWritten == 0 )
				dsp::fill( mValue, array, startIndex );

			size_t count = size_t( endIndex - startIndex );
			float timeBeginNormalized = float( timeBegin - ramp->mTimeBegin + startIndex * samplePeriod ) / ramp->mDuration;
			float timeEndNormalized = float( timeBegin - ramp->mTimeBegin + endIndex * samplePeriod ) / ramp->mDuration;
			float timeIncr = ( timeEndNormalized - timeBeginNormalized ) / (float)count;

			ramp->mRampFn( array + startIndex, count, timeBeginNormalized, timeIncr, make_pair( ramp->mValueBegin, ramp->mValueEnd ) );
			samplesWritten += count;

			// if this ramp ended with the current processing block, update mValue then remove ramp
			if( endIndex < arrayLength ) {
				ramp->mIsComplete = true;
				mValue = ramp->mValueEnd;
				rampIt = mRamps.erase( rampIt );
			}
			else if( samplesWritten == arrayLength ) {
				mValue = array[arrayLength - 1];
				break;
			}
			else
				++rampIt;
		}
		else
			++rampIt;
	}

	// if after all ramps we still haven't written enough samples, fill with the final mValue, which was updated above to be the last ramp's mValueEnd.
	if( samplesWritten < arrayLength )
		dsp::fill( mValue, array + (size_t)samplesWritten, size_t( arrayLength - samplesWritten ) );

	return samplesWritten != 0;
}

// ----------------------------------------------------------------------------------------------------
// MARK: - Protected
// ----------------------------------------------------------------------------------------------------

void Param::resetImpl()
{
	if( ! mRamps.empty() ) {
		for( auto &ramp : mRamps )
			ramp->cancel();

		mRamps.clear();
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