#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Log.h"
#include "cinder/System.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class GlslProgTestApp : public AppNative {
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;

	void testGlslProgInclude();

	gl::GlslProgRef			mGlslProg;
};

void GlslProgTestApp::setup()
{
	testGlslProgInclude();
}

void GlslProgTestApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'r' ) {
		CI_LOG_V( "reload" );
		setup();
	}
}

void GlslProgTestApp::testGlslProgInclude()
{
	try {
		auto format = gl::GlslProg::Format()
							.vertex(  loadAsset( "passthrough.vert" ) )
							.fragment( loadAsset( "shaderWithInclude.frag" ) );

		mGlslProg = gl::GlslProg::create( format );
	}
	catch( std::exception &exc ) {
		CI_LOG_E( "exception caught, type: " << System::demangleTypeName( typeid( exc ).name() ) << ", what: " << exc.what() );
	}

}

void GlslProgTestApp::update()
{
}

void GlslProgTestApp::draw()
{
	gl::clear( Color( 0.1, 0.2, 0.1 ) );

	if( mGlslProg ) {
		gl::ScopedGlslProg glslScope( mGlslProg );
		gl::drawSolidRect( getWindowBounds() );
	}
}

CINDER_APP_NATIVE( GlslProgTestApp, RendererGl )
