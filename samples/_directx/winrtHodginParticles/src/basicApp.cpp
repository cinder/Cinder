#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererDx.h"
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

int			quadCount = 0;

float			floorLevel = 400.0f;
dx::TextureRef	particleImg, emitterImg;
bool			ALLOWFLOOR = false;
bool			ALLOWGRAVITY = false;
bool			ALLOWPERLIN = false;
bool			ALLOWTRAILS = false;

Vec3f		gravity( 0, 0.35f, 0 );
const int	CINDER_FACTOR = 5; // how many times more particles than the Java version

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
	//settings->setWindowSize( 1024, 768 );
	settings->setFrameRate( 60.0f );
	//settings->setFullScreen( false );
}

void BasicApp::setup()
{
	particleImg = dx::Texture::create( loadImage(loadAsset("particle.png")));
	emitterImg = dx::Texture::create( loadImage( loadAsset("emitter.png")));
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

	mEmitter.update(mMousePos);
	
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
	dx::color( 1.0, 1.0, 1.0, 1.0 );

	quadCount = 0;

	// to accommodate resizable screen, we'll recalculate where the floor should be every frame just in case it's changed
	floorLevel = 2 / 3.0f * getWindowHeight();
			
	dx::enableAdditiveBlending();
	
	mEmitter.render();

	dx::color( 1.0, 1.0, 1.0, 1.0 );
	std::stringstream s;
	s << "Framerate:" << getAverageFps() << " QUAD count:" << quadCount;
	dx::drawString(s.str(),Vec2f(10.0f,10.0f),Color::white(),mFont);
}

void renderImage( Vec3f _loc, float _diam, Color _col, float _alpha )
{
	quadCount++;

	dx::pushMatrices();
	dx::translate( _loc.x, _loc.y, _loc.z );
	dx::scale( _diam, _diam, _diam );
	dx::color( _col.r, _col.g, _col.b, _alpha );
	dx::texCoord(0,0);	dx::vertex( -.5, -.5);
	dx::texCoord(1,0);	dx::vertex(  .5, -.5);
	dx::texCoord(1,1);	dx::vertex(  .5,  .5);
	dx::texCoord(0,1);	dx::vertex( -.5,  .5);
	dx::popMatrices();
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( BasicApp, RendererDx )