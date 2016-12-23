#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/android/CinderAndroid.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// We'll create a new Cinder Application by deriving from the App class
class SetWallpaper : public App {
  public:
	void setup();
    void touchesBegan( TouchEvent event );
	void draw();
};

void SetWallpaper::setup()
{

}

void SetWallpaper::touchesBegan( TouchEvent event )
{
	Surface surf = Surface( 1080, 1920, false );
	uint8_t* pixels = surf.getData();
	for( int y = 0; y < surf.getHeight(); ++y ) {
		for( int x = 0; x < surf.getWidth(); ++x ) {
			*(pixels + 0) = x^y;
			*(pixels + 1) = x;
			*(pixels + 2) = y;
			pixels += surf.getPixelBytes();
		}
	}
	ci::android::setWallpaper( surf );
}

void SetWallpaper::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );
}

// This line tells Cinder to actually create the application
CINDER_APP( SetWallpaper, RendererGl )
