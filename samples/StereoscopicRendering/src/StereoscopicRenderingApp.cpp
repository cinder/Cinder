/*
 Copyright (c) 2012-2104, Paul Houx
 All rights reserved.

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

/*
	This sample will show how to use the CameraStereo class to setup and render stereoscopic images.
	The camera contains different matrices for the left and right eye of the viewer. By rendering the scene
	twice, once for each eye, we can view the scene in 3D on monitors or televisions that support 3D.

	Here, we divide the window into a left and right half and render the scene to each half. This is called
	side-by-side stereoscopic and is supported by most 3D televisions. Simply connect your computer to
	such a television, run the sample in full screen and enable the TV's 3D mode.

	When creating your own stereoscopic application, be careful how you choose your convergence.
	An excellent article can be found here:
	http://paulbourke.net/miscellaneous/stereographics/stereorender/

	The CameraStereo class is based on the Off-Axis method described in this article.
	*/

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/VboMesh.h"

#include "cinder/Camera.h"
#include "cinder/Font.h"
#include "cinder/ImageIo.h"
#include "cinder/CameraUi.h"
#include "cinder/ObjLoader.h"
#include "cinder/Rand.h"
#include "cinder/TriMesh.h"
#include "cinder/Utilities.h"

#include "StereoAutoFocuser.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class StereoscopicRenderingApp : public App {
public:
	enum FocusMethod { SET_CONVERGENCE, SET_FOCUS, AUTO_FOCUS };
	enum RenderMethod { MONO, ANAGLYPH_RED_CYAN, SIDE_BY_SIDE, OVER_UNDER, INTERLACED_HORIZONTAL };

	typedef struct InstanceData {
		vec3 position;
		vec3 color;
	} InstanceData;
public:
	static void prepareSettings( Settings *settings );

	void setup() override;
	void update() override;
	void draw() override;

	void keyDown( KeyEvent event ) override;

	void resize() override;
private:
	void createFbo();
	void createGrid();

	void renderAnaglyph( const ivec2 &size, const ColorA &left, const ColorA &right );
	void renderSideBySide( const ivec2 &size );
	void renderOverUnder( const ivec2 &size );
	void renderInterlacedHorizontal( const ivec2 &size );

	void render();
	void renderUI();
private:
	bool					mDrawUI;
	bool					mDrawAutoFocus;

	FocusMethod				mFocusMethod;
	RenderMethod			mRenderMethod;

	CameraUi				mCamUi;
	CameraStereo			mCamera;

	StereoAutoFocuser		mAF;

	gl::GlslProgRef			mShaderPhong;
	gl::GlslProgRef			mShaderInstancedPhong;
	gl::GlslProgRef			mShaderAnaglyph;
	gl::GlslProgRef			mShaderInterlaced;

	gl::BatchRef			mBatchTrombone;
	gl::BatchRef			mBatchNote;
	gl::VertBatchRef		mGrid;

	int		kGridSize;
	int		kNumNotes;
	gl::VboRef				mInstanceDataVbo;

	gl::FboRef				mFbo;

	Color					mColorBackground;

	Font					mFont;
};

void StereoscopicRenderingApp::prepareSettings( Settings *settings )
{
	// Create a 16:9 window.
	settings->setWindowSize( 960, 540 );
	settings->setTitle( "Stereoscopic Rendering" );

	// Allow high frame rates to test performance.
	settings->disableFrameRate();
}

