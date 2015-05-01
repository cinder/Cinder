#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/svg/Svg.h"
#include "cinder/cairo/Cairo.h"

// enable this to automatically write a quicktime to your home directory
#if 0
	#define RECORD_MOVIE
	#include "cinder/qtime/MovieWriter.h"
#endif

using namespace ci;
using namespace ci::app;
using namespace std;

class AnimatedRevealApp : public App {
  public:
	void update();
	void draw();
	void fileDrop( FileDropEvent event );
	void load( const fs::path &path );
	
	svg::DocRef		mDoc;
	gl::TextureRef	mTex;
	int				mMinRenderElement;
	bool			mDone;
	
#if defined( RECORD_MOVIE )
	qtime::MovieWriter		mMovie;
#endif
};

Surface renderCairo( svg::DocRef doc, const svg::RenderVisitor &visitor )
{
	cairo::SurfaceImage srf;
	if( doc->getWidth() && doc->getHeight() )
		srf = cairo::SurfaceImage( doc->getWidth(), doc->getHeight(), true );
	else
		srf = cairo::SurfaceImage( 640, 480, true );
	cairo::Context ctx( srf );

	ctx.setSourceRgb( 1, 1, 1 );
	ctx.paint();
	ctx.render( *doc, visitor );
	srf.flush();
	Surface result = srf.getSurface();
	return srf.getSurface();
}

void AnimatedRevealApp::load( const fs::path &path )
{
	mMinRenderElement = 0;
	try { 
		if( path.extension() == ".svgz" ) // compressed
			mDoc = svg::Doc::createFromSvgz( loadFile( path ) );
		else
			mDoc = svg::Doc::create( loadFile( path ) );
			
		mDone = false;
#if defined( RECORD_MOVIE )
		mMovie = qtime::MovieWriter( fs::path( getHomeDirectory() + "AnimatedReveal.mov" ), mDoc->getWidth(), mDoc->getHeight() );
#endif
	}
	catch( ci::Exception &exc ) {
		console() << "failed to load doc, what: " << exc.what() << endl;
	}
}

void AnimatedRevealApp::fileDrop( FileDropEvent event )
{
	load( event.getFile( 0 ) );
}

// This svg::RenderVisitor draws all elements as outlines, and
// then progressively more of the elements filled in
// The minIdx dictates the number of elements which are definitely drawn filled
// the minPixelArea kicks in once we've drawn 'minIdx' elements. It represents
// the number of pixels that need to be drawn before we decide to stop drawing elements
// filled, continuing by drawing them as outlines.
// At the end of the render, *resultMinIdx contains the index of the last filled item
struct SlowFillVisitor {
	SlowFillVisitor( int minIdx, size_t minPixelArea, int *resultMinIdx, bool *resultDone )
		: mMinIdx( minIdx ), mMinPixelArea( minPixelArea ),
			mResultMinIdx( resultMinIdx ), mResultDone( resultDone )
	{
		mCurrentPixelArea = 0;
		mCurIdx = 0;
		*mResultDone = true;
	}

	bool operator()( const svg::Node &node, svg::Style *style )
	{
		if( ++mCurIdx <= mMinIdx ) // indices below the min just pass through as-is
			return true;
		else if( mCurrentPixelArea < mMinPixelArea ) {
			// this node is "on the edge". Add up the area of the bounding boxes until
			// we cross mMinPixelArea threshold, at which point we'll just draw nodes as outline
			mCurrentPixelArea += node.getBoundingBox().getWidth() * node.getBoundingBox().getHeight();
			*mResultMinIdx = mCurIdx;
			style->setStroke( svg::Paint( Color( 0, 0.5f, 1.0f ) ) );
			style->setFill( svg::Paint() );
			*mResultDone = false;
			return true;
		}
		else {
			*mResultDone = false;
			return false;
		}
	}
	
	int		mCurIdx, mMinIdx, *mResultMinIdx;
	bool	*mResultDone;
	size_t	mMinPixelArea, mCurrentPixelArea;
};

void AnimatedRevealApp::update()
{
	if( ( ! mDoc ) || mDone )
		return;

	// we'll stop drawing filled after we've drawn new pixels that are 1% of the size of the image
	size_t imageSize, minArea;
	if( mDoc->getWidth() && mDoc->getHeight() )
		imageSize = mDoc->getWidth() * mDoc->getHeight();
	else
		imageSize = 640 * 480;
	minArea = 1 + imageSize * 0.01f;

	Surface frame = renderCairo( mDoc, SlowFillVisitor( mMinRenderElement, minArea, &mMinRenderElement, &mDone ) );
#if defined( RECORD_MOVIE )
	mMovie.addFrame( frame );
	if( mDone ) {
		mMovie.addFrame( frame ); // otherwise we seem to lose the last frame
		mMovie.finish();
	}
#endif
	mTex = gl::Texture::create( frame );
}

void AnimatedRevealApp::draw()
{
	gl::enableAlphaBlending();
	gl::clear(); 
	if( mTex )
		gl::draw( mTex );
	else
		gl::drawStringCentered( "Drag & Drop an SVG file", getWindowCenter() );
}


CINDER_APP( AnimatedRevealApp, RendererGl )
