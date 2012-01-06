/*
 * Code Copyright 2011 Chris McKenzie ( http://chrismckenzie.com )
 * Used with permission for the Cinder Project ( http://libcinder.org )
 *
 * Alfphabet-IV.ttf Copyright (C) 2007-2010 OSP (Pierre Huyghebaert and Ludivine Loiseau)
 * SIL OPEN FONT LICENSE Version 1.1
 *
 */

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include <vector>
#include <list>

#include "Resources.h"
#include "Character.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TextInputTweenApp : public AppBasic {
  public:
	void prepareSettings( Settings *settings );
	void setup();
	void keyDown( KeyEvent event );
	void update();
	void draw();
	
	void addChar( char c );
	void removeChar();
	
	Vec3f getRandomVec3f( float min, float max );
	
	gl::TextureFontRef	mTextureFont;
	
	vector<Character>	mCharacters;
	list<Character>		mDyingCharacters;
	string				mText;
	
	CameraPersp			mCam;
	float				mCamDist;
	
	Anim<Matrix44f>		mSceneMatrix;
	Matrix44f			mSceneDestMatrix;
};

void TextInputTweenApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 700, 700 );
	settings->setFrameRate( 60 );
}

void TextInputTweenApp::setup()
{
	gl::enableAlphaBlending();
	
	// font
	Font customFont( Font( loadResource( RES_CUSTOM_FONT ), 100 ) );
	gl::TextureFont::Format f;
	f.enableMipmapping( true );
	mTextureFont = gl::TextureFont::create( customFont, f );
	
	// camera
	mCamDist = 600.0f;
	mCam.setPerspective( 75.0f, getWindowAspectRatio(), 0.1f, 5000.0f );
	mCam.lookAt( Vec3f( 0.0f, 0.0f, mCamDist ), Vec3f::zero(), Vec3f::yAxis() );
	
	// scene
	mSceneMatrix = mSceneDestMatrix = Matrix44f::identity();
	
	// init text
	addChar( 'T' );
	addChar( 'Y' );
	addChar( 'P' );
	addChar( 'E' );
	addChar( ' ' );
}

void TextInputTweenApp::keyDown( KeyEvent event )
{
	char ch = event.getChar();
	string specialChars = " .,:;/?\\!@#%^&*()-_=+[]<>'\""; // the remaining characters that the font provides
	
	if( event.getCode() == KeyEvent::KEY_BACKSPACE )
		removeChar();
	else if( isalnum( ch ) )
		addChar( ch );
	else if( specialChars.find( ch ) != -1 )
		addChar( ch );
}

void TextInputTweenApp::addChar( char c )
{
	c = tolower( c ); // Alphabet-IV.tff seems to be missing some capital letters (strange, since it's an all-caps font)
	int count = mCharacters.size();
	
	if( count > 0 )
		mSceneDestMatrix.translate( Vec3f( mCharacters.back().getKernBounds().getWidth() / 2.0f, 0.0f, 0.0f ) );
	
	Matrix44f randStartMatrix = mSceneDestMatrix;
	randStartMatrix.translate( getRandomVec3f( 100.0f, 600.0f ) );
	randStartMatrix.rotate( getRandomVec3f( 2.0f, 6.0f ) );
	
	mCharacters.push_back( Character( mTextureFont, string( &c, 1 ), randStartMatrix ) );
	
	mSceneDestMatrix.translate( Vec3f( mCharacters.back().getKernBounds().getWidth() / 2.0f, 0.0f, 0.0f ) );

	float t = (count + 281)/50.0f;
	mSceneDestMatrix.rotate( Vec3f( sin(t)*0.1f, cos(t)*0.2f, cos(t)*0.05f ) ); // makes the scene meander
	
	mCharacters.back().animIn( timeline(), mSceneDestMatrix );
	
	timeline().apply( &mSceneMatrix, mSceneDestMatrix, 1.0f, EaseOutAtan( 10 ) );
}

void TextInputTweenApp::removeChar()
{
	if( ! mCharacters.empty() ) {
		mDyingCharacters.push_back( mCharacters.back() );
		mCharacters.pop_back();
		
		if( ! mCharacters.empty() )
			mSceneDestMatrix = mCharacters.back().getDestMatrix();
		else
			mSceneDestMatrix = Matrix44f::identity();
		
		Matrix44f randStartMatrix = mSceneDestMatrix;
		randStartMatrix.translate( getRandomVec3f( 100.0f, 600.0f ) );
		randStartMatrix.rotate( getRandomVec3f( 2.0f, 6.0f ) );
		
		mDyingCharacters.back().animOut( timeline(), randStartMatrix );
		
		timeline().apply( &mSceneMatrix, mSceneDestMatrix, 1.0f, EaseOutAtan( 10 ) );
	}
}

void TextInputTweenApp::update()
{
	float camDistDest = 600.0f + sin( mCharacters.size() * 0.1f ) * 300.0f;
	mCamDist -= ( mCamDist - camDistDest ) * 0.1f;
	
	mCam.lookAt( Vec3f( 0.0f, 0.0f, mCamDist ), Vec3f::zero(), Vec3f::yAxis() );
}

void TextInputTweenApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatrices( mCam );
	gl::multModelView( mSceneMatrix().inverted() );
	
	for( vector<Character>::iterator it = mCharacters.begin(); it != mCharacters.end(); ++it )
		it->draw();

	for( list<Character>::iterator it = mDyingCharacters.begin(); it != mDyingCharacters.end(); ++it ) 
		it->draw();
		
	if( ( ! mDyingCharacters.empty() ) && mDyingCharacters.front().isDead() )
		mDyingCharacters.pop_front();
}


Vec3f TextInputTweenApp::getRandomVec3f( float min, float max )
{
	return Rand::randVec3f() * Rand::randFloat( min, max );
}


CINDER_APP_BASIC( TextInputTweenApp, RendererGl )
