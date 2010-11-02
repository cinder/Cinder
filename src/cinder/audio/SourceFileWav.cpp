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
#include "cinder/audio/SourceFileWav.h"

namespace cinder { namespace audio {

#if defined(CINDER_LITTLE_ENDIAN)
	const uint32_t gRiffMarker = 'FFIR';
	const uint32_t gRifxMarker = 'XFIR';
	const uint32_t gWaveMarker = 'EVAW';
	const uint32_t gDataMarker = 'atad';
	const uint32_t gFmtMarker = ' tmf';
#else
	const uint32_t gRiffMarker = 'RIFF';
	const uint32_t gRifxMarker = 'RIFX';
	const uint32_t gWaveMarker = 'WAVE';
	const uint32_t gDataMarker = 'data';
	const uint32_t gFmtMarker = 'fmt ';
#endif

enum {
	WAV_FORMAT_UNKOWN = 0x000,
	WAV_FORMAT_PCM = 0x0001,		// PCM Format
	WAV_FORMAT_MS_ADPCM = 0x0002,	// Microsoft ADPCM
	WAV_FORMAT_IEEE_FLOAT = 0x0003,// Microsoft 32 bit float format
	WAV_FORMAT_ALAW = 0x0006,
	WAV_FORMAT_MULAW = 0x0007,
	WAV_FORMAT_DIALOGIC_OKI_ADPCM = 0x0011, // IMA ADPCM
	WAV_FORMAT_YAMAHA_ADPCM = 0x0016, // Yamaha ADPCM
	WAV_FORMAT_GSM610 = 0x0031,
	WAV_FORMAT_G721_ADPCM = 0x0040, 
	WAV_FORMAT_MPEG = 0x0050,
	WAV_FORMAT_MP3 = 0x0055
};	

template<typename T>
void readStreamWithEndianess( IStreamRef aIStream, T* param, bool isBigEndian )
{
	if( isBigEndian ) {
		aIStream->readBig( param );
		return;
	}
	aIStream->readLittle( param );
}

LoaderSourceFileWavRef LoaderSourceFileWav::createRef( SourceFileWav *source, Target *target ) {
	return LoaderSourceFileWavRef( new LoaderSourceFileWav( source, target ) );
}

LoaderSourceFileWav::LoaderSourceFileWav( SourceFileWav * source, Target * target ) 
	: Loader(), mSource( source ), mSampleOffset( 0 )
{
	mStream = mSource->mDataSource->createStream();
	mStream->seekAbsolute( mSource->mDataStart );
}

LoaderSourceFileWav::~LoaderSourceFileWav()
{
}

uint64_t LoaderSourceFileWav::getSampleOffset() const
{
	return mSampleOffset;
}

//TODO: protect mSampleOffset with mutex
void LoaderSourceFileWav::setSampleOffset( uint64_t anOffset )
{
	mSampleOffset = anOffset;
	mStream->seekAbsolute( mSource->mDataStart + ( anOffset * mSource->mBlockAlign ) );
}

void LoaderSourceFileWav::loadData( BufferList *ioData )
{	
	if( mSampleOffset + ioData->mBuffers[0].mSampleCount > mSource->mSampleCount ) {
		ioData->mBuffers[0].mSampleCount = mSource->mSampleCount - mSampleOffset;
	}
	
	uint32_t dataSize = ioData->mBuffers[0].mSampleCount * mSource->mBlockAlign;

#if defined(CINDER_LITTLE_ENDIAN)
	bool nativeLittleEndian = true;
#elif
	bool nativeLittleEndian = false;
#endif

	//if native endianess matches source endianess just read it
	if( (nativeLittleEndian && ! mSource->mIsBigEndian ) || ( ! nativeLittleEndian && mSource->mIsBigEndian ) ) {
		mStream->readData( ioData->mBuffers[0].mData, dataSize );
	} else {
		//TODO: readWithEndianess with casted buffers
	}
	mSampleOffset += ioData->mBuffers[0].mSampleCount;
	ioData->mBuffers[0].mDataByteSize = dataSize;
}

void SourceFileWav::registerSelf()
{
	static const int32_t SOURCE_PRIORITY = 3;
	
	IoRegistrar::SourceCreationFunc sourceFunc = SourceFileWav::createRef;
	
	IoRegistrar::registerSourceType( "wav", sourceFunc, 1 );
	IoRegistrar::registerSourceGeneric( sourceFunc, SOURCE_PRIORITY );
}

SourceFileWavRef	SourceFileWav::createFileWavRef( DataSourceRef dataSourceRef )
{
	return SourceFileWavRef( new SourceFileWav( dataSourceRef ) );
}

SourceFileWav::SourceFileWav( DataSourceRef dataSourceRef )
	: Source(), mDataSource( dataSourceRef )
{
	mDataType = DATA_UNKNOWN;
	mSampleRate = 0;
	mChannelCount = 0;
	mBitsPerSample = 0;
	mBlockAlign = 0;
	mIsPcm = FALSE;
	mIsBigEndian = FALSE;
	mIsInterleaved = FALSE;
	
	IStreamRef stream = mDataSource->createStream();

	uint32_t fileSize = 0;
	
	uint32_t chunkName = 0;
	uint32_t chunkSize = 0;
	
	uint32_t riffType = 0;
	
	stream->readData( &chunkName, 4 );
	if( chunkName == gRiffMarker ) {
		mIsBigEndian = false;
	} else if( chunkName == gRifxMarker ) {
		mIsBigEndian = true;
	} else {
		throw IoExceptionFailedLoad();
	}
	
	readStreamWithEndianess( stream, &fileSize, mIsBigEndian );
	fileSize = fileSize + 4 + sizeof( int );
	
	stream->readData( &riffType, 4 );
	if( riffType != gWaveMarker ) {
		throw IoExceptionFailedLoad();
	}
	
	
	uint32_t chunkEnd = 0;
	uint32_t chunks = 0;
	static const uint8_t hasFormat = 1;
	static const uint8_t hasData = 1 << 1;
	
	while( stream->tell() < fileSize) {
		stream->readData( &chunkName, 4 );
		readStreamWithEndianess( stream, &chunkSize, mIsBigEndian );
		chunkEnd = stream->tell() + chunkSize;
		
		if( chunkName == gFmtMarker ) {
			readFormatChunk( stream );
			chunks |= hasFormat;
		} else if( chunkName == gDataMarker ) {
			mDataLength = chunkSize;
			mDataStart = stream->tell();
			chunks |= hasData;
		}
		stream->seekAbsolute( chunkEnd );
	}
	
	if( chunks != ( hasFormat | hasData ) ) {
		throw IoExceptionFailedLoad();
	}

	mSampleCount = mDataLength / mBlockAlign;
	
	//Pull all of the data
	//mData = (int16_t *)calloc( 1, mDataLength );
	//stream->seekSet( mDataStart );
	//stream->readData( mData, mDataLength );


}

SourceFileWav::~SourceFileWav()
{
}

void SourceFileWav::readFormatChunk( IStreamRef stream )
{
	readStreamWithEndianess( stream, &mAudioFormat, mIsBigEndian );
	readStreamWithEndianess( stream, &mChannelCount, mIsBigEndian );
	readStreamWithEndianess( stream, &mSampleRate, mIsBigEndian );
	readStreamWithEndianess( stream, &mByteRate, mIsBigEndian );
	readStreamWithEndianess( stream, &mBlockAlign, mIsBigEndian );
	readStreamWithEndianess( stream, &mBitsPerSample, mIsBigEndian );
	
	if(	mAudioFormat == WAV_FORMAT_PCM || mAudioFormat == WAV_FORMAT_IEEE_FLOAT ) {
		mIsPcm = true;
		mIsInterleaved = true;
		if( mAudioFormat == WAV_FORMAT_IEEE_FLOAT && mBitsPerSample == 32 ) {
			mDataType = FLOAT32;
		} else {
			if( mBitsPerSample == 32 ) {
				mDataType = INT32;
			} else if( mBitsPerSample == 16 ) {
				mDataType = INT16;
			} else if( mBitsPerSample == 8 ) {
				mDataType = UINT8;
			}
		}
	}
}

}} //namespace