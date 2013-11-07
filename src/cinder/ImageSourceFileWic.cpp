/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

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

#include "cinder/ImageSourceFileWic.h"
#include "cinder/Utilities.h"
#include "cinder/msw/CinderMsw.h"

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
	#include <D2D1.h>
#endif
#include <wincodec.h>
#include <wincodecsdk.h>
#pragma comment( lib, "WindowsCodecs.lib" )

namespace cinder {

///////////////////////////////////////////////////////////////////////////////
// ImageSourceFileWic
ImageSourceFileWicRef ImageSourceFileWic::createFileWicRef( DataSourceRef dataSourceRef, ImageSource::Options options )
{
	return ImageSourceFileWicRef( new ImageSourceFileWic( dataSourceRef, options ) );
}

ImageSourceFileWic::ImageSourceFileWic( DataSourceRef dataSourceRef, ImageSource::Options options )
	: ImageSource()
{
	::HRESULT hr = S_OK;

	// Initialize COM
	msw::initializeCom();
	
    // Create WIC factory
    IWICImagingFactory *IWICFactoryP = NULL;
#if defined(CLSID_WICImagingFactory1)
	hr = ::CoCreateInstance( CLSID_WICImagingFactory1, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&IWICFactoryP) );
#else
	hr = ::CoCreateInstance( CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&IWICFactoryP) );
#endif
	if( ! SUCCEEDED( hr ) )
		throw ImageIoException( "Could not create WIC Image Factory." );

	std::shared_ptr<IWICImagingFactory> IWICFactory = msw::makeComShared( IWICFactoryP );
	
    // Create a decoder
	IWICBitmapDecoder *decoderP = NULL;
#if defined( CINDER_WINRT)
		std::string s = dataSourceRef->getFilePath().string();
		std::wstring filePath =	std::wstring(s.begin(), s.end());                 
#else
		std::wstring filePath =	dataSourceRef->getFilePath().wstring().c_str();
#endif

	if( dataSourceRef->isFilePath() ) {
		hr = IWICFactory->CreateDecoderFromFilename(
				filePath.c_str(),                      // Image to be decoded
//				dataSourceRef->getFilePath().wstring().c_str(),                      // Image to be decoded
				NULL,                            // Do not prefer a particular vendor
				GENERIC_READ,                    // Desired read access to the file
				WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
				&decoderP                        // Pointer to the decoder
			);
		if( ! SUCCEEDED(hr) )
			throw ImageIoExceptionFailedLoad( "Could not create WIC Decoder from filename." );

	}
	else { // have to use a buffer
		IWICStream *pIWICStream = NULL;
		hr = IWICFactory->CreateStream( &pIWICStream );
		if( ! SUCCEEDED(hr) )
			throw ImageIoExceptionFailedLoad( "Could not create WIC Stream." );
		std::shared_ptr<IWICStream> stream = msw::makeComShared( pIWICStream );
		
		Buffer buffer = dataSourceRef->getBuffer();
		hr = stream->InitializeFromMemory( reinterpret_cast<BYTE*>( buffer.getData() ), buffer.getDataSize() );
		if( ! SUCCEEDED(hr) )
			throw ImageIoExceptionFailedLoad( "Could not initialize WIC Stream." );
		
		hr = IWICFactory->CreateDecoderFromStream( stream.get(), NULL, WICDecodeMetadataCacheOnDemand, &decoderP );
		if( ! SUCCEEDED(hr) )
			throw ImageIoExceptionFailedLoad( "Could not create WIC Decoder from stream." );
	}
	std::shared_ptr<IWICBitmapDecoder> decoder = msw::makeComShared( decoderP );

    // Retrieve the 'index' frame of the image from the decoder
	IWICBitmapFrameDecode *frameP = NULL;
	hr = decoder->GetFrame( options.getIndex(), &frameP );
	if( ! SUCCEEDED(hr) )
		throw ImageIoExceptionFailedLoad( "Could not retrieve index frame from WIC Decoder." );
	std::shared_ptr<IWICBitmapFrameDecode> frame = msw::makeComShared( frameP );

	UINT width = 0, height = 0;
	frame->GetSize( &width, &height );
	mWidth = width; mHeight = height;
	
	GUID pixelFormat = { 0 }, convertPixelFormat;
	frame->GetPixelFormat( &pixelFormat );
	
	bool requiresConversion = processFormat( pixelFormat, &convertPixelFormat );
	mRowBytes = mWidth * ImageIo::dataTypeBytes( mDataType ) * channelOrderNumChannels( mChannelOrder );

	mData = std::shared_ptr<uint8_t>( new uint8_t[mRowBytes * mHeight], boost::checked_array_delete<uint8_t> );

	if( requiresConversion ) {
		IWICFormatConverter *pIFormatConverter = NULL;	
		hr = IWICFactory->CreateFormatConverter( &pIFormatConverter );
		if( ! SUCCEEDED( hr ) )
			throw ImageIoExceptionFailedLoad( "Could not create WIC Format Converter." );
		std::shared_ptr<IWICFormatConverter> formatConverter = msw::makeComShared( pIFormatConverter );
		hr = formatConverter->Initialize( frame.get(), convertPixelFormat, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeCustom );
		if( ! SUCCEEDED( hr ) )
			throw ImageIoExceptionFailedLoad( "Could not initialize WIC Format Converter." );
		hr = formatConverter->CopyPixels( NULL, (UINT)mRowBytes, mRowBytes * mHeight, mData.get() );
	}
	else
		hr = frame->CopyPixels( NULL, (UINT)mRowBytes, mRowBytes * mHeight, mData.get() );
}

