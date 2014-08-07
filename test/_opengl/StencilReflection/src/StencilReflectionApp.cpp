#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Camera.h"
//#include "cinder/GeoIo.h"
#include "cinder/Matrix44.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class StencilReflectionApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    
    gl::FboRef  mFbo;
    CameraPersp mCam;
    float rotation;
    bool rotate;
    gl::BatchRef mBatch;
    gl::GlslProgRef mGlsl;
    gl::TextureRef mTexture;
    bool            colorTexture;
};

void StencilReflectionApp::setup()
{
    gl::bindStockShader( gl::ShaderDef().color() );
    
    gl::Fbo::Format mFormat ;
#if defined( CINDER_GLES )
    mFormat.colorTexture().depthTexture().samples( 4 ).stencilBuffer();
	mGlsl = gl::GlslProg::create( loadAsset( "shader_es2.vert" ), loadAsset( "shader_es2.frag" ) );
#else

//    mFormat.colorTexture().stencilBuffer().depthTexture( gl::Texture::Format().internalFormat( GL_DEPTH_COMPONENT32F ) ).samples(16);
    mFormat.colorTexture().stencilBuffer().disableDepth().samples( 16 );
	mGlsl = gl::GlslProg::create( loadAsset( "shader.vert" ), loadAsset( "shader.frag" ) ); 
#endif
    mFbo = gl::Fbo::create( toPixels( getWindowWidth() ), toPixels( getWindowHeight() ), mFormat );
    colorTexture = false;
    mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 ); 
    mCam.lookAt( Vec3f( 0, -5, -1 ), Vec3f( 0, 0, 1 ) ); 
      
    rotate = rotation = 0;
    rotation = .2;
    gl::enableDepthWrite(); 
    gl::enableDepthRead();
//    gl::enableAlphaBlending();
	
}

void StencilReflectionApp::mouseDown( MouseEvent event )
{
//    if( !colorTexture ) {
//        mTexture = mFbo->getDepthTexture();
//        colorTexture = true;
//    }
//    else {
//    } 
}

void StencilReflectionApp::update()
{
    if( rotate ) {
        rotation -= 0.05f;
    }
    else {
        rotation += 0.05f;
    }
    
    mFbo->bindFramebuffer();
	// clear out the window with black
	gl::clearColor( ColorA( 1, 1, 1, 1 ) );
    gl::clear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT ) ;
    gl::pushMatrices();
        gl::setMatrices( mCam );
        gl::multModelMatrix( Matrix44f::createTranslation( Vec3f( 0, -2, -1.5 ) ) );
        gl::multModelMatrix( Matrix44f::createRotation( Vec3f( 0, 0, 1 ), -rotation ) );
    
    // enable stencil test to be able to give the stencil buffers values.
        gl::enable( GL_STENCIL_TEST );
            gl::stencilFunc( GL_ALWAYS, 1, 0xFF );
            gl::stencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
            gl::stencilMask( 0xFF );
            gl::depthMask( GL_FALSE );
            gl::clear( GL_STENCIL_BUFFER_BIT );
    // draw the seperation plane into the stencil buffer.
            gl::pushModelMatrix();
                gl::multModelMatrix( Matrix44f::createTranslation(Vec3f( 0, 0, 1.5 ) ) );
                gl::color( ColorA( 0, 0, 0, 1 ) );
                    gl::drawSolidRect( Rectf( -1.25, -1.25, 1.25, 1.25 ) );
            gl::popModelMatrix();
    // now tell the stencil what type of stenciling it has.
            gl::stencilFunc( GL_EQUAL, 1, 0xFF );
            gl::stencilMask( 0x00 );
            gl::depthMask( GL_TRUE );
            gl::pushModelMatrix();
                gl::multModelMatrix( Matrix44f::createTranslation( Vec3f( 0, 0, 1 ) ) );
                gl::color( ColorA( .5f, .5f, .5f, .5f ) );
                gl::drawCube( Vec3f( 0, 0, 1), Vec3f( 1, 1, 1 ) );
    //                mBatch->draw();
            gl::popModelMatrix();
    
        gl::disable( GL_STENCIL_TEST );
    
    gl::color( ColorA( 1, 1, 1, 1 ) );
    gl::drawCube( Vec3f( 0, 0, 1), Vec3f( 1, 1, 1 ) );
    gl::popMatrices();
    mFbo->unbindFramebuffer();
	
	mTexture = mFbo->getColorTexture();
}

