/*
Copyright (c) 2014, Paul Houx - All rights reserved.
This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Camera.h"
#include "cinder/Timer.h"

#include "FXAA.h"
#include "Pistons.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PostProcessingAAApp : public AppNative {
public:
	void prepareSettings( Settings* settings ) override;

	void setup() override;
	void update() override;
	void draw() override;

	void render();

	void resize() override;
	void mouseDown( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;


private:
	CameraPersp         mCamera;
	Pistons             mPistons;

	gl::FboRef          mFbo;

	FXAA                mFXAA;

	Timer               mTimer;
	double              mTime;
	double              mTimeOffset;
};

void PostProcessingAAApp::prepareSettings( Settings* settings )
{
	settings->disableFrameRate();
	settings->setWindowSize( 1280, 720 );
}

void PostProcessingAAApp::setup()
{
	mPistons.setup();
	mFXAA.setup();

	mTimeOffset = 0.0;
	mTimer.start();
}

void PostProcessingAAApp::update()
{
	// Keep track of time.
	mTime = mTimer.getSeconds() + mTimeOffset;

	// Animate our camera.
	double t = mTime / 10.0;

	float phi = (float) t;
	float theta = 3.14159265f * ( 0.25f + 0.2f * math<float>::sin( phi * 0.9f ) );
	float x = 150.0f * math<float>::cos( phi ) * math<float>::cos( theta );
	float y = 150.0f * math<float>::sin( theta );
	float z = 150.0f * math<float>::sin( phi ) * math<float>::cos( theta );

	mCamera.setEyePoint( Vec3f( x, y, z ) );
	mCamera.setCenterOfInterestPoint( Vec3f( 1, 50, 0 ) );
	mCamera.setAspectRatio( getWindowAspectRatio() );
	mCamera.setFov( 40.0f );

	// Update the pistons.
	mPistons.update( mCamera );
}

void PostProcessingAAApp::draw()
{
	// Render our scene to an Fbo.
	render();

	// Clear the main buffer.
	gl::clear();
	gl::color( Color::white() );

	// Bind our scene as a texture and render.
	gl::TextureRef texture = mFbo->getColorTexture();
	gl::ScopedGlslProg glslProg( gl::context()->getStockShader( gl::ShaderDef().texture( texture ) ) );
	gl::ScopedTextureBind bind( texture );

	mFXAA.apply( mFbo );
	/*
	Area flipped = texture->getBounds();
	flipped.y1 = flipped.y2;
	flipped.y2 = 0;
	gl::drawSolidRect( flipped );
	*/
}

void PostProcessingAAApp::render()
{
	// Bind the Fbo. Automatically unbinds it at the end of this function.
	gl::ScopedFramebuffer fbo( mFbo );

	// Clear the buffer.
	gl::clear();
	gl::color( Color::white() );
	
	// Render our scene.
	mPistons.draw( mCamera, float( mTime ) );
}

void PostProcessingAAApp::resize()
{
	// For this sample, we want a non-multisampled buffer and bilinear interpolation.
	gl::Texture2d::Format tfmt;
	tfmt.setMinFilter( GL_LINEAR );
	tfmt.setMagFilter( GL_LINEAR );

	gl::Fbo::Format fmt;
	fmt.setColorTextureFormat( tfmt );

	mFbo = gl::Fbo::create( getWindowWidth(), getWindowHeight(), fmt );
}

void PostProcessingAAApp::mouseDown( MouseEvent event )
{
}

void PostProcessingAAApp::keyDown( KeyEvent event )
{
	switch( event.getCode() ) {
	case KeyEvent::KEY_ESCAPE:
		quit();
		break;
	case KeyEvent::KEY_SPACE:
		// Start/stop the animation
		if( mTimer.isStopped() ) {
			mTimeOffset += mTimer.getSeconds();
			mTimer.start();
		}
		else
			mTimer.stop();
		break;
		/*case KeyEvent::KEY_1:
			mMode = Mode::EDGE_DETECTION;
			break;
			case KeyEvent::KEY_2:
			mMode = Mode::BLEND_WEIGHTS;
			break;
			case KeyEvent::KEY_3:
			mMode = Mode::BLEND_NEIGHBORS;
			break;*/
	case KeyEvent::KEY_v:
		if( gl::isVerticalSyncEnabled() )
			gl::enableVerticalSync( false );
		else
			gl::enableVerticalSync();
		break;
	}
}

CINDER_APP_NATIVE( PostProcessingAAApp, RendererGl )
