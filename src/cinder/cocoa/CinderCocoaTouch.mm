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

#include "cinder/cocoa/CinderCocoaTouch.h"
#include "cinder/cocoa/CinderCocoa.h"

#import <UIKit/UIKit.h>

// this is not meant to be a public-facing class, and we will try to gaurantee a unique name due to ObjC's lack of anonymous namespaces
/// \cond
@interface WriteToSavedPhotosAlbumDelegate_CINDER_ANONYMOUS_NS : NSObject {}
@end
@implementation WriteToSavedPhotosAlbumDelegate_CINDER_ANONYMOUS_NS
- (void)image:(UIImage *)image didFinishSavingWithError:(NSError *)error contextInfo:(void *)contextInfo {
	cinder::cocoa::SafeUiImage *safeImage = reinterpret_cast<cinder::cocoa::SafeUiImage*>( contextInfo );
	delete safeImage;
	[self release];
}
@end // WriteToSavedPhotosAlbumDelegate_CINDER_ANONYMOUS_NS
/// \endcond

namespace cinder { namespace cocoa {

Surface8u convertUiImage( UIImage *uiImage, bool assumeOwnership )
{
	CGImageRef imageRef = uiImage.CGImage;
	if( ! imageRef )
		return Surface8u();
	
	Surface8u result( createImageSource( imageRef ) );
	if( assumeOwnership ) {
		[uiImage retain];
		result.setDeallocator( cocoa::safeCocoaRelease, uiImage );
	}
	
	return result;
}

SafeUiImage::SafeUiImage( UIImage *uiImage, CGImageRef imageRef )
{
	if( uiImage )
		[uiImage retain];
	if( imageRef )
		::CGImageRetain( imageRef );
	mObj = shared_ptr<std::pair<UIImage*,CGImageRef> >( new std::pair<UIImage*,CGImageRef>( uiImage, imageRef ), SafeUiImage::destroy );
}

void SafeUiImage::destroy( std::pair<UIImage*,CGImageRef> *data )
{
	if( data ) {
		if( data->first )
			[data->first release];
		if( data->second )
			::CGImageRelease( data->second );
		delete data;
	}
}

SafeUiImage createUiImage( const ImageSourceRef imageSource )
{
	SafeUiImage result;
	CGImageRef imageRef = createCgImage( imageSource );
	if( imageRef ) {
		UIImage *uiImage =[UIImage imageWithCGImage:imageRef];
		result = SafeUiImage( uiImage, imageRef );
		::CGImageRelease( imageRef );
	}

	return result;
}

void writeToSavedPhotosAlbum( const ImageSourceRef imageSource )
{
	cocoa::SafeUiImage uiImage = cocoa::createUiImage( imageSource );
	if( uiImage ) {
		// Supposedly this routine gets bummed if you destroy its UIImage, so we'll need to setup a callback
		WriteToSavedPhotosAlbumDelegate_CINDER_ANONYMOUS_NS *saveFinishedDelegate = [[WriteToSavedPhotosAlbumDelegate_CINDER_ANONYMOUS_NS alloc] init];
		::UIImageWriteToSavedPhotosAlbum( uiImage, saveFinishedDelegate, @selector(image:didFinishSavingWithError:contextInfo:), new cocoa::SafeUiImage( uiImage ) );
	}
}

} } // namespace cinder::cocoa