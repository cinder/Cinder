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

#include "cinder/Cinder.h"
#if ( _WIN32_WINNT >= _WIN32_WINNT_VISTA )

#include "cinder/audio/msw/FileMediaFoundation.h"
#include "cinder/audio/dsp/Converter.h"
#include "cinder/audio/Exception.h"
#include "cinder/Log.h"

#include <mfidl.h>
#include <mfapi.h>
#include <mfreadwrite.h>
#include <propvarutil.h>

#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mfreadwrite.lib")

// TODO: try to minimize the number of copies between IMFSourceReader::ReadSample's IMFSample and loading audio::Buffer
// - currently uses an intermediate vector<float>
// - want to have minimal copies when loading a buffer for BufferPlayerNodeNode, so
// - this is on hold until audio::Converter is re-addressed

using namespace std;
using namespace ci;

namespace cinder { namespace audio { namespace msw {

bool MediaFoundationInitializer::sIsMfInitialized = false;

namespace {

inline double	nanoSecondsToSeconds( LONGLONG ns )		{ return (double)ns / 10000000.0; } 
inline LONGLONG secondsToNanoSeconds( double seconds )	{ return (LONGLONG)seconds * 10000000; }

::GUID getMfAudioFormat( SampleType sampleType )
{
	switch( sampleType ) {
		case SampleType::INT_16:	return ::MFAudioFormat_PCM;
		case SampleType::INT_24:	return ::MFAudioFormat_PCM;
		case SampleType::FLOAT_32:	return ::MFAudioFormat_Float;
		default: break;
	}

	CI_ASSERT_NOT_REACHABLE();
	return MFAudioFormat_PCM;
}

size_t getBytesPerSample( SampleType sampleType )
{
	switch( sampleType ) {
		case SampleType::INT_16:	return 2;
		case SampleType::INT_24:	return 3;
		case SampleType::FLOAT_32:	return 4;
		default: break;
	}

	CI_ASSERT_NOT_REACHABLE();
	return 0;
}

} // anonymous namespace

// ----------------------------------------------------------------------------------------------------
// SourceFileMediaFoundation
// ----------------------------------------------------------------------------------------------------

SourceFileMediaFoundation::SourceFileMediaFoundation()
	: SourceFile( 0 ), mCanSeek( false ), mSeconds( 0 ), mReadBufferPos( 0 ), mFramesRemainingInReadBuffer( 0 )
{
}

SourceFileMediaFoundation::SourceFileMediaFoundation( const DataSourceRef &dataSource, size_t sampleRate )
	: SourceFile( sampleRate ), mDataSource( dataSource ), mCanSeek( false ), mSeconds( 0 ), mReadBufferPos( 0 ), mFramesRemainingInReadBuffer( 0 )
{
	MediaFoundationInitializer::initMediaFoundation();
	initReader();
}

SourceFileRef SourceFileMediaFoundation::cloneWithSampleRate( size_t sampleRate ) const
{
	auto result = make_shared<SourceFileMediaFoundation>( mDataSource, sampleRate );
	result->initReader();
	result->setupSampleRateConversion();

	return result;
}

SourceFileMediaFoundation::~SourceFileMediaFoundation()
{
}

size_t SourceFileMediaFoundation::performRead( Buffer *buffer, size_t bufferFrameOffset, size_t numFramesNeeded )
{
	CI_ASSERT( buffer->getNumFrames() >= bufferFrameOffset + numFramesNeeded );

	size_t readCount = 0;
	while( readCount < numFramesNeeded ) {
		// first drain any frames that were previously read from an IMFSample
		if( mFramesRemainingInReadBuffer ) {
			size_t remainingToDrain = std::min( mFramesRemainingInReadBuffer, numFramesNeeded );
			size_t writeOffset = bufferFrameOffset + readCount;
			buffer->copyOffset( mReadBuffer, remainingToDrain, writeOffset, mReadBufferPos );

			mReadBufferPos += remainingToDrain;
			mFramesRemainingInReadBuffer -= remainingToDrain;
			readCount += remainingToDrain;
			continue; // check if we've filled the requested number of frames
		}

		CI_ASSERT( ! mFramesRemainingInReadBuffer );

		mReadBufferPos = 0;
		bool endOfFile;
		size_t outNumFrames = processNextReadSample( &endOfFile );
		if( ! outNumFrames ) {
			if( endOfFile ) {
				// Whoops, IMFSourceReader::ReadSample() must of reported EOF before we reached the expected number of frames. I've seen this happen with mp3s.
				// - so we'll just zero out the remaining frames, so the SourceFile receives the expected number of frames as reported when parsing the header.
				mReadBuffer.zero();
				outNumFrames = numFramesNeeded - readCount;
			}
			else {
				CI_LOG_W( "Could not read the expected number of samples: " << numFramesNeeded << ", readCount: " << readCount );
				break;
			}
		}
		else if( outNumFrames + readCount > numFramesNeeded ) {
			// if the IMFSample num frames is over the specified buffer size, 
			// record how many samples are left over and use up what was asked for.
			mFramesRemainingInReadBuffer = outNumFrames + readCount - numFramesNeeded;
			outNumFrames = numFramesNeeded - readCount;
		}

		size_t writeOffset = bufferFrameOffset + readCount;
		buffer->copyOffset( mReadBuffer, outNumFrames, writeOffset, 0 );

		mReadBufferPos += outNumFrames;
		readCount += outNumFrames;
	}

	return readCount;
}

void SourceFileMediaFoundation::performSeek( size_t readPositionFrames )
{
	if( ! mCanSeek )
		return;

	mReadBufferPos = mFramesRemainingInReadBuffer = 0;

	double positionSeconds = (double)readPositionFrames / (double)mSampleRate;
	if( positionSeconds > mSeconds ) {
		// don't attempt seek beyond bounds
		return;
	}

	LONGLONG position = secondsToNanoSeconds( positionSeconds );
	PROPVARIANT seekVar;
	HRESULT hr = ::InitPropVariantFromInt64( position, &seekVar );
	CI_ASSERT( hr == S_OK );
	hr = mSourceReader->SetCurrentPosition( GUID_NULL, seekVar );
	CI_ASSERT( hr == S_OK );
	hr = PropVariantClear( &seekVar );
	CI_ASSERT( hr == S_OK );
}

// TODO: test setting MF_LOW_LATENCY attribute
void SourceFileMediaFoundation::initReader()
{
	CI_ASSERT( mDataSource );
	mFramesRemainingInReadBuffer = 0;

	::IMFAttributes *attributes;
	HRESULT hr = ::MFCreateAttributes( &attributes, 1 );
	CI_ASSERT( hr == S_OK );
	auto attributesPtr = ci::msw::makeComUnique( attributes );

	::IMFSourceReader *sourceReader;

	if( mDataSource->isFilePath() ) {
		hr = ::MFCreateSourceReaderFromURL( mDataSource->getFilePath().wstring().c_str(), attributesPtr.get(), &sourceReader );
		if( hr != S_OK ) {
			string errorString = string( "SourceFileMediaFoundation: Failed to create SourceReader from URL: " ) +  mDataSource->getFilePath().string(); 
			if( hr == HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ) )
				errorString += ", file not found.";
			throw AudioFileExc( errorString );
		}
	}
	else {
		mComIStream = ci::msw::makeComUnique( new ci::msw::ComIStream( mDataSource->createStream() ) );
		::IMFByteStream *byteStream;
		hr = ::MFCreateMFByteStreamOnStream( mComIStream.get(), &byteStream );
		CI_ASSERT( hr == S_OK );
		mByteStream = ci::msw::makeComUnique( byteStream );

		hr = ::MFCreateSourceReaderFromByteStream( byteStream, attributesPtr.get(), &sourceReader );
		if( hr != S_OK )
			throw AudioFileExc( "SourceFileMediaFoundation: Failed to create SourceReader from resource." );
	}

