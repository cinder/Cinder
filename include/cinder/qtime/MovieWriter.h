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

// This path is not used on 64-bit Mac or Windows. On the Mac we only use this path for <=Mac OS 10.7
#if ( defined( CINDER_MAC ) && ( ! defined( __LP64__ ) ) && ( MAC_OS_X_VERSION_MIN_REQUIRED < 1080 ) ) || ( defined( CINDER_MSW ) && ( ! defined( _WIN64 ) ) )

#include "cinder/ImageIo.h"
#include "cinder/Stream.h"
#include "cinder/qtime/QuickTime.h"

#include <string>

// These forward declarations prevent us from having to bring all of QuickTime into the global namespace in MSW
//! \cond
#if defined( CINDER_MSW )
	typedef struct ComponentInstanceRecord		ComponentInstanceRecord;
	typedef ComponentInstanceRecord *			ComponentInstance;
	typedef ComponentInstance					DataHandler;
	typedef struct TrackType**					Track;
	typedef struct MediaType**					Media;
	typedef struct OpaqueICMCompressionSession*			ICMCompressionSessionRef;
	typedef struct OpaqueICMCompressionSessionOptions*  ICMCompressionSessionOptionsRef;
	typedef const struct OpaqueICMEncodedFrame* ICMEncodedFrameRef;
	typedef signed long							OSStatus;
	typedef unsigned long						CodecType;
	typedef unsigned long						ICMCompressionPassModeFlags;
#else
	#include <QuickTime/QuickTime.h>
	#include <QuickTime/ImageCompression.h>
#endif // defined( CINDER_MSW )
//! \endcond

namespace cinder { namespace qtime {

class MovieWriter;
typedef std::shared_ptr<MovieWriter>	MovieWriterRef;

class MovieWriter {
	struct Obj;

  public:
	//! Defines the encoding parameters of a MovieWriter
	class Format {
	  public:
		Format();
		Format( uint32_t codec, float quality );
		Format( const ICMCompressionSessionOptionsRef settings, uint32_t codec, float quality, float frameRate, bool enableMultiPass );
		Format( const Format &format );
		~Format();

		const Format& operator=( const Format &format );

		//! Returns the four character code for the QuickTime codec. Types can be found in QuickTime's ImageCompression.h.
		uint32_t	getCodec() const { return mCodec; }
		//! Sets the four character code for the QuickTime codec. Defaults to \c PNG (\c 'png '). Additional types can be found in QuickTime's \c ImageCompression.h.
		Format&		setCodec( uint32_t codec ) { mCodec = codec; return *this; }
		//! Returns the overall quality for encoding in the range of [\c 0,\c 1.0]. Defaults to \c 0.99. \c 1.0 corresponds to lossless.
		float		getQuality() const { return mQualityFloat; }
		//! Sets the overall quality for encoding. Must be in a range of [\c 0,\c 1.0]. Defaults to \c 0.99. \c 1.0 corresponds to lossless.
		Format&		setQuality( float quality );
		//! Returns the standard duration of a frame measured in seconds
		float		getDefaultDuration() const { return mDefaultTime; }
		//! Sets the default duration of a frame, measured in seconds. Defaults to \c 1/30 sec, meaning \c 30 FPS.
		Format&		setDefaultDuration( float defaultDuration ) { mDefaultTime = defaultDuration; return *this; }
		//! Returns the integer base value for the encoding time scale. Defaults to \c 600
		long		getTimeScale() const { return mTimeBase; }
		//! Sets the integer base value for encoding time scale. Defaults to \c 600.
		Format&		setTimeScale( long timeScale ) { mTimeBase = timeScale; return *this; }
		//! Returns the gamma value by which image data is encoded.
		float		getGamma() const { return mGamma; }
		//! Sets the gamma value by which image data is encoded. Defaults to \c 2.5 on MSW and \c 2.2 on Mac OS X.
		Format&		setGamma( float gamma ) { mGamma = gamma; return *this; }
		//! Returns if temporal compression (allowing \b P or \b B frames) is enabled. Defaults to \c true.
		bool		isTemporal() const;
		//! Enables temporal compression (allowing \b P or \b B frames). Defaults to \c true.
		Format&		enableTemporal( bool enable = true );
		//! Returns if frame reordering is enabled. Defaults to \c true. In order to encode \b B frames, a compressor must reorder frames, which means that the order in which they will be emitted and stored (the decode order) is different from the order in which they were presented to the compressor (the display order).
		bool		isReordering() const;
		//! Enables frame reordering. Defaults to \c true. In order to encode \b B frames, a compressor must reorder frames, which means that the order in which they will be emitted and stored (the decode order) is different from the order in which they were presented to the compressor (the display order).
		Format&		enableReordering( bool enable = true );
		//! Gets the maximum number of frames between key frames. Default is \c 0, which indicates that the compressor should choose where to place all key frames. Compressors are allowed to generate key frames more frequently if this would result in more efficient compression.
		int32_t		getMaxKeyFrameRate() const;
		//! Sets the maximum number of frames between key frames. Default is \c 0, which indicates that the compressor should choose where to place all key frames. Compressors are allowed to generate key frames more frequently if this would result in more efficient compression.
		Format&		setMaxKeyFrameRate( int32_t rate );
		//! Returns whether a codec is allowed to change frame times. Defaults to \c true. Some compressors are able to identify and coalesce runs of identical frames and output single frames with longer duration, or output frames at a different frame rate from the original.
		bool		isFrameTimeChanges() const;
		//! Sets whether a codec is allowed to change frame times. Defaults to \c true. Some compressors are able to identify and coalesce runs of identical frames and output single frames with longer duration, or output frames at a different frame rate from the original.
		Format&		enableFrameTimeChanges( bool enable = true );
		//! Returns whether multiPass encoding is enabled. Defaults to \c false.
		bool		isMultiPass() const { return mEnableMultiPass; }
		//! Enables multiPass encoding. Defaults to \c false. While multiPass encoding can result in significantly smaller movies, it often takes much longer to compress and requires the creation of two temporary files for storing intermediate results.
		Format&		enableMultiPass( bool enable = true ) { mEnableMultiPass = enable; return *this; }

