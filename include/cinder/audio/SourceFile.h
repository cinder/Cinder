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
#include "cinder/audio/PcmBuffer.h"
#include "cinder/audio/CocoaCaConverter.h"

#include <AudioToolbox/AudioFile.h>


namespace cinder { namespace audio {

typedef shared_ptr<class SourceFile>	SourceFileRef;
typedef shared_ptr<class LoaderSourceFile>	LoaderSourceFileRef;

class LoaderSourceFile : public Loader {
 public:
	static LoaderSourceFileRef	createRef( SourceFile *source, Target *target );
	
	~LoaderSourceFile() {}
	
	uint64_t getSampleOffset() const;
	void setSampleOffset( uint64_t anOffset );
	void loadData( BufferList *ioData );
 private:
	static void dataInputCallback( Loader* aLoader, uint32_t *ioSampleCount, BufferList *ioData, AudioStreamPacketDescription * packetDescriptions );

	LoaderSourceFile( SourceFile *source, Target *target );
	
	SourceFile						* mSource;
	shared_ptr<CocoaCaConverter>	mConverter;
	uint64_t						mPacketOffset;
		
};

class SourceFile : public Source {
  public:
	static SourceRef		createRef( DataSourceRef dataSourceRef ) { return createFileRef( dataSourceRef ); }
	static SourceFileRef	createFileRef( DataSourceRef dataSourceRef );
	~SourceFile();

	virtual LoaderRef createLoader( Target *target ) { return LoaderSourceFile::createRef( this, target ); }

	double getDuration() const { return mDuration; };

	static void		registerSelf();
  protected:
	SourceFile( DataSourceRef dataSourceRef );
	
	uint64_t packetAtTime( double aTime ) const;
	double timeAtPacket( uint64_t aPacket ) const;
	
	uint64_t packetAtSample( uint64_t aSample ) const;
	uint64_t sampleAtPacket( uint64_t aPacket ) const;
	
	shared_ptr<OpaqueAudioFileID>	mFileRef;
	
	uint64_t	mPacketCount;
	uint64_t	mByteCount;
	double		mDuration;
	uint32_t	mMaxPacketSize;
	
	friend class LoaderSourceFile;
};

REGISTER_AUDIOIO( SourceFile )

}} // namespace