void StereoscopicRenderingApp::setup()
{
	kGridSize = 100;
	kNumNotes = 2 * kGridSize + 1;

	// Enable stereoscopic rendering by default.
	mRenderMethod = SIDE_BY_SIDE;

	// Enable auto-focussing by default.
	mFocusMethod = AUTO_FOCUS;
	mDrawAutoFocus = false;

	// Setup the camera. This is very similar to a standard perspective camera.
	mCamera.lookAt( vec3( 0.2f, 1.3f, -11.5f ), vec3( 0.5f, 1.5f, -0.1f ) );
	mCamera.setFov( 60.0f );

	mCamUi = CameraUi( &mCamera, getWindow() );

	try {
		// Load shaders.
		mShaderPhong = gl::GlslProg::create( loadAsset( "shaders/phong.vert" ), loadAsset( "shaders/phong.frag" ) );
		mShaderInstancedPhong = gl::GlslProg::create( loadAsset( "shaders/instanced_phong.vert" ), loadAsset( "shaders/phong.frag" ) );
		mShaderAnaglyph = gl::GlslProg::create( loadAsset( "shaders/anaglyph.vert" ), loadAsset( "shaders/anaglyph.frag" ) );
		mShaderInterlaced = gl::GlslProg::create( loadAsset( "shaders/interlaced.vert" ), loadAsset( "shaders/interlaced.frag" ) );

		// Load models.
		TriMeshRef triMesh = TriMesh::create();
		{
			// Trombone.
			triMesh->read( loadAsset( "models/trombone.msh" ) );
			mBatchTrombone = gl::Batch::create( *triMesh, mShaderPhong );
		}
		{
			// Music note.
			triMesh->read( loadAsset( "models/note.msh" ) );

			// Create instance data.
			std::vector<InstanceData> positions( kNumNotes );
			mInstanceDataVbo = gl::Vbo::create( GL_ARRAY_BUFFER, positions.size() * sizeof( InstanceData ), positions.data(), GL_DYNAMIC_DRAW );

			geom::BufferLayout instanceDataLayout;
			instanceDataLayout.append( geom::Attrib::CUSTOM_0, 3, sizeof( InstanceData ), offsetof( InstanceData, position ), 1 /* per instance*/ );
			instanceDataLayout.append( geom::Attrib::CUSTOM_1, 3, sizeof( InstanceData ), offsetof( InstanceData, color ), 1 /* per instance*/ );

			// Add our instance data buffer to the music note model.
			gl::VboMeshRef mesh = gl::VboMesh::create( *triMesh );
			mesh->appendVbo( instanceDataLayout, mInstanceDataVbo );

			gl::Batch::AttributeMapping mapping;
			mapping[geom::Attrib::CUSTOM_0] = "aInstancePosition";
			mapping[geom::Attrib::CUSTOM_1] = "aInstanceColor";

			mBatchNote = gl::Batch::create( mesh, mShaderInstancedPhong, mapping );
		}

		//
		createGrid();
	}
	catch( const std::exception &e ) {
		// Something went wrong, display error and quit.
		console() << e.what() << std::endl;
		quit();
	}

	mColorBackground = Color( 0.8f, 0.8f, 0.8f );

	mFont = Font( "Verdana", 24.0f );
	mDrawUI = true;
}

void StereoscopicRenderingApp::update()
{
	float	d, f;
	Area	area;

	// Update the positions of our notes.
	InstanceData* data = (InstanceData*) mInstanceDataVbo->map( GL_WRITE_ONLY );
	const float seconds = float( getElapsedSeconds() );
	for( int i = -kGridSize; i <= kGridSize; ++i ) {
		Rand rnd( i );

		float t = rnd.nextFloat() * 200.0f + 2.0f * seconds;
		float r = rnd.nextFloat() * 360.0f + 60.0f * seconds;
		float z = fmodf( 5.0f * t, 200.0f ) - 100.0f;
		Color c = Color( CM_HSV, rnd.nextFloat(), 1.0f, 1.0f );

		data->position = vec3( i * 0.5f, 0.15f + 1.0f * math<float>::abs( sinf( 3.0f * t ) ), -z );
		data->color = vec3( c.r, c.g, c.b );
		data++;
	}
	mInstanceDataVbo->unmap();

	// Perform auto-focus.
	switch( mFocusMethod ) {
	case SET_CONVERGENCE:
		// Auto-focus by calculating distance to center of interest.
		d = glm::distance( mCamera.getPivotPoint(), mCamera.getEyePoint() );
		f = math<float>::min( 5.0f, d * 0.5f );

		// The setConvergence() method will not change the eye separation distance, 
		// which may cause the parallax effect to become uncomfortably big. 
		mCamera.setConvergence( f );
		mCamera.setEyeSeparation( 0.05f );
		break;
	case SET_FOCUS:
		// Auto-focus by calculating distance to center of interest.
		d = glm::distance( mCamera.getPivotPoint(), mCamera.getEyePoint() );
		f = math<float>::min( 5.0f, d * 0.5f );

		// The setConvergence( value, true ) method will automatically calculate a fitting value for the eye separation distance.
		// There is still no guarantee that the parallax effect stays within comfortable levels,
		// because there may be objects very near to the camera compared to the point we are looking at.
		mCamera.setConvergence( f, true );
		break;
	case AUTO_FOCUS:
		// Here, we use the StereoAutoFocuser class to determine the best focal length,
		// based on the contents of the current depth buffer. This is by far the best method of
		// the three, because it guarantees the parallax effect will never be out of bounds.
		// Depending on the rendering method, we can sample different area's of the screen
		// to optimally detect details. This is not required, however.
		// Use the UP and DOWN keys to adjust the intensity of the parallax effect.
		switch( mRenderMethod ) {
			case MONO:
				break;
			case SIDE_BY_SIDE:
				// Sample half the left eye, half the right eye.
				area = Area( gl::getViewport().first, gl::getViewport().first + gl::getViewport().second );
				area.expand( -area.getWidth() / 4, 0 );
				mAF.autoFocus( &mCamera, area );
				break;
			case OVER_UNDER:
				// Sample half the left eye, half the right eye.
				area = Area( gl::getViewport().first, gl::getViewport().first + gl::getViewport().second );
				area.expand( 0, -area.getHeight() / 4 );
				mAF.autoFocus( &mCamera, area );
				break;
			case ANAGLYPH_RED_CYAN:
				// Sample the depth buffer of one of the FBO's.
				mAF.autoFocus( &mCamera, mFbo );
				break;
		}
		break;
	}
}

