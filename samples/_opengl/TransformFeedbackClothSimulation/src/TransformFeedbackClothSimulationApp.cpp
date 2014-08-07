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

typedef std::vector<std::pair<gl::VaoRef, std::vector<gl::VboRef>>> PingPongBuffers;

enum BUFFER_TYPE {
	POSITION = 0,
	VELOCITY = 1,
	CONNECTION = 2
};

enum
{
	POINTS_X			= 50,
	POINTS_Y			= 50,
	POINTS_TOTAL		= (POINTS_X * POINTS_Y),
	CONNECTIONS_TOTAL	= (POINTS_X - 1) * POINTS_Y + (POINTS_Y - 1) * POINTS_X
};

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
	PingPongBuffers				mBuffers;
	gl::TransformFeedbackObjRef mFeedbackObjs[2];
	gl::VboRef					mElementBuffer;
	gl::BufferTextureRef		mPosBufferTextures[2];
	int							mIterationsPerFrame, mIterationIndex;
	bool						drawLines, drawPoints, mouseMoving;
	Vec2f						currentMousePosition;
};

void TransformFeedbackClothSimulationApp::setup()
{
	mIterationsPerFrame = 32;
	drawLines = drawPoints = 1;
	mouseMoving = mIterationIndex = 0;
	gl::viewport(0, 0, getWindowWidth()*2, getWindowHeight()*2);
	
	loadShaders();
	loadBuffers();
}

void TransformFeedbackClothSimulationApp::mouseDrag( MouseEvent event )
{
	mouseMoving = true;
	float x = lmap( (float)event.getPos().x, (float)0, (float)getWindowWidth(), (float)-34, (float)34 );
	float y = lmap( (float)event.getPos().y, (float)0, (float)getWindowHeight(), (float)34, (float)-34 );
	currentMousePosition.set(x, y);
}

void TransformFeedbackClothSimulationApp::mouseUp( MouseEvent event )
{
	currentMousePosition.set(640, 480);
}

void TransformFeedbackClothSimulationApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'r' )
		loadShaders();
	else if( event.getChar() == 'l' )
		drawLines = !drawLines;
	else if( event.getChar() == 'p' )
		drawPoints = !drawPoints;
}

void TransformFeedbackClothSimulationApp::update()
{
	int i;
	
	mUpdateGlsl->bind();
	
	gl::enable( GL_RASTERIZER_DISCARD );
	
	for( i = mIterationsPerFrame; i != 0; --i ) {	
		if( mouseMoving ) {
			mUpdateGlsl->uniform("mouse_pos", currentMousePosition );
			mouseMoving = false;
		}
		
		mBuffers[mIterationIndex & 1].first->bind();
		mPosBufferTextures[mIterationIndex & 1]->bindTexture();
		
		mIterationIndex++;
		
		mFeedbackObjs[mIterationIndex & 1]->bind();
		
		gl::beginTransformFeedback( GL_POINTS );
		gl::drawArrays( GL_POINTS, 0, POINTS_TOTAL );
		gl::endTransformFeedback();
	}
	
	gl::disable( GL_RASTERIZER_DISCARD );
}

void TransformFeedbackClothSimulationApp::draw()
{
	// clear out the window with black
	gl::clear();
	
	mRenderGlsl->bind();
	
	if( drawPoints ) {
		glPointSize(4.0f);
		gl::drawArrays(GL_POINTS, 0, POINTS_TOTAL);
	}
	if( drawLines ) {
		mElementBuffer->bind();
		gl::drawElements( GL_LINES, CONNECTIONS_TOTAL * 2, GL_UNSIGNED_INT, NULL );
	}
}

void TransformFeedbackClothSimulationApp::loadShaders()
{
	std::vector<std::string> tf_varyings;
	tf_varyings.push_back( "tf_position_mass" );
	tf_varyings.push_back( "tf_velocity" );
	
	gl::GlslProg::Format updateFormat;
	updateFormat.vertex( loadAsset( "update.vert" ) )
		.feedbackFormat( GL_SEPARATE_ATTRIBS )
		.feedbackVaryings( tf_varyings )
		.attribLocation( "position_mass", 0 )
		.attribLocation( "velocity", 1 )
		.attribLocation( "connection", 2 );
	
	try {
		if( mUpdateGlsl ) mUpdateGlsl.reset();
		mUpdateGlsl = gl::GlslProg::create(updateFormat);
	}
	catch( const gl::GlslNullProgramExc &ex ) {
		console() << "Update Shader failure: " << ex.what() << endl;
		quit();
	}
	catch( const gl::GlslProgCompileExc &ex ) {
		console() << "Update Shader failure: " << ex.what() << endl;
		quit();
	}
	
	gl::GlslProg::Format renderFormat;
	renderFormat.vertex( loadAsset( "render.vert" ) )
		.fragment( loadAsset( "render.frag" ) )
		.attribLocation( "position", 0 );
	
	try {
		if( mRenderGlsl ) mRenderGlsl.reset();
		mRenderGlsl = gl::GlslProg::create(renderFormat);
	}
	catch( const gl::GlslProgCompileExc &ex ) {
		console() << "Render Shader failure: " << ex.what() << endl;
		quit();
	}
}