	mSourceReader = ci::msw::makeComUnique( sourceReader );

	// get files native format
	::IMFMediaType *nativeType;
	::WAVEFORMATEX *nativeFormat;
	UINT32 formatSize;
	hr = mSourceReader->GetNativeMediaType( MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &nativeType );
	CI_ASSERT( hr == S_OK );
	auto nativeTypeUnique = ci::msw::makeComUnique( nativeType );

	hr = ::MFCreateWaveFormatExFromMFMediaType( nativeType, &nativeFormat, &formatSize );
	CI_ASSERT( hr == S_OK );

	mNumChannels = nativeFormat->nChannels;
	mSampleRate = nativeFormat->nSamplesPerSec;

	GUID outputSubType = MFAudioFormat_PCM; // default to PCM 16-bit int, upgrade if we can.
	mSampleType = SampleType::INT_16;

	if( nativeFormat->wBitsPerSample == 32 ) {
		mSampleType = SampleType::FLOAT_32;
		outputSubType = MFAudioFormat_Float;
	}
	else if( nativeFormat->wBitsPerSample == 24 ) {
		mSampleType = SampleType::INT_24;
		if( mNumChannels > 1 )
			mBitConverterBuffer.setSize( getMaxFramesPerRead(), mNumChannels );
	}

	::CoTaskMemFree( nativeFormat );

