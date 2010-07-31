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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Surface.h"

#include <CoreGraphics/CoreGraphics.h>
#include <CoreFoundation/CoreFoundation.h>

#if defined( __OBJC__ )
	@class UIImage;
#else
	class UIImage;
#endif

namespace cinder { namespace cocoa {

/** \brief Converts a UIImage* into a cinder::Surface8u
	If \a assumeOwnership the result will increment the retain count on construction and decrement it upon the its own destruction. **/
Surface8u convertUiImage( UIImage *uiImage, bool assumeOwnership = false );

/** \brief A class which encapsulate a UIImage and an associated CGImageRef in order to work around memory ownership ambiguities in Cocoa Touch **/
class SafeUiImage {
  public:
	SafeUiImage() {}
	SafeUiImage( UIImage *uiImage, CGImageRef imageRef );
	
	operator UIImage*() const { if( mObj ) return mObj->first; else return 0; }

  private:
	static void destroy( std::pair<UIImage*,CGImageRef> *data );

	shared_ptr<std::pair<UIImage*,CGImageRef> >	mObj;
};

/** Loads an ImageSourceRef \a imageSource into a SafeUiImage, which acts like a shared_ptr but can cast itself to UIImage* automatically **/
SafeUiImage createUiImage( const ImageSourceRef imageSource );

//! Writes an image to the device's Photo Album
void writeToSavedPhotosAlbum( const ImageSourceRef imageSource );

} } // namespace cinder::cocoa

