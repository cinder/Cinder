#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include <list>
using namespace ci;
using namespace ci::app;
using namespace std;

#include "cinder/app/android/EventManagerAndroid.h"
#include <jni.h>

#include <fstream>

// We'll create a new Cinder Application by deriving from the App class
class UrlLoader : public App {
  public:
    void setup();
	void mouseDrag( MouseEvent event );
#if defined( CINDER_ANDROID )
    void touchesBegan( TouchEvent event );
#endif
	void keyDown( KeyEvent event );
	void draw();

	// This will maintain a list of points which we will draw line segments between
	list<vec2>		mPoints;
};

jmethodID GetStaticMethod( JNIEnv* env, jclass clazz, const std::string& methodName, const std::string& methodSig )
{
    if( nullptr == env ) {
        throw std::invalid_argument( "paramenter env is null" );
    }

    jmethodID result = env->GetStaticMethodID( clazz, methodName.c_str(), methodSig.c_str() );
    if( NULL == result ) {
        throw std::runtime_error( "couldn't get static method " + methodName + " with sig " + methodSig );
    }

    return result;
}

void UrlLoader::setup()
{
    android_app* app = EventManagerAndroid::instance()->getNativeApp();
    ANativeActivity* activity = app->activity;
    JavaVM* vm = activity->vm;

    JNIEnv* env = nullptr;
    if( JNI_OK != vm->AttachCurrentThread( &env, NULL ) ) {
        console() << "Unable to get environment" << std::endl;
        return;
    }
    console() << "Got environment" << std::endl;

    jclass cls = env->GetObjectClass( activity->clazz );
    if( NULL == cls ) {
        console() << "Unable to get class" << std::endl;
        jthrowable exc = env->ExceptionOccurred();
        if( exc ) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
        return;
    }
    console() << "Got class" << std::endl;

    try {
        jmethodID loadUrlStart = GetStaticMethod( env, cls, "loadUrlStart", "(Ljava/lang/String;)J" );
        jmethodID loadUrlClear = GetStaticMethod( env, cls, "loadUrlClear", "()V" );
        jmethodID loadUrlGetResponseCode = GetStaticMethod( env, cls, "loadUrlGetResponseCode", "()I" );
        jmethodID loadUrlGetErrorMsg = GetStaticMethod( env, cls, "loadUrlGetErrorMsg", "()Ljava/lang/String;" );
        jmethodID loadUrlGetData = GetStaticMethod( env, cls, "loadUrlGetData", "()[B" );

        jstring jUrlStr = env->NewStringUTF( "https://farm9.staticflickr.com/8583/16821610285_a0892342af_k.jpg" );
        jlong res = env->CallStaticLongMethod( cls, loadUrlStart, jUrlStr );
        env->DeleteLocalRef( jUrlStr );

        if( -1 == res ) {
            jstring jstr = (jstring)env->CallStaticObjectMethod( cls, loadUrlGetErrorMsg );
            if(NULL != jstr) {
                const char* cstr = env->GetStringUTFChars( jstr, NULL );
                std::string str = std::string( cstr );
                env->ReleaseStringUTFChars( jstr, cstr );

                throw std::runtime_error(str.c_str());
            }
        }

        jbyteArray jBytes = (jbyteArray)env->CallStaticObjectMethod( cls, loadUrlGetData );
        if( NULL == jBytes ) {
            throw std::runtime_error( "loadUrlGetdata failed" );
        }

        size_t len = env->GetArrayLength( jBytes );
        jbyte* dataPtr = (jbyte*)env->GetByteArrayElements( jBytes, NULL );
        if( NULL == dataPtr ) {
            throw std::runtime_error( "unable to get bytearray" );
        }

        std::vector<jbyte> dataArray = std::vector<jbyte>( len );
        memcpy( &(dataArray[0]), dataPtr, len );

        env->ReleaseByteArrayElements( jBytes, dataPtr, 0 );

        env->CallStaticVoidMethod( cls, loadUrlClear );


        std::ofstream os( "/storage/emulated/0/myfile.jpg", std::ios::binary );
        os.write( (const char*)&(dataArray[0]), len );
        os.close();

        console() << "Done!!!!!!" << std::endl;
    }
    catch( const std::exception& e ) {
        console() << "ERROR: " << e.what() << std::endl;
    }

/*
    jmethodID staticMethod = env->GetStaticMethodID( cls, "loadUrl", "(Ljava/lang/String;)[B" );
    if( NULL == staticMethod ) {
        console() << "Unable to get static method" << std::endl;
        jthrowable exc = env->ExceptionOccurred();
        if( exc ) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
        return;
    }
    console() << "Got static method" << std::endl;

    jstring jUrlStr = env->NewStringUTF( "http://www.google.com" );
    if(NULL == jUrlStr) {
        console() << "Unable to get construct string" << std::endl;
        jthrowable exc = env->ExceptionOccurred();
        if( NULL != exc ) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
        return;
    }

    jbyteArray bytes = (jbyteArray)env->CallStaticObjectMethod( cls, staticMethod, jUrlStr );
    if( NULL == bytes ) {
        jthrowable exc = env->ExceptionOccurred();
        if( NULL != exc ) {
            jmethodID toString = env->GetMethodID(env->FindClass( "java/lang/Object"), "toString", "()Ljava/lang/String;" );
            jstring estring = (jstring)env->CallObjectMethod( exc, toString );

            jboolean isCopy;
            const char* msg = env->GetStringUTFChars( estring, &isCopy );

            console() << "Exception: " << msg << std::endl;

            env->ExceptionClear();
        }
        return;
    }

    env->DeleteLocalRef( jUrlStr );
*/

    vm->DetachCurrentThread();
}

void UrlLoader::mouseDrag( MouseEvent event )
{
	mPoints.push_back( event.getPos() );
}

#if defined( CINDER_ANDROID )
void UrlLoader::touchesBegan( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
        mPoints.push_back( touchIt->getPos() );
	}
}
#endif

void UrlLoader::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
}

void UrlLoader::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );

	gl::color( 1.0f, 0.5f, 0.25f );	
	gl::begin( GL_LINE_STRIP );
	for( auto pointIter = mPoints.begin(); pointIter != mPoints.end(); ++pointIter ) {
		gl::vertex( *pointIter );
	}
	gl::end();
}

// This line tells Cinder to actually create the application
CINDER_APP( UrlLoader, RendererGl )
