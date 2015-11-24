#include "GstPlayer.h"

#ifndef CINDER_LINUX_EGL_ONLY
    #include "glfw/glfw3.h"
    #define GLFW_EXPOSE_NATIVE_X11
    #define GLFW_EXPOSE_NATIVE_GLX
    #include "glfw/glfw3native.h"
#endif

#include "cinder/app/AppBase.h"
#include "cinder/gl/Environment.h"

namespace gst { namespace video {

GstData::GstData()
: mPaused(false)
, mIsBuffering(false)
, mIsLive(false)
, mPosition(-1)
, mWidth(-1)
, mHeight(-1)
, mVideoHasChanged(false)
, mVideoFormat(GST_VIDEO_FORMAT_RGBA)
, mAsyncStateChangePending(false)
, mCurrentState(GST_STATE_NULL)
, mTargetState(GST_STATE_NULL)
, mIsPrerolled(false)
, mDuration(-1)
, mIsDone(false)
, mIsLoaded(false)
, mIsPlayable(false)
, mRequestedSeekTime(-1)
, mRequestedSeek(false)
, mLoop(false)
, mPalindrome(false)
, mRate(1.0f)
, mIsStream(false)
, mHasAudio(false)
{
}
void GstData::reset() {
    mIsPrerolled = false;
    mVideoHasChanged = false;
    mIsBuffering = false;
    mPosition = 0;
    mWidth = -1;
    mHeight = -1;
    mAsyncStateChangePending = false;
    mTargetState = mCurrentState = GST_STATE_NULL;
    mHasAudio = false;
}

gboolean checkBusMessages( GstBus* bus, GstMessage* message, gpointer userPlayer )
{
    GstPlayer* player = static_cast<GstPlayer*>( userPlayer );
    GstData & data = player->getGstData();
    
    switch ( GST_MESSAGE_TYPE( message ) )
    {
        case GST_MESSAGE_ERROR:
        {
            GError *err = nullptr;
            gchar *dbg;
            gst_message_parse_error ( message, &err, &dbg );
            gst_object_default_error ( message->src, err, dbg );
            g_error_free ( err );
            g_free ( dbg );
            GstStateChangeReturn state = gst_element_set_state( player->getPipeline(), GST_STATE_NULL );
            if( state == GST_STATE_CHANGE_FAILURE ) {
                g_error( " FAILED to set the pipeline to nullptr state after ERROR occured\n" );
            }
            break;
        }
	case GST_MESSAGE_HAVE_CONTEXT:
	{
	     GstContext *context;
	     const gchar *context_type;
	     gchar *context_str;

	     gst_message_parse_have_context( message, &context );
	     context_type = gst_context_get_context_type( context );
	     context_str = gst_structure_to_string( gst_context_get_structure( context ) );
	     g_print ("Got __HAVE__ context %s from element %s\n", context_type, GST_ELEMENT_NAME( GST_MESSAGE_SRC( message ) ) );

	     g_free( context_str );
	     if( context ) gst_context_unref( context );
	     break;
	}
	case GST_MESSAGE_NEED_CONTEXT:
	{
	     const gchar *context_type;
      	     gst_message_parse_context_type( message, &context_type );
	     g_print( "Got **NEED** context %s from element %s\n", context_type, GST_ELEMENT_NAME( GST_MESSAGE_SRC( message ) ) );
	     GstContext* context = nullptr;
	     if( g_strcmp0( context_type, GST_GL_DISPLAY_CONTEXT_TYPE ) == 0 ) {
	        context = gst_context_new( GST_GL_DISPLAY_CONTEXT_TYPE, TRUE );
		gst_context_set_gl_display( context, data.mCinderDisplay );
		gst_element_set_context( GST_ELEMENT( message->src ), context );
	     }
	     else if( g_strcmp0( context_type, "gst.gl.app_context" ) == 0 ) {
		context = gst_context_new( "gst.gl.app_context", TRUE );
		GstStructure *s = gst_context_writable_structure( context );
		gst_structure_set( s, "context", GST_GL_TYPE_CONTEXT, data.mCinderContext, NULL );
		gst_element_set_context( GST_ELEMENT( message->src ), context );
	     }
	    if( context ) gst_context_unref( context ); 
	    break;
	}
        case GST_MESSAGE_BUFFERING:
        {
            if( data.mIsLive ) break; ///> No buffering for live sources.
            gint percent = 0;
            gst_message_parse_buffering( message, &percent );
            g_print ("Buffering (%3d%%)\r", percent);
            if( percent == 100 ) {
                data.mIsBuffering = false;
                if( data.mTargetState == GST_STATE_PLAYING ) {
                    g_print( " BUFFERRING COMPLETE ! \n" );
                    gst_element_set_state( player->getPipeline(), GST_STATE_PLAYING );
                }
            }
            else {
                if( !data.mIsBuffering && data.mTargetState == GST_STATE_PLAYING ) {
                    gst_element_set_state( player->getPipeline(), GST_STATE_PAUSED );
                    g_print(  " BUFFERRING IN PROGRESS...\n" );
                }
                data.mIsBuffering = true;
            }
            break;
        }
        case GST_MESSAGE_CLOCK_LOST: ///> Possible due to network connection error when streaming..
        {
            /* Get a new clock */
            gst_element_set_state (player->getPipeline(), GST_STATE_PAUSED);
            gst_element_set_state (player->getPipeline(), GST_STATE_PLAYING);
            break;
        }
        case GST_MESSAGE_DURATION_CHANGED:
        {
            break;
        }
        case GST_MESSAGE_STATE_CHANGED:
        {
            if( GST_MESSAGE_SRC( message ) == GST_OBJECT( player->getPipeline() ) ) {
                GstState old, current, pending;
                gst_message_parse_state_changed( message, &old, &current, &pending );
                
                data.mCurrentState = current;
                
                g_print( "Pipeline state changed from : %s to %s\n", gst_element_state_get_name( old ), gst_element_state_get_name ( current ) );
                switch ( current ) {
                    case GST_STATE_PAUSED:
                    {
                        data.mIsPrerolled = true;
                        data.mPaused = true;
                        data.mIsLoaded = true;
                        break;
                    }
                    case GST_STATE_PLAYING:
                    {
                        data.mPaused = false;
                        data.mIsPlayable = true;
                        break;
                    }
                    case GST_STATE_READY:
                    {
			// If we just started reset our data.
			if( old == GST_STATE_NULL ) data.reset();
                        data.mIsPrerolled = false;
                        data.mVideoHasChanged = true;
                        data.mIsDone = false;
                        data.mPosition = 0;
                        break;
                    }
                    case GST_STATE_NULL:
                    {
                        data.reset();
                        break;
                    }
                    default:
                        break;
                }
            }
            break;
        }
        case GST_MESSAGE_ASYNC_DONE:
        {
            GstState current, pending;
            GstStateChangeReturn stateChangeResult = gst_element_get_state( player->getPipeline(), &current, &pending, 250*GST_NSECOND );
            g_print( "Pipeline ASYNC DONE ... Current state : %s | Pending state : %s\n", gst_element_state_get_name( current ), gst_element_state_get_name ( pending ) );
            switch ( current ) {
                case GST_STATE_READY:
                    data.mIsPrerolled = false;
                    data.mVideoHasChanged = true;
                    data.mPosition = 0;
                    break;
                case GST_STATE_PAUSED:
                    /*GstQuery *query;
                     gint64 start, end;
                     query = gst_query_new_seeking (GST_FORMAT_TIME);
                     if (gst_element_query (player->getPipeline(), query)) {
                     
                     gst_query_parse_seeking (query, nullptr, nullptr, &start, &end);
                     g_print ("Seeking is ENABLED from %" GST_TIME_FORMAT " to %" GST_TIME_FORMAT "\n",
                     GST_TIME_ARGS (start), GST_TIME_ARGS (end));
                     }
                     else {
                     g_printerr ("Seeking query failed.");
                     }
                     gst_query_unref (query);*/
                    
                    if( data.mRequestedSeek && !data.mIsBuffering ) {
                        GstEvent* seekEvent;
                        GstSeekFlags seekFlags = GstSeekFlags( GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE );
                        seekEvent = gst_event_new_seek( data.mRate, GST_FORMAT_TIME, seekFlags, GST_SEEK_TYPE_SET, data.mRequestedSeekTime, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE );
                        gboolean successSeek = gst_element_send_event( player->getPipeline(), seekEvent);
                        if( !successSeek ) {
                            g_warning("seek failed");
                        }
                        data.mRequestedSeek = false;
                    }
                    data.mIsPrerolled = true;
                    data.mIsPlayable = true;
                    data.mIsLoaded = true;
                    data.mPaused = true;
                    break;
                case GST_STATE_PLAYING:
                    data.mPaused = false;
                    break;
                default:
                    break;
            }
            data.mAsyncStateChangePending = false;
            break;
        }
        case GST_MESSAGE_EOS:
        {
            if( data.mLoop ) {
                if( data.mPalindrome ) {
                    // Toggle the direction we are playing.
                    data.mRate = -data.mRate;
                    player->setRate( data.mRate );
                }
                else {
                    // If playing back on reverse start the loop from the
                    // end of the file
                    if( data.mRate < 0 ){
                        player->seekToTime(player->getDurationNanos());
                    }
                    else{
                        // otherwise restart from beginning.
                        player->seekToTime(0);
                    }
                }
            }
            data.mVideoHasChanged = false;
            data.mIsDone = true;
            break;
        }
        default:
            break;
    }
    return TRUE; 
}

static void cb_new_pad( GstElement* src, GstPad* newPad, GstPlayer* player )
{ 
    // Here we check to see if we have audio and video.
    // If we have audio then we create and link the audio branch to uridecodebin.
    // This approach allows to process files with or without audio through uridecodebin.
    // For video, the creation of the branch is happening during loading and here we just link
    // since we assume that our *video file* has at least one *video channel*...
    GstData & data = player->getGstData();
    GstPad *sinkPad = nullptr; 
    GstCaps *new_pad_caps = nullptr;
    GstStructure *new_pad_structure = nullptr;
    const gchar *new_pad_type = nullptr;
    new_pad_caps= gst_pad_get_current_caps( newPad );
    new_pad_structure = gst_caps_get_structure( new_pad_caps, 0 );
    new_pad_type = gst_structure_get_name( new_pad_structure );
    if( g_str_has_prefix( new_pad_type, "audio/x-raw" ) ){
	    // Add and link the audio branch to our pipeline.
        gst_bin_add_many( GST_BIN( player->getPipeline() ), data.mAudioQueue, data.mAudioconvert, data.mAudiosink, NULL );
        if( !gst_element_link_many( data.mAudioQueue, data.mAudioconvert, data.mAudiosink, nullptr ) ) {
            g_printerr( " FAILED to LINK AUDIO elements.\n" );
        }
        data.mHasAudio = true;
        sinkPad = gst_element_get_static_pad( data.mAudioQueue, "sink" ); 
    }
    else if( g_str_has_prefix( new_pad_type, "video/x-raw" )  ) {
	    // Here we are on the video branch.
        sinkPad = gst_element_get_static_pad( data.mVideoQueue, "sink" );
    }

    GstPadLinkReturn ret;
    g_print ("Received new pad '%s' from '%s'\n", GST_PAD_NAME (newPad), GST_ELEMENT_NAME (src));
    if( gst_pad_is_linked( sinkPad ) ) {
    	g_print( "Pad already linked! Nothing to do...\n");
	return;
    }
   
    // Here the actual linking of the uridecodebin pad aka newPad with our relevant sinkPad ( audio or video ) is happening.
    ret = gst_pad_link( newPad, sinkPad );
    if (GST_PAD_LINK_FAILED (ret)) {
    	g_print ("  Type is '%s' but link failed.\n", new_pad_type);
    } else {
    	g_print ("  Link succeeded (type '%s').\n", new_pad_type);
    }

    if( data.mHasAudio ) {
        // Added/linked elements inside the pad-added callback need to be 
        // explicitely synced with their parent state.
    	gst_element_sync_state_with_parent( data.mAudioconvert );
    	gst_element_sync_state_with_parent( data.mAudioQueue );
    	gst_element_sync_state_with_parent( data.mAudiosink );
    }

    if( new_pad_caps != nullptr ) gst_caps_unref( new_pad_caps );
    gst_object_unref( sinkPad );
}

GstPlayer::GstPlayer()
: mGMainLoop(nullptr)
, mGstBus(nullptr)
, mGstPipeline(nullptr)
, mGstAppSink(nullptr)
, mNewFrame(false)
, mUsingCustomPipeline(false)
{
    bool success = initializeGStreamer();
    if( success ) startGMainLoopThread();
}

GstPlayer::~GstPlayer()
{
    cleanup();
}

void GstPlayer::cleanup()
{

    if( mGstPipeline != nullptr ) {
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
    mGMainLoop = g_main_loop_new( nullptr, FALSE );
    mGMainLoopThread = std::thread( &GstPlayer::startGMainLoop, this, mGMainLoop );
}

void GstPlayer::startGMainLoop( GMainLoop* loop )
{
    g_main_loop_run( loop );
}

bool GstPlayer::initializeGStreamer()
{
    if( !gst_is_initialized() ) {
        
        guint major;
        guint minor;
        guint micro;
        guint nano;
        
        gst_version( &major, &minor, &micro, &nano );
        
        GError* err;
        /// If we havent already initialized GStreamer do this.
        if( !gst_init_check( nullptr, nullptr, &err ) ) {
            if( err->message ) {
                g_error( "FAILED to initialize GStreamer : %s\n", err->message );
            }
            else {
                g_error( "FAILED to initialize GStreamer due to unknown error ...\n" );
            }
            return false;
        }
        else {
            
            g_print( "Initialized GStreamer version %i.%i.%i.%i\n", major, minor, micro, nano );
            return true;
        }
    }
    return true;
}

void GstPlayer::resetPipeline()
{
    if( mGstPipeline ) {

        gst_element_set_state( mGstPipeline, GST_STATE_NULL);
        gst_element_get_state( mGstPipeline, nullptr, nullptr, GST_CLOCK_TIME_NONE);
        
        gst_object_unref( GST_OBJECT( mGstPipeline ) );
        mGstPipeline = nullptr;
        
	    mGstData.mCinderContext = nullptr;
	    mGstData.mCinderDisplay = nullptr;

        // Unref-ing the pipeline takes care of all children as well
        // except the audio branch for which we kept a reference that we need to unref explicitly.
        mGstData.mVideoQueue = nullptr;
        mGstData.mGLupload = nullptr;
        mGstData.mGLcolorconvert = nullptr;
        mGstAppSink = nullptr;

        gst_object_unref( mGstData.mAudioQueue );
        mGstData.mAudioQueue = nullptr;

        gst_object_unref( mGstData.mAudioconvert );
        mGstData.mAudioconvert = nullptr;

        gst_object_unref( mGstData.mAudiosink );
        mGstData.mAudiosink = nullptr;
    }
}
    
void GstPlayer::resetBus()
{
    bool success = g_source_remove( busID );
    if( success ) {
        busID = -1;
    }
    mGstBus = nullptr;
}

void GstPlayer::setCustomPipeline( const GstCustomPipelineData &customPipeline )
{
    // Similar to loading and maybe merged at some point.
    // If we have a custom pipeline we reset before re-using.
    if( mGstPipeline ) {
        resetPipeline();
        // Reset the bus since the associated pipeline got destroyed.
        resetBus();
    }

    GError *error = nullptr;
    mGstPipeline = gst_parse_launch( customPipeline.pipeline.c_str(), &error );
    
    if( error != nullptr ) {
        g_print( "Could not construct custom pipeline: %s\n", error->message );
        g_error_free (error);
    }
    
    // Prepare for loading custom pipeline.
    // We lock until we switch to GST_STATE_READY since there is not much to do if we dont reach at least there.
    gst_element_set_state( mGstPipeline, GST_STATE_READY );
    gst_element_get_state( mGstPipeline, NULL, NULL, GST_CLOCK_TIME_NONE );
    

    if( mGstPipeline ) {
        // Currently assumes that the pipeline has an appsink named 'videosink'.
        // This is just for testing and it has to be more generic.
        mGstAppSink = gst_bin_get_by_name (GST_BIN (mGstPipeline), "videosink");
    }
    
    addBusWatch( mGstPipeline );
    
    if( mGstAppSink ) {
        GstAppSinkCallbacks appSinkCallbacks;
        appSinkCallbacks.eos = onGstEos;
        appSinkCallbacks.new_preroll = onGstPreroll;
        appSinkCallbacks.new_sample = onGstSample;
    
        gst_app_sink_set_callbacks( GST_APP_SINK( mGstAppSink ), &appSinkCallbacks, this, 0 );
    }
    mUsingCustomPipeline = true;

    // Reset the buffers for the next pre-roll.
    resetVideoBuffers();

    mInputVideoBuffers = g_async_queue_new();
    mOutputVideoBuffers = g_async_queue_new();

    g_object_set_data( G_OBJECT( mGstAppSink ), "queue_input_buf", mInputVideoBuffers );
    g_object_set_data( G_OBJECT( mGstAppSink ), "queue_output_buf", mOutputVideoBuffers );

    // async pre-roll
    gst_element_set_state( mGstPipeline, GST_STATE_PAUSED );
}
    
void GstPlayer::addBusWatch( GstElement* pipeline )
{
    mGstBus = gst_pipeline_get_bus( GST_PIPELINE( pipeline ) );
    busID = gst_bus_add_watch( mGstBus, checkBusMessages, this );
    gst_object_unref ( mGstBus );
}
    
bool GstPlayer::initialize()
{        
    auto _platformData = std::dynamic_pointer_cast<ci::gl::PlatformDataLinux>( ci::gl::context()->getPlatformData() );
#if defined( CINDER_LINUX_EGL_ONLY )
    mGstData.mCinderDisplay = (GstGLDisplay*) gst_gl_display_egl_new_with_egl_display( _platformData->mDisplay );
    mGstData.mCinderContext= gst_gl_context_new_wrapped( (GstGLDisplay*)mGstData.mCinderDisplay, (guintptr)_platformData->mContext, GST_GL_PLATFORM_EGL, GST_GL_API_GLES2 );
#else
    mGstData.mCinderDisplay = (GstGLDisplay*) gst_gl_display_x11_new_with_display( ::glfwGetX11Display() );
    mGstData.mCinderContext = gst_gl_context_new_wrapped( (GstGLDisplay*)mGstData.mCinderDisplay, (guintptr)::glfwGetGLXContext( _platformData->mContext ), GST_GL_PLATFORM_GLX, GST_GL_API_OPENGL );
#endif

    return true;
}

void GstPlayer::resetCustomPipeline()
{
    // In case we use load after we 've set a custom pipeline ..
    if( mGstPipeline ) {
        resetPipeline(); // reset the custom pipeline
        resetBus(); // reset the associated bus
        mUsingCustomPipeline = false;
    }
}

void GstPlayer::load( std::string _path )
{
    // If we are already using a pipeline we have to reset for now.
    if( mUsingCustomPipeline ) {
        resetCustomPipeline();
    }

    if( !mGstPipeline ) {

        std::string capsGL = "video/x-raw(memory:GLMemory), format=RGBA";
        mGstPipeline = gst_pipeline_new( "cinder-pipeline" );
        mGstData.mUriDecode = gst_element_factory_make( "uridecodebin", "uridecode" );	
        mGstData.mVideoQueue = gst_element_factory_make( "queue", "videoqueue" );
        g_object_set( mGstData.mVideoQueue, "max-size-buffers", 5, (void*)nullptr );
        mGstData.mGLupload = gst_element_factory_make( "glupload", "upload" );
        mGstData.mGLcolorconvert = gst_element_factory_make( "glcolorconvert", "convert" );
        mGstAppSink = gst_element_factory_make( "appsink", "videosink" );
        g_object_set( mGstAppSink, "sync", true, (void*)nullptr );
        g_object_set( mGstAppSink, "max-buffers", 1, (void*)nullptr );
        g_object_set( mGstAppSink, "drop", true, (void*)nullptr );

        mGstData.mAudioQueue = gst_element_factory_make( "queue", "audioqueue" );
        g_object_ref(mGstData.mAudioQueue); // We keep a reference on the audio elements so that we can add/remove at will.
        g_object_set( mGstData.mAudioQueue, "max-size-buffers", 5, (void*)nullptr );
        mGstData.mAudioconvert = gst_element_factory_make( "audioconvert", "audioconv" );
        g_object_ref(mGstData.mAudioconvert);
        mGstData.mAudiosink = gst_element_factory_make( "autoaudiosink", "audiosink" );
        g_object_ref(mGstData.mAudiosink);

        if( !mGstPipeline || !mGstData.mUriDecode || !mGstData.mVideoQueue || !mGstData.mGLupload || !mGstData.mGLcolorconvert || !mGstAppSink || !mGstData.mAudioQueue || !mGstData.mAudioconvert || !mGstData.mAudiosink ) {
            g_printerr( "Not all elements could be created !\n" );
        }

        gst_bin_add_many( GST_BIN( mGstPipeline ), mGstData.mUriDecode, mGstData.mVideoQueue, mGstData.mGLupload, mGstData.mGLcolorconvert, mGstAppSink, NULL );
        if( !gst_element_link_many( mGstData.mVideoQueue, mGstData.mGLupload, mGstData.mGLcolorconvert, mGstAppSink, nullptr ) ) {
            g_printerr( " FAILED to LINK VIDEO elements.\n" );
        }

        GError *error = nullptr;
        if( error != nullptr ) {
            g_print( "Could not construct custom pipeline: %s\n", error->message );
            g_error_free (error);
        }

        // With this cb we check if we have audio or not and if yes we link the audio branch of the pipeline.
        g_signal_connect( mGstData.mUriDecode, "pad-added", G_CALLBACK( cb_new_pad ), this );

        addBusWatch( mGstPipeline );
        
        if( mGstAppSink ) {
            GstAppSinkCallbacks appSinkCallbacks;
            appSinkCallbacks.eos = onGstEos;
            appSinkCallbacks.new_preroll = onGstPreroll;
            appSinkCallbacks.new_sample = onGstSample;
        
            gst_app_sink_set_callbacks( GST_APP_SINK( mGstAppSink ), &appSinkCallbacks, this, 0 );
            GstCaps* caps = gst_caps_from_string( capsGL.c_str() );
            gst_app_sink_set_caps( GST_APP_SINK( mGstAppSink), caps);
            gst_caps_unref(caps);
        }
    }
    
    //Prepare for (re)loading....
    //
    // We lock until we switch to GST_STATE_READY since
    // there is not much we can do if dont reach this state.
    gst_element_set_state( mGstPipeline, GST_STATE_READY );
    gst_element_get_state( mGstPipeline, NULL, NULL, GST_CLOCK_TIME_NONE );

    // Unlink the audio branch from the pipeline if we are are reloading and 
    // the previous video had audio.
    // This is necessary since uridecodebin will block if we try to process a video
    // that has no audio channel with the audio branch linked.
    // The linking of the audio branch happens dynamically in the pad-added cb
    // where we check for the presence of an audio stream.
    unlinkAudioBranch();

    // Reset the buffers after we have reached GST_STATE_READY.
    resetVideoBuffers();
    
    // Create the queues that hold the incoming appsink buffers.
    if( !mInputVideoBuffers ) {
	    mInputVideoBuffers = g_async_queue_new();
        g_object_set_data( G_OBJECT( mGstAppSink ), "queue_input_buf", mInputVideoBuffers );
    }
    if( !mOutputVideoBuffers ) {
        mOutputVideoBuffers = g_async_queue_new();
        g_object_set_data( G_OBJECT( mGstAppSink ), "queue_output_buf", mOutputVideoBuffers );
    }

    if( _path.find( "://", 0 ) == std::string::npos ) {
        GError* err = nullptr;
        gchar* uri = gst_filename_to_uri( _path.c_str(), &err );
        _path = uri;
        g_free(uri);
        if( err ) g_free( err );
        mGstData.mIsStream = false;
    }
    else {
        mGstData.mIsStream = true;
    }

    // set the new movie path
    g_object_set( G_OBJECT ( gst_bin_get_by_name( GST_BIN(mGstPipeline), "uridecode" ) ), "uri", _path.c_str(), nullptr );
    
    // and preroll async.
    gst_element_set_state( mGstPipeline, GST_STATE_PAUSED );
}

void GstPlayer::unlinkAudioBranch() 
{
    // If we had audio remove and unlink the audio elements.
    // On the next load the audio branch will be re-linked if an audio stream is present.
    if( mGstPipeline && mGstData.mHasAudio ) {
        gst_element_set_state( mGstData.mAudioQueue, GST_STATE_NULL );
        gst_element_get_state( mGstData.mAudioQueue, nullptr, nullptr, GST_CLOCK_TIME_NONE );
        gst_element_set_state( mGstData.mAudioconvert, GST_STATE_NULL );
        gst_element_get_state( mGstData.mAudioconvert, nullptr, nullptr, GST_CLOCK_TIME_NONE );
        gst_element_set_state( mGstData.mAudiosink, GST_STATE_NULL );
        gst_element_get_state( mGstData.mAudiosink, nullptr, nullptr, GST_CLOCK_TIME_NONE );

        gst_bin_remove_many( GST_BIN( mGstPipeline ), mGstData.mAudioQueue, mGstData.mAudioconvert, mGstData.mAudiosink, nullptr );

        mGstData.mHasAudio = false;
    }
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
    return mGstData.mVideoHasChanged;
}

int GstPlayer::width() const
{
    return mGstData.mWidth;
}

int GstPlayer::height() const
{
    return mGstData.mHeight;
}

bool GstPlayer::isPaused() const
{
    return mGstData.mPaused;
}

bool GstPlayer::isPlayable() const
{
    return mGstData.mIsPlayable;
}

bool GstPlayer::isLoaded() const
{
    return mGstData.mIsLoaded;
}

bool GstPlayer::isLiveSource() const
{
    return mGstData.mIsLive;
}

bool GstPlayer::isBuffering() const
{
    return mGstData.mIsBuffering;
}

bool GstPlayer::isPrerolled() const
{
    return mGstData.mIsPrerolled;
}

GstVideoFormat GstPlayer::format() const
{
    return mGstData.mVideoFormat;
}

gint64 GstPlayer::getDurationNanos()
{
    if( !mGstPipeline ) return -1;
    
    gint64 duration;
    if( isPrerolled() ) {
        if( !gst_element_query_duration( mGstPipeline, GST_FORMAT_TIME, &duration)) {
            g_warning( " Cannot query duration. ");
            return -1.0;
        }
        mGstData.mDuration = duration;
    }
    else{
        mGstData.mDuration = -1;
        g_warning("Cannot query duration ! Pipeline is NOT PRE-ROLLED ");
    }
    return mGstData.mDuration;
}

float GstPlayer::getDurationSeconds()
{
    gint64 duration = getDurationNanos();
    return (float)duration / 1000000000.0f ;
}

gint64 GstPlayer::getPositionNanos()
{
    if( !mGstPipeline ) return -1;
    
    gint64 pos = 0;
    if( isPrerolled() ) {
        if( !gst_element_query_position( mGstPipeline, GST_FORMAT_TIME, &pos ) ) {
            g_warning("Cannot query position ! ");
            return -1.0;
        }
        mGstData.mPosition = pos;
    }
    else {
        mGstData.mPosition = -1.0f;
        g_warning("Cannot query duration ! Pipeline is NOT PRE-ROLLED ");
    }
    return mGstData.mPosition;
}

float GstPlayer::getPositionSeconds()
{
    gint64 posNanos = getPositionNanos();
    return (float)posNanos / 1000000000.0f;
}

void GstPlayer::setVolume( float targetVolume )
{
    if( !mGstPipeline ) return;
    g_object_set( G_OBJECT(mGstPipeline), "volume", (gdouble)targetVolume, nullptr );
}

float GstPlayer::getVolume()
{
    if( !mGstPipeline ) return -1;
    float currentVolume;
    g_object_get( mGstPipeline, "volume", &currentVolume, nullptr );
    return currentVolume;
}

bool GstPlayer::isDone() const
{
    return mGstData.mIsDone;
}

void GstPlayer::seekToTime( float seconds )
{
    if( !mGstPipeline || seconds == getDurationSeconds() ) return;
    // When doing flushing seeks the pipeline will pre-roll
    // which means that we might be in a GST_STATE_CHANGE_ASYNC when we request the seek ( ..when fast-seeking ).
    // If thats the case then we should wait for GST_MESSAGE_ASYNC_DONE before executing the
    // next seek.
    if( getStateChange() == GST_STATE_CHANGE_ASYNC || isBuffering() ) {
        mGstData.mAsyncStateChangePending = true;
        mGstData.mRequestedSeek = true;
        mGstData.mRequestedSeekTime = seconds*GST_SECOND; // convert to nanos.
        return;
    }
    gint64 timeToSeek = seconds*GST_SECOND;
    sendSeekEvent( timeToSeek );
}

void GstPlayer::setLoop( bool loop, bool palindrome )
{
    mGstData.mLoop = loop;
    mGstData.mPalindrome = palindrome;
}

void GstPlayer::setRate( float rate )
{
    if( rate == getRate() ) return; // Avoid unnecessary rate change;
    // A rate equal to 0 is not valid and has to be handled by pausing the pipeline.
    if( rate == 0 ){
        setPipelineState(GST_STATE_PAUSED);
        return;
    }
    
    if( rate < 0 && isStream() ) {
        g_print( "No reverse playback supported for streams!\n " );
        return;
    }
    
    mGstData.mRate = rate;
    // We need the position in case we have switched
    // to reverse playeback i.e rate < 0
    gint64 timeToSeek = getPositionNanos();
    sendSeekEvent( timeToSeek );
}

float GstPlayer::getRate() const
{
    return mGstData.mRate;
}

bool GstPlayer::hasNewFrame() const
{
    return mNewFrame;
}

bool GstPlayer::isStream() const
{
    return mGstData.mIsStream;
}

bool GstPlayer::sendSeekEvent( gint64 seekTime )
{
    GstEvent* seekEvent;
    GstSeekFlags seekFlags = GstSeekFlags( GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE );
    
    if( getRate() > 0.0 ){
        seekEvent = gst_event_new_seek( getRate(), GST_FORMAT_TIME, seekFlags, GST_SEEK_TYPE_SET, seekTime, GST_SEEK_TYPE_SET, GST_CLOCK_TIME_NONE);
    }
    else {
        seekEvent = gst_event_new_seek( getRate(), GST_FORMAT_TIME, seekFlags, GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_SET, seekTime );
    }
    
    gboolean successSeek = gst_element_send_event( mGstPipeline, seekEvent);
    
    if( !successSeek ){
        g_warning("seek failed");
        return false;
    }
    return successSeek;
}

GstElement* GstPlayer::getPipeline()
{
    return mGstPipeline;
}

GstState GstPlayer::getCurrentState()
{
    GstState current;
    gst_element_get_state( mGstPipeline, &current, nullptr, 0 );
    return current;
}

GstState GstPlayer::getPendingState()
{
    GstState pending;
    gst_element_get_state( mGstPipeline, nullptr, &pending, 0 );
    return pending;
}

GstStateChangeReturn GstPlayer::getStateChange()
{
    GstState current, pending;
    GstStateChangeReturn stateChange = gst_element_get_state( mGstPipeline, &current, &pending, 0 );
    g_print( "Pipeline state changed to CURRENT : %s with PENDING %s\n", gst_element_state_get_name( current ), gst_element_state_get_name ( pending ) );
    
    return stateChange;
}

bool GstPlayer::setPipelineState( GstState targetState )
{
    if( !mGstPipeline ) return false;
    
    GstState current, pending;
    gst_element_get_state( mGstPipeline, &current, &pending, 0 );
    
    // If we are buffering let the buffering complete before updating the state.
    // except if we have requested a new movie to be loaded.
    if( isBuffering() && targetState > GST_STATE_READY ) {
        mGstData.mTargetState = targetState;
        mGstData.mCurrentState = current;
    }
    else if( isBuffering() && targetState <= GST_STATE_READY ) {
        mGstData.mIsBuffering = false;
    }
    
    if( current == targetState || pending == targetState ) return true;
    
    GstStateChangeReturn stateChangeResult = gst_element_set_state( mGstPipeline, targetState );
    
    return checkStateChange( stateChangeResult, targetState );
}

bool GstPlayer::checkStateChange( GstStateChangeReturn stateChangeResult, const GstState & target )
{
    switch( stateChangeResult ) {
        case GST_STATE_CHANGE_FAILURE:
            g_warning( "**** Failed to change state.. ****" );
            return false;
            
        case GST_STATE_CHANGE_SUCCESS:
            g_print( "Pipeline state change SUCCESS from : %s to %s\n", gst_element_state_get_name( mGstData.mCurrentState ), gst_element_state_get_name ( target ) );
            
            mGstData.mTargetState = mGstData.mCurrentState = target;
            
            return true;;
            
        case GST_STATE_CHANGE_ASYNC:
            mGstData.mTargetState = target;
            mGstData.mCurrentState = getCurrentState();
            mGstData.mAsyncStateChangePending = true;
            g_print( "Pipeline state change ASYNC from : %s to %s\n", gst_element_state_get_name( mGstData.mCurrentState ), gst_element_state_get_name ( target ) );
            
            return true;;
            
        case GST_STATE_CHANGE_NO_PREROLL:
            mGstData.mCurrentState = getCurrentState();
            mGstData.mTargetState = target;
            mGstData.mIsLive = true;
            return true;
            
        default:
            return false;
    }
}

GstData& GstPlayer::getGstData()
{
    return mGstData;
}

ci::gl::Texture2dRef GstPlayer::getVideoTexture()
{
    if( mNewFrame ){
    	// Pop any old buffers.
    	GAsyncQueue *mOutputVideoBuffers = nullptr;
    	mOutputVideoBuffers = (GAsyncQueue*)g_object_get_data( G_OBJECT(mGstAppSink), "queue_output_buf" );
	    GstBuffer* old = nullptr;
        if( g_async_queue_length( mOutputVideoBuffers ) > 0 ) {
            old = (GstBuffer*)g_async_queue_pop( mOutputVideoBuffers );
        }
        // We keep the buffer memory around until we are done with this texture.
        auto deleter = [ old ] ( ci::gl::Texture *texture ) {
                delete texture;
                if( old ) gst_buffer_unref( old );
        };

        int currentTextureID = -1;
        mMutex.lock();
        currentTextureID = mGstTextureID;
        mMutex.unlock();

        videoTexture = ci::gl::Texture::create( GL_TEXTURE_2D, currentTextureID, width(), height(), true, deleter );
        if( videoTexture ) videoTexture->setTopDown();
        mNewFrame = false;
    }
    return videoTexture;
}

void GstPlayer::resetVideoBuffers()
{
  if( mInputVideoBuffers ) {
      while( g_async_queue_length( mInputVideoBuffers ) > 0 ) {
            GstBuffer *buf = (GstBuffer*)g_async_queue_pop( mInputVideoBuffers );
	        gst_buffer_unref( buf );
      }
  }

  if( mOutputVideoBuffers ) {
      while( g_async_queue_length( mOutputVideoBuffers ) > 0 ) {
            GstBuffer *buf = (GstBuffer*)g_async_queue_pop( mOutputVideoBuffers );
            gst_buffer_unref( buf );
      }
  }
  
  if( mInputVideoBuffers ) {
      g_async_queue_unref( mInputVideoBuffers );
      mInputVideoBuffers = nullptr;
  }
  if( mOutputVideoBuffers ) {
      g_async_queue_unref( mOutputVideoBuffers );
      mOutputVideoBuffers = nullptr;
  }
}

void GstPlayer::onGstEos( GstAppSink* sink, gpointer userData )
{
}

void GstPlayer::eos()
{
}

GstFlowReturn GstPlayer::onGstPreroll( GstAppSink* sink, gpointer userData )
{
    GstPlayer* me = static_cast<GstPlayer*>( userData );
    GstSample* newSample = gst_app_sink_pull_preroll( sink );
    me->sample( newSample, sink );
    gst_sample_unref( newSample );
    newSample = nullptr;
    return GST_FLOW_OK;
}

void GstPlayer::sample( GstSample* sample, GstAppSink* sink )
{
    GstBuffer* buffer;
    buffer = gst_sample_get_buffer( sample );
   
    // Save the buffer for avoiding override on next sample and use with the async queue.
    gst_buffer_ref( buffer );

    GAsyncQueue *mInputVideoBuffers = nullptr;

    mInputVideoBuffers = (GAsyncQueue*)g_object_get_data( G_OBJECT( sink ), "queue_input_buf" );
    g_async_queue_push( mInputVideoBuffers, buffer );

    if( g_async_queue_length( mInputVideoBuffers ) > 0 ) {
	    updateTexture( sample, sink );
	    mNewFrame = true;
    }
}

GstFlowReturn GstPlayer::onGstSample( GstAppSink* sink, gpointer userData )
{
    GstPlayer* me = static_cast<GstPlayer*>( userData );
    GstSample* newSample = gst_app_sink_pull_sample( sink );
    me->sample( newSample, sink );
    gst_sample_unref( newSample );
    newSample = nullptr;
    return GST_FLOW_OK;
}

void GstPlayer::updateTexture( GstSample* sample, GstAppSink* sink )
{
    GAsyncQueue* mInputVideoBuffers = (GAsyncQueue*) g_object_get_data( G_OBJECT( sink ), "queue_input_buf" );

    // Grab a buffer from the queue.
    GstBuffer* buffer = (GstBuffer*)g_async_queue_pop( mInputVideoBuffers );

    // Map the GL memory for reading. This will give us the texture id that arrives from GStreamer.
    gst_buffer_map( buffer, &mMemoryMapInfo, (GstMapFlags)(GST_MAP_READ | GST_MAP_GL) ); 

    GstCaps* currentCaps = gst_sample_get_caps( sample );
    gboolean success = gst_video_info_from_caps( &mVideoInfo, currentCaps );

    if( success ) {
        mGstData.mWidth = mVideoInfo.width;
        mGstData.mHeight = mVideoInfo.height;
        mGstData.mVideoFormat = mVideoInfo.finfo->format;
        /*std::cout << " FORMAT : " << mVideoInfo.finfo->name;
        std::cout << " WIDTH : " << mGstData.mWidth;
        std::cout << " HEIGHT : " << mGstData.mHeight;
        std::cout << " BytesPerRow : " << mGstData.mWidth*(mVideoInfo.finfo->pixel_stride[0]);
        std::cout << " STRIDE PLANE 2 : " << mVideoInfo.stride[1];
        std::cout << " STRIDE PLANE 3 : " << mVideoInfo.stride[2] << std::endl;*/
    }

    // Save the texture ID.
    mMutex.lock();
    mGstTextureID = *(guint*)mMemoryMapInfo.data;
    mMutex.unlock();

    // Unmap the memory. 
    gst_buffer_unmap( buffer, &mMemoryMapInfo );

    // Push the buffer into the out queue for unref-ing later on.
    g_async_queue_push( mOutputVideoBuffers, buffer );
}

} }
