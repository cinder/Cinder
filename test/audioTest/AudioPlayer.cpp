#include "AudioPlayer.h"

namespace fli {

AudioPlayerTrack::AudioPlayerTrack( shared_ptr<AudioSource> aAudioSource, AudioPlayer * aAudioPlayer )
{
	mAudioPlayer = aAudioPlayer;
	mAudioSource = aAudioSource;
	
	mIsLooping = false;
	mIsPlaying = false;
	
	setup();
}

AudioPlayerTrack::~AudioPlayerTrack()
{
	cleanup();
}

void AudioPlayerTrack::setup()
{
#if defined( FLI_MAC )
	mPacketOffset = 0;
	mCurrentPacketDescriptions = 0;
	
	mSourceConverter = new AudioConverterRef;
	
	AudioStreamBasicDescription sourceDescription;
	
	sourceDescription.mFormatID = mAudioSource->mNativeFormatID; //kAudioFormatLinearPCM;
	sourceDescription.mFormatFlags = mAudioSource->mNativeFormatFlags; //CalculateLPCMFlags( mAudioSource->getBitsPerSample(), mAudioSource->getBlockAlign() * 8, false, false, false );
	sourceDescription.mSampleRate = mAudioSource->getSampleRate();
	sourceDescription.mBytesPerPacket = mAudioSource->mBytesPerPacket; //( mAudioSource->getBitsPerSample() * mAudioSource->getChannelCount() ) / 8;
	sourceDescription.mFramesPerPacket = mAudioSource->mFramesPerPacket; //1;
	sourceDescription.mBytesPerFrame = mAudioSource->mBytesPerFrame; //( mAudioSource->getBitsPerSample() * mAudioSource->getChannelCount() ) / 8;
	sourceDescription.mChannelsPerFrame = mAudioSource->getChannelCount();
	sourceDescription.mBitsPerChannel = mAudioSource->getBitsPerSample();
	sourceDescription.mReserved = mAudioSource->mReserved; //0;
	
	OSStatus err = noErr;
	err = AudioConverterNew( &sourceDescription, mAudioPlayer->mPlayerDescription, mSourceConverter );
	if( err != noErr ) {
		std::cout << "Error setting up converter." << std::endl;
	}
#endif
}

void AudioPlayerTrack::cleanup()
{
#if defined( FLI_MAC )
	if( mCurrentPacketDescriptions ) {
		delete [] mCurrentPacketDescriptions;
		mCurrentPacketDescriptions = 0;
	}
#endif
	
	if( mCurrentPacketDescriptions ) {
		free( mSourceBuffer );
		mSourceBuffer = 0;
	}
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AudioPlayer

#if defined( FLI_MAC )
//oncomplete callback for mac
void AudioPlayer::onComplete(void *aData, ScheduledAudioFileRegion *fileRegion, OSStatus result)
{
	//TODO: Anoyingly, on really short files that can be loaded into a single buffer, this will actually get called before playback starts
	AudioPlayer * audioPlayer = (AudioPlayer *)aData;
	audioPlayer->onStop();
}
#endif

AudioPlayer::AudioPlayer()
{	
	mTracks = new std::vector<AudioPlayerTrack *>;
	
#if defined( FLI_MAC )
	mIsPaused = false;
	
#elif defined( FLI_MSW )
	mGraphBuilder = 0;
	mMediaEvent = 0;
	mMediaControl = 0;
	mSeek = 0;
	mBasicAudio = 0;
#endif

	setupGraph();
}


AudioPlayer::~AudioPlayer()
{
	if( isRunning() ) {
		stop();
	}

#if defined( FLI_MAC )
	AUGraphUninitialize( mGraph );
	AUGraphClose( mGraph );
#elif defined( FLI_MSW )
	if( mBasicAudio ) mBasicAudio->Release();
	if( mSeek ) mSeek->Release();
	if( mMediaEvent ) mMediaEvent->Release();
	if( mMediaControl ) mMediaControl->Release();
	if( mGraphBuilder ) mGraphBuilder->Release();

	CoUninitialize();
#endif
}

/*void AudioPlayer::setSource( AudioSource * aAudioBuffer )
{
	setSource( shared_ptr<AudioSource>( aAudioBuffer ) );
}

void AudioPlayer::setSource( shared_ptr<AudioSource> aAudioSource )
{
	mAudioSource = aAudioSource;
	if( isRunning() ) {
		stop();
		play();
	}
}*/

void AudioPlayer::play()
{
#if defined( FLI_MAC )
	if( ! mIsPaused ) {
		//setupSource();
	}
	
	//play!
	OSStatus err = AUGraphStart( mGraph );
	if( err == noErr )
		mIsPaused = false;
		
#elif defined( FLI_MSW )
	mMediaControl->Run();
#endif
}

void AudioPlayer::pause()
{
#if defined( FLI_MAC )
	if( ! isRunning() ) return;

	OSStatus err = AUGraphStop( mGraph );
	if( err == noErr )
		mIsPaused = true;
#elif defined( FLI_MSW )
	HRESULT hr = mMediaControl->Pause();
#endif
}

void AudioPlayer::stop()
{
#if defined( FLI_MAC )
	//this triggers the onstop callback
	AudioUnitReset( mSourceUnit, kAudioUnitScope_Global, NULL );
#elif defined( FLI_MSW )
	HRESULT hr = mMediaControl->Stop();
	if( hr == S_OK )
		onStop();  //might not need this once event listening is in place
#endif
}

bool AudioPlayer::isRunning() const {
#if defined( FLI_MAC )
	Boolean isRunning;
	AUGraphIsRunning( mGraph, &isRunning );
	return mIsPaused || isRunning;
#elif defined( FLI_MSW )
	 OAFilterState theState = 0;
	 mMediaControl->GetState( 0, &theState );
	 if( theState != State_Stopped ) {
		return true;
	 }
	 return false;
#endif
}

bool AudioPlayer::isPlaying() const 
{
#if defined( FLI_MAC )
	return isRunning() && ! mIsPaused;
#elif defined( FLI_MSW )
	OAFilterState theState;
	HRESULT hr = mMediaControl->GetState( 0, &theState );
	if( hr == S_OK ) {
		if( theState != State_Running ) {
			return false;
		}
	}
	return true;
#endif
}

double AudioPlayer::getCurrentPlayTime() const 
{
#if defined( FLI_MAC )
	//TODO: once you can schedule a start time this value will need to be added to that start time
	/*UInt32 size = sizeof( AudioTimeStamp );
	AudioTimeStamp timestamp;
	AudioUnitGetProperty( mSourceUnit, kAudioUnitProperty_CurrentPlayTime, kAudioUnitScope_Output, 0, &timestamp, &size );
	return ( timestamp.mSampleTime / ( mAudioSource->getSampleRate() / 1000.0 ) );*/
	//TODO!!
	return 0.0;
#elif defined( FLI_MSW )
	HRESULT hr;
	LONGLONG currentTime = 0;
	hr = mSeek->GetCurrentPosition( &currentTime );
	if( FAILED(hr) )
		return 0.0;
	//convert 100-nanoseonds to seconds
	return ( currentTime / 10000.0 );
#endif
}

//tmp
void AudioPlayer::doMeter()
{
#if defined( FLI_MAC )
	if( ! isRunning() || mIsPaused ) return;

	OSStatus err;
	float aValue;
	err = AudioUnitGetParameter( mMixerUnit, kStereoMixerParam_PreAveragePower, kAudioUnitScope_Input, 0, &aValue );
	std::cout << "Pre Average Power L:" << aValue << " : " << err << std::endl;
	err = AudioUnitGetParameter( mMixerUnit, kStereoMixerParam_PreAveragePower + 1, kAudioUnitScope_Input, 0, &aValue );
	std::cout << "Pre Average Power R:" << aValue << " : " << err << std::endl;
	
	err = AudioUnitGetParameter( mMixerUnit, kStereoMixerParam_PrePeakHoldLevel, kAudioUnitScope_Input, 0, &aValue );
	std::cout << "Pre Peak Hold Level L:" << aValue << std::endl;
	err = AudioUnitGetParameter( mMixerUnit, kStereoMixerParam_PrePeakHoldLevel + 1, kAudioUnitScope_Input, 0, &aValue );
	std::cout << "Pre Peak Hold Level R:" << aValue << std::endl;
	
	err = AudioUnitGetParameter( mMixerUnit, kStereoMixerParam_PostAveragePower, kAudioUnitScope_Input, 0, &aValue );
	std::cout << "Post Average Power L:" << aValue << std::endl;
	err = AudioUnitGetParameter( mMixerUnit, kStereoMixerParam_PostAveragePower + 1, kAudioUnitScope_Input, 0, &aValue );
	std::cout << "Post Average Power R:" << aValue << std::endl;
	
	err = AudioUnitGetParameter( mMixerUnit, kStereoMixerParam_PostPeakHoldLevel, kAudioUnitScope_Input, 0, &aValue );
	std::cout << "Post Peak Hold Level L:" << aValue << std::endl;
	err = AudioUnitGetParameter( mMixerUnit, kStereoMixerParam_PostPeakHoldLevel + 1, kAudioUnitScope_Input, 0, &aValue );
	std::cout << "Post Peak Hold Level R:" << aValue << std::endl;
	
	std::cout << std::endl;
#elif defined( FLI_MSW )

#endif
}

void AudioPlayer::getSample() 
{
#if defined( FLI_MAC )
	
#elif defined( FLI_MSW )

#endif
}

float AudioPlayer::getVolume() const
{
#if defined( FLI_MAC )
	float aValue = 0.0;
	OSStatus err = AudioUnitGetParameter( mMixerUnit, kStereoMixerParam_Volume, kAudioUnitScope_Output, 0, &aValue );
	if( err ) {
		std::cout << "get volume err:" << err << std::endl;
	}
	return aValue;
#elif defined( FLI_MSW )
	long intVal;
	mBasicAudio->get_Volume( &intVal );
	return  (float) ( 10000 + intVal ) / 10000.0;
#endif
}

void AudioPlayer::setVolume( float aValue ) 
{
	if( aValue >= 0.0 && aValue <= 1.0 ) {
#if defined( FLI_MAC )
		OSStatus err = AudioUnitSetParameter( mMixerUnit, kStereoMixerParam_Volume, kAudioUnitScope_Output, 0, aValue, 0 );
		if( err ) {
			std::cout << "set volume err:" << err << std::endl;
		}
#elif defined( FLI_MSW )
		//convert 0.0 to 1.0 range to -10000 to 0 range
		long intValue = 0 - ( ( long ) ( ( 1.0 - aValue ) * 10000 ) );
		mBasicAudio->put_Volume( intValue );
#endif
	}
}

float AudioPlayer::getBalance() const {
	float aValue = 0.0;
#if defined( FLI_MAC )
	OSStatus err = AudioUnitGetParameter( mMixerUnit, kStereoMixerParam_Pan, kAudioUnitScope_Input, 0, &aValue );
	if( err ) {
		std::cout << "get pan err:" << err << std::endl;
	}
	aValue = aValue * 2.0 - 1.0;
#elif defined( FLI_MSW ) 
	long intVal;
	mBasicAudio->get_Balance( &intVal );
	
	//convert from -10,000 to 10,000 range
	aValue = intVal / 10000.0;
#endif
	return aValue;
}

void AudioPlayer::setBalance( float aValue ) {
	if( aValue >= -1.0 && aValue <= 1.0 ) {
#if defined( FLI_MAC )
		//convert value to between 0.0 and 1.0
		aValue = aValue * 0.5 + 0.5;
		OSStatus err = AudioUnitSetParameter( mMixerUnit, kStereoMixerParam_Pan, kAudioUnitScope_Input, 0, aValue, 0 );
		if( err ) {
			std::cout << "set pan err:" << err << std::endl;
		}
#elif defined( FLI_MSW )
		//convert value to -10,000 to 10,000 range
		aValue = aValue * 10000;
		mBasicAudio->put_Balance( aValue );
#endif
	}
}

void AudioPlayer::addSource( AudioSource * aAudioSource, bool autoPlay )
{
	addSource( shared_ptr<AudioSource>( aAudioSource ), autoPlay );
}

void AudioPlayer::addSource( shared_ptr<AudioSource> aAudioSource, bool autoPlay )
{
	AudioPlayerTrack * aTrack = new AudioPlayerTrack( aAudioSource, this );
	mTracks->push_back( aTrack );
	setupTrack( aTrack, mTracks->size() - 1 );
	
	if( autoPlay ) {
		aTrack->play();
	}
}

/*void AudioPlayer::setupSource()
{
#if defined( FLI_MAC )
	if( mSourceConverter ) {
		AudioConverterDispose( *mSourceConverter );
	}
	
	if( mSourceBuffer ) {
		free( mSourceBuffer );
		mSourceBuffer = 0;
	}
	
	mPacketOffset = 0;
	
	mSourceConverter = new AudioConverterRef;
	
	AudioStreamBasicDescription sourceDescription;
	
	sourceDescription.mFormatID = kAudioFormatLinearPCM;
	sourceDescription.mFormatFlags = CalculateLPCMFlags( mAudioSource->getBitsPerSample(), mAudioSource->getBlockAlign() * 8, false, false, false );
	sourceDescription.mSampleRate = mAudioSource->getSampleRate();
	sourceDescription.mBytesPerPacket = ( mAudioSource->getBitsPerSample() * mAudioSource->getChannelCount() ) / 8;
	sourceDescription.mFramesPerPacket = 1;
	sourceDescription.mBytesPerFrame = ( mAudioSource->getBitsPerSample() * mAudioSource->getChannelCount() ) / 8;
	sourceDescription.mChannelsPerFrame = mAudioSource->getChannelCount();
	sourceDescription.mBitsPerChannel = mAudioSource->getBitsPerSample();
	sourceDescription.mReserved = 0;
	
	OSStatus err = noErr;
	err = AudioConverterNew( &sourceDescription, mPlayerDescription, mSourceConverter );
	if( err != noErr ) {
		std::cout << "Error setting up converter." << std::endl;
	}
#elif( FLI_MSW )

#endif
}*/

void AudioPlayer::setupGraph()
{
#if defined( FLI_MAC )
	NewAUGraph( &mGraph );
	
	ComponentDescription cd;
	cd.componentManufacturer = kAudioUnitManufacturer_Apple;
	cd.componentFlags = 0;
	cd.componentFlagsMask = 0;
	
	//output node
	cd.componentType = kAudioUnitType_Output;
	cd.componentSubType = kAudioUnitSubType_DefaultOutput;
	
	//connect & setup
	AUGraphOpen( mGraph );
	
	//initialize component - todo add error checking
	if( AUGraphAddNode( mGraph, &cd, &mOutputNode ) != noErr ) {
		std::cout << "Error 1!" << std::endl;
	}
	
	if( AUGraphNodeInfo( mGraph, mOutputNode, NULL, &mOutputUnit ) != noErr ) {
		std::cout << "Error 2!" << std::endl;	
	}
	
	UInt32 sizer;
	Boolean outWritable;
	AudioUnitGetPropertyInfo( mOutputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 0, &sizer, &outWritable);
	
	//stereo mixer node
	cd.componentType = kAudioUnitType_Mixer;
	cd.componentSubType = kAudioUnitSubType_StereoMixer;
	AUGraphAddNode( mGraph, &cd, &mMixerNode);
	
	/*//file AU Node
	cd.componentType = kAudioUnitType_Generator;
	//cd.componentSubType = kAudioUnitSubType_AudioFilePlayer;
	cd.componentSubType = kAudioUnitSubType_ScheduledSoundPlayer;
	AUGraphAddNode( mGraph, &cd, &mSourceNode );
	
	
	//overload listener of FileNode
	AUGraphNodeInfo( mGraph, mSourceNode, NULL, &mSourceUnit );
	AudioUnitAddRenderNotify(mSourceUnit, fli_renderCallback, this);*/
	
	//setup mixer AU
	OSStatus err;
	err = AUGraphNodeInfo( mGraph, mMixerNode, NULL, &mMixerUnit );
	if( err ) {
		std::cout << "Error 4" << std::endl;
	}
	
	UInt32 dsize;
	mPlayerDescription = new AudioStreamBasicDescription;
	AudioUnitGetProperty( mOutputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, mPlayerDescription, &dsize );
	
	//AudioStreamBasicDescription desc;
	//mPlayerDescription->mFormatID = kAudioFormatLinearPCM;
	//mPlayerDescription->mFormatFlags = kAudioFormatFlagsCanonical;
	//mPlayerDescription->mFormatFlags = kAudioFormatFlagsCanonical;
	//desc.mBytesPerPacket = mAudioSource->getChannelCount() * mAudioSource->getBitsPerSample() / 8; /*2 * sizeof( float )*/; /* channel count * sizeof( AudioSampleType ) TODO: make this actually work based on the audioSource*/
	//desc.mFramesPerPacket = 1;
	//desc.mBytesPerFrame = desc.mBytesPerPacket;
	//desc.mChannelsPerFrame = mAudioSource->getChannelCount();
	//desc.mBitsPerChannel = mAudioSource->getBitsPerSample();
	//mPlayerDescription->mSampleRate = mAudioSource->getSampleRate();
	//desc.mReserved = 0;
	
	
	//TODO: cleanup error checking in all of this
	ComponentResult err2;
	err2 = AudioUnitSetProperty( mMixerUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, mPlayerDescription, dsize );
	if( err2 ) {
		std::cout << "Error setting output unit output stream format" << std::endl;
	}
	
	err2 = AudioUnitSetProperty( mMixerUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 0, mPlayerDescription, dsize );
	if( err2 ) {
		std::cout << "Error setting mixer unit input stream format" << std::endl;
	}
	
	err2 = AudioUnitSetProperty( mOutputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, mPlayerDescription, dsize );
	if( err2 ) {
		std::cout << "Error setting output unit input stream format" << std::endl;
	}
	
	/*err2 = AudioUnitSetProperty( mOutputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 0, &desc, sizeof(desc) );
	if( err2 ) {
		
		std::cout << "Error setting output unit output stream format" << std::endl;
	}*/
	
	/*desc.mChannelsPerFrame = 0;
	desc.mBitsPerChannel = 0;
	desc.mReserved = 0;*/
	
	//connect inputs
	//AUGraphConnectNodeInput( mGraph, mSourceNode, 0, mMixerNode, 0 );
	AUGraphConnectNodeInput( mGraph, mMixerNode, 0, mOutputNode, 0 );
	
	AudioStreamBasicDescription outDesc;
	UInt32 size = sizeof(outDesc);
	AudioUnitGetProperty( mOutputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 0, &outDesc, &size );
	
	AUGraphInitialize( mGraph );
	
	//race condition work around??
	usleep( 10 * 1000 );
	
	//TODO: tell the output AU about the order of the channels if there are more than 2 

	// turn metering ON
	UInt32 data = 1;
	AudioUnitSetProperty( mMixerUnit, kAudioUnitProperty_MeteringMode, kAudioUnitScope_Global, 0, &data, sizeof(data) );	
#elif defined( FLI_MSW )
	HRESULT hr;
	//initialize COM --this will probably conflict with com initialization in Capture code
	hr = CoInitialize( NULL );
	if( FAILED( hr ) ) {
		//handle error
		return;
	}

	//initialize direct show objects
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder,(void**)&mGraphBuilder);
	
	if ( FAILED( hr ) ) {
		//handle error
		return;
	}
	mGraphBuilder->QueryInterface( IID_IMediaControl, (void**)&mMediaControl );
	mGraphBuilder->QueryInterface( IID_IMediaEvent, (void**)&mMediaEvent );
	mGraphBuilder->QueryInterface( IID_IMediaSeeking, (void**)&mSeek );
	mGraphBuilder->QueryInterface( IID_IBasicAudio, (void**)&mBasicAudio );

	//create graph
	WCHAR * wfilename = (WCHAR *)mAudioSource->getNativeFileRef();
	hr = mGraphBuilder->RenderFile( wfilename, NULL );
	if ( FAILED( hr ) ) {
		//handle error
		return;
	}

	//set time format
	mSeek->SetTimeFormat( &TIME_FORMAT_MEDIA_TIME );
#endif
}

void AudioPlayer::setupTrack( AudioPlayerTrack * aAudioTrack, int inputBusNo )
{
	OSStatus err = noErr;

	AURenderCallbackStruct rcbs;
	rcbs.inputProc = &fli_renderCallback;
	rcbs.inputProcRefCon = (void *)aAudioTrack;
	
	err = AudioUnitSetProperty( mMixerUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, inputBusNo, &rcbs, sizeof(rcbs) );
	
	if( err ) {
		std::cout << "Error 1" << std::endl;
	}
}


void AudioPlayer::onStop()
{
	//TODO - reset to begining of audio?
#if defined( FLI_MAC )
	OSStatus err = AUGraphStop( mGraph );
	if( err == noErr ) {
		mIsPaused = false;
	}
#elif defined( FLI_MSW )
	
#endif
}

#if defined( FLI_MAC )
OSStatus fli_renderCallback( void * audioTrack, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32  inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData)
{	
	OSStatus err = noErr;
	//TODO: when track is paused fill the buffer with silent data
	
	fli::AudioPlayerTrack * theTrack = (fli::AudioPlayerTrack *)audioTrack;
	
	//TODO: only crate this if outputFormat bytesPerPacket is 0??
	AudioStreamPacketDescription * outputPacketDescriptions = new AudioStreamPacketDescription[inNumberFrames];
	
	err = AudioConverterFillComplexBuffer( *theTrack->mSourceConverter, fli::fli_dataInputCallback, audioTrack, &inNumberFrames, ioData, outputPacketDescriptions );
	if( err ) {
		/*std::cout << "kAudioConverterErr_FormatNotSupported:" << kAudioConverterErr_FormatNotSupported << std::endl;
		std::cout << "kAudioConverterErr_UnspecifiedError:" << kAudioConverterErr_UnspecifiedError << std::endl;
		std::cout << "kAudioConverterErr_PropertyNotSupported:" << kAudioConverterErr_PropertyNotSupported << std::endl;
		std::cout << "kAudioConverterErr_InvalidInputSize:" << kAudioConverterErr_InvalidInputSize << std::endl;
		std::cout << "kAudioConverterErr_InvalidOutputSize:" << kAudioConverterErr_InvalidOutputSize << std::endl;
		std::cout << "kAudioConverterErr_BadPropertySizeError:" << kAudioConverterErr_BadPropertySizeError << std::endl;
		std::cout << "kAudioConverterErr_RequiresPacketDescriptionsError:" << kAudioConverterErr_RequiresPacketDescriptionsError << std::endl;
		std::cout << "kAudioConverterErr_InputSampleRateOutOfRange:" << kAudioConverterErr_InputSampleRateOutOfRange << std::endl;
		std::cout << "kAudioConverterErr_OutputSampleRateOutOfRange:" << kAudioConverterErr_OutputSampleRateOutOfRange << std::endl;*/
		std::cout << "Error filling complex buffer" << std::endl;
	}
	
	//TODO: save most recent PCM data in a buffer here for potential FFT or Wave Form display?? Maybe do this in a mixerUnit output callback?
	delete [] outputPacketDescriptions;
	
    return noErr;
}

OSStatus fli_dataInputCallback( AudioConverterRef inAudioConverter, UInt32 *ioNumberDataPackets, AudioBufferList *ioData, AudioStreamPacketDescription **outDataPacketDescriptions, void *audioTrack )
{
	OSStatus    err = noErr;

	fli::AudioPlayerTrack * theTrack = (fli::AudioPlayerTrack *)audioTrack;
	shared_ptr<AudioSource> theAudioSource = theTrack->mAudioSource;

	theTrack->cleanup();	
	uint32_t bytesCopied = 0;

    // initialize in case of failure
    ioData->mBuffers[0].mData = NULL;
    ioData->mBuffers[0].mDataByteSize = 0;
	
	if( ! theTrack->mIsPlaying ) {
		return noErr;
	}
	
	theTrack->mCurrentPacketDescriptions = new AudioStreamPacketDescription[*ioNumberDataPackets];
	
	theAudioSource->getData( &theTrack->mSourceBuffer, &bytesCopied, theTrack->mCurrentPacketDescriptions, (uint32_t)theTrack->mPacketOffset, (uint32_t)*ioNumberDataPackets, theTrack->mIsLooping );
	
	ioData->mBuffers[0].mData = theTrack->mSourceBuffer;
	ioData->mBuffers[0].mDataByteSize = bytesCopied;
	
	if( outDataPacketDescriptions ) {
		*outDataPacketDescriptions = theTrack->mCurrentPacketDescriptions;
	} else {
		*outDataPacketDescriptions = NULL;
	}
	
	theTrack->mPacketOffset += *ioNumberDataPackets;
	
	if( ioData->mBuffers[0].mData == NULL ) {
		theTrack->mIsPlaying = false;
	}
	
    return err;
}

} //namespace fli

#endif