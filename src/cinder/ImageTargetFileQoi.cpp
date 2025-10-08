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

#include "cinder/ImageTargetFileQoi.h"
#include "cinder/Log.h"

// Note: QOI_IMPLEMENTATION is defined in ImageSourceFileQoi.cpp
// to avoid multiple definition errors
#define QOI_NO_STDIO
#include "qoi/qoi.h"

#include <cstdio>
#include <cstdlib>

namespace cinder {

namespace {

// Cross-platform wrapper for qoi_write that handles Unicode paths properly
int qoiWritePath( const fs::path &path, const void *data, const qoi_desc *desc )
{
#if defined( CINDER_MSW )
	FILE *f = _wfopen( path.wstring().c_str(), L"wb" );
#else
	FILE *f = fopen( path.string().c_str(), "wb" );
#endif

	if( ! f )
		return 0;

	int size;
	void *encoded = qoi_encode( data, desc, &size );
	if( ! encoded ) {
		fclose( f );
		return 0;
	}

	fwrite( encoded, 1, size, f );
	fflush( f );
	int err = ferror( f );
	fclose( f );

	free( encoded );
	return err ? 0 : size;
}

} // anonymous namespace

void ImageTargetFileQoi::registerSelf()
{
	static bool alreadyRegistered = false;
	const int32_t PRIORITY = 1;

	if( alreadyRegistered )
		return;
	alreadyRegistered = true;

	ImageIoRegistrar::TargetCreationFunc func = ImageTargetFileQoi::create;
	ImageIoRegistrar::registerTargetType( "qoi", func, PRIORITY, "qoi" );
}

ImageTargetRef ImageTargetFileQoi::create( DataTargetRef dataTarget, ImageSourceRef imageSource, ImageTarget::Options options, const std::string &extensionData )
{
	return ImageTargetRef( new ImageTargetFileQoi( dataTarget, imageSource, options, extensionData ) );
}

ImageTargetFileQoi::ImageTargetFileQoi( DataTargetRef dataTarget, ImageSourceRef imageSource, ImageTarget::Options options, const std::string &extensionData )
	: mDataTarget( dataTarget )
{
	if( ! ( mDataTarget->providesFilePath() || mDataTarget->getStream() ) ) {
		throw ImageIoExceptionFailedWrite( "No file path or stream provided" );
	}

	setSize( imageSource->getWidth(), imageSource->getHeight() );
	ImageIo::ColorModel cm = options.isColorModelDefault() ? imageSource->getColorModel() : options.getColorModel();

	switch( cm ) {
		case ImageIo::ColorModel::CM_RGB:
			mNumComponents = ( imageSource->hasAlpha() ) ? 4 : 3;
			setColorModel( ImageIo::ColorModel::CM_RGB );
			setChannelOrder( ( mNumComponents == 4 ) ? ImageIo::ChannelOrder::RGBA : ImageIo::ChannelOrder::RGB );
		break;
		default:
			throw ImageIoExceptionIllegalColorModel( "QOI only supports RGB/RGBA color models" );
	}

	setDataType( ImageIo::DataType::UINT8 );
	mRowBytes = mNumComponents * imageSource->getWidth();

	if( mDataTarget->providesFilePath() ) {
		mFilePath = dataTarget->getFilePath();
	}

	mData = std::unique_ptr<uint8_t[]>( new uint8_t[mHeight * mRowBytes] );
}

void* ImageTargetFileQoi::getRowPointer( int32_t row )
{
	return &mData.get()[row * mRowBytes];
}

void ImageTargetFileQoi::finalize()
{
	qoi_desc desc;
	desc.width = (unsigned int)mWidth;
	desc.height = (unsigned int)mHeight;
	desc.channels = mNumComponents;
	desc.colorspace = QOI_SRGB;

	if( ! mFilePath.empty() ) {
		if( ! qoiWritePath( mFilePath, mData.get(), &desc ) )
			throw ImageIoExceptionFailedWrite( "Failed to write QOI image" );
	}
	else {
		// Encode to memory
		int encodedSize = 0;
		void *encoded = qoi_encode( mData.get(), &desc, &encodedSize );
		if( ! encoded )
			throw ImageIoExceptionFailedWrite( "Failed to encode QOI image" );

		// Write to stream
		OStream *stream = mDataTarget->getStream().get();
		stream->writeData( encoded, static_cast<size_t>( encodedSize ) );

		// Free the encoded buffer
		free( encoded );
	}
}

} // namespace cinder
