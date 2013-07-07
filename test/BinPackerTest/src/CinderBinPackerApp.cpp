#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/BinPacker.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CinderBinPackerApp : public AppNative {
public:
	void setup();
	void keyDown( KeyEvent event );	
	void update();
	void draw();

	BinPacker					mBinPacker;
	std::vector<Area>			mRectangles;
	std::vector<BinPacker::Pack>	mPacks;
};

void CinderBinPackerApp::setup()
{
	mBinPacker = BinPacker().setMaxBinCount(4);
}

void CinderBinPackerApp::keyDown( KeyEvent event )
{
	int size = Rand::randInt(16,64);
	mRectangles.push_back( Area(0, 0, size, size << 1) );

	try { mBinPacker.pack( mRectangles ); }
	catch(...) { }
}

void CinderBinPackerApp::update()
{
}

void CinderBinPackerApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 

	Rand rnd;
	for(unsigned j=0;j<mBinPacker.getMaxBinCount();++j)
	{
		gl::pushModelView();
		gl::translate( j * mBinPacker.getWidth(), 0.0f, 0.0f );
		
		mPacks = mBinPacker.getPacks(j);
		for(unsigned i=0;i<mPacks.size();++i) {
			rnd.seed(mPacks[i].id+12345);
			gl::color( Color( (rnd.nextUint() & 0xFF) / 255.0f, (rnd.nextUint() & 0xFF) / 255.0f, (rnd.nextUint() & 0xFF) / 255.0f ) );
			gl::drawSolidRect( Rectf( mPacks[i].area ) );
		}

		gl::color( 1, 1, 0 );

		std::vector<Area> packs = mBinPacker.getBin(j);
		for(unsigned i=0;i<packs.size();++i)
			gl::drawStrokedRect( Rectf( packs[i] ) );

		gl::popModelView();
	}
}

CINDER_APP_NATIVE( CinderBinPackerApp, RendererGl )
