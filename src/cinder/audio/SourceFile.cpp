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
	#if defined( __MAC_OS_X_VERSION_MAX_ALLOWED ) && (__MAC_OS_X_VERSION_MAX_ALLOWED < 1060)
		#define kAudioFileReadPermission fsRdPerm
	#endif
#endif

namespace cinder { namespace audio {

LoaderSourceFileRef	LoaderSourceFile::createRef( SourceFile *source, Target *target )
{
	return LoaderSourceFileRef( new LoaderSourceFile( source, target ) );
}

LoaderSourceFile::LoaderSourceFile( SourceFile *source, Target *target )
	: mSource( source ), mPacketOffset( 0 )
{
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
		throw IoExceptionUnsupportedDataFormat();
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
	
	mConverter = shared_ptr<CocoaCaConverter>( new CocoaCaConverter( this, &LoaderSourceFile::dataInputCallback, sourceDescription, targetDescription, mSource->mMaxPacketSize ) );
}

uint64_t LoaderSourceFile::getSampleOffset() const { 
	return mSource->sampleAtPacket( mPacketOffset ); 
}

void LoaderSourceFile::setSampleOffset( uint64_t anOffset ) {
	mPacketOffset = mSource->packetAtSample( anOffset );
}

void LoaderSourceFile::loadData( BufferList *ioData )
{	
	mConverter->loadData( ioData );
}

void LoaderSourceFile::dataInputCallback( Loader* aLoader, uint32_t *ioNumberDataPackets, BufferList *ioData, AudioStreamPacketDescription * packetDescriptions )
{
	LoaderSourceFile * theLoader = dynamic_cast<LoaderSourceFile *>( aLoader );
	SourceFile * theSource = theLoader->mSource;
	
	OSStatus err = AudioFileReadPackets( theSource->mFileRef.get(), false, (UInt32 *)&(ioData->mBuffers[0].mDataByteSize), packetDescriptions, theLoader->mPacketOffset, (UInt32 *)ioNumberDataPackets, ioData->mBuffers[0].mData );
	if( err ) {
		//throw ??
	}
	theLoader->mPacketOffset += *ioNumberDataPackets;
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
	
	IoRegistrar::registerSourceGeneric( sourceFunc, SOURCE_PRIORITY );
	
}

SourceFileRef	SourceFile::createFileRef( DataSourceRef dataSourceRef ) {
	return SourceFileRef( new SourceFile( dataSourceRef ) );
}

SourceFile::SourceFile( DataSourceRef dataSourceRef )
	: Source()
{
	OSStatus err = noErr;
	AudioFileID aFileRef;
	if( dataSourceRef->isFilePath() ) {
		::CFStringRef pathString = cocoa::createCfString( dataSourceRef->getFilePath() );
		::CFURLRef urlRef = ::CFURLCreateWithFileSystemPath( kCFAllocatorDefault, pathString, kCFURLPOSIXPathStyle, false );
		err = AudioFileOpenURL( urlRef, kAudioFileReadPermission, 0, &aFileRef );
		::CFRelease( pathString );
		::CFRelease( urlRef );
		if( err ) {
#if defined(CINDER_MAC)
			//TODO: find iphone equivalent of fnfErr
			if( err == fnfErr ) {
				throw IoExceptionSourceNotFound();
			}
#endif
			throw IoExceptionFailedLoad();
		}
	} else if( dataSourceRef->isUrl() ) {
		::CFURLRef urlRef = cocoa::createCfUrl( dataSourceRef->getUrl() );
		err = AudioFileOpenURL( urlRef, kAudioFileReadPermission, 0, &aFileRef );
		::CFRelease( urlRef );
		if( err ) {
			throw IoExceptionFailedLoad();
		}
	}
	mFileRef = shared_ptr<OpaqueAudioFileID>( aFileRef, AudioFileClose );
	
	
	//load header info
	AudioStreamBasicDescription nativeFormatDescription;
	UInt32 size = sizeof( AudioStreamBasicDescription );
	err = AudioFileGetProperty( aFileRef, kAudioFilePropertyDataFormat, &size, &nativeFormatDescription );
	if( err ) {
		throw IoExceptionFailedLoad();
	}
	
	loadFromCaAudioStreamBasicDescription( this, &nativeFormatDescription );
	
	size = sizeof( uint64_t );
	err = AudioFileGetProperty( aFileRef, kAudioFilePropertyAudioDataPacketCount, &size, &mPacketCount );
	if( err ) {
		throw IoExceptionFailedLoad();
	}
	
	size = sizeof( uint64_t );
	err = AudioFileGetProperty( aFileRef, kAudioFilePropertyAudioDataByteCount, &size, &mByteCount );
	if( err ) {
		throw IoExceptionFailedLoad();
	}
	
	size = sizeof( uint32_t );
	err = AudioFileGetProperty( aFileRef, kAudioFilePropertyMaximumPacketSize, &size, &mMaxPacketSize );
	if( err ) {
		throw IoExceptionFailedLoad();
	}
	
	size = sizeof( double );
	err = AudioFileGetProperty( aFileRef, kAudioFilePropertyEstimatedDuration, &size, &mDuration );
	if( err ) {
		throw IoExceptionFailedLoad();
	}
}

SourceFile::~SourceFile()
{
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