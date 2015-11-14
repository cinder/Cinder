/*
 Copyright (c) 2015, The Cinder Project

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

namespace audioloader {

class FileLoader;

} // namespace audioloader

namespace cinder { namespace audio { namespace linux {

class SourceFileAudioLoader : public SourceFile {
  public:
	SourceFileAudioLoader();
	SourceFileAudioLoader( const DataSourceRef &dataSource, size_t sampleRate );
	virtual ~SourceFileAudioLoader();

	SourceFileRef	cloneWithSampleRate( size_t sampleRate ) const	override;

	size_t			getNumChannels() const override;
	size_t			getSampleRateNative() const override;

	size_t			performRead( Buffer *buffer, size_t bufferFrameOffset, size_t numFramesNeeded ) override;
	void			performSeek( size_t readPositionFrames ) override;

	static std::vector<std::string>	getSupportedExtensions();

  private:
	std::unique_ptr<audioloader::FileLoader>	mFileLoader;

	DataSourceRef	mDataSource;  // stored so that clone() can tell if original data source is a file
	ci::IStreamRef	mStream;
	BufferDynamic	mAudioData;

	BufferDynamic	mReadBuffer;

	void 			init();
};

} } } // namespace cinder::audio::linux