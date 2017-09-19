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

#include "cinder/audio/Source.h"
#include "cinder/audio/dsp/Converter.h"
#include "cinder/audio/FileOggVorbis.h"

#include "cinder/Utilities.h"

#if defined( CINDER_COCOA )
	#include "cinder/audio/cocoa/FileCoreAudio.h"
#elif defined( CINDER_MSW )
	#include "cinder/audio/msw/FileMediaFoundation.h"
#elif defined( CINDER_LINUX )
 	#include "cinder/audio/linux/FileAudioLoader.h"
#endif

#include <cmath>

using namespace std;

namespace cinder { namespace audio {

// TODO: these should be replaced with a generic registrar derived from the ImageIo stuff.

// static
unique_ptr<SourceFile> SourceFile::create( const DataSourceRef &dataSource, size_t sampleRate )
{
	unique_ptr<SourceFile> result;

#if ! defined( CINDER_UWP ) || ( _MSC_VER > 1800 )
	if( dataSource->getFilePathHint().extension().string() == ".ogg" )
#else
	if( dataSource->getFilePathHint().extension() == ".ogg" )
#endif
		result.reset( new SourceFileOggVorbis( dataSource, sampleRate ) );
	else {
#if defined( CINDER_COCOA )
		result.reset( new cocoa::SourceFileCoreAudio( dataSource, sampleRate ) );
#elif defined( CINDER_MSW )
		result.reset( new msw::SourceFileMediaFoundation( dataSource, sampleRate ) );
#elif defined( CINDER_LINUX )
		result.reset( new linux::SourceFileAudioLoader( dataSource, sampleRate ) );		
#endif
	}

	if( result )
		result->setupSampleRateConversion();

	return result;
}

// static
vector<std::string> SourceFile::getSupportedExtensions()
{
#if defined( CINDER_COCOA )
	vector<string> result = cocoa::SourceFileCoreAudio::getSupportedExtensions();
#elif defined( CINDER_MSW )
	vector<string> result = msw::SourceFileMediaFoundation::getSupportedExtensions();
#elif defined( CINDER_LINUX )	
	vector<string> result = linux::SourceFileAudioLoader::getSupportedExtensions();
#else
	vector<string> result;
#endif

	if( find( result.begin(), result.end(), "ogg" ) == result.end() )
		result.push_back( "ogg" );

	return result;
}

Source::Source( size_t sampleRate )
	: mSampleRate( sampleRate ), mMaxFramesPerRead( 4096 )
{
}

Source::~Source()
{
}

SourceFile::SourceFile( size_t sampleRate )
	: Source( sampleRate ), mNumFrames( 0 ), mFileNumFrames( 0 ), mReadPos( 0 )
{
}

void SourceFile::setupSampleRateConversion()
{
	size_t nativeSampleRate = getSampleRateNative();
	size_t outputSampleRate = getSampleRate();
	if( ! outputSampleRate ) {
		outputSampleRate = nativeSampleRate;
		setSampleRate( nativeSampleRate );
	}
	else if( outputSampleRate != nativeSampleRate ) {
		mNumFrames = (size_t)std::ceil( (float)mFileNumFrames * (float)outputSampleRate / (float)nativeSampleRate );

		if( ! supportsConversion() ) {
			size_t numChannels = getNumChannels();
			mConverter = audio::dsp::Converter::create( nativeSampleRate, outputSampleRate, numChannels, numChannels, getMaxFramesPerRead() );
			mConverterReadBuffer.setSize( getMaxFramesPerRead(), numChannels );
		}
	}
	else {
		mNumFrames = mFileNumFrames;
		mConverter.reset();
	}
}

size_t SourceFile::read( Buffer *buffer )
{
	CI_ASSERT( buffer->getNumChannels() == getNumChannels() );
	CI_ASSERT( mReadPos < mNumFrames );

	size_t numRead;

	if( mConverter ) {
		size_t sourceBufFrames = size_t( buffer->getNumFrames() * (float)getSampleRateNative() / (float)getSampleRate() );
		size_t numFramesNeeded = std::min( mFileNumFrames - mReadPos, std::min( getMaxFramesPerRead(), sourceBufFrames ) );

		mConverterReadBuffer.setNumFrames( numFramesNeeded );
		performRead( &mConverterReadBuffer, 0, numFramesNeeded );
		pair<size_t, size_t> count = mConverter->convert( &mConverterReadBuffer, buffer );
		numRead = count.second;
	}
	else {
		size_t numFramesNeeded = std::min( mNumFrames - mReadPos, std::min( getMaxFramesPerRead(), buffer->getNumFrames() ) );
		numRead = performRead( buffer, 0, numFramesNeeded );
	}

	mReadPos += numRead;
	return numRead;
}

BufferRef SourceFile::loadBuffer()
{
	seek( 0 );

	BufferRef result = make_shared<Buffer>( mNumFrames, getNumChannels() );

	if( mConverter ) {
		// TODO: need BufferView's in order to reduce number of copies
		Buffer converterDestBuffer( mConverter->getDestMaxFramesPerBlock(), getNumChannels() );
		size_t readCount = 0;
		while( true ) {
			size_t framesNeeded = std::min( getMaxFramesPerRead(), mFileNumFrames - readCount );
			if( framesNeeded == 0 )
				break;

			// make sourceBuffer num frames match outNumFrames so that Converter doesn't think it has more
			if( framesNeeded != mConverterReadBuffer.getNumFrames() )
				mConverterReadBuffer.setNumFrames( framesNeeded );

			size_t outNumFrames = performRead( &mConverterReadBuffer, 0, framesNeeded );
			CI_ASSERT( outNumFrames == framesNeeded );

			pair<size_t, size_t> count = mConverter->convert( &mConverterReadBuffer, &converterDestBuffer );
			result->copyOffset( converterDestBuffer, count.second, mReadPos, 0 );

			readCount += outNumFrames;
			mReadPos += count.second;
		}
	}
	else {
		size_t readCount = performRead( result.get(), 0, mNumFrames );
		mReadPos = readCount;
	}

	return result;
}

void SourceFile::seek( size_t readPositionFrames )
{
	if( readPositionFrames >= mNumFrames )
		return;

	if( getSampleRate() == getSampleRateNative() ) {
		performSeek( readPositionFrames );
	}
	else {
		// adjust read pos for samplerate conversion so that it is relative to file num frames
		size_t fileReadPos = size_t( (float)readPositionFrames * (float)mFileNumFrames / (float)mNumFrames );
		performSeek( fileReadPos );
	}

	mReadPos = readPositionFrames;
}

} } // namespace cinder::audio
