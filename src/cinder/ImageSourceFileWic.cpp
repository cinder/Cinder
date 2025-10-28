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

#include <set>

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
	#include <D2D1.h>
#endif
#include <wincodec.h>
#include <wincodecsdk.h>
#pragma comment( lib, "WindowsCodecs.lib" )

// Define GUIDs that may be missing in older Windows SDKs
#ifndef GUID_WICPixelFormat32bppRGB
DEFINE_GUID(GUID_WICPixelFormat32bppRGB, 0xd98c6b95, 0x3efe, 0x47d6, 0xbb, 0x25, 0xeb, 0x17, 0x48, 0xab, 0x0c, 0xf1);
#endif
#ifndef GUID_WICPixelFormat64bppPRGBAHalf
DEFINE_GUID(GUID_WICPixelFormat64bppPRGBAHalf, 0x58ad26c2, 0xc623, 0x4d9d, 0xb3, 0x20, 0x38, 0x7e, 0x49, 0xf8, 0xc4, 0x42);
#endif
#ifndef GUID_WICPixelFormat96bppRGBFloat
DEFINE_GUID(GUID_WICPixelFormat96bppRGBFloat, 0xe3fed78f, 0xe8db, 0x4acf, 0x84, 0xc1, 0xe9, 0x7f, 0x61, 0x36, 0xb3, 0x27);
#endif

namespace cinder {

///////////////////////////////////////////////////////////////////////////////
// ImageSourceFileWic
void ImageSourceFileWic::registerSelf()
{
	const int32_t SOURCE_PRIORITY = 2;

	try {
		msw::initializeCom();

		msw::ComPtr<IWICImagingFactory> factory;
		::HRESULT hr = ::CoCreateInstance( CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**)factory.releaseAndGetAddressOf() );
		if( ! SUCCEEDED(hr) )
			return;

		msw::ComPtr<IEnumUnknown> enumerator;
		hr = factory->CreateComponentEnumerator( WICDecoder, WICComponentEnumerateDefault, enumerator.releaseAndGetAddressOf() );
		if( ! SUCCEEDED(hr) )
			return;

		// Track which extensions we've registered
		std::set<std::string> registered;
		ImageIoRegistrar::SourceCreationFunc sourceFunc = ImageSourceFileWic::create;

		for( UINT i = 0; ; ++i ) {
			msw::ComPtr<IUnknown> pUnknown;
			ULONG fetched = 0;
			hr = enumerator->Next( 1, pUnknown.releaseAndGetAddressOf(), &fetched );
			if( hr != S_OK || fetched == 0 )
				break;

			msw::ComPtr<IWICBitmapCodecInfo> codecInfo;
			hr = pUnknown->QueryInterface( IID_IWICBitmapCodecInfo, (void**)codecInfo.releaseAndGetAddressOf() );

			if( ! SUCCEEDED(hr) || !codecInfo )
				continue;

			UINT length = 0;
			hr = codecInfo->GetFileExtensions( 0, NULL, &length );
			if( SUCCEEDED(hr) && length > 0 ) {
				std::vector<wchar_t> buffer( length );
				hr = codecInfo->GetFileExtensions( length, buffer.data(), &length );
				if( SUCCEEDED(hr) ) {
					std::wstring extString( buffer.data() );

					// Extensions are comma-separated, e.g., ".jpg,.jpeg,.jpe"
					size_t start = 0;
					while( start < extString.length() ) {
						size_t comma = extString.find( L',', start );
						if( comma == std::wstring::npos )
							comma = extString.length();

						std::wstring ext = extString.substr( start, comma - start );
						// Remove leading dot if present
						if( ! ext.empty() && ext[0] == L'.' )
							ext = ext.substr( 1 );

						if( ! ext.empty() ) {
							std::string narrowExt = msw::toUtf8String( ext );

							// Register each extension only once
							if( registered.find( narrowExt ) == registered.end() ) {
								ImageIoRegistrar::registerSourceType( narrowExt, sourceFunc, SOURCE_PRIORITY );
								registered.insert( narrowExt );
							}
						}

						start = comma + 1;
					}
				}
			}
		}
	}
	catch( ... ) {
		// If enumeration fails, just return without registering anything
	}
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
	msw::ComPtr<IWICBitmapDecoder> decoder;
	std::wstring filePath =	dataSourceRef->getFilePath().wstring().c_str();