	mBytesPerSample = getBytesPerSample( mSampleType );
	mReadBuffer.setSize( getMaxFramesPerRead(), mNumChannels );

	// set output type, which loads the proper decoder:
	::IMFMediaType *outputType;
	hr = ::MFCreateMediaType( &outputType );
	auto outputTypeRef = ci::msw::makeComUnique( outputType );
	hr = outputTypeRef->SetGUID( MF_MT_MAJOR_TYPE, MFMediaType_Audio );
	CI_ASSERT( hr == S_OK );
	hr = outputTypeRef->SetGUID( MF_MT_SUBTYPE, outputSubType );
	CI_ASSERT( hr == S_OK );

	hr = mSourceReader->SetCurrentMediaType( MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, outputTypeRef.get() );
	CI_ASSERT( hr == S_OK );

	// after the decoder is loaded, we have to now get the 'complete' output type before retrieving its format
	::IMFMediaType *completeOutputType;
	hr = mSourceReader->GetCurrentMediaType( MF_SOURCE_READER_FIRST_AUDIO_STREAM, &completeOutputType );
	CI_ASSERT( hr == S_OK );

	::WAVEFORMATEX *format;
	hr = ::MFCreateWaveFormatExFromMFMediaType( completeOutputType, &format, &formatSize );
	CI_ASSERT( hr == S_OK );
	::CoTaskMemFree( format );

	// get seconds:
	::PROPVARIANT durationProp;
	hr = mSourceReader->GetPresentationAttribute( MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &durationProp );
	CI_ASSERT( hr == S_OK );
	LONGLONG duration = durationProp.uhVal.QuadPart;
	
	mSeconds = nanoSecondsToSeconds( duration );
	mNumFrames = mFileNumFrames = size_t( mSeconds * (double)mSampleRate );

