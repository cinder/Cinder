#pragma once


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
#include <condition_variable>

#include "cinder/gl/Texture.h"
#include "cinder/gl/Context.h"

#if GST_CHECK_VERSION(1, 4, 5)
	#include <gst/gl/gstglconfig.h>

	#if defined( CINDER_GL_ES )
		#undef GST_GL_HAVE_OPENGL
		#undef GST_GL_HAVE_GLES2
		#undef GST_GL_HAVE_PLATFORM_GLX
	#else // Desktop
		#undef GST_GL_HAVE_GLES2
		#undef GST_GL_HAVE_PLATFORM_EGL
		#undef GST_GL_HAVE_GLEGLIMAGEOES
	#endif

	#if defined( CINDER_MAC )
		// Avoid GL include clashes with glload.
		#undef GST_GL_HAVE_OPENGL
	#endif

	#include <gst/gl/gstglcontext.h>
	#include <gst/gl/gstgldisplay.h>
	#include <gst/gl/gstglmemory.h>

	#if defined( CINDER_LINUX_EGL_ONLY )
		#include <gst/gl/egl/gstgldisplay_egl.h>
	#elif defined( CINDER_LINUX )
		#include <gst/gl/x11/gstgldisplay_x11.h>
	#endif

	#define CINDER_GST_HAS_GL
#endif

#define CINDER_USE_PLAYBIN

namespace gst { namespace video {

struct GstCustomPipelineData {
	std::string pipeline;
	std::string video_sink;
	std::string audio_sink;
	std::string caps;
	std::string uri;
};

class GstPlayer;

struct GstData {
	GstData();
	~GstData();

	void reset();
	void prepareForNewVideo();
	void updateState( const GstState& current );

	std::atomic<bool> isPaused;

	std::atomic<bool> isBuffering; // Streaming.
	std::atomic<bool> isLive; // We disable buffering if on live sources ( webcams, etc. )
	std::atomic<gint64> position;

	std::atomic<int> width;
	std::atomic<int> height;
	std::atomic<bool> videoHasChanged; // did we load a new video ?
	std::atomic<GstVideoFormat> videoFormat;
	std::atomic<GstState> targetState, currentState;
	std::atomic<bool> isPrerolled;
	std::atomic<gint64> duration;
	std::atomic<bool> isDone;
	std::atomic<bool> isLoaded;
	std::atomic<bool> isPlayable;
	std::atomic<float> requestedSeekTime;
	std::atomic<bool> isSeeking;
	std::atomic<bool> requestedSeek;
	std::atomic<bool> loop;
	std::atomic<bool> palindrome;
	std::atomic<float> rate;
	std::atomic<bool> isStream;
	std::atomic<float> frameRate;
	std::atomic<int> numFrames;
	std::atomic<int> fpsNom;
	std::atomic<int> fpsDenom;
	std::atomic<float> pixelAspectRatio;

	GstMapInfo memoryMapInfo; // Memory map that holds the incoming frame.
	GstVideoInfo videoInfo; // For retrieving video info.
	GstElement* pipeline = nullptr;
	GstElement* appSink = nullptr;
	GstElement* videoBin = nullptr;
	GstPlayer* player = nullptr;
#if defined( CINDER_GST_HAS_GL )
	GstElement* glupload = nullptr;
	GstElement* glcolorconvert = nullptr;

	GstElement* rawCapsFilter = nullptr;
#endif
};

class GstPlayer {
	public:
		GstPlayer();
		virtual ~GstPlayer();

		bool initialize();

		void setCustomPipeline( const GstCustomPipelineData &customPipeline );

		void load( const std::string& path );
		bool newVideo() const;

		void play();
		void stop();

		int width() const;
		int height() const;

		bool isPrerolled() const;
		bool isPaused() const;
		bool isPlayable() const;
		bool isLoaded() const;
		bool isBuffering() const;
		bool isLiveSource() const;
		int stride() const;
		void setLoop( bool loop = true, bool palindrome = false );
		bool setRate( float rate );
		bool stepForward();

		float getRate() const;

		bool hasNewFrame() const;

		GstVideoFormat format() const;

		gint64 getPositionNanos();
		float getPositionSeconds();
		gint64 getDurationNanos();
		float getDurationSeconds();
		float getFramerate() const;
		float getPixelAspectRatio() const;
		int getNumFrames();

		bool hasAudio() const;
		bool hasVisuals() const;

		void setVolume( float targetVolume );
		float getVolume();

		bool isDone() const;

		GstElement* getPipeline();

		void seekToTime( float seconds, bool forceSeek = false );
		void seekToFrame( int frame );

		bool isStream() const;

		ci::gl::Texture2dRef getVideoTexture();

	private:
		bool initializeGStreamer();

		void constructPipeline();

		void startGMainLoopThread();
		void startGMainLoop( GMainLoop* loop );

		static void onGstEos( GstAppSink* sink, gpointer userData );
		static GstFlowReturn onGstSample( GstAppSink* sink, gpointer userData );
		static GstFlowReturn onGstPreroll( GstAppSink* sink, gpointer userData );
		void processNewSample( GstSample* sample );
		void getVideoInfo( const GstVideoInfo& videoInfo );

		bool setPipelineState( GstState targetState );
		bool checkStateChange( GstStateChangeReturn stateChangeResult );
		GstStateChangeReturn getStateChange();

		GstState getCurrentState();
		GstState getPendingState();

		bool sendSeekEvent( gint64 seekTime );

		void addBusWatch( GstElement* pipeline );

		void resetCustomPipeline();
		void resetPipeline();
		void resetBus();
		void cleanup();
		void resetVideoBuffers();
		void resetGLBuffers();
		void resetSystemMemoryBuffers();

		void createTextureFromMemory();
		void createTextureFromID();

	private:
		GMainLoop* mGMainLoop; // Needed for message activation since we are not using signals.
		GstBus* mGstBus; // Delivers the messages.
		int mBusId; // Save the id of the bus for releasing when not needed.
		std::thread mGMainLoopThread; // Seperate thread for GMainLoop.

		std::mutex mMutex; // Protect	since the appsink callbacks are executed from the streaming thread internally from GStreamer.

		bool mUsingCustomPipeline;
		GstData mGstData; // Data that describe the current state of the pipeline.

		ci::gl::Texture2dRef mVideoTexture;

		std::shared_ptr<unsigned char> mFrontVBuffer = nullptr;
		std::shared_ptr<unsigned char> mBackVBuffer = nullptr;

		std::shared_ptr<GstBuffer> mCurrentBuffer;
		std::shared_ptr<GstBuffer> mNewBuffer;

		GLint getTextureID( GstBuffer* newBuffer );

		std::atomic<bool> mNewFrame;
};

}} // namespace gst::video
