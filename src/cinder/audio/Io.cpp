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

#include "cinder/audio/Io.h"
#include "cinder/Utilities.h"

#if defined(CINDER_MSW)
	// this is necessary to force the instantiation of the IMAGEIO_REGISTER macro
	#include "cinder/audio/SourceFileWindowsMedia.h"
	#include "cinder/audio/SourceFileWav.h"	
#endif

using namespace std;

namespace cinder { namespace audio {

SourceRef load( const std::string &path, std::string extension )
{
	return load( DataSourcePath::createRef( path ), extension );
}

SourceRef load( DataSourceRef dataSource, std::string extension )
{
	if( extension.empty() )
		extension = getPathExtension( dataSource->getFilePathHint() );
	
	return IoRegistrar::createSource( dataSource, extension );
}

#if defined(CINDER_COCOA)
void Io::loadFromCaAudioStreamBasicDescription( Io * anIo, const AudioStreamBasicDescription * nativeFormatDescription )
{
	anIo->mSampleRate = nativeFormatDescription->mSampleRate;
	anIo->mNativeFormatId = nativeFormatDescription->mFormatID;
	anIo->mNativeFormatFlags = nativeFormatDescription->mFormatFlags;
	anIo->mBytesPerPacket = nativeFormatDescription->mBytesPerPacket;
	anIo->mFramesPerPacket = nativeFormatDescription->mFramesPerPacket;
	anIo->mBytesPerFrame = nativeFormatDescription->mBytesPerFrame;
	anIo->mChannelCount = nativeFormatDescription->mChannelsPerFrame;
	anIo->mBitsPerSample = nativeFormatDescription->mBitsPerChannel;
	
	anIo->mDataType = DATA_UNKNOWN;
	anIo->mIsBigEndian = false;
	anIo->mIsInterleaved = false;
	anIo->mBlockAlign = 0;
	if( anIo->mNativeFormatId == kAudioFormatLinearPCM ) {
		anIo->mIsPcm = true;
		anIo->mBlockAlign = anIo->mBytesPerFrame;
		anIo->mIsBigEndian = nativeFormatDescription->mFormatFlags & kAudioFormatFlagIsBigEndian;
		anIo->mIsInterleaved = ! ( nativeFormatDescription->mFormatFlags & kAudioFormatFlagIsNonInterleaved );
		if( ( nativeFormatDescription->mFormatFlags & kAudioFormatFlagIsFloat ) && anIo->mBitsPerSample == 32 ) {
			anIo->mDataType = FLOAT32;
		} else if( ( nativeFormatDescription->mFormatFlags & kAudioFormatFlagIsSignedInteger ) ) {
			if( anIo->mBitsPerSample == 32 ) {
				anIo->mDataType = INT32;
			} else if( anIo->mBitsPerSample == 16 ) {
				anIo->mDataType = INT16;
			} else if( anIo->mBitsPerSample == 8 ) {
				anIo->mDataType = INT8;
			}
		} else {
			if( anIo->mBitsPerSample == 32 ) {
				anIo->mDataType = UINT32;
			} else if( anIo->mBitsPerSample == 16 ) {
				anIo->mDataType = UINT16;
			} else if( anIo->mBitsPerSample == 8 ) {
				anIo->mDataType = UINT8;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void Loader::fillBufferListFromCaBufferList( BufferList * aBufferList, const AudioBufferList * caBufferList )
{
	aBufferList->mNumberBuffers = caBufferList->mNumberBuffers;
	for( int i = 0; i < caBufferList->mNumberBuffers; i++ ) {
		aBufferList->mBuffers[i].mNumberChannels = caBufferList->mBuffers[i].mNumberChannels;
		aBufferList->mBuffers[i].mDataByteSize = caBufferList->mBuffers[i].mDataByteSize;
		aBufferList->mBuffers[i].mData = caBufferList->mBuffers[i].mData;
	}
}

shared_ptr<AudioBufferList> Loader::createCaBufferList( const BufferList * aBufferList )
{
	shared_ptr<AudioBufferList> caBufferList( (AudioBufferList *)malloc( sizeof( UInt32 ) + ( sizeof( AudioBuffer ) * aBufferList->mNumberBuffers ) ), free );
	
	caBufferList->mNumberBuffers = aBufferList->mNumberBuffers;
	for( int i = 0; i < caBufferList->mNumberBuffers; i++ ) {
		caBufferList->mBuffers[i].mNumberChannels = aBufferList->mBuffers[i].mNumberChannels;
		caBufferList->mBuffers[i].mDataByteSize = aBufferList->mBuffers[i].mDataByteSize;
		caBufferList->mBuffers[i].mData = aBufferList->mBuffers[i].mData;
	}
	
	return caBufferList;
}
#endif

///////////////////////////////////////////////////////////////////////////////
IoRegistrar::Inst* IoRegistrar::instance()
{
	static shared_ptr<Inst> sInst;
	if( ! sInst ) {
		sInst = shared_ptr<Inst>( new IoRegistrar::Inst );
	}
	return sInst.get();
}

SourceRef IoRegistrar::createSource( DataSourceRef dataSource, string extension )
{
	return instance()->createSource( dataSource, extension );
}

SourceRef IoRegistrar::Inst::createSource( DataSourceRef dataSource, string extension )
{
	std::transform( extension.begin(), extension.end(), extension.begin(), static_cast<int(*)(int)>( tolower ) );

	// for non-empty extensions we'll walk everyone who is registered for this extension
	if( ! extension.empty() ) {
		map<string,multimap<int32_t,IoRegistrar::SourceCreationFunc> >::iterator sIt = mSources.find( extension );
		if( sIt != mSources.end() ) {
			for( multimap<int32_t,IoRegistrar::SourceCreationFunc>::const_iterator sourcesIt = sIt->second.begin(); sourcesIt != sIt->second.end(); ++sourcesIt ) {
				try {
					return (*(sourcesIt->second))( dataSource );
				}
				catch( ... ) { // we'll ignore exceptions and move to the next handler
				}
			}
		}
	}

	// if there is no extension, or none of the registered types got it, we'll have try the generic loaders	
	for( map<int32_t, IoRegistrar::SourceCreationFunc>::const_iterator genericIt = mGenericSources.begin(); genericIt != mGenericSources.end(); ++genericIt ) {
		try {
			return (*(genericIt->second))( dataSource );
		}
		catch( ... ) { // we'll ignore exceptions and move to the next handler
		}
	}
	
	// failure
	return SourceRef();
}

void IoRegistrar::registerSourceType( string extension, SourceCreationFunc func, int32_t priority ) 
{
	instance()->registerSourceType( extension, func, priority );
}

void IoRegistrar::Inst::registerSourceType( string extension, SourceCreationFunc func, int32_t priority )
{
	// make sure the extension is all lower-case
	std::transform( extension.begin(), extension.end(), extension.begin(), static_cast<int(*)(int)>(tolower) );	
	
	map<string,multimap<int32_t,SourceCreationFunc> >::iterator sIt = mSources.find( extension );
	if( sIt != mSources.end() ) {	// do we already have a source of this extension?
		// let's make sure this func is not already registered against this extension (which can happen in the big generic handlers)
		for( multimap<int32_t,SourceCreationFunc>::const_iterator existIt = sIt->second.begin(); existIt != sIt->second.end(); ++existIt )
			if( existIt->second == func )
				return;
		sIt->second.insert( make_pair( priority, func ) );
	}
	else {
		multimap<int32_t,SourceCreationFunc> newMap;
		newMap.insert( make_pair( priority, func ) );
		mSources.insert( make_pair( extension, newMap ) );
	}
}

void IoRegistrar::registerSourceGeneric( SourceCreationFunc func, int32_t priority ) 
{
	instance()->registerSourceGeneric( func, priority );
}

void IoRegistrar::Inst::registerSourceGeneric( SourceCreationFunc func, int32_t priority )
{
		mGenericSources.insert( make_pair( priority, func ) );
}

}} //namespace