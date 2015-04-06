#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Texture.h"
#include <list>
using namespace ci;
using namespace ci::app;
using namespace std;

#include "cinder/android/JniHelper.h"
using ci::android::JniHelper;

#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "libcinder", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "libcinder", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"libcinder", __VA_ARGS__))

// We'll create a new Cinder Application by deriving from the App class
class JavaTextureLoad : public App {
  public:
    void setup();
    void touchesBegan( TouchEvent event );
	void draw();

	// This will maintain a list of points which we will draw line segments between
	list<vec2>		mPoints;

    jclass          mActivityClass;
    jmethodID       mLoadTextureMethodId;

    gl::TextureRef  mTex;
};

static void detach_thread( void *chunk )
{
    LOGI( "========================== detach_thread ==========================" );
}

pthread_key_t key;
void make_key()
{
    pthread_key_create( &key, detach_thread );
    LOGI( "========================== make_key ==========================" );
}

void JavaTextureLoad::setup()
{
    make_key();

    if( JniHelper::get()->AttachCurrentThread() ) {
        jclass rclass = JniHelper::get()->retrieveClass( "org/libcinder/samples/javatextureload/JavaTextureLoadActivity" );
        mActivityClass = (jclass)JniHelper::get()->NewGlobalRef( rclass );
        if(NULL != mActivityClass) {
            mLoadTextureMethodId = JniHelper::get()->GetStaticMethodId( mActivityClass, "loadTexture", "(I)V" );
        }

        JniHelper::get()->DeatchCurrentThread();
    }

    mTex = gl::Texture::create( 1920, 1200 );

    if( ( NULL != mLoadTextureMethodId ) && JniHelper::get()->AttachCurrentThread() ) {
        JniHelper::get()->CallStaticVoidMethod( mActivityClass, mLoadTextureMethodId, mTex->getId() );

        JniHelper::get()->DeatchCurrentThread();
    }
}

void JavaTextureLoad::touchesBegan( TouchEvent event )
{
	for( const auto& touch : event.getTouches() ) {
        mPoints.push_back( touch.getPos() );
	}
}

void JavaTextureLoad::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );

	gl::color( 1.0f, 0.5f, 0.25f );	
	gl::begin( GL_LINE_STRIP );
	for( auto pointIter = mPoints.begin(); pointIter != mPoints.end(); ++pointIter ) {
		gl::vertex( *pointIter );
	}
	gl::end();

	gl::color( 1.0f, 1.0f, 1.0f );
	gl::draw( mTex );
}

// This line tells Cinder to actually create the application
CINDER_APP( JavaTextureLoad, RendererGl )
