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

#include "cinder/audio/Target.h"
#include "cinder/audio/dsp/Converter.h"
#include "cinder/CinderAssert.h"

#include "cinder/Utilities.h"

#if defined( CINDER_COCOA )
	#include "cinder/audio/cocoa/FileCoreAudio.h"
#elif defined( CINDER_MSW )
	#include "cinder/audio/msw/FileMediaFoundation.h"
#endif

using namespace std;

namespace cinder { namespace audio {

// TODO: these should be replaced with a generic registrar derived from the ImageIo stuff.

std::unique_ptr<TargetFile> TargetFile::create( const DataTargetRef &dataTarget, size_t sampleRate, size_t numChannels, SampleType sampleType, size_t sampleRateTarget, const std::string &extension )
{
#if ! defined( CINDER_UWP ) || ( _MSC_VER > 1800 )
	std::string ext = dataTarget->getFilePathHint().extension().string();
#else
	std::string ext = dataTarget->getFilePathHint().extension();
#endif
	ext = ( ( ! ext.empty() ) && ( ext[0] == '.' ) ) ? ext.substr( 1, string::npos ) : ext;

#if defined( CINDER_COCOA )
	return std::unique_ptr<TargetFile>( new cocoa::TargetFileCoreAudio( dataTarget, sampleRate, numChannels, sampleType, sampleRateTarget, ext ) );
#elif defined( CINDER_MSW )
	CI_ASSERT_MSG( sampleRateTarget == 0 || sampleRateTarget == sampleRate, "sample rate conversion not yet implemented on MSW" );
	return std::unique_ptr<TargetFile>( new msw::TargetFileMediaFoundation( dataTarget, sampleRate, numChannels, sampleType, ext ) );
#endif
}

std::unique_ptr<TargetFile> TargetFile::create( const fs::path &path, size_t sampleRate, size_t numChannels, SampleType sampleType, size_t targetSampleRate, const std::string &extension )
{
	return create( (DataTargetRef)writeFile( path ), sampleRate, numChannels, sampleType, targetSampleRate, extension );
}

TargetFile::TargetFile( size_t sampleRate, size_t numChannels, SampleType sampleType, size_t sampleRateTarget )
	: mSampleRate( sampleRate ), mNumChannels( numChannels ), mSampleType( sampleType ), mSampleRateTarget( sampleRateTarget ), mMaxFramesPerConversion( 4092 )
{
	setupSampleRateConversion();
}

TargetFile::~TargetFile() = default;

void TargetFile::setupSampleRateConversion()
{
	if ( ! mSampleRateTarget ) {
		mSampleRateTarget = mSampleRate;
	} else if ( mSampleRateTarget != mSampleRate) {
		mConverter = audio::dsp::Converter::create( mSampleRate, mSampleRateTarget, getNumChannels(), getNumChannels(), mMaxFramesPerConversion );
		mConverterSourceBuffer.setSize( mMaxFramesPerConversion, getNumChannels() );
		mConverterDestBuffer.setSize( mMaxFramesPerConversion, getNumChannels() );
	}
}

void TargetFile::write( const Buffer *buffer )
{
	write( buffer, buffer->getNumFrames(), 0 );
}

void TargetFile::write( const Buffer *buffer, size_t numFrames )
{
	if( ! numFrames )
		return;

	CI_ASSERT_MSG( numFrames <= buffer->getNumFrames(), "numFrames out of bounds" );

	write( buffer, numFrames, 0 );
}

void TargetFile::write( const Buffer *buffer, size_t numFrames, size_t frameOffset )
{
	if( ! numFrames )
		return;

	CI_ASSERT_MSG( numFrames + frameOffset <= buffer->getNumFrames(), "numFrames + frameOffset out of bounds" );

	if( mConverter ) {
		auto currFrame = frameOffset;
		auto lastFrame = frameOffset + numFrames;

		// process buffer in chunks of mMaxFramesPerConversion
		while ( currFrame != lastFrame ) {
			auto numSourceFrames = std::min( mMaxFramesPerConversion, lastFrame - currFrame );
			auto numDestFrames = size_t( numSourceFrames * (float)getSampleRateTarget() / (float)getSampleRate() );

			// copy buffer into temporary buffer to remove frame offset (needed for mConverter->convert)
			mConverterSourceBuffer.copyOffset( *buffer, numSourceFrames, 0, currFrame );

			mConverterSourceBuffer.setNumFrames( numSourceFrames );
			mConverterDestBuffer.setNumFrames( numDestFrames );
			tie( numSourceFrames, numDestFrames ) = mConverter->convert( &mConverterSourceBuffer, &mConverterDestBuffer );

			performWrite( &mConverterDestBuffer, numDestFrames, 0 );

			currFrame += numSourceFrames;
		}
	} else {
		performWrite( buffer, numFrames, frameOffset );
	}
}

} } // namespace cinder::audio
