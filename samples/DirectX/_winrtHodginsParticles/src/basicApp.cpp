#include "cinder/app/AppBasic.h"
#include "cinder/Perlin.h"
#include "cinder/ImageIo.h"
#include "cinder/Font.h"

#include "Particle.h"
#include "Emitter.h"

#include "cinder/dx/dx.h"
#include "cinder/dx/DxTexture.h"

using namespace ci;
using namespace ci::app;

// global variables
int			counter = 0;
float		floorLevel = 400.0f;
dx::Texture *particleImg, *emitterImg;
bool		ALLOWFLOOR = false;
bool		ALLOWGRAVITY = false;
bool		ALLOWPERLIN = false;
bool		ALLOWTRAILS = false;
Vec3f		gravity( 0, 0.35f, 0 );
const int	CINDER_FACTOR = 10; // how many times more particles than the Java version

class BasicApp : public AppBasic {
  public:
	void prepareSettings( Settings *settings );
	void setup();
	
	void mouseDown( MouseEvent event );	
	void mouseUp( MouseEvent event );
	void mouseMove( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void keyDown( KeyEvent event );	
	
	void update();
	void draw();
	
	Font		mFont;
	Emitter		mEmitter;
	bool		mouseIsDown;
	Vec2i		mMousePos;
};

void BasicApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1024, 768 );
	settings->setFrameRate( 60.0f );
	settings->setFullScreen( false );
}

void BasicApp::setup()
{
	particleImg = new dx::Texture( loadImage(loadAsset("particle.png")));
	emitterImg = new dx::Texture( loadImage( loadAsset("emitter.png")));
	mFont = Font( "Arial", 24.0f );
	mouseIsDown = false;
	mMousePos = getWindowCenter();
}

void BasicApp::mouseDown( MouseEvent event )
{
	mouseIsDown = true;
	mMousePos = event.getPos();
}

void BasicApp::mouseUp( MouseEvent event )
{
	mouseIsDown = false;
}

void BasicApp::mouseMove( MouseEvent event )
{
	mMousePos = event.getPos();
}

void BasicApp::mouseDrag( MouseEvent event )
{
	mMousePos = event.getPos();
}

void BasicApp::keyDown( KeyEvent event )
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

void BasicApp::update()
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

void BasicApp::draw()
{
	dx::clear( Color(0,0,0), true);
//	glClearColor( 0, 0, 0, 0 );
//	glClear( GL_COLOR_BUFFER_BIT );
	
	// to accommodate resizable screen, we'll recalculate where the floor should be every frame just in case it's changed
	floorLevel = 2 / 3.0f * getWindowHeight();
	
	// Turns on additive blending so we can draw a bunch of glowing images without
	// needing to do any depth testing.
	
	//glDepthMask( GL_FALSE );
	//glDisable( GL_DEPTH_TEST );
	//glEnable( GL_BLEND );
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	dx::enableAdditiveBlending();
	
	mEmitter.exist( mMousePos );	
	std::stringstream s;
	s << "Framerate:" << getAverageFps();
	dx::drawString(s.str(),Vec2f(10.0f,10.0f),Color::white(),mFont);
}

// It would be faster to just make QUADS calls directly to the loc
// without dealing with pushing and popping for every particle. The reason
// I am doing it this longer way is due to a billboarding problem which will come
// up later on.
void renderImage( Vec3f _loc, float _diam, Color _col, float _alpha )
{

	dx::pushMatrices();
	dx::translate( _loc.x, _loc.y, _loc.z );
	dx::begin( GL_QUADS );
		dx::texCoord(0,0);	dx::vertex( -25, -25);
		dx::texCoord(1,0);	dx::vertex(  25, -25);
		dx::texCoord(1,1);	dx::vertex(  25,  25);
		dx::texCoord(0,1);	dx::vertex( -25,  25);
	dx::end();
	dx::popMatrices();

}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( BasicApp, RendererDx )