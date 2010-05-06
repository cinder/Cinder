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
	#include "cinder/audio/CocoaCaConverter.h"
#endif

namespace cinder { namespace audio {

template<typename> class LoaderSourceCallback;

template<typename T>
class Callback : public Source {
	typedef void (T::*CallbackFn)( uint64_t inSampleOffset, uint32_t ioSampleCount, Buffer *ioBuffer );
	
  public: 
	Callback( T& callbackObj, CallbackFn callbackFn, uint32_t aSampleRate = 44100, uint16_t aChannelCount = 2, uint16_t aBitsPerSample = 32, uint16_t aBlockAlign = 8 )
		: Source(), mCallbackFn( callbackFn ), mCallbackObj( callbackObj )
	{
		mIsPcm = true;
		mSampleRate = aSampleRate;
		mChannelCount = aChannelCount;
		mBitsPerSample = aBitsPerSample;
		mBlockAlign = aBlockAlign;
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
	CallbackFn mCallbackFn;
	
	friend class LoaderSourceCallback<T>;
	//friend shared_ptr<Callback<T> > createCallback( void (T::*callbackFn)( uint64_t inSampleOffset, uint32_t ioSampleCount, Buffer *ioBuffer ), T& callbackObj );
};

template<typename T>
shared_ptr<Callback<T> > createCallback( T& callbackObj, void (T::*callbackFn)( uint64_t inSampleOffset, uint32_t ioSampleCount, Buffer *ioBuffer ) )
{
	return shared_ptr<Callback<T> >( new Callback<T>( callbackObj, callbackFn ) );
}

template<typename T>
class LoaderSourceCallback : public Loader {
	public:
		static LoaderRef	createRef( Callback<T> *source, Target *target )
		{
			return shared_ptr<LoaderSourceCallback<T> >( new LoaderSourceCallback<T>( source, target ) );
		}
		
		~LoaderSourceCallback() {}
		
		uint64_t getSampleOffset() const { return mSampleOffset; }
		void setSampleOffset( uint64_t anOffset ) { mSampleOffset = anOffset; }
		void loadData( uint32_t *ioSampleCount, BufferList *ioData ) {	
#if defined( CINDER_COCOA )
			mConverter->loadData( ioSampleCount, ioData );
			mSampleOffset += *ioSampleCount;
#elif defined( CINDER_MSW )
			mSource->getData( mSampleOffset, *ioSampleCount, &ioData->mBuffers[0] );
			mSampleOffset += *ioSampleCount;
#endif
		}
	private:
		LoaderSourceCallback( Callback<T> *source, Target *target );
		Callback<T>						* mSource;
		uint64_t						mSampleOffset;

#if defined(CINDER_COCOA)
		static void dataInputCallback( Loader* aLoader, uint32_t *ioSampleCount, BufferList *ioData, AudioStreamPacketDescription * packetDescriptions ) {
			LoaderSourceCallback * theLoader = dynamic_cast<LoaderSourceCallback *>( aLoader );
			Callback<T> * theSource = theLoader->mSource;	
			theSource->getData( theLoader->mSampleOffset, *ioSampleCount, &ioData->mBuffers[0] );
		}
		shared_ptr<CocoaCaConverter>	mConverter;
#endif	
};

template<typename T>
LoaderSourceCallback<T>::LoaderSourceCallback( Callback<T> *source, Target *target )
	: mSource( source ), mSampleOffset( 0 )
{
#if defined( CINDER_COCOA )
	AudioStreamBasicDescription sourceDescription;
	
	sourceDescription.mFormatID = kAudioFormatLinearPCM; //kAudioFormatLinearPCM;
	sourceDescription.mFormatFlags = CalculateLPCMFlags( mSource->getBitsPerSample(), mSource->getBlockAlign() * 8, mSource->isFloat(), mSource->isBigEndian(), ( ! mSource->isInterleaved() ) /*is non interleaved*/ );
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
	
	mConverter = shared_ptr<CocoaCaConverter>( new CocoaCaConverter( this, &LoaderSourceCallback<T>::dataInputCallback, sourceDescription, targetDescription, mSource->getBlockAlign() ) );
#endif
}

}} //namespace