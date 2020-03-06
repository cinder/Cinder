#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/CameraUi.h"

#include "cinder/gl/Vao.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/BufferTexture.h"
#include "cinder/gl/GlslProg.h"

#include "cinder/CinderImGui.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const uint32_t POINTS_X				= 50;
const uint32_t POINTS_Y				= 50;
const uint32_t POINTS_TOTAL			= (POINTS_X * POINTS_Y);
const uint32_t CONNECTIONS_TOTAL	= (POINTS_X - 1) * POINTS_Y + (POINTS_Y - 1) * POINTS_X;

const uint32_t POSITION_INDEX		= 0;
const uint32_t VELOCITY_INDEX		= 1;
const uint32_t CONNECTION_INDEX		= 2;

class ClothSimulationApp : public App {
  public:
	ClothSimulationApp();
	void update() override;
	void draw() override;
	
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void mouseUp( MouseEvent event ) override;
	void updateRayPosition( const ci::ivec2 &mousePos, bool useDistance );
	
	void setupBuffers();
	void setupGlsl();
	void updateParams();
	
	std::array<gl::VaoRef, 2>			mVaos;
	std::array<gl::VboRef, 2>			mPositions, mVelocities, mConnections;
	std::array<gl::BufferTextureRef, 2>	mPositionBufTexs;
	gl::VboRef							mLineIndices;
	gl::GlslProgRef						mUpdateGlsl, mRenderGlsl;
	
	CameraPersp							mCam;
	CameraUi							mCamUi;
	float								mCurrentCamRotation;
	uint32_t							mIterationsPerFrame, mIterationIndex;
	bool								mDrawPoints, mDrawLines, mUpdate;
};

ClothSimulationApp::ClothSimulationApp()
: mIterationsPerFrame( 16 ), mIterationIndex( 0 ),
	mDrawPoints( true ), mDrawLines( true ),
	mCurrentCamRotation( 0.0f ), mUpdate( true ),
	mCam( getWindowWidth(), getWindowHeight(), 20.0f, 0.01f, 1000.0f )
{
	mCamUi = CameraUi( &mCam );
	vec3 eye = vec3( sin( mCurrentCamRotation ) * 140.0f, 0,
					cos( mCurrentCamRotation ) * 140.0f );
	vec3 target = vec3( 0.0f );
	mCam.lookAt( eye, target );
	
	setupGlsl();
	setupBuffers();

	ImGui::Initialize();
}

void ClothSimulationApp::setupBuffers()
{
	int i, j;
	
	std::array<vec4, POINTS_TOTAL> positions;
	std::array<vec3, POINTS_TOTAL> velocities;
	std::array<ivec4, POINTS_TOTAL> connections;
	
	int n = 0;
	for ( j = 0; j < POINTS_Y; j++ ) {
		float fj = (float)j / (float)POINTS_Y;
		for ( i = 0; i < POINTS_X; i++) {
			float fi = (float)i / (float)POINTS_X;
			
			// create our initial positions, Basically a plane
			positions[n] = vec4( ( fi - 0.5f ) * (float)POINTS_X,
								 ( fj - 0.5f ) * (float)POINTS_Y,
								 0.6f * sinf( fi ) * cosf( fj ),
								 1.0f );
			// zero out velocities
			velocities[n] = vec3( 0.0f );
			// to create connections we'll use an integer buffer.
			// The value -1 refers to the fact that there's no
			// connection. This helps for the edge cases of the plane
			// and also the top of the cloth which we want to be static.
			connections[n] = ivec4( -1 );
			
			// check the edge cases and initialize the connections to be
			// basically, above, below, left, and right of this point
			if ( j != ( POINTS_Y - 1 ) ) {
				if ( i != 0 ) connections[n][0] = n - 1;					// left
				if ( j != 0 ) connections[n][1] = n - POINTS_X;				// above
				if (i != (POINTS_X - 1)) connections[n][2] = n + 1;			// right
				if (j != (POINTS_Y - 1)) connections[n][3] = n + POINTS_X;	// below
			}
			n++;
		}
	}
	
	for ( i = 0; i < 2; i++ ) {
		mVaos[i] = gl::Vao::create();
		gl::ScopedVao scopeVao( mVaos[i] );
		{
			// buffer the positions
			mPositions[i] = gl::Vbo::create( GL_ARRAY_BUFFER, positions.size() * sizeof(vec4), positions.data(), GL_STATIC_DRAW );
			{
				// bind and explain the vbo to your vao so that it knows how to distribute vertices to your shaders.
				gl::ScopedBuffer sccopeBuffer( mPositions[i] );
				gl::vertexAttribPointer( POSITION_INDEX, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*) 0 );
				gl::enableVertexAttribArray( POSITION_INDEX );
			}
			// buffer the velocities
			mVelocities[i] = gl::Vbo::create( GL_ARRAY_BUFFER, velocities.size() * sizeof(vec3), velocities.data(), GL_STATIC_DRAW );
			{
				// bind and explain the vbo to your vao so that it knows how to distribute vertices to your shaders.
				gl::ScopedBuffer scopeBuffer( mVelocities[i] );
				gl::vertexAttribPointer( VELOCITY_INDEX, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*) 0 );
				gl::enableVertexAttribArray( VELOCITY_INDEX );
			}
			// buffer the connections
			mConnections[i] = gl::Vbo::create( GL_ARRAY_BUFFER, connections.size() * sizeof(ivec4), connections.data(), GL_STATIC_DRAW );
			{
				// bind and explain the vbo to your vao so that it knows how to distribute vertices to your shaders.
				gl::ScopedBuffer scopeBuffer( mConnections[i] );
				gl::vertexAttribIPointer( CONNECTION_INDEX, 4, GL_INT, 0, (const GLvoid*) 0 );
				gl::enableVertexAttribArray( CONNECTION_INDEX );
			}
		}
	}
	// create your two BufferTextures that correspond to your position buffers.
	mPositionBufTexs[0] = gl::BufferTexture::create( mPositions[0], GL_RGBA32F );
	mPositionBufTexs[1] = gl::BufferTexture::create( mPositions[1], GL_RGBA32F );
	
	int lines = (POINTS_X - 1) * POINTS_Y + (POINTS_Y - 1) * POINTS_X;
	// create the indices to draw links between the cloth points
	mLineIndices = gl::Vbo::create( GL_ELEMENT_ARRAY_BUFFER, lines * 2 * sizeof(int), nullptr, GL_STATIC_DRAW );
	
	auto e = (int *) mLineIndices->mapReplace();
	for (j = 0; j < POINTS_Y; j++) {
		for (i = 0; i < POINTS_X - 1; i++) {
			*e++ = i + j * POINTS_X;
			*e++ = 1 + i + j * POINTS_X;
		}
	}
	
	for (i = 0; i < POINTS_X; i++) {
		for (j = 0; j < POINTS_Y - 1; j++) {
			*e++ = i + j * POINTS_X;
			*e++ = POINTS_X + i + j * POINTS_X;
		}
	}
	mLineIndices->unmap();
}

