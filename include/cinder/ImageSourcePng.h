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

struct png_struct_def;
typedef struct png_info_def png_info;

namespace cinder {

struct ci_png_info;

typedef std::shared_ptr<class ImageSourcePng>	ImageSourcePngRef;

class ImageSourcePng : public ImageSource {
  public:
	static ImageSourcePngRef	createRef( DataSourceRef dataSourceRef, ImageSource::Options options = ImageSource::Options() );
	static ImageSourceRef		createSourceRef( DataSourceRef dataSourceRef, ImageSource::Options options = ImageSource::Options() ) { return createRef( dataSourceRef, options ); }
	~ImageSourcePng();

	virtual void	load( ImageTargetRef target );

	static void		registerSelf();

  protected:
	ImageSourcePng( DataSourceRef dataSourceRef, ImageSource::Options options );
	bool loadHeader();
	
	std::shared_ptr<ci_png_info>	mCiInfoPtr;
	png_struct_def					*mPngPtr;
	png_info						*mInfoPtr;
};

class ImageSourcePngException : public ImageIoException {
  public:
	ImageSourcePngException( const std::string &description ) : ImageIoException( description ) {}
};

} // namespace cinder
