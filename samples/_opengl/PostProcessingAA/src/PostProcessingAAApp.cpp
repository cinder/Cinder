/*
Copyright (c) 2014, Paul Houx - All rights reserved.
This code is intended for use with the Cinder C++ library: http://libcinder.org

FXAA:
http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf
Copyright (c) 2010, 2011 NVIDIA CORPORATION. ALL RIGHTS RESERVED.

SMAA:
http://www.iryoku.com/smaa/
Copyright (c) 2013 Jorge Jimenez (jorge@iryoku.com)
Copyright (c) 2013 Jose I. Echevarria (joseignacioechevarria@gmail.com)
Copyright (c) 2013 Belen Masia (bmasia@unizar.es)
Copyright (c) 2013 Fernando Navarro (fernandn@microsoft.com)
Copyright (c) 2013 Diego Gutierrez (diegog@unizar.es)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that
the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and
the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/Timer.h"
#include "cinder/Utilities.h"
#include "cinder/Log.h"

#include "fxaa/FXAA.h"
#include "smaa/SMAA.h"
#include "Pistons.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PostProcessingAAApp : public App {
  public:
	enum SMAAMode { SMAA_EDGE_DETECTION, SMAA_BLEND_WEIGHTS, SMAA_BLEND_NEIGHBORS };
	enum DividerMode { MODE_COMPARISON, MODE_ORIGINAL1, MODE_FXAA, MODE_SMAA, MODE_ORIGINAL2, MODE_COUNT };
  public:
	static void prepare( Settings *settings );
	
	void setup() override;
	void update() override;
	void draw() override;

	void render();

	void resize() override;
	void mouseMove( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;

private:
	CameraPersp         mCamera;         // 3D camera to render our scene.
	Pistons             mPistons;        // Moving, colored pistons.

	Timer               mTimer;          // We can pause and resume our timer.
	double              mTime;
	double              mTimeOffset;

	double              mFrameTime;      // Used to calculate frame rate.
	double              mFrameRate;

	gl::FboRef          mFboScene;       // Non-anti-aliased frame buffer to render our scene to.
	gl::FboRef          mFboFinal;       // Frame buffer that will contain the anti-aliased result.

	FXAA                mFXAA;           // Takes care of applying FXAA anti-aliasing to our scene.
	SMAA                mSMAA;           // Takes care of applying SMAA anti-aliasing to our scene.
	SMAAMode            mSMAAMode;       // Allows us to see the 3 separate passes of the SMAA algorithm.

	gl::TextureRef      mInfoFXAA;       // Info texture.
	gl::TextureRef      mInfoSMAA;       // Info texture.
	gl::TextureRef      mInfoOriginal;   // Info texture.

	vec2                mDivider;        // Determines which part of our scene is anti-aliased.
	DividerMode         mDividerMode;    // Allows us to see each of the modes (Original, FXAA, SMAA) full screen.
	int                 mDividerWidth;
	int                 mPixelSize;      // Allows us to zoom in on the scene.

	vec2                mMouse;          // Keep track of the mouse cursor.
};

void PostProcessingAAApp::prepare( App::Settings *settings )
{
	settings->disableFrameRate();
	settings->setWindowSize( 1280, 720 );
}

void PostProcessingAAApp::setup()
{
	try {
		mInfoFXAA = gl::Texture::create( loadImage( loadAsset( "fxaa.png" ) ) );
		mInfoSMAA = gl::Texture::create( loadImage( loadAsset( "smaa.png" ) ) );
		mInfoOriginal = gl::Texture::create( loadImage( loadAsset( "original.png" ) ) );
	}
	catch( const std::exception& e ) {
		CI_LOG_EXCEPTION( "Failed to load textures.", e );
		quit();
	}

	mSMAAMode = SMAA_BLEND_NEIGHBORS;

	mTime = 0.0;
	mTimeOffset = 0.0;
	mTimer.start();

	mDivider = mMouse = getWindowSize() / 2;
	mDividerWidth = getWindowWidth() / 4;
	mDividerMode = MODE_COMPARISON;
	mPixelSize = 1;

	mFrameTime = getElapsedSeconds();
	mFrameRate = 0.0;

	// Disable vertical sync, so we can evaluate the frame rate.
	gl::enableVerticalSync( false );

	// Disable alpha blending (which is enabled by default) for SMAA to work correctly.
	gl::disableAlphaBlending();
}

void PostProcessingAAApp::update()
{
	// Keep track of time.
	mTime = mTimer.getSeconds() + mTimeOffset;

	// Show frame rate in window title.
	double elapsed = getElapsedSeconds() - mFrameTime;
	mFrameTime += elapsed;
	mFrameRate = 0.95 * mFrameRate + 0.05 * ( 1.0 / elapsed );
	getWindow()->setTitle( std::string( "PostProcessingAAApp - Frame rate: " ) + toString( int( mFrameRate ) ) );

	// Animate our camera.
	double t = mTime / 10.0;

	float phi = (float) t;
	float theta = 3.14159265f * ( 0.25f + 0.2f * math<float>::sin( phi * 0.9f ) );
	float x = 150.0f * math<float>::cos( phi ) * math<float>::cos( theta );
	float y = 150.0f * math<float>::sin( theta );
	float z = 150.0f * math<float>::sin( phi ) * math<float>::cos( theta );

	mCamera.lookAt( vec3( x, y, z ), vec3( 1, 50, 0 ) );
	mCamera.setFov( 40.0f );

	// Update the pistons.
	mPistons.update( mCamera );

	// Move the divider.
	mDivider += 0.25f * ( mMouse - mDivider );
}

void PostProcessingAAApp::draw()
{
	// Render our scene to an Fbo.
	render();

	// Clear the main buffer.
	gl::clear();
	gl::color( Color::white() );

	// Draw non-anti-aliased scene.
	if( mDividerMode == MODE_COMPARISON || mDividerMode == MODE_ORIGINAL1 || mDividerMode == MODE_ORIGINAL2 )
		gl::draw( mFboScene->getColorTexture(), getWindowBounds() );

	// Draw FXAA-anti-aliased scene.
	if( mDividerMode == MODE_COMPARISON || mDividerMode == MODE_FXAA ) {
		mFXAA.apply( mFboFinal, mFboScene );

		if( mDividerMode == MODE_COMPARISON ) {
			gl::pushMatrices();
			gl::setMatricesWindow( mDividerWidth, getWindowHeight() );
			gl::pushViewport( (int) mDivider.x - mDividerWidth, 0, mDividerWidth, getWindowHeight() );
			gl::draw( mFboFinal->getColorTexture(), getWindowBounds().getMoveULTo( vec2( -( mDivider.x - mDividerWidth ), 0 ) ) );
			gl::popViewport();
			gl::popMatrices();
		}
		else {
			gl::draw( mFboFinal->getColorTexture(), getWindowBounds() );
		}
	}

	// Draw SMAA-anti-aliased scene.
	if( mDividerMode == MODE_COMPARISON || mDividerMode == MODE_SMAA ) {
		mSMAA.apply( mFboFinal, mFboScene );

		gl::TextureRef tex;
		switch( mSMAAMode ) {
		case SMAA_EDGE_DETECTION: tex = mSMAA.getEdgePass(); break;
		case SMAA_BLEND_WEIGHTS: tex = mSMAA.getBlendPass(); break;
		case SMAA_BLEND_NEIGHBORS: tex = mFboFinal->getColorTexture(); break;
		}

		if( mDividerMode == MODE_COMPARISON ) {
			gl::pushMatrices();
			gl::setMatricesWindow( mDividerWidth, getWindowHeight() );
			gl::pushViewport( (int) mDivider.x, 0, mDividerWidth, getWindowHeight() );
			gl::draw( tex, getWindowBounds().getMoveULTo( vec2( -mDivider.x, 0 ) ) );
			gl::popViewport();
			gl::popMatrices();
		}
		else {
			gl::pushMatrices();
			gl::setMatricesWindow( getWindowWidth(), getWindowHeight() );
			gl::draw( tex, getWindowBounds() );
			gl::popMatrices();
		}
	}

	// Draw divider.
	if( mDividerMode == MODE_COMPARISON ) {
		gl::drawLine( vec2( (float) mDivider.x, 0 ), vec2( (float) mDivider.x, (float) getWindowHeight() ) );
		gl::drawLine( vec2( (float) ( mDivider.x - mDividerWidth ), 0 ), vec2( (float) ( mDivider.x - mDividerWidth ), (float) getWindowHeight() ) );
		gl::drawLine( vec2( (float) ( mDivider.x + mDividerWidth ), 0 ), vec2( (float) ( mDivider.x + mDividerWidth ), (float) getWindowHeight() ) );
	}

	// Draw info.
	gl::enableAlphaBlending();
	switch( mDividerMode ) {
	case MODE_COMPARISON:
		gl::draw( mInfoOriginal, vec2( mDivider.x - mDividerWidth * 3 / 2 - 128, 32 ) );
		gl::draw( mInfoFXAA, vec2( mDivider.x - mDividerWidth * 1 / 2 - 128, 32 ) );
		gl::draw( mInfoSMAA, vec2( mDivider.x + mDividerWidth * 1 / 2 - 128, 32 ) );
		gl::draw( mInfoOriginal, vec2( mDivider.x + mDividerWidth * 3 / 2 - 128, 32 ) );
		break;
	case MODE_ORIGINAL1:
	case MODE_ORIGINAL2:
		gl::draw( mInfoOriginal, vec2( getWindowWidth() / 2 - 128, 32 ) );
		break;
	case MODE_FXAA:
		gl::draw( mInfoFXAA, vec2( getWindowWidth() / 2 - 128, 32 ) );
		break;
	case MODE_SMAA:
		gl::draw( mInfoSMAA, vec2( getWindowWidth() / 2 - 128, 32 ) );
		break;
	default:
		break;
	}
	gl::disableAlphaBlending();
}

void PostProcessingAAApp::render()
{
	// Bind the Fbo. Automatically unbinds it at the end of this function.
	gl::ScopedFramebuffer fbo( mFboScene );

	// Clear the buffer.
	gl::clear( ColorA( 0, 0, 0, 0 ) );
	gl::color( Color::white() );

	// Render our scene.
	gl::pushViewport( 0, 0, mFboScene->getWidth(), mFboScene->getHeight() );
	mPistons.draw( mCamera, float( mTime ) );
	gl::popViewport();
}

void PostProcessingAAApp::resize()
{
	gl::Texture2d::Format tfmt;
	tfmt.setMinFilter( GL_NEAREST );
	tfmt.setMagFilter( GL_NEAREST );
	tfmt.setInternalFormat( GL_RGBA8 );

	gl::Fbo::Format fmt;
	fmt.setColorTextureFormat( tfmt );

	mFboScene = gl::Fbo::create( getWindowWidth() / mPixelSize, getWindowHeight() / mPixelSize, fmt );
	mFboFinal = gl::Fbo::create( getWindowWidth() / mPixelSize, getWindowHeight() / mPixelSize, fmt );

	// Update the camera's aspect ratio.
	mCamera.setAspectRatio( getWindowAspectRatio() );

	// Resize the divider.
	mDividerWidth = getWindowWidth() / 4;
}

void PostProcessingAAApp::mouseMove( MouseEvent event )
{
	mMouse = event.getPos();

	if( mMouse.x < 100 )
		mMouse.x = 0;
	else if( mMouse.x > getWindowWidth() - 100 )
		mMouse.x = (float) getWindowWidth();
}

void PostProcessingAAApp::mouseDrag( MouseEvent event )
{
	mMouse = event.getPos();

	if( mMouse.x < 50 )
		mMouse.x = 0;
	else if( mMouse.x > getWindowWidth() - 50 )
		mMouse.x = (float) getWindowWidth();
}

void PostProcessingAAApp::keyDown( KeyEvent event )
{
	switch( event.getCode() ) {
	case KeyEvent::KEY_ESCAPE:
		// Quit the application.
		quit();
		break;
	case KeyEvent::KEY_SPACE:
		// Start/stop the animation.
		if( mTimer.isStopped() ) {
			mTimeOffset += mTimer.getSeconds();
			mTimer.start();
		}
		else {
			mTimer.stop();
		}
		break;
	case KeyEvent::KEY_1:
		// While in SMAA mode, show the result of the edge detection shader pass.
		mSMAAMode = SMAAMode::SMAA_EDGE_DETECTION;
		break;
	case KeyEvent::KEY_2:
		// While in SMAA mode, show the result of the blend weights shader pass.
		mSMAAMode = SMAAMode::SMAA_BLEND_WEIGHTS;
		break;
	case KeyEvent::KEY_3:
		// While in SMAA mode, show the result of the blend neightbors shader pass.
		mSMAAMode = SMAAMode::SMAA_BLEND_NEIGHBORS;
		break;
	case KeyEvent::KEY_v:
		// Toggle vertical sync.
		if( gl::isVerticalSyncEnabled() )
			gl::enableVerticalSync( false );
		else
			gl::enableVerticalSync();
		break;
	case KeyEvent::KEY_LEFT:
	case KeyEvent::KEY_DOWN:
		// Switch to the next mode (comparison > original > FXAA > SMAA > original).
		if( (int) mDividerMode > 0 )
			mDividerMode = (DividerMode) ( (int) mDividerMode - 1 );
		else
			mDividerMode = (DividerMode) ( (int) DividerMode::MODE_COUNT - 1 );
		break;
	case KeyEvent::KEY_RIGHT:
	case KeyEvent::KEY_UP:
		// Switch to the previous mode (comparison < original < FXAA < SMAA < original).
		mDividerMode = (DividerMode) ( ( (int) mDividerMode + 1 ) % DividerMode::MODE_COUNT );
		break;
	case KeyEvent::KEY_PLUS:
	case KeyEvent::KEY_EQUALS:
	case KeyEvent::KEY_KP_PLUS:
		// Zoom in on the scene, so you can better see the anti-aliasing.
		if( mPixelSize < 8 ) {
			mPixelSize <<= 1; resize();
		}
		break;
	case KeyEvent::KEY_MINUS:
	case KeyEvent::KEY_UNDERSCORE:
	case KeyEvent::KEY_KP_MINUS:
		// Zoom out from the scene.
		if( mPixelSize > 1 ) {
			mPixelSize >>= 1; resize();
		}
		break;

	}
}

CINDER_APP( PostProcessingAAApp, RendererGl, &PostProcessingAAApp::prepare )
