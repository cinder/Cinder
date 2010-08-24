#include "cinder/app/AppBasic.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Capture.h"
#include "cinder/Text.h"

using namespace ci;
using namespace ci::app;
using namespace std;

static const int WIDTH = 640, HEIGHT = 480;

class CaptureApp : public AppBasic {
 public:	
	void setup();
	void keyDown( KeyEvent event );
	void update();
	void draw();
	
 private:
	vector<Capture>		mCaptures;
	vector<gl::Texture>	mTextures;
	vector<gl::Texture>	mNameTextures;
	vector<Surface>		mRetainedSurfaces;
};

void CaptureApp::setup()
{	
	// list out the devices
	vector<Capture::DeviceRef> devices( Capture::getDevices() );
	for( vector<Capture::DeviceRef>::const_iterator deviceIt = devices.begin(); deviceIt != devices.end(); ++deviceIt ) {
		Capture::DeviceRef device = *deviceIt;
		console() << "Found Device " << device->getName() << " ID: " << device->getUniqueId() << std::endl;
		try {
			if( device->checkAvailable() ) {
				mCaptures.push_back( Capture( WIDTH, HEIGHT, device ) );
				mCaptures.back().start();
			
				// placeholder text
				mTextures.push_back( gl::Texture() );

				// render the name as a texture
				TextLayout layout;
				layout.setFont( Font( "Arial", 24 ) );
				layout.setColor( Color( 1, 1, 1 ) );
				layout.addLine( device->getName() );
				mNameTextures.push_back( gl::Texture( layout.render( true ) ) );
			}
			console() << "device is NOT available" << std::endl;
		}
		catch( CaptureExc & ) {
			console() << "Unable to initialize device: " << device->getName() << endl;
		}
	}
}

void CaptureApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
	else if( event.getChar() == ' ' ) {
		mCaptures.back().isCapturing() ? mCaptures.back().stop() : mCaptures.back().start();
	}
	else if( event.getChar() == 'r' ) {
		// retain a random surface to exercise the surface caching code
		int device = rand() % ( mCaptures.size() );
		mRetainedSurfaces.push_back( mCaptures[device].getSurface() );
		console() << mRetainedSurfaces.size() << " surfaces retained." << std::endl;
	}
	else if( event.getChar() == 'u' ) {
		// unretain retained surface to exercise the Capture's surface caching code
		if( ! mRetainedSurfaces.empty() )
			mRetainedSurfaces.pop_back();
		console() << mRetainedSurfaces.size() << " surfaces retained." << std::endl;
	}
}

void CaptureApp::update()
{
	for( vector<Capture>::iterator cIt = mCaptures.begin(); cIt != mCaptures.end(); ++cIt ) {
		if( cIt->checkNewFrame() ) {
			Surface8u surf = cIt->getSurface();
			mTextures[cIt - mCaptures.begin()] = gl::Texture( surf );
		}
	}
}

void CaptureApp::draw()
{
	gl::enableAlphaBlending();
	gl::clear( Color::black() );

	if( mCaptures.empty() )
		return;

	float width = getWindowWidth() / mCaptures.size();	
	float height = width / ( WIDTH / (float)HEIGHT );
	float x = 0, y = ( getWindowHeight() - height ) / 2.0f;
	for( vector<Capture>::iterator cIt = mCaptures.begin(); cIt != mCaptures.end(); ++cIt ) {	
		// draw the latest frame
		gl::color( Color::white() );
		if( mTextures[cIt-mCaptures.begin()] )
			gl::draw( mTextures[cIt-mCaptures.begin()], Rectf( x, y, x + width, y + height ) );
			
		// draw the name
		gl::color( Color::black() );	
		gl::draw( mNameTextures[cIt-mCaptures.begin()], Vec2f( x + 10 + 1, y + 10 + 1 ) );
		gl::color( Color( 0.5, 0.75, 1 ) );
		gl::draw( mNameTextures[cIt-mCaptures.begin()], Vec2f( x + 10, y + 10 ) );

		x += width;
	}
}


CINDER_APP_BASIC( CaptureApp, RendererGl )
