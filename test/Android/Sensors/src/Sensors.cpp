#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/MotionManager.h"
#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

// We'll create a new Cinder Application by deriving from the App class
class Sensors : public App {
public:
	void setup();
    void touchesBegan( TouchEvent event );
    void touchesEnded( TouchEvent event );
	void draw();

private:
	bool 	mPrint = false;
};

std::string getOrientationString(int io)
{
	std::string result = "Unknown";
	switch( io ) {
		case ci::app::InterfaceOrientation::Portrait: {
			result = "Portrait";
		}
		break;

		case ci::app::InterfaceOrientation::PortraitUpsideDown: {
			result = "PortraitUpsideDown";
		}
		break;

		case ci::app::InterfaceOrientation::LandscapeLeft: {
			result = "LandscapeLeft";
		}
		break;

		case ci::app::InterfaceOrientation::LandscapeRight: {
			result = "LandscapeRight";
		}
		break;
	};
	return result;
}

void prepareSettings( Sensors::Settings* settings )
{
	settings->setFullScreen( true );
}

void Sensors::setup()
{
	MotionManager::enable( 60.0f, MotionManager::Accelerometer );

	int io = getOrientation();
	console() << "InterfaceOrientation: " << getOrientationString( io ) << " : " << io << std::endl;

	console() << "Window Size: " << getWindowSize() << std::endl;
}

void Sensors::touchesBegan( TouchEvent event )
{
	mPrint = true;
}

void Sensors::touchesEnded( TouchEvent event )
{
	mPrint = false;
}

void Sensors::draw()
{
	if( mPrint ) {
		//console() << "accel: " << MotionManager::getAccelerometer() << std::endl;
		console() << "rotation: " << MotionManager::getRotation( getOrientation() ) << std::endl;
	}

	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
	gl::enableDepthRead();
	gl::enableDepthWrite();

	gl::setMatricesWindowPersp( getWindowSize(), 60.0f, 1.0f, 100000.0f );
	gl::pushModelMatrix();
		glCullFace( GL_BACK );
		gl::translate( getWindowSize() / 2 );
		gl::scale( vec3( 200.0f, 200.0f, 200.0f ) );
		gl::color( 1.0f, 1.0f, 1.0f );
		//gl::drawSphere( vec3(0.0f), 1.0f, 8 );
		gl::rotate( MotionManager::getRotation( getOrientation() ) );
		gl::drawColorCube( vec3(0.0f), vec3(1.0f) );
	gl::popModelMatrix();
}

// This line tells Cinder to actually create the application
CINDER_APP( Sensors, RendererGl, prepareSettings )
