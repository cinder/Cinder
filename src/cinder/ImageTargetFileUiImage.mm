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

#include "cinder/ImageTargetFileUiImage.h"
#include "cinder/cocoa/CinderCocoa.h"

#if ! defined( CINDER_COCOA_TOUCH )
	#error "This is only intended for compilation on the Cocoa Touch platform"
#endif

#include <UIKit/UIImage.h>

using namespace std;

namespace cinder {

void ImageTargetFileUiImage::registerSelf()
{
	const int32_t PRIORITY = 2;

	ImageIoRegistrar::TargetCreationFunc func = ImageTargetFileUiImage::createRef;

	ImageIoRegistrar::registerTargetType( "png", func, PRIORITY, "png" );
	ImageIoRegistrar::registerTargetType( "jpg", func, PRIORITY, "jpeg" ); ImageIoRegistrar::registerTargetType( "jpeg", func, PRIORITY, "jpeg" ); ImageIoRegistrar::registerTargetType( "jpe", func, PRIORITY, "jpeg" );
}

ImageTargetRef ImageTargetFileUiImage::createRef( DataTargetRef dataTarget, ImageSourceRef imageSource, const string &extensionData )
{
	return ImageTargetRef( new ImageTargetFileUiImage( dataTarget, imageSource, extensionData ) );
}

namespace { // anonymous namespace

extern "C" size_t cgDataConsumerPutBytes( void *info, const void *buffer, size_t count )
{
	OStreamRef *ostream = reinterpret_cast<OStreamRef*>( info );
	(*ostream)->writeData( buffer, count );
	return count;
}

extern "C" void cgDataConsumerRelease( void *info )
{
	delete reinterpret_cast<OStreamRef*>( info );
}

} // anonymous namespace

ImageTargetFileUiImage::ImageTargetFileUiImage( DataTargetRef dataTarget, ImageSourceRef imageSource, const string &extensionData )
	: ImageTargetCgImage( imageSource ), mExtension( extensionData ), mDataTarget( dataTarget )
{
}

void ImageTargetFileUiImage::finalize()
{
	ImageTargetCgImage::finalize();

	UIImage *image = [UIImage imageWithCGImage:mImageRef];
	if( ! image )
		throw ImageIoExceptionFailedWrite();
	
	NSData *data = 0;
	if( mExtension == "png" ) {
		data = UIImagePNGRepresentation( image );
	}
	else if( mExtension == "jpeg" ) {
		data = UIImageJPEGRepresentation( image, 1.0f );
	}
	
	if( ! data )
		throw ImageIoExceptionFailedWrite();
		
	OStreamRef stream( mDataTarget->getStream() );
	if( ! stream )
		throw ImageIoExceptionFailedWrite();
		
	stream->writeData( [data bytes], [data length] );
}

} // namespace cinder