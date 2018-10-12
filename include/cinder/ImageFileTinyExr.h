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

#pragma once

#include "cinder/ImageIo.h"

#define USE_PLANAR_CHANNELS 1

typedef struct _EXRVersion EXRVersion;
typedef struct _EXRHeader  EXRHeader;
typedef struct _EXRImage   EXRImage;

namespace cinder {

typedef std::shared_ptr<class ImageTargetFileTinyExr> ImageTargetFileTinyExrRef;

typedef std::shared_ptr<class ImageSourceFileTinyExr>	ImageSourceFileTinyExrRef;

class ImageSourceFileTinyExr : public ImageSource {
  public:
	static ImageSourceRef create( DataSourceRef dataSource, ImageSource::Options options = ImageSource::Options() );

	void load( ImageTargetRef target ) override;

	static void		registerSelf();

protected:
	ImageSourceFileTinyExr( DataSourceRef dataSourceRef, ImageSource::Options options );

	std::unique_ptr<EXRHeader, std::function<int( EXRHeader * )>> mExrHeader; // We're using the provided FreeEXRHeader function as a custom deleter
	std::unique_ptr<EXRImage, std::function<int( EXRImage * )>>   mExrImage;  // We're using the provided FreeEXRImage function as a custom deleter
};

class ImageTargetFileTinyExr : public ImageTarget {
  public:
	static ImageTargetRef		create( DataTargetRef dataTarget, ImageSourceRef imageSource, ImageTarget::Options options, const std::string &extensionData );

	void*	getRowPointer( int32_t row ) override;
	void	finalize() override;
	
	static void		registerSelf();
	
  protected:
	ImageTargetFileTinyExr( DataTargetRef dataTarget, ImageSourceRef imageSource, ImageTarget::Options options, const std::string &extensionData );

	uint8_t                  mNumComponents;
	fs::path                 mFilePath;
	std::vector<float>       mData;
	std::vector<std::string> mChannelNames;
};

class ImageIoExceptionFailedLoadTinyExr : public ImageIoExceptionFailedLoad {
  public:
	ImageIoExceptionFailedLoadTinyExr( const std::string &description )
		: ImageIoExceptionFailedLoad( description )
	{}
};

class ImageIoExceptionFailedWriteTinyExr : public ImageIoExceptionFailedWrite {
  public:
	ImageIoExceptionFailedWriteTinyExr( const std::string &description )
		: ImageIoExceptionFailedWrite( description )
	{}
};

} // namespace cinder
