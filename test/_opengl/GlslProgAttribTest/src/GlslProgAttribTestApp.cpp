// * Should render as rotating circle of color cubes using per-instance mat4 attribute
// * Forward/backward moving pair of R/G/B triple of spheres, one offset on Y from the other
// 'c' key should change RGB -> CMY, testing uploading uniform arrays

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/GeomIo.h"

#include "cinder/gl/Batch.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/gl/Vbo.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const int NUM_CUBES = 100;

class GlslProgAttribTestApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override { mCamUi.mouseDown( event ); }
	void mouseDrag( MouseEvent event ) override { mCamUi.mouseDrag( event ); }
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;
	
	void setupBuffers();
	
	gl::BatchRef		mCube, mSpheres, mSpheres2;
	gl::GlslProgRef		mGlsl, mUniformGlsl, mUniformGlsl2;
	gl::VboRef			mModelMatricesVbo;
	
	std::vector<mat4>	mModelMatricesCpu;
	
	CameraPersp			mCam;
	CameraUi			mCamUi;
	ColorA				mColorsRgb[3], mColorsCmy[3];
	bool				mRgbNotCmy;
};

void GlslProgAttribTestApp::setup()
{
	mModelMatricesCpu.resize( NUM_CUBES );
	
	mGlsl = gl::GlslProg::create( gl::GlslProg::Format()
								 .vertex( loadAsset( "InstancedModelMatrix.vert" ) )
								 .fragment( loadAsset( "InstancedModelMatrix.frag" ) ) );
	mUniformGlsl  = gl::GlslProg::create( gl::GlslProg::Format()
								 .vertex( loadAsset( "UniformTest.vert" ) )
								 .fragment( loadAsset( "UniformTest.frag" ) ) );
	mUniformGlsl2  = gl::GlslProg::create( gl::GlslProg::Format()
								 .vertex( loadAsset( "UniformTest.vert" ) )
								 .fragment( loadAsset( "UniformTest.frag" ) ) );

	setupBuffers();
	
	mCam.setPerspective( 60.0f, getWindowAspectRatio(), .01f, 1000.0f );
	mCam.lookAt( vec3( 0, 0, 10 ), vec3( 0 ) );
	mCamUi.setCamera( &mCam );
	
	gl::enableDepthRead();
	gl::enableDepthWrite();

	mRgbNotCmy = true;
	
	mColorsRgb[0] = ColorA( 1, 0, 0, 1 );
	mColorsRgb[1] = ColorA( 0, 1, 0, 1 );
	mColorsRgb[2] = ColorA( 0, 0, 1, 1 );

	mColorsCmy[0] = ColorA( 0, 1, 1, 1 );
	mColorsCmy[1] = ColorA( 1, 0, 1, 1 );
	mColorsCmy[2] = ColorA( 1, 1, 0, 1 );
}

void GlslProgAttribTestApp::setupBuffers()
{
	mModelMatricesVbo = gl::Vbo::create( GL_ARRAY_BUFFER,
										mModelMatricesCpu.size() * sizeof(mat4),
										nullptr,
										GL_DYNAMIC_DRAW );
	geom::BufferLayout layout;
	layout.append( geom::CUSTOM_0, 16, sizeof(mat4), 0, 1 );
	
	auto cubeVboMesh = gl::VboMesh::create( geom::Cube().size( vec3( 1 ) ).colors() );
	cubeVboMesh->appendVbo( layout, mModelMatricesVbo );
	
	mCube = gl::Batch::create( cubeVboMesh, mGlsl, { { geom::CUSTOM_0, "aModelMatrix" } } );

	auto sphere1 = geom::Sphere() >> geom::Translate( vec3( -1, 0, 0 ) ) >> geom::AttribFn<vec2,vec2>( geom::TEX_COORD_0, []( vec2 ) { return vec2(0,0); } );
	auto sphere2 = geom::Sphere() >> geom::Translate( vec3( 0, 0, 0 ) ) >> geom::AttribFn<vec2,vec2>( geom::TEX_COORD_0, []( vec2 ) { return vec2(1,0); } );
	auto sphere3 = geom::Sphere() >> geom::Translate( vec3( 1, 0, 0 ) ) >> geom::AttribFn<vec2,vec2>( geom::TEX_COORD_0, []( vec2 ) { return vec2(2,0); } );
	auto spheres = sphere1 & sphere2 & sphere3;
	
	mSpheres = gl::Batch::create( spheres, mUniformGlsl );
	// without fbf9c8f2c813e562a23c561c147babc72e064c5e this line would crash
	mSpheres2 = gl::Batch::create( spheres >> geom::Translate( 0, 1, 0 ), mUniformGlsl2 );
}

void GlslProgAttribTestApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'c' ) {
		mRgbNotCmy = ! mRgbNotCmy;
	}
}

void GlslProgAttribTestApp::update()
{
	int i = 0;
	float seperation = 8.0f;
	for( auto & mat : mModelMatricesCpu ) {
		float zOffset	= ( ( ( float(i) / NUM_CUBES ) - 0.5f ) * seperation ) + ( seperation * .1 );
		float timeVal	= getElapsedSeconds() + i;
		float sinVal	= sin( timeVal );
		float cosVal	= cos( timeVal );
		
		auto position	= vec3( cosVal, sinVal, zOffset );
		auto rotation	= quat( vec3( 0, timeVal, 0 ) );
		auto scale		= vec3( ( sinVal + 1.0f ) / 2.0f + .3f  );
		
		mat = mat4();
		mat *= ci::translate( position );
		mat *= ci::toMat4( rotation );
		mat *= ci::scale( scale );
		i++;
	}
	
	mModelMatricesVbo->bufferSubData( 0, mModelMatricesCpu.size() * sizeof( mat4 ), mModelMatricesCpu.data() );
}

void GlslProgAttribTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatrices( mCam );
	mCube->drawInstanced( mModelMatricesCpu.size() );

	if( mRgbNotCmy ) {
		mSpheres->getGlslProg()->uniform( "uColors", (vec4*)mColorsRgb, 3 );
		mSpheres2->getGlslProg()->uniform( "uColors[0]", mColorsRgb[0] );
		mSpheres2->getGlslProg()->uniform( "uColors[1]", (vec4*)&mColorsRgb[1], 2 );
	}
	else {
		mSpheres->getGlslProg()->uniform( "uColors", (vec4*)mColorsCmy, 3 );
		mSpheres2->getGlslProg()->uniform( "uColors[0]", mColorsCmy[0] );
		mSpheres2->getGlslProg()->uniform( "uColors[1]", (vec4*)&mColorsCmy[1], 2 );
	}

	mUniformGlsl->uniform( "uColors[a]", vec4() ); // should log an 'Unknown uniform' warning

	// test setting uniforms in struct array
	mUniformGlsl->uniform( "uTestStruct[0].value", 1.0f );
	mUniformGlsl->uniform( "uTestStruct[1].value", 1.0f );
	mUniformGlsl->uniform( "uTestStruct[2].value", 1.0f );

	mUniformGlsl->uniform( "uTestStruct[2].blah", 1.0f ); // should log an 'Unknown uniform' warning
	mUniformGlsl->uniform( "uTestStruct[b].value", 1.0f ); // should log an 'Unknown uniform' warning
	mUniformGlsl->uniform( "uTestStruct[blah].value", 1.0f ); // should log an 'Unknown uniform' warning

	gl::translate( 0, 0, 5 * sin( getElapsedSeconds() ) );	
	mSpheres->draw();
	mSpheres2->draw();
}

CINDER_APP( GlslProgAttribTestApp, RendererGl, []( App::Settings *settings ){ settings->setMultiTouchEnabled( false ); } )
