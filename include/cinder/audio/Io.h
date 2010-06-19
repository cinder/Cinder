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
#include "cinder/DataSource.h"
#include "cinder/audio/PcmBuffer.h"

#include <map>
#include <string>

#if defined(CINDER_COCOA)
	#include <AudioToolbox/AudioFile.h>
#elif defined(CINDER_MSW)
	#include <windows.h>
	#include <mmsystem.h> 
#endif

namespace cinder { namespace audio {

typedef shared_ptr<class Source>		SourceRef;
typedef shared_ptr<class Target>		TargetRef;
typedef shared_ptr<class Loader>		LoaderRef;

class Io {
 public:
	typedef enum DataType { UINT8, INT8, UINT16, INT16, UINT32, INT32, FLOAT32, DATA_UNKNOWN } DataType;
	
	//! the number of samples per second that each channel should played or recorded at.
	int32_t			getSampleRate() const { return mSampleRate; }
	//! the number of channels of audio data
	int16_t			getChannelCount() const { return mChannelCount; }
	//! the number of significant bits per channel in each sample
	int16_t			getBitsPerSample() const { return mBitsPerSample; }
	//! the number of bytes in each sample.  For interleaved data it's ussually ( sampleRate * channelCount ) / 8, for non-interleaved data ( sampleRate / 8 ). 
	int16_t			getBlockAlign() const { return mBlockAlign; }
	//! The format of the audio data
	DataType		getDataType() const { return mDataType; }
	//! Whether the audio data is interleaved or non-interleaved.
	bool			isInterleaved() const { return mIsInterleaved; }
	//! Whether or not the data is stored in Linear-PCM.
	bool			isPcm() const { return mIsPcm; }
	//! Whether or not the data is stored in Float format or Integer format.
	bool			isFloat() const { return ( mDataType == FLOAT32 ); }
	//! Whether or not the data is big-endian.
	bool			isBigEndian() const { return mIsBigEndian; }
	
 protected:
#if defined(CINDER_COCOA)
	static void loadFromCaAudioStreamBasicDescription( Io * anIo, const AudioStreamBasicDescription * audioDescription );
#endif
	Io() 
		: mSampleRate( 0 ), mChannelCount( 0 ), mBitsPerSample( 0 ), mBlockAlign( 0 ), mDataType( DATA_UNKNOWN ), 
			mIsInterleaved( true ), mIsPcm( false ), mIsBigEndian( false )
	{}
	
	int32_t			mSampleRate;
	int16_t			mChannelCount;
	int16_t			mBitsPerSample; //Significant Bits Per Sample
	int16_t			mBlockAlign; //BytesPerSample
	DataType		mDataType;
	bool			mIsInterleaved;
	bool			mIsPcm;
	bool			mIsBigEndian;
	
#if defined(CINDER_COCOA)
	//TODO: fix this
	uint32_t	mNativeFormatId;
	uint32_t	mNativeFormatFlags;
	uint32_t	mBytesPerPacket;
	uint32_t	mFramesPerPacket;
	uint32_t	mBytesPerFrame;
#endif
};

class Target : public Io {
 public:
	virtual ~Target() {}
 protected:
	Target() {}
};

class Source : public Io {
 public:
	virtual ~Source() {}
	virtual LoaderRef createLoader( Target *target ) { return LoaderRef(); }
	
	virtual double getDuration() const = 0;
 protected:
	
};

#if defined(CINDER_MSW)
	typedef HRESULT (*LoaderDataCallback)( void * audioData, uint32_t dataSize, void * track, uint64_t sampleTime, uint32_t sampleDuration );
#endif

class Loader {
 public:
	virtual ~Loader() {}
	virtual uint32_t getOptimalBufferSize() const { return 0; };

	virtual void loadData( BufferList *ioData ) = 0;
	virtual uint64_t getSampleOffset() const = 0;
	virtual void setSampleOffset( uint64_t anOffset ) = 0;
 protected:
#if defined(CINDER_COCOA)
	static void fillBufferListFromCaBufferList( BufferList * aBufferList, const AudioBufferList * caBufferList );
	static shared_ptr<AudioBufferList> createCaBufferList( const BufferList * caBufferList );
#endif
 
	Loader() {}
};

class IoException : public Exception {
};

class IoExceptionFailedLoad : public IoException {
};

class IoExceptionSourceNotFound : public IoExceptionFailedLoad {
};
	
class IoExceptionUnsupportedDataType : public IoException {
};

class IoExceptionUnsupportedDataFormat : public IoException {
};

struct IoRegistrar {
	typedef SourceRef (*SourceCreationFunc)( DataSourceRef );

	static SourceRef	createSource( DataSourceRef dataSource, std::string extension );
	
	static void		registerSourceType( std::string extension, SourceCreationFunc func, int32_t priority = 2 );
	static void		registerSourceGeneric( SourceCreationFunc func, int32_t priority = 2 );
	
  private:
	
	struct Inst {
		void	registerSourceType( std::string extension, SourceCreationFunc func, int32_t priority );
		void	registerSourceGeneric( SourceCreationFunc func, int32_t priority );
	
		SourceRef	createSource( DataSourceRef dataSource, std::string extension );
		
		std::map<std::string, std::multimap<int32_t,SourceCreationFunc> >	mSources;
		std::map<int32_t, SourceCreationFunc>								mGenericSources;
	};
	
	static IoRegistrar::Inst*	instance();
	
	friend class Io;
};

//! Loads an audio from the file path \a path. Optional \a extension parameter allows specification of a file type. For example, "mp3" would force the file to load as a MP3
SourceRef	load( const std::string &path, std::string extension = "" );
//! Loads an audio from \a dataSource. Optional \a extension parameter allows specification of a file type. For example, "mp3" would force the file to load as a MP3
SourceRef	load( DataSourceRef dataSource, std::string extension = "" );

template<typename T>
struct IoRegistrant {
	IoRegistrant() {
		(void) register_object;
	}
  private:
	struct exec_register {
		exec_register() {
			T::registerSelf();
		}
	};
	
	static exec_register register_object;
};

template<typename D> typename IoRegistrant<D>::exec_register IoRegistrant<D>::register_object;

#define REGISTER_AUDIOIO( TYPE ) \
struct IoRegisterT##TYPE : public IoRegistrant<TYPE> { \
	IoRegisterT##TYPE() : IoRegistrant<TYPE>() {} \
};

}} // namespace

