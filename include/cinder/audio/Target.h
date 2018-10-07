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
#include "cinder/audio/SampleType.h"

#include "cinder/DataTarget.h"

namespace cinder { namespace audio {

typedef std::shared_ptr<class TargetFile>		TargetFileRef;

//! Base class that is used to create and write to an audio destination. Currently only supports .wav encoding.
class CI_API TargetFile {
  public:
	static std::unique_ptr<TargetFile> create( const DataTargetRef &dataTarget, size_t sampleRate, size_t numChannels, SampleType sampleType = SampleType::INT_16, std::string extension = "" );
	static std::unique_ptr<TargetFile> create( const fs::path &path, size_t sampleRate, size_t numChannels, SampleType sampleType = SampleType::INT_16, std::string extension = "" );
	virtual ~TargetFile() {}

	void write( const Buffer *buffer );
	void write( const Buffer *buffer, size_t numFrames );
	void write( const Buffer *buffer, size_t numFrames, size_t frameOffset );

	size_t getSampleRate() const	{ return mSampleRate; }
	size_t getNumChannels() const	{ return mNumChannels; }

  protected:
	TargetFile( size_t sampleRate, size_t numChannels, SampleType sampleType )
		: mSampleRate( sampleRate ), mNumChannels( numChannels ), mSampleType( sampleType )
	{}

	// Implement to write \a numFrames frames of \a buffer to file. The writing begins at \a frameOffset.
	virtual void performWrite( const Buffer *buffer, size_t numFrames, size_t frameOffset ) = 0;

	size_t			mSampleRate, mNumChannels;
	SampleType		mSampleType;
};

} } // namespace cinder::audio
