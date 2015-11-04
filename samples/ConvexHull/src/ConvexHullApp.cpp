#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ConvexHull.h"
#include "cinder/Rand.h"
#include "cinder/params/Params.h"
#include "cinder/Font.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ConvexHullApp : public App {
  public:
	void setup();
	void makeNewLetter();
	void mouseDown( MouseEvent event );	
	void mouseDrag( MouseEvent event );
	void update();
	void draw();

	bool				mUsingShape;
	vector<vec2>		mPoints;
	Shape2d				mShape;

	PolyLine2f			mConvexHull;
	
	params::InterfaceGlRef	mParams;
};

void ConvexHullApp::setup()
{
	mUsingShape = true;
	mParams = params::InterfaceGl::create( "App parameters", ivec2( 180, 100 ) );
	mParams->addParam( "Use Glyph", &mUsingShape );
	mParams->addButton( "Clear Points", [&] { mPoints.clear(); } );
	
	if( mUsingShape )
		makeNewLetter();
}

void ConvexHullApp::makeNewLetter()
{
	Font arial( "Times", 512 );
	mShape = arial.getGlyphShape( arial.getGlyphChar( '!' + randInt( 92 ) ) );
//	mat2 transform = MatrixAffine2f::makeTranslate( vec2( 100, getWindowHeight() - 100 ) );
	mShape.transform( translate( mat3(), vec2( 100, getWindowHeight() - 100 ) ) );
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

CINDER_APP( ConvexHullApp, RendererGl )