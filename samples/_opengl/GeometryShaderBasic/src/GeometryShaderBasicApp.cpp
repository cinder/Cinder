#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Batch.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class GeometryShaderIntroApp : public App {
  public:
	void setup();
	void mouseDrag( MouseEvent event );
	void draw();
	
	gl::VertBatchRef	mBatch;
	gl::GlslProgRef		mGlsl;
	int					mNumSides;
	float				mRadius;
};

void GeometryShaderIntroApp::setup()
{
	mNumSides = 2;
	mRadius = 100;

	// setup shader
	try {
		mGlsl = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "basic.vert" ) )
									 .fragment( loadAsset( "basic.frag" ) )
									 .geometry( loadAsset( "basic.geom" ) ) );
	}
	catch( gl::GlslProgCompileExc ex ) {
		cout << ex.what() << endl;
		quit();
	}

	// setup VertBatch with a single point at the origin
	mBatch = gl::VertBatch::create();
	mBatch->vertex( vec2( 0 ) );
	mBatch->color( 1, 0, 0 );
}

void GeometryShaderIntroApp::mouseDrag( MouseEvent event )
{
	mNumSides = (((float)event.getX() / getWindowWidth()) * 30) + 3;
	mNumSides = constrain( mNumSides, 2, 64 );
	
	mRadius = ((float)event.getY() / getWindowHeight()) * ( getWindowWidth() / 2.0f );
	mRadius = constrain( mRadius, 1.0f, getWindowWidth() / 2.0f );
}

void GeometryShaderIntroApp::draw()
{
	gl::clear();
	
	gl::setMatricesWindow( getWindowWidth(), getWindowHeight() );
	gl::translate( getWindowCenter() );

	gl::ScopedGlslProg glslProg( mGlsl );
	mGlsl->uniform( "uNumSides", mNumSides );
	mGlsl->uniform( "uRadius", mRadius );	
	mBatch->draw();
}

CINDER_APP( GeometryShaderIntroApp, RendererGl )
