#include "cinder/CameraUi.h"
#include "cinder/Json.h"
#include "cinder/Log.h"
#include "cinder/Rand.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

using namespace ci;
using namespace app;
using namespace std;

/* This sample shows the effect called "pixel fighting" (or z-fighting), where thin or co-planar objects
 * can't be rendered correctly due to insufficient precision of the depth buffer.
 *
 * Here, we create a large field of colored tiles, each of the six faces a different color. With sufficient
 * depth buffer precision, we should only see the top of the tiles, which has a green color.
 *
 * However, when depth buffer precision is insufficient, parts of the bottom and sides will also appear,
 * which looks like a stipple or noise pattern.
 *
 * Insufficient depth buffer precision is often caused by a very small near clip distance of the camera,
 * relative to the far clip distance. 16-bit or 24-bit depth buffers have even lower precision.
 *
 * You'd think that using a floating point depth buffer would solve things, but this is not the case.
 * The mantissa of a 32-bit float is still only 23 bits wide and the exponent is almost completely used up
 * very close to the near clip distance, leaving little precision for the vast majority of the scene.
 *
 * Floating point values have a lot of precision around zero, so it would be great if we could somehow
 * setup OpenGL to use a value of 1 for the near distance and 0 for the far distance. Since the introduction
 * of the GL_ARB_clip_control extension, we can. This has become part of OpenGL from version 4.5 onward.
 *
 * Using this sample, you can see the effects. A small near clip distance causes pixel fighting, but when
 * we reverse the depth values for the near and far clip planes, pixel fighting is virtually gone.
 *
 * Because this requires a floating point depth buffer, we render the scene to an Fbo. The main buffer can
 * not use a floating point depth buffer. Once rendered, we blit the scene to the main buffer.
 *
 * For the best results, this is what we need to do:
 *  1) Use a floating point depth buffer (GL_DEPTH_COMPONENT32F).
 *  2) Use `glClipControl` to change the default depth values from GL_NEGATIVE_ONE_TO_ONE to GL_ZERO_TO_ONE.
 *  3) Adjust our projection matrix to output values from 1 (near) to 0 (far), instead of -1 (near) to 1 (far).
 *  4) Clear the depth buffer with a value of 0 instead of 1.
 *  5) Set the depth comparison to GL_GREATER instead of GL_LESS.
 *  6) (Optional) Use an infinite far clip distance to reduce rounding errors during the matrix calculations.
 *  7) (Optional) Don't concatenate the view and projection matrices on the CPU, again to reduce rounding errors during the matrix calculations.
 *
 * Luckily, Cinder can do steps 1 through 5 for you by calling `gl::enableDepthReversed()` at the start of your application.
 * Step 6 and 7 can easily be done using the `CameraPersp` class and through a simple shader trick, also shown in this sample.
 */

class FightingPixelFightingApp : public App {
  public:
	static void prepare( Settings *settings ) { settings->setWindowSize( 1024, 768 ); }

	void setup() override;
	void draw() override;
	void resize() override;

	void initializeGui();

  private:
	params::InterfaceGlRef mParams;
	CameraPersp			   mCam;
	CameraUi			   mCamUi;
	gl::FboRef			   mFbo;
	gl::VboRef			   mInstances;
	gl::VboMeshRef		   mMesh;
	gl::GlslProgRef		   mGlsl;
	gl::BatchRef		   mBatch;
	GLint				   mDepthBufferFormat = GL_DEPTH_COMPONENT32F;
	int					   mPreset = 0;
	bool				   mUseConcatenatedProjection = true;

	static const int kNumPerSide = 64;
	static const int kNumInstances = kNumPerSide * kNumPerSide;
};

