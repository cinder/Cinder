/*
 * Code Copyright 2011 Chris McKenzie ( http://chrismckenzie.com )
 * Used with permission for the Cinder Project ( http://libcinder.org )
 *
 * Alfphabet-IV.ttf Copyright (C) 2007-2010 OSP (Pierre Huyghebaert and Ludivine Loiseau)
 * SIL OPEN FONT LICENSE Version 1.1
 *
 */

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
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

class TextInputTweenApp : public App {
  public:
	static void prepareSettings( Settings *settings );
	void setup();
	void keyDown( KeyEvent event );
	void update();
	void draw();

	void addChar( char c );
	void removeChar();

	gl::TextureFontRef	mTextureFont;

	vector<Character>	mCharacters;
	list<Character>		mDyingCharacters;
	string				mText;

	CameraPersp			mCam;
	float				mCamDist;

	Anim<mat4>		mSceneMatrix;
	mat4			mSceneDestMatrix;
};

void TextInputTweenApp::prepareSettings( Settings *settings )
{
#if defined( CINDER_MAC )
    	settings->setHighDensityDisplayEnabled(true);
#endif
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
	mCam.lookAt( vec3( 0.0f, 0.0f, mCamDist ), vec3( 0 ), vec3( 0, 1, 0 ) );

	// scene
	mSceneMatrix = mSceneDestMatrix = mat4();

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
		mSceneDestMatrix *= translate( vec3( mCharacters.back().getKernBounds().getWidth() / 2.0f, 0.0f, 0.0f ) );

	mat4 randStartMatrix = mSceneDestMatrix;
	randStartMatrix *= translate( randVec3() * randFloat( 100.0f, 600.0f ) );
	randStartMatrix *= rotate( randFloat( 2.0f, 6.0f ), randVec3() );
	mCharacters.push_back( Character( mTextureFont, string( &c, 1 ), randStartMatrix ) );

	mSceneDestMatrix *= translate( vec3( mCharacters.back().getKernBounds().getWidth() / 2.0f, 0.0f, 0.0f ) );

	// make the scene meander
	float t = ( count + 281 ) / 50.0f;
	mSceneDestMatrix *= glm::eulerAngleYXZ( cosf( t ) * 0.2f, sinf( t ) * 0.1f, cosf( t ) * 0.05f );

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
			mSceneDestMatrix = mat4();

		mat4 randStartMatrix = mSceneDestMatrix;
		randStartMatrix *= translate( randVec3() * randFloat( 100.0f, 600.0f ) );
		randStartMatrix *= rotate( randFloat( 2.0f, 6.0f ), randVec3() );

		mDyingCharacters.back().animOut( timeline(), randStartMatrix );

		timeline().apply( &mSceneMatrix, mSceneDestMatrix, 1.0f, EaseOutAtan( 10 ) );
	}
}

void TextInputTweenApp::update()
{
	float camDistDest = 600.0f + sin( mCharacters.size() * 0.1f ) * 300.0f;
	mCamDist -= ( mCamDist - camDistDest ) * 0.1f;

	mCam.lookAt( vec3( 0.0f, 0.0f, mCamDist ), vec3( 0 ), vec3( 0, 1, 0 ) );
}

void TextInputTweenApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatrices( mCam );
	gl::multModelMatrix( inverse( mSceneMatrix() ) );

	for( auto &c : mCharacters )
		c.draw();

	for( auto &c : mDyingCharacters )
		c.draw();

	if( ( ! mDyingCharacters.empty() ) && mDyingCharacters.front().isDead() )
		mDyingCharacters.pop_front();
}

CINDER_APP( TextInputTweenApp, RendererGl, &TextInputTweenApp::prepareSettings )