// returns true if we need conversion
bool ImageSourceFileWic::processFormat( const ::GUID &guid, ::GUID *convertGUID )
{
	if( ( guid == GUID_WICPixelFormat1bppIndexed ) ||
		( guid == GUID_WICPixelFormat2bppIndexed ) || 
		( guid == GUID_WICPixelFormat4bppIndexed ) ||
		( guid == GUID_WICPixelFormat8bppIndexed ) ||
		( guid == GUID_WICPixelFormat16bppBGR555 ) ||
		( guid == GUID_WICPixelFormat16bppBGR565 ) ) {
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); *convertGUID = GUID_WICPixelFormat32bppRGBA;
		return true;
	}
	else if( guid == GUID_WICPixelFormat24bppBGR ) {		
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); *convertGUID = GUID_WICPixelFormat32bppRGBA;
		return true;
	}
	else if( guid == GUID_WICPixelFormat24bppRGB ) {
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); *convertGUID = GUID_WICPixelFormat32bppRGBA;
		return true;
	}
	else if( guid == GUID_WICPixelFormat32bppBGR ) {
		setChannelOrder( ImageIo::BGRX ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 );
	}
	else if( guid == GUID_WICPixelFormat32bppBGRA ) {
		setChannelOrder( ImageIo::BGRA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 );
	}
	else if( guid == GUID_WICPixelFormat32bppPBGRA ) {
		setChannelOrder( ImageIo::BGRA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); setPremultiplied( true );
	}
	else if( guid == GUID_WICPixelFormat48bppRGB ) {
		setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT16 );
	}
	else if( guid == GUID_WICPixelFormat64bppRGBA ) {
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT16 );
	}
	else if( guid == GUID_WICPixelFormat64bppPRGBA ) {
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT16 ); setPremultiplied( true );
	}	
	else if( guid == GUID_WICPixelFormat128bppRGBFloat ) {
		setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 );
	}
	else if( guid == GUID_WICPixelFormat8bppGray ) {
		setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::UINT8 );
	}
	else if( guid == GUID_WICPixelFormat16bppGray ) {
		setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::UINT16 );
	}
	else if( guid == GUID_WICPixelFormat32bppGrayFloat ) {
		setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT32 );
	}
	else
		throw ImageIoException( "Unsupported format." );

	return false;
}

void ImageSourceFileWic::load( ImageTargetRef target )
{
	// get a pointer to the ImageSource function appropriate for handling our data configuration
	ImageSource::RowFunc func = setupRowFunc( target );
	
	const uint8_t *data = mData.get();
	for( int32_t row = 0; row < mHeight; ++row ) {
		((*this).*func)( target, row, data );
		data += mRowBytes;
	}
}

void ImageSourceFileWic::registerSelf()
{
	const int32_t SOURCE_PRIORITY = 2;
	
	ImageIoRegistrar::registerSourceGeneric( ImageSourceFileWic::createRef, SOURCE_PRIORITY );
}

} // namespace ci