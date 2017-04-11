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

#include "cinder/audio/Buffer.h"
#include "cinder/DataSource.h"
#include "cinder/Noncopyable.h"

namespace cinder { namespace audio {
	
typedef std::shared_ptr<class Source>			SourceRef;
typedef std::shared_ptr<class SourceFile>		SourceFileRef;

namespace dsp {
	class Converter;
}

//! Base class that is used to load and read from an audio source.
class CI_API Source : private Noncopyable {
  public:
	virtual ~Source();

	//! Returns the user facing samplerate (output).
	size_t	getSampleRate() const				{ return mSampleRate; }

	//! Returns the number of channels.
	virtual size_t	getNumChannels() const = 0;
	//! Returns the true samplerate of the Source. \note Actual output samplerate may differ. \see getSampleRate()
	virtual size_t	getSampleRateNative() const = 0;

	//! Returns the maximum number of frames that can be read with one call to read().
	size_t			getMaxFramesPerRead() const			{ return mMaxFramesPerRead; }
	//! Sets the maximum number of frames that can be read in one chunk.
	virtual void	setMaxFramesPerRead( size_t count )		{ mMaxFramesPerRead = count; }

	//! Loads either as many frames as \t buffer can hold, or as many as there are left. \return number of frames read into \a buffer.
	virtual size_t read( Buffer *buffer ) = 0;

	//! Returns the metadata, if any, as a string.
	virtual std::string getMetaData() const	{ return std::string(); }

  protected:
	Source( size_t sampleRate );

	//! Implement to perform read of \numFramesNeeded frames into \a buffer starting at offset \a bufferFrameOffset
	//! \return the actual number of frames read.
	virtual size_t performRead( Buffer *buffer, size_t bufferFrameOffset, size_t numFramesNeeded ) = 0;
	//! Implementations should override and return true if they can provide samplerate conversion.  If false (default), a Converter will be used if needed.
	virtual bool supportsConversion()	{ return false; }
	//! Allows implementations to set the output samplerate
	void setSampleRate( size_t sampleRate )	{ mSampleRate = sampleRate; }

	std::unique_ptr<dsp::Converter>		mConverter;
	BufferDynamic						mConverterReadBuffer;

  private:
	size_t								mSampleRate, mMaxFramesPerRead;
};

//! Loads and reads from an audio file source.
class CI_API SourceFile : public Source {
  public:
	//! Creates a new SourceFile from \a dataSource, with optional output samplerate.  If \a sampleRate equals 0 the native file's samplerate is used.
	static std::unique_ptr<SourceFile> create( const DataSourceRef &dataSource, size_t sampleRate = 0 );
	virtual ~SourceFile()	{}

	size_t	read( Buffer *buffer ) override;

	//! Returns a copy of this Source, with identical properties and pointing at the same data source.
	SourceFileRef clone() const		{ return cloneWithSampleRate( getSampleRate() ); }
	//! Returns an copy of this Source with all properties identical except the sampleRate. This is useful if the SourceFile must match a samplerate that was unknown when it was first constructed.
	virtual SourceFileRef cloneWithSampleRate( size_t sampleRate ) const = 0;

	//! Loads and returns the entire contents of this SourceFile. \return a BufferRef containing the file contents.
	BufferRef loadBuffer();
	//! Seek the read position to \a readPositionFrames
	void	seek( size_t readPositionFrames );
	//! Seek to read position \a readPositionSeconds
	void	seekToTime( double readPositionSeconds )	{ return seek( size_t( readPositionSeconds * (double)getSampleRate() ) ); }
	//! Returns the current read position in frames.
	size_t	getReadPosition() const						{ return mReadPos; }
	//! Returns the current read position in seconds.
	double	getReadPositionSeconds() const				{ return mReadPos / (double)getSampleRate(); }
	//! Returns the length in frames.
	size_t	getNumFrames() const						{ return mNumFrames; }
	//! Returns the length in seconds.
	double	getNumSeconds() const						{ return (double)getNumFrames() / (double)getSampleRate(); }

	//! Returns a vector of extensions that SourceFile support for loading. Suitable for the \a extensions parameter of getOpenFilePath().
	static std::vector<std::string>	getSupportedExtensions();

  protected:
	SourceFile( size_t sampleRate );

	//! Implement to perform seek. \a readPositionFrames is in native file units.
	virtual void performSeek( size_t readPositionFrames ) = 0;
	//! Sets up samplerate conversion if needed. Can be overridden by implementation if they handle samplerate conversion in a specific way, else it is handled generically with a dsp::Converter.
	virtual void setupSampleRateConversion();

	size_t mNumFrames, mFileNumFrames, mReadPos;
};

//! Convenience method for loading a SourceFile from \a dataSource. \return SourceFileRef. \see SourceFile::create()
inline SourceFileRef	load( const DataSourceRef &dataSource, size_t sampleRate = 0 )	{ return SourceFile::create( dataSource, sampleRate ); }

} } // namespace cinder::audio
