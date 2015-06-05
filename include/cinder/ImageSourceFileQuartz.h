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
#include "cinder/ImageIo.h"
#include "cinder/Exception.h"
#include "cinder/cocoa/CinderCocoa.h"

typedef struct CGImage *CGImageRef;

namespace cinder {

typedef std::shared_ptr<class ImageSourceFileQuartz>	ImageSourceFileQuartzRef;

class ImageSourceFileQuartz : public cocoa::ImageSourceCgImage {
  public:
	static ImageSourceRef			createRef( DataSourceRef dataSourceRef, ImageSource::Options options ) { return createFileQuartzRef( dataSourceRef, options ); }
	static ImageSourceFileQuartzRef	createFileQuartzRef( DataSourceRef dataSourceRef, ImageSource::Options options );

	//! Returns a CFDictionaryRef of properties returned by Quartz ImageIO. Result should not be freed.
	CFDictionaryRef	getQuartzProperties() const;
	//! Returns a CFDictionaryRef of properties returned by Quartz ImageIO specific to the particular image index optionally passed in ImageSource::Options, such as a particular frame of an animated GIF. Result should not be freed.
	CFDictionaryRef	getQuartzIndexProperties() const;

	static void		registerSelf();

  protected:
	ImageSourceFileQuartz( CGImageRef imageRef, ImageSource::Options options, std::shared_ptr<const struct __CFDictionary> imageProperties, std::shared_ptr<const struct __CFDictionary> imageIndexProps, int32_t numFrames );
	
	std::shared_ptr<const struct __CFDictionary>		mImageProperties, mImageIndexProperties;
};

class ImageSourceFileQuartzExceptionUnsupportedData : public ImageIoException {
};

} // namespace cinder