	::PROPVARIANT seekProp;
	hr = mSourceReader->GetPresentationAttribute( MF_SOURCE_READER_MEDIASOURCE, MF_SOURCE_READER_MEDIASOURCE_CHARACTERISTICS, &seekProp );
	CI_ASSERT( hr == S_OK );
	ULONG flags = seekProp.ulVal;
	mCanSeek = ( ( flags & MFMEDIASOURCE_CAN_SEEK ) == MFMEDIASOURCE_CAN_SEEK );
}

size_t SourceFileMediaFoundation::processNextReadSample( bool *endOfFile )
{
	::IMFSample *mediaSample;
	DWORD streamFlags = 0;
	LONGLONG timeStamp;
	*endOfFile = false;
	HRESULT hr = mSourceReader->ReadSample( MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, NULL, &streamFlags, &timeStamp, &mediaSample );
	CI_ASSERT( hr == S_OK );

	if( streamFlags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED ) {
		CI_LOG_W( "type change unhandled" );
		return 0;
	}
	if( streamFlags & MF_SOURCE_READERF_ENDOFSTREAM ) {
		*endOfFile = true;
		return 0;
	}
	if( ! mediaSample ) {
		// out of samples
		mediaSample->Release();
		return 0;
	}

	auto samplePtr = ci::msw::makeComUnique( mediaSample );

	DWORD bufferCount;
	hr = samplePtr->GetBufferCount( &bufferCount );
	CI_ASSERT( hr == S_OK );

	CI_ASSERT( bufferCount == 1 ); // just looking out for a file type with more than one buffer.. haven't seen one yet.

	// get the buffer
	::IMFMediaBuffer *mediaBuffer;
	BYTE *audioData = NULL;
	DWORD audioDataLength;

	hr = samplePtr->ConvertToContiguousBuffer( &mediaBuffer );
	hr = mediaBuffer->Lock( &audioData, NULL, &audioDataLength );

	size_t numChannels = mNumChannels;
	size_t numFramesRead = audioDataLength / ( mBytesPerSample * numChannels );

	mReadBuffer.setNumFrames( numFramesRead );

	if( mSampleType == SampleType::FLOAT_32 ) {
		float *sourceFloatSamples = (float *)audioData;
		if( numChannels == 1 )
			memcpy( mReadBuffer.getData(), sourceFloatSamples, numFramesRead * sizeof( float ) );
		else
			dsp::deinterleave( sourceFloatSamples, mReadBuffer.getData(), mReadBuffer.getNumFrames(), numChannels, numFramesRead );
	}
	else if( mSampleType == SampleType::INT_16 ) {
		int16_t *sourceInt16Samples = (int16_t *)audioData;
		dsp::deinterleave( sourceInt16Samples, mReadBuffer.getData(), mReadBuffer.getNumFrames(), numChannels, numFramesRead );
	}
	else if( mSampleType == SampleType::INT_24 ) {
		const char *sourceInt24Samples = (const char *)audioData;
		if( numChannels == 1 )
			dsp::convertInt24ToFloat( sourceInt24Samples, mReadBuffer.getData(), numFramesRead );
		else {
			if( mBitConverterBuffer.getNumFrames() != numFramesRead )
				mBitConverterBuffer.setNumFrames( numFramesRead );

			dsp::convertInt24ToFloat( sourceInt24Samples, mBitConverterBuffer.getData(), numFramesRead * numChannels );
			dsp::deinterleave( mBitConverterBuffer.getData(), mReadBuffer.getData(), mReadBuffer.getNumFrames(), numChannels, numFramesRead );
		}
	}
	else
		CI_ASSERT_NOT_REACHABLE();

	hr = mediaBuffer->Unlock();
	CI_ASSERT( hr == S_OK );

	mediaBuffer->Release();
	return numFramesRead;
}

// From what I can tell, there is no straightforward way to ask Media Foundation for a list of extensions that relate to the decoders it supports
// So instead we create the list by hand, with microsoft's supported audio formats webpage as reference: http://msdn.microsoft.com/en-us/library/windows/desktop/dd757927(v=vs.85).aspx 
// To note, I tried MFTEnumEx and was able to get a list of GUID's for codecs, but not extensions. Also, it appears this method is unavailable in Windows Runtime.
// static
vector<std::string> SourceFileMediaFoundation::getSupportedExtensions()
{
	vector<std::string> result;
	result.push_back( "3g2" );
	result.push_back( "3gp2" );
	result.push_back( "3gp" );
	result.push_back( "3gpp" );
	result.push_back( "m4a" );
	result.push_back( "m4v" );
	result.push_back( "mp4v" );
	result.push_back( "mp4" );
	result.push_back( "m2ts" );
	result.push_back( "asf" );
	result.push_back( "wm" );
	result.push_back( "wmv" );
	result.push_back( "wma" );
	result.push_back( "aac" );
	result.push_back( "adt" );
	result.push_back( "adts" );
	result.push_back( "mp3" );
	result.push_back( "wav" );
	result.push_back( "avi" );
	result.push_back( "ac3" );
	result.push_back( "ec3" );
	return result;
}

// ----------------------------------------------------------------------------------------------------
// TargetFileMediaFoundation
// ----------------------------------------------------------------------------------------------------

TargetFileMediaFoundation::TargetFileMediaFoundation( const DataTargetRef &dataTarget, size_t sampleRate, size_t numChannels, SampleType sampleType, const std::string &extension )
	: TargetFile( sampleRate, numChannels, sampleType ), mStreamIndex( 0 )
{
	MediaFoundationInitializer::initMediaFoundation();

	::IMFSinkWriter *sinkWriter;
	HRESULT hr = ::MFCreateSinkWriterFromURL( dataTarget->getFilePath().wstring().c_str(), NULL, NULL, &sinkWriter );
	if( hr != S_OK ) {
		string errorString = string( "TargetFileMediaFoundation: Failed to create SinkWriter from URL: " ) +  dataTarget->getFilePath().string(); 
		if( hr == HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ) )
			errorString += ", file not found.";
		throw AudioFileExc( errorString );
	}

