#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include "cinder/app/AppBasic.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;

class SurfaceTestApp : public AppBasic {
 public:	
	void prepareSettings( Settings *settings );
	void setup();
	virtual void update();
	virtual void draw();
	
	void keyDown( KeyEvent event );
	void mouseDown( MouseEvent event );

	int offsetX, offsetY;
	bool animating;
	Surface				mSurfaceRedBlue, mSurfaceBlueGreen, mSurfaceComposite;
	gl::Texture			mTexture;
};


void renderRedBlueGradient( Surface *s )
{
	Surface::Iter iter = s->getIter();

	while( iter.line() ) {
		while( iter.pixel() ) {
			iter.r() = iter.x() / (float)s->getWidth() * 255;
			iter.g() = 0;
			iter.b() = iter.y() / (float)s->getHeight() * 255;
		}
		iter.line();
	}
}

void renderBlueGreenGradient( Surface *s )
{
	Surface::Iter iter = s->getIter();

	while( iter.line() ) {
		while( iter.pixel() ) {
			iter.r() = 0;
			iter.g() = iter.x() / (float)s->getWidth() * 255;
			iter.b() = iter.y() / (float)s->getHeight() * 255;
		}
	}
}

void copyRandom( const Surface &background, const Surface &foreground, Surface *dest )
{
	const int ITERATIONS = 1000;
	
	dest->copyFrom( background, dest->getBounds() );
	
	for( int i = 0; i < ITERATIONS; ++i ) {
		Area randArea( 0, 0, Rand::randInt( dest->getWidth() ), Rand::randInt( dest->getHeight() ) );
		randArea.offsetBy( Vec2i( Rand::randInt( 30 ), Rand::randInt( 30 ) ) );
		
		Vec2i randOffset( Rand::randInt( dest->getWidth() ), Rand::randInt( dest->getHeight() ) );
		dest->copyFrom( foreground, randArea, randOffset );
	}
}

void SurfaceTestApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 640, 480 );
	settings->setFullScreen( false );
	settings->setResizable( true );
	settings->setFrameRate( 60.0f );
}

void SurfaceTestApp::setup()
{
	const int TEXTURE_WIDTH = 640, TEXTURE_HEIGHT = 480;

	mSurfaceRedBlue = Surface( TEXTURE_WIDTH, TEXTURE_HEIGHT, false ); 
	renderRedBlueGradient( &mSurfaceRedBlue );
	mSurfaceBlueGreen = Surface( TEXTURE_WIDTH, TEXTURE_HEIGHT, false ); 
	renderBlueGreenGradient( &mSurfaceBlueGreen );
	
	mSurfaceComposite = Surface( TEXTURE_WIDTH, TEXTURE_HEIGHT, false ); 
	
	mTexture = gl::Texture( mSurfaceComposite );
	
	offsetX = offsetY = 0;
	animating = true;
	glEnable( GL_TEXTURE_2D );
}

void SurfaceTestApp::keyDown( KeyEvent event )
{
	if( event.getCode() == KeyEvent::KEY_f ) {
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getChar() == ' ' ) {
		animating = true;
	}
}

void SurfaceTestApp::mouseDown( MouseEvent event )
{
	if( event.isLeft() )
		animating = false;
		
	if( ! animating )
		copyRandom( mSurfaceRedBlue, mSurfaceBlueGreen, &mSurfaceComposite );
}

void SurfaceTestApp::update()
{
	if( animating ) {
		offsetX = ( offsetX + 5 ) % mSurfaceComposite.getWidth();
		offsetY = ( offsetY + 5 ) % mSurfaceComposite.getHeight();
		Area copyArea( 0, 0, offsetX, mSurfaceComposite.getHeight() );
		mSurfaceComposite.copyFrom( mSurfaceBlueGreen, copyArea, Vec2i( 0, offsetY ) );
		copyArea.set( offsetX, 0, mSurfaceComposite.getWidth(), mSurfaceComposite.getHeight() );
		mSurfaceComposite.copyFrom( mSurfaceRedBlue, copyArea, Vec2i( 0, offsetY ) );
	}
	
	mTexture = gl::Texture( mSurfaceComposite );
}

void SurfaceTestApp::draw()
{
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_TEXTURE_2D );

	gl::draw( mTexture );
}


CINDER_APP_BASIC( SurfaceTestApp, RendererGL )