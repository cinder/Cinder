#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/TransformFeedbackObj.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/BufferTexture.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// This sample is based on the Spring-Mass System, presented in the 7th chapter
// of the OpenGL SuperBible, 6th edition. For a more in-depth explanation of
// what is going on here, please refer to that.

const int POSITION_INDEX	= 0;
const int VELOCITY_INDEX	= 1;
const int CONNECTION_INDEX	= 2;

const int POINTS_X			= 50;
const int POINTS_Y			= 50;
const int POINTS_TOTAL		= (POINTS_X * POINTS_Y);
const int CONNECTIONS_TOTAL	= (POINTS_X - 1) * POINTS_Y + (POINTS_Y - 1) * POINTS_X;

class TransformFeedbackClothSimulationApp : public AppNative {
  public:
	void setup();
	void mouseDrag( MouseEvent event );
	void mouseUp( MouseEvent event );
	void keyDown( KeyEvent event );
	void update();
	void draw();
	
	void loadShaders();
	void loadBuffers();
	
	gl::GlslProgRef				mUpdateGlsl, mRenderGlsl;
	gl::VaoRef					mVaos[2];
	gl::VboRef					mPositions[2], mVelocities[2], mConnections, mLineElementBuffer;
	gl::TransformFeedbackObjRef mFeedbackObjs[2];
	gl::BufferTextureRef		mPosBufferTextures[2];
	int							mIterationsPerFrame, mIterationIndex, mTriangleIndices, mLineIndices;
	bool						drawLines, drawPoints, mouseMoving, drawTexture;
	vec2						currentMousePosition;
};

void TransformFeedbackClothSimulationApp::setup()
{
	mIterationsPerFrame = 32;
	drawLines = drawPoints = drawTexture = 1;
	mouseMoving = mIterationIndex = 0;
	gl::viewport(0, 0, getWindowWidth()*2, getWindowHeight()*2);
	
	loadShaders();
	loadBuffers();
}

void TransformFeedbackClothSimulationApp::mouseDrag( MouseEvent event )
{
	mouseMoving = true;
	// Some mouse math to put the position somewhere in the flag
	currentMousePosition.x = lmap( (float)event.getPos().x, (float)0, (float)getWindowWidth(), (float)-34, (float)34 );
	currentMousePosition.y = lmap( (float)event.getPos().y, (float)0, (float)getWindowHeight(), (float)34, (float)-34 );
}

void TransformFeedbackClothSimulationApp::mouseUp( MouseEvent event )
{
	// reset the mouse position
	mouseMoving = true;
	currentMousePosition.x = 640;
	currentMousePosition.y = 480;
}

void TransformFeedbackClothSimulationApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'l' )
		drawLines = !drawLines;
	else if( event.getChar() == 'p' )
		drawPoints = !drawPoints;
	else if( event.getChar() == 't' )
		drawTexture = !drawTexture;
}

void TransformFeedbackClothSimulationApp::update()
{
	gl::ScopedGlslProg	updateScope( mUpdateGlsl );
	gl::ScopedState		stateScope( GL_RASTERIZER_DISCARD, true );
	
	// This for loop allows iteration on the gpu of solving the
	// physics of the cloth.
	// Change mIterationsPerFrame to see the difference it makes
	for( int i = mIterationsPerFrame; i != 0; --i ) {
		// Pick using the mouse if it's pressed
		if( mouseMoving ) {
			mUpdateGlsl->uniform( "mouse_pos", currentMousePosition );
			mouseMoving = false;
		}
		
		gl::ScopedVao			vaoScope( mVaos[mIterationIndex & 1] );
		gl::ScopedTextureBind	textureBind( mPosBufferTextures[mIterationIndex & 1]->getTarget(),
											mPosBufferTextures[mIterationIndex & 1]->getId() );
		
		mIterationIndex++;
		
		mFeedbackObjs[mIterationIndex & 1]->bind();
		
		gl::beginTransformFeedback( GL_POINTS );
		gl::drawArrays( GL_POINTS, 0, POINTS_TOTAL );
		gl::endTransformFeedback();
	}
}

