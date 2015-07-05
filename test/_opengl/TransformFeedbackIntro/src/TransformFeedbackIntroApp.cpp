#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

#include "cinder/gl/Vao.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/GlslProg.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// This sample is based off a sample from the website http://open.gl/feedback .
// Read the tutorial for a more in-depth look at what is happening behind the scenes.

const uint32_t	TransformIndex = 0;
const uint32_t	NumVertices = 5;

class TransformFeedbackIntroApp : public App {
  public:
	void setup();
	
	void setupShaders();
	void setupBuffers();
	void runTransformFeedback();
	
	gl::GlslProgRef mGlsl;
	gl::VaoRef		mVao;
	gl::VboRef		mInitialVbo, mCaptureVbo;
};

void TransformFeedbackIntroApp::setup()
{
	setupShaders();
	setupBuffers();
	runTransformFeedback();
	quit();
}

void TransformFeedbackIntroApp::setupShaders()
{
	try {
		// We create a vector of "Varyings" to let OpenGl know what we plan
		// on capturing, "outValue" in this case
		vector<std::string> varyings( 1 );
		varyings[TransformIndex] = "outValue";
		
		// Take note that we are omitting a fragment shader, because we're
		// employing GL_RASTERIZER_DISCARD state when dealing with TF.
		// Also, our shader simply takes in one float and does a sqrt and
		// writes the value out to our feedbackVarying, outValue.
		mGlsl = gl::GlslProg::create( gl::GlslProg::Format()
									 .vertex( "#version 150 core\n"
											 "\n"
											 "in float inValue;\n"
											 "out float outValue;\n"
											 "\n"
											 "void main() {\n"
											 "	 outValue = sqrt(inValue);\n"
											 "}" )
									 .attribLocation( "inValue", TransformIndex )
									 .feedbackVaryings( varyings )
									 .feedbackFormat( GL_INTERLEAVED_ATTRIBS ) );
	}
	catch( const gl::GlslProgCompileExc &exc ) {
		CI_LOG_EXCEPTION( "failed to load glsl", exc );
		quit();
	}
}

void TransformFeedbackIntroApp::setupBuffers()
{
	mVao = gl::Vao::create();
	
	float data[NumVertices] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
	
	// Create the initial data Vbo
	mInitialVbo = gl::Vbo::create( GL_ARRAY_BUFFER, sizeof(float) * NumVertices, data, GL_STATIC_DRAW );
	// Create the capture Vbo, to capture the caluculated values from the vertex shader
	mCaptureVbo = gl::Vbo::create( GL_ARRAY_BUFFER, sizeof(float) * NumVertices, nullptr, GL_STATIC_READ );
	
	gl::ScopedVao		scopeVao( mVao );
	gl::ScopedBuffer	scopeBuffer( mInitialVbo );
	gl::enableVertexAttribArray( TransformIndex );
	gl::vertexAttribPointer( TransformIndex, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0 );
}

void TransformFeedbackIntroApp::runTransformFeedback()
{
	gl::ScopedVao		scopeVao( mVao );
	gl::ScopedGlslProg	scopeGlsl( mGlsl );
	gl::ScopedState		scopeState( GL_RASTERIZER_DISCARD, true );
	// Bind the capture vbo to the TransformVertex binding point to capture the calculations
	gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, TransformIndex, mCaptureVbo );
	
	// Begin Transform Feedback, using the primitive we plan to draw
	gl::beginTransformFeedback( GL_POINTS );
	gl::drawArrays( GL_POINTS, 0, NumVertices );
	// End Transform Feedback
	gl::endTransformFeedback();
	
    glFlush();
	
    // Fetch and print results
    GLfloat feedback[NumVertices];
	mCaptureVbo->getBufferSubData( 0, sizeof(float) * NumVertices, feedback );
	
    console() << feedback[0] << " "
				<< feedback[1] << " "
				<< feedback[2] << " "
				<< feedback[3] << " "
				<< feedback[4] << std::endl;
}

CINDER_APP( TransformFeedbackIntroApp, RendererGl )
