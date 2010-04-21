#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/params/Params.h"
#include "Resources.h"

#define SIZE 512

using namespace ci;
using namespace ci::app;
using namespace std;

class RDiffusionApp : public AppBasic {
  public:
	void	prepareSettings( Settings *settings );
	void	setup();
	void	update();
	void	draw();
	void	keyDown( KeyEvent event );
	void	mouseMove( MouseEvent event );
	void	mouseDown( MouseEvent event );
	void	mouseDrag( MouseEvent event );
	void	mouseUp( MouseEvent event );
	void	resetFBOs();
	
	params::InterfaceGl		mParams;
		
	int				mCurrentFBO;
	int				mOtherFBO;
	gl::Fbo			mFBOs[2];
	gl::GlslProg	mShader;
	gl::Texture		mTexture;
	
	Vec2f			mMouse;
	bool			mMousePressed;
	
	float			mReactionU;
	float			mReactionV;
	float			mReactionK;
	float			mReactionF;
	
	static const int		FBO_WIDTH = SIZE, FBO_HEIGHT = SIZE;
};

void RDiffusionApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( SIZE, SIZE );
	settings->setFrameRate( 60.0f );
}

void RDiffusionApp::mouseDown( MouseEvent event )
{
	mMousePressed = true;
}

void RDiffusionApp::mouseUp( MouseEvent event )
{
	mMousePressed = false;
}

void RDiffusionApp::mouseMove( MouseEvent event )
{
	mMouse = event.getPos();
}

void RDiffusionApp::mouseDrag( MouseEvent event )
{
	mMouse = event.getPos();
}

void RDiffusionApp::setup()
{
	mReactionU = 0.25f;
	mReactionV = 0.04f;
	mReactionK = 0.047f;
	mReactionF = 0.1f;

	mMousePressed = false;
	
	// Setup the parameters
	mParams = params::InterfaceGl( "Parameters", Vec2i( 175, 100 ) );
	mParams.addParam( "Reaction u", &mReactionU, "min=0.0 max=0.4 step=0.01 keyIncr=u keyDecr=U" );
	mParams.addParam( "Reaction v", &mReactionV, "min=0.0 max=0.4 step=0.01 keyIncr=v keyDecr=V" );
	mParams.addParam( "Reaction k", &mReactionK, "min=0.0 max=1.0 step=0.001 keyIncr=k keyDecr=K" );	
	mParams.addParam( "Reaction f", &mReactionF, "min=0.0 max=1.0 step=0.001 keyIncr=f keyDecr=F" );
	
	gl::Fbo::Format format;
	format.enableDepthBuffer( false );
	
	mCurrentFBO = 0;
	mOtherFBO = 1;
	mFBOs[0] = gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );
	mFBOs[1] = gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );
	
	mShader = gl::GlslProg( loadResource( RES_PASS_THRU_VERT ), loadResource( RES_GSRD_FRAG ) );
	mTexture = gl::Texture( loadImage( loadResource( RES_STARTER_IMAGE ) ) );
	mTexture.setWrap( GL_REPEAT, GL_REPEAT );
	mTexture.setMinFilter( GL_LINEAR );
	mTexture.setMagFilter( GL_LINEAR );
	mTexture.bind( 1 );
	
	resetFBOs();
}

void RDiffusionApp::update()
{	
	const int ITERATIONS = 25;
	// normally setMatricesWindow flips the projection vertically so that the upper left corner is 0,0
	// but we don't want to do that when we are rendering the FBOs onto each other, so the last param is false
	gl::setMatricesWindow( mFBOs[0].getSize(), false );
	gl::setViewport( mFBOs[0].getBounds() );
	for( int i = 0; i < ITERATIONS; i++ ) {
		mCurrentFBO = ( mCurrentFBO + 1 ) % 2;
		mOtherFBO   = ( mCurrentFBO + 1 ) % 2;
		
		mFBOs[ mCurrentFBO ].bindFramebuffer();
		mFBOs[ mOtherFBO ].bindTexture();
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	  
		mShader.bind();
		mShader.uniform( "texture", 0 );
		mShader.uniform( "srcTexture", 1 );
		mShader.uniform( "width", (float)FBO_WIDTH );
		mShader.uniform( "ru", mReactionU );
		mShader.uniform( "rv", mReactionV );
		mShader.uniform( "k", mReactionK );
		mShader.uniform( "f", mReactionF );
		
		gl::drawSolidRect( mFBOs[ mCurrentFBO ].getBounds() );
		
		mShader.unbind();
		if( mMousePressed ){
			glColor3f( 1.0f, 1.0f, 1.0f );
			RectMapping windowToFBO( getWindowBounds(), mFBOs[mCurrentFBO].getBounds() );
			gl::drawSolidCircle( windowToFBO.map( mMouse ), 50.0f, 32 );
		}

		mFBOs[ mCurrentFBO ].unbindFramebuffer();
	}
}

void RDiffusionApp::draw()
{
	gl::clear( ColorA( 0, 0, 0, 0 ) );
	gl::setMatricesWindow( getWindowSize() );
	gl::setViewport( getWindowBounds() );
	gl::draw( mFBOs[mCurrentFBO].getTexture(), getWindowBounds() );
	
	params::InterfaceGl::draw();
}

void RDiffusionApp::resetFBOs()
{
	mTexture.bind( 0 );
	gl::setMatricesWindow( mFBOs[0].getSize(), false );
	gl::setViewport( mFBOs[0].getBounds() );
	for( int i = 0; i < 2; i++ ){
		mFBOs[i].bindFramebuffer();
		gl::draw( mTexture, mFBOs[i].getBounds() );
	}
	gl::Fbo::unbindFramebuffer();
}

void RDiffusionApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'r' ) {
		resetFBOs();
	}
}

CINDER_APP_BASIC( RDiffusionApp, RendererGl )
