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
void ImageSourceFileWic::registerSelf()
{
	const int32_t SOURCE_PRIORITY = 2;

	// there doesn't seem to be a way to iterate supported types on WIC, so this list was constructed
	// based on https://msdn.microsoft.com/en-us/library/windows/desktop/gg430027(v=vs.85).aspx "Native WIC Codecs"
	ImageIoRegistrar::registerSourceType( "bmp", ImageSourceFileWic::create, SOURCE_PRIORITY ); ImageIoRegistrar::registerSourceType( "dib", ImageSourceFileWic::create, SOURCE_PRIORITY );
	ImageIoRegistrar::registerSourceType( "dds", ImageSourceFileWic::create, SOURCE_PRIORITY );
	ImageIoRegistrar::registerSourceType( "gif", ImageSourceFileWic::create, SOURCE_PRIORITY );
	ImageIoRegistrar::registerSourceType( "wdp", ImageSourceFileWic::create, SOURCE_PRIORITY ); // HD Photo Format
	ImageIoRegistrar::registerSourceType( "ico", ImageSourceFileWic::create, SOURCE_PRIORITY );
	ImageIoRegistrar::registerSourceType( "jpe", ImageSourceFileWic::create, SOURCE_PRIORITY ); ImageIoRegistrar::registerSourceType( "jpg", ImageSourceFileWic::create, SOURCE_PRIORITY );
		ImageIoRegistrar::registerSourceType( "jpeg", ImageSourceFileWic::create, SOURCE_PRIORITY );
	ImageIoRegistrar::registerSourceType( "jxr", ImageSourceFileWic::create, SOURCE_PRIORITY ); // JPEG XR
	ImageIoRegistrar::registerSourceType( "png", ImageSourceFileWic::create, SOURCE_PRIORITY );
	ImageIoRegistrar::registerSourceType( "tif", ImageSourceFileWic::create, SOURCE_PRIORITY ); ImageIoRegistrar::registerSourceType( "tiff", ImageSourceFileWic::create, SOURCE_PRIORITY );

	ImageIoRegistrar::registerSourceGeneric( ImageSourceFileWic::create, SOURCE_PRIORITY );
}

IWICImagingFactory* ImageSourceFileWic::getFactory()
{
	static IWICImagingFactory *sIWICFactory = []() {
		IWICImagingFactory *result = NULL;
#if defined(CLSID_WICImagingFactory1)
		::HRESULT hr = ::CoCreateInstance( CLSID_WICImagingFactory1, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&result) );
#else
		::HRESULT hr = ::CoCreateInstance( CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&result) );
#endif
		if( ! SUCCEEDED( hr ) )
			throw ImageIoException( "Could not create WIC Image Factory." );

		return result;
	}();

	return sIWICFactory;
}

ImageSourceRef ImageSourceFileWic::create( DataSourceRef dataSourceRef, ImageSource::Options options )
{
	return ImageSourceRef( new ImageSourceFileWic( dataSourceRef, options ) );
}

