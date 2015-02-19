#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct Monitor {
  public:
	Monitor( DisplayRef display, Colorf color )
		: mArea( display->getBounds() ), mColor( color )
	{}

	void draw() {
		gl::color( mColor * 0.25f );
		gl::drawSolidRect( Rectf( mArea ).scaledCentered( 0.99f ) );
		gl::color( mColor );
		gl::drawStrokedRect( Rectf(mArea).scaledCentered(0.99f) );
	}

	Colorf	mColor;
	Area	mArea;
};

class DisplayTestApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
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
	for (auto &display : Display::getDisplays()) {
		mGlobalBounds.include(display->getBounds());
		mMonitors.push_back(Monitor(display, Colorf(CM_HSV, hue, 1, 0.4f)));
		hue += hueDelta;
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

void DisplayTestApp::mouseDown( MouseEvent event )
{
}

void DisplayTestApp::update()
{
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
	gl::color( 1.0f, 1.0f, 1.0f, 0.75f );
	gl::drawSolidRect( Rectf( getWindowBounds() + getWindowPos() ) );
	gl::color(1.0f, 1.0f, 1.0f, 1 );
	gl::drawStrokedRect(Rectf(getWindowBounds() + getWindowPos()));
}

CINDER_APP( DisplayTestApp, RendererGl )
