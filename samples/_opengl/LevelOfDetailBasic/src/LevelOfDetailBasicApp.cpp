#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/Rand.h"

// Reference:
// https://www.researchgate.net/publication/235611365_A_Rendering_Pipeline_for_Real-Time_Crowds

using namespace ci;
using namespace ci::app;
using namespace std;

struct TeapotInstIn {
	vec3 mPosition;
};

struct TeapotInstOut {
	vec3 mPosition;
	float mLod;
};

const int NUM_LODS = 3;
const int NUM_INSTANCES = 2000;

class LevelOfDetailBasicApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;

	void loadGlslProgs();
	void setupBuffers();

	void setFrustumCullingUniforms( const gl::GlslProgRef &glsl );
	void runTransformFeedback( int lod );
	void drawLod( int lod );

	CameraPersp					mCamera;
	CameraUi					mCameraUi;

	vector<TeapotInstIn>		mTeapots;

	gl::VboRef					mInputVbo;
	gl::VaoRef					mUpdateVao;
	gl::GlslProgRef				mUpdateGlsl;
	gl::GlslProgRef				mRenderGlsl;

	gl::VboRef					mOutputLodVbos[NUM_LODS];
	gl::BatchRef				mLodBatches[NUM_LODS];
	gl::QueryRef				mQueryObjs[NUM_LODS];
	gl::TransformFeedbackObjRef	mTransformFeedbackObjs[NUM_LODS];

	gl::VboRef					mAllTeapotsVbo;
	gl::BatchRef				mAllTeapotsBatch;

	GLuint						mLodInstanceCounts[NUM_LODS];

	bool						mWireframe;
	bool						mLodEnabled;
	bool						mVisualizeLods;
	bool						mZoomedIn;
};

void LevelOfDetailBasicApp::setup()
{
	mWireframe		= false;
	mLodEnabled		= true;
	mVisualizeLods	= true;
	mZoomedIn		= false;

	mCamera.setPerspective( 60.0f, getWindowAspectRatio(), 0.1f, 1000.0f );
	mCamera.lookAt( vec3( 0, 0, 0 ), vec3( 10, 0, 0 ) );
	mCameraUi.setCamera( &mCamera );
	mCameraUi.connect( getWindow() );

	loadGlslProgs();
	setupBuffers();

	getWindow()->getSignalKeyDown().connect( [&] ( KeyEvent &e ) {
		if( e.getCode() == KeyEvent::KEY_w ) mWireframe		^= true;
		if( e.getCode() == KeyEvent::KEY_l ) mLodEnabled	^= true;
		if( e.getCode() == KeyEvent::KEY_v ) mVisualizeLods	^= true;
		if( e.getCode() == KeyEvent::KEY_z ) mZoomedIn		^= true;
	} );

	gl::enableVerticalSync( false );
}

void LevelOfDetailBasicApp::loadGlslProgs()
{
	gl::GlslProg::Format updateGlslFormat;
	updateGlslFormat.vertex( loadAsset( "update.vert" ) )
					.geometry( loadAsset( "update.geom" ) )
					.feedbackVaryings( { "oPosition", "oLod" } )
					.feedbackFormat( GL_INTERLEAVED_ATTRIBS );

	gl::GlslProg::Format renderGlslFormat;
	renderGlslFormat.vertex( loadAsset( "render.vert" ) )
					.fragment( loadAsset( "render.frag" ) );

	try {
		mUpdateGlsl = gl::GlslProg::create( updateGlslFormat );
		mRenderGlsl = gl::GlslProg::create( renderGlslFormat );
	}
	catch( const gl::GlslProgCompileExc &ex ) {
		console() << "Glsl compile error: " << ex.what() << endl;
	}
}