void ClothSimulationApp::setupGlsl()
{
	// These are the names of our out going vertices. GlslProg needs to
	// know which attributes should be captured by Transform FeedBack.
	std::vector<std::string> feedbackVaryings({
		"tf_position_mass",
		"tf_velocity"
	});
	
	gl::GlslProg::Format updateFormat;
	updateFormat.vertex( loadAsset( "update.vert" ) )
				// Because we have separate buffers with which
				// to capture attributes, we're using GL_SEPERATE_ATTRIBS
				.feedbackFormat( GL_SEPARATE_ATTRIBS )
				// We also send the names of the attributes to capture
				.feedbackVaryings( feedbackVaryings );
	
	mUpdateGlsl = gl::GlslProg::create( updateFormat );
	// Set this, otherwise it will be set to vec3( 0, 0, 0 ),
	// which is in the center of the cloth
	mUpdateGlsl->uniform( "rayPosition", mCam.getEyePoint() );
	
	gl::GlslProg::Format renderFormat;
	renderFormat.vertex( loadAsset( "render.vert" ) )
				.fragment( loadAsset( "render.frag" ) );
	
	mRenderGlsl = gl::GlslProg::create( renderFormat );
}

void ClothSimulationApp::update()
{
	updateParams();

	if( ! mUpdate ) return;
	
	gl::ScopedGlslProg	scopeGlsl( mUpdateGlsl );
	gl::ScopedState		scopeState( GL_RASTERIZER_DISCARD, true );
	
	for( auto i = mIterationsPerFrame; i != 0; --i ) {
		// Bind the vao that has the original vbo attached,
		// these buffers will be used to read from.
		gl::ScopedVao scopedVao( mVaos[mIterationIndex & 1] );
		// Bind the BufferTexture, which contains the positions
		// of the first vbo. We'll cycle through the neighbors
		// using the connection buffer so that we can derive our
		// next position and velocity to write to Transform Feedback
		gl::ScopedTextureBind scopeTex( mPositionBufTexs[mIterationIndex & 1]->getTarget(), mPositionBufTexs[mIterationIndex & 1]->getId() );
		
		// We iterate our index so that we'll be using the
		// opposing buffers to capture the data
		mIterationIndex++;
		
		// Now bind our opposing buffers to the correct index
		// so that we can capture the values coming from the shader
		gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, POSITION_INDEX, mPositions[mIterationIndex & 1] );
		gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, VELOCITY_INDEX, mVelocities[mIterationIndex & 1] );
		
		// Begin Transform feedback with the correct primitive,
		// In this case, we want GL_POINTS, because each vertex
		// exists by itself
		gl::beginTransformFeedback( GL_POINTS );
		// Now we issue our draw command which puts all of the
		// setup in motion and processes all the vertices
		gl::drawArrays( GL_POINTS, 0, POINTS_TOTAL );
		// After that we issue an endTransformFeedback command
		// to tell OpenGL that we're finished capturing vertices
		gl::endTransformFeedback();
	}
}

void ClothSimulationApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	
	// Notice that this vao holds the buffers we've just
	// written to with Transform Feedback. It will show
	// the most recent positions
	gl::ScopedVao scopeVao( mVaos[mIterationIndex & 1] );
	gl::ScopedGlslProg scopeGlsl( mRenderGlsl );
	gl::setMatrices( mCam );
	gl::setDefaultShaderVars();
	
	if( mDrawPoints ) {
		gl::pointSize( 4.0f);
		gl::drawArrays( GL_POINTS, 0, POINTS_TOTAL );
	}
	if( mDrawLines ) {
		gl::ScopedBuffer scopeBuffer( mLineIndices );
		gl::drawElements( GL_LINES, CONNECTIONS_TOTAL * 2, GL_UNSIGNED_INT, nullptr );
	}

}

void ClothSimulationApp::mouseDown( MouseEvent event )
{
	if( event.isRightDown() )
		mCamUi.mouseDrag( event.getPos(), true, false, false );
	else
		updateRayPosition( event.getPos(), true );
}

void ClothSimulationApp::mouseDrag( MouseEvent event )
{
	if( event.isRightDown() )
		mCamUi.mouseDrag( event.getPos(), true, false, false );
	else
		updateRayPosition( event.getPos(), true );
}

void ClothSimulationApp::mouseUp( MouseEvent event )
{
	updateRayPosition( event.getPos(), false );
}

void ClothSimulationApp::updateRayPosition( const ci::ivec2 &mousePos, bool useDistance )
{
	auto ray = mCam.generateRay( mousePos, getWindowSize() );
	auto dist = ci::distance( mCam.getEyePoint(), vec3() );
	auto rayPosition = ray.calcPosition( useDistance ? dist : 0 );
	mUpdateGlsl->uniform( "rayPosition", rayPosition );
}

void ClothSimulationApp::updateParams()
{
	static vec3 gravity = vec3( 0.0, -0.08, 0.0 );
	static float restLength = 0.88;
	static float dampingConst = 2.8;
	static float springConstant = 7.1;

	ImGui::Begin( "Cloth Simulation" );
	ImGui::Text( "Update Params" );
	ImGui::Checkbox( "Update", &mUpdate );
	ImGui::SliderInt( "Updates/Frame", (int*)&mIterationsPerFrame, 1, 16 );
	if( ImGui::DragFloat3( "Gravity", &gravity, 0.01f ) ) {
		mUpdateGlsl->uniform( "gravity", gravity );
	}
	if( ImGui::DragFloat( "Rest Length", &restLength, 0.01f, 0.0f, 30.0f ) ) {
		mUpdateGlsl->uniform( "rest_length", restLength );
	}
	if( ImGui::DragFloat( "Damping Constant", &dampingConst, 0.01f, 1.5f, 25.0f ) ) {
		mUpdateGlsl->uniform( "c", dampingConst );
	}
	if( ImGui::DragFloat( "Spring Constant", &springConstant, 0.01f, 0.1f, 17.0f ) ) {
		mUpdateGlsl->uniform( "k", springConstant );
	}
	if( ImGui::Button( "Eject Button" ) ) {
		restLength = 2.0;
		mUpdateGlsl->uniform( "rest_length", restLength );
		dampingConst = 5.0;
		mUpdateGlsl->uniform( "c", dampingConst );
	}
	if( ImGui::Button( "Reset" ) ) {
		gravity = vec3( 0.0, -0.08, 0.0 );
		mUpdateGlsl->uniform( "gravity", gravity );
		restLength = 0.88;
		mUpdateGlsl->uniform( "rest_length", restLength );
		dampingConst = 2.8;
		mUpdateGlsl->uniform( "c", dampingConst );
		springConstant = 7.1;
		mUpdateGlsl->uniform( "k", springConstant );
	}
	ImGui::Separator();
	ImGui::Text( "Render Params" );
	ImGui::Checkbox( "Draw Lines", &mDrawLines );
	ImGui::Checkbox( "Draw Points", &mDrawPoints );
	ImGui::Text( "Right Mouse Button Rotates" );
	ImGui::End();
}

CINDER_APP( ClothSimulationApp, RendererGl(),
[&]( App::Settings *settings ) {
	settings->setWindowSize( 1280, 720 );
})
