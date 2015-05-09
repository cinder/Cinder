#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/CameraUi.h"
#include "cinder/Rect.h"

#include "RayMarcher.h"

using namespace ci;
using namespace ci::app;

class RayMarcherApp : public App {
 public:	
	RayMarcherApp() : mMarcher( &mCamera ) {}
	
	void		prepareSettings( Settings *settings );

	void		setup();
	void		keyDown( KeyEvent event );	
	void		resize();
	void		update();
	void		draw();

	std::shared_ptr<Surface8u>	mImageSurface;
	gl::Texture2dRef			mImageTexture;

	RayMarcher		mMarcher;
	CameraPersp		mCamera;
	CameraUi		mCamUi;
	vec3			mStartEyePoint;
	int				mCurrentLine;
	
	gl::GlslProgRef	mGlsl;
};

void RayMarcherApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 400, 300 );
	settings->setFullScreen( false );
	settings->setResizable( true );
	settings->setFrameRate( 30.0f );
}

void RayMarcherApp::setup()
{
	mCamera = CameraPersp();
	mStartEyePoint = vec3( 15, 21, 27.5 ) * 0.65f;
	mCamera.lookAt( mStartEyePoint, vec3( 0 ), vec3( 0, 1, 0 ) );
	mCamUi = CameraUi( &mCamera, getWindow() );
	mCamUi.getSignalCameraChange().connect( [this] { mCurrentLine = 0; } );
	
	mGlsl = gl::GlslProg::create( gl::GlslProg::Format()
								 .vertex( CI_GLSL( 150,
												  uniform mat4 ciModelViewProjection;
												  uniform mat3 ciNormalMatrix;
												  in vec4 ciPosition;
												  in vec3 ciNormal;
												  
												  out vec3 vNormal;
												  
												  void main( void ) {
													  vNormal = ciNormalMatrix * ciNormal;
													  gl_Position = ciModelViewProjection * ciPosition;
												  }
												  ) )
								 .fragment( CI_GLSL( 150,
													uniform vec3 uLightDir;
													in vec3 vNormal;
													out vec3 oColor;
													void main( void ) {
														oColor = vec3( clamp( dot( vNormal, uLightDir ), 0.0, 1.0 ) );
													}
													) ) );
}

void RayMarcherApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 's' ) {
		mMarcher.randomScene();
		mCurrentLine = 0;
	}
}

void RayMarcherApp::resize()
{
	mImageSurface = std::shared_ptr<Surface8u>( new Surface8u( getWindowWidth(), getWindowHeight(), false ) );
	mImageTexture = gl::Texture::create( *mImageSurface );
	mCurrentLine = 0;
	
	mCamera.setPerspective( 45, getWindowWidth() / (float)getWindowHeight(), 0.1f, 100.0f );
}

void RayMarcherApp::update()
{
	if( mCurrentLine < getWindowHeight() ) {
		mMarcher.renderScanline( mCurrentLine, mImageSurface.get() );
		mImageTexture->update( *mImageSurface );
		mCurrentLine++;
	}
}

void RayMarcherApp::draw()
{
	gl::clear();
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::setMatrices( mCamera );
	
	mGlsl->bind();
	mGlsl->uniform( "uLightDir", vec3( gl::getViewMatrix() * vec4( RayMarcher::sLightDir, 0.0 ) ) );
	mMarcher.renderSceneGL();

	gl::setMatricesWindow( getWindowSize() );
	// draw as much of the texture as we've rendered
	gl::disableDepthRead();
	gl::disableDepthWrite();

	gl::getStockShader( gl::ShaderDef().texture() )->bind();
	gl::color( 1, 1, 1 );
	gl::ScopedTextureBind tex0( mImageTexture );
	gl::drawSolidRect( Rectf( 0, 0, getWindowWidth(), mCurrentLine ),
					   vec2( 0, 1 ),
					   vec2( 1, 1 - mCurrentLine / float(mImageTexture->getHeight()) ) );
}

CINDER_APP( RayMarcherApp, RendererGl )