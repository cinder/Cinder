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

#include "cinder/audio/Node.h"
#include "cinder/audio/SampleType.h"
#include "cinder/Filesystem.h"

namespace cinder { namespace audio {

typedef std::shared_ptr<class SampleRecorderNode> SampleRecorderNodeRef;
typedef std::shared_ptr<class BufferRecorderNode> BufferRecorderNodeRef;

//! Base Node class for recording audio samples. Inherits from NodeAudioPullable, and therefore does not need to be connected to an output.
class CI_API SampleRecorderNode : public NodeAutoPullable {
  public:
	//! Returns the current write position, which represents how many samples are currently recorded.
	size_t getWritePosition() const		{ return mWritePos; }
  protected:
	SampleRecorderNode( const Format &format = Format() );

	std::atomic<size_t>		mWritePos;
};

//! Records its inputs to a Buffer.  The Buffer record size should be specified by the user (the default size is 44100 frames). Also supports writing the recorded samples to file.
class CI_API BufferRecorderNode : public SampleRecorderNode {
  public:
	BufferRecorderNode( const Format &format = Format() );
	//! Constructs a BufferRecordNode and initializes its buffer to be large enough to hold \a numFrames frames.
	BufferRecorderNode( size_t numFrames, const Format &format = Format() );

	//! Starts recording. Resets the write position to zero (call disable() to pause recording).
	void start();
	//! Stops recording. Same as calling disable().
	void stop();

	//! \brief Sets the length of the recording buffer in frames.
	//!
	//! If the write position is non-zero, the old contents will be preserved (by copying it to the newly allocated Buffer).
	//! If \a shrinkToFit is set to `true`, the internal Buffer will be down-sized if necessary, otherwise it will only re-allocate when growing while changing its dimensions to match \a numFrames (default shrinkToFit = false).
	void setNumFrames( size_t numFrames, bool shrinkToFit = false );
	//! Sets the length of the recording buffer in seconds. \see setNumFrames
	void setNumSeconds( double numSeconds, bool shrinkToFit = false );

	//! Returns the length of the recording buffer in frames.
	size_t		getNumFrames() const	{ return mRecorderBuffer.getNumFrames(); }
	//! Returns the length of the recording buffer in seconds.
	double		getNumSeconds() const;

	//! \brief Returns a copy of the recored samples, up to the current write position.
	//!
	//! This method is non locking, and as such any resizing calls must be performed on the same thread or be otherwise synchronized.
	BufferRef	getRecordedCopy() const;

	//! \brief Writes the currently recorded samples to a file at \a filePath
	//!
	//! The encoding format is derived from \a filePath's extension and \a sampleType (default = SampleType::INT_16).
	//! \note throws AudioFileExc if the write request cannot be completed.
	void writeToFile( const cinder::fs::path &filePath, SampleType sampleType = SampleType::INT_16 );

	//! Returns the frame of the last buffer overrun or 0 if none since the last time this method was called. When this happens, it means the recorded buffer probably has skipped some frames.
	uint64_t getLastOverrun();

  protected:
	void initialize()				override;
	void process( Buffer *buffer )	override;

	void initBuffers( size_t numFrames );

	BufferDynamic			mRecorderBuffer;
	BufferDynamicRef		mCopiedBuffer;
	std::atomic<uint64_t>	mLastOverrun;
};

} } // namespace cinder::audio
