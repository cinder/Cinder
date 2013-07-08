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

	switch( event.isShiftDown() )
	{
	case 0:
		try
		{ 
			// mPacked will contain all Area's of mUnpacked in the exact same order,
			// but moved to a different spot in the bin. Unpacked will not be altered.
			// If rotated, (x1,y1) will be the lower left corner and (x2,y2) will be
			// the upper right corner of the Area.
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
		break;
	case 1:
		// create a list of references
		std::vector<Area*>	refs;
		for(unsigned i=0;i<mUnpacked.size();++i)
			refs.push_back( &mUnpacked[i] );

		try
		{ 
			// This will apply packing to all Area's in mUnpacked
			mBinPacker.pack( refs ); 
		}
		catch(...) 
		{  
			// the bin is not large enough to contain all Area's, so let's
			// double the size...
			int size = mBinPacker.getWidth();
			mBinPacker.setSize( size << 1, size << 1 );

			/// ...and try again
			mBinPacker.pack( refs ); 
		}

		// use the mUnpacked buffer in the draw() call
		mPacked.clear();
		break;
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

	if(mPacked.empty())
	{
		// packing has been done in-place, so use the mUnpacked array
		for(unsigned i=0;i<mUnpacked.size();++i) {
			rnd.seed(i+12345);
			gl::color( Color( (rnd.nextUint() & 0xFF) / 255.0f, (rnd.nextUint() & 0xFF) / 255.0f, (rnd.nextUint() & 0xFF) / 255.0f ) );
			gl::drawSolidRect( Rectf( mUnpacked[i] ) );
		}
	}
	else
	{
		// packing has been done by copy, so use the mPacked array
		for(unsigned i=0;i<mPacked.size();++i) {
			rnd.seed(i+12345);
			gl::color( Color( (rnd.nextUint() & 0xFF) / 255.0f, (rnd.nextUint() & 0xFF) / 255.0f, (rnd.nextUint() & 0xFF) / 255.0f ) );
			gl::drawSolidRect( Rectf( mPacked[i] ) );
		}
	}
}

CINDER_APP_NATIVE( CinderBinPackerApp, RendererGl )