	if( dataSourceRef->isFilePath() ) {
		hr = factory->CreateDecoderFromFilename(
				filePath.c_str(),                      // Image to be decoded
//				dataSourceRef->getFilePath().wstring().c_str(),                      // Image to be decoded
				NULL,                            // Do not prefer a particular vendor
				GENERIC_READ,                    // Desired read access to the file
				WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
				decoder.releaseAndGetAddressOf()                         // Pointer to the decoder
			);
		if( ! SUCCEEDED(hr) )
			throw ImageIoExceptionFailedLoad( "Could not create WIC Decoder from filename." );

	}
	else { // have to use a buffer
		hr = factory->CreateStream( mStream.releaseAndGetAddressOf() );
		if( ! SUCCEEDED(hr) )
			throw ImageIoExceptionFailedLoad( "Could not create WIC Stream." );

		mBuffer = dataSourceRef->getBuffer();
		hr = mStream->InitializeFromMemory( reinterpret_cast<BYTE*>( mBuffer->getData() ), static_cast<DWORD>( mBuffer->getSize() ) );
		if( ! SUCCEEDED(hr) )
			throw ImageIoExceptionFailedLoad( "Could not initialize WIC Stream." );

		hr = factory->CreateDecoderFromStream( mStream.get(), NULL, WICDecodeMetadataCacheOnDemand, decoder.releaseAndGetAddressOf() );
		if( ! SUCCEEDED(hr) )
			throw ImageIoExceptionFailedLoad( "Could not create WIC Decoder from stream." );
	}

	// Parse # of frames
	UINT frameCount = 1;
	hr = decoder->GetFrameCount( &frameCount );
	if( ! SUCCEEDED(hr) )
		throw ImageIoExceptionFailedLoad( "Could not retrieve frame count from WIC Decoder." );
	else
		setFrameCount( frameCount );

