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
	enum Mode { SINGLE_COPY, MULTI_COPY };

	void prepareSettings( Settings *settings );
	void setup();
	void keyDown( KeyEvent event );	
	void update();
	void draw();

	BinPacker					mBinPackerSingle;
	MultiBinPacker				mBinPackerMulti;

	std::vector<Area>			mUnpacked;
	std::vector<BinnedArea>		mPacked;

	Mode						mMode;
};

void CinderBinPackerApp::prepareSettings(Settings *settings)
{
	settings->setWindowSize(512, 512);
}

void CinderBinPackerApp::setup()
{
	mMode = SINGLE_COPY;

	mBinPackerSingle.setSize( 128, 128 );
	mBinPackerMulti.setSize( 128, 128 );
}

void CinderBinPackerApp::keyDown( KeyEvent event )
{
	switch( event.getCode() )
	{
	case KeyEvent::KEY_1:
		// enable single bin, copy mode
		mMode = SINGLE_COPY;
		break;
	case KeyEvent::KEY_2:
		// enable multi bin, copy mode
		mMode = MULTI_COPY;
		break;
	default:
		// add an Area of random size to mUnpacked
		//int size = math<int>::pow(2, Rand::randInt(4, 7));
		int size = Rand::randInt(16, 64);
		mUnpacked.push_back( Area(0, 0, size, size) );
		break;
	}

	switch( mMode )
	{
	case SINGLE_COPY:
		// show the total number of Area's in the window title bar
		getWindow()->setTitle( "CinderBinPackerApp | Single Bin | " + ci::toString( mUnpacked.size() ) );

		try
		{ 
			// mPacked will contain all Area's of mUnpacked in the exact same order,
			// but moved to a different spot in the bin. Unpacked will not be altered.
			mPacked = mBinPackerSingle.pack( mUnpacked ); 
		}
		catch(...) 
		{  
			// the bin is not large enough to contain all Area's, so let's
			// double the size...
			int size = mBinPackerSingle.getWidth();
			mBinPackerSingle.setSize( size << 1, size << 1 );

			/// ...and try again
			mPacked = mBinPackerSingle.pack( mUnpacked ); 
		}
		break;
	case MULTI_COPY:
		// show the total number of Area's in the window title bar
		getWindow()->setTitle( "CinderBinPackerApp | Multi Bin | " + ci::toString( mUnpacked.size() ) );

		try
		{ 
			//  mPacked will contain all Area's of mUnpacked in the exact same order,
			// but moved to a different spot in the bin and represented as a BinnedArea.
			// Use the BinnedArea::getBin() method to find out to which bin the Area belongs.
			// BinnedAreas can be used directly as Areas, conversion will happen automatically.
			mPacked = mBinPackerMulti.pack( mUnpacked ); 
		}
		catch(...) 
		{  
			// will only throw if any of the input rects is too big to fit a single bin, 
			// which is not the case in this demo
		}
		break;
	}
}

void CinderBinPackerApp::update()
{
}

void CinderBinPackerApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	// draw all packed Area's
	Rand rnd;	

	switch( mMode )
	{
	case SINGLE_COPY:
		// draw the borders of the bin
		gl::color( Color( 1, 1, 0 ) );
		gl::drawStrokedRect( Rectf( Vec2f::zero(), mBinPackerSingle.getSize() ) );

		// packing has been done by copy, so use the mPackedSingle array
		for(unsigned i=0;i<mPacked.size();++i) {
			rnd.seed(i+12345);
			gl::color( Color( (rnd.nextUint() & 0xFF) / 255.0f, (rnd.nextUint() & 0xFF) / 255.0f, (rnd.nextUint() & 0xFF) / 255.0f ) );
			gl::drawSolidRect( Rectf( mPacked[i] ) );
		}
		break;
	case MULTI_COPY:
		{
			unsigned n = (unsigned) floor( getWindowWidth() / (float) mBinPackerMulti.getWidth() );

			// packing has been done by multi-copy, so use the mPackedMulti array
			for(unsigned i=0;i<mPacked.size();++i) {
				int bin = mPacked[i].getBin();

				gl::pushModelView();
				gl::translate( (float) ((bin % n) * mBinPackerMulti.getWidth()), (float) ((bin / n) * mBinPackerMulti.getHeight()), 0.0f );

				// draw bin
				rnd.seed(i+12345);
				gl::color( Color( (rnd.nextUint() & 0xFF) / 255.0f, (rnd.nextUint() & 0xFF) / 255.0f, (rnd.nextUint() & 0xFF) / 255.0f ) );
				gl::drawSolidRect( Rectf( mPacked[i] ) );

				// draw the borders of the bin
				gl::color( Color( 1, 1, 0 ) );
				gl::drawStrokedRect( Rectf( Vec2f::zero(), mBinPackerMulti.getSize() ) );

				gl::popModelView();
			}
		}
		break;
	}
}

CINDER_APP_NATIVE( CinderBinPackerApp, RendererGl )
