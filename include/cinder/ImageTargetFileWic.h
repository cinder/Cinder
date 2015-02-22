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

#ifndef GUID
typedef struct _GUID GUID;
#endif
struct IWICBitmapEncoder;
struct IWICBitmapFrameEncode;

namespace cinder {

typedef std::shared_ptr<class ImageTargetFileWic> ImageTargetFileWicRef;

class ImageTargetFileWic : public ImageTarget {
  public:
	static ImageTargetRef		create( DataTargetRef dataTarget, ImageSourceRef imageSource, ImageTarget::Options options, const std::string &extensionData );
	
	void*	getRowPointer( int32_t row ) override;
	void	finalize() override;
	
	static void		registerSelf();
	
  protected:
	ImageTargetFileWic( DataTargetRef dataTarget, ImageSourceRef imageSource, ImageTarget::Options options, const std::string &extensionData );
	
	void		setupPixelFormat( const GUID &guid );
	
	std::shared_ptr<uint8_t>	mData;
	int32_t						mRowBytes;
	DataTargetRef				mDataTarget;
	const GUID					*mCodecGUID;
	
	std::shared_ptr<IWICBitmapEncoder>			mEncoder;
	std::shared_ptr<IWICBitmapFrameEncode>		mBitmapFrame;
};

} // namespace cinder