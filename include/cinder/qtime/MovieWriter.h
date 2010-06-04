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

#include "cinder/Cinder.h"
#include "cinder/qtime/QuickTime.h"
#include "cinder/ImageIo.h"

#include <string>

// These forward declarations prevent us from having to bring all of QuickTime into the global namespace in MSW
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
#endif // defined( CINDER_MSW )

namespace cinder { namespace qtime {

class MovieWriter {
	struct Obj;

  public:
	class Format {
	  public:
		Format();
		Format( uint32_t codec, float quality );
		Format( const ICMCompressionSessionOptionsRef settings, uint32_t codec, float quality, float frameRate );
		Format( const Format &format );
		~Format();

		const Format& operator=( const Format &format );

		//! Returns the four character code for the QuickTime codec. Types can be found in QuickTime's ImageCompression.h.
		uint32_t	getCodec() const { return mCodec; }
		//! Sets the four character code for the QuickTime codec. Defaults to PNG ('png '). Additional types can be found in QuickTime's ImageCompression.h.
		Format&		setCodec( uint32_t codec ) { mCodec = codec; return *this; }
		//! Sets the overall quality for encoding. Must be in a range of [0,1.0]. Defaults to 0.99. 1.0 corresponds to lossless.
		Format&		setQuality( float quality );
		//! Sets the standard duration of a frame. Defaults to 1/30, meaning 30Hz.
		Format&		setDefaultDuration( float defaultTime ) { mDefaultTime = defaultTime; return *this; }
		//! Sets the integer base value for encoding time scale. Defaults to 600.
		Format&		setTimeScale( long timeScale ) { mTimeBase = timeScale; return *this; }
		//! Enables temporal compression (allowing \b P or \b B frames). Defaults to true.
		Format&		enableTemporal( bool enable = true );
		//! Enables frame reordering. Defaults to true. In order to encode B frames, a compressor must reorder frames, which means that the order in which they will be emitted and stored (the decode order) is different from the order in which they were presented to the compressor (the display order).
		Format&		enableReordering( bool enable = true );
		//! Sets the maximum number of frames between key frames. Default is 0, which indicates that the compressor should choose where to place all key frames. Compressors are allowed to generate key frames more frequently if this would result in more efficient compression.
		Format&		setMaxKeyFrameRate( int32_t rate );
		//! Sets whether a codec is allowed to change frame times. Defaults to true. Some compressors are able to identify and coalesce runs of identical frames and output single frames with longer duration, or output frames at a different frame rate from the original.
		Format&		enableFrameTimeChanges( bool enable );

	  private:
		void		initDefaults();

		uint32_t	mCodec;
		long		mTimeBase;
		float		mDefaultTime;

		ICMCompressionSessionOptionsRef		mOptions;

		friend MovieWriter;
		friend Obj;
	};


	MovieWriter() {}
	MovieWriter( const std::string &path, int32_t width, int32_t height, const Format &format = Format::Format() );

	//! Returns the Movie's default frame duration measured in seconds. You can also think of this as the Movie's frameRate.
	float	getDefaultDuration() const { return mObj->mFormat.mDefaultTime; }
	//! Returns the width of the Movie in pixels
	int32_t	getWidth() const { return mObj->mWidth; }
	//! Returns the height of the Movie in pixels
	int32_t	getHeight() const { return mObj->mHeight; }
	//! Returns the size of the Movie in pixels
	Vec2i	getSize() const { return Vec2i( getWidth(), getHeight() ); }
	//! Returns the Movie's aspect ratio, which is its width / height
	float	getAspectRatio() const { return getWidth() / (float)getHeight(); }
	//! Returns the bounding Area of the Movie in pixels: [0,0]-(width,height)
	Area	getBounds() const { return Area( 0, 0, getWidth(), getHeight() ); }

	//! Returns the Movie's Format
	const Format&	getFormat() const { return mObj->mFormat; }

	//! Presents the user with the standard compression options dialog. Optional \a previewImage provides a still image as a preview. Returns \c false if user cancelled.
	static bool		getUserCompressionSettings( Format *result, ImageSourceRef previewImage = ImageSourceRef() );

	/** \brief Appends a frame to the Movie. The optional \a duration parameter allows a frame to be inserted for a time other than the Format's default duration.
		\note Calling addFrame() after a call to finish() will throw a MovieWriterExcAlreadyFinished exception. **/
	void addFrame( const ImageSourceRef &imageSource, float duration = -1.0f ) { mObj->addFrame( imageSource, duration ); }
	
	//! Completes the encoding of the movie and closes the file. Calling finish() more than once has no effect.
	void finish() { mObj->finish(); }
	
	enum { CODEC_H264 = 'avc1', CODEC_JPEG = 'jpeg', CODEC_MP4 = 'mp4v', CODEC_PNG = 'png ', CODEC_RAW = 'raw ', CODEC_ANIMATION = 'rle ' };

  private:
	/// \cond
	struct Obj {
		Obj( const std::string &path, int32_t width, int32_t height, const Format &format );
		~Obj();
		
		void	addFrame( const ImageSourceRef &imageSource, float duration );
		void	createCompressionSession();
		void	finish();
		
		static OSStatus encodedFrameOutputCallback( void *refCon, ::ICMCompressionSessionRef session, OSStatus err, ICMEncodedFrameRef encodedFrame, void *reserved );

		::Movie						mMovie;
		::DataHandler				mDataHandler;
		::Track						mTrack;
		::Media						mMedia;
		::ICMCompressionSessionRef	mCompressionSession;

		std::string		mPath;
		::TimeValue		mCurrentTimeValue;
		
		int32_t		mWidth, mHeight;
		Format		mFormat;
		bool		mFinished;
	};
	/// \endcond
	
	shared_ptr<Obj>		mObj;  

  public:
	//@{
	//! Emulates shared_ptr-like behavior
	typedef shared_ptr<Obj> MovieWriter::*unspecified_bool_type;
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