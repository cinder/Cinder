#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/android/CinderAndroid.h"
#include "cinder/Log.h"
#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

#include <fstream>

// We'll create a new Cinder Application by deriving from the App class
class GetSpecialDirs : public App {
  public:
	void setup();
	void mouseDrag( MouseEvent event );
#if defined( CINDER_ANDROID )
    void touchesBegan( TouchEvent event );
#endif
	void keyDown( KeyEvent event );
	void draw();

	// This will maintain a list of points which we will draw line segments between
	list<vec2>		mPoints;
};

void GetSpecialDirs::setup()
{
	console() << "cinder::android::getCacheDirectory()    : " << ci::android::getCacheDirectory() << std::endl;
	console() << "cinder::android::getPicturesDirectory() : " << ci::android::getPicturesDirectory() << std::endl;

	/*
	try {
		auto file = ci::android::getPicturesDirectory() / "window.png";
		CI_LOG_I("Attempting to write fake file: " << file.string());

		std::ofstream os( file.string().c_str(), std::ofstream::binary );
		if( os.is_open() ) {
			std::vector<char> buf( 10000 );
			os.write( &buf[0], buf.size() );
			os.close();
			CI_LOG_I("Wrote fake file");
		}
	}
	catch( std::exception &exc )
	{
		CI_LOG_E("Exception writing fake file: " << exc.what());
	}
	*/

	try {
		auto file = ci::android::getPicturesDirectory() / "window.png";
		CI_LOG_I("Attempting to save window to: " << file.string());
		auto surf = copyWindowSurface();
		console() << "width    : " << surf.getWidth() << std::endl;
		console() << "height   : " << surf.getHeight() << std::endl;
		console() << "pixelInc : " << (int)surf.getPixelInc() << std::endl;
		console() << "rowBytes : " << surf.getRowBytes() << std::endl;
		CI_LOG_I("Copied window surface.");
		writeImage(file, surf);
		CI_LOG_I("Wrote image");
	}
	catch( std::exception &exc )
	{
		CI_LOG_E("Exception writing image: " << exc.what());
	}
	catch( ... )
	{
		CI_LOG_E("Unknown exception writing image");
	}
}

void GetSpecialDirs::mouseDrag( MouseEvent event )
{
	mPoints.push_back( event.getPos() );
}

#if defined( CINDER_ANDROID )
void GetSpecialDirs::touchesBegan( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
        mPoints.push_back( touchIt->getPos() );
	}
}
#endif

void GetSpecialDirs::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
}

void GetSpecialDirs::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );

	gl::color( 1.0f, 0.5f, 0.25f );	
	gl::begin( GL_LINE_STRIP );
	for( auto pointIter = mPoints.begin(); pointIter != mPoints.end(); ++pointIter ) {
		gl::vertex( *pointIter );
	}
	gl::end();
}

// This line tells Cinder to actually create the application
CINDER_APP( GetSpecialDirs, RendererGl )
