/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.
 
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

#include "cinder/ImageTargetFileStbImage.h"
#include "cinder/Log.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#include "stb/stb_image_write.h"

namespace cinder {

void ImageTargetFileStbImage::registerSelf()
{
	static bool alreadyRegistered = false;
	const int32_t PRIORITY = 3;
	
	if( alreadyRegistered )
		return;
	alreadyRegistered = true;

	ImageIoRegistrar::TargetCreationFunc func = ImageTargetFileStbImage::create;
	ImageIoRegistrar::registerTargetType( "png", func, PRIORITY, "png" );
	ImageIoRegistrar::registerTargetType( "bmp", func, PRIORITY, "bmp" );
	ImageIoRegistrar::registerTargetType( "tga", func, PRIORITY, "tga" );
	ImageIoRegistrar::registerTargetType( "hdr", func, PRIORITY, "hdr" );
}

ImageTargetRef ImageTargetFileStbImage::create( DataTargetRef dataTarget, ImageSourceRef imageSource, ImageTarget::Options options, const std::string &extensionData )
{
	return ImageTargetRef( new ImageTargetFileStbImage( dataTarget, imageSource, options, extensionData ) );
}

ImageTargetFileStbImage::ImageTargetFileStbImage( DataTargetRef dataTarget, ImageSourceRef imageSource, ImageTarget::Options options, const std::string &extensionData )
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
		case ImageIo::ColorModel::CM_GRAY:
			mNumComponents = ( imageSource->hasAlpha() ) ? 2 : 1;
			setColorModel( ImageIo::ColorModel::CM_GRAY );
			setChannelOrder( ( mNumComponents == 2 ) ? ImageIo::ChannelOrder::YA : ImageIo::ChannelOrder::Y );
		break;
		default:
			throw ImageIoExceptionIllegalColorModel();
	}

	mExtension = extensionData;
	if( mExtension == "hdr" ) { // Radiance files are always float*
		setDataType( ImageIo::DataType::FLOAT32 );
		mRowBytes = mNumComponents * imageSource->getWidth() * sizeof(float);
	}
	else {
		setDataType( ImageIo::DataType::UINT8 );
		mRowBytes = mNumComponents * imageSource->getWidth() * sizeof(float);
	}

	if( mDataTarget->providesFilePath() ) {
		mFilePath = dataTarget->getFilePath();
	}
	
	mData = std::unique_ptr<uint8_t[]>( new uint8_t[mHeight * mRowBytes] );
}

void* ImageTargetFileStbImage::getRowPointer( int32_t row )
{
	return &mData.get()[row * mRowBytes];
}

void stbWriteToStream( void *context, void *data, int size )
{
	OStream *stream = reinterpret_cast<OStream *>( context );
	stream->writeData( data, static_cast<size_t>( size ) );
}

void ImageTargetFileStbImage::finalize()
{
	if( ! mFilePath.empty() ) {
		if( mExtension == "png" ) {
			if( ! stbi_write_png( mFilePath.string().c_str(), (int)mWidth, (int)mHeight, mNumComponents, mData.get(), (int)mRowBytes ) )
				throw ImageIoExceptionFailedWrite();
		}
		else if( mExtension == "bmp" ) {
			if( ! stbi_write_bmp( mFilePath.string().c_str(), (int)mWidth, (int)mHeight, mNumComponents, mData.get() ) )
				throw ImageIoExceptionFailedWrite();
		}
		else if( mExtension == "tga" ) {
			if( ! stbi_write_tga( mFilePath.string().c_str(), (int)mWidth, (int)mHeight, mNumComponents, mData.get() ) )
				throw ImageIoExceptionFailedWrite();
		}
		else if( mExtension == "hdr" ) {
			if( ! stbi_write_hdr( mFilePath.string().c_str(), (int)mWidth, (int)mHeight, mNumComponents, (const float*)mData.get() ) )
				throw ImageIoExceptionFailedWrite();
		}
	}
	else {
		OStream *stream = mDataTarget->getStream().get();
		if( mExtension == "png" ) {
			if( ! stbi_write_png_to_func( stbWriteToStream, stream, (int)mWidth, (int)mHeight, mNumComponents, mData.get(), (int)mRowBytes ) )
				throw ImageIoExceptionFailedWrite();
		}
		else if( mExtension == "bmp" ) {
			if( ! stbi_write_bmp_to_func( stbWriteToStream, stream, (int)mWidth, (int)mHeight, mNumComponents, mData.get() ) )
				throw ImageIoExceptionFailedWrite();
		}
		else if( mExtension == "tga" ) {
			if( ! stbi_write_tga_to_func( stbWriteToStream, stream, (int)mWidth, (int)mHeight, mNumComponents, mData.get() ) )
				throw ImageIoExceptionFailedWrite();
		}
		else if( mExtension == "hdr" ) {
			if( ! stbi_write_hdr_to_func( stbWriteToStream, stream, (int)mWidth, (int)mHeight, mNumComponents, (const float*)mData.get() ) )
				throw ImageIoExceptionFailedWrite();
		}
	}
}

} // namespace cinder
