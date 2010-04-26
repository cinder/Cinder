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

#include "cinder/audio/SourceFileWindowsMedia.h"
#include "cinder/msw/cinderMSW.h"

#include <sstream>

namespace cinder { namespace audio {

LoaderSourceFileWindowsMediaRef LoaderSourceFileWindowsMedia::createRef( SourceFileWindowsMedia *source, Target *target ) {
	return LoaderSourceFileWindowsMediaRef( new LoaderSourceFileWindowsMedia( source, target ) );
}

LoaderSourceFileWindowsMedia::LoaderSourceFileWindowsMedia( SourceFileWindowsMedia * source, Target * target ) 
	: Loader(), mSource( source ), mSampleOffset( 0 ), mSrcSampleRate( 0 ), mSrcChannelCount( 0 ), mSrcBitsPerSample( 0 )
{
	::HRESULT hr;

	//setup readers
	hr = ::WMCreateSyncReader(0, 0, &mReader);
	if( FAILED( hr ) ) {
		//TODO: throw
		return;
	}

	hr = mReader->QueryInterface( IID_IWMHeaderInfo, (void**)&mHeaderInfo );
	if(FAILED(hr)) {
		//TODO: throw
		return;
	}

	//turn data into stream
	::HGLOBAL hMem;
	::IStream* pStream;
	
	uint32_t dataSize = mSource->getLength();
	hMem = ::GlobalAlloc( GMEM_MOVEABLE | GMEM_NODISCARD | GMEM_SHARE, dataSize );

	LPVOID pMem = ::GlobalLock( hMem );

	void * data = mSource->mBuffer.getData();
	::CopyMemory( pMem, data, dataSize );
	::GlobalUnlock( hMem );

	hr = ::CreateStreamOnHGlobal( hMem, TRUE, &pStream );
	if(FAILED(hr)) {
		//todo: throw
		return;
	}

	//hr = mReaderAdvanced2->OpenStream( pStream, &mCallback, this );
	hr = mReader->OpenStream( pStream/*, &mCallback, this*/ );
	if(FAILED(hr)) {
		//todo: throw
		return;
	}
	
	//wait for file to open
	//::WaitForSingleObject( mOpenEvent, INFINITE );
	
	//ensure that there is only one stream in the file
	uint32_t nOutputCount;
	hr = mReader->GetOutputCount( &nOutputCount );
	if( FAILED(hr) ) {
		//throw
		return;
	}
	
	//ensure this is audio only
	if( nOutputCount != 1 ) {
		//throw
		return;
	}

	uint32_t nOutputFormatCount;
	hr = mReader->GetOutputFormatCount(0, &nOutputFormatCount);
	if(FAILED(hr)) {
		//TODO: throw
		return;
	}
	
	uint32_t nFormatSize = 0; 
	BYTE* pBuf = 0;
	IWMOutputMediaProps* pProps = 0;
	for( uint32_t j = 0; j < nOutputFormatCount; j++ ) {
		hr = mReader->GetOutputFormat( 0, j, &pProps  );
		if(FAILED(hr)) {
			continue;
		}

		//get required size of the media type structure
		uint32_t nNewSize = 0;
		hr = pProps->GetMediaType( NULL, & nNewSize );
		if( FAILED(hr) ) {
			continue;
		}

		if(nNewSize > nFormatSize)
		{
			if( pBuf ) {
				delete [] pBuf;
				pBuf = 0;
			}

			nFormatSize = nNewSize;
			pBuf = new BYTE[nFormatSize];
		}

		WM_MEDIA_TYPE* pType = (WM_MEDIA_TYPE*) pBuf;
		size_t acrunk = sizeof( WM_MEDIA_TYPE );
		hr = pProps->GetMediaType( pType, &nFormatSize );
		if(FAILED(hr))
			continue;
		
		if( IsEqualIID( WMFORMAT_WaveFormatEx, pType->formattype ) )
		{
			//casting format buffer as WAVEFORMATEX
			//when formattype == WMFORMAT_WaveFormatEx, pbFormat will be
			//WAVEFORMATEX or WAVEFORMATEXTENSIBLE, both of which can be cast
			//safely as WAVEFORMATEX
			mOutputFormat = *((WAVEFORMATEX *)pType->pbFormat);

			if( ( mOutputFormat.wFormatTag == ( ( target->isFloat() ) ? WAVE_FORMAT_IEEE_FLOAT : WAVE_FORMAT_PCM ) ) &&
				( mOutputFormat.nChannels == target->getChannelCount() ) &&
				( mOutputFormat.wBitsPerSample == target->getBitsPerSample() ) &&
				( mOutputFormat.nSamplesPerSec == target->getSampleRate() ) &&
				( mOutputFormat.nBlockAlign == target->getBlockAlign() )
			) {
				mSrcSampleRate = mOutputFormat.nSamplesPerSec;
				mSrcChannelCount = mOutputFormat.nChannels;
				mSrcBitsPerSample = mOutputFormat.wBitsPerSample;
				mSrcBlockAlign = mOutputFormat.nBlockAlign;
				break;
			}
		}
		if( pProps ) {
			pProps->Release();
			pProps = 0;
		}


	}
	if( pBuf ) {
		delete [] pBuf;
		pBuf = 0;
	}

	hr = mReader->SetOutputProps( 0, pProps );
	if( FAILED( hr ) ) {
		//TODO: handle error
		return;
	}

	if( pProps ) {
		pProps->Release();
		pProps = 0;
	}

	WORD wStreamNum = 0;
	WMT_ATTR_DATATYPE Type;
	QWORD dwDuration = 0;
	WORD wLength = 8;
	hr = mHeaderInfo->GetAttributeByName( &wStreamNum, g_wszWMDuration, &Type, (BYTE*)&dwDuration, &wLength );
	if( FAILED( hr ) ) {
		//TODO: handle error
		return;
	}

	//divide by 10 million to get seconds
	double fTime = double( dwDuration ) / 10000000.0f;

	mStreamSize = (uint32_t)( fTime * mOutputFormat.nAvgBytesPerSec * 1.5 );
	
	mMaxBufferSize = 0;
	hr = mReader->GetMaxOutputSampleSize( 0, &mMaxBufferSize );
	if( FAILED( hr ) ) {
		//todo: throw
		return;
	}

	//set data to not be compressed
	hr = mReader->SetReadStreamSamples( 0, FALSE );
	if( FAILED( hr ) ) {
		//todo: throw
		return;
	}
}

LoaderSourceFileWindowsMedia::~LoaderSourceFileWindowsMedia()
{
	//clean everything up
	mHeaderInfo->Release();
	mReader->Release();
}

uint64_t LoaderSourceFileWindowsMedia::getSampleOffset() const
{
	return mSampleOffset;
}

void LoaderSourceFileWindowsMedia::setSampleOffset( uint64_t anOffset )
{
	mSampleOffset = anOffset;
	mReader->SetRange( timeAtSample( mSampleOffset ), 0 );
}

void LoaderSourceFileWindowsMedia::loadData( uint32_t *ioSampleCount, BufferList *ioData )
{	
	HRESULT hr;
	INSSBuffer * outBuffer;
	QWORD pcnsSampleTime;
	QWORD pcnsDuration;
	DWORD pdwFlags;
	DWORD pdwOutputNum;
	WORD pwStreamNum;

	hr = mReader->GetNextSample( 0, &outBuffer, &pcnsSampleTime, &pcnsDuration, &pdwFlags, &pdwOutputNum, &pwStreamNum );
	if( hr == NS_E_NO_MORE_SAMPLES ) {
		*ioSampleCount = 0;
		return;
	}

	DWORD bufferLen;
	BYTE * rawBuffer = 0;
	hr = outBuffer->GetBufferAndLength( &rawBuffer, &bufferLen );
	if( hr != S_OK ) {
		//TODO
	}

	uint32_t bufferSize = bufferLen;
	if( bufferSize > ioData->mBuffers[0].mDataByteSize ) {
		bufferSize = ioData->mBuffers[0].mDataByteSize;
	}

	memcpy( ioData->mBuffers[0].mData, rawBuffer, bufferSize );
	ioData->mBuffers[0].mDataByteSize = bufferSize;
	*ioSampleCount = bufferSize / mSrcBlockAlign;
	mSampleOffset += *ioSampleCount;
	

//	LONGLONG duration = (*ioSampleCount / mSrcChannelCount ) / (double)mSrcSampleRate * 10000000L;
	//hr = mReader->SetRange( mTimeOffset, 0 ); //TODO, just implement this in seeking
}

uint64_t LoaderSourceFileWindowsMedia::timeAtSample( uint64_t aSample ) const
{
	return aSample / (double)mSrcSampleRate * 10000000L;
}

uint64_t LoaderSourceFileWindowsMedia::sampleAtTime( uint64_t aTime) const
{
	return aTime / 10000000.0 * mSrcSampleRate;
}

void SourceFileWindowsMedia::registerSelf()
{
	static const int32_t SOURCE_PRIORITY = 2;
	
	IoRegistrar::SourceCreationFunc sourceFunc = SourceFileWindowsMedia::createRef;
	
	//TODO: find a way to enumerate Windows Media supported file formats
	IoRegistrar::registerSourceType( "mp3", sourceFunc, SOURCE_PRIORITY );
	IoRegistrar::registerSourceType( "wma", sourceFunc, SOURCE_PRIORITY );
}

SourceFileWindowsMediaRef	SourceFileWindowsMedia::createFileRef( DataSourceRef dataSourceRef )
{
	return SourceFileWindowsMediaRef( new SourceFileWindowsMedia( dataSourceRef ) );
}

SourceFileWindowsMedia::SourceFileWindowsMedia( DataSourceRef dataSourceRef )
	: Source()
{
	mDataType = DATA_UNKNOWN;
	mSampleRate = 0;
	mChannelCount = 0;
	mBitsPerSample = 0;
	mBlockAlign = 0;
	mIsPcm = FALSE;
	mIsBigEndian = TRUE;
	mIsInterleaved = FALSE;
	/*if( dataSourceRef->isFilePath() ) {
		//TODO: implement
	} else if ( dataSourceRef->isFilePath() ) {
		//TODO: implement WindowsMedia network functionallity
	}else { //have to use buffer
		//TODO: move current implementation into this
	}*/
	
	mBuffer = dataSourceRef->getBuffer();


}

SourceFileWindowsMedia::~SourceFileWindowsMedia()
{
	
}

}} //namespace