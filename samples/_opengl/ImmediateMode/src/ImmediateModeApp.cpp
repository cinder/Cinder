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

	void mouseDown(  MouseEvent event ) override;

	gl::GlslProgRef		mGlsl;

	gl::VertBatchRef	mBatch;

	std::vector<vec2>	mPoints;
};

void ImmediateModeApp::setup()
{
	mGlsl = gl::getStockShader( gl::ShaderDef().color() );
	mGlsl->bind();

	mBatch = gl::VertBatch::create( GL_POINTS, true );

}

void ImmediateModeApp::mouseDown(  MouseEvent event )
{
	mPoints.push_back( event.getPos() );
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



	// draw
	mBatch->begin( GL_LINE_STRIP );
	for( const vec2 &point : mPoints ) {
		mBatch->vertex( vec4( point, 0, 1 ), Color( 1.0f, 0.5f, 0.25f ) );
	}
	mBatch->draw();
}

CINDER_APP( ImmediateModeApp, RendererGl )
