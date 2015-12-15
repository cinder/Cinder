#pragma once

#include "cinder/gl/Texture.h"

#ifdef __linux__
#include <cstring>
#endif

#include <iostream>

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/video/video.h>
#define GST_USE_UNSTABLE_API

#include <atomic>
#include <thread>
#include <mutex>

#if GST_CHECK_VERSION(1, 4, 5)
	#include <gst/gl/gstglconfig.h>

	#if defined( CINDER_GL_ES )
		#undef GST_GL_HAVE_OPENGL
		#undef GST_GL_HAVE_PLATFORM_GLX
	#else // Desktop
		#undef GST_GL_HAVE_GLES2
		#undef GST_GL_HAVE_PLATFORM_EGL
		#undef GST_GL_HAVE_GLEGLIMAGEOES
	#endif

	#include <gst/gl/gstglcontext.h>
	#include <gst/gl/gstgldisplay.h>

	#if defined( CINDER_LINUX_EGL_ONLY )
		#include <gst/gl/egl/gstgldisplay_egl.h>
	#else
		#include <gst/gl/x11/gstgldisplay_x11.h>
	#endif

	#define CINDER_GST_HAS_GL
#endif

namespace gst { namespace video {
 
	struct GstCustomPipelineData {
		std::string pipeline;
		std::string video_sink;
		std::string audio_sink;
		std::string caps;
		std::string uri;
	};
	
	struct GstData {
		GstData();

		void reset();

		std::atomic<bool> 			mPaused;
		std::atomic<bool> 			mIsBuffering; // Streaming..
		std::atomic<bool> 			mIsLive; // We disable buffering if on live sources ( webcams, etc. )
		std::atomic<gint64> 		mPosition;
		std::atomic<int> 			mWidth;
		std::atomic<int>			mHeight;
		std::atomic<bool> 			mVideoHasChanged; // did we load a new video ?
		std::atomic<GstVideoFormat>	mVideoFormat;
		std::atomic<bool> 			mAsyncStateChangePending;
		std::atomic<GstState>		mTargetState, mCurrentState;
		std::atomic<bool> 			mIsPrerolled;
		std::atomic<gint64> 		mDuration;
		std::atomic<bool> 			mIsDone;
		std::atomic<bool> 			mIsLoaded;
		std::atomic<bool> 			mIsPlayable;
		std::atomic<gint64> 		mRequestedSeekTime;
		std::atomic<bool> 			mRequestedSeek;
		std::atomic<bool> 			mLoop;
		std::atomic<bool> 			mPalindrome;
		std::atomic<float> 			mRate;
		std::atomic<bool> 			mIsStream;
		std::atomic<bool> 			mHasAudio;
		std::atomic<float> 			mFrameRate;

#if defined( CINDER_GST_HAS_GL )
		GstGLContext* mCinderContext = nullptr;
		GstGLDisplay* mCinderDisplay = nullptr;
#endif

		GstElement* mUriDecode 		= nullptr;
		GstElement* mGLupload 		= nullptr;
		GstElement* mGLcolorconvert = nullptr;
		GstElement* mVideoconvert   = nullptr;
		GstElement* mAudioconvert 	= nullptr;
		GstElement* mAudiosink 		= nullptr;
		GstElement* mAudioQueue 	= nullptr;
		GstElement* mVideoQueue 	= nullptr;
	};
	
	class GstPlayer {
	public:
		GstPlayer();
		virtual ~GstPlayer();
		
		bool 					initialize();
		
		void 					setCustomPipeline( const GstCustomPipelineData &customPipeline );
		
		void 					load( const std::string& path );
		bool 					newVideo() const;
		
		void 					play();
		void 					stop();
		
		int 					width() const;
		int 					height() const;
		
		bool 					isPrerolled() const;
		bool 					isPaused() const;
		bool 					isPlayable() const;
		bool 					isLoaded() const;
		bool 					isBuffering() const;
		bool 					isLiveSource() const;
		int  					stride() const;
		void 					setLoop( bool loop = true, bool palindrome = false );
		bool 					setRate( float rate );
		
		float 					getRate() const;
		
		bool 					hasNewFrame() const;
		
		GstVideoFormat			format() const;
		
		gint64 					getPositionNanos();
		float 					getPositionSeconds();
		gint64 					getDurationNanos();
		float 					getDurationSeconds();
        float                   getFramerate() const;
		
        bool                    hasAudio() const;

		void 					setVolume( float targetVolume );
		float 					getVolume();
		
		bool 					isDone() const;
		
		GstElement* 			getPipeline();
		
		void 					seekToTime( float seconds );
		
		bool 					isStream() const;

		GstData&				getGstData();

		ci::gl::Texture2dRef	getVideoTexture();


	private:		
		bool 					initializeGStreamer();

		void 					constructGLPipeline();
        void                    constructPipeline();

		void 					startGMainLoopThread();
		void 					startGMainLoop( GMainLoop* loop );
		
		// Fired from appsink..
		static void 			onGstEos( GstAppSink* sink, gpointer userData );
		static GstFlowReturn	onGstPreroll( GstAppSink* sink, gpointer userData );
		static GstFlowReturn 	onGstSample( GstAppSink* sink, gpointer userData );
		// ..and forwarded to the following.
		void 					eos();
		void 					sample( GstSample* sample, GstAppSink* sink );
		
		// States..
		bool 					setPipelineState( GstState targetState );
		bool 					checkStateChange( GstStateChangeReturn stateChangeResult, const GstState & target );
		GstStateChangeReturn 	getStateChange();

		GstState 				getCurrentState();
		GstState 				getPendingState();
		
		bool					 sendSeekEvent( gint64 seekTime );
		
		void					addBusWatch( GstElement* pipeline );

		// Resets.
		void 					resetCustomPipeline();
		void 					resetPipeline();
		void 					resetBus();
		void 					cleanup();
		void 					resetVideoBuffers();
		void 					resetGLBuffers();
		void 					resetSystemMemoryBuffers();
		void 					unlinkAudioBranch();

		void 					createTextureFromMemory();
		void 					createTextureFromID();
		void 					updateTexture( GstSample* sample, GstAppSink* sink );

	private:
		GMainLoop* 				mGMainLoop; // Needed for message activation since we are not using signals.
		GstBus* 				mGstBus; // Delivers the messages.
		int  					mBusId; // Save the id of the bus for releasing when not needed.
		std::thread	 			mGMainLoopThread; // Seperate thread for GMainLoop.
		
		GstMapInfo 				mMemoryMapInfo; // Memory map that holds the Gst_GL texture ID.
		GstVideoInfo 			mVideoInfo; // For retrieving video info.
		GstElement* 			mGstPipeline; // Our pipeline.
		GstElement* 			mGstAppSink; // Raw buffer destination and eos.
		
		std::mutex 				mMutex; // Protect  since the appsink callbacks are executed from the streaming thread internally from GStreamer.
		
		bool 					mUsingCustomPipeline;
		GstData 				mGstData; // Data that describe the current state of the pipeline.
		
		std::atomic<bool> 		mNewFrame;

		ci::gl::Texture2dRef	videoTexture;
		GLint 					mGstTextureID;
	
		GAsyncQueue*			mInputVideoBuffers = nullptr;
		GAsyncQueue* 			mOutputVideoBuffers = nullptr;
		
		unsigned char* 			mFrontVBuffer = nullptr;
		unsigned char* 			mBackVBuffer = nullptr;		
	};
	
}} // namespace gst::video
