#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct Monitor {
  public:
	Monitor( DisplayRef display, Colorf color, bool mainDisplay )
		: mArea( display->getBounds() ), mColor( color ), mMainDisplay( mainDisplay )
	{
		app::console() << display->getName() << ": " << mArea << " contentScale: " << display->getContentScale() << std::endl;
	}

	void draw() {
		gl::color( mColor * 0.25f );
		Rectf r = Rectf( mArea ).scaledCentered( 0.99f );
		gl::drawSolidRect( r );
		gl::color( mColor );
		gl::drawStrokedRect( r );
		if( mMainDisplay )
			gl::drawSolidRect( Rectf( r.x1, r.y1, r.x2, r.y1 + 40 ) );
	}

	bool		mMainDisplay;
	Colorf		mColor;
	Area		mArea;
};

class DisplayTestApp : public App {
  public:
	void setup() override;
	void draw() override;
	void populateMonitors();

	std::vector<Monitor>		mMonitors;
	Rectf						mGlobalBounds;
};

void DisplayTestApp::populateMonitors()
{
	mMonitors.clear();
	mGlobalBounds = Rectf(0, 0, 0, 0);
	float hueDelta = 1.0f / (Display::getDisplays().size() + 3);
	float hue = 0;
	bool mainDisplay = true;
	for( auto &display : Display::getDisplays()) {
		mGlobalBounds.include(display->getBounds());
		mMonitors.push_back( Monitor( display, Colorf(CM_HSV, hue, 1, 0.4f), mainDisplay ) );
		hue += hueDelta;
		mainDisplay = false;
	}
}

void DisplayTestApp::setup()
{
	populateMonitors();

	getSignalDisplayConnected().connect( [this]( const DisplayRef &display ) {
		console() << "Display connected." << std::endl;
		populateMonitors();
	} );
	getSignalDisplayDisconnected().connect([this](const DisplayRef &display) {
		console() << "Display disconnected." << std::endl;
		populateMonitors();
	} );
	getSignalDisplayChanged().connect([this](const DisplayRef &display) {
		console() << "Display changed." << std::endl;
		populateMonitors();
	} );
}

void DisplayTestApp::draw()
{
	gl::enableAlphaBlending();

	Rectf scaledBounds = mGlobalBounds;
	if( mGlobalBounds.getAspectRatio() > getWindowAspectRatio() )
		scaledBounds.scaleCentered( vec2( 1, mGlobalBounds.getAspectRatio() / getWindowAspectRatio() ) );
	else
		scaledBounds.scaleCentered( vec2(getWindowAspectRatio() / mGlobalBounds.getAspectRatio(), 1 ) );
	scaledBounds.scaleCentered( 1.1f );

	gl::clear( Color( 0, 0, 0 ) ); 
	gl::setMatrices( CameraOrtho( scaledBounds.getLowerLeft().x, scaledBounds.getLowerRight().x, scaledBounds.getLowerLeft().y, scaledBounds.getUpperLeft().y, -1, 1 ) );

	// draw the monitors
	for( auto &monitor : mMonitors )
		monitor.draw();

	// draw the window
	gl::color( 1.0f, 1.0f, 1.0f, 0.55f );
	gl::drawSolidRect( Rectf( getWindowBounds() + getWindowPos() ) );
	gl::color(1.0f, 1.0f, 1.0f, 1 );
	gl::drawStrokedRect(Rectf(getWindowBounds() + getWindowPos()));
}

CINDER_APP( DisplayTestApp, RendererGl )
