#include "cinder/app/AppBasic.h"

#include "ciMsaFluidSolver.h"
#include "ciMsaFluidDrawerGl.h"

using namespace ci;
using namespace ci::app;

class msaFluidBasicApp : public AppBasic {
 public:
	void	setup();
 
	void	fadeToColor( float r, float g, float b, float speed );
	void	addToFluid( Vec2f pos, Vec2f vel, bool addColor, bool addForce );
	void	keyDown( KeyEvent event );
	void	mouseMove( MouseEvent event );
	void	mouseDrag( MouseEvent event );
	void	resize( int w, int h );
	
	void	update();
	void	draw();
	
	int					fluidCellsX;
	bool				resizeFluid;
	bool				drawFluid;
	
	ciMsaFluidSolver	fluidSolver;
	ciMsaFluidDrawerGl	fluidDrawer;	
	
	Vec2i				pMouse;
};

void msaFluidBasicApp::setup()
{
	console() << "ciMSAFluid Demo | (c) 2009 Mehmet Akten | www.memo.tv" << std::endl;
	
	// setup fluid stuff
	fluidCellsX	= 150;
	fluidSolver.setup( fluidCellsX, fluidCellsX );
    fluidSolver.enableRGB(true).setFadeSpeed(0.002f).setDeltaT(0.5f).setVisc(0.00015f).setColorDiffusion(0);
	fluidDrawer.setup( &fluidSolver );
	
	drawFluid			= true;
	
	setFrameRate( 60.0f );
	
	pMouse = getWindowCenter();
	resizeFluid			= true;
	
	gl::enableAlphaBlending();
}

void msaFluidBasicApp::fadeToColor( float r, float g, float b, float speed )
{
	glColor4f( r, g, b, speed );
	gl::drawSolidRect( getWindowBounds() );
}

void msaFluidBasicApp::addToFluid( Vec2f pos, Vec2f vel, bool addColor, bool addForce )
{
	pos.x = constrain( pos.x, 0.0f, 1.0f );
	pos.y = constrain( pos.y, 0.0f, 1.0f );
	
	const float colorMult = 50;
	const float velocityMult = 50;
	
	if( addColor ) {
		float hue = ( getElapsedFrames() % 360 ) / 360.0f;
		fluidSolver.addColorAtPos( pos, Color( CM_HSV, hue, 1, 1 ) * colorMult );
	}
	
	if( addForce )
		fluidSolver.addForceAtPos( pos, vel * velocityMult );
	
	if( ! drawFluid && getElapsedFrames()%5==0 )
		fadeToColor( 0, 0, 0, 0.1f );
}

void msaFluidBasicApp::update()
{
	if( resizeFluid ) {
		fluidSolver.setSize(fluidCellsX, fluidCellsX / getWindowAspectRatio() );
		fluidDrawer.setup(&fluidSolver);
		resizeFluid = false;
	}
	
	fluidSolver.update();
}

void msaFluidBasicApp::draw()
{
	if( drawFluid ) {
		glColor3f(1, 1, 1);
		fluidDrawer.draw(0, 0, getWindowWidth(), getWindowHeight());
	}
}

void msaFluidBasicApp::resize( int w, int h )
{
	resizeFluid = true;
}

void msaFluidBasicApp::keyDown( KeyEvent event )
{ 
    switch( event.getChar() ) {
		case 'f':
			setFullScreen( ! isFullScreen() );
		break;
		case ' ':
			fluidSolver.randomizeColor();
		break;
    }
}

void msaFluidBasicApp::mouseMove( MouseEvent event )
{
	Vec2f mouseNorm = Vec2f( event.getPos() ) / getWindowSize();
	Vec2f mouseVel = Vec2f( event.getPos() - pMouse ) / getWindowSize();
	addToFluid( mouseNorm, mouseVel, true, true );
	pMouse = event.getPos();
}

void msaFluidBasicApp::mouseDrag( MouseEvent event )
{
	Vec2f mouseNorm = Vec2f( event.getPos() ) / getWindowSize();
	Vec2f mouseVel = Vec2f( event.getPos() - pMouse ) / getWindowSize();
	addToFluid( mouseNorm, mouseVel, false, true );
	pMouse = event.getPos();
}

CINDER_APP_BASIC( msaFluidBasicApp, RendererGl )
