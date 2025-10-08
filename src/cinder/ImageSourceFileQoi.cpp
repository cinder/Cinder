/*
 Copyright (c) 2025, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include "cinder/ImageSourceFileQoi.h"
#define QOI_NO_STDIO
#define QOI_IMPLEMENTATION
#include "qoi/qoi.h"

#include <cstdio>
#include <cstdlib>

namespace cinder {

namespace {

// Cross-platform wrapper for qoi_read that handles Unicode paths properly
void* qoiReadPath( const fs::path &path, qoi_desc *desc, int channels )
{
#if defined( CINDER_MSW )
	FILE *f = _wfopen( path.wstring().c_str(), L"rb" );
#else
	FILE *f = fopen( path.string().c_str(), "rb" );
#endif

	if( ! f )
		return nullptr;

	fseek( f, 0, SEEK_END );
	int size = ftell( f );
	if( size <= 0 || fseek( f, 0, SEEK_SET ) != 0 ) {
		fclose( f );
		return nullptr;
	}

	void *data = malloc( size );
	if( ! data ) {
		fclose( f );
		return nullptr;
	}

	int bytesRead = (int)fread( data, 1, size, f );
	fclose( f );

	void *pixels = ( bytesRead != size ) ? nullptr : qoi_decode( data, bytesRead, desc, channels );
	free( data );
	return pixels;
}

} // anonymous namespace

///////////////////////////////////////////////////////////////////////////////
// Registrar
void ImageSourceFileQoi::registerSelf()
{
	static bool alreadyRegistered = false;
	static const int32_t SOURCE_PRIORITY = 2;

	if( alreadyRegistered )
		return;
	alreadyRegistered = true;

	ImageIoRegistrar::SourceCreationFunc sourceFunc = ImageSourceFileQoi::create;
	ImageIoRegistrar::registerSourceType( "qoi", sourceFunc, SOURCE_PRIORITY );
}


///////////////////////////////////////////////////////////////////////////////
// ImageSourceFileQoi
ImageSourceFileQoi::ImageSourceFileQoi( DataSourceRef dataSourceRef, ImageSource::Options /*options*/ )
	: mData( nullptr ), mRowBytes( 0 )
{
	qoi_desc desc;
	int channels = 0;

	if( dataSourceRef->isFilePath() ) {
		mData = (uint8_t*)qoiReadPath( dataSourceRef->getFilePath(), &desc, 0 );
		if( ! mData )
			throw ImageIoExceptionFailedLoad( "Failed to load QOI image" );

		channels = desc.channels;
		mRowBytes = desc.width * channels;
	}
	else { // we'll use a dataref from the buffer
		BufferRef buffer = dataSourceRef->getBuffer();
		mData = (uint8_t*)qoi_decode( buffer->getData(), (int)buffer->getSize(), &desc, 0 );
		if( ! mData )
			throw ImageIoExceptionFailedLoad( "Failed to decode QOI image" );

		channels = desc.channels;
		mRowBytes = desc.width * channels;
	}

	setDataType( ImageIo::UINT8 );
	setSize( desc.width, desc.height );

	switch( channels ) {
		case 3:
			setColorModel( ImageIo::CM_RGB );
			setChannelOrder( ImageIo::ChannelOrder::RGB );
		break;
		case 4:
			setColorModel( ImageIo::CM_RGB );
			setChannelOrder( ImageIo::ChannelOrder::RGBA );
		break;
		default:
			free( mData );
			throw ImageIoException( "QOI: Unsupported number of channels" );
	}
}


ImageSourceFileQoi::~ImageSourceFileQoi()
{
	if( mData )
		free( mData );
}

void ImageSourceFileQoi::load( ImageTargetRef target )
{
	ImageSource::RowFunc func = setupRowFunc( target );
	for( int32_t row = 0; row < mHeight; ++row ) {
		((*this).*func)( target, row, mData + row * mRowBytes );
	}
}

} // namespace cinder