ImageSourceFileWic::ImageSourceFileWic( DataSourceRef dataSourceRef, ImageSource::Options options )
	: ImageSource()
{
	::HRESULT hr = S_OK;

	// Initialize COM
	msw::initializeCom();
	
    // Create WIC factory
	IWICImagingFactory* factory = getFactory();
	
    // Create a decoder
	IWICBitmapDecoder *decoderP = NULL;
#if defined( CINDER_UWP )
		std::string s = dataSourceRef->getFilePath().string();
		std::wstring filePath =	std::wstring(s.begin(), s.end());                 
#else
		std::wstring filePath =	dataSourceRef->getFilePath().wstring().c_str();
#endif

	if( dataSourceRef->isFilePath() ) {
		hr = factory->CreateDecoderFromFilename(
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
		hr = factory->CreateStream( &pIWICStream );
		if( ! SUCCEEDED(hr) )
			throw ImageIoExceptionFailedLoad( "Could not create WIC Stream." );
		mStream = msw::makeComShared( pIWICStream );
		
		BufferRef buffer = dataSourceRef->getBuffer();
		hr = mStream->InitializeFromMemory( reinterpret_cast<BYTE*>( buffer->getData() ), buffer->getSize() );
		if( ! SUCCEEDED(hr) )
			throw ImageIoExceptionFailedLoad( "Could not initialize WIC Stream." );
		
		hr = factory->CreateDecoderFromStream( mStream.get(), NULL, WICDecodeMetadataCacheOnDemand, &decoderP );
		if( ! SUCCEEDED(hr) )
			throw ImageIoExceptionFailedLoad( "Could not create WIC Decoder from stream." );
	}
	std::shared_ptr<IWICBitmapDecoder> decoder = msw::makeComShared( decoderP );

	// Parse # of frames
	UINT frameCount = 1;
	hr = decoder->GetFrameCount( &frameCount );
	if( ! SUCCEEDED(hr) )
		throw ImageIoExceptionFailedLoad( "Could not retrieve frame count from WIC Decoder." );
	else
		setFrameCount( frameCount );

    // Retrieve the 'index' frame of the image from the decoder
	IWICBitmapFrameDecode *frameP = NULL;
	hr = decoder->GetFrame( options.getIndex(), &frameP );
	if( ! SUCCEEDED(hr) )
		throw ImageIoExceptionFailedLoad( "Could not retrieve index frame from WIC Decoder." );
	mFrame = msw::makeComShared( frameP );

	UINT width = 0, height = 0;
	mFrame->GetSize( &width, &height );
	mWidth = width; mHeight = height;
	
	hr = mFrame->GetPixelFormat( &mPixelFormat );
	if( ! SUCCEEDED(hr) )
		throw ImageIoExceptionFailedLoad( "Could not retrieve pixel format from WIC Decoder." );
	
	mRequiresConversion = processFormat( mPixelFormat, &mConvertPixelFormat );
	mRowBytes = mWidth * ImageIo::dataTypeBytes( mDataType ) * channelOrderNumChannels( mChannelOrder );
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
		setChannelOrder( ImageIo::BGR ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 );
	}
	else if( guid == GUID_WICPixelFormat24bppRGB ) {
		setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 );
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
	else if( guid == GUID_WICPixelFormat32bppCMYK || guid == GUID_WICPixelFormat64bppCMYK || guid == GUID_WICPixelFormat40bppCMYKAlpha || guid == GUID_WICPixelFormat80bppCMYKAlpha ) {
		throw ImageIoExceptionIllegalColorModel( "CMYK pixel format not supported." );
	}
	else
		throw ImageIoException( "Unsupported format." );

	return false;
}

void ImageSourceFileWic::load( ImageTargetRef target )
{
	// get a pointer to the ImageSource function appropriate for handling our data configuration
	ImageSource::RowFunc func = setupRowFunc( target );

	std::unique_ptr<uint8_t[]> data( new uint8_t[mRowBytes * mHeight] );

	if( mRequiresConversion ) {
		IWICFormatConverter *pIFormatConverter = NULL;	
		::HRESULT hr = getFactory()->CreateFormatConverter( &pIFormatConverter );
		if( ! SUCCEEDED( hr ) )
			throw ImageIoExceptionFailedLoad( "Could not create WIC Format Converter." );
		std::shared_ptr<IWICFormatConverter> formatConverter = msw::makeComShared( pIFormatConverter );
		hr = formatConverter->Initialize( mFrame.get(), mConvertPixelFormat, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeCustom );
		if( ! SUCCEEDED( hr ) )
			throw ImageIoExceptionFailedLoad( "Could not initialize WIC Format Converter." );
		hr = formatConverter->CopyPixels( NULL, (UINT)mRowBytes, mRowBytes * mHeight, data.get() );
	}
	else
		mFrame->CopyPixels( NULL, (UINT)mRowBytes, mRowBytes * mHeight, data.get() );
	
	const uint8_t *dataPtr = data.get();
	for( int32_t row = 0; row < mHeight; ++row ) {
		((*this).*func)( target, row, dataPtr );
		dataPtr += mRowBytes;
	}
}

} // namespace ci