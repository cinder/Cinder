#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/gl/Vao.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/GlslProg.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// Vertex shader
const GLchar* vertexShaderSrc = CI_GLSL( 150,
									 in float inValue;
									 out float outValue;
									 
									 void main() {
										 outValue = sqrt(inValue);
									 }
									 );


class TransformFeedbackIntroApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );
	void update();
	void draw();
	
	void glOriginalWay();
	
	void setupShaders();
	void setupBuffers();
	
	gl::GlslProgRef mGlsl;
	gl::VaoRef		mVao;
	gl::VboRef		mInitialVbo, mTransformVbo;
};

void TransformFeedbackIntroApp::setup()
{
	bool usingGl = false;
	
	if( usingGl ) {
		glOriginalWay();
	}
	else {
		setupShaders();
		setupBuffers();
	}
}

void TransformFeedbackIntroApp::glOriginalWay()
{
	// Compile shader
    GLuint shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader, 1, &vertexShaderSrc, nullptr);
    glCompileShader(shader);
	
    // Create program and specify transform feedback variables
    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
	
    const GLchar* feedbackVaryings[] = { "outValue" };
    glTransformFeedbackVaryings(program, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
	
    glLinkProgram(program);
    glUseProgram(program);
	
    // Create VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
	
    // Create input VBO and vertex format
    GLfloat data[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
	
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	
    GLint inputAttrib = glGetAttribLocation(program, "inValue");
    glEnableVertexAttribArray(inputAttrib);
    glVertexAttribPointer(inputAttrib, 1, GL_FLOAT, GL_FALSE, 0, 0);
	
    // Create transform feedback buffer
    GLuint xbo;
    glGenBuffers(1, &xbo);
    glBindBuffer(GL_ARRAY_BUFFER, xbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), nullptr, GL_STATIC_READ);
	
    // Perform feedback transform
    glEnable(GL_RASTERIZER_DISCARD);
	
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, xbo);
	
    glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, 5);
    glEndTransformFeedback();
	
    glDisable(GL_RASTERIZER_DISCARD);
	
    glFlush();
	
    // Fetch and print results
    GLfloat feedback[5];
    glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);
	
    console() << feedback[0] << " " <<  feedback[1] << " " << feedback[2] << " " << feedback[3] << " " << feedback[4] << std::endl;
	
	quit();
}

void TransformFeedbackIntroApp::setupShaders()
{
	vector<std::string> varyings;
	varyings.push_back( "outValue" );
	gl::GlslProg::Format mFormat = gl::GlslProg::Format();
								mFormat.vertex( loadAsset( "basicTransformFeedback.vert" ) );
								mFormat.feedbackVaryings( varyings ).feedbackFormat( GL_INTERLEAVED_ATTRIBS );
	try {
		mGlsl = gl::GlslProg::create( mFormat );
		mGlsl->bind();
	}
	catch( const gl::GlslProgCompileExc &ex ) {
		console() << ex.what() << endl;
		shutdown();
	}
}

void TransformFeedbackIntroApp::setupBuffers()
{
	mVao = gl::Vao::create();
	mVao->bind();

	float data[5] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
	
	mInitialVbo = gl::Vbo::create( GL_ARRAY_BUFFER, sizeof(float) * 5, data, GL_STATIC_DRAW );
	mInitialVbo->bind();
	
	GLint inputAttrib = glGetAttribLocation( mGlsl->getHandle(), "inValue");
	gl::enableVertexAttribArray( inputAttrib );
	gl::vertexAttribPointer( inputAttrib, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0 );
	
	mTransformVbo = gl::Vbo::create( GL_ARRAY_BUFFER, sizeof(float) * 5, nullptr, GL_STATIC_READ );
	
	// Bind the transform vbo to the first index binding point to receive the transform feedback
	gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, mTransformVbo );
	
	gl::enable( GL_RASTERIZER_DISCARD );
	
	gl::beginTransformFeedback( GL_POINTS );
	gl::drawArrays( GL_POINTS, 0, 5 );
	gl::endTransformFeedback();
	
	gl::disable( GL_RASTERIZER_DISCARD );
	
    glFlush();
	
    // Fetch and print results
    GLfloat feedback[5];
	mTransformVbo->getBufferSubData( 0, sizeof(float) * 5, feedback );
	
    console() << feedback[0] << " " <<  feedback[1] << " " << feedback[2] << " " << feedback[3] << " " << feedback[4] << std::endl;
	
	quit();
}

void TransformFeedbackIntroApp::mouseDown( MouseEvent event )
{
}

void TransformFeedbackIntroApp::update()
{
}

void TransformFeedbackIntroApp::draw()
{
	gl::clear();
}

CINDER_APP_NATIVE( TransformFeedbackIntroApp, RendererGl )