void StereoscopicRenderingApp::draw()
{
	// Clear color and depth buffers.
	gl::viewport( 0, 0, getWindowWidth(), getWindowHeight() );
	gl::clear( mColorBackground );

	// Stereoscopic rendering.
	switch( mRenderMethod ) {
	case MONO:
		// Render mono camera.
		mCamera.disableStereo();
		render();
		break;
	case ANAGLYPH_RED_CYAN:
		renderAnaglyph( getWindowSize(), Color( 1, 0, 0 ), Color( 0, 1, 1 ) );
		break;
	case SIDE_BY_SIDE:
		renderSideBySide( getWindowSize() );
		break;
	case OVER_UNDER:
		renderOverUnder( getWindowSize() );
		break;
	case INTERLACED_HORIZONTAL:
		renderInterlacedHorizontal( getWindowSize() );
		break;
	}

	// draw auto focus visualizer
	if( mDrawAutoFocus ) mAF.draw();
}

void StereoscopicRenderingApp::keyDown( KeyEvent event )
{
	switch( event.getCode() ) {
	case KeyEvent::KEY_ESCAPE:
		quit();
		break;
	case KeyEvent::KEY_f:
		// toggle full screen.
		setFullScreen( !isFullScreen() );
		break;
	case KeyEvent::KEY_v:
		// toggle vertical sync.
		gl::enableVerticalSync( !gl::isVerticalSyncEnabled() );
		break;
	case KeyEvent::KEY_d:
		// toggle visualizer.
		mDrawAutoFocus = !mDrawAutoFocus;
		break;
	case KeyEvent::KEY_u:
		// toggle interface.
		mDrawUI = !mDrawUI;
		break;
	case KeyEvent::KEY_UP:
		// increase the parallax effect (towards negative parallax).
		if( mFocusMethod == AUTO_FOCUS )
			mAF.setDepth( mAF.getDepth() + 0.01f );
		break;
	case KeyEvent::KEY_DOWN:
		// decrease the parallax effect (towards positive parallax).
		if( mFocusMethod == AUTO_FOCUS )
			mAF.setDepth( mAF.getDepth() - 0.01f );
		break;
	case KeyEvent::KEY_SPACE:
		// reset the parallax effect to 'no parallax for the nearest object'.
		mAF.setDepth( 1.0f );
		break;
	case KeyEvent::KEY_LEFT:
		// reduce the auto focus speed.
		mAF.setSpeed( mAF.getSpeed() - 0.01f );
		break;
	case KeyEvent::KEY_RIGHT:
		// increase the auto focus speed.
		mAF.setSpeed( mAF.getSpeed() + 0.01f );
		break;
	case KeyEvent::KEY_1:
		mFocusMethod = SET_CONVERGENCE;
		break;
	case KeyEvent::KEY_2:
		mFocusMethod = SET_FOCUS;
		break;
	case KeyEvent::KEY_3:
		mFocusMethod = AUTO_FOCUS;
		break;
	case KeyEvent::KEY_F1:
		mRenderMethod = MONO;
		createFbo();
		break;
	case KeyEvent::KEY_F2:
		mRenderMethod = ANAGLYPH_RED_CYAN;
		createFbo();
		break;
	case KeyEvent::KEY_F3:
		mRenderMethod = SIDE_BY_SIDE;
		createFbo();
		break;
	case KeyEvent::KEY_F4:
		mRenderMethod = OVER_UNDER;
		createFbo();
		break;
	case KeyEvent::KEY_F5:
		mRenderMethod = INTERLACED_HORIZONTAL;
		createFbo();
		break;
	}
}

