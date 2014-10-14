#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;

class SuperformulaGpuApp : public AppNative {
  public:	
	void	setup() override;
	void	resize() override;
	void	update() override;
	void	draw() override;

	void	setupGeometry();
	
	CameraPersp				mCam;
	gl::BatchRef			mBatch, mNormalsBatch;
	gl::GlslProgRef			mGlsl, mNormalsGlsl;
	mat4					mRotation;
	params::InterfaceGlRef	mParams;

	bool					mDrawNormals;
	float					mNormalsLength;
	int						mSubdivisions;
	
	float			mA1, mA2;
	float			mB1, mB2;
	float			mM1, mM2;
	float			mN11, mN12;
	float			mN21, mN22;
	float			mN31, mN32;
};

void SuperformulaGpuApp::setupGeometry()
{
	auto plane = geom::Plane().subdivisions( ivec2( mSubdivisions, mSubdivisions ) );
	mBatch = gl::Batch::create( plane, mGlsl );
	mNormalsBatch = gl::Batch::create( geom::VertexNormalLines( plane, 0.0f ), mNormalsGlsl );
}

void SuperformulaGpuApp::setup()
{
	mA1 = mA2 = 1.0f;
	mB1 = mB2 = 1.0f;
	mM1 = mM2 = 5.0f;
	mN11 = mN12 = 1.0f;
	mN21 = mN22 = 1.0f;
	mN31 = mN32 = 2.0f;

	mDrawNormals = false;
	mNormalsLength = 0.25f;
	mSubdivisions = 100;

	mParams = params::InterfaceGl::create( getWindow(), "App parameters", toPixels( ivec2( 200, 400 ) ) );
	mParams->addParam( "A (1)", &mA1 ).min( 0 ).max( 5 ).step( 0.05f );
	mParams->addParam( "B (1)", &mB1 ).min( 0 ).max( 5 ).step( 0.05f );
	mParams->addParam( "M (1)", &mM1 ).min( 0 ).max( 20 ).step( 0.25f );
	mParams->addParam( "N1 (1)", &mN11 ).min( 0 ).max( 100 ).step( 1.0 );
	mParams->addParam( "N2 (1)", &mN21 ).min( -50 ).max( 100 ).step( 0.5f );
	mParams->addParam( "N3 (1)", &mN31 ).min( -50 ).max( 100 ).step( 0.5f );
	mParams->addSeparator();
	mParams->addParam( "A (2)", &mA2 ).min( 0 ).max( 5 ).step( 0.05f );
	mParams->addParam( "B (2)", &mB2 ).min( 0 ).max( 5 ).step( 0.05f );
	mParams->addParam( "M (2)", &mM2 ).min( 0 ).max( 20 ).step( 0.25f );
	mParams->addParam( "N1 (2)", &mN12 ).min( 0 ).max( 100 ).step( 1.0 );
	mParams->addParam( "N2 (2)", &mN22 ).min( -50 ).max( 100 ).step( 0.5f );
	mParams->addParam( "N3 (2)", &mN32 ).min( -50 ).max( 100 ).step( 0.5f );
	mParams->addSeparator();
	mParams->addParam( "Subdivisions", &mSubdivisions ).min( 5 ).max( 500 ).step( 30 ).updateFn( [&] { setupGeometry(); } );
	mParams->addSeparator();
	mParams->addParam( "Draw Normals", &mDrawNormals );
	mParams->addParam( "Normals Length", &mNormalsLength ).min( 0.0f ).max( 2.0f ).step( 0.05f );
	
	mCam.lookAt( vec3( 3, 2, 4 ) * 0.75f, vec3( 0 ) );
	
	mGlsl = gl::GlslProg::create( loadAsset( "shader.vert" ), loadAsset( "shader.frag" ) );

	mNormalsGlsl = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "normals_shader.vert" ) ).fragment( loadAsset( "normals_shader.frag" ) )
											.attrib( geom::CUSTOM_0, "vNormalWeight" ) );
	setupGeometry();

	gl::enableDepthWrite();
	gl::enableDepthRead();
}

void SuperformulaGpuApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	gl::setMatrices( mCam );
}

void SuperformulaGpuApp::update()
{
	// Rotate the by 2 degrees around an arbitrary axis
	mRotation *= rotate( toRadians( 0.2f ), normalize( vec3( cos( getElapsedSeconds() / 3 ), 1, sin( getElapsedSeconds() / 5 ) ) ) );
	
	gl::ScopedGlslProg glslScp( mBatch->getGlslProg() );
	mBatch->getGlslProg()->uniform( "uA1", mA1 );
	mBatch->getGlslProg()->uniform( "uB1", mB1 );	
	mBatch->getGlslProg()->uniform( "uM1", mM1 );
	mBatch->getGlslProg()->uniform( "uN11", mN11 );	
	mBatch->getGlslProg()->uniform( "uN21", mN21 );
	mBatch->getGlslProg()->uniform( "uN31", mN31 );	

	mBatch->getGlslProg()->uniform( "uA2", mA2 );
	mBatch->getGlslProg()->uniform( "uB2", mB2 );	
	mBatch->getGlslProg()->uniform( "uM2", mM2 );
	mBatch->getGlslProg()->uniform( "uN12", mN12 );	
	mBatch->getGlslProg()->uniform( "uN22", mN22 );
	mBatch->getGlslProg()->uniform( "uN32", mN32 );	

	mNormalsBatch->getGlslProg()->uniform( "uA1", mA1 );
	mNormalsBatch->getGlslProg()->uniform( "uB1", mB1 );	
	mNormalsBatch->getGlslProg()->uniform( "uM1", mM1 );
	mNormalsBatch->getGlslProg()->uniform( "uN11", mN11 );	
	mNormalsBatch->getGlslProg()->uniform( "uN21", mN21 );
	mNormalsBatch->getGlslProg()->uniform( "uN31", mN31 );	

	mNormalsBatch->getGlslProg()->uniform( "uA2", mA2 );
	mNormalsBatch->getGlslProg()->uniform( "uB2", mB2 );	
	mNormalsBatch->getGlslProg()->uniform( "uM2", mM2 );
	mNormalsBatch->getGlslProg()->uniform( "uN12", mN12 );	
	mNormalsBatch->getGlslProg()->uniform( "uN22", mN22 );
	mNormalsBatch->getGlslProg()->uniform( "uN32", mN32 );
	mNormalsBatch->getGlslProg()->uniform( "uNormalsLength", mNormalsLength );
}

void SuperformulaGpuApp::draw()
{
	gl::clear();

	gl::setMatrices( mCam );
	gl::pushMatrices();
		gl::multModelMatrix( mRotation );
		mBatch->draw();
		gl::color( 1, 0.5, 0.25, 1 );
		if( mDrawNormals )
			mNormalsBatch->draw();
	gl::popMatrices();

	mParams->draw();
}

CINDER_APP_NATIVE( SuperformulaGpuApp, RendererGl )