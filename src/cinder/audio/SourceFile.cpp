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

#include "cinder/audio/SourceFile.h"
#include "cinder/cocoa/CinderCocoa.h"

#include <iostream>

#if defined( CINDER_COCOA )
	#include <CoreFoundation/CoreFoundation.h>
#endif

namespace cinder { namespace audio {

//TODO: abstract this to be an CaAudioConverter Loader? 

LoaderSourceFileRef	LoaderSourceFile::createRef( SourceFile *source, Target *target )
{
	return LoaderSourceFileRef( new LoaderSourceFile( source, target ) );
}

LoaderSourceFile::LoaderSourceFile( SourceFile *source, Target *target )
	: mSource( source ), mPacketOffset( 0 ), mCurrentPacketDescriptions( 0 )
{
	mConverterBuffer.mNumberBuffers = 0;
	mConverterBuffer.mBuffers = NULL;

	AudioStreamBasicDescription sourceDescription;
	
	sourceDescription.mFormatID = source->mNativeFormatId; //kAudioFormatLinearPCM;
	sourceDescription.mFormatFlags = source->mNativeFormatFlags; //CalculateLPCMFlags( mSource->getBitsPerSample(), mSource->getBlockAlign() * 8, false, false, false );
	sourceDescription.mSampleRate = source->getSampleRate();
	sourceDescription.mBytesPerPacket = source->mBytesPerPacket; //( mSource->getBitsPerSample() * mSource->getChannelCount() ) / 8;
	sourceDescription.mFramesPerPacket = source->mFramesPerPacket; //1;
	sourceDescription.mBytesPerFrame = source->mBytesPerFrame; //( mSource->getBitsPerSample() * mSource->getChannelCount() ) / 8;
	sourceDescription.mChannelsPerFrame = source->getChannelCount();
	sourceDescription.mBitsPerChannel = source->getBitsPerSample();
	
	AudioStreamBasicDescription targetDescription;
	
	if( ! target->isPcm() ) {
		//throw!
	}
	
	//right now this always converts to linear PCM --that's probably ok
	targetDescription.mFormatID = kAudioFormatLinearPCM; //target->mNativeFormatId;
	targetDescription.mFormatFlags = CalculateLPCMFlags( target->getBitsPerSample(), target->getBlockAlign() * 8, target->isFloat(), target->isBigEndian(), ( ! target->isInterleaved() ) ); //target->mNativeFormatFlags
	targetDescription.mSampleRate = target->getSampleRate();
	targetDescription.mBytesPerPacket =  ( mSource->getBitsPerSample() * mSource->getChannelCount() ) / 8; //target->mBytesPerPacket;
	targetDescription.mFramesPerPacket = 1; //target->mFramesPerPacket;
	targetDescription.mBytesPerFrame = ( mSource->getBlockAlign() ); //target->mBytesPerFrame;
	targetDescription.mChannelsPerFrame = target->getChannelCount();
	targetDescription.mBitsPerChannel = target->getBitsPerSample();
	
	OSStatus err = noErr;
	err = AudioConverterNew( &sourceDescription, &targetDescription, &mConverter );
}

LoaderSourceFile::~LoaderSourceFile()
{
	cleanupPacketDescriptions();
	cleanupConverterBuffer();
	
	AudioConverterDispose( mConverter );
}

uint64_t LoaderSourceFile::getSampleOffset() const { 
	return mSource->sampleAtPacket( mPacketOffset ); 
}

void LoaderSourceFile::setSampleOffset( uint64_t anOffset ) {
	mPacketOffset = mSource->packetAtSample( anOffset );
}

void LoaderSourceFile::loadData( uint32_t *ioSampleCount, BufferList *ioData )
{	
	shared_ptr<AudioBufferList> nativeBufferList = createCaBufferList( ioData );

	AudioStreamPacketDescription * outputPacketDescriptions = new AudioStreamPacketDescription[*ioSampleCount];
	OSStatus err = AudioConverterFillComplexBuffer( mConverter, LoaderSourceFile::dataInputCallback, (void *)this, (UInt32 *)ioSampleCount, nativeBufferList.get(), outputPacketDescriptions );
	delete [] outputPacketDescriptions;
	if( err ) {
		//throw
	}
	
	fillBufferListFromCaBufferList( ioData, nativeBufferList.get() );
}

void LoaderSourceFile::cleanupPacketDescriptions()
{
	if( mCurrentPacketDescriptions ) {
		delete [] mCurrentPacketDescriptions;
		mCurrentPacketDescriptions = 0;
	}
}

void LoaderSourceFile::cleanupConverterBuffer() 
{
	for( int i = 0; i < mConverterBuffer.mNumberBuffers; i++ ) {
		free( mConverterBuffer.mBuffers[i].mData );
	}
	if( mConverterBuffer.mBuffers ) {
		delete [] mConverterBuffer.mBuffers;
		mConverterBuffer.mBuffers = NULL;
	}
}

OSStatus LoaderSourceFile::dataInputCallback( AudioConverterRef inAudioConverter, UInt32 *ioNumberDataPackets, AudioBufferList *ioData, AudioStreamPacketDescription **outDataPacketDescriptions, void *audioLoader )
{
	OSStatus err = noErr;
	
	LoaderSourceFile * theLoader = (LoaderSourceFile *)audioLoader;
	SourceFile * theSource = theLoader->mSource;
	
	theLoader->cleanupConverterBuffer();
	
	theLoader->mConverterBuffer.mNumberBuffers = ioData->mNumberBuffers;
	theLoader->mConverterBuffer.mBuffers = new Buffer[theLoader->mConverterBuffer.mNumberBuffers];
	for( int i = 0; i < theLoader->mConverterBuffer.mNumberBuffers; i++ ) {
		theLoader->mConverterBuffer.mBuffers[i].mNumberChannels = ioData->mBuffers[i].mNumberChannels;
		theLoader->mConverterBuffer.mBuffers[i].mDataByteSize = ( *ioNumberDataPackets * theSource->mMaxPacketSize * theLoader->mConverterBuffer.mBuffers[i].mNumberChannels );
		theLoader->mConverterBuffer.mBuffers[i].mData = malloc( theLoader->mConverterBuffer.mBuffers[i].mDataByteSize );
	}
	
	theLoader->cleanupPacketDescriptions();
	theLoader->mCurrentPacketDescriptions = new AudioStreamPacketDescription[*ioNumberDataPackets];
	
	err = AudioFileReadPackets( theSource->mFileRef, false, (UInt32 *)&(theLoader->mConverterBuffer.mBuffers[0].mDataByteSize), theLoader->mCurrentPacketDescriptions, theLoader->mPacketOffset, (UInt32 *)ioNumberDataPackets, theLoader->mConverterBuffer.mBuffers[0].mData );
	
	//ioData->mBuffers[0].mData = theTrack->mSourceBuffer;
	for( int i = 0; i < ioData->mNumberBuffers; i++ ) {
			ioData->mBuffers[i].mData = theLoader->mConverterBuffer.mBuffers[i].mData;
			ioData->mBuffers[i].mNumberChannels = theLoader->mConverterBuffer.mBuffers[i].mNumberChannels;
			ioData->mBuffers[i].mDataByteSize = theLoader->mConverterBuffer.mBuffers[i].mDataByteSize;
	}
	
	if( outDataPacketDescriptions ) {
		*outDataPacketDescriptions = theLoader->mCurrentPacketDescriptions;
	}
	
	theLoader->mPacketOffset += *ioNumberDataPackets;
	
	/*if( theTrack->mIsLooping && theTrack->mPacketOffset >= theSource->getPacketCount() ) {
		theTrack->mPacketOffset = 0;
	}*/
	
    return err;
}


void SourceFile::registerSelf()
{
	static const int32_t SOURCE_PRIORITY = 2;
	
	IoRegistrar::SourceCreationFunc sourceFunc = SourceFile::createRef;
	
	OSStatus err;
	UInt32 propertySize = 0;
	err = AudioFileGetGlobalInfoSize(kAudioFileGlobalInfo_ReadableTypes, 0, NULL, &propertySize);
	if( err ) {
		//registers no types on error
		return;
	}
	uint32_t * audioTypes = (uint32_t *)malloc(propertySize);
	err = AudioFileGetGlobalInfo(kAudioFileGlobalInfo_ReadableTypes, 0, NULL, &propertySize, audioTypes );
	if( err ) {
		//registers no types on error
		free(audioTypes);
		return;
	}
	
	int nTypes = propertySize / sizeof(uint32_t);
	for( int i = 0; i < nTypes; i++ ) {
		CFArrayRef extensions = NULL;
		propertySize = sizeof(extensions);
		err = AudioFileGetGlobalInfo(kAudioFileGlobalInfo_ExtensionsForType, sizeof(uint32_t), (UInt32 *)&audioTypes[i], &propertySize, &extensions);
		if ( ! err ) {
			CFIndex nExtensions = CFArrayGetCount(extensions);
			for( int j = 0; j < nExtensions; j++ ) {
				CFStringRef cfext = (CFStringRef)CFArrayGetValueAtIndex(extensions, j);
				Boolean res = FALSE;
				char * ext = NULL;
				uint8_t extLen = 3;
				while( ! res && extLen < 5 ) {
					ext = new char[extLen];
					res = CFStringGetCString( cfext, ext, extLen * sizeof(char), kCFStringEncodingASCII );
					if( res ) {
						IoRegistrar::registerSourceType( ext, sourceFunc, SOURCE_PRIORITY );
					}
					delete ext;
					ext = NULL;
					extLen++;
				}
			}
		}		
	}
	free(audioTypes);
	
}

SourceFileRef	SourceFile::createFileRef( DataSourceRef dataSourceRef ) {
	return SourceFileRef( new SourceFile( dataSourceRef ) );
}

SourceFile::SourceFile( DataSourceRef dataSourceRef )
	: Source()
{
	OSStatus err = noErr;
	if( dataSourceRef->isFilePath() ) {
		::CFStringRef pathString = cocoa::createCfString( dataSourceRef->getFilePath() );
		::CFURLRef urlRef = ::CFURLCreateWithFileSystemPath( kCFAllocatorDefault, pathString, kCFURLPOSIXPathStyle, false );
		err = AudioFileOpenURL( urlRef, fsRdPerm, 0, &mFileRef );
		::CFRelease( pathString );
		::CFRelease( urlRef );
		if( err ) {
			//throw
		}
	} else if( dataSourceRef->isUrl() ) {
		::CFURLRef urlRef = cocoa::createCfUrl( dataSourceRef->getUrl() );
		err = AudioFileOpenURL( urlRef, fsRdPerm, 0, &mFileRef );
		::CFRelease( urlRef );
		if( err ) {
			//throw
		}
	}
	
	//load header info
	AudioStreamBasicDescription nativeFormatDescription;
	UInt32 size = sizeof( AudioStreamBasicDescription );
	err = AudioFileGetProperty( mFileRef, kAudioFilePropertyDataFormat, &size, &nativeFormatDescription );
	if( err ) {
		// throw
	}
	
	loadFromCaAudioStreamBasicDescription( this, &nativeFormatDescription );
	
	size = sizeof( uint64_t );
	err = AudioFileGetProperty( mFileRef, kAudioFilePropertyAudioDataPacketCount, &size, &mPacketCount );
	if( err ) {
		//throw
	}
	
	size = sizeof( uint64_t );
	err = AudioFileGetProperty( mFileRef, kAudioFilePropertyAudioDataByteCount, &size, &mByteCount );
	if( err ) {
		//throw
	}
	
	size = sizeof( uint32_t );
	err = AudioFileGetProperty( mFileRef, kAudioFilePropertyMaximumPacketSize, &size, &mMaxPacketSize );
	if( err ) {
		//throw
	}
	
	size = sizeof( double );
	err = AudioFileGetProperty( mFileRef, kAudioFilePropertyEstimatedDuration, &size, &mDuration );
	if( err ) {
		//throw
	}
}

SourceFile::~SourceFile()
{
	AudioFileClose( mFileRef );
}

uint64_t SourceFile::packetAtTime( double aTime ) const
{
	return (uint64_t)( ( aTime * mSampleRate ) / mFramesPerPacket );
}

double SourceFile::timeAtPacket( uint64_t aPacket ) const
{
	return ( aPacket * mFramesPerPacket ) / (double)mSampleRate;
}

uint64_t SourceFile::packetAtSample( uint64_t aSample ) const
{
	return aSample  / (uint64_t)mFramesPerPacket;
}

uint64_t SourceFile::sampleAtPacket( uint64_t aPacket ) const
{
	return ( mFramesPerPacket * aPacket  );
}

}} //namespace