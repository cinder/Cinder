#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/svg/Svg.h"
#include "cinder/svg/SvgGl.h"
#include "cinder/cairo/Cairo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SimpleViewerApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );
	void draw();
	void fileDrop( FileDropEvent event );
	void load( fs::path path );

	bool					mUseCairo;
	svg::DocRef				mDoc;
	gl::Texture				mTex;
};


void SimpleViewerApp::setup()
{
	mUseCairo = true;
}

void SimpleViewerApp::mouseDown( MouseEvent event )
{
	mUseCairo = ! mUseCairo;
}

void SimpleViewerApp::fileDrop( FileDropEvent event )
{
	load( event.getFile( 0 ) );
}

gl::Texture renderCairoToTexture( svg::DocRef doc )
{
	cairo::SurfaceImage srf;
	if( doc->getWidth() && doc->getHeight() )
		srf = cairo::SurfaceImage( doc->getWidth(), doc->getHeight(), true );
	else
		srf = cairo::SurfaceImage( 640, 480, true );
	cairo::Context ctx( srf );

	ctx.render( *doc );
	srf.flush();
	
	return gl::Texture( srf.getSurface() );
}

void SimpleViewerApp::load( fs::path path )
{
	try {
		if( path.extension() != ".svgz" ) 
			mDoc = svg::Doc::create( path );
		else // compressed
			mDoc = svg::Doc::createFromSvgz( loadFile( path ) );
		
		mTex = renderCairoToTexture( mDoc );
	}
	catch( ... ) {
	} // ignore errors
}

void SimpleViewerApp::draw()
{
	// clear out the window with black
	gl::clear( Color::gray( 0.5f ) );
	gl::enableAlphaBlending();

	if( mDoc ) {
		gl::color( Color::white() );
		
		if( mTex ) {
			if( mUseCairo )
				gl::draw( mTex );
			else
				gl::draw( *mDoc );
		}
	}
	else {
		gl::drawStringCentered( "Drag & Drop an SVG file", getWindowCenter() );
		gl::drawStringCentered( "Click to toggle between Cairo & OpenGL", getWindowCenter() + Vec2f( 0, 20 ) );
	}
}


CINDER_APP_BASIC( SimpleViewerApp, RendererGl )