void TransformFeedbackClothSimulationApp::draw()
{
	// clear out the window with black
	gl::clear();
		
	// Render the Line and/or Point version of the flag
	gl::ScopedGlslProg glslScope( mRenderGlsl );
	if( drawPoints ) {
		glPointSize( 4.0f );
		gl::drawArrays( GL_POINTS, 0, POINTS_TOTAL );
	}
	if( drawLines ) {
		gl::ScopedBuffer bufferScope( mLineElementBuffer );
		gl::drawElements( GL_LINES, mLineIndices, GL_UNSIGNED_INT, nullptr );
	}
}

void TransformFeedbackClothSimulationApp::loadShaders()
{
	// Create a vector of Transform Feedback "Varyings".
	// These strings tell OpenGL what to look for when capturing
	// Transform Feedback data. For instance, tf_position_mass,
	// and tf_velocity are variables in the update.vert that we
	// write our calculations to.
	std::vector<std::string> varyings(2);
	varyings[POSITION_INDEX] = "tf_position_mass";
	varyings[VELOCITY_INDEX] = "tf_velocity";
	
	gl::GlslProg::Format updateFormat;
	// Notice that we don't offer a fragment shader. We don't need
	// one because we're not trying to write pixels while updating
	// the position, velocity, etc. data to the screen.
	updateFormat.vertex( loadAsset( "update.vert" ) )
	// This option will be either GL_SEPARATE_ATTRIBS or GL_INTERLEAVED_ATTRIBS,
	// depending on the structure of our data, below. We're using multiple
	// buffers. Therefore, we're using GL_SEPERATE_ATTRIBS
		.feedbackFormat( GL_SEPARATE_ATTRIBS )
	// Pass the varyings to the glsl
		.feedbackVaryings( varyings )
		.attribLocation( "position_mass",	POSITION_INDEX )
		.attribLocation( "velocity",		VELOCITY_INDEX )
		.attribLocation( "connection",		CONNECTION_INDEX );
	
	try {
		mUpdateGlsl = gl::GlslProg::create( updateFormat );
	}
	catch( const gl::GlslProgCompileExc &ex ) {
		console() << "Update Shader failure: " << ex.what() << endl;
		quit();
	}
	
	mUpdateGlsl->uniform( "mouse_pos", vec2(640, 480) );
	
	gl::GlslProg::Format renderFormat;
	renderFormat.vertex( loadAsset( "render.vert" ) )
		.fragment( loadAsset( "render.frag" ) )
		.attribLocation( "position", POSITION_INDEX );
	
	try {
		mRenderGlsl = gl::GlslProg::create( renderFormat );
	}
	catch( const gl::GlslProgCompileExc &ex ) {
		console() << "Render Shader failure: " << ex.what() << endl;
		quit();
	}
}