void LevelOfDetailBasicApp::setupBuffers()
{
	// populate initial instance data
	mTeapots.resize( NUM_INSTANCES );
	for( auto &teapot : mTeapots ) {
		teapot.mPosition = randVec3() * 100.0f;
		teapot.mPosition.y = randFloat( -10.0f, 10.0f );
	}

	// setup transform feedback VAO
	{
		mUpdateVao				= gl::Vao::create();
		mInputVbo				= gl::Vbo::create( GL_ARRAY_BUFFER, mTeapots, GL_STATIC_DRAW );

		gl::ScopedVao			vao_( mUpdateVao );
		gl::ScopedBuffer		vbo_( mInputVbo );

		size_t stride			= sizeof( TeapotInstIn );
		GLuint positionLocation	= mUpdateGlsl->getAttribSemanticLocation( geom::Attrib::POSITION );

		gl::enableVertexAttribArray( positionLocation );
		gl::vertexAttribPointer( positionLocation, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>( 0 ) );
	}

	// setup per-LOD buffers and other objects
	for( int i = 0; i < NUM_LODS; ++i ) {
		mOutputLodVbos[i]			= gl::Vbo::create( GL_ARRAY_BUFFER, sizeof( TeapotInstOut ) * NUM_INSTANCES, nullptr, GL_STATIC_READ );
		mTransformFeedbackObjs[i]	= gl::TransformFeedbackObj::create();
		mQueryObjs[i]				= gl::Query::create( GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN );

		mTransformFeedbackObjs[i]->bind();
		gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, mOutputLodVbos[i] );
		mTransformFeedbackObjs[i]->unbind();

		int subdivision				= int( pow( 2, 5 - i * 2 ) ); // LOD0: 32, LOD1: 8, LOD2: 2
		geom::AttribSet requested	= { geom::Attrib::POSITION, geom::Attrib::NORMAL };
		auto lodVboMesh				= gl::VboMesh::create( geom::Teapot().subdivisions( subdivision ), requested );

		size_t stride				= sizeof( TeapotInstOut );

		geom::BufferLayout instanceDataVboLayout;
		instanceDataVboLayout.append( geom::CUSTOM_0, 3, stride, offsetof( TeapotInstOut, mPosition ), 1 );
		instanceDataVboLayout.append( geom::CUSTOM_1, 1, stride, offsetof( TeapotInstOut, mLod ), 1 );

		lodVboMesh->appendVbo( instanceDataVboLayout, mOutputLodVbos[i] );

		mLodBatches[i] = gl::Batch::create( lodVboMesh, mRenderGlsl, {
			{ geom::Attrib::CUSTOM_0, "InstancePosition" },
			{ geom::Attrib::CUSTOM_1, "InstanceLod" }
		} );
	}

	// setup buffers of the non-LOD batch for performance comparison
	{
		geom::AttribSet requested	= { geom::Attrib::POSITION, geom::Attrib::NORMAL };
		auto vboMesh				= gl::VboMesh::create( geom::Teapot().subdivisions( 32 ), requested );
		mAllTeapotsVbo				= gl::Vbo::create( GL_ARRAY_BUFFER, sizeof( TeapotInstIn ) * NUM_INSTANCES, nullptr, GL_STATIC_DRAW );
		
		mAllTeapotsVbo->bufferData( sizeof( TeapotInstIn ) * NUM_INSTANCES, mTeapots.data(), GL_STATIC_DRAW );

		geom::BufferLayout instanceDataVboLayout;
		instanceDataVboLayout.append( geom::CUSTOM_0, 3, sizeof( TeapotInstIn ), 0, 1 );

		vboMesh->appendVbo( instanceDataVboLayout, mAllTeapotsVbo );

		mAllTeapotsBatch = gl::Batch::create( vboMesh, mRenderGlsl, {
			{ geom::Attrib::CUSTOM_0, "InstancePosition" }
		} );
	}
}

void LevelOfDetailBasicApp::setFrustumCullingUniforms( const gl::GlslProgRef &glsl )
{
	vec3 Z = normalize( mCamera.getViewDirection() );
	vec3 X = normalize( cross( Z, mCamera.getWorldUp() ) );
	vec3 Y = normalize( cross( X, Z ) );

	glsl->uniform( "uNear",			mCamera.getNearClip() );
	glsl->uniform( "uFar",			mCamera.getFarClip() );
	glsl->uniform( "uCameraPos",	mCamera.getEyePoint() );
	glsl->uniform( "uAspectRatio",	mCamera.getAspectRatio() );
	glsl->uniform( "uTanFov",		tan( glm::radians( mCamera.getFov() / 2 ) ) );
	glsl->uniform( "uCameraX",		X );
	glsl->uniform( "uCameraY",		Y );
	glsl->uniform( "uCameraZ",		Z );
	glsl->uniform( "uDelta",		0.4f );
}

