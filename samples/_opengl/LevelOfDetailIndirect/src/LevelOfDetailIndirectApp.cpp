#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/Rand.h"

// References:
// https://www.researchgate.net/publication/235611365_A_Rendering_Pipeline_for_Real-Time_Crowds
// https://www.opengl.org/registry/specs/ARB/query_buffer_object.txt
// http://rastergrid.com/blog/2011/06/multi-draw-indirect-is-here/

using namespace ci;
using namespace ci::app;
using namespace std;

#define ATOMIC_COUNTER			0
#define QUERY_BUFFER_OBJECT		1
#define TECHNIQUE				QUERY_BUFFER_OBJECT

struct TeapotInstIn {
	vec3 mPosition;
};

struct TeapotInstOut {
	vec3 mPosition;
	float mLod;
};

struct IndirectDrawCommand {
	GLuint elementCount;
	GLuint instanceCount;
	GLuint firstIndex;
	GLuint baseVertex;
	GLuint baseInstance;
};

const int NUM_LODS = 3;
const int NUM_INSTANCES = 2000;

class LevelOfDetailIndirectApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;

	void loadGlslProgs();
	void setupBuffers();

	void setFrustumCullingUniforms( const gl::GlslProgRef& glsl );
	void runTransformFeedback( int lod );
	void drawLodIndirect( int lod );

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

	gl::BufferObjRef			mIndirectBuffers[NUM_LODS];
	IndirectDrawCommand			mIndirectDrawCommands[NUM_LODS];

	bool						mWireframe;
	bool						mVisualizeLods;
	bool						mZoomedIn;
	bool						mCPUReadBack;
};

void LevelOfDetailIndirectApp::setup()
{
	mWireframe		= false;
	mVisualizeLods	= true;
	mZoomedIn		= false;
	mCPUReadBack	= false;

	mCamera.setPerspective( 60.0f, getWindowAspectRatio(), 0.1f, 1000.0f );
	mCamera.lookAt( vec3( 0, 0, 0 ), vec3( 10, 0, 0 ) );
	mCameraUi.setCamera( &mCamera );
	mCameraUi.connect( getWindow() );

	loadGlslProgs();
	setupBuffers();

	getWindow()->getSignalKeyDown().connect( [&] ( KeyEvent &e ) {
		if( e.getCode() == KeyEvent::KEY_w ) mWireframe		^= true;
		if( e.getCode() == KeyEvent::KEY_v ) mVisualizeLods	^= true;
		if( e.getCode() == KeyEvent::KEY_z ) mZoomedIn		^= true;
		if( e.getCode() == KeyEvent::KEY_r ) mCPUReadBack	^= true;
	} );

	gl::enableVerticalSync( false );
}

void LevelOfDetailIndirectApp::loadGlslProgs()
{
	gl::GlslProg::Format updateGlslFormat;
	updateGlslFormat.vertex( loadAsset( "update.vert" ) )
		.geometry( loadAsset( "update.geom" ) )
#if TECHNIQUE == ATOMIC_COUNTER
		.define( "USE_ATOMIC_COUNTER" )
#endif
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

void LevelOfDetailIndirectApp::setupBuffers()
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

		// set up indirect draw command buffers, fill in index count of each LOD's mesh
		GLuint elementCount = lodVboMesh->getNumIndices();
		mIndirectDrawCommands[i] = { elementCount, 0, 0, 0, 0 };
		mIndirectBuffers[i] = gl::BufferObj::create( GL_DRAW_INDIRECT_BUFFER, sizeof( IndirectDrawCommand ), &mIndirectDrawCommands[i], GL_STATIC_COPY );
	}
}

void LevelOfDetailIndirectApp::setFrustumCullingUniforms( const gl::GlslProgRef& glsl )
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

