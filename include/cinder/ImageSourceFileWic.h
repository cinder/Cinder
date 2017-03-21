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
#include "cinder/ImageIO.h"
#include "cinder/Exception.h"

#include <guiddef.h>

// WIC forward declarations
struct IWICImagingFactory;
struct IWICBitmapFrameDecode;
struct IWICStream;

namespace cinder {

typedef std::shared_ptr<class ImageSourceFileWic>	ImageSourceFileWicRef;

class ImageSourceFileWic : public ImageSource {
  public:
	static ImageSourceRef			create( DataSourceRef dataSourceRef, ImageSource::Options options = ImageSource::Options() );

	virtual void	load( ImageTargetRef target );

	static void registerSelf();

  protected:
	ImageSourceFileWic( DataSourceRef dataSourceRef, ImageSource::Options options );
	
	static IWICImagingFactory* getFactory();

	bool	processFormat( const ::GUID &guid, ::GUID *convertGUID );

	std::shared_ptr<IWICBitmapFrameDecode>	mFrame;
	std::shared_ptr<IWICStream>				mStream;
	ci::BufferRef							mBuffer;
	bool									mRequiresConversion;
	int32_t									mRowBytes;
	::GUID									mPixelFormat, mConvertPixelFormat;
};

class ImageSourceFileWicExceptionUnsupportedData : public ImageIoException {
};



} // namespace ci
