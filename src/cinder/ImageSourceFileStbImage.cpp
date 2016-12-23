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

#include "cinder/ImageSourceFileStbImage.h"
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb/stb_image.h"

namespace cinder {

///////////////////////////////////////////////////////////////////////////////
// Registrar
void ImageSourceFileStbImage::registerSelf()
{
	static bool alreadyRegistered = false;
	static const int32_t SOURCE_PRIORITY = 3; // OS-default are 2; this is lower priority
	
	if( alreadyRegistered )
		return;
	alreadyRegistered = true;
	
	ImageIoRegistrar::SourceCreationFunc sourceFunc = ImageSourceFileStbImage::create;
	ImageIoRegistrar::registerSourceType( "jpg", sourceFunc, SOURCE_PRIORITY );
	ImageIoRegistrar::registerSourceType( "jpeg", sourceFunc, SOURCE_PRIORITY );
	ImageIoRegistrar::registerSourceType( "png", sourceFunc, SOURCE_PRIORITY );
	ImageIoRegistrar::registerSourceType( "bmp", sourceFunc, SOURCE_PRIORITY );
	ImageIoRegistrar::registerSourceType( "psd", sourceFunc, SOURCE_PRIORITY );
	ImageIoRegistrar::registerSourceType( "tga", sourceFunc, SOURCE_PRIORITY );
	ImageIoRegistrar::registerSourceType( "gif", sourceFunc, SOURCE_PRIORITY );
	ImageIoRegistrar::registerSourceType( "hdr", sourceFunc, SOURCE_PRIORITY );

	ImageIoRegistrar::registerSourceGeneric( ImageSourceFileStbImage::create, SOURCE_PRIORITY );
}


///////////////////////////////////////////////////////////////////////////////
// ImageSourceFileStbImage
ImageSourceFileStbImage::ImageSourceFileStbImage( DataSourceRef dataSourceRef, ImageSource::Options /*options*/ )
	: mData8u( nullptr ), mData32f( nullptr ), mRowBytes( 0 )
{
	int width = 0, height = 0, components = 0;

	if( dataSourceRef->isFilePath() ) {
		if( stbi_is_hdr( dataSourceRef->getFilePath().string().c_str() ) ) {
			mData32f = stbi_loadf( dataSourceRef->getFilePath().string().c_str(), &width, &height, &components, 0 /*any # of components*/ );
			if( ! mData32f )
				throw ImageIoException( stbi_failure_reason() );

			mRowBytes = width * components * sizeof( float );
		}
		else {
			mData8u = stbi_load( dataSourceRef->getFilePath().string().c_str(), &width, &height, &components, 0 /*any # of components*/ );
			if( ! mData8u )
				throw ImageIoException( stbi_failure_reason() );

			mRowBytes = width * components;
		}
	}
	else { // we'll use a dataref from the buffer
		BufferRef buffer = dataSourceRef->getBuffer();
		if( stbi_is_hdr_from_memory( (unsigned char*)buffer->getData(), (int)buffer->getSize() ) ) {
			mData32f = stbi_loadf_from_memory( (unsigned char*)buffer->getData(), (int)buffer->getSize(), &width, &height, &components, 0 /*any # of components*/ );
			if( ! mData32f )
				throw ImageIoException( stbi_failure_reason() );
			
			mRowBytes = width * components * sizeof(float);
		}
		else {
			mData8u = stbi_load_from_memory( (unsigned char*)buffer->getData(), (int)buffer->getSize(), &width, &height, &components, 0 /*any # of components*/ );
			if( ! mData8u )
				throw ImageIoException( stbi_failure_reason() );
				
			mRowBytes = width * components;
		}
	}

	if( mData8u )
		setDataType( ImageIo::UINT8 );
	else if( mData32f )
		setDataType( ImageIo::FLOAT32 );
	setSize( width, height );

	switch( components ) {
		case 1:
			setColorModel( ImageIo::CM_GRAY );
			setChannelOrder( ImageIo::ChannelOrder::Y );
		break;
		case 2:
			setColorModel( ImageIo::CM_GRAY );
			setChannelOrder( ImageIo::ChannelOrder::YA );
		break;
		case 3:
			setColorModel( ImageIo::CM_RGB );
			setChannelOrder( ImageIo::ChannelOrder::RGB );			
		break;
		case 4:
			setColorModel( ImageIo::CM_RGB );
			setChannelOrder( ImageIo::ChannelOrder::RGBA );
		break;
		default:
			throw ImageIoException();
	}
}


ImageSourceFileStbImage::~ImageSourceFileStbImage()
{
	if( mData8u )
		stbi_image_free( (void*)mData8u );
	if( mData32f )
		stbi_image_free( (void*)mData32f );
}

void ImageSourceFileStbImage::load( ImageTargetRef target )
{
	ImageSource::RowFunc func = setupRowFunc( target );
	const uint8_t *data = ( mData8u ) ? mData8u : reinterpret_cast<uint8_t*>( mData32f );
	for( int32_t row = 0; row < mHeight; ++row ) {
		((*this).*func)( target, row, data + row * mRowBytes );
	}
}

} // namespace cinder
