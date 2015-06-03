#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/ip/Hdr.h"
#include "cinder/gl/gl.h"
#include "VoronoiGpu.h"

#include <vector>

using namespace ci;
using namespace ci::app;
using namespace std;

class VoronoiGpuApp : public App {
 public:
	void setup();
	void calculateVoronoiTexture();
	
	void mouseDown( MouseEvent event );
	void keyDown( KeyEvent event );
	
	void draw();

	vector<ivec2>		mPoints;
	gl::Texture2dRef	mTexture;
	bool				mShowDistance;
};

void VoronoiGpuApp::setup()
{
	mShowDistance = false;
	// register window changed display callback
	getWindow()->getSignalDisplayChange().connect( [this] { calculateVoronoiTexture(); } );
	
	mPoints.push_back( toPixels( ivec2( 100, 100 ) ) );
	mPoints.push_back( toPixels( ivec2( 200, 120 ) ) );
	mPoints.push_back( toPixels( ivec2( 130, 140 ) ) );
	mPoints.push_back( toPixels( ivec2( 200, 200 ) ) );
}

void VoronoiGpuApp::calculateVoronoiTexture()
{
	if( mShowDistance ) {
		Channel32f rawDistanceMap = calcDistanceMapGpu( mPoints, toPixels( getWindowWidth() ), toPixels( getWindowHeight() ) );
		// we need to convert the raw distances into a normalized range of 0-1 so we can show them sensibly
		ip::hdrNormalize( &rawDistanceMap );
		mTexture = gl::Texture2d::create( rawDistanceMap );
	}
	else {
		Surface32f rawDistanceMap = calcDiscreteVoronoiGpu( mPoints, toPixels( getWindowWidth() ), toPixels( getWindowHeight() ) );
		// we need to convert the site locations into a normalized range of 0-1 so we can show them sensibly
		ip::hdrNormalize( &rawDistanceMap );
		mTexture = gl::Texture2d::create( rawDistanceMap );
	}
}

void VoronoiGpuApp::mouseDown( MouseEvent event )
{
	mPoints.push_back( toPixels( event.getPos() ) );
	calculateVoronoiTexture();
}

void VoronoiGpuApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'd' ) {
		mShowDistance = ! mShowDistance;
		calculateVoronoiTexture();
	}
	else if( event.getChar() == 'x' ) {
		mPoints.clear();
		calculateVoronoiTexture();
	}
}

void VoronoiGpuApp::draw()
{
	gl::clear();
	gl::setMatricesWindow( getWindowSize() );
	
	gl::color( Color( 1, 1, 1 ) );
	if( mTexture ) {
		gl::draw( mTexture, toPoints( mTexture->getBounds() ) );
	}
	
	// draw the voronoi sites in yellow
	gl::color( Color( 1.0f, 1.0f, 0.0f ) );	
	for( vector<ivec2>::const_iterator ptIt = mPoints.begin(); ptIt != mPoints.end(); ++ptIt )
		gl::drawSolidCircle( toPoints( vec2( *ptIt ) ), 2.0f );
	
	gl::enableAlphaBlending();
	gl::drawStringRight( "Click to add a point", vec2( getWindowWidth() - toPixels( 10 ), getWindowHeight() - toPixels( 20 ) ), Color( 1, 0.3, 0 ), Font( "Arial", toPixels( 12 ) ) );
	gl::disableAlphaBlending();
}

// This line tells Cinder to actually create the application
CINDER_APP( VoronoiGpuApp, RendererGl )