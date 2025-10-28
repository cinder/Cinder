/*
 Copyright (c) 2010, The Barbarian Group
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

#include "cinder/ImageTargetFileWic.h"
#include "cinder/Utilities.h"
#include "cinder/msw/CinderMsw.h"

#include <memory>
#include <map>
#include <set>

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
	#include <D2D1.h>
#endif
#include <wincodec.h>
#include <wincodecsdk.h>
#pragma comment( lib, "WindowsCodecs.lib" )

using namespace std;

namespace cinder {

namespace {
// This is only here to avoid a static initialization issue where the map can't be gauranteed to have
// been initialized when ::registerSelf() is called
map<string,const GUID*>& getExtensionMap()
{
	static shared_ptr<map<string,const GUID*> > sInst;
	if( ! sInst ) {
		sInst = shared_ptr<map<string,const GUID*> >( new map<string,const GUID*> );
	}
	return *sInst;
}
} // anonymous namespace

// this map translates the extensionData data we get passed in the constructor into a CLSID for WIC
// this is a pointer to navigate around static init issues
void ImageTargetFileWic::registerSelf()
{
	const int32_t PRIORITY = 2;
	ImageIoRegistrar::TargetCreationFunc func = ImageTargetFileWic::create;

	try {
		msw::initializeCom();

		msw::ComPtr<IWICImagingFactory> factory;
		::HRESULT hr = ::CoCreateInstance( CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**)factory.releaseAndGetAddressOf() );
		if( ! SUCCEEDED(hr) )
			return;

		msw::ComPtr<IEnumUnknown> enumerator;
		hr = factory->CreateComponentEnumerator( WICEncoder, WICComponentEnumerateDefault, enumerator.releaseAndGetAddressOf() );
		if( ! SUCCEEDED(hr) )
			return;

		// Static storage for container format GUIDs to ensure they persist
		static std::vector<std::shared_ptr<GUID>> sContainerFormatGuids;

		// Track which extensions we've registered
		std::set<std::string> registered;

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
			if( SUCCEEDED(hr) && codecInfo ) {

				// Get the container format GUID
				GUID containerFormat;
				hr = codecInfo->GetContainerFormat( &containerFormat );
				if( ! SUCCEEDED(hr) )
					continue;

				UINT length = 0;
				hr = codecInfo->GetFileExtensions( 0, NULL, &length );
				if( SUCCEEDED(hr) && length > 0 ) {
					std::vector<wchar_t> buffer( length );
					hr = codecInfo->GetFileExtensions( length, buffer.data(), &length );
					if( SUCCEEDED(hr) ) {
						std::wstring extString( buffer.data() );

						// Allocate a persistent GUID pointer for this codec
						auto guidPtr = std::make_shared<GUID>( containerFormat );
						sContainerFormatGuids.push_back( guidPtr );

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

								// Map all extension synonyms to the same container format GUID
								getExtensionMap()[narrowExt] = guidPtr.get();

								// Register each extension only once
								if( registered.find( narrowExt ) == registered.end() ) {
									ImageIoRegistrar::registerTargetType( narrowExt, func, PRIORITY, narrowExt );
									registered.insert( narrowExt );
								}
							}

							start = comma + 1;
						}
					}
				}
			}
		}
	}
	catch( ... ) {
		// If enumeration fails, just return without registering anything
	}
}

ImageTargetRef ImageTargetFileWic::create( DataTargetRef dataTarget, ImageSourceRef imageSource, ImageTarget::Options options, const string &extensionData )
{
	return ImageTargetRef( new ImageTargetFileWic( dataTarget, imageSource, options, extensionData ) );
}

ImageTargetFileWic::ImageTargetFileWic( DataTargetRef dataTarget, ImageSourceRef imageSource, ImageTarget::Options options, const string &extensionData )
	: ImageTarget(), mDataTarget( dataTarget )
{
	mCodecGUID = getExtensionMap()[extensionData];

	setSize( imageSource->getWidth(), imageSource->getHeight() );

	// determine the pixel format we'll request
	WICPixelFormatGUID formatGUID;
	if( imageSource->hasAlpha() ) {
		bool premultAlpha = imageSource->isPremultiplied();
		// WIC doesn't support gray+alpha, so we need to do RGBA regardless
		if( imageSource->getDataType() == ImageIo::UINT8 )
			formatGUID = premultAlpha ? GUID_WICPixelFormat32bppPBGRA : GUID_WICPixelFormat32bppBGRA;
		else if( imageSource->getDataType() == ImageIo::UINT16 )
			formatGUID = premultAlpha ? GUID_WICPixelFormat64bppPRGBA : GUID_WICPixelFormat64bppRGBA;
		else
			formatGUID = premultAlpha ? GUID_WICPixelFormat128bppPRGBAFloat : GUID_WICPixelFormat128bppRGBAFloat;
	}
	else {
		ImageIo::ColorModel cm = options.isColorModelDefault() ? imageSource->getColorModel() : options.getColorModel();
		if( cm == ImageIo::CM_GRAY ) {
			if( imageSource->getDataType() == ImageIo::UINT8 )
				formatGUID = GUID_WICPixelFormat8bppGray;
			else if( imageSource->getDataType() == ImageIo::UINT16 )
				formatGUID = GUID_WICPixelFormat16bppGray;
			else
				formatGUID = GUID_WICPixelFormat32bppGrayFloat;
		}
		else { // RGB
			if( imageSource->getDataType() == ImageIo::UINT8 )
				formatGUID = GUID_WICPixelFormat24bppBGR;
			else if( imageSource->getDataType() == ImageIo::UINT16 )
				formatGUID = GUID_WICPixelFormat48bppRGB;
			else
				formatGUID = GUID_WICPixelFormat128bppRGBFloat;
		}
	}
	
	::HRESULT hr = S_OK;

	msw::initializeCom();

 // Create WIC factory
    msw::ComPtr<IWICImagingFactory> IWICFactory;
    hr = ::CoCreateInstance( CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**)IWICFactory.releaseAndGetAddressOf() );
	if( ! SUCCEEDED( hr ) )
		throw ImageIoExceptionFailedWrite( "Could not create WIC Factory." );

	hr = IWICFactory->CreateEncoder( *mCodecGUID, 0, mEncoder.releaseAndGetAddressOf() );
	if( ! SUCCEEDED( hr ) )
		throw ImageIoExceptionFailedWrite( "Could not create WIC Encoder." );

	// create the stream
	msw::ComPtr<IWICStream> stream;
	hr = IWICFactory->CreateStream( stream.releaseAndGetAddressOf() );
	if( ! SUCCEEDED(hr) )
		throw ImageIoExceptionFailedWrite( "Could not create WIC stream." );
	
	// initialize the stream based on properties of the cinder::DataSouce
	if( mDataTarget->providesFilePath() ) {
		std::wstring filePath =	mDataTarget->getFilePath().wstring().c_str();
		hr = stream->InitializeFromFilename( filePath.c_str(), GENERIC_WRITE );
		if( ! SUCCEEDED(hr) )
			throw ImageIoExceptionFailedWrite( "Could not initialize WIC Stream from filename." );
	}
	else {
		msw::ComPtr<msw::ComOStream> comOStream( new msw::ComOStream( mDataTarget->getStream() ) );
		hr = stream->InitializeFromIStream( comOStream.get() );
		if( ! SUCCEEDED(hr) )
			throw ImageIoExceptionFailedWrite( "Could not initialize WIC Stream from IStream." );
	}
	
	hr = mEncoder->Initialize( stream.get(), WICBitmapEncoderNoCache );
	if( ! SUCCEEDED( hr ) )
		throw ImageIoExceptionFailedWrite( "Could not initialize WIC Encoder." );

	// create the frame encoder
	IPropertyBag2 *pPropertybag = NULL;
	hr = mEncoder->CreateNewFrame( mBitmapFrame.releaseAndGetAddressOf(), &pPropertybag );
	if( ! SUCCEEDED( hr ) )
		throw ImageIoExceptionFailedWrite( "Could not ceate WIC Frame." );

	// setup the propertyBag to express quality
	PROPBAG2 option = { 0 };
    option.pstrName = const_cast<wchar_t*>( L"ImageQuality" );
    VARIANT varValue;    
    VariantInit(&varValue);
    varValue.vt = VT_R4;
    varValue.fltVal = options.getQuality();      
    hr = pPropertybag->Write( 1, &option, &varValue );

	hr = mBitmapFrame->Initialize( pPropertybag );
	if( ! SUCCEEDED( hr ) )
		throw ImageIoExceptionFailedWrite( "Could not initialize WIC PROPBAG2." );
	
	hr = mBitmapFrame->SetSize( mWidth, mHeight );
	if( ! SUCCEEDED( hr ) )
		throw ImageIoExceptionFailedWrite( "Could not set WIC Frame size." );
	
	// ask for our ideal pixel format and then process the one we actually get
	hr = mBitmapFrame->SetPixelFormat( &formatGUID );
	if( ! SUCCEEDED( hr ) )
		throw ImageIoExceptionFailedWrite( "Could not set WIC Frame pixel format." );
	
	setupPixelFormat( formatGUID );
	
	mData = shared_ptr<uint8_t>( new uint8_t[mHeight * mRowBytes], std::default_delete<uint8_t[]>() );
}

void ImageTargetFileWic::setupPixelFormat( const GUID &guid )
{
	if( guid == GUID_WICPixelFormat24bppBGR ) {
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
		setChannelOrder( ImageIo::BGRA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 );// mIsPremultipliedAlpha = true;
	}
	else if( guid == GUID_WICPixelFormat48bppRGB ) {
		setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT16 );
	}
	else if( guid == GUID_WICPixelFormat64bppRGBA ) {
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT16 );
	}
	else if( guid == GUID_WICPixelFormat64bppPRGBA ) {
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT16 ); // mIsPremultipliedAlpha = true;
	}
	else if( guid == GUID_WICPixelFormat128bppRGBFloat ) {
		setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 );
	}
	else if( guid == GUID_WICPixelFormat128bppRGBAFloat ) {
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 );
	}
	else if( guid == GUID_WICPixelFormat128bppPRGBAFloat ) {
		setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); // mIsPremultipliedAlpha = true;
	}
	else if( guid == GUID_WICPixelFormat8bppGray ) {
		setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::UINT8 );
	}
	else if( guid == GUID_WICPixelFormat16bppGray ) {
		setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::UINT16 );
	}
	else if( guid == GUID_WICPixelFormat16bppGrayFixedPoint ) {
		setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::UINT16 );
	}
	else if( guid == GUID_WICPixelFormat16bppGrayHalf ) {
		setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT16 );
	}
	else if( guid == GUID_WICPixelFormat32bppGrayFloat ) {
		setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT32 );
	}
	else if( guid == GUID_WICPixelFormat32bppGrayFixedPoint ) {
		setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT32 );
	}
	else
		throw ImageIoException( "Unsupported format." );
	
	int32_t bitsPerComponent;
	bool writingAlpha = hasAlpha();
	bool isFloat = true;
	switch( getDataType() ) {
		case ImageIo::UINT8: bitsPerComponent = 8; isFloat = false; break;
		case ImageIo::UINT16: bitsPerComponent = 16; isFloat = false; break;
		default: bitsPerComponent = 32; isFloat = true;
	}
	uint8_t numChannels;
	switch( getColorModel() ) {
		case ImageIo::CM_GRAY:
			numChannels = ( writingAlpha ) ? 2 : 1; break;
		default:
			numChannels = ( writingAlpha ) ? 4 : 3;
	}
	int32_t bitsPerPixel = numChannels * bitsPerComponent;
	mRowBytes = ( mWidth * ( bitsPerPixel / 8 ) + 3 ) & ~3;

	return;
}

void* ImageTargetFileWic::getRowPointer( int32_t row )
{
	return &mData.get()[row * mRowBytes];
}

void ImageTargetFileWic::finalize()
{
	::HRESULT hr = S_OK;
	
	hr = mBitmapFrame->WritePixels( mHeight, mRowBytes, mHeight * mRowBytes, mData.get() );
	if( ! SUCCEEDED( hr ) )
		throw ImageIoExceptionFailedWrite( "Could not write WIC Frame pixels." );

	hr = mBitmapFrame->Commit();
	if( ! SUCCEEDED( hr ) )
		throw ImageIoExceptionFailedWrite( "Could not commit WIC Frame." );

	hr = mEncoder->Commit();
	if( ! SUCCEEDED( hr ) )
		throw ImageIoExceptionFailedWrite( "Could not commit WIC Encoder." );
}

} // namespace cinder