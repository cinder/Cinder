/*
 Copyright (c) 2014, The Cinder Project

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

#include "cinder/audio/cocoa/FileCoreAudio.h"
#include "cinder/audio/cocoa/CinderCoreAudio.h"
#include "cinder/cocoa/CinderCocoa.h"
#include "cinder/audio/Exception.h"

#include <AudioToolbox/AudioFile.h>

using namespace std;
using namespace ci;

namespace cinder { namespace audio { namespace cocoa {

// ----------------------------------------------------------------------------------------------------
// MARK: - SourceFileCoreAudio
// ----------------------------------------------------------------------------------------------------

SourceFileCoreAudio::SourceFileCoreAudio()
	: SourceFile( 0 )
{}

SourceFileCoreAudio::SourceFileCoreAudio( const DataSourceRef &dataSource, size_t sampleRate )
	: SourceFile( sampleRate ), mDataSource( dataSource )
{
	CI_ASSERT_MSG( mDataSource->isFilePath(), "only DataSource type supported is file" );

	initImpl();
}

SourceFileRef SourceFileCoreAudio::cloneWithSampleRate( size_t sampleRate ) const
{
	shared_ptr<SourceFileCoreAudio> result( new SourceFileCoreAudio( mDataSource, sampleRate ) );
	result->setupSampleRateConversion();

	return result;
}

SourceFileCoreAudio::~SourceFileCoreAudio()
{
}

void SourceFileCoreAudio::initImpl()
{
	::CFURLRef fileUrl = ci::cocoa::createCfUrl( Url( mDataSource->getFilePath().string() ) );
	::ExtAudioFileRef audioFile;
	OSStatus status = ::ExtAudioFileOpenURL( fileUrl, &audioFile );
	if( fileUrl ) {
		CFRelease(fileUrl);
		fileUrl = NULL;
	}
	if( status != noErr ) {
		throw AudioFileExc( string( "could not open audio source file: " ) + mDataSource->getFilePath().string(), (int32_t)status );
	}

	mExtAudioFile = ExtAudioFilePtr( audioFile );

	::AudioStreamBasicDescription fileFormat;
	UInt32 propSize = sizeof( fileFormat );
	status = ::ExtAudioFileGetProperty( audioFile, kExtAudioFileProperty_FileDataFormat, &propSize, &fileFormat );
	CI_VERIFY( status == noErr );

	mNumChannels = fileFormat.mChannelsPerFrame;
	mSampleRateNative = fileFormat.mSampleRate;

	size_t outputSampleRate = getSampleRate(); // may be 0 at init
	if( ! outputSampleRate )
		outputSampleRate = mSampleRateNative;

	SInt64 numFrames;
	propSize = sizeof( numFrames );
	status = ::ExtAudioFileGetProperty( audioFile, kExtAudioFileProperty_FileLengthFrames, &propSize, &numFrames );
	CI_VERIFY( status == noErr );
	mNumFrames = mFileNumFrames = static_cast<size_t>( numFrames );

	::AudioStreamBasicDescription outputFormat = audio::cocoa::createFloatAsbd( outputSampleRate, mNumChannels );
	status = ::ExtAudioFileSetProperty( mExtAudioFile.get(), kExtAudioFileProperty_ClientDataFormat, sizeof( outputFormat ), &outputFormat );
	CI_VERIFY( status == noErr );

	// numFrames will be updated at read time
	mBufferList = createNonInterleavedBufferListShallow( mNumChannels );
}

size_t SourceFileCoreAudio::performRead( Buffer *buffer, size_t bufferFrameOffset, size_t numFramesNeeded )
{
	for( int ch = 0; ch < mNumChannels; ch++ ) {
		mBufferList->mBuffers[ch].mDataByteSize = UInt32( numFramesNeeded * sizeof( float ) );
		mBufferList->mBuffers[ch].mData = &buffer->getChannel( ch )[bufferFrameOffset];
	}

	UInt32 frameCount = (UInt32)numFramesNeeded;
	OSStatus status = ::ExtAudioFileRead( mExtAudioFile.get(), &frameCount, mBufferList.get() );
	CI_VERIFY( status == noErr );

	return (size_t)frameCount;
}

void SourceFileCoreAudio::performSeek( size_t readPositionFrames )
{
	OSStatus status = ::ExtAudioFileSeek( mExtAudioFile.get(), readPositionFrames );
	CI_VERIFY( status == noErr );
}

// static
vector<string> SourceFileCoreAudio::getSupportedExtensions()
{
	vector<string> result;

	::CFArrayRef extensionsCF;
	UInt32 propSize = sizeof( extensionsCF );
	OSStatus status = ::AudioFileGetGlobalInfo( kAudioFileGlobalInfo_AllExtensions, 0, NULL, &propSize, &extensionsCF );
	CI_VERIFY( status == noErr );

	CFIndex extCount = ::CFArrayGetCount( extensionsCF );
	for( CFIndex index = 0; index < extCount; ++index ) {
		string ext = ci::cocoa::convertCfString( (CFStringRef)::CFArrayGetValueAtIndex( extensionsCF, index ) );
		result.push_back( ext );
	}

	::CFRelease( extensionsCF );
	
	return result;
}

// ----------------------------------------------------------------------------------------------------
// MARK: - TargetFileCoreAudio
// ----------------------------------------------------------------------------------------------------

TargetFileCoreAudio::TargetFileCoreAudio( const DataTargetRef &dataTarget, size_t sampleRate, size_t numChannels, SampleType sampleType, const std::string &extension )
	: TargetFile( sampleRate, numChannels, sampleType )
{
	::CFURLRef targetUrl = ci::cocoa::createCfUrl( Url( dataTarget->getFilePath().string() ) );
	::AudioFileTypeID fileType = getFileTypeIdFromExtension( extension );

	::AudioStreamBasicDescription fileAsbd;
	switch( mSampleType ) {
		case SampleType::INT_16:
			fileAsbd = createInt16Asbd( mSampleRate, mNumChannels, true );
			break;
		case SampleType::FLOAT_32:
			fileAsbd = createFloatAsbd( mSampleRate, mNumChannels, true );
			break;
		default:
			CI_ASSERT_NOT_REACHABLE();
	}

	::ExtAudioFileRef audioFile;
	OSStatus status = ::ExtAudioFileCreateWithURL( targetUrl, fileType, &fileAsbd, nullptr, kAudioFileFlags_EraseFile, &audioFile );
	if( status == kAudioFileUnsupportedDataFormatError )
		throw AudioFileExc( string( "Extended Audio File Services: Unsupported format error, target file: " ) + dataTarget->getFilePath().string(), (int32_t)status );
	else if( status != noErr )
		throw AudioFileExc( string( "Extended Audio File Services faild: target file: " ) + dataTarget->getFilePath().string(), (int32_t)status );

	::CFRelease( targetUrl );
	mExtAudioFile = ExtAudioFilePtr( audioFile );

	::AudioStreamBasicDescription clientAsbd = createFloatAsbd( mSampleRate, mNumChannels, false );

	status = ::ExtAudioFileSetProperty( mExtAudioFile.get(), kExtAudioFileProperty_ClientDataFormat, sizeof( clientAsbd ), &clientAsbd );
	CI_VERIFY( status == noErr );

	mBufferList = createNonInterleavedBufferListShallow( mNumChannels );
}

void TargetFileCoreAudio::performWrite( const Buffer *buffer, size_t numFrames, size_t frameOffset )
{
	for( int ch = 0; ch < mNumChannels; ch++ ) {
		mBufferList->mBuffers[ch].mDataByteSize = (UInt32)numFrames * sizeof( float );
		mBufferList->mBuffers[ch].mData = (void *)( buffer->getChannel( ch ) + frameOffset );
	}

	OSStatus status = ::ExtAudioFileWrite( mExtAudioFile.get(), (UInt32)numFrames, mBufferList.get() );
	CI_VERIFY( status == noErr );
}

::AudioFileTypeID TargetFileCoreAudio::getFileTypeIdFromExtension( const std::string &ext )
{
	if( ext == "aiff" )
		return kAudioFileAIFFType;
	else if( ext == "wav" )
		return kAudioFileWAVEType;
	else if( ext == "mp3" )
		return kAudioFileMP3Type;
	else if( ext == "m4a" )
		return kAudioFileM4AType;
	else if( ext == "aac" )
		return kAudioFileAAC_ADTSType;

	throw AudioFileExc( string( "TargetFileCoreAudio: unexpected extension '" ) + ext + "'" );
}

} } } // namespace cinder::audio::cocoa
