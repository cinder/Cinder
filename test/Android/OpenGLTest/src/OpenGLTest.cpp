#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Log.h"
#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

// We'll create a new Cinder Application by deriving from the App class
class OpenGLTest : public App {
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

  private:
	ci::gl::FboRef				mFbo;
	ci::gl::Texture2dRef		mFboColorAtt;
	ci::gl::Texture2dRef		mFboDepthAtt;
};

void OpenGLTest::setup()
{
	GLint maxBuffers = 0;
	glGetIntegerv( GL_MAX_DRAW_BUFFERS, &maxBuffers );
	console() << "maxBuffers=" << maxBuffers << std::endl;

	// create the textures we're using for offscreen rendering
	vec2 resolution = toPixels( getWindowSize() );

	console() << "resolution=" << resolution << std::endl;

	mFboColorAtt = gl::Texture2d::create( resolution.x, resolution.y, gl::Texture2d::Format().internalFormat( GL_RGB ).minFilter( GL_LINEAR ).magFilter( GL_LINEAR ) );
	mFboDepthAtt = gl::Texture2d::create( resolution.x, resolution.y, gl::Texture2d::Format().internalFormat( GL_RGBA ).minFilter( GL_NEAREST ).magFilter( GL_NEAREST ) );

	// and attach them to a newly created fbo
	//try
	{
		auto fboFormat = gl::Fbo::Format();
		fboFormat.attachment( GL_COLOR_ATTACHMENT0, mFboColorAtt );
		fboFormat.attachment( GL_COLOR_ATTACHMENT1, mFboDepthAtt );
		fboFormat.depthBuffer();
		mFbo = gl::Fbo::create( resolution.x, resolution.y, fboFormat );
	}
//	catch( const std::exception& e ) {
//		CI_LOG_E( std::string("Fbo create failed: ") + e.what() );
//	}

   	console() << "GL_EXT_color_buffer_float: " << gl::isExtensionAvailable("GL_EXT_color_buffer_float")  << std::endl;
   	console() << "GL_EXT_color_buffer_half_float: " << gl::isExtensionAvailable("GL_EXT_color_buffer_half_float")  << std::endl;


    //console() << "GL_OES_EGL_image_external: " << gl::isExtensionAvailable("GL_OES_EGL_image_external")  << std::endl;
    //console() << "GL_TEXTURE_EXTERNAL_OES: " << GL_TEXTURE_EXTERNAL_OES << std::endl;
}

void OpenGLTest::mouseDrag( MouseEvent event )
{
	mPoints.push_back( event.getPos() );
}

#if defined( CINDER_ANDROID )
void OpenGLTest::touchesBegan( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
        mPoints.push_back( touchIt->getPos() );
	}
}
#endif

void OpenGLTest::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
}

void OpenGLTest::draw()
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
CINDER_APP( OpenGLTest, RendererGl )
