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

#include <gst/gl/gstglconfig.h>
#include <gst/gl/gstglcontext.h>
#include <gst/gl/gstgldisplay.h>
#include <gst/gl/x11/gstgldisplay_x11.h>


namespace gst { namespace video {

 
    struct GstCustomPipelineData{
        std::string pipeline;
        std::string video_sink;
        std::string audio_sink;
        std::string caps;
        std::string uri;
    };
    
    struct GstData{
        GstData();
        
        void reset() {
            mIsPrerolled = false;
            mVideoHasChanged = false;
            mPosition = 0;
            mWidth = -1;
            mHeight = -1;
            mAsyncStateChangePending = false;
            mTargetState = mCurrentState = GST_STATE_NULL;
        }
        
        std::atomic<bool> mPaused;
        std::atomic<bool> mIsBuffering; // Streaming..
        std::atomic<bool> mIsLive; // We disable buffering if on live sources ( webcams, etc. )
        std::atomic<gint64> mPosition;
        std::atomic<int> mWidth;
        std::atomic<int> mHeight;
        std::atomic<bool> mVideoHasChanged; // did we load a new video ?
        std::atomic<GstVideoFormat> mVideoFormat;
        std::atomic<bool> mAsyncStateChangePending;
        std::atomic<GstState> mTargetState, mCurrentState;
        std::atomic<bool> mIsPrerolled;
        std::atomic<gint64> mDuration;
        std::atomic<bool> mIsDone;
        std::atomic<bool> mIsLoaded;
        std::atomic<bool> mIsPlayable;
        std::atomic<gint64> mRequestedSeekTime;
        std::atomic<bool> mRequestedSeek;
        std::atomic<bool> mLoop;
        std::atomic<bool> mPalindrome;
        std::atomic<float> mRate;
        std::atomic<bool> mIsStream;

	GstGLContext* mCinderContext = nullptr;
	GstGLDisplay* mCinderDisplay = nullptr;
    };
    
    class GstPlayer
    {
    public:
        GstPlayer();
        ~GstPlayer();
        
        bool initialize();
        
        void setCustomPipeline( const GstCustomPipelineData &customPipeline );
        
        void load( std::string _path );
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
        int  stride() const;
        void setLoop( bool loop = true, bool palindrome = false );
        void setRate( float rate );
        
        float getRate() const;
        
        bool hasNewFrame() const;
        
        GstVideoFormat format() const;
        
        gint64 getPositionNanos();
        float getPositionSeconds();
        gint64 getDurationNanos();
        float getDurationSeconds();
        
        void setVolume( float targetVolume );
        float getVolume();
        
        bool isDone() const;
        
        GstElement* getPipeline();
        
        void seekToTime( float seconds );
        
        bool isStream() const;

        GstData& getGstData();

	ci::gl::Texture2dRef getVideoTexture();
    private:
        
        bool initializeGStreamer();
        void startGMainLoopThread();
        void startGMainLoop( GMainLoop* loop );
        
        // Fired from appsink..
        static void onGstEos( GstAppSink* sink, gpointer userData );
        static GstFlowReturn onGstPreroll( GstAppSink* sink, gpointer userData );
        static GstFlowReturn onGstSample( GstAppSink* sink, gpointer userData );
        // ..and forwarded to the following.
        void eos();
        void sample( GstSample* sample, GstAppSink* sink );
        
        // States..
        bool setPipelineState( GstState targetState );
        bool checkStateChange( GstStateChangeReturn stateChangeResult, const GstState & target );
        GstStateChangeReturn getStateChange();

        GstState getCurrentState();
        GstState getPendingState();
        
        bool sendSeekEvent( gint64 seekTime );
        
        void addBusWatch( GstElement* pipeline );
        // Resets.
        void resetCustomPipeline();
        void resetPipeline();
        void resetBus();
        void cleanup();
        void resetVideoBuffers();

        void updateTexture( GstSample* sample, GstAppSink* sink );
    private:
        GMainLoop* mGMainLoop; // Needed for message activation since we are not using signals.
        GstBus* mGstBus; // Delivers the messages.
        int  busID; // Save the id of the bus for releasing when not needed.
        std::thread mGMainLoopThread; // Seperate thread for GMainLoop.
        
        GstMapInfo mMemoryMapInfo; // Memory map that holds the Gst_GL texture ID.
        GstVideoInfo mVideoInfo; // For retrieving video info.
        GstElement* mGstPipeline; // Our pipeline.
        GstElement* mGstAppSink; // Raw buffer destination and eos.
        
        std::mutex mMutex; // Protect  since the appsink callbacks are executed from the streaming thread internally from GStreamer.
        
        bool mUsingCustomPipeline;
        GstData mGstData; // Data that describe the current state of the pipeline.
        
        std::atomic<int> mStride; // Video stride.. This needs more thinking..
        std::atomic<bool> mNewFrame;

	ci::gl::Texture2dRef videoTexture;
	GLint mGstTextureID;
	
	GAsyncQueue* queue_input_buf = nullptr;
	GAsyncQueue* queue_output_buf = nullptr;
    };
    
} }
