#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/Font.h"
#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;

class ExtrudeApp : public AppNative {
  public:	
	void	setup() override;
	void	keyDown( KeyEvent event ) override;
	void	resize() override;
	void	update() override;
	void	draw() override;
	
	void	makeGeom();
	
	CameraPersp				mCam;
	bool					mDrawNormals, mDrawWireframe, mCaps;
	gl::BatchRef			mBatch, mNormalsBatch;
	gl::GlslProgRef			mGlsl;
	Font					mFont;
	mat4					mRotation;
	char					mCurrentChar;
	float					mApproximation, mDepth;
	int						mSubdivisions;
	params::InterfaceGlRef	mParams;
};

void ExtrudeApp::setup()
{
	gl::enableDepthRead();
	gl::enableDepthWrite();

	mDrawNormals = false;
	mDrawWireframe = false;
	mCaps = true;
	mApproximation = 2.5f;
	mDepth = 2.2f;
	mSubdivisions = 1;
	mParams = params::InterfaceGl::create( getWindow(), "App parameters", toPixels( ivec2( 200, 400 ) ) );
	mParams->addParam( "Approximation", &mApproximation ).min( 0.1f ).max( 20.0f ).step( 0.1f ).updateFn( [=] { makeGeom(); } );
	mParams->addParam( "Depth", &mDepth ).min( 0.01f ).max( 7.0f ).step( 0.25f ).updateFn( [=] { makeGeom(); } );
	mParams->addParam( "Subdivisions", &mSubdivisions ).min( 1 ).max( 12 ).updateFn( [=] { makeGeom(); } );
	mParams->addParam( "Caps", &mCaps ).updateFn( [=] { makeGeom(); } );
	mParams->addParam( "Wireframe", &mDrawWireframe ).updateFn( [=] { makeGeom(); } );
	mParams->addParam( "Draw Normals", &mDrawNormals ).updateFn( [=] { makeGeom(); } );

	mCam.lookAt( vec3( 30, 20, 40 ), vec3( 0 ) );
	
	mGlsl = gl::GlslProg::create( loadAsset( "shader.vert" ), loadAsset( "shader.frag" ) );
	
	mFont = Font( "Arial", 64 );
	mCurrentChar = '{';
	makeGeom();
	
	mGlsl->bind();
}

void ExtrudeApp::keyDown( KeyEvent event )
{
	mCurrentChar = event.getChar();
	makeGeom();
}

void ExtrudeApp::makeGeom()
{
	// get the shape for this character
	Shape2d shape = mFont.getGlyphShape( mFont.getGlyphChar( mCurrentChar ) );
	auto bounds = shape.calcPreciseBoundingBox();
	shape.transform( MatrixAffine2f::makeTranslate( -bounds.getCenter() ) );
	
	// and extrude it
	auto extrudeSource = geom::Extrude( shape, mDepth, mApproximation ).caps( mCaps ).subdivisions( mSubdivisions );
	mBatch = gl::Batch::create( extrudeSource, mGlsl );
	mNormalsBatch = gl::Batch::create( geom::VertexNormalLines( extrudeSource, 3.0f ), gl::getStockShader( gl::ShaderDef().color() ) );
}

void ExtrudeApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	gl::setMatrices( mCam );
}

void ExtrudeApp::update()
{
	// Rotate the cube by 2 degrees around an arbitrary axis
	mRotation *= rotate( toRadians( 0.8f ), normalize( vec3( sin( getElapsedSeconds() * 0.5 ), 1, -0.3 ) ) );
}

void ExtrudeApp::draw()
{
	gl::clear();

	gl::setMatrices( mCam );
	gl::pushMatrices();
		gl::multModelMatrix( mRotation );
		gl::color( Color( 1, 1, 1 ) );
		if( mDrawWireframe )
			gl::enableWireframe();
		mBatch->draw();
		gl::disableWireframe();
		gl::color( Color( 1.0f, 0.5f, 0.25f ) );
		if( mDrawNormals && mNormalsBatch )
			mNormalsBatch->draw();
	gl::popMatrices();
	
	mParams->draw();
}

CINDER_APP_NATIVE( ExtrudeApp, RendererGl )