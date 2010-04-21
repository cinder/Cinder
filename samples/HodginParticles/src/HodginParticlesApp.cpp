#include "cinder/app/AppBasic.h"
#include "cinder/Perlin.h"
#include "cinder/ImageIo.h"
#include "Resources.h"

#include "Particle.h"
#include "Emitter.h"

using namespace ci;
using namespace ci::app;

// global variables
int			counter = 0;
float		floorLevel = 400.0f;
gl::Texture *particleImg, *emitterImg;
bool		ALLOWFLOOR = false;
bool		ALLOWGRAVITY = false;
bool		ALLOWPERLIN = false;
bool		ALLOWTRAILS = false;
Vec3f		gravity( 0, 0.35f, 0 );
const int	CINDER_FACTOR = 10; // how many times more particles than the Java version

class HodginParticlesApp : public AppBasic {
 public:	
	Renderer* prepareRenderer() { return new RendererGl( RendererGl::AA_MSAA_2 ); }
	void prepareSettings( Settings *settings );
	void setup();
	void mouseDown( MouseEvent event );	
	void mouseUp( MouseEvent event );
	void keyDown( KeyEvent event );	
	void update();
	void draw();
	
	Emitter emitter;
	bool mouseIsDown;
};

void HodginParticlesApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1024, 768 );
	settings->setFrameRate( 60.0f );
	settings->setFullScreen( false );
}

void HodginParticlesApp::setup()
{
	particleImg = new gl::Texture( loadImage( loadResource( RES_PARTICLE ) ) );
	emitterImg = new gl::Texture( loadImage( loadResource( RES_EMITTER ) ) );
	mouseIsDown = false;
}

void HodginParticlesApp::mouseDown( MouseEvent event )
{
	mouseIsDown = true;
}

void HodginParticlesApp::mouseUp( MouseEvent event )
{
	mouseIsDown = false;
}

void HodginParticlesApp::keyDown( KeyEvent event )
{
	char key = event.getChar();
	
	if( ( key == 'g' ) || ( key == 'G' ) )
		ALLOWGRAVITY = ! ALLOWGRAVITY;
	else if( ( key == 'p' ) || ( key == 'P' ) )
		ALLOWPERLIN = ! ALLOWPERLIN;
	else if( ( key == 't' ) || ( key == 'T' ) )
		ALLOWTRAILS = ! ALLOWTRAILS;
	else if( ( key == 'f' ) || ( key == 'F' ) )
		ALLOWFLOOR = ! ALLOWFLOOR;
}

void HodginParticlesApp::update()
{
	counter++;
	
	if( mouseIsDown ) {
		if( ALLOWTRAILS && ALLOWFLOOR ) {
			emitter.addParticles( 5 * CINDER_FACTOR );
		}
		else {
			emitter.addParticles( 10 * CINDER_FACTOR );
		}
	}
}

void HodginParticlesApp::draw()
{
	glClearColor( 0, 0, 0, 0 );
	glClear( GL_COLOR_BUFFER_BIT );
	
	// to accommodate resizable screen, we'll recalculate where the floor should be every frame just in case it's changed
	floorLevel = 2 / 3.0f * getWindowHeight();
	
	// Turns on additive blending so we can draw a bunch of glowing images without
	// needing to do any depth testing.
	glDepthMask( GL_FALSE );
	glDisable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	
	emitter.exist( getMousePos() );	
}

// It would be faster to just make QUADS calls directly to the loc
// without dealing with pushing and popping for every particle. The reason
// I am doing it this longer way is due to a billboarding problem which will come
// up later on.
void renderImage( Vec3f _loc, float _diam, Color _col, float _alpha )
{
	glPushMatrix();
	glTranslatef( _loc.x, _loc.y, _loc.z );
	glScalef( _diam, _diam, _diam );
	glColor4f( _col.r, _col.g, _col.b, _alpha );
	glBegin( GL_QUADS );
		glTexCoord2f(0, 0);    glVertex2f(-.5, -.5);
		glTexCoord2f(1, 0);    glVertex2f( .5, -.5);
		glTexCoord2f(1, 1);    glVertex2f( .5,  .5);
		glTexCoord2f(0, 1);    glVertex2f(-.5,  .5);
	glEnd();
	glPopMatrix();
}


CINDER_APP_BASIC( HodginParticlesApp, RendererGl )