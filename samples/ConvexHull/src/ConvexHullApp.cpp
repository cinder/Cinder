#include "cinder/app/AppNative.h"
#include "cinder/ConvexHull.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ConvexHullApp : public AppNative {
  public:
	void setup();
	void makeNewLetter();
	void mouseDown( MouseEvent event );	
	void mouseDrag( MouseEvent event );
	void update();
	void draw();

	bool				mUsingShape;
	vector<Vec2f>		mPoints;
	Shape2d				mShape;

	PolyLine2f			mConvexHull;
	
	params::InterfaceGlRef	mParams;
};

void ConvexHullApp::setup()
{
	mUsingShape = true;
	mParams = params::InterfaceGl::create( "App parameters", Vec2i( 180, 100 ) );
	mParams->addParam( "Use Shape", &mUsingShape );
	mParams->addButton( "Clear Points", [&] { mPoints.clear(); } );
	
	if( mUsingShape )
		makeNewLetter();
}

void ConvexHullApp::makeNewLetter()
{
	Font arial( "Times", 512 );
	mShape = arial.getGlyphShape( arial.getGlyphChar( '!' + randInt( 92 ) ) );
	MatrixAffine2f transform = MatrixAffine2f::makeTranslate( Vec2f( 100, getWindowHeight() - 100 ) );
	mShape.transform( transform );
}

void ConvexHullApp::update()
{
	// this should be cached but doesn't matter for this demo
	if( mUsingShape )
		mConvexHull = calcConvexHull( mShape );
	else
		mConvexHull = calcConvexHull( mPoints );
}

void ConvexHullApp::mouseDown( MouseEvent event )
{
	if( mUsingShape )
		makeNewLetter();
	else
		mPoints.push_back( event.getPos() );
}

void ConvexHullApp::mouseDrag( MouseEvent event )
{
	if( mUsingShape )
		makeNewLetter();
	else
		mPoints.back() = event.getPos();
}

void ConvexHullApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
	gl::enableAlphaBlending();

	// draw solid convex hull
	gl::color( ColorA( 0.8f, 0, 1.0f, 0.2f ) );	
	gl::drawSolid( mConvexHull );
	gl::color( ColorA( 0.8f, 0, 1.0f, 0.8f ) );	
	gl::draw( mConvexHull );
	
	// draw convex hull points
	gl::color( Color( 0.0f, 0, 1.0f ) );	
	for( auto ptIt = mConvexHull.begin(); ptIt != mConvexHull.end(); ++ptIt ) {
		gl::drawSolidCircle( *ptIt, 4 );
	}
	
	gl::color( ColorA( 1.0f, 0.8f, 0, 0.6f ) );
	if( mUsingShape ) {
		gl::draw( mShape );
	}
	else {
		for( auto ptIt = mPoints.begin(); ptIt != mPoints.end(); ++ptIt ) {
			gl::drawSolidCircle( *ptIt, 3 );
		}	
	}
		
	mParams->draw();
}

CINDER_APP_NATIVE( ConvexHullApp, RendererGl )