    // Retrieve the 'index' frame of the image from the decoder
	hr = decoder->GetFrame( options.getIndex(), mFrame.releaseAndGetAddressOf() );
	if( ! SUCCEEDED(hr) )
		throw ImageIoExceptionFailedLoad( "Could not retrieve index frame from WIC Decoder." );

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
	else if( guid == GUID_WICPixelFormatBlackWhite || guid == GUID_WICPixelFormat2bppGray || guid == GUID_WICPixelFormat4bppGray ) {
		// Convert low bit-depth grayscale to 8-bit grayscale
		setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::UINT8 ); *convertGUID = GUID_WICPixelFormat8bppGray;
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
	else if( guid == GUID_WICPixelFormat128bppRGBAFloat ) {
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 );
	}
	else if( guid == GUID_WICPixelFormat128bppPRGBAFloat ) {
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); setPremultiplied( true );
	}
	else if( guid == GUID_WICPixelFormat8bppGray ) {
		setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::UINT8 );
	}
	else if( guid == GUID_WICPixelFormat16bppGray ) {
		setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::UINT16 );
	}
	else if( guid == GUID_WICPixelFormat16bppGrayFixedPoint ) {
		// Fixed point is similar to UINT16 for our purposes
		setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::UINT16 );
	}
	else if( guid == GUID_WICPixelFormat16bppGrayHalf ) {
		setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT16 );
	}
	else if( guid == GUID_WICPixelFormat32bppGrayFloat ) {
		setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT32 );
	}
	else if( guid == GUID_WICPixelFormat32bppGrayFixedPoint ) {
		// Convert fixed point to float for consistency
		setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT32 ); *convertGUID = GUID_WICPixelFormat32bppGrayFloat;
		return true;
	}
	// RGB/BGR variants and conversions
	else if( guid == GUID_WICPixelFormat32bppRGB ) {
		setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 );
	}
	else if( guid == GUID_WICPixelFormat32bppRGBA ) {
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 );
	}
	else if( guid == GUID_WICPixelFormat32bppPRGBA ) {
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); setPremultiplied( true );
	}
	else if( guid == GUID_WICPixelFormat48bppBGR ) {
		setChannelOrder( ImageIo::BGR ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT16 );
	}
	else if( guid == GUID_WICPixelFormat64bppBGRA ) {
		setChannelOrder( ImageIo::BGRA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT16 );
	}
	else if( guid == GUID_WICPixelFormat64bppPBGRA ) {
		setChannelOrder( ImageIo::BGRA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT16 ); setPremultiplied( true );
	}
	// Fixed-point RGB formats - convert to float to preserve precision
	else if( guid == GUID_WICPixelFormat48bppRGBFixedPoint ) {
		// Fixed-point has fractional precision - convert to float
		setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); *convertGUID = GUID_WICPixelFormat128bppRGBFloat;
		return true;
	}
	else if( guid == GUID_WICPixelFormat48bppBGRFixedPoint ) {
		// Convert BGR fixed-point to RGB float
		setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); *convertGUID = GUID_WICPixelFormat128bppRGBFloat;
		return true;
	}
	else if( guid == GUID_WICPixelFormat64bppRGBAFixedPoint ) {
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); *convertGUID = GUID_WICPixelFormat128bppRGBAFloat;
		return true;
	}
	else if( guid == GUID_WICPixelFormat64bppBGRAFixedPoint ) {
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); *convertGUID = GUID_WICPixelFormat128bppRGBAFloat;
		return true;
	}
	else if( guid == GUID_WICPixelFormat64bppRGBFixedPoint ) {
		setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); *convertGUID = GUID_WICPixelFormat128bppRGBFloat;
		return true;
	}
	else if( guid == GUID_WICPixelFormat96bppRGBFixedPoint ) {
		// Convert to float
		setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); *convertGUID = GUID_WICPixelFormat128bppRGBFloat;
		return true;
	}
	else if( guid == GUID_WICPixelFormat128bppRGBAFixedPoint ) {
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); *convertGUID = GUID_WICPixelFormat128bppRGBAFloat;
		return true;
	}
	else if( guid == GUID_WICPixelFormat128bppRGBFixedPoint ) {
		setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); *convertGUID = GUID_WICPixelFormat128bppRGBFloat;
		return true;
	}
	// Half-float RGB formats
	else if( guid == GUID_WICPixelFormat48bppRGBHalf ) {
		setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT16 );
	}
	else if( guid == GUID_WICPixelFormat64bppRGBAHalf ) {
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT16 );
	}
	else if( guid == GUID_WICPixelFormat64bppPRGBAHalf ) {
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT16 ); setPremultiplied( true );
	}
	else if( guid == GUID_WICPixelFormat64bppRGBHalf ) {
		// Convert to 48bpp RGB half
		setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT16 ); *convertGUID = GUID_WICPixelFormat48bppRGBHalf;
		return true;
	}
	// 96bpp and 128bpp float formats
	else if( guid == GUID_WICPixelFormat96bppRGBFloat ) {
		// Convert to 128bpp RGB float (adds padding channel)
		setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); *convertGUID = GUID_WICPixelFormat128bppRGBFloat;
		return true;
	}
	// 10-bit and HDR formats - convert to 16-bit or float
	else if( guid == GUID_WICPixelFormat32bppBGR101010 ) {
		// Convert to 48bpp RGB (16-bit per channel)
		setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT16 ); *convertGUID = GUID_WICPixelFormat48bppRGB;
		return true;
	}
	else if( guid == GUID_WICPixelFormat32bppRGBA1010102 || guid == GUID_WICPixelFormat32bppRGBA1010102XR ||
	         guid == GUID_WICPixelFormat32bppR10G10B10A2 || guid == GUID_WICPixelFormat32bppR10G10B10A2HDR10 ) {
		// Convert 10-bit formats to 64bpp RGBA
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT16 ); *convertGUID = GUID_WICPixelFormat64bppRGBA;
		return true;
	}
	else if( guid == GUID_WICPixelFormat32bppRGBE ) {
		// RGBE (shared exponent) - convert to float
		setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); *convertGUID = GUID_WICPixelFormat128bppRGBFloat;
		return true;
	}
	// 16bpp BGRA5551 format
	else if( guid == GUID_WICPixelFormat16bppBGRA5551 ) {
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); *convertGUID = GUID_WICPixelFormat32bppRGBA;
		return true;
	}
	// Multi-channel formats (3-8 channels) - convert to RGBA
	else if( guid == GUID_WICPixelFormat24bpp3Channels || guid == GUID_WICPixelFormat48bpp3Channels ) {
		// 3-channel formats - treat as RGB
		setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); *convertGUID = GUID_WICPixelFormat24bppRGB;
		return true;
	}
	else if( guid == GUID_WICPixelFormat32bpp4Channels || guid == GUID_WICPixelFormat64bpp4Channels ) {
		// 4-channel formats - convert to RGBA
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); *convertGUID = GUID_WICPixelFormat32bppRGBA;
		return true;
	}
	else if( guid == GUID_WICPixelFormat40bpp5Channels || guid == GUID_WICPixelFormat80bpp5Channels ||
	         guid == GUID_WICPixelFormat48bpp6Channels || guid == GUID_WICPixelFormat96bpp6Channels ||
	         guid == GUID_WICPixelFormat56bpp7Channels || guid == GUID_WICPixelFormat112bpp7Channels ||
	         guid == GUID_WICPixelFormat64bpp8Channels || guid == GUID_WICPixelFormat128bpp8Channels ) {
		// Multi-channel formats (5-8 channels) - convert to RGBA
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); *convertGUID = GUID_WICPixelFormat32bppRGBA;
		return true;
	}
	// Multi-channel with alpha formats - convert to RGBA
	else if( guid == GUID_WICPixelFormat32bpp3ChannelsAlpha || guid == GUID_WICPixelFormat64bpp3ChannelsAlpha ||
	         guid == GUID_WICPixelFormat40bpp4ChannelsAlpha || guid == GUID_WICPixelFormat80bpp4ChannelsAlpha ||
	         guid == GUID_WICPixelFormat48bpp5ChannelsAlpha || guid == GUID_WICPixelFormat96bpp5ChannelsAlpha ||
	         guid == GUID_WICPixelFormat56bpp6ChannelsAlpha || guid == GUID_WICPixelFormat112bpp6ChannelsAlpha ||
	         guid == GUID_WICPixelFormat64bpp7ChannelsAlpha || guid == GUID_WICPixelFormat128bpp7ChannelsAlpha ||
	         guid == GUID_WICPixelFormat72bpp8ChannelsAlpha || guid == GUID_WICPixelFormat144bpp8ChannelsAlpha ) {
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); *convertGUID = GUID_WICPixelFormat32bppRGBA;
		return true;
	}
	// YUV/YCbCr formats - convert to RGB
	else if( guid == GUID_WICPixelFormat8bppY ) {
		// Y channel only - treat as grayscale
		setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::UINT8 ); *convertGUID = GUID_WICPixelFormat8bppGray;
		return true;
	}
	else if( guid == GUID_WICPixelFormat8bppCb || guid == GUID_WICPixelFormat8bppCr || guid == GUID_WICPixelFormat16bppCbCr ) {
		// Chroma channels - convert to RGB
		setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); *convertGUID = GUID_WICPixelFormat24bppRGB;
		return true;
	}
	// CMYK formats - convert to RGB
	else if( guid == GUID_WICPixelFormat32bppCMYK ) {
		// Convert CMYK to RGB
		setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); *convertGUID = GUID_WICPixelFormat24bppRGB;
		return true;
	}
	else if( guid == GUID_WICPixelFormat64bppCMYK ) {
		// Convert 64bpp CMYK to 48bpp RGB
		setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT16 ); *convertGUID = GUID_WICPixelFormat48bppRGB;
		return true;
	}
	else if( guid == GUID_WICPixelFormat40bppCMYKAlpha ) {
		// Convert CMYK+Alpha to RGBA
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); *convertGUID = GUID_WICPixelFormat32bppRGBA;
		return true;
	}
	else if( guid == GUID_WICPixelFormat80bppCMYKAlpha ) {
		// Convert 80bpp CMYK+Alpha to 64bpp RGBA
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT16 ); *convertGUID = GUID_WICPixelFormat64bppRGBA;
		return true;
	}
	// Unknown format - try to convert to RGBA as fallback
	else {
		// Last resort - request conversion to 32bppRGBA
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); *convertGUID = GUID_WICPixelFormat32bppRGBA;
		return true;
	}

	return false;
}

void ImageSourceFileWic::load( ImageTargetRef target )
{
	// get a pointer to the ImageSource function appropriate for handling our data configuration
	ImageSource::RowFunc func = setupRowFunc( target );

	std::unique_ptr<uint8_t[]> data( new uint8_t[mRowBytes * mHeight] );

	if( mRequiresConversion ) {
		msw::ComPtr<IWICFormatConverter> formatConverter;
		::HRESULT hr = getFactory()->CreateFormatConverter( formatConverter.releaseAndGetAddressOf() );
		if( ! SUCCEEDED( hr ) )
			throw ImageIoExceptionFailedLoad( "Could not create WIC Format Converter." );
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