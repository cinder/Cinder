#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ImmediateModeApp : public App {
  public:
	void setup();
	void draw();

	gl::GlslProgRef		mGlsl;
};

void ImmediateModeApp::setup()
{
	mGlsl = gl::getStockShader( gl::ShaderDef().color() );
	mGlsl->bind();
}

void ImmediateModeApp::draw()
{
	gl::clear();	

	gl::VertBatch vb( GL_TRIANGLES );
		vb.color( 1, 0, 0 );
		vb.vertex( getWindowWidth() / 2, 50 );
		vb.color( 0, 1, 0 );
		vb.vertex( getWindowWidth() - 50, getWindowHeight() - 50 );
		vb.color( 0, 0, 1 );
		vb.vertex( 50, getWindowHeight() - 50 );
	vb.draw();
}

CINDER_APP( ImmediateModeApp, RendererGl )