void FightingPixelFightingApp::setup()
{
	// Setup the GUI.
	initializeGui();

	// Setup our camera to use a very small value for the near clip distance. This will cause pixel fighting.
	mCam.setPerspective( 30.0f, getWindowAspectRatio(), 0.0001f, 100.0f );
	// Position the camera just above our floor tiles to increase the pixel fighting effect.
	mCam.lookAt( vec3( kNumPerSide / 2, 1, kNumPerSide / 2 ), vec3( -kNumPerSide / 2, 0, -kNumPerSide / 2 ), vec3( 0, 1, 0 ) );

	// Allow us to move the camera.
	mCamUi.setCamera( &mCam );
	mCamUi.connect( getWindow() );

	// Create a list of positions for the floor tiles.
	vector<vec4> positions;
	positions.reserve( kNumInstances );

	for( int x = 0; x < kNumPerSide; ++x )
		for( int z = 0; z < kNumPerSide; ++z )
			positions.emplace_back( x - kNumPerSide / 2, 0, z - kNumPerSide / 2, 1 );

	// Store the positions on the GPU in a vertex buffer object.
	mInstances = gl::Vbo::create( GL_ARRAY_BUFFER, kNumInstances * sizeof( vec4 ), positions.data(), GL_STATIC_DRAW );

	// Create a colored cube mesh and tell it to use the position buffer.
	geom::BufferLayout layout;
	layout.append( geom::CUSTOM_0, 4, 0, 0, 1 );

	mMesh = gl::VboMesh::create( geom::Cube().size( 1, 0.01f, 1 ).colors() );
	mMesh->appendVbo( layout, mInstances );

	// Load the shader and create a batch.
	try {
		mGlsl = gl::GlslProg::create( loadAsset( "objects.vert" ), loadAsset( "objects.frag" ) );
		mBatch = gl::Batch::create( mMesh, mGlsl, { { geom::CUSTOM_0, "iPosition" } } );
	}
	catch( const std::exception &exc ) {
		CI_LOG_E( exc.what() );
	}
}

void FightingPixelFightingApp::draw()
{
	// Render the floor tiles to our Fbo.
	if( mFbo ) {
		gl::ScopedMatrices	scpMatrices;
		gl::ScopedFramebuffer scpFbo( mFbo );

		// After a call to `gl::enableDepthReversed()`, Cinder will automatically use
		// the correct method of clearing the buffer and setting up the camera.
		gl::clear();
		gl::setMatrices( mCam );

		// After a call to `gl::enableDepthReversed()`, Cinder will automatically use
		// the correct depth comparison.
		gl::ScopedDepth scpDepth( true );

		// Render the floor tiles.
		if( mBatch ) {
			mBatch->getGlslProg()->uniform( "uUseConcatenatedProjection", mUseConcatenatedProjection );
			mBatch->drawInstanced( kNumInstances );
		}
	}

	// Render the final result to the main buffer.
	if( mFbo )
		gl::draw( mFbo->getColorTexture() );

	// Draw the GUI.
	if( mParams )
		mParams->draw();
}

void FightingPixelFightingApp::resize()
{
	// Create an Fbo the size of our window, with the specified depth buffer format.
	const auto width = getWindowWidth();
	const auto height = getWindowHeight();
	const auto fmt = gl::Fbo::Format().depthBuffer( mDepthBufferFormat );
	mFbo = gl::Fbo::create( width, height, fmt );
}