	mSinkWriter = ci::msw::makeComUnique( sinkWriter );

	mSampleSize = getBytesPerSample( mSampleType );
	const auto bitsPerSample = 8 * mSampleSize;
	const auto blockAlignment = mNumChannels * mSampleSize;
	const auto averageBytesPerSecond = mSampleRate * blockAlignment;

	// Set the output media type.

	IMFMediaType *mediaType;
	hr = ::MFCreateMediaType( &mediaType );
	CI_ASSERT( hr == S_OK );
	auto mediaTypeManaged = ci::msw::makeComUnique( mediaType );

	hr = mediaType->SetGUID( MF_MT_MAJOR_TYPE, MFMediaType_Audio );
	CI_ASSERT( hr == S_OK );

	const ::GUID audioFormat = getMfAudioFormat( mSampleType );
	hr = mediaType->SetGUID( MF_MT_SUBTYPE, audioFormat );
	CI_ASSERT( hr == S_OK );

	hr = mediaType->SetUINT32( MF_MT_AUDIO_SAMPLES_PER_SECOND, static_cast<UINT32>( mSampleRate ) );
	CI_ASSERT( hr == S_OK );

	hr = mediaType->SetUINT32( MF_MT_AUDIO_BITS_PER_SAMPLE, static_cast<UINT32>( bitsPerSample ) );
	CI_ASSERT( hr == S_OK );

	hr = mediaType->SetUINT32( MF_MT_AUDIO_BLOCK_ALIGNMENT, static_cast<UINT32>( blockAlignment ) );
	CI_ASSERT( hr == S_OK );

	hr = mediaType->SetUINT32( MF_MT_AUDIO_AVG_BYTES_PER_SECOND, static_cast<UINT32>( averageBytesPerSecond ) );
	CI_ASSERT( hr == S_OK );

	hr = mediaType->SetUINT32( MF_MT_AUDIO_NUM_CHANNELS, static_cast<UINT32>( mNumChannels ) );
	CI_ASSERT( hr == S_OK );

	hr = mediaType->SetUINT32( MF_MT_ALL_SAMPLES_INDEPENDENT, 1 );
	CI_ASSERT( hr == S_OK );

	hr = mediaType->SetUINT32( MF_MT_FIXED_SIZE_SAMPLES, 1 );
	CI_ASSERT( hr == S_OK );

	hr = mSinkWriter->AddStream( mediaType, &mStreamIndex );
	CI_ASSERT( hr == S_OK );

