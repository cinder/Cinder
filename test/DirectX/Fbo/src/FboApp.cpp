#include "cinder/app/AppNative.h"
#include "cinder/app/RendererDx.h"
#include "cinder/dx/dx.h"
#include "cinder/dx/DxRenderTarget.h"
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
	dx::RenderTargetRef		mRenderTarget;
};

void FboApp::setup()
{
	mRenderTarget = dx::RenderTarget::create( getWindowWidth(), getWindowHeight() );
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

	mRenderTarget->bindFramebuffer();

	dx::color( Color( 1, 0, 0 ) );
	dx::drawSolidRect( Rectf( 0*dx, 0, 1*dx, getWindowHeight() ) );
	dx::color( Color( 0, 1, 0 ) );
	dx::drawSolidRect( Rectf( 1*dx, 0, 2*dx, getWindowHeight() ) );
	//dx::color( Color( 0, 0, 1 ) );
	//dx::drawSolidRect( Rectf( 2*dx, 0, 3*dx, getWindowHeight() ) );

	mRenderTarget->unbindFramebuffer();

	// clear out the window with black
	dx::clear( Color( 0, 0, 0 ) ); 

	dx::draw( mRenderTarget->getTexture() );
}

CINDER_APP_NATIVE( FboApp, RendererDx )
