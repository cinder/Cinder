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
	
	ImageIoRegistrar::registerTargetType( "png", func, PRIORITY, "png" );
	getExtensionMap()["png"] = &GUID_ContainerFormatPng;
	ImageIoRegistrar::registerTargetType( "tif", func, PRIORITY, "tif" ); ImageIoRegistrar::registerTargetType( "tiff", func, PRIORITY, "tif" );
	getExtensionMap()["tif"] = &GUID_ContainerFormatTiff;
	ImageIoRegistrar::registerTargetType( "jpg", func, PRIORITY, "jpg" ); ImageIoRegistrar::registerTargetType( "jpeg", func, PRIORITY, "jpg" ); ImageIoRegistrar::registerTargetType( "jpe", func, PRIORITY, "jpg" );
	getExtensionMap()["jpg"] = &GUID_ContainerFormatJpeg;
	ImageIoRegistrar::registerTargetType( "bmp", func, PRIORITY, "bmp" );
	getExtensionMap()["bmp"] = &GUID_ContainerFormatBmp;
// Disabling GIF because we don't handle converting to paletted formats yet
//	ImageIoRegistrar::registerTargetType( "gif", func, PRIORITY, "gif" );
//	getExtensionMap()["gif"] = GUID_ContainerFormatGif;
	ImageIoRegistrar::registerTargetType( "wmp", func, PRIORITY, "wmp" );
	getExtensionMap()["wmp"] = &GUID_ContainerFormatWmp;
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
    IWICImagingFactory *IWICFactoryP = NULL;
    hr = ::CoCreateInstance( CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&IWICFactoryP) );
	if( ! SUCCEEDED( hr ) )
		throw ImageIoExceptionFailedWrite( "Could not create WIC Factory." );
	shared_ptr<IWICImagingFactory> IWICFactory = msw::makeComShared( IWICFactoryP );

	IWICBitmapEncoder *encoderP = NULL;
	hr = IWICFactory->CreateEncoder( *mCodecGUID, 0, &encoderP );
	if( ! SUCCEEDED( hr ) )
		throw ImageIoExceptionFailedWrite( "Could not create WIC Encoder." );
	mEncoder = msw::makeComShared( encoderP );
	
	// create the stream		
	IWICStream *pIWICStream = NULL;
	hr = IWICFactory->CreateStream( &pIWICStream );
	if( ! SUCCEEDED(hr) )
		throw ImageIoExceptionFailedWrite( "Could not create WIC stream." );
	shared_ptr<IWICStream> stream = msw::makeComShared( pIWICStream );
	
	// initialize the stream based on properties of the cinder::DataSouce
	if( mDataTarget->providesFilePath() ) {
#if defined( CINDER_UWP )
		std::string s = mDataTarget->getFilePath().string();
		std::wstring filePath =	std::wstring(s.begin(), s.end());                 
#else
		std::wstring filePath =	mDataTarget->getFilePath().wstring().c_str();
#endif
		hr = stream->InitializeFromFilename( filePath.c_str(), GENERIC_WRITE );
		if( ! SUCCEEDED(hr) )
			throw ImageIoExceptionFailedWrite( "Could not initialize WIC Stream from filename." );
	}
	else {
		shared_ptr<msw::ComOStream> comOStream = msw::makeComShared( new msw::ComOStream( mDataTarget->getStream() ) );
		hr = stream->InitializeFromIStream( comOStream.get() );
		if( ! SUCCEEDED(hr) )
			throw ImageIoExceptionFailedWrite( "Could not initialize WIC Stream from IStream." );
	}
	
	hr = mEncoder->Initialize( stream.get(), WICBitmapEncoderNoCache );
	if( ! SUCCEEDED( hr ) )
		throw ImageIoExceptionFailedWrite( "Could not initialize WIC Encoder." );

	// create the frame encoder
	IPropertyBag2 *pPropertybag = NULL;
	IWICBitmapFrameEncode *pBitmapFrame = NULL;
	hr = mEncoder->CreateNewFrame( &pBitmapFrame, &pPropertybag );
	if( ! SUCCEEDED( hr ) )
		throw ImageIoExceptionFailedWrite( "Could not ceate WIC Frame." );
	mBitmapFrame = msw::makeComShared( pBitmapFrame );

	// setup the propertyBag to express quality
	PROPBAG2 option = { 0 };
    option.pstrName = L"ImageQuality";
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