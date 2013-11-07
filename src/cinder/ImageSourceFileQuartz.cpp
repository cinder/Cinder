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

#include "cinder/ImageSourceFileQuartz.h"
#include "cinder/cocoa/CinderCocoa.h"

#if defined( CINDER_COCOA_TOUCH )
	#include <MobileCoreServices/MobileCoreServices.h>
	#include <ImageIO/ImageIO.h>
#else
	#include <CoreServices/CoreServices.h>
#endif

namespace cinder {

///////////////////////////////////////////////////////////////////////////////
// Registrar
void ImageSourceFileQuartz::registerSelf()
{
	static const int32_t SOURCE_PRIORITY = 2;
	
	ImageIoRegistrar::SourceCreationFunc sourceFunc = ImageSourceFileQuartz::createRef;
	
	CFArrayRef sourceTypes = ::CGImageSourceCopyTypeIdentifiers();
	CFIndex sourceCount = ::CFArrayGetCount( sourceTypes );
	for( CFIndex st = 0; st < sourceCount; ++st ) {
		::CFStringRef uti = (::CFStringRef)::CFArrayGetValueAtIndex( sourceTypes, st );
		if( ! uti )
			continue;
		::CFDictionaryRef dict = (CFDictionaryRef)UTTypeCopyDeclaration( uti );
		if( dict ) {
			::CFDictionaryRef tagSpecDict = (CFDictionaryRef)::CFDictionaryGetValue( dict, kUTTypeTagSpecificationKey );
			if( tagSpecDict ) {
				::CFTypeRef extensions = (CFTypeRef)::CFDictionaryGetValue( tagSpecDict, kUTTagClassFilenameExtension );
				if( extensions ) {
					::CFTypeID type = ::CFGetTypeID( extensions );
					if( type == ::CFStringGetTypeID() ) {		// single extension
						ImageIoRegistrar::registerSourceType( cocoa::convertCfString( (CFStringRef)extensions ), sourceFunc, SOURCE_PRIORITY );
					}
					else if( type == ::CFArrayGetTypeID() ) {	// array of extensions
						::CFArrayRef extensionsArr = ::CFArrayRef( extensions );
						CFIndex extCount = ::CFArrayGetCount( extensionsArr );
						for( CFIndex ext = 0; ext < extCount; ++ext ) {
							ImageIoRegistrar::registerSourceType( cocoa::convertCfString( (CFStringRef)::CFArrayGetValueAtIndex( extensionsArr, ext ) ), sourceFunc, SOURCE_PRIORITY );
						}
					}			
				}
			}
			
			::CFRelease( dict );
		}
	}
	
	::CFRelease( sourceTypes );
	
	ImageIoRegistrar::registerSourceGeneric( ImageSourceFileQuartz::createRef, SOURCE_PRIORITY );
}


///////////////////////////////////////////////////////////////////////////////
// ImageSourceFileQuartz
ImageSourceFileQuartzRef ImageSourceFileQuartz::createFileQuartzRef( DataSourceRef dataSourceRef, ImageSource::Options options )
{
	std::shared_ptr<CGImageSource> sourceRef;
	std::shared_ptr<CGImage> imageRef;
	
	::CFStringRef keys[1] = { kCGImageSourceShouldAllowFloat };
	::CFBooleanRef values[1] = { kCFBooleanTrue };
	const std::shared_ptr<__CFDictionary> optionsDict( (__CFDictionary*)CFDictionaryCreate( kCFAllocatorDefault, (const void **)&keys, (const void **)&values, 1, NULL, NULL ), cocoa::safeCfRelease );

	if( dataSourceRef->isFilePath() ) {
		::CFStringRef pathString = cocoa::createCfString( dataSourceRef->getFilePath().string() );
		::CFURLRef urlRef = ::CFURLCreateWithFileSystemPath( kCFAllocatorDefault, pathString, kCFURLPOSIXPathStyle, false );
		sourceRef = std::shared_ptr<CGImageSource>( ::CGImageSourceCreateWithURL( urlRef, optionsDict.get() ), cocoa::safeCfRelease );
		::CFRelease( pathString );
		::CFRelease( urlRef );
	}
	else if( dataSourceRef->isUrl() ) {
		::CFURLRef urlRef = cocoa::createCfUrl( dataSourceRef->getUrl() );
		if( ! urlRef )
			throw ImageIoException( "Could not create CFURLRef from data source." );
		sourceRef = std::shared_ptr<CGImageSource>( ::CGImageSourceCreateWithURL( urlRef, optionsDict.get() ), cocoa::safeCfRelease );
		::CFRelease( urlRef );		
	}
	else { // last ditch, we'll use a dataref from the buffer
		::CFDataRef dataRef = cocoa::createCfDataRef( dataSourceRef->getBuffer() );
		if( ! dataRef )
			throw ImageIoExceptionFailedLoad( "Could not create CFDataRef from data source." );
		
		sourceRef = std::shared_ptr<CGImageSource>( ::CGImageSourceCreateWithData( dataRef, optionsDict.get() ), cocoa::safeCfRelease );
		::CFRelease( dataRef );
	}
	
	if( sourceRef ) {
		imageRef = std::shared_ptr<CGImage>( ::CGImageSourceCreateImageAtIndex( sourceRef.get(), options.getIndex(), optionsDict.get() ), CGImageRelease );
		if( ! imageRef )
			throw ImageIoExceptionFailedLoad( "Core Graphics coult not create image data." );
	}
	else
		throw ImageIoExceptionFailedLoad( "Failed to load CGImageSource." );

	const std::shared_ptr<__CFDictionary> imageProperties( (__CFDictionary*)::CGImageSourceCopyProperties( sourceRef.get(), NULL ), ::CFRelease );
	const std::shared_ptr<__CFDictionary> imageIndexProperties( (__CFDictionary*)::CGImageSourceCopyPropertiesAtIndex( sourceRef.get(), options.getIndex(), NULL ), ::CFRelease );

	return ImageSourceFileQuartzRef( new ImageSourceFileQuartz( imageRef.get(), options, imageProperties, imageIndexProperties ) );
}

ImageSourceFileQuartz::ImageSourceFileQuartz( CGImageRef imageRef, ImageSource::Options options, std::shared_ptr<const struct __CFDictionary> imageProperties, std::shared_ptr<const struct __CFDictionary> imageIndexProps )
	: ImageSourceCgImage( imageRef, options ), mImageProperties( imageProperties ), mImageIndexProperties( imageIndexProps )
{
}

CFDictionaryRef	ImageSourceFileQuartz::getQuartzProperties() const
{
	return mImageProperties.get();
}

CFDictionaryRef	ImageSourceFileQuartz::getQuartzIndexProperties() const
{
	return mImageIndexProperties.get();
}

} // namespace cinder