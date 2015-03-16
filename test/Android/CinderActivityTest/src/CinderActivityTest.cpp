#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include <list>

#include "cinder/app/android/EventManagerAndroid.h"
#include <jni.h>
using namespace ci;
using namespace ci::app;
using namespace std;

//#include <android/log.h>
//
//#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "cinder", __VA_ARGS__))
//#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "cinder", __VA_ARGS__))
//#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"cinder", __VA_ARGS__))
//
//JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *pjvm, void *reserved)
//{
//    LOGI( "====== JNI_OnLoad ======" );
//
//    return JNI_VERSION_1_6;
//}

// We'll create a new Cinder Application by deriving from the App class
class CinderActivityTest : public App {
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

void CinderActivityTest::setup()
{
    android_app* app = EventManagerAndroid::instance()->getNativeApp();
    ANativeActivity* activity = app->activity;
    JavaVM* vm = activity->vm;

    JNIEnv* env = nullptr;
    //if( JNI_OK != vm->GetEnv( reinterpret_cast<void**>( &env ), JNI_VERSION_1_6 ) ) {
    if( JNI_OK != vm->AttachCurrentThread( &env, NULL ) ) {
        console() << "Unable to get environment" << std::endl;
        return;
    }
    console() << "Got environment" << std::endl;

    //jclass cls = env->FindClass( "org/libcinder/samples/cinderactivitytest/CinderActivityTestActivity" );
    //jclass cls = env->FindClass( "android/app/NativeActivity" );
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

    jmethodID method = env->GetStaticMethodID( cls, "javaFunction", "()V" );
    if( NULL == method ) {
        console() << "Unable to get static method" << std::endl;
        jthrowable exc = env->ExceptionOccurred();
        if( exc ) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
        return;
    }
    console() << "Got static method" << std::endl;

    env->CallStaticVoidMethod( cls, method );
}

void CinderActivityTest::mouseDrag( MouseEvent event )
{
	mPoints.push_back( event.getPos() );
}

#if defined( CINDER_ANDROID )
void CinderActivityTest::touchesBegan( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
        mPoints.push_back( touchIt->getPos() );
	}
}
#endif

void CinderActivityTest::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
}

void CinderActivityTest::draw()
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
CINDER_APP( CinderActivityTest, RendererGl )
