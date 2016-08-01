 
// Quell the GL macro redefined warnings.
#if defined( __CLANG__ )
	#pragma diagnostic push
	#pragma diagnostic ignored "-Wmacro-redefined"
#else // GCC
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wmacro-redefined"	
#endif

#include "cinder/linux/GstPlayer.h"

#if defined( CINDER_GST_HAS_GL ) && ! defined( CINDER_MAC )
	#if ! defined( CINDER_LINUX_EGL_ONLY )
		// These files will include a glfw_config.h that's custom to Cinder.
		#include "glfw/glfw3.h"
		#include "glfw/glfw3native.h"
	#endif
#endif

#include "cinder/app/AppBase.h"
#include "cinder/gl/Environment.h"
#include <chrono>

namespace gst { namespace video {

static bool         sUseGstGl = false;
static const int    sEnableAsyncStateChange = false;

GstData::GstData()
    : isPaused( false ),
    isBuffering( false ),
    isLive( false ),
    position( -1 ),
    width( -1 ),
    height( -1 ),
    videoHasChanged( false ),
    videoFormat( GST_VIDEO_FORMAT_RGBA ),
    currentState( GST_STATE_NULL ),
    targetState( GST_STATE_NULL ),
    isPrerolled( false ),
    duration( -1 ),
    isDone( false ),
    isLoaded( false ),
    isPlayable( false ),
    requestedSeekTime( -1 ),
    requestedSeek( false ),
    loop( false ),
    palindrome( false ),
    rate( 1.0f ),
    isStream( false ),
    frameRate( -1.0f ),
    numFrames( 0 ),
    pixelAspectRatio( 0.0f ),
    player( nullptr )
{
}

GstData::~GstData()
{
    if( player ) player = nullptr;
}

void GstData::reset() 
{
    player = nullptr;
}

void GstData::prepareForNewVideo()
{
    videoHasChanged 	= true;
    isPaused 		= false;
    isBuffering 	= false;
    isLive 		= false;
    position 		= -1;
    isPrerolled 	= false;
    isBuffering		= false;
    position 		= 0;
    width 	        = -1;
    height	        = -1;
    rate 		= 1.0f;
    frameRate		= -1;
    numFrames		= 0;
}

void GstData::updateState( const GstState& current )
{
    currentState = current;

    switch ( currentState ) {
            case GST_STATE_NULL: {
                    reset();
                    break;
            }
            case GST_STATE_READY: {
                    prepareForNewVideo();
                    break;
            }
            case GST_STATE_PAUSED: {
                    isPrerolled     = true;
                    isPaused 	    = true;
                    isLoaded 	    = true;
                    isPlayable 	    = true;
                    break;
            }
            case GST_STATE_PLAYING: {
                    isDone          = false;
                    isPaused        = false;	
                    isPlayable      = true;
                    break;
            }
            default: break;
    }
}


gboolean checkBusMessages( GstBus* bus, GstMessage* message, gpointer userData )
{
    if( ! userData ){
        return true;
    }

    GstData & data = *( static_cast<GstData*>( userData ) );
    
    switch ( GST_MESSAGE_TYPE( message ) )
    {
        case GST_MESSAGE_ERROR: {
            GError *err = nullptr;
            gchar *dbg;
            gst_message_parse_error( message, &err, &dbg );
            gst_object_default_error( message->src, err, dbg );
            g_error_free ( err );
            g_free ( dbg );

            GstStateChangeReturn state = gst_element_set_state( data.pipeline, GST_STATE_NULL );
            if( state == GST_STATE_CHANGE_FAILURE ) {
                g_error( "Failed to set the pipeline to nullptr state after ERROR occured\n" );
            }
            break;
        }

        case GST_MESSAGE_HAVE_CONTEXT: {
            GstContext *context = nullptr;
            const gchar *context_type = nullptr;
            gchar *context_str = nullptr;

            gst_message_parse_have_context( message, &context );
            context_type = gst_context_get_context_type( context );
            context_str = gst_structure_to_string( gst_context_get_structure( context ) );
            g_print( "Have context %s from element %s\n", context_type, GST_ELEMENT_NAME( GST_MESSAGE_SRC( message ) ) );
            g_free( context_str );

            if( context ) {
                gst_context_unref( context );
            }
            break;
        }

#if defined( CINDER_GST_HAS_GL )
        case GST_MESSAGE_NEED_CONTEXT: {
            const gchar *context_type = nullptr;
            gst_message_parse_context_type( message, &context_type );

            g_print( "Need context %s from element %s\n", context_type, GST_ELEMENT_NAME( GST_MESSAGE_SRC( message ) ) );

            GstContext* context = nullptr;
            if( g_strcmp0( context_type, GST_GL_DISPLAY_CONTEXT_TYPE ) == 0 ) {
                context = gst_context_new( GST_GL_DISPLAY_CONTEXT_TYPE, TRUE );
                gst_context_set_gl_display( context, data.display );
                gst_element_set_context( GST_ELEMENT( message->src ), context );
            }
            else if( g_strcmp0( context_type, "gst.gl.app_context" ) == 0 ) {
                context = gst_context_new( "gst.gl.app_context", TRUE );
                GstStructure *s = gst_context_writable_structure( context );
                gst_structure_set( s, "context", GST_GL_TYPE_CONTEXT, data.context, nullptr );
                gst_element_set_context( GST_ELEMENT( message->src ), context );
            }

            if( context ) { 
                gst_context_unref( context ); 
            }
            break;
        }
#endif

        case GST_MESSAGE_BUFFERING: {
            if( data.isLive ) break; // No buffering for live sources.
            gint percent = 0;
            gst_message_parse_buffering( message, &percent );
            g_print( "Buffering (%3d%%)\r", percent );
            if( percent == 100 ) {
                data.isBuffering = false;
                g_print( " Buffering Complete ! \n" );
                if( data.targetState == GST_STATE_PLAYING ) {
                    gst_element_set_state( data.pipeline, GST_STATE_PLAYING );
                }
            }
            else {
                if( ! data.isBuffering && data.targetState == GST_STATE_PLAYING ) {
                    gst_element_set_state( data.pipeline, GST_STATE_PAUSED );
                    g_print(  " Buffering in process....\n" );
                }
                data.isBuffering = true;
            }
            break;
        }

        // Possible due to network connection error when streaming..
        case GST_MESSAGE_CLOCK_LOST: {
            // Get a new clock
            gst_element_set_state( data.pipeline, GST_STATE_PAUSED );
            gst_element_set_state( data.pipeline, GST_STATE_PLAYING );
            break;
        }

        case GST_MESSAGE_DURATION_CHANGED: {
            break;
        }

        case GST_MESSAGE_STATE_CHANGED: {
            if( GST_MESSAGE_SRC( message ) == GST_OBJECT( data.pipeline ) ) {

                GstState old, current, pending;
                gst_message_parse_state_changed( message, &old, &current, &pending );
                if( old != current ) {
                    g_print( "Pipeline state changed from : %s to %s with pending %s\n", gst_element_state_get_name( old ), gst_element_state_get_name ( current ), gst_element_state_get_name( pending) );
                }

                data.updateState(  current );
            }
            break;
        }
        case GST_MESSAGE_ASYNC_DONE: {

            if( data.player ) data.position = data.player->getPositionNanos();

            switch( data.currentState ) {
                    case GST_STATE_PAUSED: {
                            if( data.requestedSeek  ) {
                                if( data.player ) data.player->seekToTime( data.requestedSeekTime );
                                    data.requestedSeek = false;
                                }
                            break;
                    }
                    default: break;
            }
            break;
        }

        case GST_MESSAGE_EOS: {
            if( data.loop ) {
                if( data.palindrome && ! data.isStream && ! data.isLive ) {
                    // Toggle the direction we are playing.
                    if( data.player ) data.player->setRate( -data.player->getRate() );
                }
                else {
                    // If playing back on reverse start the loop from the
                    // end of the file
                    if( data.rate < 0 ){
                        if( data.player ) data.player->seekToTime( data.player->getDurationSeconds() );
                    }
                    else{
                        // otherwise restart from beginning.
                        if( data.player ) data.player->seekToTime( 0 );
                    }
                }
            }
            data.videoHasChanged = false;
            data.isDone = true;
            break;
        }

        default: break;
    }

    return true; 
}

GstPlayer::GstPlayer()
    : mGMainLoop( nullptr ),
    mGstBus( nullptr ),
    mNewFrame( false ),
    mUnblockStreamingThread( false ),
    mUsingCustomPipeline( false )
{
    bool success = initializeGStreamer();

    if( success ) {
        mGstData.player = this;
        startGMainLoopThread();
    }
}

GstPlayer::~GstPlayer()
{
    cleanup();
}

void GstPlayer::cleanup()
{
    if( mGstData.pipeline != nullptr ) {
        resetPipeline();
        // Reset the bus since the associated pipeline got resetted.
        resetBus();
    }

    resetVideoBuffers();
    
    if( g_main_loop_is_running( mGMainLoop ) ) {
        g_main_loop_quit( mGMainLoop );
        g_main_loop_unref( mGMainLoop );
    }
	
    if( mGMainLoopThread.joinable() ){
        mGMainLoopThread.join();
    }
}

void GstPlayer::startGMainLoopThread()
{   
    mGMainLoop = g_main_loop_new( nullptr, false );
    mGMainLoopThread = std::thread( &GstPlayer::startGMainLoop, this, mGMainLoop );
}

void GstPlayer::startGMainLoop( GMainLoop* loop )
{
    g_main_loop_run( loop );
}

bool GstPlayer::initializeGStreamer()
{
    if( ! gst_is_initialized() ) {		
        guint major;
        guint minor;
        guint micro;
        guint nano;
        
        gst_version( &major, &minor, &micro, &nano );
        
        GError* err;
        /// If we havent already initialized GStreamer do this.
        if( ! gst_init_check( nullptr, nullptr, &err ) ) {
                if( err->message ) {
                        g_error( "FAILED to initialize GStreamer : %s\n", err->message );
                }
                else {
                        g_error( "FAILED to initialize GStreamer due to unknown error ...\n" );
                }
                return false;
        }
        else {
                if( major >= 1 && minor >= 6 ) {
                        sUseGstGl = true;
                }
                else {
                        sUseGstGl = false;
                }
                g_print( "Initialized GStreamer version %i.%i.%i.%i\n", major, minor, micro, nano );
                return true;
        }
    }
    return true;
}

void GstPlayer::unblockStreamingThread()
{
    mMutex.lock();
    mUnblockStreamingThread = true;
    mMutex.unlock();
    mStreamingThreadCV.notify_one();
}

void GstPlayer::resetPipeline()
{
    if( ! mGstData.pipeline ) {
        return;
    }

    // Unblock the streaming thread for executing state change.
    unblockStreamingThread();

    gst_element_set_state( mGstData.pipeline, GST_STATE_NULL );
    gst_element_get_state( mGstData.pipeline, nullptr, nullptr, GST_CLOCK_TIME_NONE );
    
    gst_object_unref( GST_OBJECT( mGstData.pipeline ) );
    mGstData.pipeline = nullptr;
    
    if( sUseGstGl ) {
#if defined( CINDER_GST_HAS_GL )
        mGstData.videoBin = nullptr;
        mGstData.context = nullptr;
        mGstData.display = nullptr;
        mGstData.glupload = nullptr;
        mGstData.glcolorconvert = nullptr;
        if( mGstData.bufferQueue ){
            resetGLBuffers(); // GstBuffers need to be flushed here if we are destroying the pipeline.
            g_async_queue_unref( mGstData.bufferQueue );
            mGstData.bufferQueue = nullptr;
        }
#endif
    }
}
	
void GstPlayer::resetBus()
{
    bool success = g_source_remove( mBusId );
    if( success ) {
        mBusId = -1;
    }
    mGstBus = nullptr;
}

void GstPlayer::setCustomPipeline( const GstCustomPipelineData &customPipeline )
{
    // Similar to loading and maybe merged at some point.
    // If we have a custom pipeline we reset before re-using.
    if( mGstData.pipeline ) {
        resetPipeline();
        // Reset the bus since the associated pipeline got destroyed.
        resetBus();
    }

    GError *error = nullptr;
    mGstData.pipeline = gst_parse_launch( customPipeline.pipeline.c_str(), &error );
    
    if( error != nullptr ) {
        g_print( "Could not construct custom pipeline: %s\n", error->message );
        g_error_free (error);
    }
	
    // Prepare for loading custom pipeline.
    // We lock until we switch to GST_STATE_READY since there is not much to do if we dont reach at least there.
    gst_element_set_state( mGstData.pipeline, GST_STATE_READY );
    gst_element_get_state( mGstData.pipeline, NULL, NULL, GST_CLOCK_TIME_NONE );

    if( mGstData.pipeline ) {
        // Currently assumes that the pipeline has an appsink named 'videosink'.
        // This is just for testing and it has to be more generic.
        mGstData.appSink = gst_bin_get_by_name( GST_BIN( mGstData.pipeline ), "videosink" );
    }
	
    addBusWatch( mGstData.pipeline );
	
    if( mGstData.appSink ) {
        GstAppSinkCallbacks 	    	appSinkCallbacks;
        appSinkCallbacks.eos 		= onGstEos;
        appSinkCallbacks.new_preroll 	= onGstPreroll;
        appSinkCallbacks.new_sample 	= onGstSample;

        gst_app_sink_set_callbacks( GST_APP_SINK( mGstData.appSink ), &appSinkCallbacks, this, 0 );
    }

    mUsingCustomPipeline = true;

    // Reset the buffers for the next pre-roll.
    resetVideoBuffers();

    // async pre-roll
    gst_element_set_state( mGstData.pipeline, GST_STATE_PAUSED );
}
	
void GstPlayer::addBusWatch( GstElement* pipeline )
{
    mGstBus = gst_pipeline_get_bus( GST_PIPELINE( pipeline ) );
    mBusId  = gst_bus_add_watch( mGstBus, checkBusMessages, &mGstData );
    gst_object_unref( mGstBus );
}

bool GstPlayer::initialize()
{		
#if defined( CINDER_GST_HAS_GL ) 
    if( sUseGstGl ) {
#if defined( CINDER_LINUX )
        auto platformData   = std::dynamic_pointer_cast<ci::gl::PlatformDataLinux>( ci::gl::context()->getPlatformData() );
#if defined( CINDER_LINUX_EGL_ONLY )
        mGstData.display    = (GstGLDisplay*) gst_gl_display_egl_new_with_egl_display( platformData->mDisplay );
        mGstData.context    = gst_gl_context_new_wrapped( (GstGLDisplay*)mGstData.display, (guintptr)platformData->mContext, GST_GL_PLATFORM_EGL, GST_GL_API_GLES2 );
#else
        mGstData.display    = (GstGLDisplay*) gst_gl_display_x11_new_with_display( ::glfwGetX11Display() );
  #if defined( CINDER_GL_ES )
        mGstData.context    = gst_gl_context_new_wrapped( (GstGLDisplay*)mGstData.display, (guintptr)::glfwGetEGLContext( platformData->mContext ), GST_GL_PLATFORM_GLX, GST_GL_API_GLES2 );
  #else
        mGstData.context    = gst_gl_context_new_wrapped( (GstGLDisplay*)mGstData.display, (guintptr)::glfwGetGLXContext( platformData->mContext ), GST_GL_PLATFORM_GLX, GST_GL_API_OPENGL );
  #endif 
#endif 
#elif defined( CINDER_MAC )
   	 mGstData.display   = gst_gl_display_new ();
	 auto platformData  = std::dynamic_pointer_cast<ci::gl::PlatformDataMac>( ci::gl::context()->getPlatformData() );
	 mGstData.context   = gst_gl_context_new_wrapped( GST_GL_DISPLAY( mGstData.display ), (guintptr)platformData->mCglContext, GST_GL_PLATFORM_CGL, GST_GL_API_OPENGL );
#endif
    }
#endif // defined( CINDER_GST_HAS_GL )
    return true;
}

void GstPlayer::resetCustomPipeline()
{
    // In case we use load after we 've set a custom pipeline ..
    if( mGstData.pipeline ) {
        resetPipeline(); // reset the custom pipeline
        resetBus(); // reset the associated bus
        mUsingCustomPipeline = false;
    }
}

void GstPlayer::constructPipeline()
{
    if( mGstData.pipeline ) return;

    mGstData.pipeline   = gst_element_factory_make( "playbin", "playbinsink" );
    if( ! mGstData.pipeline ) {
        // Not much we can do without at least playbin...
        g_printerr( "Failed to create playbin pipeline!\n" );
        return;
    }

    mGstData.videoBin 	= gst_bin_new( "cinder-vbin" );
    if( ! mGstData.videoBin ) g_printerr( "Failed to create video bin!\n" );

    mGstData.appSink 	= gst_element_factory_make( "appsink", "videosink" );
    if( ! mGstData.appSink ) {
        g_printerr( "Failed to create app sink element!\n" );
    }
    else {
        gst_app_sink_set_max_buffers( GST_APP_SINK( mGstData.appSink ), 1 );
        gst_app_sink_set_drop( GST_APP_SINK( mGstData.appSink ), true );
        gst_base_sink_set_qos_enabled( GST_BASE_SINK( mGstData.appSink ), true );
        gst_base_sink_set_sync( GST_BASE_SINK( mGstData.appSink ), true );
        gst_base_sink_set_max_lateness( GST_BASE_SINK( mGstData.appSink ), 20 * GST_MSECOND );

        GstAppSinkCallbacks 		appSinkCallbacks;
        appSinkCallbacks.eos  		= onGstEos;
        appSinkCallbacks.new_preroll 	= onGstPreroll;
        appSinkCallbacks.new_sample  	= onGstSample;

	std::string capsDescr = "video/x-raw(memory:GLMemory), format=RGBA";
        if( ! sUseGstGl ) {
            capsDescr = "video/x-raw, format=RGBA";
        }
    
        gst_app_sink_set_callbacks( GST_APP_SINK( mGstData.appSink ), &appSinkCallbacks, this, 0 );
        GstCaps* caps = gst_caps_from_string( capsDescr.c_str() );
        gst_app_sink_set_caps( GST_APP_SINK( mGstData.appSink ), caps );
        gst_caps_unref( caps );
    }   

 	GstPad *pad = nullptr;

    if( sUseGstGl ) {
#if defined( CINDER_GST_HAS_GL )
        mGstData.glupload           = gst_element_factory_make( "glupload", "upload" );
        if( ! mGstData.glupload ) g_printerr( "Failed to create GL upload element!\n" );

        mGstData.glcolorconvert     = gst_element_factory_make( "glcolorconvert", "convert" );
        if( ! mGstData.glcolorconvert ) g_printerr( "Failed to create GL convert element!\n" );

        mGstData.rawCapsFilter	    = gst_element_factory_make( "capsfilter", "rawcapsfilter" );
        if( mGstData.rawCapsFilter ) g_object_set( G_OBJECT( mGstData.rawCapsFilter ), "caps", gst_caps_from_string( "video/x-raw" ), nullptr );
        else g_printerr( "Failed to create raw caps filter element!\n" );

        gst_bin_add_many( GST_BIN( mGstData.videoBin ),  mGstData.rawCapsFilter, mGstData.glupload, mGstData.glcolorconvert, mGstData.appSink, nullptr );

        if( ! gst_element_link_many( mGstData.rawCapsFilter, mGstData.glupload, mGstData.glcolorconvert,  mGstData.appSink, nullptr ) ) {
            g_printerr( "Failed to link video elements...!\n" );
        }

        pad = gst_element_get_static_pad( mGstData.rawCapsFilter, "sink" );
        gst_element_add_pad( mGstData.videoBin, gst_ghost_pad_new( "sink", pad ) );

        mGstData.bufferQueue = g_async_queue_new();
#endif
    }
    else{
        gst_bin_add( GST_BIN( mGstData.videoBin ), mGstData.appSink );
        pad = gst_element_get_static_pad( mGstData.appSink, "sink" );
        gst_element_add_pad( mGstData.videoBin, gst_ghost_pad_new( "sink", pad ) );
    }

    if( pad ) {
        gst_object_unref( pad );
        pad = nullptr;
    }

    g_object_set( G_OBJECT( mGstData.pipeline ), "video-sink", mGstData.videoBin, nullptr );

    addBusWatch( mGstData.pipeline );
}

void GstPlayer::load( const std::string& path )
{
    // If we are already using a custom pipeline we have to reset for now.
    if( mUsingCustomPipeline ) {
        resetCustomPipeline();
    }

    // Construct and link the elements of our pipeline if this is our first run..
    if( ! mGstData.pipeline ) {
        constructPipeline();
    }
	
    // Prepare for (re)loading....
    setPipelineState( GST_STATE_READY );

    // Reset the buffers after we have reached GST_STATE_READY.
    resetVideoBuffers();

    std::string uriPath;
    if( path.find( "://", 0 ) == std::string::npos ) {
        GError* err = nullptr;
        gchar* uri = gst_filename_to_uri( path.c_str(), &err );
        uriPath = std::string( static_cast<const char*>( uri ) );
        g_free( uri );
        if( err ) { 
            g_free( err );
        }
        mGstData.isStream = false;
    }
    else {
        mGstData.isStream = true;
    }

    // set the new movie path
    const gchar* uri = ( ! uriPath.empty() ) ? uriPath.c_str() : path.c_str();
    g_object_set( G_OBJECT ( mGstData.pipeline ), "uri", uri, nullptr );

    // and preroll.
    setPipelineState( GST_STATE_PAUSED );
}

void GstPlayer::play()
{
    setPipelineState( GST_STATE_PLAYING );
}

void GstPlayer::stop()
{
    setPipelineState( GST_STATE_PAUSED );   
}
	
bool GstPlayer::newVideo() const
{
    return mGstData.videoHasChanged;
}

int GstPlayer::width() const
{
    return mGstData.width;
}

int GstPlayer::height() const
{
    return mGstData.height;
}

bool GstPlayer::isPaused() const
{
    return mGstData.isPaused;
}

bool GstPlayer::isPlayable() const
{
    return mGstData.isPlayable;
}

bool GstPlayer::isLoaded() const
{
    return mGstData.isLoaded;
}

bool GstPlayer::isLiveSource() const
{
    return mGstData.isLive;
}

bool GstPlayer::isBuffering() const
{
    return mGstData.isBuffering;
}

bool GstPlayer::isPrerolled() const
{
    return mGstData.isPrerolled;
}

GstVideoFormat GstPlayer::format() const
{
    return mGstData.videoFormat;
}

float GstPlayer::getFramerate() const
{
    return mGstData.frameRate;
}

bool GstPlayer::hasAudio() const
{
    if( ! mGstData.pipeline ) return false;

    gint numAudioChannels = 0;
    g_object_get( G_OBJECT( mGstData.pipeline ), "n-audio", &numAudioChannels, nullptr );
    return ( numAudioChannels > 0 ) ? true : false;
}

bool GstPlayer::hasVisuals() const
{
    if( ! mGstData.pipeline ) return false;

    gint numVideoStreams = 0;
    g_object_get( G_OBJECT( mGstData.pipeline ), "n-video", &numVideoStreams, nullptr );
    return ( numVideoStreams > 0 ) ? true : false;
}

gint64 GstPlayer::getDurationNanos()
{
    if( ! mGstData.pipeline ) return -1;
	
    gint64 duration;
    if( isPrerolled() ) {
        if( !gst_element_query_duration( mGstData.pipeline, GST_FORMAT_TIME, &duration)) {
            g_warning( "Cannot query duration." );
            return -1.0;
        }
        mGstData.duration = duration;
    }
    else{
        mGstData.duration = -1;
        g_warning( "Cannot query duration ! Pipeline is NOT PRE-ROLLED" );
    }
    return mGstData.duration;
}

float GstPlayer::getDurationSeconds()
{
    gint64 duration = getDurationNanos();
    return (float)duration / 1000000000.0f ;
}

int GstPlayer::getNumFrames() const
{
    return mGstData.numFrames;
}

float GstPlayer::getPixelAspectRatio() const
{	
    return mGstData.pixelAspectRatio;
}

gint64 GstPlayer::getPositionNanos()
{
    if( !mGstData.pipeline ) return -1;
	
    gint64 pos = 0;
    if( isPrerolled() ) {
        if( ! gst_element_query_position( mGstData.pipeline, GST_FORMAT_TIME, &pos ) ) {
            g_warning( "Cannot query position ! " );
            return -1.0;
        }
        mGstData.position = pos;
    }
    else {
        mGstData.position = -1.0f;
        g_warning("Cannot query duration ! Pipeline is not prerolled.. ");
    }
    return mGstData.position;
}

float GstPlayer::getPositionSeconds()
{
    gint64 posNanos = getPositionNanos();
    return (float)posNanos / 1000000000.0f;
}

void GstPlayer::setVolume( float targetVolume )
{
    if( ! mGstData.pipeline ) return;
    g_object_set( G_OBJECT( mGstData.pipeline ), "volume", (gdouble)targetVolume, nullptr );
}

float GstPlayer::getVolume()
{
    if( ! mGstData.pipeline ) return -1;
    float currentVolume;
    g_object_get( G_OBJECT( mGstData.pipeline ), "volume", &currentVolume, nullptr );
    return currentVolume;
}

bool GstPlayer::isDone() const
{
    return mGstData.isDone;
}

void GstPlayer::seekToTime( float seconds )
{
    if( ! mGstData.pipeline ) return;
    // When doing flushing seeks the pipeline will pre-roll
    // which means that we might be in a GST_STATE_CHANGE_ASYNC when we request the seek ( ..when fast-seeking ).
    // If thats the case then we should wait for GST_MESSAGE_ASYNC_DONE before executing the
    // next seek.
    auto timeToSeek = seconds*GST_SECOND;
    if( getStateChange() == GST_STATE_CHANGE_ASYNC || isBuffering() ) {
        mGstData.requestedSeek = true;
        mGstData.requestedSeekTime = timeToSeek; // convert to nanos.
        return;
    }
    sendSeekEvent( timeToSeek );
}

void GstPlayer::stepForward()
{
    setPipelineState( GST_STATE_PAUSED );
    gst_element_send_event( mGstData.pipeline, gst_event_new_step ( GST_FORMAT_BUFFERS, 1, getRate(), TRUE, FALSE ) );
}

void GstPlayer::setLoop( bool loop, bool palindrome )
{
    mGstData.loop = loop;
    mGstData.palindrome = palindrome;
}

bool GstPlayer::setRate( float rate )
{
    if( rate == getRate() ) return true; // Avoid unnecessary rate change;
    // A rate equal to 0 is not valid and has to be handled by pausing the pipeline.
    if( rate == 0 ){
        return setPipelineState( GST_STATE_PAUSED );
    }
    
    if( rate < 0 && isStream() ) {
        g_print( "No reverse playback supported for streams!\n " );
        return false;
    }
	
    mGstData.rate = rate;
    // We need the position in case we have switched
    // to reverse playeback i.e rate < 0
    gint64 timeToSeek = getPositionNanos();
    return sendSeekEvent( timeToSeek );
}

float GstPlayer::getRate() const
{
    return mGstData.rate;
}

bool GstPlayer::hasNewFrame() const
{
    return mNewFrame;
}

bool GstPlayer::isStream() const
{
    return mGstData.isStream;
}

bool GstPlayer::sendSeekEvent( gint64 seekTime )
{
    GstEvent* seekEvent;
    GstSeekFlags seekFlags = GstSeekFlags( GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE );
    
    if( getRate() > 0.0 ){
        seekEvent = gst_event_new_seek( getRate(), GST_FORMAT_TIME, seekFlags, GST_SEEK_TYPE_SET, seekTime, GST_SEEK_TYPE_SET, GST_CLOCK_TIME_NONE );
    }
    else {
        seekEvent = gst_event_new_seek( getRate(), GST_FORMAT_TIME, seekFlags, GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_SET, seekTime );
    }
	
    gboolean successSeek = gst_element_send_event( mGstData.pipeline, seekEvent );
	
    if( ! successSeek ){
        g_warning("seek failed");
        return false;
    }
    return successSeek;
}

GstElement* GstPlayer::getPipeline()
{
    return mGstData.pipeline;
}

GstState GstPlayer::getCurrentState()
{
    GstState current;
    gst_element_get_state( mGstData.pipeline, &current, nullptr, 0 );
    return current;
}

GstState GstPlayer::getPendingState()
{
    GstState pending;
    gst_element_get_state( mGstData.pipeline, nullptr, &pending, 0 );
    return pending;
}

GstStateChangeReturn GstPlayer::getStateChange()
{
    GstState current, pending;
    GstStateChangeReturn stateChange = gst_element_get_state( mGstData.pipeline, &current, &pending, 0 );
    g_print( "Pipeline CURRENT state : %s with PENDING %s\n", gst_element_state_get_name( current ), gst_element_state_get_name( pending ) );
    
    return stateChange;
}

bool GstPlayer::setPipelineState( GstState targetState )
{
    if( ! mGstData.pipeline )
        return false;
	
    GstState current, pending;
    gst_element_get_state( mGstData.pipeline, &current, &pending, 0 );
    
    if( current == targetState || pending == targetState )
        return true; // Avoid unnecessary state changes.
	
    mGstData.targetState = targetState;
	
    GstStateChangeReturn stateChangeResult = gst_element_set_state( mGstData.pipeline, mGstData.targetState );
    g_print( "Pipeline state about to change from : %s to %s\n", gst_element_state_get_name( current ), gst_element_state_get_name( targetState ));

    // Unblock the streaming thread for not delaying state change.
    unblockStreamingThread();
	
    if( ! sEnableAsyncStateChange && stateChangeResult == GST_STATE_CHANGE_ASYNC ) {
        g_print( " Blocking until pipeline state changes from : %s to %s\n", gst_element_state_get_name( current ), gst_element_state_get_name( targetState ));
        stateChangeResult = gst_element_get_state( mGstData.pipeline, &current, &pending, GST_CLOCK_TIME_NONE);
    }
	
    return checkStateChange( stateChangeResult );
}

bool GstPlayer::checkStateChange( GstStateChangeReturn stateChangeResult )
{
    switch( stateChangeResult ) {
        case GST_STATE_CHANGE_FAILURE: {
	    g_warning( "** Pipeline failed to change state.. **" );
            return false;	
        }
        case GST_STATE_CHANGE_SUCCESS: {
            g_print( "Pipeline state changed SUCCESSFULLY from : %s to %s\n", gst_element_state_get_name( mGstData.currentState ), gst_element_state_get_name ( mGstData.targetState ) );
            // Our target state is now the current one also.
            mGstData.updateState( mGstData.targetState );
            return true;
        }	
        case GST_STATE_CHANGE_ASYNC: {
            g_print( "Pipeline state change will happen ASYNC from : %s to %s\n", gst_element_state_get_name( mGstData.currentState ), gst_element_state_get_name ( mGstData.targetState ) );
            return true;
        }
        case GST_STATE_CHANGE_NO_PREROLL: {
            mGstData.isLive = true;
            return true;
        }
        default:
            return false;
    }
}

void GstPlayer::createTextureFromMemory()
{
    mMutex.lock();
    std::swap( mFrontVBuffer, mBackVBuffer );
    mMutex.unlock();

    if( mFrontVBuffer ) {
        mVideoTexture = ci::gl::Texture::create( mFrontVBuffer, GL_RGBA, width(), height() );
        if( mVideoTexture ) mVideoTexture->setTopDown();
    }

    unblockStreamingThread();
}

void GstPlayer::createTextureFromID()
{
    GLint textureID = mGstTextureID;

    // Grab the last generated buffer for unref-ing on texture destruction.
    GstBuffer* old = nullptr;
    if( mGstData.bufferQueue ) {
        old = (GstBuffer*)g_async_queue_try_pop( mGstData.bufferQueue );
    }

    auto deleter = [ this, old ] ( ci::gl::Texture* texture ) mutable { 
            if( old ) gst_buffer_unref( old );
	    old = nullptr;
	    delete texture;
    };

    mVideoTexture = ci::gl::Texture::create( GL_TEXTURE_2D, textureID, mGstData.width, mGstData.height, true, deleter );

    if( mVideoTexture ) {
        mVideoTexture->setTopDown();
    }

    // Unblock the streaming thread for processing next frame.
    unblockStreamingThread();
}

ci::gl::Texture2dRef GstPlayer::getVideoTexture()
{
    if( mNewFrame ){
        if( ! sUseGstGl ) {
            createTextureFromMemory();
        }
        else {
            createTextureFromID();
        }
        mNewFrame = false;
    }
    return mVideoTexture;
}

void GstPlayer::resetVideoBuffers()
{
    if( sUseGstGl ) {
        resetGLBuffers();
    }
    else {
        resetSystemMemoryBuffers();
    }
}

void GstPlayer::resetGLBuffers()
{
    if( mGstData.bufferQueue ) {
        while( g_async_queue_length( mGstData.bufferQueue ) > 0 ) {
                GstBuffer* old = (GstBuffer*)g_async_queue_pop( mGstData.bufferQueue );
                if( old ) gst_buffer_unref( old );
                old = nullptr;
        }
    }
}

void GstPlayer::resetSystemMemoryBuffers()
{
    mMutex.lock();
    // Take care of the front buffer.
    if( mFrontVBuffer ) {
        delete mFrontVBuffer;
        mFrontVBuffer = nullptr;
    }

    // Take care of the back buffer.
    if( mBackVBuffer ) {
        delete mBackVBuffer;
        mBackVBuffer = nullptr;
    }
    mMutex.unlock();
}

void GstPlayer::onGstEos( GstAppSink* sink, gpointer userData )
{
}

GstFlowReturn GstPlayer::onGstPreroll( GstAppSink* sink, gpointer userData )
{
    GstPlayer* me = static_cast<GstPlayer*>( userData );
    me->processNewSample( gst_app_sink_pull_preroll( sink ) );
    return GST_FLOW_OK;
}

GstFlowReturn GstPlayer::onGstSample( GstAppSink* sink, gpointer userData )
{
    GstPlayer* me = static_cast<GstPlayer*>( userData );
    me->processNewSample( gst_app_sink_pull_sample( sink ) );
    return GST_FLOW_OK;
}

void GstPlayer::getVideoInfo( const GstVideoInfo& videoInfo )
{
    mGstData.width  		= videoInfo.width;
    mGstData.height 	    	= videoInfo.height;
    mGstData.videoFormat    	= videoInfo.finfo->format;
    mGstData.frameRate 		= (float)videoInfo.fps_n / (float)videoInfo.fps_d;	
    mGstData.pixelAspectRatio   = (float)videoInfo.par_n / (float)videoInfo.par_d ;

    if( isPrerolled() && mGstData.numFrames == 0 ) {
        mGstData.numFrames = (int)( getDurationNanos() * (float)videoInfo.fps_n / (float)videoInfo.fps_d / (float) GST_SECOND );	
    }
}

void GstPlayer::processNewSample( GstSample* sample )
{
    GstBuffer* newBuffer = nullptr;
    if( sUseGstGl ) {
#if defined( CINDER_GST_HAS_GL )
        // Keep only the last buffer around.
        if( mGstData.bufferQueue ) {
            if( g_async_queue_length( mGstData.bufferQueue ) >= 2 ){
                GstBuffer* old = (GstBuffer*)g_async_queue_pop( mGstData.bufferQueue );
                if( old ) {
                    gst_buffer_unref( old );
                    old = nullptr;
                }
            }
        }
        // Pull the memory buffer from the sample.
        newBuffer = gst_sample_get_buffer( sample );

        // Save the buffer for avoiding override on next sample.
        // The incoming buffer is owened and managed by GStreamer.
        // In order to keep the texture in memory until the application has
        // finished processing it and to avoid override by the GStreamer streaming thread
        // we need to keep a reference to it.
        // The buffer is unref-ed during texture destruction.
        g_async_queue_push( mGstData.bufferQueue, gst_buffer_ref( newBuffer ) );

        if( newVideo() ) {
            // Grab video info.
            GstCaps* currentCaps    = gst_sample_get_caps( sample );
            gboolean success	    = gst_video_info_from_caps( &mGstData.videoInfo, currentCaps );
            if( success ) {
                getVideoInfo( mGstData.videoInfo );
            }
            ///Reset the new video flag .
            mGstData.videoHasChanged = false;
        }

        // We 've saved the buffer in the queue so unref the sample.
        gst_sample_unref( sample );
        sample = nullptr;

        // Map the memory and update texture id.
        updateTextureID( newBuffer );

        mNewFrame = true;
#endif
    }
    else {
        mMutex.lock();

        newBuffer = gst_sample_get_buffer( sample );
        gst_buffer_map( newBuffer, &mGstData.memoryMapInfo, GST_MAP_READ ); // Map the buffer for reading the data.

            // We have pre-rolled so query info and allocate buffers if we have a new video.
        if( newVideo() ) {
            GstCaps* currentCaps = gst_sample_get_caps( sample );
            gboolean success = gst_video_info_from_caps( &mGstData.videoInfo, currentCaps );
            if( success ) {
                getVideoInfo( mGstData.videoInfo );
            }

            if( ! mFrontVBuffer ) {
                mFrontVBuffer = new unsigned char[ mGstData.memoryMapInfo.size ];
            }
			
            if( ! mBackVBuffer ) {
                mBackVBuffer = new unsigned char[ mGstData.memoryMapInfo.size ];
            }
			
            ///Reset the new video flag .
            mGstData.videoHasChanged = false;
        }

        memcpy( mBackVBuffer, mGstData.memoryMapInfo.data, mGstData.memoryMapInfo.size );
		
        gst_buffer_unmap( newBuffer, &mGstData.memoryMapInfo ); 

        mNewFrame = true;

        mMutex.unlock();

        // Finished working with the sample - unref-it.
        gst_sample_unref( sample );
        sample = nullptr;
    }

    // Pause the streaming thread until the new Cinder texture is created.
    std::unique_lock<std::mutex> uniqueLock( mMutex );
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> frameInterval ( 1000.0 / (double)mGstData.frameRate );

    mStreamingThreadCV.wait_until( uniqueLock, now + frameInterval, [ this ]{ return mUnblockStreamingThread.load(); } );

    mUnblockStreamingThread = false;
}

void GstPlayer::updateTextureID( GstBuffer* newBuffer )
{
#if defined( CINDER_GST_HAS_GL )
    // Map the GL memory for reading. This will give us the texture id that arrives from GStreamer.
    gst_buffer_map( newBuffer, &mGstData.memoryMapInfo, (GstMapFlags)( GST_MAP_READ | GST_MAP_GL ) ); 

    // Save the texture ID.
    mGstTextureID = *(guint*)mGstData.memoryMapInfo.data;
    // Unmap the memory. 
    gst_buffer_unmap( newBuffer, &mGstData.memoryMapInfo );
#endif
}

}} // namespace gst::video

#if defined( __CLANG__ )
	#pragma diagnostic pop 
#else // GCC
	#pragma GCC diagnostic pop
#endif
