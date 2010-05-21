#include "cinder/app/AppBasic.h"

#include "ciMsaFluidSolver.h"
#include "ciMsaFluidDrawerGl.h"

#include "Particle.h"
#include "ParticleSystem.h"

using namespace ci;
using namespace ci::app;

class msaFluidParticlesApp : public AppBasic {
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
	bool				drawParticles;
	bool				renderUsingVA;
	
	ciMsaFluidSolver	fluidSolver;
	ciMsaFluidDrawerGl	fluidDrawer;	
	ParticleSystem		particleSystem;
	
	Vec2i				pMouse;
};

void msaFluidParticlesApp::setup()
{
	console() << "ciMSAFluid Demo | (c) 2009 Mehmet Akten | www.memo.tv" << std::endl;
	
	// setup fluid stuff
	fluidSolver.setup(100, 100);
    fluidSolver.enableRGB(true).setFadeSpeed(0.002).setDeltaT(0.5).setVisc(0.00015).setColorDiffusion(0);
	fluidDrawer.setup( &fluidSolver );
	particleSystem.setFluidSolver( &fluidSolver );
	
	fluidCellsX			= 150;
	
	drawFluid			= true;
	drawParticles		= true;
	renderUsingVA		= true;
	
	setFrameRate( 60.0f );
	
	pMouse = getWindowCenter();
	resizeFluid			= true;
	
	gl::enableAlphaBlending();
}

void msaFluidParticlesApp::fadeToColor( float r, float g, float b, float speed )
{
	glColor4f( r, g, b, speed );
	gl::drawSolidRect( getWindowBounds() );
}

// add force and dye to fluid, and create particles
void msaFluidParticlesApp::addToFluid( Vec2f pos, Vec2f vel, bool addColor, bool addForce )
{
    float speed = vel.x * vel.x  + vel.y * vel.y * getWindowAspectRatio() * getWindowAspectRatio();    // balance the x and y components of speed with the screen aspect ratio
    if( speed > 0 ) {
		pos.x = constrain( pos.x, 0.0f, 1.0f );
		pos.y = constrain( pos.y, 0.0f, 1.0f );
		
        const float colorMult = 100;
        const float velocityMult = 30;
		
		if( addColor ) {
			Color drawColor( CM_HSV, ( getElapsedFrames() % 360 ) / 360.0f, 1, 1 );
			
			fluidSolver.addColorAtPos( pos, drawColor * colorMult );

			if( drawParticles )
				particleSystem.addParticles( pos * Vec2f( getWindowSize() ), 10 );
		}
		
		if( addForce )
			fluidSolver.addForceAtPos( pos, vel * velocityMult );
		
		if( ! drawFluid && getElapsedFrames()%5==0 )
			fadeToColor( 0, 0, 0, 0.1f );
    }
}

void msaFluidParticlesApp::update()
{
	if( resizeFluid ) {
		fluidSolver.setSize(fluidCellsX, fluidCellsX / getWindowAspectRatio() );
		fluidDrawer.setup(&fluidSolver);
		resizeFluid = false;
	}
	
	fluidSolver.update();
}

void msaFluidParticlesApp::draw()
{
	if( drawFluid ) {
		glColor3f(1, 1, 1);
		fluidDrawer.draw(0, 0, getWindowWidth(), getWindowHeight());
	}
	if( drawParticles )
		particleSystem.updateAndDraw( drawFluid );
}


void msaFluidParticlesApp::resize( int w, int h )
{
	particleSystem.setWindowSize( Vec2i( w, h ) );
	resizeFluid = true;
}

void msaFluidParticlesApp::keyDown( KeyEvent event )
{ 
    switch( event.getChar() ) {
		case 'f':
			setFullScreen( ! isFullScreen() );
		break;
		case ' ':
			fluidSolver.randomizeColor();
		break;
		case 'p':
			drawParticles = ! drawParticles;
		break;
		case 'b': {
			Timer timer;
			timer.start();
			const int ITERS = 1000;
			for( int i = 0; i < ITERS; ++i )
				fluidSolver.update();
			timer.stop();
			console() << ITERS << " iterations took " << timer.getSeconds() << " seconds." << std::endl;
		}
		break;
    }
}

void msaFluidParticlesApp::mouseMove( MouseEvent event )
{
	Vec2f mouseNorm = Vec2f( event.getPos() ) / getWindowSize();
	Vec2f mouseVel = Vec2f( event.getPos() - pMouse ) / getWindowSize();
	addToFluid( mouseNorm, mouseVel, true, true );
	pMouse = event.getPos();
}

void msaFluidParticlesApp::mouseDrag( MouseEvent event )
{
	Vec2f mouseNorm = Vec2f( event.getPos() ) / getWindowSize();
	Vec2f mouseVel = Vec2f( event.getPos() - pMouse ) / getWindowSize();
	addToFluid( mouseNorm, mouseVel, false, true );
	pMouse = event.getPos();
}

CINDER_APP_BASIC( msaFluidParticlesApp, RendererGl )
