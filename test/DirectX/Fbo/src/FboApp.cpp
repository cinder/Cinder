#include "cinder/app/AppNative.h"
#include "cinder/app/RendererDx.h"
#include "cinder/dx/dx.h"
#include "cinder/dx/DxFbo.h"
using namespace ci;
using namespace ci::app;
using namespace std;

class FboApp : public AppNative {
public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();

private:
	dx::FboRef		mFbo;
};

void FboApp::setup()
{
	mFbo = dx::Fbo::create( getWindowWidth(), getWindowHeight() );
}

void FboApp::mouseDown( MouseEvent event )
{
}

void FboApp::update()
{
}

void FboApp::draw()
{
	float dx = getWindowWidth()/3.0f;

	mFbo->bindFramebuffer();

	dx::color( Color( 1, 0, 0 ) );
	dx::drawSolidRect( Rectf( 0*dx, 0, 1*dx, getWindowHeight() ) );
	dx::color( Color( 0, 1, 0 ) );
	dx::drawSolidRect( Rectf( 1*dx, 0, 2*dx, getWindowHeight() ) );
	//dx::color( Color( 0, 0, 1 ) );
	//dx::drawSolidRect( Rectf( 2*dx, 0, 3*dx, getWindowHeight() ) );

	mFbo->unbindFramebuffer();

	// clear out the window with black
	dx::clear( Color( 0, 0, 0 ) ); 

	dx::draw( mFbo->getTexture() );
}

CINDER_APP_NATIVE( FboApp, RendererDx )
