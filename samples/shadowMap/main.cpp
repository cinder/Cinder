// derived from http://web.imrc.kist.re.kr/~jwkim/Glsl/
#include "Resources.h"

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Material.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Light.h"
#include "cinder/gl/DisplayList.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/ip/Hdr.h"

#include <iostream>
#include <math.h>

using namespace ci;
using namespace ci::app;

static const int SHADOW_MAP_RESOLUTION = 1024;
static const Color BLUE( 0.25f, 0.25f, 1.0f ), RED( 1.0f, 0.55f, 0.55f );


class ShadowMapSample : public AppBasic {
 public:
	void setup();
	void drawQuad( const float size );
	void keyDown( KeyEvent event );
	void resize( ResizeEvent event );
	void initShadowMap();
	void updateShadowMap();
	void draw();
	void update();

	gl::GlslProg		mShader;
	gl::Fbo				mDepthFbo;
	CameraPersp			*mCamera;
	bool				mPaused;
	gl::Light			*mLight;
	bool				mLookThroughCamera, mDrawDepthMap;
	gl::DisplayList		mTorus, mBackboard;
};

void ShadowMapSample::setup()
{
	glPolygonOffset( 1.0f, 1.0f );
	glEnable( GL_LIGHTING );
	glEnable( GL_DEPTH_TEST );

	mPaused = false;
	mLookThroughCamera = true;
	mDrawDepthMap = false;
	
	mCamera = new CameraPersp( getWindowWidth(), getWindowHeight(), 45.0f );
	mCamera->lookAt( Vec3f( 5, 5, 5 ), Vec3f( 0, 0, 0 ) );
	mCamera->setPerspective( 45.0f, getWindowAspectRatio(), 0.1f, 100.0f );
	
	mLight = new gl::Light( gl::Light::POINT, 0 );
	mLight->lookAt( Vec3f( 1, 5, 1 ), Vec3f( 0, 0, 0 ) );
	mLight->setAmbient( Color( 0.3f, 0.3f, 0.3f ) );
	mLight->setDiffuse( Color( 0.5f, 0.5f, 0.5f ) );
	mLight->setSpecular( Color( 0.5f, 0.5f, 0.5f ) );
	mLight->setShadowParams( 60.0f, 0.5f, 8.0f );
	mLight->update( *mCamera );
	mLight->enable();

	gl::Material torusMaterial;
	torusMaterial.setSpecular( BLUE );
	torusMaterial.setDiffuse( BLUE );
	torusMaterial.setAmbient( Color( 0.1f, 0.1f, 0.1f ) );
	torusMaterial.setShininess( 25.0f );

	gl::Material backboardMaterial;
	backboardMaterial.setAmbient( RED );
	backboardMaterial.setDiffuse( RED );	
	backboardMaterial.setShininess( 1.0f );

	initShadowMap();

	mTorus = gl::DisplayList( GL_COMPILE );
	mTorus.newList();
		gl::drawTorus( 1.0f, 0.3f, 32, 64 );
	mTorus.endList();
	mTorus.setMaterial( torusMaterial );
	
	mBackboard = gl::DisplayList( GL_COMPILE );
	mBackboard.newList();
		gl::drawCube( Vec3f( 0.0f, -2.5f, 0.0f ), Vec3f( 5.0f, 0.1f, 5.0f ) );
	mBackboard.endList();
	mBackboard.setMaterial( backboardMaterial );

	mShader = gl::GlslProg( loadResource( RES_SHADOWMAP_VERT ), loadResource( RES_SHADOWMAP_FRAG ) );
	mShader.bind();
	mShader.uniform( "depthTexture", 0 );
}

