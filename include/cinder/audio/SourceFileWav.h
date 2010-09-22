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

namespace cinder { namespace audio {

typedef shared_ptr<class SourceFileWav>	SourceFileWavRef;
typedef shared_ptr<class LoaderSourceFileWav>	LoaderSourceFileWavRef;

class LoaderSourceFileWav : public Loader {
 public:
	static LoaderSourceFileWavRef	createRef( SourceFileWav *source, Target *target );	
	~LoaderSourceFileWav();

	uint32_t getOptimalBufferSize() const { return 0; }
	void loadData( BufferList *ioData );

	uint64_t getSampleOffset() const;
	void setSampleOffset( uint64_t anOffset );
protected:
	LoaderSourceFileWav( SourceFileWav * source, Target * target );

	SourceFileWav	* mSource;
	IStreamRef		mStream;
	uint64_t		mSampleOffset;
};

class SourceFileWav : public Source {
 public:
	static SourceRef					createRef( DataSourceRef dataSourceRef ) { return createFileWavRef( dataSourceRef ); }
	static SourceFileWavRef				createFileWavRef( DataSourceRef dataSourceRef );
	~SourceFileWav();

	LoaderRef createLoader( Target *target ) { return LoaderSourceFileWav::createRef( this, target ); }

	uint32_t getLength() const { return mDataLength; };
	double getDuration() const { /*TODO*/ return 0.0;  }

	static void		registerSelf();
 private:
	SourceFileWav( DataSourceRef dataSourceRef );
	void readFormatChunk( IStreamRef stream );
	IStreamRef createStream();
	
	DataSourceRef	mDataSource;
	uint32_t		mDataLength;
	uint32_t		mDataStart;
	uint64_t		mSampleCount;

	uint16_t		mAudioFormat;
	uint32_t		mByteRate;

	friend LoaderSourceFileWav;
};

REGISTER_AUDIOIO( SourceFileWav )

}} //namespace