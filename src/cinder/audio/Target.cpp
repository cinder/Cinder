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
#include "cinder/CinderAssert.h"
#include "cinder/audio/FileOggVorbis.h"

#include "cinder/Utilities.h"

#if defined( CINDER_COCOA )
	#include "cinder/audio/cocoa/FileCoreAudio.h"
#elif defined( CINDER_MSW )
	#include "cinder/audio/msw/FileMediaFoundation.h"
#endif

using namespace std;

namespace cinder { namespace audio {

// TODO: these should be replaced with a generic registrar derived from the ImageIo stuff.

std::unique_ptr<TargetFile> TargetFile::create( const DataTargetRef &dataTarget, size_t sampleRate, size_t numChannels, SampleType sampleType, std::string ext )
{
	if( ext.empty() ) {
#if ! defined( CINDER_UWP ) || ( _MSC_VER > 1800 )
		ext = dataTarget->getFilePathHint().extension().string();
#else
		ext = dataTarget->getFilePathHint().extension();
#endif
		if ( ! ext.empty() && ( ext[0] == '.' ) ) {
			ext.erase( ext.begin() );
		}
	}

	if( ext == "ogg" ) {
		return std::unique_ptr<TargetFile>( new TargetFileOggVorbis( dataTarget, sampleRate, numChannels, sampleType ) );
	}

#if defined( CINDER_COCOA )
	return std::unique_ptr<TargetFile>( new cocoa::TargetFileCoreAudio( dataTarget, sampleRate, numChannels, sampleType, ext ) );
#elif defined( CINDER_MSW )
	return std::unique_ptr<TargetFile>( new msw::TargetFileMediaFoundation( dataTarget, sampleRate, numChannels, sampleType, ext ) );
#else
	return nullptr;
#endif
}

std::unique_ptr<TargetFile> TargetFile::create( const fs::path &path, size_t sampleRate, size_t numChannels, SampleType sampleType, std::string extension )
{
	return create( writeFile( path ), sampleRate, numChannels, sampleType, extension );
}

void TargetFile::write( const Buffer *buffer )
{
	performWrite( buffer, buffer->getNumFrames(), 0 );
}

void TargetFile::write( const Buffer *buffer, size_t numFrames )
{
	if( ! numFrames )
		return;

	CI_ASSERT_MSG( numFrames <= buffer->getNumFrames(), "numFrames out of bounds" );

	performWrite( buffer, numFrames, 0 );
}

void TargetFile::write( const Buffer *buffer, size_t numFrames, size_t frameOffset )
{
	if( ! numFrames )
		return;

	CI_ASSERT_MSG( numFrames + frameOffset <= buffer->getNumFrames(), "numFrames + frameOffset out of bounds" );

	performWrite( buffer, numFrames, frameOffset );
}

} } // namespace cinder::audio
