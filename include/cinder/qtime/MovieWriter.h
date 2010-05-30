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


#if defined( CINDER_MAC )
	#ifdef __OBJC__
		@class QTMovie;
	#else
		class QTMovie;
	#endif
#endif

#include "cinder/ImageIo.h"

namespace cinder { namespace qtime {

// more types can be found in ImageCompression.h
enum MovieWriterCodecType {
	MP4,
	H264,
	H263,
	PNG,
	PXLT,
	RAW
};

enum MovieWriterQuality {
	LOSSLESS          = 0X00000400,
	MAX               = 0X000003FF,
	MIN               = 0X00000000,
	LOW               = 0X00000100,
	NORMAL            = 0X00000200,
	HIGH              = 0X00000300
};
	
	
	
class MovieWriter {
public:
	MovieWriter();
	MovieWriter(const std::string &path, MovieWriterCodecType type = RAW, MovieWriterQuality quality = HIGH);
	~MovieWriter();

	void addFrame(const ImageSourceRef &imageSource);	
	void finish(void);

	
private:
	#if defined( CINDER_MAC )
		QTMovie         *mMovie;
	#endif
	std::string mOutpath;
	std::string mCodec;
	int mQuality;
};
}}