void StencilReflectionApp::draw()
{
    
    gl::clearColor( Color( 0, 0, 0 ) );
	
    gl::clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
    {
		
        gl::ScopedScissor scissor( Vec2i( 0, 0 ), Vec2i( toPixels( getWindowWidth() ) / 2, toPixels( getWindowHeight() ) ) );
//        gl::TextureBindScope tex( mFbo->getDepthTexture() );
        
        gl::pushMatrices(); 
            gl::setMatricesWindow( toPixels( getWindowSize() ) );
    
//        mFbo->bindTexture();
                gl::color(1, 1, 1, 1);
                gl::multModelMatrix( Matrix44f::createRotation( Vec3f( 0, 0, 1 ), toRadians( 180.0f ) ) );
//				mTexture = mFbo->getTexture();
                gl::draw( mTexture, Vec2i( -toPixels( getWindowWidth() ), -toPixels( getWindowHeight() ) ) );
//				gl::draw( mFbo->getTexture(), Vec2i( -toPixels( getWindowWidth() ), -toPixels( getWindowHeight() ) ) );
//            gl::drawSolidRect( Rectf( 0, 0, getWindowWidth(), getWindowHeight() ) );
//        mFbo->unbindTexture();
    
        gl::popMatrices();
    }
    {
        gl::ScopedScissor scissor( Vec2i( toPixels( getWindowWidth() ) / 2, 0 ), Vec2i( toPixels( getWindowWidth() ) / 2, toPixels( getWindowHeight() ) ) );
        
        gl::clearColor( ColorA( 1, 1, 1, 1 ) );
        gl::clear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
        gl::pushMatrices();
        gl::setMatrices( mCam );
        gl::multModelMatrix( Matrix44f::createTranslation( Vec3f( 0, -2, -1.5 ) ) );
        gl::multModelMatrix( Matrix44f::createRotation( Vec3f( 0, 0, 1 ), rotation ) );
        
        // enable stencil test to be able to give the stencil buffers values.
        gl::enable( GL_STENCIL_TEST );
        gl::stencilFunc( GL_ALWAYS, 1, 0xFF );
        gl::stencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
        gl::stencilMask( 0xFF );
        gl::depthMask( GL_FALSE );
        gl::clear( GL_STENCIL_BUFFER_BIT );
        // draw the seperation plane into the stencil buffer.
        gl::pushModelMatrix();
        gl::multModelMatrix( Matrix44f::createTranslation(Vec3f( 0, 0, 1.5 ) ) );
        gl::color( ColorA( 0, 0, 0, 1 ) );
        gl::drawSolidRect( Rectf( -1.25, -1.25, 1.25, 1.25 ) );
        gl::popModelMatrix();
        // now tell the stencil what type of stenciling it has.
        gl::stencilFunc( GL_EQUAL, 1, 0xFF );
        gl::stencilMask( 0x00 );
        gl::depthMask( GL_TRUE );
        gl::pushModelMatrix();
        gl::multModelMatrix( Matrix44f::createTranslation( Vec3f( 0, 0, 1 ) ) );
        gl::color( ColorA( .5f, .5f, .5f, .5f ) );
        gl::drawCube( Vec3f( 0, 0, 1), Vec3f( 1, 1, 1 ) );
        gl::popModelMatrix();
        
        gl::disable( GL_STENCIL_TEST );
        
        gl::color( ColorA( 1, 1, 1, 1 ) );
        gl::drawCube( Vec3f( 0, 0, 1), Vec3f( 1, 1, 1 ) );
        gl::popMatrices();
    }
}

auto options = RendererGl::Options().stencil().depthBufferDepth( 32 );

CINDER_APP_NATIVE( StencilReflectionApp, RendererGl( options ) )