void LevelOfDetailIndirectApp::runTransformFeedback( int lod )
{
	gl::ScopedVao		vao_( mUpdateVao );
	gl::ScopedGlslProg	glsl_( mUpdateGlsl );
	gl::ScopedState		state_( GL_RASTERIZER_DISCARD, true );
	gl::ScopedMatrices	mtx_;

	gl::setMatrices( mCamera );
	gl::setDefaultShaderVars();

	setFrustumCullingUniforms( mUpdateGlsl );
	mUpdateGlsl->uniform( "uCurrentLod", float( lod ) );
	
#if TECHNIQUE == ATOMIC_COUNTER
	// clear the instance count field of indirect buffer
	IndirectDrawCommand reset = mIndirectDrawCommands[lod];
	reset.instanceCount = 0;
	mIndirectBuffers[lod]->bufferSubData( 0 , sizeof( IndirectDrawCommand ), &reset );

	// note that we're binding to a target that is different from the buffer's designated target
	gl::bindBufferBase( GL_ATOMIC_COUNTER_BUFFER, 0, mIndirectBuffers[lod] );
#endif

	mTransformFeedbackObjs[lod]->bind();

#if TECHNIQUE == QUERY_BUFFER_OBJECT
	mQueryObjs[lod]->begin();
#endif

	gl::beginTransformFeedback( GL_POINTS );
	gl::drawArrays( GL_POINTS, 0, NUM_INSTANCES );
	gl::endTransformFeedback();

#if TECHNIQUE == QUERY_BUFFER_OBJECT
	mQueryObjs[lod]->end();
#endif
}

void LevelOfDetailIndirectApp::drawLodIndirect( int lod )
{
#if TECHNIQUE == QUERY_BUFFER_OBJECT
	// write the number of primitives generated by transform feedback into indirect buffer as instance count
	mIndirectBuffers[lod]->bind( GL_QUERY_BUFFER );
	mQueryObjs[lod]->getValueUInt( reinterpret_cast<GLuint*>( offsetof( IndirectDrawCommand, instanceCount ) ) );
#endif

	// read instance count from the indirect buffer to CPU, which hurts performance
	if( mCPUReadBack )
		mIndirectBuffers[lod]->getBufferSubData( 0, sizeof( IndirectDrawCommand ), &mIndirectDrawCommands[lod] );

	gl::ScopedBuffer indirect_( mIndirectBuffers[lod] );

	mLodBatches[lod]->getGlslProg()->uniform( "uVisualizeLods", mVisualizeLods );
	mLodBatches[lod]->drawIndirect( reinterpret_cast<GLvoid*>( 0 ) );
}

void LevelOfDetailIndirectApp::update()
{
	getWindow()->setTitle( to_string( int( getAverageFps() ) ) );

	mCamera.setFov( mZoomedIn ? 8.0f : 60.0f );

	for( int i = 0; i < NUM_LODS; ++i )
		runTransformFeedback( i );
}

void LevelOfDetailIndirectApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	gl::ScopedMatrices	mtx_;
	gl::ScopedDepth		depth_( true );

	// render scene
	{
		gl::setMatrices( mCamera );

		if( mWireframe )
			gl::enableWireframe();

		for( int i = 0; i < NUM_LODS; ++i )
			drawLodIndirect( i );

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
		layoutL.addLine( "(W) Toggle wireframe" );
		layoutL.addLine( "(Z) Toggle zoom" );
		layoutL.addLine( "(R) Toggle instance count read back" );

		layoutR.clear( ColorA::gray( 0.2f, 0.5f ) );
		layoutR.setFont( Font( "Arial", 18 ) );
		layoutR.setColor( Color::white() );
		layoutR.setLeadingOffset( 3 );
		layoutR.addRightLine( string( "FPS: " + to_string( int( getAverageFps() ) ) ) );

		if( mCPUReadBack )
			for( int i = 0; i < NUM_LODS; ++i )
				layoutR.addRightLine( string( "LOD" + to_string( i ) + ": " + to_string( mIndirectDrawCommands[i].instanceCount ) ) );
		else
			layoutR.addRightLine( "Instance count not available on the CPU, turn on read back to see" );
		
		auto texL = gl::Texture::create( layoutL.render( true ) );
		gl::draw( texL, vec2( 16, 10 ) );

		auto texR = gl::Texture::create( layoutR.render( true ) );
		gl::draw( texR, vec2( getWindowWidth() - texR->getWidth() - 16, 10 ) );
	}
}

CINDER_APP( LevelOfDetailIndirectApp, RendererGl( RendererGl::Options().msaa( 16 ) ), [] ( App::Settings *settings ) {
	settings->setHighDensityDisplayEnabled();
	settings->setWindowSize( 1200, 800 );
	settings->disableFrameRate();
})
