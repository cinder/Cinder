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

#include "cinder/audio/InputNode.h"
#include "cinder/audio/Source.h"
#include "cinder/audio/dsp/RingBuffer.h"

#include <thread>
#include <mutex>
#include <condition_variable>

namespace cinder { namespace audio {

typedef std::shared_ptr<class SamplePlayerNode>				SamplePlayerNodeRef;
typedef std::shared_ptr<class BufferPlayerNode>				BufferPlayerNodeRef;
typedef std::shared_ptr<class FilePlayerNode>				FilePlayerNodeRef;

//! \brief Base Node class for sampled audio playback. Can do operations like seek and loop.
//!
//! SamplePlayerNode itself doesn't process any audio, but contains the common interface for InputNode's that do.
//! The ChannelMode is set to Node::ChannelMode::SPECIED and it always matches the sample's number of channels (or is equal to 1 if there is no source).
//! \see BufferPlayerNode, FilePlayerNode
class SamplePlayerNode : public InputNode {
  public:
	virtual ~SamplePlayerNode() {}

	//! Starts playing the sample from the beginning.
	virtual void start();
	//! Starts playing the sample at \a when seconds, measured against Context::getNumProcessedSeconds().
	void start( double when );
	//! Stops playing the sample, returns the read position to the beginning and disables processing.
	virtual void stop();
	//! Stops playing the sample at \a when seconds, measured against Context::getNumProcessedSeconds().
	void stop( double when );

	//! Seek the read position to \a readPositionFrames.
	virtual void seek( size_t positionFrames ) = 0;
	//! Seek to read position \a readPositionSeconds,
	void seekToTime( double positionSeconds );
	//! Returns the current read position in frames.
	size_t getReadPosition() const	{ return mReadPos; }
	//! Returns the current read position in seconds.
	double getReadPositionTime() const;
	//! Returns the total number of seconds this SamplePlayerNode will play from beginning to end.
	double getNumSeconds() const;
	//! Returns the total number of frames this SamplePlayerNode will play from beginning to end.
	size_t getNumFrames() const	{ return mNumFrames; }
	//! Returns whether the SamplePlayerNode has reached EOF (end of file). If true, isEnabled() will also return false.
	bool isEof() const				{ return mIsEof; }

	//! Sets whether playing continues from beginning after the end is reached (default = false)
	void setLoopEnabled( bool b = true )	{ mLoop = b; }
	//! Gets whether playing continues from beginning after the end is reached (default = false)
	bool isLoopEnabled() const			{ return mLoop; }
	//! Sets the begin loop marker in frames (default = 0, max = getNumFrames()).
	void setLoopBegin( size_t positionFrames );
	//! Sets the begin loop marker in seconds (default = 0, max = getNumSeconds()).
	void setLoopBeginTime( double positionSeconds );
	//! Sets the end loop marker in frames (default = getNumFrames(), max = getNumFrames()).
	void setLoopEnd( size_t positionFrames );
	//! Sets the end loop marker in seconds (default = getNumSeconds(), max = getNumSeconds()).
	void setLoopEndTime( double positionSeconds );
	//! Returns the begin loop marker in frames.
	size_t getLoopBegin() const	{ return mLoopBegin; }
	//! Returns the begin loop marker in seconds.
	double getLoopBeginTime() const;
	//! Returns the end loop marker in frames.
	size_t getLoopEnd() const	{ return mLoopEnd;	}
	//! Returns the end loop marker in seconds.
	double getLoopEndTime() const;

  protected:
	SamplePlayerNode( const Format &format = Format() );

	size_t				mNumFrames;
	std::atomic<size_t> mReadPos, mLoopBegin, mLoopEnd;
	std::atomic<bool>	mLoop, mIsEof;
};

//! Buffer-based SamplePlayerNode, where all samples are loaded into memory before playback.
class BufferPlayerNode : public SamplePlayerNode {
  public:
	//! Constructs a BufferPlayerNode without a buffer, with the assumption one will be set later. \note Format::channels() can still be used to allocate the expected channel count ahead of time.
	BufferPlayerNode( const Format &format = Format() );
	//! Constructs a BufferPlayerNode with \a buffer. \note Channel mode is always ChannelMode::SPECIFIED and num channels matches \a buffer. Format::channels() is ignored.
	BufferPlayerNode( const BufferRef &buffer, const Format &format = Format() );

	virtual ~BufferPlayerNode() {}

	void seek( size_t readPositionFrames ) override;

	//! Loads and stores a reference to a Buffer created from the entire contents of \a sourceFile. Resets the loop points to 0:getNumFrames()).
	void loadBuffer( const SourceFileRef &sourceFile );
	//! Sets the current Buffer. Safe to do while enabled. Resets the loop points to 0:getNumFrames()).
	void setBuffer( const BufferRef &buffer );
	//! returns a shared_ptr to the current Buffer.
	const BufferRef& getBuffer() const	{ return mBuffer; }

  protected:
	void enableProcessing()			override;
	void process( Buffer *buffer )	override;

	BufferRef mBuffer;
};

//! File-based SamplePlayerNode, where samples are constantly streamed from file. Suitable for large audio files.
class FilePlayerNode : public SamplePlayerNode {
  public:
	//! Constructs a FilePlayerNode with optional \a format.
	FilePlayerNode( const Format &format = Format() );
	//! Constructs a FilePlayerNode that plays \a sourceFile and optionally specifying \a isReadAsync (default = true). Can also provide an optional \a format. \note \a sourceFile's samplerate is forced to match this Node's Context.
	FilePlayerNode( const SourceFileRef &sourceFile, bool isReadAsync = true, const Format &format = Node::Format() );
	virtual ~FilePlayerNode();

	void stop() override;
	void seek( size_t readPositionFrames ) override;

	//! Returns whether reading occurs asynchronously (default is false). If true, file reading is done from an internal thread, if false it is done directly on the audio thread.
	bool isReadAsync() const	{ return mIsReadAsync; }

	//! \note \a sourceFile's samplerate is forced to match this Node's Context. Resets the loop points to 0:getNumFrames()).
	void setSourceFile( const SourceFileRef &sourceFile );
	const SourceFileRef& getSourceFile() const	{ return mSourceFile; }

	//! Returns the frame of the last buffer underrun or 0 if none since the last time this method was called.
	uint64_t getLastUnderrun();
	//! Returns the frame of the last buffer overrun or 0 if none since the last time this method was called.
	uint64_t getLastOverrun();

  protected:
	void initialize()				override;
	void uninitialize()				override;
	void enableProcessing()			override;
	void disableProcessing()		override;
	void process( Buffer *buffer )	override;

	void readAsyncImpl();
	void readImpl();
	void seekImpl( size_t readPos );
	void stopImpl();
	void destroyReadThreadImpl();

	std::vector<dsp::RingBuffer>				mRingBuffers;	// used to transfer samples from io to audio thread, one ring buffer per channel
	BufferDynamic								mIoBuffer;		// used to read samples from the file on read thread, resizeable so the ringbuffer can be filled

	SourceFileRef								mSourceFile;
	size_t										mBufferFramesThreshold, mRingBufferPaddingFactor;
	std::atomic<uint64_t>						mLastUnderrun, mLastOverrun;

	std::unique_ptr<std::thread>				mReadThread;
	std::mutex									mAsyncReadMutex;
	std::condition_variable						mIssueAsyncReadCond;
	bool										mIsReadAsync, mAsyncReadShouldQuit;
};

} } // namespace cinder::audio
