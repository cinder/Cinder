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

#include "cinder/audio/CocoaCaConverter.h"

namespace cinder { namespace audio {

CocoaCaConverter::CocoaCaConverter( Loader * aLoader, LoaderFunction loaderFn, const AudioStreamBasicDescription& sourceDescription, const AudioStreamBasicDescription& targetDescription, uint32_t maxPacketSize )
	: mLoader( aLoader ), mCurrentPacketDescriptions( NULL ), mLoaderFunction( loaderFn ), mMaxPacketSize( maxPacketSize )
{	
	mConverterBuffer.mNumberBuffers = 0;
	mConverterBuffer.mBuffers = NULL;
	
	OSStatus err = noErr;
	err = AudioConverterNew( &sourceDescription, &targetDescription, &mConverter );
	if( err ) {
		throw IoExceptionUnsupportedDataFormat();
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
}

CocoaCaConverter::~CocoaCaConverter()
{
	cleanupPacketDescriptions();
	cleanupConverterBuffer();
	AudioConverterDispose( mConverter );
}

void CocoaCaConverter::loadData( BufferList *ioData )
{	
	shared_ptr<AudioBufferList> nativeBufferList = createCaBufferList( ioData );

	UInt32 aSampleCount = ioData->mBuffers[0].mSampleCount;
	AudioStreamPacketDescription * outputPacketDescriptions = new AudioStreamPacketDescription[aSampleCount];
	
	OSStatus err = AudioConverterFillComplexBuffer( mConverter, CocoaCaConverter::dataInputCallback, (void *)this, &aSampleCount, nativeBufferList.get(), outputPacketDescriptions );
	delete [] outputPacketDescriptions;
	if( err ) {
		//throw
	}
	
	fillBufferListFromCaBufferList( ioData, nativeBufferList.get(), (uint32_t)aSampleCount );
}

void CocoaCaConverter::fillBufferListFromCaBufferList( BufferList * aBufferList, const AudioBufferList * caBufferList, uint32_t aSampleCount )
{
	aBufferList->mNumberBuffers = caBufferList->mNumberBuffers;
	for( int i = 0; i < caBufferList->mNumberBuffers; i++ ) {
		aBufferList->mBuffers[i].mNumberChannels = caBufferList->mBuffers[i].mNumberChannels;
		aBufferList->mBuffers[i].mDataByteSize = caBufferList->mBuffers[i].mDataByteSize;
		aBufferList->mBuffers[i].mSampleCount = aSampleCount;
		aBufferList->mBuffers[i].mData = caBufferList->mBuffers[i].mData;
	}
}

shared_ptr<AudioBufferList> CocoaCaConverter::createCaBufferList( const BufferList * aBufferList )
{
	shared_ptr<AudioBufferList> caBufferList( (AudioBufferList *)malloc( sizeof( UInt32 ) + ( sizeof( AudioBuffer ) * aBufferList->mNumberBuffers ) ), free );
	
	caBufferList->mNumberBuffers = aBufferList->mNumberBuffers;
	for( int i = 0; i < caBufferList->mNumberBuffers; i++ ) {
		caBufferList->mBuffers[i].mNumberChannels = aBufferList->mBuffers[i].mNumberChannels;
		caBufferList->mBuffers[i].mDataByteSize = aBufferList->mBuffers[i].mDataByteSize;
		caBufferList->mBuffers[i].mData = aBufferList->mBuffers[i].mData;
	}
	
	return caBufferList;
}

void CocoaCaConverter::cleanupPacketDescriptions()
{
	if( mCurrentPacketDescriptions ) {
		delete [] mCurrentPacketDescriptions;
		mCurrentPacketDescriptions = 0;
	}
}

void CocoaCaConverter::cleanupConverterBuffer() 
{
	for( int i = 0; i < mConverterBuffer.mNumberBuffers; i++ ) {
		free( mConverterBuffer.mBuffers[i].mData );
	}
	if( mConverterBuffer.mBuffers ) {
		delete [] mConverterBuffer.mBuffers;
		mConverterBuffer.mBuffers = NULL;
	}
}

OSStatus CocoaCaConverter::dataInputCallback( AudioConverterRef inAudioConverter, UInt32 *ioNumberDataPackets, AudioBufferList *ioData, AudioStreamPacketDescription **outDataPacketDescriptions, void *audioLoader )
{
	OSStatus err = noErr;
	
	CocoaCaConverter * theConverter = reinterpret_cast<CocoaCaConverter *>( audioLoader );
	
	theConverter->cleanupConverterBuffer();
	
	theConverter->mConverterBuffer.mNumberBuffers = ioData->mNumberBuffers;
	theConverter->mConverterBuffer.mBuffers = new BufferGeneric[theConverter->mConverterBuffer.mNumberBuffers];
	for( int i = 0; i < theConverter->mConverterBuffer.mNumberBuffers; i++ ) {
		theConverter->mConverterBuffer.mBuffers[i].mNumberChannels = ioData->mBuffers[i].mNumberChannels;
		theConverter->mConverterBuffer.mBuffers[i].mDataByteSize = ( *ioNumberDataPackets * theConverter->mMaxPacketSize );
		theConverter->mConverterBuffer.mBuffers[i].mData = malloc( theConverter->mConverterBuffer.mBuffers[i].mDataByteSize );
	}
	
	theConverter->cleanupPacketDescriptions();
	theConverter->mCurrentPacketDescriptions = new AudioStreamPacketDescription[*ioNumberDataPackets];
	
	(*theConverter->mLoaderFunction)( theConverter->mLoader, (uint32_t *)ioNumberDataPackets, &theConverter->mConverterBuffer, theConverter->mCurrentPacketDescriptions );
	
	for( int i = 0; i < ioData->mNumberBuffers; i++ ) {
		ioData->mBuffers[i].mData = theConverter->mConverterBuffer.mBuffers[i].mData;
		ioData->mBuffers[i].mNumberChannels = theConverter->mConverterBuffer.mBuffers[i].mNumberChannels;
		ioData->mBuffers[i].mDataByteSize = theConverter->mConverterBuffer.mBuffers[i].mDataByteSize;
	}
	
	if( outDataPacketDescriptions ) {
		*outDataPacketDescriptions = theConverter->mCurrentPacketDescriptions;
	}
	
    return err;
}


}} //namespace