void StereoscopicRenderingApp::resize()
{
	createFbo();
}

void StereoscopicRenderingApp::createFbo()
{
	ivec2 size = getWindowSize();

	// By doubling the horizontal resolution, we can effectively render
	// both the left and right eye views side by side at full resolution.
	if( mRenderMethod == ANAGLYPH_RED_CYAN || mRenderMethod == INTERLACED_HORIZONTAL )
		size.x *= 2;

	// Do we really need to create the fbo?
	if( mFbo && mFbo->getWidth() == size.x && mFbo->getHeight() == size.y )
		return;

	// Create fbo.
	gl::Texture::Format tfmt;
	tfmt.setMagFilter( GL_LINEAR );

	gl::Fbo::Format fmt;
	fmt.setColorTextureFormat( tfmt );
	fmt.setSamples( 8 );

	mFbo = gl::Fbo::create( size.x, size.y, fmt );
}

void StereoscopicRenderingApp::createGrid()
{
	const Color color( 0.8f, 0.8f, 0.8f );

	mGrid = gl::VertBatch::create( GL_LINES );

	mGrid->begin( GL_LINES );
	for( int i = -kGridSize; i <= kGridSize; ++i ) {
		mGrid->vertex( vec4( (float) i, 0, -kGridSize, 1 ), color );
		mGrid->vertex( vec4( (float) i, 0, kGridSize, 1 ), color );
		mGrid->vertex( vec4( -kGridSize, 0, (float) i, 1 ), color );
		mGrid->vertex( vec4( kGridSize, 0, (float) i, 1 ), color );
	}
	mGrid->end();
}

void StereoscopicRenderingApp::renderAnaglyph( const ivec2 &size, const ColorA &left, const ColorA &right )
{
	{
		// bind the FBO
		gl::ScopedFramebuffer fbo( mFbo );

		// render the scene using the side-by-side technique
		renderSideBySide( mFbo->getSize() );
	}

	// enable the anaglyph shader
	gl::ScopedGlslProg shader( mShaderAnaglyph );
	mShaderAnaglyph->uniform( "uTex0", 0 );
	mShaderAnaglyph->uniform( "uColorLeft", left );
	mShaderAnaglyph->uniform( "uColorRight", right );

	// bind the FBO texture and draw a full screen rectangle,
	// which conveniently is exactly what the following lines do
	gl::ScopedTextureBind texture( mFbo->getColorTexture() );
	gl::drawSolidRect( Rectf( 0, 0, float( size.x ), float( size.y ) ) );
}

void StereoscopicRenderingApp::renderSideBySide( const ivec2 &size )
{
	// draw to left half of window only
	gl::ScopedViewport viewport( 0, 0, size.x / 2, size.y );
	gl::clear( mColorBackground );

	// render left camera
	mCamera.enableStereoLeft();
	render();

	// draw to right half of window only
	gl::viewport( size.x / 2, 0, size.x / 2, size.y );

	// render right camera
	mCamera.enableStereoRight();
	render();
}

void StereoscopicRenderingApp::renderOverUnder( const ivec2 &size )
{
	// draw to top half of window only
	gl::ScopedViewport viewport( 0, size.y / 2, size.x, size.y / 2 );
	gl::clear( mColorBackground );

	// render left camera
	mCamera.enableStereoLeft();
	render();

	// draw to bottom half of window only
	gl::viewport( 0, 0, size.x, size.y / 2 );

	// render right camera
	mCamera.enableStereoRight();
	render();
}

void StereoscopicRenderingApp::renderInterlacedHorizontal( const ivec2 &size )
{
	// bind the FBO and clear its buffer
	mFbo->bindFramebuffer();

	// render the scene using the over-under technique
	renderSideBySide( mFbo->getSize() );

	// unbind the FBO
	mFbo->unbindFramebuffer();

	// enable the interlace shader
	gl::ScopedGlslProg shader( mShaderInterlaced );
	mShaderInterlaced->uniform( "uTex0", 0 );
	mShaderInterlaced->uniform( "uWindowOrigin", vec2( getWindowPos() ) );
	mShaderInterlaced->uniform( "uWindowSize", vec2( getWindowSize() ) );

	// bind the FBO texture and draw a full screen rectangle,
	// which conveniently is exactly what the following lines do
	mFbo->bindTexture( 0 );
	gl::drawSolidRect( Rectf( 0, 0, float( size.x ), float( size.y ) ) );
	mFbo->unbindTexture();
}

