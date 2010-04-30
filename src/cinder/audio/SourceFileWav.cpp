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
#include <cstring>

namespace cinder { namespace audio {

LoaderSourceFileWavRef LoaderSourceFileWav::createRef( SourceFileWav *source, Target *target ) {
	return LoaderSourceFileWavRef( new LoaderSourceFileWav( source, target ) );
}

LoaderSourceFileWav::LoaderSourceFileWav( SourceFileWav * source, Target * target ) 
	: Loader(), mSource( source ), mSampleOffset( 0 )
{

}

LoaderSourceFileWav::~LoaderSourceFileWav()
{
}

uint64_t LoaderSourceFileWav::getSampleOffset() const
{
	return mSampleOffset;
}

void LoaderSourceFileWav::setSampleOffset( uint64_t anOffset )
{
	mSampleOffset = anOffset;
}

void LoaderSourceFileWav::loadData( uint32_t *ioSampleCount, BufferList *ioData )
{	

}

void SourceFileWav::registerSelf()
{
	static const int32_t SOURCE_PRIORITY = 2;
	
	IoRegistrar::SourceCreationFunc sourceFunc = SourceFileWav::createRef;
	
	//TODO: find a way to enumerate Windows Media supported file formats
	IoRegistrar::registerSourceType( "wav", sourceFunc, SOURCE_PRIORITY );
}

SourceFileWavRef	SourceFileWav::createFileWavRef( DataSourceRef dataSourceRef )
{
	return SourceFileWavRef( new SourceFileWav( dataSourceRef ) );
}

SourceFileWav::SourceFileWav( DataSourceRef dataSourceRef )
	: Source()
{
	mDataType = DATA_UNKNOWN;
	mSampleRate = 0;
	mChannelCount = 0;
	mBitsPerSample = 0;
	mBlockAlign = 0;
	mIsPcm = FALSE;
	mIsBigEndian = TRUE;
	mIsInterleaved = FALSE;
	/*if( dataSourceRef->isFilePath() ) {
		//TODO: implement
	} else if ( dataSourceRef->isFilePath() ) {
		//TODO: implement WindowsMedia network functionallity
	}else { //have to use buffer
		//TODO: move current implementation into this
	}*/
	IStreamRef stream = dataSourceRef->getStream();

	uint32_t fileSize = 0;
	
	char chunkName[5] = { 0 };
	uint32_t chunkSize = 0;
	
	char riffType[5] = { 0 };
	
	stream->readData( chunkName, 4 );
	stream->readLittle( &fileSize );
	fileSize = fileSize + 4 + sizeof( int );
	
	stream->readData( riffType, 4 );
	if( strcmp( "RIFF", chunkName ) != 0 || strcmp( "WAVE", riffType ) != 0  ) {
		//TODO: Throw Invalid Audio File exception
	}
	
	
	uint32_t chunkEnd = 0;
	
	while( stream->tell() < fileSize) {
		stream->readData( chunkName, 4 );
		stream->readLittle( &chunkSize );
		chunkEnd = stream->tell() + chunkSize;

		OutputDebugStringA( chunkName );
		
		if( strcmp( "fmt ", chunkName ) == 0 ) {
			//readFormatChunk();
		} else if( strcmp( "data", chunkName ) == 0 ) {
			mDataLength = chunkSize;
			mDataStart = stream->tell();
		}
		
		stream->seekAbsolute( chunkEnd );
	}
	
	mSampleCount = mDataLength / ( mBitsPerSample / 8 );
	
	//Pull all of the data
	//mData = (int16_t *)calloc( 1, mDataLength );
	//mStream->seekSet( mDataStart );
	//mStream->readData( mData, mDataLength );


}

SourceFileWav::~SourceFileWav()
{
	
}


}} //namespace