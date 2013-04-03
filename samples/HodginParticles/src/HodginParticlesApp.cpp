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
gl::TextureRef particleImg, emitterImg;
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
	void mouseMove( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void keyDown( KeyEvent event );	
	
	void update();
	void draw();
	
	Emitter		mEmitter;
	bool		mouseIsDown;
	Vec2i		mMousePos;
};

void HodginParticlesApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1024, 768 );
	settings->setFrameRate( 60.0f );
	settings->setFullScreen( false );
}

void HodginParticlesApp::setup()
{
	particleImg = gl::Texture::create( loadImage( loadResource( RES_PARTICLE ) ) );
	emitterImg = gl::Texture::create( loadImage( loadResource( RES_EMITTER ) ) );

	mouseIsDown = false;
	mMousePos = getWindowCenter();
}

void HodginParticlesApp::mouseDown( MouseEvent event )
{
	mouseIsDown = true;
	mMousePos = event.getPos();
}

void HodginParticlesApp::mouseUp( MouseEvent event )
{
	mouseIsDown = false;
}

void HodginParticlesApp::mouseMove( MouseEvent event )
{
	mMousePos = event.getPos();
}

void HodginParticlesApp::mouseDrag( MouseEvent event )
{
	mMousePos = event.getPos();
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
			mEmitter.addParticles( 5 * CINDER_FACTOR );
		}
		else {
			mEmitter.addParticles( 10 * CINDER_FACTOR );
		}
	}
}

void HodginParticlesApp::draw()
{
	gl::clear();
	gl::enableAdditiveBlending();
	
	// to accommodate resizable screen, we'll recalculate where the floor should be every frame just in case it's changed
	floorLevel = 2 / 3.0f * getWindowHeight();
	
	mEmitter.exist( mMousePos );	
}

// It would be faster to just make QUADS calls directly to the loc
// without dealing with pushing and popping for every particle. The reason
// I am doing it this longer way is due to a billboarding problem which will come
// up later on.
void renderImage( Vec3f _loc, float _diam, Color _col, float _alpha )
{
	gl::pushMatrices();
	gl::translate( _loc.x, _loc.y, _loc.z );
	gl::scale( _diam, _diam, _diam );
	gl::color( _col.r, _col.g, _col.b, _alpha );
	gl::begin( GL_QUADS );
		gl::texCoord(0, 0);    gl::vertex(-.5, -.5);
		gl::texCoord(1, 0);    gl::vertex( .5, -.5);
		gl::texCoord(1, 1);    gl::vertex( .5,  .5);
		gl::texCoord(0, 1);    gl::vertex(-.5,  .5);
	gl::end();
	gl::popMatrices();
}


CINDER_APP_BASIC( HodginParticlesApp, RendererGl )