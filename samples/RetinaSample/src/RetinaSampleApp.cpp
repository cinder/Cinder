#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class RetinaSampleApp : public AppNative {
  public:
	void prepareSettings( Settings *settings ) override;
	void setup();
	void mouseDrag( MouseEvent event );
	void mouseDown( MouseEvent event );
	void keyDown( KeyEvent event );
	void displayChange();
	void draw();
	
	// This will maintain a list of points which we will draw line segments between
	list<Vec2f>		mPoints;
};

void RetinaSampleApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 800, 600 );
	settings->enableHighDensityDisplay();
	console() << "settings->getHighDensityDisplayEnabled()= " << settings->isHighDensityDisplayEnabled() << endl;
	
}

void RetinaSampleApp::setup()
{
	getWindow()->getSignalDisplayChange().connect( std::bind( &RetinaSampleApp::displayChange, this ) );
}

void RetinaSampleApp::mouseDrag( MouseEvent event )
{
	console() << event.getPos()	<< endl;
	mPoints.push_back( event.getPos() );
}

void RetinaSampleApp::mouseDown( MouseEvent event )
{
	console() << "getWindowContentScale = " << getWindowContentScale() << endl;
	console() << event.getPos()	<< endl;
}

void RetinaSampleApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
}

void RetinaSampleApp::displayChange()
{
	console() << "Window display changed: " << getWindow()->getDisplay()->getBounds() << std::endl;
	console() << "ContentScale = " << getWindowContentScale() << endl;
	console() << "getWindowCenter() = " << getWindowCenter() << endl;
	console() << "getWindow()->toPixels( 1.0f ) = " << toPixels( 1.0f ) << endl;

}

void RetinaSampleApp::draw()
{
	
	// float c = getWindowContentScale();
	
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );
	gl::color( 1.0f, 0.5f, 0.25f );
	
	gl::pushMatrices();
		gl::begin( GL_LINE_STRIP );
		glLineWidth( getWindow()->toPixels( 1.0f ) );
		for( auto pointIter = mPoints.begin(); pointIter != mPoints.end(); ++pointIter ) {
			gl::vertex( *pointIter );
		}
		gl::end();
	gl::popMatrices();
	
	gl::pushMatrices();
		glColor3f( 1.0f, 0.2f, 0.15f );
		gl::translate( getWindowCenter() );
		gl::rotate( getElapsedSeconds() * 5 );
		gl::drawSolidRect( Rectf( -100, -100, 100, 100 ) );
	gl::popMatrices();
	
}

CINDER_APP_NATIVE( RetinaSampleApp, RendererGl )
