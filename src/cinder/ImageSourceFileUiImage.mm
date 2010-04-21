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

#include "cinder/ImageSourceFileUiImage.h"
#include "cinder/cocoa/CinderCocoa.h"

#if ! defined( CINDER_COCOA_TOUCH )
	#error "This is only intended for compilation on the Cocoa Touch platform"
#endif

#include <UIKit/UIImage.h>

namespace cinder {

///////////////////////////////////////////////////////////////////////////////
// Registrar
void ImageSourceFileUiImage::registerSelf()
{
	static const int32_t SOURCE_PRIORITY = 2;
	
	ImageIoRegistrar::SourceCreationFunc sourceFunc = ImageSourceFileUiImage::createRef;

	ImageIoRegistrar::registerSourceType( "tif", sourceFunc, SOURCE_PRIORITY ); ImageIoRegistrar::registerSourceType( "tiff", sourceFunc, SOURCE_PRIORITY );
	ImageIoRegistrar::registerSourceType( "jpg", sourceFunc, SOURCE_PRIORITY ); ImageIoRegistrar::registerSourceType( "jpeg", sourceFunc, SOURCE_PRIORITY );
	ImageIoRegistrar::registerSourceType( "gif", sourceFunc, SOURCE_PRIORITY );	
	ImageIoRegistrar::registerSourceType( "png", sourceFunc, SOURCE_PRIORITY );
	ImageIoRegistrar::registerSourceType( "bmp", sourceFunc, SOURCE_PRIORITY ); ImageIoRegistrar::registerSourceType( "bmpf", sourceFunc, SOURCE_PRIORITY );
	ImageIoRegistrar::registerSourceType( "ico", sourceFunc, SOURCE_PRIORITY );
	ImageIoRegistrar::registerSourceType( "cur", sourceFunc, SOURCE_PRIORITY );
	ImageIoRegistrar::registerSourceType( "xbm", sourceFunc, SOURCE_PRIORITY );
	
	ImageIoRegistrar::registerSourceGeneric( ImageSourceFileUiImage::createRef, SOURCE_PRIORITY );
}


///////////////////////////////////////////////////////////////////////////////
// ImageSourceFileUiImage
ImageSourceFileUiImageRef ImageSourceFileUiImage::createFileUiImageRef( DataSourceRef dataSourceRef )
{
	UIImage *uiImage = 0;
	if( dataSourceRef->isFilePath() ) {
		uiImage = [UIImage imageWithContentsOfFile:cocoa::SafeNsString( dataSourceRef->getFilePath() )];
		[uiImage retain];
	}
	else { // no file path, so we'll use a dataref from the buffer
		uiImage = [UIImage imageWithData:cocoa::SafeNsData( dataSourceRef->getBuffer() )];
		[uiImage retain];
	}

	CGImageRef imageRef = 0;
	if( uiImage ) {
		imageRef = ((UIImage*)uiImage).CGImage;
		if( ! imageRef )
			throw ImageIoExceptionFailedLoad();
	}
	else
		throw ImageIoExceptionFailedLoad();		

	return ImageSourceFileUiImageRef( new ImageSourceFileUiImage( uiImage, imageRef ) );
}

ImageSourceFileUiImage::ImageSourceFileUiImage( /*UIImage**/void *uiImage, CGImageRef imageRef )
	: ImageSourceCgImage( imageRef ), mUiImage( uiImage )
{
}

ImageSourceFileUiImage::~ImageSourceFileUiImage()
{
	if( mUiImage )
		[(UIImage*)mUiImage release];
}

} // namespace cinder