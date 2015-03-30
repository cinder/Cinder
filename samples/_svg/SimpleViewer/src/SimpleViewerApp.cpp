#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/svg/Svg.h"
#include "cinder/ip/Fill.h"
#include "cinder/svg/SvgGl.h"
#include "cinder/cairo/Cairo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SimpleViewerApp : public App {
  public:
	void setup();
	void mouseDown( MouseEvent event );
	void draw();
	void fileDrop( FileDropEvent event );
	void load( fs::path path );

	bool					mUseCairo;
	svg::DocRef				mDoc;
	gl::TextureRef			mTex;
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

gl::TextureRef renderCairoToTexture( svg::DocRef doc )
{
	cairo::SurfaceImage srf( 640, 480, true );
	if( doc->getWidth() && doc->getHeight() )
		srf = cairo::SurfaceImage( doc->getWidth(), doc->getHeight(), true );
	else
		srf = cairo::SurfaceImage( 640, 480, true );

	cairo::Context ctx( srf );
	ctx.render( *doc );
	ctx.flush();

	return gl::Texture::create( srf.getSurface() );
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
	catch( ci::Exception &exc ) {
		console() << "exception caught parsing svg doc, what: " << exc.what() << endl;
	}
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
		gl::drawStringCentered( "Click to toggle between Cairo & OpenGL", getWindowCenter() + vec2( 0, 20 ) );
	}
}


CINDER_APP( SimpleViewerApp, RendererGl )
