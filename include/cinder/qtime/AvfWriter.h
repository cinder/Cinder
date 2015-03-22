#pragma once

#include <string>

#include "cinder/Cinder.h"
#include "cinder/ImageIo.h"
#include "cinder/Stream.h"

#if defined( CINDER_MAC ) || defined( CINDER_COCOA_TOUCH )
	#include "cinder/cocoa/CinderCocoa.h"
	#include <CoreVideo/CoreVideo.h>

	#if defined( CINDER_MAC )
	#elif defined( CINDER_COCOA_TOUCH )
	#endif

	#if defined( __OBJC__ )
		@class AVAsset, AVAssetWriter, AVAssetWriterInput, AVAssetWriterInputPixelBufferAdaptor, NSDate;

	#else
		class AVAsset;
		class AVAssetWriter;
		class AVAssetWriterInput;
		class AVAssetWriterInputPixelBufferAdaptor;

	#endif
#endif

// RE-IMPLEMENT
//typedef void* ICMCompressionSessionRef;
//typedef void* ICMCompressionSessionOptionsRef;
//typedef void* ICMEncodedFrameRef;
//typedef void* ICMMultiPassStorageRef;

namespace cinder { namespace qtime {

class MovieWriter;
typedef std::shared_ptr<MovieWriter> MovieWriterRef;

class MovieWriter {
  public:
	//! Defines the encoding parameters of a MovieWriter
	class Format {
	  public:
		Format();
		Format( uint32_t codec, float quality );
//		Format( const ICMCompressionSessionOptionsRef settings, uint32_t codec, float quality, float frameRate, bool enableMultiPass );
		Format( uint32_t codec, float quality, float frameRate, bool enableMultiPass = false );
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

		friend class MovieWriter;
	};


	MovieWriter() {}
	MovieWriter( const fs::path &path, int32_t width, int32_t height, const Format &format = Format::Format() );
	~MovieWriter();

	static MovieWriterRef	create( const fs::path &path, int32_t width, int32_t height, const Format &format = Format::Format() )
		{ return std::shared_ptr<MovieWriter>( new MovieWriter( path, width, height, format ) ); }

	//! Returns the Movie's default frame duration measured in seconds. You can also think of this as the Movie's frameRate.
	float	getDefaultDuration() const { return mFormat.mDefaultTime; }
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

	/** \brief Presents the user with the standard compression options dialog. Optional \a previewImage provides a still image as a preview (currently ignored on Mac OS X). Returns \c false if user cancelled.
		\image html qtime/MovieWriter/qtime_settings_small.png **/
	static bool getUserCompressionSettings( Format* result, ImageSourceRef previewImage = ImageSourceRef() );

	/** \brief Appends a frame to the Movie. The optional \a duration parameter allows a frame to be inserted for a time other than the Format's default duration.
		\note Calling addFrame() after a call to finish() will throw a MovieWriterExcAlreadyFinished exception.
	**/
//	void addFrame( const ImageSourceRef& imageSource, float duration = -1.0f );
	void addFrame( const Surface8u& imageSource, float duration = -1.0f );
	
	//! Returns the number of frames in the movie
	uint32_t	getNumFrames() const { return mNumFrames; }

	//! Completes the encoding of the movie and closes the file. Calling finish() more than once has no effect.
	void finish();
	
	enum { CODEC_H264 = 'avc1', CODEC_JPEG = 'jpeg', CODEC_MP4 = 'mp4v', CODEC_PNG = 'png ', CODEC_RAW = 'raw ', CODEC_ANIMATION = 'rle ' };

  private:
	void createCompressionSession();

	AVAssetWriter* mWriter;
	AVAssetWriterInput* mWriterSink;
	AVAssetWriterInputPixelBufferAdaptor* mSinkAdapater;
	
	fs::path		mPath;
	uint32_t		mNumFrames;
	int64_t			mCurrentTimeValue;
	int32_t			mWidth, mHeight;
	Format			mFormat;
	bool			mRequestedMultiPass, mDoingMultiPass, mFinished;

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