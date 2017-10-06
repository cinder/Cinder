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

#include "cinder/audio/SamplePlayerNode.h"
#include "cinder/audio/Context.h"
#include "cinder/CinderMath.h"

using namespace ci;
using namespace std;

namespace cinder { namespace audio {

// ----------------------------------------------------------------------------------------------------
// SamplePlayerNode
// ----------------------------------------------------------------------------------------------------

SamplePlayerNode::SamplePlayerNode( const Format &format )
	: InputNode( format ), mNumFrames( 0 ), mReadPos( 0 ), mLoop( false ),
		mLoopBegin( 0 ), mLoopEnd( 0 )
{
	setChannelMode( ChannelMode::SPECIFIED );
}

void SamplePlayerNode::start()
{
	seek( 0 );
	enable();
}

void SamplePlayerNode::stop()
{
	disable();
	seek( 0 );
}

void SamplePlayerNode::start( double when )
{
	getContext()->scheduleEvent( when, shared_from_this(), true, [this] { start(); } );
}

void SamplePlayerNode::stop( double when )
{
	getContext()->scheduleEvent( when, shared_from_this(), false, [this] { stop(); } );
}

void SamplePlayerNode::setLoopBegin( size_t positionFrames )
{
	mLoopBegin = positionFrames < mLoopEnd ? positionFrames : mLoopEnd.load();
}

void SamplePlayerNode::setLoopEnd( size_t positionFrames )
{
	mLoopEnd = positionFrames < mNumFrames ? positionFrames : mNumFrames;

	if( mLoopBegin > mLoopEnd )
		mLoopBegin = mLoopEnd.load();
}

void SamplePlayerNode::seekToTime( double readPositionSeconds )
{
	seek( size_t( readPositionSeconds * (double)getSampleRate() ) );
}

void SamplePlayerNode::setLoopBeginTime( double positionSeconds )
{
	setLoopBegin( size_t( positionSeconds * (double)getSampleRate() ) );
}

void SamplePlayerNode::setLoopEndTime( double positionSeconds )
{
	setLoopEnd( size_t( positionSeconds * (double)getSampleRate() ) );
}

double SamplePlayerNode::getReadPositionTime() const
{
	return (double)mReadPos / (double)getSampleRate();
}

double SamplePlayerNode::getLoopBeginTime() const
{
	return (double)mLoopBegin / (double)getSampleRate();
}

double SamplePlayerNode::getLoopEndTime() const
{
	return (double)mLoopEnd / (double)getSampleRate();
}

double SamplePlayerNode::getNumSeconds() const
{
	return (double)mNumFrames / (double)getSampleRate();
}

// ----------------------------------------------------------------------------------------------------
// BufferPlayerNode
// ----------------------------------------------------------------------------------------------------

BufferPlayerNode::BufferPlayerNode( const Format &format )
	: SamplePlayerNode( format )
{
}

BufferPlayerNode::BufferPlayerNode( const BufferRef &buffer, const Format &format )
	: SamplePlayerNode( format ), mBuffer( buffer )
{
	size_t numFrames = mBuffer ? mBuffer->getNumFrames() : 0;
	mNumFrames = mLoopEnd = numFrames;

	// force channel mode to match buffer
	setNumChannels( mBuffer->getNumChannels() );
}

void BufferPlayerNode::enableProcessing()
{
	if( ! mBuffer ) {
		disable();
		return;
	}

	mIsEof = false;
}

void BufferPlayerNode::seek( size_t readPositionFrames )
{
	mIsEof = false;
	mReadPos = math<size_t>::clamp( readPositionFrames, 0, mNumFrames );
}

void BufferPlayerNode::setBuffer( const BufferRef &buffer )
{
	lock_guard<mutex> lock( getContext()->getMutex() );

	if( buffer ) {
		if( getNumChannels() != buffer->getNumChannels() ) {
			setNumChannels( buffer->getNumChannels() );
			configureConnections();
		}
		
		mNumFrames = buffer->getNumFrames();
	}
	else
		mNumFrames = 0;

	mBuffer = buffer;

	// reset loop markers
	mLoopBegin = 0;
	mLoopEnd = mNumFrames;
}

void BufferPlayerNode::loadBuffer( const SourceFileRef &sourceFile )
{
	size_t sampleRate = getSampleRate();
	if( sampleRate == sourceFile->getSampleRate() )
		setBuffer( sourceFile->loadBuffer() );
	else {
		auto sf = sourceFile->cloneWithSampleRate( sampleRate );
		setBuffer( sf->loadBuffer() );
	}
}

void BufferPlayerNode::process( Buffer *buffer )
{
	const auto &frameRange = getProcessFramesRange();

	size_t readPos = mReadPos;
	size_t numFrames = frameRange.second - frameRange.first;
	size_t readEnd = mLoop ? mLoopEnd.load() : mNumFrames;

	size_t readCount = 0;
	if( readPos <= readEnd ) {
		readCount = min( readEnd - readPos, numFrames );
		buffer->copyOffset( *mBuffer, readCount, frameRange.first, readPos );
	}

	if( readCount < numFrames  ) {
		// End of File. If looping copy from beginning, otherwise disable and mark mIsEof.
		if( mLoop ) {
			size_t readBegin = mLoopBegin;
			size_t readLeft = min( numFrames - readCount, mNumFrames - readBegin );

			buffer->copyOffset( *mBuffer, readLeft, readCount, readBegin );
			mReadPos.store( readBegin + readLeft );
		}
		else {
			mIsEof = true;
			mReadPos = mNumFrames;
			disable();
		}
	}
	else
		mReadPos += readCount;
}

// ----------------------------------------------------------------------------------------------------
// FilePlayerNode
// ----------------------------------------------------------------------------------------------------

FilePlayerNode::FilePlayerNode( const Format &format )
	: SamplePlayerNode( format ), mRingBufferPaddingFactor( 2 ), mLastUnderrun( 0 ), mLastOverrun( 0 ), mIsReadAsync( true )
{
}

FilePlayerNode::FilePlayerNode( const SourceFileRef &sourceFile, bool isReadAsync, const Format &format )
	: SamplePlayerNode( format ), mSourceFile( sourceFile ), mIsReadAsync( isReadAsync ), mRingBufferPaddingFactor( 2 ),
		mLastUnderrun( 0 ), mLastOverrun( 0 )
{
	if( mSourceFile ) {
		mNumFrames = mSourceFile->getNumFrames();

		// force channel mode to match buffer
		setNumChannels( mSourceFile->getNumChannels() );
	}
}

FilePlayerNode::~FilePlayerNode()
{
	if( isInitialized() )
		destroyReadThreadImpl();
}

void FilePlayerNode::initialize()
{
	if( mSourceFile ) {
		// Ensure the SourceFile's output samplerate matches ours.
		size_t sampleRate = getSampleRate();
		if( mSourceFile->getSampleRate() != sampleRate )
			mSourceFile = mSourceFile->cloneWithSampleRate( sampleRate );

		mNumFrames = mSourceFile->getNumFrames();

		mIoBuffer.setSize( mSourceFile->getMaxFramesPerRead(), getNumChannels() );

		for( size_t i = 0; i < getNumChannels(); i++ )
			mRingBuffers.emplace_back( mSourceFile->getMaxFramesPerRead() * mRingBufferPaddingFactor );

		mBufferFramesThreshold = mRingBuffers[0].getSize() / 2;
	}

	if( ! mLoopEnd  || mLoopEnd > mNumFrames )
		mLoopEnd = mNumFrames;

	if( mIsReadAsync ) {
		mAsyncReadShouldQuit = false;
		mReadThread = unique_ptr<thread>( new thread( bind( &FilePlayerNode::readAsyncImpl, this ) ) );
	}
}

void FilePlayerNode::uninitialize()
{
	destroyReadThreadImpl();
	mRingBuffers.clear();
}

void FilePlayerNode::enableProcessing()
{
	if( ! mSourceFile ) {
		disable();
		return;
	}

	mIsEof = false;
}

void FilePlayerNode::disableProcessing()
{
}

void FilePlayerNode::stop()
{
	if( mIsReadAsync ) {
		lock_guard<mutex> lock( mAsyncReadMutex );
		stopImpl();
	}
	else {
		auto ctx = getContext();
		if( ! ctx->isAudioThread() ) {
			lock_guard<mutex> lock( ctx->getMutex() );
			stopImpl();
		}
		else {
			// called from audio thread, lock is already held for the duration of this block
			stopImpl();
		}
	}
}

void FilePlayerNode::seek( size_t readPositionFrames )
{
	if( mIsReadAsync ) {
		lock_guard<mutex> lock( mAsyncReadMutex );
		seekImpl( readPositionFrames );
	}
	else {
		auto ctx = getContext();
		if( ! ctx->isAudioThread() ) {
			lock_guard<mutex> lock( ctx->getMutex() );
			seekImpl( readPositionFrames );
		}
		else {
			// called from audio thread, lock is already held for the duration of this block
			seekImpl( readPositionFrames );
		}
	}
}

void FilePlayerNode::setSourceFile( const SourceFileRef &sourceFile )
{
	lock_guard<mutex> lock( getContext()->getMutex() );

	bool wasEnabled = isEnabled();
	disable();

	// ensure the source's samplerate matches the context
	size_t sampleRate = getSampleRate();
	if( sourceFile->getSampleRate() == sampleRate )
		mSourceFile = sourceFile;
	else
		mSourceFile = sourceFile->cloneWithSampleRate( sampleRate );

	// reset num frames and loop markers
	mNumFrames = mSourceFile->getNumFrames();
	mLoopBegin = 0;
	mLoopEnd = mNumFrames;

	if( getNumChannels() != mSourceFile->getNumChannels() ) {
		setNumChannels( mSourceFile->getNumChannels() );
		configureConnections();
	}

	if( wasEnabled )
		enable();
}

uint64_t FilePlayerNode::getLastUnderrun()
{
	uint64_t result = mLastUnderrun;
	mLastUnderrun = 0;
	return result;
}

uint64_t FilePlayerNode::getLastOverrun()
{
	uint64_t result = mLastOverrun;
	mLastOverrun = 0;
	return result;
}

void FilePlayerNode::process( Buffer *buffer )
{
	size_t numFrames = buffer->getNumFrames();
	size_t readPos = mReadPos;
	size_t numReadAvail = mRingBuffers[0].getAvailableRead();

	if( numReadAvail < mBufferFramesThreshold ) {
		if( mIsReadAsync )
			mIssueAsyncReadCond.notify_one();
		else
			readImpl();
	}

	size_t readCount = std::min( numReadAvail, numFrames );

	for( size_t ch = 0; ch < buffer->getNumChannels(); ch++ ) {
		if( ! mRingBuffers[ch].read( buffer->getChannel( ch ), readCount ) )
			mLastUnderrun = getContext()->getNumProcessedFrames();
	}

	// handle loop or EOF
	if( readCount < numFrames ) {
		// TODO: if looping, should fill with samples from the beginning of file
		// - these should also already be in the ringbuffer, since a seek is done there as well. Rethink this path.
		readPos += readCount;
		if( mLoop && readPos >= mLoopEnd )
			seekImpl( mLoopBegin );
		else if( readPos >= mNumFrames ) {
			mIsEof = true;
			disable();
		}
	}
}

void FilePlayerNode::readAsyncImpl()
{
	size_t lastReadPos = mReadPos;
	while( true ) {
		unique_lock<mutex> lock( mAsyncReadMutex );
		mIssueAsyncReadCond.wait( lock );

		if( mAsyncReadShouldQuit )
			return;

		size_t readPos = mReadPos;
		if( readPos != lastReadPos )
			mSourceFile->seek( readPos );

		readImpl();
		lastReadPos = mReadPos;
	}
}

void FilePlayerNode::readImpl()
{
	size_t readPos = mReadPos;
	size_t availableWrite = mRingBuffers[0].getAvailableWrite();
	size_t readEnd = mLoop ? mLoopEnd.load() : mNumFrames;
	size_t numFramesToRead = readEnd < readPos ? 0 : min( availableWrite, readEnd - readPos );

	if( ! numFramesToRead ) {
		mLastOverrun = getContext()->getNumProcessedFrames();
		return;
	}

	// safety check that the SourceFile is on the correct read position, which could happen if two users are simultaneously reading from the same file.
	if( readPos != mSourceFile->getReadPosition() )
		mSourceFile->seek( readPos );

	mIoBuffer.setNumFrames( numFramesToRead );

	size_t numRead = mSourceFile->read( &mIoBuffer );
	mReadPos += numRead;

	for( size_t ch = 0; ch < getNumChannels(); ch++ ) {
		if( ! mRingBuffers[ch].write( mIoBuffer.getChannel( ch ), numRead ) ) {
			mLastOverrun = getContext()->getNumProcessedFrames();
			return;
		}
	}
}

void FilePlayerNode::seekImpl( size_t readPos )
{
	if( ! mSourceFile )
		return;

	mIsEof = false;
	mReadPos = math<size_t>::clamp( readPos, 0, mNumFrames );

	// if async mode, readAsyncImpl() will notice mReadPos was updated and do the seek there.
	if( ! mIsReadAsync )
		mSourceFile->seek( mReadPos );
}

void FilePlayerNode::stopImpl()
{
	disable();

	for( auto &ringBuffer : mRingBuffers )
		ringBuffer.clear();

	seekImpl( 0 );
}

void FilePlayerNode::destroyReadThreadImpl()
{
	if( mIsReadAsync && mReadThread ) {
		mAsyncReadShouldQuit = true;
		mIssueAsyncReadCond.notify_one();
		mReadThread->join();
	}
}

} } // namespace cinder::audio
