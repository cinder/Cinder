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

#include "cinder/audio/Source.h"
#include "cinder/audio/Target.h"
#include "cinder/audio/SampleType.h"
#include "cinder/audio/msw/MswUtil.h"
#include "cinder/msw/CinderMsw.h"

#include <vector>

struct IMFSourceReader;
struct IMFSinkWriter;
struct IMFByteStream;

namespace cinder { namespace audio { namespace msw {

class SourceFileMediaFoundation : public SourceFile {
  public:
	SourceFileMediaFoundation();
	SourceFileMediaFoundation( const DataSourceRef &dataSource, size_t sampleRate );
	virtual ~SourceFileMediaFoundation();

	SourceFileRef	cloneWithSampleRate( size_t sampleRate ) const	override;

	size_t		getNumChannels() const override			{ return mNumChannels; }
	size_t		getSampleRateNative() const override	{ return mSampleRate; }

	size_t		performRead( Buffer *buffer, size_t bufferFrameOffset, size_t numFramesNeeded ) override;
	void		performSeek( size_t readPositionFrames ) override;

	static std::vector<std::string>	getSupportedExtensions();

  private:
	void		initReader();
	size_t		processNextReadSample();

	ci::msw::ManagedComPtr<::IMFSourceReader>		mSourceReader;
	ci::msw::ManagedComPtr<ci::msw::ComIStream>		mComIStream;
	ci::msw::ManagedComPtr<::IMFByteStream>			mByteStream;
	DataSourceRef									mDataSource; // stored so that clone() can tell if original data source is a file or windows resource
	
	size_t				mSampleRate;
	size_t				mNumChannels;
	size_t				mBytesPerSample;
	SampleType			mSampleType;
	double				mSeconds;
	bool				mCanSeek;
	size_t				mReadBufferPos;
	size_t				mFramesRemainingInReadBuffer;

	BufferDynamic				mReadBuffer;			// used to marshal the number of samples read to the number requested, conversions
	BufferDynamicInterleaved	mBitConverterBuffer;	// only used when bit conversion and de-interleaving are done in separate steps (ex. 24-bit stereo).
};

//! \brief TargetFile implementation using Microsoft's Media Foundation Framework.
//!
//! Currently can encode WAV in 16-bit signed or 32-bit floating point samples (file extension is ignored).
class TargetFileMediaFoundation : public TargetFile {
  public:
	TargetFileMediaFoundation( const DataTargetRef &dataTarget, size_t sampleRate, size_t numChannels, SampleType sampleType, const std::string &extension );
	virtual ~TargetFileMediaFoundation();

	void performWrite( const Buffer *buffer, size_t numFrames, size_t frameOffset ) override;

  private:
	  std::unique_ptr<::IMFSinkWriter, ci::msw::ComDeleter>		mSinkWriter;

	  DWORD						mStreamIndex;
	  size_t					mSampleSize;
	  BufferDynamicInterleaved	mBitConverterBuffer;	// only used when bit conversion and interleaving are done in separate steps (ex. 24-bit stereo).
	  bool						mSamplesWritten = false;
};

class MediaFoundationInitializer {
  public:
	//! Called automatically whenever a SourceFileMediaFoundation is constructed.
	static void		initMediaFoundation();
	//! This function is not called automatically, but users may if they wish to free up memory used by Media Foundation.
	static void		shutdownMediaFoundation();

  private:
	static bool sIsMfInitialized;
};

} } } // namespace cinder::audio::msw
