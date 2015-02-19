#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/Unicode.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Texture.h"

#include "cinder/ColorManagement.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ColorTestApp : public AppNative {
  public:
	void setup() override;
	void fileDrop( FileDropEvent event ) override;
	void prepareSettings( Settings *settings ) override;
	void keyDown( KeyEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void draw() override;

	void processImage( const fs::path &path );
	
	ColorSpace			mSourceSpace, mDestSpace;
	gl::TextureRef		mCorrectedTexture, mUncorrectedTexture;

	float	mDivideOffset;
};

void ColorTestApp::prepareSettings( Settings *settings )
{
	settings->enableMultiTouch( false );
}

void ColorTestApp::setup()
{
	mDestSpace = getWindow()->getDisplay()->getColorSpace();
	
	mDivideOffset = 0.5f;
}

void ColorTestApp::processImage( const fs::path &path )
{
	Surface surface( loadImage( path ) );
	mUncorrectedTexture = gl::Texture::create( surface );
	surface.convertColorSpace( mDestSpace );
	mCorrectedTexture = gl::Texture::create( surface );
}

void ColorTestApp::fileDrop( FileDropEvent event )
{
	processImage( event.getFiles()[0] );
}

void ColorTestApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 's' ) {
		fs::path outPath = getSaveFilePath();
		if( ! outPath.empty() ) {
			Surface s( mUncorrectedTexture->createSource() );
//			writeImage( outPath, s, ImageTarget::Options().assignColorSpace( mSourceSpace ) );
		}
	}
}

void ColorTestApp::mouseDrag( MouseEvent event )
{
	mDivideOffset = event.getPos().x / (float)getWindowWidth();
}

void ColorTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::disableDepthRead();

	{
		if( mCorrectedTexture )
			gl::draw( mCorrectedTexture, toPoints( mCorrectedTexture->getBounds() ) );
		gl::ScopedScissor sscp( ivec2( 0 ), toPixels( ivec2( getWindowWidth() * mDivideOffset, getWindowHeight() ) ) );
		if( mUncorrectedTexture )
			gl::draw( mUncorrectedTexture, toPoints( mUncorrectedTexture->getBounds() ) );
	}
	
	// draw divider
	gl::color( Color::black() );
	gl::drawLine( toPixels( vec2( getWindowWidth() * mDivideOffset + 1, 0 ) ),
					toPixels( vec2( getWindowWidth() * mDivideOffset + 1, getWindowHeight() ) ) );
	gl::color( Color::white() );
	gl::drawLine( toPixels( vec2( getWindowWidth() * mDivideOffset, 0 ) ),
					toPixels( vec2( getWindowWidth() * mDivideOffset, getWindowHeight() ) ) );
}

CINDER_APP_NATIVE( ColorTestApp, RendererGl )
