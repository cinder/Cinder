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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/audio/Io.h"
#include "cinder/audio/Buffer.h"

#if defined( CINDER_COCOA )
	#include <CoreAudio/CoreAudioTypes.h>
	#include <AudioToolbox/AudioConverter.h>
#endif

namespace cinder { namespace audio {

template<typename> class Callback;

template<typename T>
class LoaderSourceCallback : public Loader {
	public:
		static LoaderRef	createRef( Callback<T> *source, Target *target )
		{
			return shared_ptr<LoaderSourceCallback<T> >( new LoaderSourceCallback<T>( source, target ) );
		}
		
		~LoaderSourceCallback();
		
		uint64_t getSampleOffset() const;
		void setSampleOffset( uint64_t anOffset );
		void loadData( uint32_t *ioSampleCount, BufferList *ioData );
	private:
		LoaderSourceCallback( Callback<T> *source, Target *target );
		Callback<T>						* mSource;
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


template<typename T>
class Callback : public Source {
	typedef void (T::*fn)( uint64_t inSampleOffset, uint32_t ioSampleCount, Buffer *ioBuffer );
	
  public:
	Callback( fn callbackFn, T& callbackObj/*, uint32_t aSampleRate = 44100, uint16_t aChannelCount = 2, uint16_t aBitsPerSample = 32, uint16_t aBlockAlign = 8*/ )
		: Source(), mCallbackFn( callbackFn ), mCallbackObj( callbackObj )
	{
		mIsPcm = true;
		/*mSampleRate = aSampleRate;
		mChannelCount = aChannelCount;
		mBitsPerSample = aBitsPerSample;
		mBlockAlign = aBlockAlign;*/
		mSampleRate = 44100;
		mChannelCount = 2;
		mBitsPerSample = 32;
		mBlockAlign = 8;
		mIsInterleaved = true;
		mDataType = FLOAT32;
		mIsBigEndian = false;
	}

	virtual ~Callback() {}
	
	LoaderRef getLoader( Target *target ) { return LoaderSourceCallback<T>::createRef( this, target ); }
	double getDuration() const { return 100.0; } //TODO: support for endless sources
	
  private:
	void getData( uint64_t inSampleOffset, uint32_t ioSampleCount, Buffer *ioBuffer ) { ( mCallbackObj.*mCallbackFn )( inSampleOffset, ioSampleCount, ioBuffer ); }
	
	T& mCallbackObj;
	fn mCallbackFn;
	
	friend class LoaderSourceCallback<T>;
	//template<typename Tt>
	//friend shared_ptr<Callback<Tt> > createCallback( void (Tt::*callbackFn)( uint64_t inSampleOffset, uint32_t ioSampleCount, Buffer *ioBuffer ), const Tt& callbackObj );
};

template<typename T>
shared_ptr<Callback<T> > createCallback( void (T::*callbackFn)( uint64_t inSampleOffset, uint32_t ioSampleCount, Buffer *ioBuffer ), T& callbackObj )
{
	return shared_ptr<Callback<T> > ( new Callback<T>( callbackFn, callbackObj ) );
}

template<typename T>
LoaderSourceCallback<T>::LoaderSourceCallback( Callback<T> *source, Target *target )
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
		/*switch(err) {
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
		}*/
	}
#endif
}

template<typename T>
LoaderSourceCallback<T>::~LoaderSourceCallback()
{
#if defined( CINDER_COCOA )
	cleanupPacketDescriptions();
	cleanupConverterBuffer();
	AudioConverterDispose( mConverter );
#endif
}

template<typename T>
uint64_t LoaderSourceCallback<T>::getSampleOffset() const { 
	return mSampleOffset; 
}

template<typename T>
void LoaderSourceCallback<T>::setSampleOffset( uint64_t anOffset ) {
	mSampleOffset = anOffset;
}

template<typename T>
void LoaderSourceCallback<T>::loadData( uint32_t *ioSampleCount, BufferList *ioData )
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
	mSource->getData( mSampleOffset, *ioSampleCount, &ioData->mBuffers[0] );
	mSampleOffset += *ioSampleCount;
#endif
}

#if defined( CINDER_COCOA )
template<typename T>
void LoaderSourceCallback<T>::cleanupPacketDescriptions()
{
	if( mCurrentPacketDescriptions ) {
		delete [] mCurrentPacketDescriptions;
		mCurrentPacketDescriptions = 0;
	}
}

template<typename T>
void LoaderSourceCallback<T>::cleanupConverterBuffer() 
{
	for( int i = 0; i < mConverterBuffer.mNumberBuffers; i++ ) {
		free( mConverterBuffer.mBuffers[i].mData );
	}
	if( mConverterBuffer.mBuffers ) {
		delete [] mConverterBuffer.mBuffers;
		mConverterBuffer.mBuffers = NULL;
	}
}

template<typename T>
OSStatus LoaderSourceCallback<T>::dataInputCallback( AudioConverterRef inAudioConverter, UInt32 *ioNumberDataPackets, AudioBufferList *ioData, AudioStreamPacketDescription **outDataPacketDescriptions, void *audioLoader )
{
	OSStatus err = noErr;
	
	LoaderSourceCallback<T> * theLoader = (LoaderSourceCallback<T> *)audioLoader;
	Callback<T> * theSource = theLoader->mSource;
	
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
	theSource->getData( theLoader->mSampleOffset, (uint32_t)*ioNumberDataPackets, &theLoader->mConverterBuffer.mBuffers[0] );
	
	
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


}} //namespace