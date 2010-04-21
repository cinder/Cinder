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

#include "cinder/ImageTargetFileQuartz.h"
#include "cinder/cocoa/CinderCocoa.h"

#include <ApplicationServices/ApplicationServices.h>

using namespace std;

namespace cinder {

void ImageTargetFileQuartz::registerSelf()
{
	const int32_t PRIORITY = 2;
	
	ImageIoRegistrar::TargetCreationFunc func = ImageTargetFileQuartz::createRef;
	
	::CFArrayRef destTypes = ::CGImageDestinationCopyTypeIdentifiers();
	::CFIndex typeCount = ::CFArrayGetCount( destTypes );
	for( ::CFIndex dt = 0; dt < typeCount; ++dt ) {
		::CFStringRef uti = (::CFStringRef)::CFArrayGetValueAtIndex( destTypes, dt );
		if( ! uti )
			continue;
		std::string utiStr = cocoa::convertCfString( uti );
			

		::CFDictionaryRef dict = (CFDictionaryRef)UTTypeCopyDeclaration( uti );
		if( dict ) {
			::CFDictionaryRef tagSpecDict = (CFDictionaryRef)::CFDictionaryGetValue( dict, kUTTypeTagSpecificationKey );
			if( tagSpecDict ) {
				::CFTypeRef extensions = (CFTypeRef)::CFDictionaryGetValue( tagSpecDict, kUTTagClassFilenameExtension );
				if( extensions ) {
					::CFTypeID type = ::CFGetTypeID( extensions );
					if( type == ::CFStringGetTypeID() ) {		// single extension
						ImageIoRegistrar::registerTargetType( cocoa::convertCfString( (CFStringRef)extensions ), func, PRIORITY, utiStr );
					}
					else if( type == ::CFArrayGetTypeID() ) {	// array of extensions
						::CFArrayRef extensionsArr = ::CFArrayRef( extensions );
						CFIndex extCount = ::CFArrayGetCount( extensionsArr );
						for( CFIndex ext = 0; ext < extCount; ++ext ) {
							ImageIoRegistrar::registerTargetType( cocoa::convertCfString( (CFStringRef)::CFArrayGetValueAtIndex( extensionsArr, ext ) ), func, PRIORITY, utiStr );						
						}
					}			
				}
			}
			
			::CFRelease( dict );
		}
	}
	
	::CFRelease( destTypes );
}

ImageTargetRef ImageTargetFileQuartz::createRef( DataTargetRef dataTarget, ImageSourceRef imageSource, const string &extensionData )
{
	return ImageTargetRef( new ImageTargetFileQuartz( dataTarget, imageSource, extensionData ) );
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

void ImageTargetFileQuartz::setupImageDestOptions()
{
	int bitsPerComponent = ImageIo::dataTypeBytes( getDataType() ) * 8;

	// setup the dictionary of options for CGImageDestinationAddImage
	mImageDestOptions = shared_ptr<__CFDictionary>( ::CFDictionaryCreateMutable( kCFAllocatorDefault, 3, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks ), cocoa::safeCfRelease );
	::CFDictionarySetValue( mImageDestOptions.get(), kCGImagePropertyIsFloat, ( getDataType() == ImageIo::FLOAT32 ) ? kCFBooleanTrue : kCFBooleanFalse );
	::CFDictionarySetValue( mImageDestOptions.get(), kCGImagePropertyHasAlpha, ( hasAlpha() ) ? kCFBooleanTrue : kCFBooleanFalse );
	::CFNumberRef depthNumber = ::CFNumberCreate( kCFAllocatorDefault, kCFNumberIntType, &bitsPerComponent );
	::CFDictionarySetValue( mImageDestOptions.get(), kCGImagePropertyDepth, depthNumber );
	::CFRelease( depthNumber );
}

ImageTargetFileQuartz::ImageTargetFileQuartz( DataTargetRef dataTarget, ImageSourceRef imageSource, const std::string &extensionData )
	: cocoa::ImageTargetCgImage( imageSource )
{
	cocoa::SafeCfString uti = cocoa::createSafeCfString( extensionData );

	mImageDest = NULL;
	if( dataTarget->providesFilePath() ) {
		cocoa::SafeCfString pathString = cocoa::createSafeCfString( dataTarget->getFilePath() );
		shared_ptr<const __CFURL> urlRef( ::CFURLCreateWithFileSystemPath( kCFAllocatorDefault, pathString.get(), kCFURLPOSIXPathStyle, false ), cocoa::safeCfRelease );
		mImageDest = ::CGImageDestinationCreateWithURL( urlRef.get(), uti.get(), 1, NULL );
	}
	else if( dataTarget->providesUrl() ) {
		shared_ptr<const __CFURL> urlRef( cocoa::createCfUrl( dataTarget->getUrl() ), cocoa::safeCfRelease );
		mImageDest = ::CGImageDestinationCreateWithURL( urlRef.get(), uti.get(), 1, NULL );
	
	}
	else { // we'll wrap a cinder::OStream in a CGDataConsumer for output
		OStreamRef *ostreamRef = new OStreamRef( dataTarget->getStream() );
		::CGDataConsumerCallbacks callbacks;
		callbacks.putBytes = cgDataConsumerPutBytes;
		callbacks.releaseConsumer = cgDataConsumerRelease;
		shared_ptr<CGDataConsumer> consumer( ::CGDataConsumerCreate( ostreamRef, &callbacks ), ::CGDataConsumerRelease );
		mImageDest = ::CGImageDestinationCreateWithDataConsumer( consumer.get(), uti.get(), 1, NULL );
	}
	
	if( ! mImageDest )
		throw ImageIoExceptionFailedWrite();
		
	setupImageDestOptions();
}

void ImageTargetFileQuartz::finalize()
{
	ImageTargetCgImage::finalize();
	
	::CGImageDestinationAddImage( mImageDest, mImageRef, mImageDestOptions.get() );
	::CGImageDestinationFinalize( mImageDest );

	// clean up
	::CFRelease( mImageDest );
}

} // namespace cinder