void TransformFeedbackClothSimulationApp::loadBuffers()
{
	int i, j, n = 0;
	
	Vec4f * initialPositions = new Vec4f[POINTS_TOTAL];
	Vec3f * initialVelocities = new Vec3f[POINTS_TOTAL];
	Vec4i * connectionVectors = new	Vec4i[POINTS_TOTAL];
	
	for( j = 0; j < POINTS_Y; j++ ) {
		float fj = (float)j / (float)POINTS_Y;
		for( i = 0; i < POINTS_X; i++ ) {
			float fi = (float)i / (float)POINTS_X;
			
			initialPositions[n] = Vec4f((fi - 0.5f) * (float)POINTS_X, // x coordinate
										(fj - 0.5f) * (float)POINTS_Y, // y coordinate
										0.6f * sinf(fi) * cosf(fj),	   // z coordinate
										1.0f);						   // homogenous coordinate or w
			initialVelocities[n] = Vec3f(0.0f, 0.0f, 0.0f);
			connectionVectors[n] = Vec4i(-1, -1, -1, -1);
			
			if( j != (POINTS_Y - 1) ) {	// if it's not one of the top row which don't move
				if( i != 0 )
					connectionVectors[n][0] = n - 1;
				if( j != 0 )
					connectionVectors[n][1] = n - POINTS_X;
				if( i != (POINTS_X - 1) )
					connectionVectors[n][2] = n + 1;
				if( j != (POINTS_Y - 1) )
					connectionVectors[n][3] = n + POINTS_X;
			}
			n++;
		}
	}
	
	mBuffers.resize(2);
	
	for( i = 0; i < 2; i++ ) {
		mBuffers[i].first = gl::Vao::create();
		mBuffers[i].first->bind();
		
		mBuffers[i].second.resize(3);
		mBuffers[i].second[POSITION] = gl::Vbo::create( GL_ARRAY_BUFFER, POINTS_TOTAL * sizeof(Vec4f), initialPositions, GL_DYNAMIC_COPY );
		mBuffers[i].second[POSITION]->bind();
		gl::vertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 0, NULL );
		gl::enableVertexAttribArray(0);
		
		mBuffers[i].second[VELOCITY] = gl::Vbo::create( GL_ARRAY_BUFFER, POINTS_TOTAL * sizeof(Vec3f), initialVelocities, GL_DYNAMIC_COPY );
		mBuffers[i].second[VELOCITY]->bind();
		gl::vertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
		gl::enableVertexAttribArray(1);
		
		if( i < 1 ) {
			mBuffers[i].second[CONNECTION] = gl::Vbo::create( GL_ARRAY_BUFFER, POINTS_TOTAL * sizeof(Vec4i), connectionVectors, GL_STATIC_DRAW );
			mBuffers[i].second[CONNECTION]->bind();
			gl::vertexAttribIPointer( 2, 4, GL_INT, 0, NULL );
			gl::enableVertexAttribArray(2);
		}
		else {
			mBuffers[i].second[CONNECTION] = mBuffers[0].second[2];
			mBuffers[i].second[CONNECTION]->bind();
			gl::vertexAttribIPointer( 2, 4, GL_INT, 0, NULL );
			gl::enableVertexAttribArray(2);
		}
		
	}
	
	delete [] initialPositions;
	delete [] initialVelocities;
	delete [] connectionVectors;
	
	// Create Transform Feedback Buffer Objects to hold the state of these two buffers, position and velocity
	mFeedbackObjs[0] = gl::TransformFeedbackObj::create();
	
	mFeedbackObjs[0]->bind();
	gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, mBuffers[0].second[POSITION] );
	gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 1, mBuffers[0].second[VELOCITY] );
	mFeedbackObjs[0]->unbind();
	
	mFeedbackObjs[1] = gl::TransformFeedbackObj::create();
	
	mFeedbackObjs[1]->bind();
	gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, mBuffers[1].second[POSITION] );
	gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 1, mBuffers[1].second[VELOCITY] );
	mFeedbackObjs[1]->unbind();
	
	// Create Texture buffers to be given lookup tables for calculations in the update shader
	mPosBufferTextures[0] = gl::BufferTexture::create( mBuffers[0].second[POSITION], GL_RGBA32F );
	mPosBufferTextures[1] = gl::BufferTexture::create( mBuffers[1].second[POSITION], GL_RGBA32F );
	
	int lines = (POINTS_X - 1) * POINTS_Y + (POINTS_Y - 1) * POINTS_X;
	
	mElementBuffer = gl::Vbo::create( GL_ELEMENT_ARRAY_BUFFER, lines * 2 * sizeof(int), NULL, GL_STATIC_DRAW );
	mElementBuffer->bind();
	
	int * e = (int *)mElementBuffer->mapBufferRange( 0, lines * 2 * sizeof(int), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT );
	
	for( j = 0; j < POINTS_Y; j++ ) {
		for( i = 0; i < POINTS_X - 1; i++ ) {
			*e++ = i + j * POINTS_X;
			*e++ = 1 + i + j * POINTS_Y;
		}
	}
	
	for( i = 0; i < POINTS_X; i++ ) {
		for( j = 0; j < POINTS_Y - 1; j++ ) {
			*e++ = i + j * POINTS_X;
			*e++ = POINTS_X + i + j * POINTS_X;
		}
	}
	mElementBuffer->unmap();
}

CINDER_APP_NATIVE( TransformFeedbackClothSimulationApp, RendererGl )