void LevelOfDetailBasicApp::runTransformFeedback( int lod )
{
	gl::ScopedVao		vao_( mUpdateVao );
	gl::ScopedGlslProg	glsl_( mUpdateGlsl );
	gl::ScopedState		state_( GL_RASTERIZER_DISCARD, true );
	gl::ScopedMatrices	mtx_;

	gl::setMatrices( mCamera );
	gl::setDefaultShaderVars();

	setFrustumCullingUniforms( mUpdateGlsl );
	mUpdateGlsl->uniform( "uCurrentLod", float( lod ) );

	mTransformFeedbackObjs[lod]->bind();

	mQueryObjs[lod]->begin();
	gl::beginTransformFeedback( GL_POINTS );
	gl::drawArrays( GL_POINTS, 0, NUM_INSTANCES );
	gl::endTransformFeedback();
	mQueryObjs[lod]->end();
}

void LevelOfDetailBasicApp::drawLod( int lod )
{
	GLuint instanceCount = mQueryObjs[lod]->getValueUInt();
	mLodInstanceCounts[lod] = instanceCount;

	mLodBatches[lod]->getGlslProg()->uniform( "uVisualizeLods", mVisualizeLods );
	mLodBatches[lod]->drawInstanced( instanceCount );
}

void LevelOfDetailBasicApp::update()
{
	getWindow()->setTitle( to_string( int( getAverageFps() ) ) );

	mCamera.setFov( mZoomedIn ? 8.0f : 60.0f );

	if( mLodEnabled ) {
		for( int i = 0; i < NUM_LODS; ++i )
			runTransformFeedback( i );
	}
}

void LevelOfDetailBasicApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	gl::ScopedMatrices	mtx_;
	gl::ScopedDepth		depth_( true );

	// render scene
	{
		gl::setMatrices( mCamera );

		if( mWireframe )
			gl::enableWireframe();

		if( mLodEnabled ) {
			for( int i = 0; i < NUM_LODS; ++i )
				drawLod( i );
		}
		else {
			mAllTeapotsBatch->getGlslProg()->uniform( "uVisualizeLods", false );
			mAllTeapotsBatch->drawInstanced( NUM_INSTANCES );
		}

		if( mWireframe )
			gl::disableWireframe();
	}

	// render UI
	{
		gl::setMatricesWindow( getWindowSize() );

		gl::ScopedBlendAlpha blend_;
		TextLayout layoutL, layoutR;

		layoutL.clear( ColorA::gray( 0.2f, 0.5f ) );
		layoutL.setFont( Font( "Arial", 18 ) );
		layoutL.setColor( Color::white() );
		layoutL.setLeadingOffset( 3 );
		layoutL.addLine( "(V) Toggle visualizing layers" );
		layoutL.addLine( "(L) Toggle LOD" );
		layoutL.addLine( "(W) Toggle wireframe" );
		layoutL.addLine( "(Z) Toggle zoom" );

		layoutR.clear( ColorA::gray( 0.2f, 0.5f ) );
		layoutR.setFont( Font( "Arial", 18 ) );
		layoutR.setColor( Color::white() );
		layoutR.setLeadingOffset( 3 );
		layoutR.addRightLine( string( "FPS: " + to_string( int( getAverageFps() ) ) ) );

		if( mLodEnabled )
			for( int i = 0; i < NUM_LODS; ++i )
				layoutR.addRightLine( string( "LOD" + to_string( i ) + ": " + to_string( mLodInstanceCounts[i] ) ) );
		else
			layoutR.addRightLine( "All instances (" + to_string( NUM_INSTANCES ) + ") drawn as LOD0" );

		auto texL = gl::Texture::create( layoutL.render( true ) );
		gl::draw( texL, vec2( 16, 10 ) );

		auto texR = gl::Texture::create( layoutR.render( true ) );
		gl::draw( texR, vec2( getWindowWidth() - texR->getWidth() - 16, 10 ) );
	}
}

CINDER_APP( LevelOfDetailBasicApp, RendererGl( RendererGl::Options().msaa( 16 ) ), [] ( App::Settings *settings ) {
	settings->setHighDensityDisplayEnabled();
	settings->setWindowSize( 1200, 800 );
	settings->disableFrameRate();
})
