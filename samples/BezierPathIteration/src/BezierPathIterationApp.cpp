// This sample demonstrates the Path2dCalcCache class in particular
// It loads an SVG and generates a Path2dCalcCache per curve.
// It then generates 2000 random particles which move along the paths at a constant speed

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/svg/Svg.h"
#include "cinder/Rand.h"

#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

class BezierPathIterationApp : public AppNative {
  public:
	void setup() override;
	void loadSvg( const fs::path &fsPath );
	void fileDrop( FileDropEvent event ) override;
	void draw() override;
	
	struct PathIter {
		PathIter( const Path2dCalcCache& pathCache, float initialDistance )
			: mPathCache( pathCache ), mCurrentDistance( initialDistance )
		{
			mLastPos = mPathCache.getPosition( mPathCache.calcTimeForDistance( mCurrentDistance ) );
		}
		
		const Path2dCalcCache&	mPathCache;
		Vec2f					mLastPos;
		float					mCurrentDistance;
	};
	
	Vec2f						mWindowOffset, mWindowScale;
	vector<Path2dCalcCache>		mPathCaches;
	// each path iterator has a reference to its path(cache) and its current position
	vector<PathIter>			mPathIters;
};

void BezierPathIterationApp::setup()
{
	loadSvg( getAssetPath( "black_type_logo.svg" ) );
}

void BezierPathIterationApp::loadSvg( const fs::path &fsPath )
{
	mPathIters.clear();
	mPathCaches.clear();

	// pull in all the paths of the SVG
	auto svg = svg::Doc::create( fsPath );
	Shape2d shape = svg->getShape();
	auto &paths = shape.getContours();
	// make a Path2dCalcCache for each path in the SVG
	for( auto &path : paths )
		mPathCaches.emplace_back( path );
	
	// Fit the SVG to the window
	gl::setMatricesWindow( getWindowSize() );
	Rectf svgBounds = shape.calcPreciseBoundingBox();
	Rectf fitRect = svgBounds.getCenteredFit( Rectf( getWindowBounds() ), false );
	mWindowOffset = ( getWindowSize() - fitRect.getSize() ) / 2;
	mWindowScale = fitRect.getSize() / svgBounds.getSize();
	
	// Generate 2000 PathIters, all assigned to random paths and random points along their respective paths
	for( int i = 0; i < 2000; ++i ) {
		const Path2dCalcCache &pathCache = mPathCaches[randInt(mPathCaches.size())];
		mPathIters.emplace_back( pathCache, randFloat() * pathCache.getLength() );
	}	
}

void BezierPathIterationApp::fileDrop( FileDropEvent event )
{
	loadSvg( event.getFiles()[0] );
}

void BezierPathIterationApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 

	// center and scale the drawing
	gl::setMatricesWindow( getWindowSize() );
	gl::translate( mWindowOffset );
	gl::scale( mWindowScale );

	gl::color( Color( 1.0f, 0.5f, 0.25f ) );
	for( auto &pathIt : mPathIters ) {
		pathIt.mCurrentDistance += 3.0f; // move 3 units along the path
		float newTime = pathIt.mPathCache.calcTimeForDistance( pathIt.mCurrentDistance );
		Vec2f pos = pathIt.mPathCache.getPosition( newTime );
		
		gl::drawLine( pathIt.mLastPos, pos );
		pathIt.mLastPos = pos;
	}
}

CINDER_APP_NATIVE( BezierPathIterationApp, RendererGl )
