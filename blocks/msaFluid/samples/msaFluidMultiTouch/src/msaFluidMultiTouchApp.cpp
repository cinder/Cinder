#include "cinder/Cinder.h"

#if defined( CINDER_COCOA_TOUCH )
	#include "cinder/app/AppCocoaTouch.h"
	typedef ci::app::AppCocoaTouch AppBase;
#else
	#include "cinder/app/AppBasic.h"
	typedef ci::app::AppBasic AppBase;
#endif

#include "ciMsaFluidSolver.h"
#include "ciMsaFluidDrawerGl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class msaFluidMultiTouchApp : public AppBase {
 public:
	void	prepareSettings( Settings *settings );
	void	setup();
 
	void	fadeToColor( float r, float g, float b, float speed );
	void	addToFluid( Vec2f pos, Vec2f vel, int id, bool addColor, bool addForce );
	void	touchesBegan( TouchEvent event );
	void	touchesMoved( TouchEvent event );
	void	resize( int w, int h );
	
	void	update();
	void	draw();
	
	int					fluidCellsX;
	bool				resizeFluid;
	
	ciMsaFluidSolver	fluidSolver;
	ciMsaFluidDrawerGl	fluidDrawer;	
};

void msaFluidMultiTouchApp::prepareSettings( Settings *settings )
{
	settings->enableMultiTouch( true );
}

void msaFluidMultiTouchApp::setup()
{
	console() << "ciMSAFluid Demo | (c) 2009 Mehmet Akten | www.memo.tv" << std::endl;
	
	// setup fluid stuff
	fluidSolver.setup(100, 100);
    fluidSolver.enableRGB(true).setFadeSpeed(0.002).setDeltaT(0.5).setVisc(0.00015).setColorDiffusion(0);
	fluidDrawer.setup( &fluidSolver );
	
	fluidCellsX			= 150;
	
	setFrameRate( 60.0f );
	
	resizeFluid			= true;
	
	gl::enableAlphaBlending();
}

void msaFluidMultiTouchApp::addToFluid( Vec2f pos, Vec2f vel, int id, bool addColor, bool addForce )
{
    float speed = vel.x * vel.x  + vel.y * vel.y * getWindowAspectRatio() * getWindowAspectRatio();    // balance the x and y components of speed with the screen aspect ratio
    if( speed > 0 ) {
		pos.x = constrain( pos.x, 0.0f, 1.0f );
		pos.y = constrain( pos.y, 0.0f, 1.0f );
		
        float colorMult = 50;
        float velocityMult = 25;
				
		if( addColor ) {
			Color drawColor( CM_HSV, ( ( id * 60 + getElapsedFrames() ) % 360 ) / 360.0f, 1, 1 );
			
			fluidSolver.addColorAtPos( pos.x, pos.y, drawColor.r * colorMult, drawColor.g * colorMult, drawColor.b * colorMult );
		}
		
		if( addForce ) {
			fluidSolver.addForceAtPos( pos, vel * velocityMult );
		}
    }
}

void msaFluidMultiTouchApp::update()
{
	if( resizeFluid ) {
		fluidSolver.setSize(fluidCellsX, fluidCellsX / getWindowAspectRatio() );
		fluidDrawer.setup(&fluidSolver);
		resizeFluid = false;
		fluidSolver.randomizeColor();
	}
	
	fluidSolver.update();
}

void msaFluidMultiTouchApp::draw()
{
	gl::setMatricesWindow( getWindowSize() );
	gl::color( Color::white() );
	fluidDrawer.draw(0, 0, getWindowWidth(), getWindowHeight());
}

void msaFluidMultiTouchApp::resize( int w, int h )
{
	resizeFluid = true;
}

void msaFluidMultiTouchApp::touchesBegan( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		Vec2f mouseNorm = Vec2f( touchIt->getPos() ) / getWindowSize();
		Vec2f mouseVel = Vec2f( touchIt->getPos() - touchIt->getPrevPos() ) / getWindowSize();
		addToFluid( mouseNorm, mouseVel, touchIt->getId(), true, true );
	}
}

void msaFluidMultiTouchApp::touchesMoved( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		Vec2f mouseNorm = Vec2f( touchIt->getPos() ) / getWindowSize();
		Vec2f mouseVel = Vec2f( touchIt->getPos() - touchIt->getPrevPos() ) / getWindowSize();
		addToFluid( mouseNorm, mouseVel, touchIt->getId(), true, true );
	}
}

#if defined( CINDER_COCOA_TOUCH )
	CINDER_APP_COCOA_TOUCH( msaFluidMultiTouchApp, RendererGl )
#else
	CINDER_APP_BASIC( msaFluidMultiTouchApp, RendererGl )
#endif