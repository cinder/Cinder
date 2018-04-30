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

#include "cinder/Cinder.h"
#include "cinder/ImageIo.h"
#include "cinder/Stream.h"

#if defined( CINDER_MAC ) || defined( CINDER_COCOA_TOUCH )
	#include "cinder/cocoa/CinderCocoa.h"
	#include <CoreVideo/CoreVideo.h>

	#if defined( __OBJC__ )
		@class AVAsset, AVAssetWriter, AVAssetWriterInput, AVAssetWriterInputPixelBufferAdaptor, NSDate;
	#else
		class AVAsset;
		class AVAssetWriter;
		class AVAssetWriterInput;
		class AVAssetWriterInputPixelBufferAdaptor;
	#endif
#endif

namespace cinder { namespace qtime {

class MovieWriter;
typedef std::shared_ptr<MovieWriter> MovieWriterRef;

class MovieWriter {
  public:
	typedef enum Codec { H264, JPEG
#if ! defined( CINDER_COCOA_TOUCH )
	, PRO_RES_4444, PRO_RES_422
#endif
	} Codec;
	typedef enum FileType { QUICK_TIME_MOVIE, MPEG4, M4V } FileType;

	//! Defines the encoding parameters of a MovieWriter
	class Format {
	  public:
		Format();

		//! Returns the codec of type MovieWriter::Codec. Default is \c Codec::H264.
		Codec		getCodec() const { return mCodec; }
		//! Sets the encoding codec. Default is \c Codec::H264
		Format&		codec( Codec codec ) { mCodec = codec; return *this; }
		//! Returns the output file type. Default is \c FileType::QUICK_TIME_MOVIE.
		FileType	getFileType() const { return mFileType; }
		//! Sets the output file type, defined in MovieWriter::FileType. Default is \c FileType::QUICK_TIME_MOVIE.
		Format&		fileType( FileType fileType ) { mFileType = fileType; return *this; }
		
		//! Returns the overall quality for encoding in the range of [\c 0,\c 1.0] for the JPEG codec. Defaults to \c 0.99. \c 1.0 corresponds to lossless.
		float		getJpegQuality() const { return mJpegQuality; }
		//! Sets the overall quality for encoding when using the JPEG codec. Must be in a range of [\c 0,\c 1.0]. Defaults to \c 0.99. \c 1.0 corresponds to lossless.
		Format&		jpegQuality( float quality );
		//! Returns the standard duration of a frame, measured in seconds. Defaults to \c 1/30 sec, meaning \c 30 FPS.
		float		getDefaultFrameDuration() const { return mDefaultFrameDuration; }
		//! Sets the default duration of a frame, measured in seconds. Defaults to \c 1/30 sec, meaning \c 30 FPS.
		Format&		defaultFrameDuration( float defaultDuration ) { mDefaultFrameDuration = defaultDuration; return *this; }
		//! Returns the integer base value for the encoding time scale. Defaults to \c 600
		long		getTimeScale() const { return mTimeBase; }
		//! Sets the integer base value for encoding time scale. Defaults to \c 600.
		Format&		setTimeScale( long timeScale ) { mTimeBase = timeScale; return *this; }

#if ( defined( CINDER_MAC ) &&  ( MAC_OS_X_VERSION_MIN_REQUIRED >= 101000 ) ) || ( defined( CINDER_COCOA_TOUCH ) && ( __IPHONE_OS_VERSION_MIN_REQUIRED >= 70000 ) )
		//! Returns if frame reordering is enabled (H.264 only). Default is \c false.
		bool		isFrameReorderingEnabled() const;
		//! Enables frame reordering (H.264 only). Defaults to \c false. In order to encode \b B frames, a compressor must reorder frames, which means that the order in which they will be emitted and stored (the decode order) is different from the order in which they were presented to the compressor (the display order).
		Format&		enableFrameReordering( bool enable = true );
#endif

		//! Returns the average bits per second for H.264 encoding. Defaults to no limit.
		float		getAverageBitsPerSecond() const { return mH264AverageBitsPerSecond; }
		//! Sets the average bits per second for H.264 encoding. Defaults to no limit.
		Format&		averageBitsPerSecond( float avgBitsPerSecond ) { mH264AverageBitsPerSecondSpecified = true; mH264AverageBitsPerSecond = avgBitsPerSecond; return *this; }
		//! Resets the average bits per second to no limit.
		void		unspecifyAverageBitsPerSecond() { mH264AverageBitsPerSecondSpecified = false; }
		//! Returns whether the average bits per second for H.264 have been specified. If \c false, no limit is imposed.
		bool		isAverageBitsPerSecondSpecified() const { return mH264AverageBitsPerSecond; }

	  private:
		Codec		mCodec;
		FileType	mFileType;
		long		mTimeBase;
		float		mDefaultFrameDuration;
		float		mJpegQuality;
		
		// H.264 only
		bool		mFrameReorderingEnabled;
		bool		mH264AverageBitsPerSecondSpecified;
		float		mH264AverageBitsPerSecond;

		friend class MovieWriter;
	};


	~MovieWriter();

	static MovieWriterRef	create( const fs::path &path, int32_t width, int32_t height, const Format &format = Format() )
		{ return std::shared_ptr<MovieWriter>( new MovieWriter( path, width, height, format ) ); }

	//! Returns the Movie's default frame duration measured in seconds. You can also think of this as the Movie's frameRate.
	float	getDefaultDuration() const { return mFormat.mDefaultFrameDuration; }
	//! Returns the width of the Movie in pixels
	int32_t	getWidth() const { return mWidth; }
	//! Returns the height of the Movie in pixels
	int32_t	getHeight() const { return mHeight; }
	//! Returns the size of the Movie in pixels
	ivec2	getSize() const { return ivec2( getWidth(), getHeight() ); }
	//! Returns the Movie's aspect ratio, which is its width / height
	float	getAspectRatio() const { return getWidth() / (float)getHeight(); }
	//! Returns the bounding Area of the Movie in pixels: [0,0]-(width,height)
	Area	getBounds() const { return Area( 0, 0, getWidth(), getHeight() ); }

	//! Returns the Movie's Format
	const Format&	getFormat() const { return mFormat; }

	//! Appends a frame to the Movie. The optional \a duration parameter allows a frame to be inserted for a durationtime other than the Format's default duration.
	void addFrame( const Surface8u& imageSource, float duration = -1.0f );
	
	//! Returns the number of frames in the movie
	uint32_t	getNumFrames() const { return mNumFrames; }

	//! Completes the encoding of the movie and closes the file. Calling finish() more than once has no effect. It is an error to call addFrame() after calling finish().
	void finish();

  private:
	MovieWriter( const fs::path &path, int32_t width, int32_t height, const Format &format );

	AVAssetWriter*							mWriter;
	AVAssetWriterInput*						mWriterSink;
	AVAssetWriterInputPixelBufferAdaptor*	mSinkAdapater;
	
	uint32_t		mNumFrames;
	double			mCurrentSeconds;
	int32_t			mWidth, mHeight;
	Format			mFormat;
	bool			mFinished;

//	IoStreamRef		mMultiPassFrameCache;
//	std::vector<std::pair<int64_t,int64_t> >	mFrameTimes;
};

class MovieWriterExc : public Exception {
};
class MovieWriterExcInvalidPath : public MovieWriterExc {
};
class MovieWriterExcFrameEncode : public MovieWriterExc {
};
class MovieWriterExcAlreadyFinished : public MovieWriterExc {
};

} } // namespace cinder::qtime