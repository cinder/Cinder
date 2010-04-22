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

#include "cinder/audio/Callback.h"
#include "cinder/audio/Buffer.h"

#if defined( CINDER_COCOA )
	#include <CoreAudio/CoreAudioTypes.h>
	#include <AudioToolbox/AudioConverter.h>
#endif

namespace cinder { namespace audio {

class LoaderSourceCallback : public Loader {
	public:
		static LoaderRef	createRef( SourceCallback *source, Target *target )
		{
			return shared_ptr<LoaderSourceCallback>( new LoaderSourceCallback( source, target ) );
		}
		
		~LoaderSourceCallback();
		
		uint64_t getSampleOffset() const;
		void setSampleOffset( uint64_t anOffset );
		void loadData( uint32_t *ioSampleCount, BufferList *ioData );
	private:
		LoaderSourceCallback( SourceCallback *source, Target *target );
		SourceCallback					* mSource;
		uint64_t						mSampleOffset;

#if defined(CINDER_COCOA)
		void cleanupPacketDescriptions();
		void cleanupConverterBuffer();
		static OSStatus dataInputCallback( AudioConverterRef inAudioConverter, UInt32 *ioNumberDataPackets, AudioBufferList *ioData, AudioStreamPacketDescription **outDataPacketDescriptions, void *audioLoader );
		AudioConverterRef				mConverter;
		AudioStreamPacketDescription	* mCurrentPacketDescriptions;
		BufferList						mConverterBuffer;
#endif	
};

class SourceCallback : public Source {
  public:
	SourceCallback( const Callback &callback ) 
		: Source()
	{
		mCallback = callback.mObj->mCallback;
		
		mIsPcm = true;
		mSampleRate = callback.getSampleRate();
		mChannelCount = callback.getChannelCount();
		mBitsPerSample = callback.getBitsPerSample();
		mBlockAlign = callback.getBlockAlign();
		mIsInterleaved = true;
		mDataType = FLOAT32;
		mIsBigEndian = false;
	}
	~SourceCallback() {}
	LoaderRef getLoader( Target *target ) { return LoaderSourceCallback::createRef( this, target ); }
	
	double getDuration() const { return 100.0; } //TODO: add support for endless sources
  private:
	CallbackFunction	mCallback;
	
