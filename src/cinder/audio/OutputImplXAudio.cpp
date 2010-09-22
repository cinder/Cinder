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

#include "cinder/audio/OutputImplXAudio.h"

namespace cinder { namespace audio {

//TODO: don't make these values fixed
TargetOutputImplXAudio::TargetOutputImplXAudio( const WAVEFORMATEX *aOutDescription )
	: Target()
{
	mSampleRate = aOutDescription->nSamplesPerSec;
	mChannelCount = aOutDescription->nChannels;
	mBitsPerSample = aOutDescription->wBitsPerSample;
	mBlockAlign = aOutDescription->nBlockAlign;
	mIsInterleaved = TRUE;
	mIsPcm = TRUE;
	mIsBigEndian = TRUE;
	mDataType = DATA_UNKNOWN;
	if( aOutDescription->wFormatTag == WAVE_FORMAT_IEEE_FLOAT ) {
		mDataType = FLOAT32;
	} else if( aOutDescription->wBitsPerSample == 32 ) {
		mDataType = INT32;
	} else if( aOutDescription->wBitsPerSample == 16 ) {
		mDataType = INT16;
	} else if( aOutDescription->wBitsPerSample == 8 ) {
		mDataType = UINT8;
	}
}

OutputImplXAudio::Track::Track( SourceRef source, OutputImplXAudio * output )
	: cinder::audio::Track(), mSource( source ), mOutput( output ), mIsPcmBuffering( false )
{
	::HRESULT hr;

	mBufferEndEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	mTrackId = mOutput->availableTrackId();
	mVoiceCallback.mTrack = this;
	
	if( source->getDataType() == Io::DATA_UNKNOWN ) {
		// for compressed data setup Target/SourceVoice for ideal setup for XAudio output
		XAUDIO2_VOICE_DETAILS masterVoiceDetails;
		mOutput->mMasterVoice->GetVoiceDetails( &masterVoiceDetails );

		mVoiceDescription.wFormatTag = WAVE_FORMAT_PCM;//WAVE_FORMAT_IEEE_FLOAT;
		mVoiceDescription.nSamplesPerSec = masterVoiceDetails.InputSampleRate;
		mVoiceDescription.nChannels = masterVoiceDetails.InputChannels;
		mVoiceDescription.wBitsPerSample = 16;
		mVoiceDescription.nBlockAlign = math<float>::ceil( ( mVoiceDescription.nChannels * mVoiceDescription.wBitsPerSample ) / 8.0 );
		mVoiceDescription.nAvgBytesPerSec = ( mVoiceDescription.nBlockAlign * mVoiceDescription.nSamplesPerSec );
		mVoiceDescription.cbSize = 0;
	} else {
		mVoiceDescription.wFormatTag = ( source->isFloat() )? WAVE_FORMAT_IEEE_FLOAT : WAVE_FORMAT_PCM;
		mVoiceDescription.nSamplesPerSec = source->getSampleRate();
		mVoiceDescription.nChannels = source->getChannelCount();
		mVoiceDescription.wBitsPerSample = source->getBitsPerSample();
		mVoiceDescription.nBlockAlign = source->getBlockAlign();//( sourceDescription.nChannels * sourceDescription.wBitsPerSample ) / 8;
		mVoiceDescription.nAvgBytesPerSec = ( mVoiceDescription.nBlockAlign * mVoiceDescription.nSamplesPerSec );
		mVoiceDescription.cbSize = 0;
	}

	if( mVoiceDescription.nChannels > XAUDIO2_MAX_AUDIO_CHANNELS ) {
		//throw
	}

	if( mVoiceDescription.nSamplesPerSec < XAUDIO2_MIN_SAMPLE_RATE || mVoiceDescription.nSamplesPerSec > XAUDIO2_MAX_SAMPLE_RATE ) {
		//throw
	}
	
	shared_ptr<TargetOutputImplXAudio> target = TargetOutputImplXAudio::createRef( &mVoiceDescription );
	mLoader = mSource->createLoader( target.get() );

	//mBufferSize = mLoader->mBufferSize;
	mBufferSize = mLoader->getOptimalBufferSize();
	if( mBufferSize <= 0 ) {
		mBufferSize = 2048 * mVoiceDescription.nBlockAlign;
	}
	mSamplesPerBuffer = mBufferSize / mVoiceDescription.nBlockAlign;

	//create buffers
	mDecodedBuffers = new uint8_t[ OutputImplXAudio::Track::sMaxBufferCount * mBufferSize ];
	mCurrentBuffer = 0;
	mCurrentTime = 0;

	hr = mOutput->mXAudio->CreateSourceVoice( &mSourceVoice, &mVoiceDescription, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &(mVoiceCallback), NULL, NULL );
	if( FAILED( hr ) )
	{
		//throw
		return;
	}
}

OutputImplXAudio::Track::~Track()
{
	stop();
	delete [] mDecodedBuffers;
	CloseHandle( mBufferEndEvent );
}

void OutputImplXAudio::Track::play()
{
	//mLoader->start();
	//fillBufferCallback();
	mIsPlaying = true;
	mQueueThread = shared_ptr<boost::thread>( new boost::thread( boost::bind( &OutputImplXAudio::Track::fillBuffer, this ) ) );

	::HRESULT hr = mSourceVoice->Start( 0, XAUDIO2_COMMIT_NOW );
	if( FAILED( hr ) ) {
		//throw
		return;
	}
}

void OutputImplXAudio::Track::stop()
{
	if( ! mIsPlaying ) return;
	mSourceVoice->Stop( 0, XAUDIO2_COMMIT_NOW ); //might not really need this
	mIsPlaying = false;
	SetEvent( mBufferEndEvent ); //signals event to end queuethread
	mQueueThread->join();
	mSourceVoice->FlushSourceBuffers();
}

void OutputImplXAudio::Track::setVolume( float aVolume )
{
	aVolume = math<float>::clamp( aVolume, 0.0, 1.0 );
	::HRESULT hr = mSourceVoice->SetVolume( aVolume );
	if( FAILED( hr ) ) {
		//throw
	}
}

float OutputImplXAudio::Track::getVolume() const
{
	float aValue = 0.0;
	mSourceVoice->GetVolume( &aValue );
	return aValue;
}

PcmBuffer32fRef OutputImplXAudio::Track::getPcmBuffer()
{
	boost::mutex::scoped_lock( mPcmBufferMutex );
	return mLoadedPcmBuffer;
}

//HRESULT OutputImplXAudio::Track::dataInputCallback( void * audioData, uint32_t dataSize, void * track, uint64_t sampleTime, uint32_t sampleDuration )
void OutputImplXAudio::Track::fillBuffer()
{
	//OutputImplXAudio::Track *theTrack = reinterpret_cast<OutputImplXAudio::Track*>( track );
	XAUDIO2_VOICE_STATE state;
	while( 1 ) {	
		mSourceVoice->GetState( &state );
		if( state.BuffersQueued >= OutputImplXAudio::Track::sMaxBufferCount ) {
			::WaitForSingleObject( mBufferEndEvent, INFINITE );
		}

		if( ! mIsPlaying ) {
			//tell the loader to go fuck itself?
			break;
		}
		
		BufferList bufferList;
		bufferList.mNumberBuffers = 1;
		BufferGeneric buffer;
		buffer.mData = &( mDecodedBuffers[mCurrentBuffer * mBufferSize] );
		buffer.mDataByteSize = mBufferSize;
		buffer.mNumberChannels = mVoiceDescription.nChannels;
		buffer.mSampleCount = mSamplesPerBuffer;
		bufferList.mBuffers = &buffer;
		
		mLoader->loadData( &bufferList );
		
		if( bufferList.mBuffers[0].mSampleCount <= 0 ) {
			stop();
			return;
		}
		
		XAUDIO2_BUFFER xbuffer = {0};
		xbuffer.AudioBytes = buffer.mDataByteSize;
		xbuffer.Flags = 0;
		xbuffer.pAudioData = (BYTE *)buffer.mData;
		::HRESULT hr = mSourceVoice->SubmitSourceBuffer( &xbuffer );
		if( hr != S_OK ) {

		}

		if( mIsPcmBuffering ) {
			uint32_t sampleCount = buffer.mDataByteSize / mVoiceDescription.nBlockAlign;
			if( ! mLoadingPcmBuffer || ( mLoadingPcmBuffer->getSampleCount() + sampleCount > mLoadingPcmBuffer->getMaxSampleCount() ) ) {
				boost::mutex::scoped_lock lock( mPcmBufferMutex );
				//TODO: make this settable, 1470 ~= 44100(samples/sec)/30(frmaes/second), also make sure the buffer isn't going to be larger than this? perhaps wrap data across buffers?
				uint32_t bufferSampleCount = 2500;
				if( mLoadingPcmBuffer ) {
					mLoadedPcmBuffer = PcmBuffer32fRef();
					mLoadedPcmBuffer = mLoadingPcmBuffer;
				}
				mLoadingPcmBuffer = PcmBuffer32fRef( new PcmBuffer32f( bufferSampleCount, mVoiceDescription.nChannels, true ) );
			}

			//TODO: only do this if Voice is not Float and make this more efficient
			//TODO: right now this only supports uint16_t
			float * copyBuffer = new float[sampleCount * buffer.mNumberChannels];
			int16_t * srcBuffer = reinterpret_cast<int16_t *>( buffer.mData );
			for( uint32_t i = 0; i < ( sampleCount * buffer.mNumberChannels ); i++ ) {
				//TODO: abstract this conversion
				copyBuffer[i] = ( ( srcBuffer[i] / 32767.0f ) + 1.0f ) * 0.5f;
			}
			mLoadingPcmBuffer->appendInterleavedData( copyBuffer, sampleCount );
			delete [] copyBuffer;
		}
		
		mCurrentBuffer++;
		mCurrentBuffer %= OutputImplXAudio::Track::sMaxBufferCount;
	}
}

/*class TargetOutputImplXAudio : public Target
{
  public:
  protected:
	TargetOutputImplXAudio
}*/

OutputImplXAudio::OutputImplXAudio()
	: OutputImpl(), mXAudio( NULL ), mMasterVoice( NULL )
{
	::HRESULT hr;

	msw::initializeCom();
	
	if( FAILED( hr = XAudio2Create( &mXAudio, 0, XAUDIO2_DEFAULT_PROCESSOR ) ) ) {
		//throw
		return;
	}
	
	if( FAILED( hr = mXAudio->CreateMasteringVoice( &mMasterVoice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, NULL ) ) ) {
		//throw
		return;
	}
}

OutputImplXAudio::~OutputImplXAudio()
{
	//TODO: cleanup
}

TrackRef OutputImplXAudio::addTrack( SourceRef aSource, bool autoPlay ) 
{
	//TargetOutputImplXAudioRef target = TargetOutputImplXAudio::createRef( this );
	//TODO: pass target instead of this?
	shared_ptr<OutputImplXAudio::Track> track = shared_ptr<OutputImplXAudio::Track>( new OutputImplXAudio::Track( aSource, this ) );
	TrackId inputBus = track->getTrackId();
	mTracks.insert( std::pair<TrackId,shared_ptr<OutputImplXAudio::Track> >( inputBus, track ) );
	if( autoPlay ) {
		track->play();
	}
	return track;
}

void OutputImplXAudio::removeTrack( TrackId trackId )
{
	if( mTracks.find( trackId ) == mTracks.end() ) {
		//TODO: throw OutputInvalidTrackExc();
		return;
	}
	
	mTracks.erase( trackId );
}

/*TargetRef OutputImplXAudio::getTarget()
{
	return TargetOutputImplXAudio::createRef( mMasterVoice );
}*/

}} //namespaces