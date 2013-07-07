#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/BinPacker.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CinderBinPackerApp : public AppNative {
public:
	void prepareSettings( Settings *settings );
	void setup();
	void keyDown( KeyEvent event );	
	void update();
	void draw();

	BinPacker					mBinPacker;
	std::vector<Area>			mUnpacked;
	std::vector<Area>			mPacked;
};

void CinderBinPackerApp::prepareSettings(Settings *settings)
{
	settings->setWindowSize(512, 512);
}

void CinderBinPackerApp::setup()
{
	mBinPacker.setSize( 128, 128 );
	//mBinPacker.enableRotation();
}

void CinderBinPackerApp::keyDown( KeyEvent event )
{
	// add an Area of random size to mUnpacked
	mUnpacked.push_back( Area(0, 0, Rand::randInt(8, 32), Rand::randInt(8, 32)) );

	// show the total number of Area's in the window title bar
	getWindow()->setTitle( "CinderBinPackerApp " + ci::toString( mUnpacked.size() ) );

	try
	{ 
		// mPacked will contain all Area's of mUnpacked in the exact same order,
		// but moved to a different spot in the bin. If rotated, (x1,y1) will be
		// the lower left corner and (x2,y2) will be the upper right corner of the Area.
		mPacked = mBinPacker.pack( mUnpacked ); 
	}
	catch(...) 
	{  
		// the bin is not large enough to contain all Area's, so let's
		// double the size...
		int size = mBinPacker.getWidth();
		mBinPacker.setSize( size << 1, size << 1 );

		/// ...and try again
		mPacked = mBinPacker.pack( mUnpacked ); 
	}
}

void CinderBinPackerApp::update()
{
}

void CinderBinPackerApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	// draw the borders of the bin
	gl::color( Color( 1, 1, 0 ) );
	gl::drawStrokedRect( Rectf( Vec2f::zero(), mBinPacker.getSize() ) );

	// draw all packed Area's
	Rand rnd;	
	for(unsigned i=0;i<mPacked.size();++i) {
		rnd.seed(i+12345);
		gl::color( Color( (rnd.nextUint() & 0xFF) / 255.0f, (rnd.nextUint() & 0xFF) / 255.0f, (rnd.nextUint() & 0xFF) / 255.0f ) );
		gl::drawSolidRect( Rectf( mPacked[i] ) );
	}
}

CINDER_APP_NATIVE( CinderBinPackerApp, RendererGl )
