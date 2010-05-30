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

// These forward declarations prevent us from having to bring all of QuickTime into the global namespace in MSW
#if defined( CINDER_MSW )
	typedef struct ComponentInstanceRecord		ComponentInstanceRecord;
	typedef ComponentInstanceRecord *			ComponentInstance;
	typedef ComponentInstance					DataHandler;
	typedef struct TrackType**					Track;
	typedef struct MediaType**					Media;
	typedef struct OpaqueICMCompressionSession* ICMCompressionSessionRef;
	typedef const struct OpaqueICMEncodedFrame* ICMEncodedFrameRef;
	typedef signed long							OSStatus;
	typedef unsigned long						CodecType;
#endif // defined( CINDER_MSW )

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
	MovieWriter() {}
	MovieWriter( const std::string &path, int32_t width, int32_t height, MovieWriterCodecType codec = RAW, MovieWriterQuality quality = HIGH );

	void addFrame( const ImageSourceRef &imageSource ) { mObj->addFrame( imageSource ); }
	void finish() { mObj->finish(); }
	
  private:
	/// \cond
	struct Obj {
		Obj( const std::string &path, int32_t width, int32_t height, MovieWriterCodecType type = RAW, MovieWriterQuality quality = HIGH );
		~Obj();
		
		void	addFrame( const ImageSourceRef &imageSource );
		void	createCompressionSession();
		void	finish();
		
		static OSStatus encodedFrameOutputCallback( void *refCon, ::ICMCompressionSessionRef session, OSStatus err, ICMEncodedFrameRef encodedFrame, void *reserved );

		::Movie						mMovie;
		::DataHandler				mDataHandler;
		::Track						mTrack;
		::Media						mMedia;
		::ICMCompressionSessionRef	mCompressionSession;

		std::string		mPath;
		::CodecType		mCodec;
		::TimeValue		mCurrentTimeValue;
		
		int32_t		mWidth, mHeight;
		int			mQuality;
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

} } // namespace cinder::qtime