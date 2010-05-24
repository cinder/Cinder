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

namespace cinder { namespace audio {

//TODO: don't make these values fixed
TargetOutputXAudio::TargetOutputXAudio( const WAVEFORMATEX *aOutDescription )
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

OutputXAudio::Track::Track( SourceRef source, OutputXAudio * output )
	: cinder::audio::Track(), mSource( source ), mOutput( output )
{
	::HRESULT hr;

	mBufferEndEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	mTrackId = mOutput->availableTrackId();
	mVoiceCallback.mTrack = this;
	
	if( source->getDataType() == Io::DATA_UNKNOWN ) {
		// for compressed data setup Target/SourceVoice for ideal setup for XAudio output
		XAUDIO2_VOICE_DETAILS masterVoiceDetails;
		mOutput->mMasterVoice->GetVoiceDetails( &masterVoiceDetails );

		mVoiceDescription.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;//WAVE_FORMAT_PCM;
		mVoiceDescription.nSamplesPerSec = masterVoiceDetails.InputSampleRate;
		mVoiceDescription.nChannels = masterVoiceDetails.InputChannels;
		mVoiceDescription.wBitsPerSample = 32;
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
	
	shared_ptr<TargetOutputXAudio> target = TargetOutputXAudio::createRef( &mVoiceDescription );
	mLoader = mSource->getLoader( target.get() );

	//mBufferSize = mLoader->mBufferSize;
	mBufferSize = mLoader->getOptimalBufferSize();
	if( mBufferSize <= 0 ) {
		mBufferSize = 2048 * mVoiceDescription.nBlockAlign;
	}
	mSamplesPerBuffer = mBufferSize / mVoiceDescription.nBlockAlign;

	//create buffers
	mDecodedBuffers = new uint8_t[ OutputXAudio::Track::sMaxBufferCount * mBufferSize ];
	mCurrentBuffer = 0;
	mCurrentTime = 0;

	hr = mOutput->mXAudio->CreateSourceVoice( &mSourceVoice, &mVoiceDescription, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &(mVoiceCallback), NULL, NULL );
	if( FAILED( hr ) )
	{
		//throw
		return;
	}
}

OutputXAudio::Track::~Track()
{
	stop();
	delete [] mDecodedBuffers;
	CloseHandle( mBufferEndEvent );
}

void OutputXAudio::Track::play()
{
	//mLoader->start();
	//fillBufferCallback();
	mIsPlaying = true;
	mQueueThread = shared_ptr<boost::thread>( new boost::thread( boost::bind( &OutputXAudio::Track::fillBuffer, this ) ) );

	::HRESULT hr = mSourceVoice->Start( 0, XAUDIO2_COMMIT_NOW );
	if( FAILED( hr ) ) {
		//throw
		return;
	}
}

void OutputXAudio::Track::stop()
{
	if( ! mIsPlaying ) return;
	mSourceVoice->Stop( 0, XAUDIO2_COMMIT_NOW ); //might not really need this
	mIsPlaying = false;
	SetEvent( mBufferEndEvent ); //signals event to end queuethread
	mQueueThread->join();
	mSourceVoice->FlushSourceBuffers();
}

void OutputXAudio::Track::setVolume( float aVolume )
{
	aVolume = math<float>::clamp( aVolume, 0.0, 1.0 );
	::HRESULT hr = mSourceVoice->SetVolume( aVolume );
	if( FAILED( hr ) ) {
		//throw
	}
}

float OutputXAudio::Track::getVolume() const
{
	float aValue = 0.0;
	mSourceVoice->GetVolume( &aValue );
	return aValue;
}

PcmBuffer32fRef OutputXAudio::Track::getPcmBuffer()
{
	boost::mutex::scoped_lock( mPcmBufferMutex );
	return mLoadedPcmBuffer;
}

//HRESULT OutputXAudio::Track::dataInputCallback( void * audioData, uint32_t dataSize, void * track, uint64_t sampleTime, uint32_t sampleDuration )
void OutputXAudio::Track::fillBuffer()
{
	//OutputXAudio::Track *theTrack = reinterpret_cast<OutputXAudio::Track*>( track );
	XAUDIO2_VOICE_STATE state;
	while( 1 ) {	
		mSourceVoice->GetState( &state );
		if( state.BuffersQueued >= OutputXAudio::Track::sMaxBufferCount ) {
			::WaitForSingleObject( mBufferEndEvent, INFINITE );
		}

		if( ! mIsPlaying ) {
			//tell the loader to go fuck itself?
			break;
		}
		
		BufferList bufferList;
		bufferList.mNumberBuffers = 1;
		Buffer buffer;
		buffer.mData = &( mDecodedBuffers[mCurrentBuffer * mBufferSize] );
		buffer.mDataByteSize = mBufferSize;
		buffer.mNumberChannels = mVoiceDescription.nChannels;
		bufferList.mBuffers = &buffer;
		
		uint32_t sampleCount = mSamplesPerBuffer;
		mLoader->loadData( &sampleCount, &bufferList );
		
		if( sampleCount <= 0 ) {
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
			//TODO: right now this only supports floating point data
			if( ! mLoadingPcmBuffer || ( mLoadingPcmBuffer->getSampleCount() + ( buffer.mDataByteSize / sizeof(float) ) > mLoadingPcmBuffer->getMaxSampleCount() ) ) {
				boost::mutex::scoped_lock lock( mPcmBufferMutex );
				uint32_t bufferSampleCount = 1470; //TODO: make this settable, 1470 ~= 44100(samples/sec)/30(frmaes/second)
				mLoadedPcmBuffer = mLoadingPcmBuffer;
				mLoadingPcmBuffer = PcmBuffer32fRef( new PcmBuffer32f( bufferSampleCount, mVoiceDescription.nChannels, true ) );
			}
			
			mLoadingPcmBuffer->appendInterleavedData( reinterpret_cast<float *>( buffer.mData ), buffer.mDataByteSize / mVoiceDescription.nBlockAlign );
		}
		
		mCurrentBuffer++;
		mCurrentBuffer %= OutputXAudio::Track::sMaxBufferCount;
	}
}

/*class TargetOutputXAudio : public Target
{
  public:
  protected:
	TargetOutputXAudio
}*/

OutputXAudio::OutputXAudio()
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

OutputXAudio::~OutputXAudio()
{
	//TODO: cleanup
}

TrackRef OutputXAudio::addTrack( SourceRef aSource, bool autoPlay ) 
{
	//TargetOutputXAudioRef target = TargetOutputXAudio::createRef( this );
	//TODO: pass target instead of this?
	shared_ptr<OutputXAudio::Track> track = shared_ptr<OutputXAudio::Track>( new OutputXAudio::Track( aSource, this ) );
	TrackId inputBus = track->getTrackId();
	mTracks.insert( std::pair<TrackId,shared_ptr<OutputXAudio::Track> >( inputBus, track ) );
	if( autoPlay ) {
		track->play();
	}
	return track;
}

void OutputXAudio::removeTrack( TrackId trackId )
{
	if( mTracks.find( trackId ) == mTracks.end() ) {
		//TODO: throw OutputInvalidTrackExc();
		return;
	}
	
	mTracks.erase( trackId );
}

}} //namespaces