	// Tell the sink writer to start accepting data.
	hr = mSinkWriter->BeginWriting();
	CI_ASSERT( hr == S_OK );
}

TargetFileMediaFoundation::~TargetFileMediaFoundation()
{
	if( mSinkWriter && mSamplesWritten ) {
		HRESULT hr = mSinkWriter->Finalize();
		CI_VERIFY( hr == S_OK );
	}
}

void TargetFileMediaFoundation::performWrite( const Buffer *buffer, size_t numFrames, size_t frameOffset )
{
	// create media sample
	::IMFSample *mediaSample;
	HRESULT hr = ::MFCreateSample( &mediaSample );
	CI_ASSERT( hr == S_OK );

	auto samplePtr = ci::msw::makeComUnique( mediaSample );

	double lengthSeconds = (double)numFrames / (double)mSampleRate;
	const LONGLONG sampleDuration = secondsToNanoSeconds( lengthSeconds );
	hr = mediaSample->SetSampleDuration( sampleDuration );
	CI_ASSERT( hr == S_OK );

	double currentTimeSeconds = (double)frameOffset / (double)mSampleRate;
	const LONGLONG sampleTime = secondsToNanoSeconds( currentTimeSeconds );
	hr = mediaSample->SetSampleTime( sampleTime );
	CI_ASSERT( hr == S_OK );

	// create media buffer and fill with audio samples.

	const DWORD bufferSizeBytes = static_cast<DWORD>( numFrames * buffer->getNumChannels() * mSampleSize );
	::IMFMediaBuffer *mediaBuffer;
	hr = ::MFCreateMemoryBuffer( bufferSizeBytes, &mediaBuffer );
	CI_ASSERT( hr == S_OK );

	hr = mediaBuffer->SetCurrentLength( bufferSizeBytes );
	CI_ASSERT( hr == S_OK );

	hr = mediaSample->AddBuffer( mediaBuffer );
	CI_ASSERT( hr == S_OK );

	BYTE *audioData;
	hr = mediaBuffer->Lock( &audioData, NULL, NULL );
	CI_ASSERT( hr == S_OK );

	if( mSampleType == SampleType::FLOAT_32 ) {
		float *destFloatSamples = (float *)audioData;
		if( mNumChannels == 1 )
			memcpy( destFloatSamples, buffer->getData(), numFrames * mSampleSize );
		else
			dsp::interleave( buffer->getData(), destFloatSamples, buffer->getNumFrames(), mNumChannels, numFrames );
	}
	else if( mSampleType == SampleType::INT_16 ) {
		int16_t *destInt16Samples = (int16_t *)audioData;
		dsp::interleave( buffer->getData(), destInt16Samples, buffer->getNumFrames(), mNumChannels, numFrames );
	}
	else if( mSampleType == SampleType::INT_24 ) {
		char *destInt24Samples = (char *)audioData;
		if( mNumChannels == 1 )
			dsp::convertFloatToInt24( buffer->getData(), destInt24Samples, numFrames );
		else {
			if( mBitConverterBuffer.getNumFrames() != numFrames || mBitConverterBuffer.getNumChannels() != mNumChannels  )
				mBitConverterBuffer.setSize( numFrames, mNumChannels );

			dsp::interleaveBuffer( buffer, &mBitConverterBuffer );
			dsp::convertFloatToInt24( mBitConverterBuffer.getData(), destInt24Samples, numFrames * mNumChannels );
		}
	}

	else
		CI_ASSERT_NOT_REACHABLE();

	hr = mediaBuffer->Unlock();
	CI_ASSERT( hr == S_OK );

	hr = mSinkWriter->WriteSample( mStreamIndex, mediaSample );
	CI_ASSERT( hr == S_OK );

	mSamplesWritten = true;
}

// ----------------------------------------------------------------------------------------------------
// MediaFoundationImpl (Startup / Shutdown)
// ----------------------------------------------------------------------------------------------------

// static
void MediaFoundationInitializer::initMediaFoundation()
{
	if( ! sIsMfInitialized ) {
		sIsMfInitialized = true;
		ci::msw::initializeCom();
		HRESULT hr = ::MFStartup( MF_VERSION );
		CI_ASSERT( hr == S_OK );
	}
}

// static
void MediaFoundationInitializer::shutdownMediaFoundation()
{
	if( sIsMfInitialized ) {
		sIsMfInitialized = false;
		HRESULT hr = ::MFShutdown();
		CI_ASSERT( hr == S_OK );
	}
}

} } } // namespace cinder::audio::msw

#endif // ( _WIN32_WINNT >= 0x0600 )