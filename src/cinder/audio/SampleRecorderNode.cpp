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

#include "cinder/audio/SampleRecorderNode.h"
#include "cinder/audio/Context.h"
#include "cinder/audio/Target.h"

using namespace ci;
using namespace std;

namespace cinder { namespace audio {

namespace {

const size_t DEFAULT_RECORD_BUFFER_FRAMES = 44100;

void resizeBufferAndShuffleChannels( BufferDynamic *buffer, size_t resultNumFrames )
{
	const size_t currentNumFrames = buffer->getNumFrames();
	const size_t sampleSize = sizeof( BufferDynamic::SampleType );

	if( currentNumFrames < resultNumFrames ) {
		// if expanding, resize and then shuffle. Make sure to get the data pointer after the resize.
		buffer->setNumFrames( resultNumFrames );
		float *data = buffer->getData();

		for( size_t ch = 1; ch < buffer->getNumChannels(); ch++ ) {
			const size_t numZeroFrames = resultNumFrames - currentNumFrames;
			const float *currentChannel = &data[ch * currentNumFrames];
			float *resultChannel = &data[ch * resultNumFrames];

			memmove( resultChannel, currentChannel, currentNumFrames * sampleSize );
			memset( resultChannel - numZeroFrames, 0, numZeroFrames * sampleSize );
		}
	}
	else if( currentNumFrames > resultNumFrames ) {
		// if shrinking, shuffle first and then resize.
		float *data = buffer->getData();

		for( size_t ch = 1; ch < buffer->getNumChannels(); ch++ ) {
			const float *currentChannel = &data[ch * currentNumFrames];
			float *resultChannel = &data[ch * resultNumFrames];

			memmove( resultChannel, currentChannel, currentNumFrames * sampleSize );
		}

		const size_t numZeroFrames = ( currentNumFrames - resultNumFrames ) * buffer->getNumChannels();
		memset( data + buffer->getSize() - numZeroFrames, 0, numZeroFrames * sampleSize );

		buffer->setNumFrames( resultNumFrames );
	}
}
	
}

// ----------------------------------------------------------------------------------------------------
// SampleRecorderNode
// ----------------------------------------------------------------------------------------------------

SampleRecorderNode::SampleRecorderNode( const Format &format )
	: NodeAutoPullable( format ), mWritePos( 0 )
{
}

// ----------------------------------------------------------------------------------------------------
// BufferRecorderNode
// ----------------------------------------------------------------------------------------------------

BufferRecorderNode::BufferRecorderNode( const Format &format )
	: SampleRecorderNode( format ), mLastOverrun( 0 )
{
	initBuffers( DEFAULT_RECORD_BUFFER_FRAMES );
}

BufferRecorderNode::BufferRecorderNode( size_t numFrames, const Format &format )
	: SampleRecorderNode( format ), mLastOverrun( 0 )
{
	initBuffers( numFrames );
}

void BufferRecorderNode::initialize()
{
	// adjust recorder buffer to match channels once initialized, since they could have changed since construction.
	bool resize = mRecorderBuffer.getNumFrames() != 0;
	mRecorderBuffer.setNumChannels( getNumChannels() );

	// if the buffer had already been resized, zero out any posisbly existing data.
	if( resize )
		mRecorderBuffer.zero();
}

void BufferRecorderNode::initBuffers( size_t numFrames )
{
	mRecorderBuffer.setSize( numFrames, getNumChannels() );
	mCopiedBuffer = make_shared<BufferDynamic>( numFrames, getNumChannels() );
}

void BufferRecorderNode::start()
{
	mWritePos = 0;
	enable();
}

void BufferRecorderNode::stop()
{
	disable();
}

void BufferRecorderNode::setNumSeconds( double numSeconds, bool shrinkToFit )
{
	setNumFrames( size_t( numSeconds * (double)getSampleRate() ), shrinkToFit );
}

double BufferRecorderNode::getNumSeconds() const
{
	return (double)getNumFrames() / (double)getSampleRate();
}

void BufferRecorderNode::setNumFrames( size_t numFrames, bool shrinkToFit )
{
	if( mRecorderBuffer.getNumFrames() == numFrames )
		return;

	lock_guard<mutex> lock( getContext()->getMutex() );

	if( mWritePos != 0 )
		resizeBufferAndShuffleChannels( &mRecorderBuffer, numFrames );
	else
		mRecorderBuffer.setNumFrames( numFrames );

	if( shrinkToFit )
		mRecorderBuffer.shrinkToFit();
}

BufferRef BufferRecorderNode::getRecordedCopy() const
{
	// first grab the number of current frames, which may be increasing as the recording continues.
	size_t numFrames = mWritePos;
	mCopiedBuffer->setSize( numFrames, mRecorderBuffer.getNumChannels() );

	mCopiedBuffer->copy( mRecorderBuffer, numFrames );
	return mCopiedBuffer;
}

void BufferRecorderNode::writeToFile( const fs::path &filePath, SampleType sampleType )
{
	size_t currentWritePos = mWritePos;
	BufferRef copiedBuffer = getRecordedCopy();

	audio::TargetFileRef target = audio::TargetFile::create( filePath, getSampleRate(), getNumChannels(), sampleType );
	target->write( copiedBuffer.get(), currentWritePos );
}

uint64_t BufferRecorderNode::getLastOverrun()
{
	uint64_t result = mLastOverrun;
	mLastOverrun = 0;
	return result;
}

void BufferRecorderNode::process( Buffer *buffer )
{
	size_t writePos = mWritePos;
	size_t numWriteFrames = buffer->getNumFrames();

	if( writePos + numWriteFrames > mRecorderBuffer.getNumFrames() )
		numWriteFrames = mRecorderBuffer.getNumFrames() - writePos;

	mRecorderBuffer.copyOffset( *buffer, numWriteFrames, writePos, 0 );

	if( numWriteFrames < buffer->getNumFrames() )
		mLastOverrun = getContext()->getNumProcessedFrames();

	const size_t writePosNew = writePos + numWriteFrames;

	// only update mWritePos if it hasn't been reset by start() in the meanwhile
	mWritePos.compare_exchange_strong( writePos, writePosNew );
}

} } // namespace cinder::audio