void StereoscopicRenderingApp::render()
{
	float seconds = (float) getElapsedSeconds();

	// enable 3D rendering
	gl::enableDepthRead();
	gl::enableDepthWrite();

	// set 3D camera matrices
	gl::pushMatrices();
	gl::setMatrices( mCamera );

	if( mShaderPhong && mBatchTrombone && mBatchNote ) {
		// enable phong shading
		gl::ScopedGlslProg shader( mShaderPhong );

		// draw trombone
		gl::pushModelView();
		{
			gl::color( 0.7f, 0.6f, 0.0f );
			gl::rotate( glm::radians( 10.0f * seconds ), vec3( 0, 1, 0 ) );
			mBatchTrombone->draw();

			// reflection
			gl::scale( 1.0f, -1.0f, 1.0f );
			mBatchTrombone->draw();
		}
		gl::popModelView();

		// draw animated notes
		mBatchNote->drawInstanced( kNumNotes );

		// reflected notes
		gl::pushModelView();
		gl::scale( 1.0f, -1.0f, 1.0f );
		mBatchNote->drawInstanced( kNumNotes );
		gl::popModelView();
	}

	// draw grid
	mGrid->draw();

	// draw floor
	gl::enableAlphaBlending();
	gl::color( ColorA( 1, 1, 1, 0.75f ) );
	gl::drawCube( vec3( 0.0f, -0.5f, 0.0f ), vec3( 2.0f * kGridSize, 1.0f, 2.0f * kGridSize ) );
	gl::disableAlphaBlending();

	// restore 2D rendering
	gl::popMatrices();
	gl::disableDepthWrite();
	gl::disableDepthRead();

	// render UI
	if( mDrawUI ) renderUI();
}

void StereoscopicRenderingApp::renderUI()
{
	float w = (float) getWindowWidth() * 0.5f;
	float h = (float) getWindowHeight();

	std::string renderMode, focusMode;
	switch( mRenderMethod ) {
	case MONO: renderMode = "Mono"; break;
	case SIDE_BY_SIDE: renderMode = "Side By Side"; break;
	case OVER_UNDER: renderMode = "Over Under"; break;
	case ANAGLYPH_RED_CYAN: renderMode = "Anaglyph Red Cyan"; break;
	case INTERLACED_HORIZONTAL: renderMode = "Interlaced Horizontal"; break;
	}
	switch( mFocusMethod ) {
	case SET_CONVERGENCE: focusMode = "setConvergence(d, false)"; break;
	case SET_FOCUS: focusMode = "setConvergence(d, true)"; break;
	case AUTO_FOCUS: focusMode = "autoFocus(cam)"; break;
	}

	std::string labels( "Render mode (F1-F5):\nFocus mode (1-3):\nFocal Length:\nEye Distance:\nAuto Focus Depth (Up/Down):\nAuto Focus Speed (Left/Right):" );
	//boost::format values = boost::format( "%s\n%s\n%.2f\n%.2f\n%.2f\n%.2f" ) % renderMode % focusMode % mCamera.getConvergence() % mCamera.getEyeSeparation() % mAF.getDepth() % mAF.getSpeed();
	char valueStr[1024];
	sprintf( valueStr, "%s\n%s\n%.2f\n%.2f\n%.2f\n%.2f", renderMode.c_str(), focusMode.c_str(), mCamera.getConvergence(), mCamera.getEyeSeparation() , mAF.getDepth(), mAF.getSpeed() );

#if(defined CINDER_MSW)
	gl::enableAlphaBlending();
	gl::drawString( labels, vec2( w - 350.0f, h - 150.0f ), Color::black(), mFont );
	gl::drawStringRight( std::string( valueStr ), vec2( w + 350.0f, h - 150.0f ), Color::black(), mFont );
	gl::disableAlphaBlending();
#else
	// \n is not supported on the mac, so we draw separate strings
	std::vector< std::string > left, right;
	left = ci::split( labels, "\n", false );
	right = ci::split( valueStr, "\n", false );

	gl::enableAlphaBlending();
	for( size_t i = 0; i < 4; ++i ) {
		gl::drawString( left[i], vec2( w - 350.0f, h - 150.0f + i * mFont.getSize() * 0.9f ), Color::black(), mFont );
		gl::drawStringRight( right[i], vec2( w + 350.0f, h - 150.0f + i * mFont.getSize() * 0.9f ), Color::black(), mFont );
	}
	gl::disableAlphaBlending();
#endif
}

CINDER_APP( StereoscopicRenderingApp, RendererGl( RendererGl::Options().msaa( 16 ) ), StereoscopicRenderingApp::prepareSettings )