	friend class LoaderSourceCallback;
};

LoaderSourceCallback::LoaderSourceCallback( SourceCallback *source, Target *target )
	: mSource( source ), mSampleOffset( 0 )
{
#if defined( CINDER_COCOA )
	mCurrentPacketDescriptions = 0;
	mConverterBuffer.mNumberBuffers = 0;
	mConverterBuffer.mBuffers = NULL;

	AudioStreamBasicDescription sourceDescription;
	
	sourceDescription.mFormatID = kAudioFormatLinearPCM; //kAudioFormatLinearPCM;
	sourceDescription.mFormatFlags = CalculateLPCMFlags( mSource->getBitsPerSample(), mSource->getBlockAlign() * 8, mSource->isFloat(), mSource->isBigEndian(), ( ! mSource->isInterleaved() ) /*is non interleaved*/ );
	//sourceDescription.mFormatFlags |= kAudioFormatFlagIsPacked;
	sourceDescription.mSampleRate = source->getSampleRate();
	sourceDescription.mBytesPerPacket = ( mSource->getBlockAlign() ); //( mSource->getBitsPerSample() * mSource->getChannelCount() ) / 8;
	sourceDescription.mFramesPerPacket = 1;
	sourceDescription.mBytesPerFrame = ( mSource->getBlockAlign() );//( mSource->getBitsPerSample() * mSource->getChannelCount() ) / 8;
	sourceDescription.mChannelsPerFrame = source->getChannelCount();
	sourceDescription.mBitsPerChannel = source->getBitsPerSample();
	
	AudioStreamBasicDescription targetDescription;
	
	if( ! target->isPcm() ) {
		//throw!
	}
	
	targetDescription.mFormatID = kAudioFormatLinearPCM; //target->mNativeFormatId;
	targetDescription.mFormatFlags = CalculateLPCMFlags( target->getBitsPerSample(), target->getBlockAlign() * 8, target->isFloat(), target->isBigEndian(), ( ! target->isInterleaved() ) ); //target->mNativeFormatFlags
	targetDescription.mSampleRate = target->getSampleRate();
	targetDescription.mBytesPerPacket =  target->getBlockAlign(); //target->mBytesPerPacket;
	targetDescription.mFramesPerPacket = 1; //target->mFramesPerPacket;
	targetDescription.mBytesPerFrame = ( target->getBlockAlign() ); //target->mBytesPerFrame;
	targetDescription.mChannelsPerFrame = target->getChannelCount();
	targetDescription.mBitsPerChannel = target->getBitsPerSample();
	
	OSStatus err = noErr;
	err = AudioConverterNew( &sourceDescription, &targetDescription, &mConverter );
	if( err ) {
		//throw
		switch(err) {
			case kAudioConverterErr_FormatNotSupported:
				std::cout << "kAudioConverterErr_FormatNotSupported" << std::endl;
			break;
			case kAudioConverterErr_OperationNotSupported:
				std::cout << "kAudioConverterErr_OperationNotSupported" << std::endl;
			break;
			case kAudioConverterErr_PropertyNotSupported:
				std::cout << "kAudioConverterErr_PropertyNotSupported" << std::endl;
			break;
			case kAudioConverterErr_InvalidInputSize:
				std::cout << "kAudioConverterErr_InvalidInputSize" << std::endl;
			break;
			case kAudioConverterErr_InvalidOutputSize:
				std::cout << "kAudioConverterErr_InvalidOutputSize" << std::endl;
			break;
			case kAudioConverterErr_UnspecifiedError:
				std::cout << "kAudioConverterErr_UnspecifiedError" << std::endl;
			break;
			case kAudioConverterErr_BadPropertySizeError:
				std::cout << "kAudioConverterErr_BadPropertySizeError" << std::endl;
			break;
			case kAudioConverterErr_RequiresPacketDescriptionsError:
				std::cout << "kAudioConverterErr_RequiresPacketDescriptionsError" << std::endl;
			break;
			case kAudioConverterErr_InputSampleRateOutOfRange:
				std::cout << "kAudioConverterErr_InputSampleRateOutOfRange" << std::endl;
			break;
			case kAudioConverterErr_OutputSampleRateOutOfRange:
				std::cout << "kAudioConverterErr_OutputSampleRateOutOfRange" << std::endl;
			break;
		}
	}
#endif
}

LoaderSourceCallback::~LoaderSourceCallback()
{
#if defined( CINDER_COCOA )
	cleanupPacketDescriptions();
	cleanupConverterBuffer();
	AudioConverterDispose( mConverter );
#endif
}

uint64_t LoaderSourceCallback::getSampleOffset() const { 
	return mSampleOffset; 
}

void LoaderSourceCallback::setSampleOffset( uint64_t anOffset ) {
	mSampleOffset = anOffset;
}

void LoaderSourceCallback::loadData( uint32_t *ioSampleCount, BufferList *ioData )
{	
#if defined( CINDER_COCOA )
	shared_ptr<AudioBufferList> nativeBufferList = createCaBufferList( ioData );

	AudioStreamPacketDescription * outputPacketDescriptions = new AudioStreamPacketDescription[*ioSampleCount];
	OSStatus err = AudioConverterFillComplexBuffer( mConverter, LoaderSourceCallback::dataInputCallback, (void *)this, (UInt32 *)ioSampleCount, nativeBufferList.get(), outputPacketDescriptions );
	delete [] outputPacketDescriptions;
	if( err ) {
		//throw
	}
	
	fillBufferListFromCaBufferList( ioData, nativeBufferList.get() );
#elif defined( CINDER_MSW )
	(*mSource->mCallback)( mSampleOffset, ioSampleCount, ioData->mBuffers[0] );
	mSampleOffset += *ioSampleCount;
#endif
}

#if defined( CINDER_COCOA )
void LoaderSourceCallback::cleanupPacketDescriptions()
{
	if( mCurrentPacketDescriptions ) {
		delete [] mCurrentPacketDescriptions;
		mCurrentPacketDescriptions = 0;
	}
}

void LoaderSourceCallback::cleanupConverterBuffer() 
{
	for( int i = 0; i < mConverterBuffer.mNumberBuffers; i++ ) {
		free( mConverterBuffer.mBuffers[i].mData );
	}
	if( mConverterBuffer.mBuffers ) {
		delete [] mConverterBuffer.mBuffers;
		mConverterBuffer.mBuffers = NULL;
	}
}

OSStatus LoaderSourceCallback::dataInputCallback( AudioConverterRef inAudioConverter, UInt32 *ioNumberDataPackets, AudioBufferList *ioData, AudioStreamPacketDescription **outDataPacketDescriptions, void *audioLoader )
{
	OSStatus err = noErr;
	
	LoaderSourceCallback * theLoader = (LoaderSourceCallback *)audioLoader;
	SourceCallback * theSource = theLoader->mSource;
	
	theLoader->cleanupConverterBuffer();
	
	theLoader->mConverterBuffer.mNumberBuffers = ioData->mNumberBuffers;
	theLoader->mConverterBuffer.mBuffers = new Buffer[theLoader->mConverterBuffer.mNumberBuffers];
	for( int i = 0; i < theLoader->mConverterBuffer.mNumberBuffers; i++ ) {
		theLoader->mConverterBuffer.mBuffers[i].mNumberChannels = ioData->mBuffers[i].mNumberChannels;
		theLoader->mConverterBuffer.mBuffers[i].mDataByteSize = ( *ioNumberDataPackets * theSource->getBlockAlign() );
		theLoader->mConverterBuffer.mBuffers[i].mData = malloc( theLoader->mConverterBuffer.mBuffers[i].mDataByteSize );
	}
	
	theLoader->cleanupPacketDescriptions();
	theLoader->mCurrentPacketDescriptions = new AudioStreamPacketDescription[*ioNumberDataPackets];
	
	//err = AudioFileReadPackets( theSource->mFileRef, false, (UInt32 *)&(theLoader->mConverterBuffer.mBuffers[0].mDataByteSize), theLoader->mCurrentPacketDescriptions, theLoader->mPacketOffset, (UInt32 *)ioNumberDataPackets, theLoader->mConverterBuffer.mBuffers[0].mData );
	(*theSource->mCallback)( theLoader->mSampleOffset, (uint32_t *)ioNumberDataPackets, &theLoader->mConverterBuffer.mBuffers[0] );
	
	
	//ioData->mBuffers[0].mData = theTrack->mSourceBuffer;
	for( int i = 0; i < ioData->mNumberBuffers; i++ ) {
			ioData->mBuffers[i].mData = theLoader->mConverterBuffer.mBuffers[i].mData;
			ioData->mBuffers[i].mNumberChannels = theLoader->mConverterBuffer.mBuffers[i].mNumberChannels;
			ioData->mBuffers[i].mDataByteSize = theLoader->mConverterBuffer.mBuffers[i].mDataByteSize;
	}
	
	if( outDataPacketDescriptions ) {
		*outDataPacketDescriptions = theLoader->mCurrentPacketDescriptions;
	}
	
	theLoader->mSampleOffset += *ioNumberDataPackets;
	
    return err;
}
#endif

Callback::Obj::Obj( CallbackFunction aCallback, uint32_t aSampleRate, uint16_t aChannelCount, uint16_t aBitsPerSample, uint16_t aBlockAlign )
	: mCallback( aCallback ), mSampleRate( aSampleRate ), mChannelCount( aChannelCount ), mBitsPerSample( aBitsPerSample ), mBlockAlign( aBlockAlign )
{
}

Callback::Obj::~Obj()
{
}


Callback::Callback( CallbackFunction aCallback, uint32_t aSampleRate, uint16_t aChannelCount, uint16_t aBitsPerSample, uint16_t aBlockAlign ) 
	: mObj( new Obj( aCallback, aSampleRate, aChannelCount, aBitsPerSample, aBlockAlign ) )
{
}

Callback::operator SourceRef() const
{
	return shared_ptr<Source>( new SourceCallback( *this ) );
}

}} //namespace