	  private:
		void		initDefaults();

		uint32_t	mCodec;
		long		mTimeBase;
		float		mDefaultTime;
		float		mQualityFloat;
		float		mGamma;
		bool		mEnableMultiPass;

		ICMCompressionSessionOptionsRef		mOptions;

		friend class MovieWriter;
		friend struct Obj;
	};


	MovieWriter() {}
	MovieWriter( const fs::path &path, int32_t width, int32_t height, const Format &format = Format::Format() );

	static MovieWriterRef	create( const fs::path &path, int32_t width, int32_t height, const Format &format = Format::Format() )
		{ return std::shared_ptr<MovieWriter>( new MovieWriter( path, width, height, format ) ); }

	//! Returns the Movie's default frame duration measured in seconds. You can also think of this as the Movie's frameRate.
	float	getDefaultDuration() const { return mObj->mFormat.mDefaultTime; }
	//! Returns the width of the Movie in pixels
	int32_t	getWidth() const { return mObj->mWidth; }
	//! Returns the height of the Movie in pixels
	int32_t	getHeight() const { return mObj->mHeight; }
	//! Returns the size of the Movie in pixels
	ivec2	getSize() const { return ivec2( getWidth(), getHeight() ); }
	//! Returns the Movie's aspect ratio, which is its width / height
	float	getAspectRatio() const { return getWidth() / (float)getHeight(); }
	//! Returns the bounding Area of the Movie in pixels: [0,0]-(width,height)
	Area	getBounds() const { return Area( 0, 0, getWidth(), getHeight() ); }

	//! Returns the Movie's Format
	const Format&	getFormat() const { return mObj->mFormat; }

	/** \brief Presents the user with the standard compression options dialog. Optional \a previewImage provides a still image as a preview (currently ignored on Mac OS X). Returns \c false if user cancelled.
		\image html qtime/MovieWriter/qtime_settings_small.png **/
	static bool		getUserCompressionSettings( Format *result, ImageSourceRef previewImage = ImageSourceRef() );

	/** \brief Appends a frame to the Movie. The optional \a duration parameter allows a frame to be inserted for a time other than the Format's default duration.
		\note Calling addFrame() after a call to finish() will throw a MovieWriterExcAlreadyFinished exception. **/
	void addFrame( const ImageSourceRef &imageSource, float duration = -1.0f ) { mObj->addFrame( imageSource, duration ); }
	
	//! Returns the number of frames in the movie
	uint32_t	getNumFrames() const { return mObj->mNumFrames; }

	//! Completes the encoding of the movie and closes the file. Calling finish() more than once has no effect.
	void finish() { mObj->finish(); }
	
	enum { CODEC_H264 = 'avc1', CODEC_JPEG = 'jpeg', CODEC_MP4 = 'mp4v', CODEC_PNG = 'png ', CODEC_RAW = 'raw ', CODEC_ANIMATION = 'rle ' };

  private:
	/// \cond
	struct Obj {
		Obj( const fs::path &path, int32_t width, int32_t height, const Format &format );
		~Obj();
		
		void	addFrame( const ImageSourceRef &imageSource, float duration );
		void	createCompressionSession();
		void	finish();
		
		static OSStatus encodedFrameOutputCallback( void *refCon, ::ICMCompressionSessionRef session, OSStatus err, ICMEncodedFrameRef encodedFrame, void *reserved );

		::Movie							mMovie;
		::DataHandler					mDataHandler;
		::Track							mTrack;
		::Media							mMedia;
		::ICMCompressionSessionRef		mCompressionSession;
		::ICMCompressionPassModeFlags 	mMultiPassModeFlags;		
		fs::path		mPath;
		uint32_t		mNumFrames;
		int64_t			mCurrentTimeValue;
		
		int32_t		mWidth, mHeight;
		Format		mFormat;
		bool		mRequestedMultiPass, mDoingMultiPass, mFinished;		

		IoStreamRef		mMultiPassFrameCache;

		std::vector<std::pair<int64_t,int64_t> >	mFrameTimes;
	};
	/// \endcond
	
	std::shared_ptr<Obj>		mObj;  

  public:
	//@{
	//! Emulates shared_ptr-like behavior
	typedef std::shared_ptr<Obj> MovieWriter::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &MovieWriter::mObj; }
	void reset() { mObj.reset(); }
	//@}  
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

#endif // end of 64-bit / 10.8+ test