void ShadowMapSample::keyDown( ci::app::KeyEvent event )
{
	switch( event.getChar() ) {
		case ' ' :
			mPaused = ! mPaused;
		break;
		case 'r': { // create a random camera position
			Vec3f eyeVec = Rand::randVec3f().normalized() * 6.0f;
			if( eyeVec.y < 0 ) eyeVec.y = -eyeVec.y;
			mCamera->lookAt( eyeVec, Vec3f(0,-2.5,0) );
			mLight->update( *mCamera );
		}
		break;
		case 'f':
			setFullScreen( ! isFullScreen() );
		break;
		case 'l': { // create a random light position
			Vec3f lightPos = Rand::randVec3f().normalized() * 4.0f;
			if( lightPos.y < 0 ) lightPos.y = -lightPos.y;			
			mLight->lookAt( lightPos, Vec3f::zero() );
			mLight->update( *mCamera );
		}
		break;
		case 'c':
			mLookThroughCamera = ! mLookThroughCamera;
		break;
		case 'd':
			mDrawDepthMap = ! mDrawDepthMap;
		break;
	}
}

void ShadowMapSample::resize( ResizeEvent event )
{
	glViewport( 0, 0, event.getWidth(), event.getHeight() );
	mCamera->setPerspective( 45.0f, getWindowAspectRatio(), 0.1f, 100.0f );
}

void ShadowMapSample::drawQuad( const float size )
{
	const float texc = 1.0f;
	float data[][5] = {	{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }, { texc, 0.0f, size, 0.0f, 0.0f }, { texc, texc, size, size, 0.0f }, { 0.0f, texc, 0.0f, size, 0.0f } };
	glInterleavedArrays(GL_T2F_V3F, 0, data);
	glDrawArrays(GL_QUADS, 0, 4);
}

void ShadowMapSample::initShadowMap()
{
	mDepthFbo = gl::Fbo( SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, false, false, true );
	mDepthFbo.bindDepthTexture();
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );	
}

void ShadowMapSample::updateShadowMap()
{
	mDepthFbo.bindFramebuffer();

	glPolygonOffset( 1.0f, 1.0f );
	glEnable( GL_POLYGON_OFFSET_FILL );
	glClear( GL_DEPTH_BUFFER_BIT );

	glPushAttrib( GL_VIEWPORT_BIT );
	glViewport( 0, 0, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION );

	gl::pushMatrices();
	
		mLight->setShadowRenderMatrices();

		mBackboard.draw();
		mTorus.draw();
		gl::drawCube( Vec3f::zero(), Vec3f( 1, 1, 1 ) );
	gl::popMatrices();

	glPopAttrib();

	glDisable( GL_POLYGON_OFFSET_FILL );

	mDepthFbo.unbindFramebuffer();
}

void ShadowMapSample::update()
{
	if( ! mPaused )
		mTorus.getModelMatrix().rotate( Vec3f( 1.0f, 1.0f, 1.0f ).normalized(), 0.05f );
}

void ShadowMapSample::draw()
{
	gl::clear();
	gl::enableDepthWrite();
	glEnable( GL_LIGHTING );
	updateShadowMap();

	gl::enableDepthRead();

	glEnable( GL_TEXTURE_2D );
	mDepthFbo.bindDepthTexture();
	mShader.bind();
	mShader.uniform( "shadowTransMatrix", mLight->getShadowTransformationMatrix( *mCamera ) );
	
	if( mLookThroughCamera )
		gl::setMatrices( *mCamera );
	else
		gl::setMatrices( mLight->getShadowCamera() );
	mLight->update( *mCamera );

	glPushMatrix();
		mBackboard.draw();
		mTorus.draw();
		gl::drawCube( Vec3f::zero(), Vec3f( 1, 1, 1 ) );
	glPopMatrix();
	
	mShader.unbind();
	mDepthFbo.unbindTexture();
	
	// Draw the lighting frustum unless we're looking through it
	if( mLookThroughCamera ) {
		glDisable( GL_LIGHTING );
		glColor3f( 1.0f, 1.0f, 0.1f );
		gl::drawFrustum( mLight->getShadowCamera() );
	}
	
	if( mDrawDepthMap ) { // there are faster ways to achieve this, but this is a handy way to see the depth map
		gl::setMatricesWindow( getWindowSize() );
		Surface32f shadowMapSurface( mDepthFbo.getDepthTexture() );
		ip::hdrNormalize( &shadowMapSurface );
		gl::color( Color::white() );
		gl::draw( gl::Texture( shadowMapSurface ), Rectf( 0, 0, 128, 128 ) );
	}
}


CINDER_APP_BASIC( ShadowMapSample, RendererGl )