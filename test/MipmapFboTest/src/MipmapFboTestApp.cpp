// Resize the window by making it smaller. In the original version of Cinder, the texture will fade to black due to missing mipmaps. 
// In the patched version, this has been fixed. I'm not sure if this fix breaks existing code.

#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;

class MipmapFboTestApp : public AppBasic {
public:
	void setup();
	void draw();

private:
	gl::TextureRef mTexture;
	gl::Fbo mFbo;
};

void MipmapFboTestApp::setup()
{
	mTexture = gl::Texture::create( loadImage( loadAsset( "tulips.jpg" ) ) );

	gl::Fbo::Format fboFormat;
	fboFormat.enableDepthBuffer( false );
	fboFormat.enableColorBuffer( true, 2 );
	fboFormat.setMinFilter( GL_LINEAR_MIPMAP_LINEAR );
	fboFormat.setMagFilter( GL_LINEAR );
	fboFormat.enableMipmapping( true );
	fboFormat.setSamples( 4 );
	mFbo = gl::Fbo( getWindowWidth(), getWindowHeight(), fboFormat );
}

void MipmapFboTestApp::draw()
{
	mFbo.bindFramebuffer();
	gl::setViewport( mFbo.getBounds() );
	gl::setMatricesWindow( mFbo.getSize(), false );
	gl::clear();

	glDrawBuffer( GL_COLOR_ATTACHMENT0 );

	gl::draw( mTexture, mFbo.getBounds() );

	glDrawBuffer( GL_COLOR_ATTACHMENT1 );

	gl::draw( mFbo.getTexture( 0 ), mFbo.getBounds() );
	mFbo.unbindFramebuffer();

	gl::setViewport( getWindowBounds() );
	gl::setMatricesWindow( getWindowSize() );
	gl::clear();

	gl::draw( mFbo.getTexture( 1 ), getWindowBounds() );

	std::string title( "MipmapFboTest" );
	title += " (" + toString( getWindowWidth() ) + ", " + toString( getWindowHeight() ) + ")";
	getWindow()->setTitle( title );
}

CINDER_APP_BASIC( MipmapFboTestApp, RendererGl )