void FightingPixelFightingApp::initializeGui()
{
	static const std::vector<std::string> depthBufferFormatLabels{ "16 Bits Integer", "24 Bits Integer", "32 Bits Integer", "32 Bits Float" };
	static const std::vector<std::string> clipControlLabels{ "-1 to 1", "0 to 1" };
	static const std::vector<std::string> presetLabels{ "Cinder Default", "Reversed Depth", "Custom" };

	mParams = params::InterfaceGl::create( "GUI", vec2( 340, 600 ) );
	mParams->setOptions("", "valueswidth=160" );
	mParams->addText( "This sample shows a technique" );
	mParams->addText( "called \"Reversed Depth\", which can" );
	mParams->addText( "be used to get rid of so-called" );
	mParams->addText( "\"Pixel Fighting\". See the source code" );
	mParams->addText( "for more information." );
	mParams->addParam<bool>( "Enable Reversed Depth",
		[this]( bool v ) {
			gl::enableDepthReversed( v );
			mPreset = 2;
		},
		[this]() { return gl::isDepthReversedEnabled(); } );
	mParams->addSeparator();
	mParams->addText( "Concatenating the view and projection" );
	mParams->addText( "matrices on the CPU can introduce" );
	mParams->addText( "rounding errors, increasing the risk" );
	mParams->addText( "of causing render artifacts." );
	mParams->addParam<bool>( "Concatenate On CPU",
		[this]( bool v ) {
			mUseConcatenatedProjection = v;
			mPreset = 2;
		},
		[this]() { return mUseConcatenatedProjection; } );
	mParams->addSeparator();
	mParams->addText( "Reversed Depth only works in combination" );
	mParams->addText( "with a floating point depth buffer," );
	mParams->addText( "but you can try other formats here as well." );
	mParams->addParam( "Depth Buffer Format", depthBufferFormatLabels,
		[this]( int v ) {
			switch( v ) {
			case 0:
				mDepthBufferFormat = GL_DEPTH_COMPONENT16;
				break;
			case 1:
				mDepthBufferFormat = GL_DEPTH_COMPONENT24;
				break;
			case 2:
				mDepthBufferFormat = GL_DEPTH_COMPONENT32;
				break;
			case 3:
				mDepthBufferFormat = GL_DEPTH_COMPONENT32F;
				break;
			}
			resize();
			mPreset = 2;
		},
		[this]() { return mDepthBufferFormat == GL_DEPTH_COMPONENT16 ? 0 : mDepthBufferFormat == GL_DEPTH_COMPONENT24 ? 1 : mDepthBufferFormat == GL_DEPTH_COMPONENT32 ? 2 : 3; } );
	mParams->addSeparator();
	mParams->addText( "Setting the near clip distance to very small values" );
	mParams->addText( "will greatly reduce depth buffer precision." );
	mParams->addText( "Try different values to see for yourself." );
	mParams->addParam<float>( "Near Clip",
		[this]( float v ) {
			mCam.setNearClip( v );
			mPreset = 2;
		},
		[this]() { return mCam.getNearClip(); } );
	mParams->addText( "The far clip distance plays almost no role" );
	mParams->addText( "when it comes to pixel fighting." );
	mParams->addParam<float>( "Far Clip",
		[this]( float v ) {
			mCam.setFarClip( v );
			mPreset = 2;
		},
		[this]() { return mCam.getFarClip(); } );
	mParams->addText( "Using an infinite far clip distance leads to" );
	mParams->addText( "fewer rounding errors, although the effect" );
	mParams->addText( "is minimal." );
	mParams->addParam<bool>( "Infinite Far Clip",
		[this]( bool v ) {
			mCam.setInfiniteFarClip( v );
			mPreset = 2;
		},
		[this]() { return mCam.isInfiniteFarClip(); } );
	mParams->addSeparator();
	mParams->addText( "Reversed Depth only works if our depth buffer" );
	mParams->addText( "is setup to use values ranging from 0 to 1." );
	mParams->addText( "By default, OpenGL uses -1 to 1." );
	mParams->addParam( "Clip Control", clipControlLabels, [this]( int v ) { gl::clipControl( GL_LOWER_LEFT, v == 0 ? GL_NEGATIVE_ONE_TO_ONE : GL_ZERO_TO_ONE ); }, //
		[this]() { return ( gl::getClipControlDepthMode() == GL_ZERO_TO_ONE ? 1 : 0 ); } );
	mParams->addSeparator();
	mParams->addParam( "Preset", presetLabels,
		[this]( int v ) {
			switch( v ) {
			case 0:
				mUseConcatenatedProjection = true;
				mDepthBufferFormat = GL_DEPTH_COMPONENT24;
				mCam.setInfiniteFarClip( false );
				gl::disableDepthReversed();
				resize();
				break;
			case 1:
				mUseConcatenatedProjection = false;
				mDepthBufferFormat = GL_DEPTH_COMPONENT32F;
				mCam.setInfiniteFarClip( true );
				gl::enableDepthReversed();
				resize();
				break;
			default:
				break;
			}

			mPreset = v;
		}, //
		[this]() { return mPreset; } );
}

CINDER_APP( FightingPixelFightingApp, RendererGl, &FightingPixelFightingApp::prepare )
