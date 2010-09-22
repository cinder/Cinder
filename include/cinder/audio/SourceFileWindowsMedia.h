/*
 Copyright (c) 2009, The Barbarian Group
 All rights reserved.

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

#include "cinder/Cinder.h"
#include "cinder/audio/Io.h"
#include "cinder/Buffer.h"

#include <Wmsdk.h>
#include <windows.h>


namespace cinder { namespace audio {

typedef shared_ptr<class SourceFileWindowsMedia>	SourceFileWindowsMediaRef;
typedef shared_ptr<class LoaderSourceFileWindowsMedia>	LoaderSourceFileWindowsMediaRef;

class LoaderSourceFileWindowsMedia : public Loader {
  public:
	static LoaderSourceFileWindowsMediaRef	createRef( SourceFileWindowsMedia *source, Target *target );	
	~LoaderSourceFileWindowsMedia();
	
	uint32_t getOptimalBufferSize() const { return mMaxBufferSize; }
	void loadData( BufferList *ioData );

	uint64_t getSampleOffset() const;
	void setSampleOffset( uint64_t anOffset );
	
  protected:
	LoaderSourceFileWindowsMedia( SourceFileWindowsMedia * source, Target * target );

	uint64_t timeAtSample( uint64_t aSample ) const;
	uint64_t sampleAtTime( uint64_t aTime ) const;

	SourceFileWindowsMedia			* mSource;
	WAVEFORMATEX					mOutputFormat;

	uint32_t						mStreamSize;
	shared_ptr<IWMSyncReader>		mReader;
	shared_ptr<IWMHeaderInfo>		mHeaderInfo;

	uint32_t						mMaxBufferSize;
	uint64_t						mSampleOffset;
	uint32_t						mSrcBlockAlign;
	uint32_t						mSrcSampleRate;
	uint16_t						mSrcChannelCount;
	uint32_t						mSrcBitsPerSample;
};


class SourceFileWindowsMedia : public Source {
  public:
	static SourceRef					createRef( DataSourceRef dataSourceRef ) { return createFileRef( dataSourceRef ); }
	static SourceFileWindowsMediaRef	createFileRef( DataSourceRef dataSourceRef );
	~SourceFileWindowsMedia();

	virtual LoaderRef createLoader( Target *target ) { return LoaderSourceFileWindowsMedia::createRef( this, target ); }
  
	uint32_t getLength() const { return mBuffer.getDataSize(); };
	double getDuration() const { /*TODO*/ return 0.0;  }

	static void		registerSelf();
  protected:
	SourceFileWindowsMedia( DataSourceRef dataSourceRef );

	cinder::Buffer mBuffer;
	shared_ptr<void> mMemHandle;

	friend LoaderSourceFileWindowsMedia;
};

REGISTER_AUDIOIO( SourceFileWindowsMedia )

class SourceFileWindowsMediaExceptionUnsupportedData : public IoException {
};

}} //namespace