void TransformFeedbackClothSimulationApp::loadBuffers()
{
	int n = 0;
	
	array<vec4, POINTS_TOTAL> positions;
	array<vec3, POINTS_TOTAL> velocities;
	array<ivec4, POINTS_TOTAL> connections;
	
	// We set all connections to -1, because these will only be updated
	// if there are connection indices. Explanation below.
	connections.fill( ivec4( -1 ) );
	
	for( int j = 0; j < POINTS_Y; j++ ) {
		float fj = (float)j / (float)POINTS_Y;
		for( int i = 0; i < POINTS_X; i++ ) {
			float fi = (float)i / (float)POINTS_X;
			
			// This fills the position buffer data, basically makes a grid
			positions[n] = vec4((fi - 0.5f) * (float)POINTS_X,	// x coordinate
								 (fj - 0.5f) * (float)POINTS_Y,	// y coordinate
								 0.6f * sinf(fi) * cosf(fj),	// z coordinate
								 1.0f);							// homogenous coordinate or w
			
			// This allows us to figure out the indices of the four points
			// surrounding the current point. This will be used to index
			// into the texture buffer.
			if( j != (POINTS_Y - 1) ) {	// if it's not one of the top row, don't move
				if( i != 0 )
					connections[n][0] = n - 1;
				if( j != 0 )
					connections[n][1] = n - POINTS_X;
				if( i != (POINTS_X - 1) )
					connections[n][2] = n + 1;
				if( j != (POINTS_Y - 1) )
					connections[n][3] = n + POINTS_X;
			}
			n++;
		}
	}
	
	// Create the Position Buffer with the intial position data
	mPositions[0] = gl::Vbo::create( GL_ARRAY_BUFFER, positions.size() * sizeof(vec4), positions.data(), GL_STATIC_DRAW );
	// Create another Position Buffer that is null, for ping-ponging
	mPositions[1] = gl::Vbo::create( GL_ARRAY_BUFFER, positions.size() * sizeof(vec4), nullptr, GL_STATIC_DRAW );
	
	// Create the Velocity Buffer with the intial velocity data
	mVelocities[0] = gl::Vbo::create( GL_ARRAY_BUFFER, velocities.size() * sizeof(vec3), velocities.data(), GL_STATIC_DRAW );
	// Create another Velocity Buffer that is null, for ping-ponging
	mVelocities[1] = gl::Vbo::create( GL_ARRAY_BUFFER, velocities.size() * sizeof(vec3), nullptr, GL_STATIC_DRAW );
	
	// Create Connection Buffer to index into the Texture Buffer
	mConnections = gl::Vbo::create( GL_ARRAY_BUFFER, connections.size() * sizeof(ivec4), connections.data(), GL_STATIC_DRAW );
	
	for( int i = 0; i < 2; i++ ) {
		// Initialize the Vao's holding the info for each buffer
		mVaos[i] = gl::Vao::create();
		
		// Bind the vao to capture index data for the glsl
		mVaos[i]->bind();
		mPositions[i]->bind();
		gl::vertexAttribPointer( POSITION_INDEX, 4, GL_FLOAT, GL_FALSE, 0, NULL );
		gl::enableVertexAttribArray( POSITION_INDEX );
		
		mVelocities[i]->bind();
		gl::vertexAttribPointer( VELOCITY_INDEX, 3, GL_FLOAT, GL_FALSE, 0, NULL );
		gl::enableVertexAttribArray( VELOCITY_INDEX );
		
		mConnections->bind();
		gl::vertexAttribIPointer( CONNECTION_INDEX, 4, GL_INT, 0, NULL );
		gl::enableVertexAttribArray( CONNECTION_INDEX );
		
		// Create a TransformFeedbackObj, which is similar to Vao
		// It's used to capture the output of a glsl and uses the
		// index of the feedback's varying variable names.
		mFeedbackObjs[i] = gl::TransformFeedbackObj::create();
		
		// Bind the TransformFeedbackObj and bind each corresponding buffer
		// to it's index.
		mFeedbackObjs[i]->bind();
		gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, POSITION_INDEX, mPositions[i] );
		gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, VELOCITY_INDEX, mVelocities[i] );
		mFeedbackObjs[i]->unbind();
		
		// Create Texture buffers to gain access to the lookup tables for
		// calculations in the update shader
		mPosBufferTextures[i] = gl::BufferTexture::create( mPositions[i], GL_RGBA32F );
	}
	
	// Create an element buffer to draw the lines (connections) between the points
	array<uint32_t, CONNECTIONS_TOTAL*2> lineIndices;
	uint32_t * e = lineIndices.data();
	for( int j = 0; j < POINTS_Y; j++ ) {
		for( int i = 0; i < POINTS_X - 1; i++ ) {
			*e++ = i + j * POINTS_X;
			*e++ = 1 + i + j * POINTS_Y;
		}
	}
	
	for( int i = 0; i < POINTS_X; i++ ) {
		for( int j = 0; j < POINTS_Y - 1; j++ ) {
			*e++ = i + j * POINTS_X;
			*e++ = POINTS_X + i + j * POINTS_X;
		}
	}
	
	mLineIndices = lineIndices.size();
	mLineElementBuffer = gl::Vbo::create( GL_ELEMENT_ARRAY_BUFFER, lineIndices.size() * sizeof(uint32_t), lineIndices.data(), GL_STATIC_DRAW );
}

CINDER_APP_NATIVE( TransformFeedbackClothSimulationApp, RendererGl )
