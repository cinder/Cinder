#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/TriMesh.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "cinder/ImageIo.h"

#include "cinder/gl/TransformFeedbackObj.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define STRINGIFY(A) #A

// This example is based on samples from the 10th chapter of the book, OpenGL 4.0
// Shading Language Cookbook. For more in-depth discussion of what is going on
// here please refer to that reference.

const int nParticles			= 4000;
const int PositionIndex			= 0;
const int VelocityIndex			= 1;
const int StartTimeIndex		= 2;
const int InitialVelocityIndex	= 3;

float mix( float x, float y, float a )
{
	return x * ( 1 - a ) + y * a;
}

class TransformFeedbackSmokeParticlesApp : public App {
  public:
	void setup();
	void mouseDown( MouseEvent event );
	void update();
	void draw();
	
	void loadBuffers();
	void loadShaders();
	void loadTexture();
	
  private:
	gl::VaoRef						mPVao[2];
	gl::VboRef						mPPositions[2], mPVelocities[2], mPStartTimes[2], mPInitVelocity;
	
	gl::GlslProgRef					mPUpdateGlsl, mPRenderGlsl;
	gl::TextureRef					mSmokeTexture;
	
	Rand							mRand;
	CameraPersp						mCam;
	TriMeshRef						mTrimesh;
	uint32_t						mDrawBuff;

	// this only works on desktop 
	#ifndef CINDER_EMSCRIPTEN
	gl::TransformFeedbackObjRef		mPFeedbackObj[2];
	#endif 
};

void TransformFeedbackSmokeParticlesApp::setup()
{
	mDrawBuff = 1;
	
	mCam.setPerspective( 60.0f, getWindowAspectRatio(), .01f, 1000.0f );
	mCam.lookAt( vec3( 0, 0, 10 ), vec3( 0, 0, 0 ) );
	
	loadTexture();
	loadShaders();
	loadBuffers();
}

void TransformFeedbackSmokeParticlesApp::loadTexture()
{
	gl::Texture::Format mTextureFormat;
	mTextureFormat.magFilter( GL_LINEAR ).minFilter( GL_LINEAR ).mipmap().internalFormat( GL_RGBA );
	mSmokeTexture = gl::Texture::create( loadImage( loadAsset( "smoke_blur.png" ) ), mTextureFormat );
}

void TransformFeedbackSmokeParticlesApp::loadShaders()
{
	try {
		// Create a vector of Transform Feedback "Varyings".
		// These strings tell OpenGL what to look for when capturing
		// Transform Feedback data. For instance, Position, Velocity,
		// and StartTime are variables in the updateSmoke.vert that we
		// write our calculations to.
		std::vector<std::string> transformFeedbackVaryings( 3 );
		transformFeedbackVaryings[PositionIndex] = "Position";
		transformFeedbackVaryings[VelocityIndex] = "Velocity";
		transformFeedbackVaryings[StartTimeIndex] = "StartTime";
		
		ci::gl::GlslProg::Format mUpdateParticleGlslFormat;
		// Notice that we don't offer a fragment shader. We don't need
		// one because we're not trying to write pixels while updating
		// the position, velocity, etc. data to the screen.
		mUpdateParticleGlslFormat.vertex( loadAsset( "updateSmoke.vert" ) )

		// in WebGL you need both shaders to make a complete program and are not allow to 
		// leave out one, even if you don't need it as in the case with Transform Feedback
		// Add a valid passthru shader.
		#ifdef CINDER_EMSCRIPTEN
		.fragment(STRINGIFY(
			precision highp float;
			out vec4 glFragColor;
			void main(){
				glFragColor = vec4(1.);
			}
		))
		#endif 

		// This option will be either GL_SEPARATE_ATTRIBS or GL_INTERLEAVED_ATTRIBS,
		// depending on the structure of our data, below. We're using multiple
		// buffers. Therefore, we're using GL_SEPERATE_ATTRIBS
			.feedbackFormat( GL_SEPARATE_ATTRIBS )
		// Pass the feedbackVaryings to glsl
			.feedbackVaryings( transformFeedbackVaryings )
			.attribLocation( "VertexPosition",			PositionIndex )
			.attribLocation( "VertexVelocity",			VelocityIndex )
			.attribLocation( "VertexStartTime",			StartTimeIndex )
			.attribLocation( "VertexInitialVelocity",	InitialVelocityIndex );
		
		mPUpdateGlsl = ci::gl::GlslProg::create( mUpdateParticleGlslFormat );
	}
	catch( const ci::gl::GlslProgCompileExc &ex ) {
		console() << "PARTICLE UPDATE GLSL ERROR: " << ex.what() << std::endl;
	}
	
	mPUpdateGlsl->uniform( "H", 1.0f / 60.0f );
	mPUpdateGlsl->uniform( "Accel", vec3( 0.0f ) );
	mPUpdateGlsl->uniform( "ParticleLifetime", 3.0f );
	
	try {
		ci::gl::GlslProg::Format mRenderParticleGlslFormat;
		// This being the render glsl, we provide a fragment shader.
		mRenderParticleGlslFormat.vertex( loadAsset( "renderSmoke.vert" ) )
			.fragment( loadAsset( "renderSmoke.frag" ) )
			.attribLocation("VertexPosition",			PositionIndex )
			.attribLocation( "VertexStartTime",			StartTimeIndex );
		
		mPRenderGlsl = ci::gl::GlslProg::create( mRenderParticleGlslFormat );
	}
	catch( const ci::gl::GlslProgCompileExc &ex ) {
		console() << "PARTICLE RENDER GLSL ERROR: " << ex.what() << std::endl;
	}
	
	mPRenderGlsl->uniform( "ParticleTex", 0 );
	mPRenderGlsl->uniform( "MinParticleSize", 1.0f );
	mPRenderGlsl->uniform( "MaxParticleSize", 64.0f );
	mPRenderGlsl->uniform( "ParticleLifetime", 3.0f );
}

void TransformFeedbackSmokeParticlesApp::loadBuffers()
{
	// Initialize positions
	std::vector<vec3> positions( nParticles, vec3( 0.0f ) );
	
	// Create Position Vbo with the initial position data
	mPPositions[0] = ci::gl::Vbo::create( GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), positions.data(), GL_STATIC_DRAW );
	// Create another Position Buffer that is null, for ping-ponging
	mPPositions[1] = ci::gl::Vbo::create( GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), nullptr, GL_STATIC_DRAW );
	
	// Reuse the positions vector that we've already made
	std::vector<vec3> normals = std::move( positions );
	
	for( auto normalIt = normals.begin(); normalIt != normals.end(); ++normalIt ) {
		// Creating a random velocity for each particle in a unit sphere
		*normalIt = ci::randVec3() * mix( 0.0f, 1.5f, mRand.nextFloat() );
	}
	
	// Create the Velocity Buffer using the newly buffered velocities
	mPVelocities[0] = ci::gl::Vbo::create( GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), normals.data(), GL_STATIC_DRAW );
	// Create another Velocity Buffer that is null, for ping-ponging
	mPVelocities[1] = ci::gl::Vbo::create( GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), nullptr, GL_STATIC_DRAW );
	// Create an initial velocity buffer, so that you can reset a particle's velocity after it's dead
	mPInitVelocity = ci::gl::Vbo::create( GL_ARRAY_BUFFER,	normals.size() * sizeof(vec3), normals.data(), GL_STATIC_DRAW );
	
	// Create time data for the initialization of the particles
	array<GLfloat, nParticles> timeData;
	float time = 0.0f;
	float rate = 0.001f;
	for( int i = 0; i < nParticles; i++ ) {
		timeData[i] = time;
		time += rate;
	}

	// Create the StartTime Buffer, so that we can reset the particle after it's dead
	mPStartTimes[0] = ci::gl::Vbo::create( GL_ARRAY_BUFFER, timeData.size() * sizeof( float ), timeData.data(), GL_DYNAMIC_COPY );
	// Create the StartTime ping-pong buffer
	mPStartTimes[1] = ci::gl::Vbo::create( GL_ARRAY_BUFFER, nParticles * sizeof( float ), nullptr, GL_DYNAMIC_COPY );
	
	for( int i = 0; i < 2; i++ ) {
		// Initialize the Vao's holding the info for each buffer
		mPVao[i] = ci::gl::Vao::create();
		
		// Bind the vao to capture index data for the glsl
		mPVao[i]->bind();
		mPPositions[i]->bind();
		ci::gl::vertexAttribPointer( PositionIndex, 3, GL_FLOAT, GL_FALSE, 0, 0 );
		ci::gl::enableVertexAttribArray( PositionIndex );
		
		mPVelocities[i]->bind();
		ci::gl::vertexAttribPointer( VelocityIndex, 3, GL_FLOAT, GL_FALSE, 0, 0 );
		ci::gl::enableVertexAttribArray( VelocityIndex );
		
		mPStartTimes[i]->bind();
		ci::gl::vertexAttribPointer( StartTimeIndex, 1, GL_FLOAT, GL_FALSE, 0, 0 );
		ci::gl::enableVertexAttribArray( StartTimeIndex );
		
		mPInitVelocity->bind();
		ci::gl::vertexAttribPointer( InitialVelocityIndex, 3, GL_FLOAT, GL_FALSE, 0, 0 );
		ci::gl::enableVertexAttribArray( InitialVelocityIndex );
		
		// Create a TransformFeedbackObj, which is similar to Vao
		// It's used to capture the output of a glsl and uses the
		// index of the feedback's varying variable names.
		#ifndef CINDER_EMSCRIPTEN
		mPFeedbackObj[i] = gl::TransformFeedbackObj::create();
		#endif 
		
		// Bind the TransformFeedbackObj and bind each corresponding buffer
		// to it's index.
		#ifndef CINDER_EMSCRIPTEN
		mPFeedbackObj[i]->bind();
		gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, PositionIndex, mPPositions[i] );
		gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, VelocityIndex, mPVelocities[i] );
		gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, StartTimeIndex, mPStartTimes[i] );
		mPFeedbackObj[i]->unbind();
		#endif 
	}
}

void TransformFeedbackSmokeParticlesApp::mouseDown( MouseEvent event )
{
}

void TransformFeedbackSmokeParticlesApp::update()
{
	// This equation just reliably swaps all concerned buffers
	mDrawBuff = 1 - mDrawBuff;
	
	gl::ScopedGlslProg	glslScope( mPUpdateGlsl );
	// We use this vao for input to the Glsl, while using the opposite
	// for the TransformFeedbackObj.
	gl::ScopedVao		vaoScope( mPVao[mDrawBuff] );
	// Because we're not using a fragment shader, we need to
	// stop the rasterizer. This will make sure that OpenGL won't
	// move to the rasterization stage.
	gl::ScopedState		stateScope( GL_RASTERIZER_DISCARD, true );
	
	mPUpdateGlsl->uniform( "Time", getElapsedFrames() / 60.0f );
	
	#ifndef CINDER_EMSCRIPTEN
	// Opposite TransformFeedbackObj to catch the calculated values
	// In the opposite buffer
	mPFeedbackObj[1-mDrawBuff]->bind();

	#elif defined( CINDER_EMSCRIPTEN )
	
	gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, PositionIndex, mPPositions[1 - mDrawBuff] );
	gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, VelocityIndex, mPVelocities[1 - mDrawBuff] );
	gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, StartTimeIndex, mPStartTimes[1 - mDrawBuff] );
	
	#endif
	
	// We begin Transform Feedback, using the same primitive that
	// we're "drawing". Using points for the particle system.
	gl::beginTransformFeedback( GL_POINTS );
	gl::drawArrays( GL_POINTS, 0, nParticles );
	gl::endTransformFeedback();	


	// in WebGL - you need to remember to unbind buffer positions, things aren't automatically unbound.
	// TransformFeedbackObj doesn't support unbinding for some reason so doing this manually. 
	#ifdef CINDER_EMSCRIPTEN
	gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, PositionIndex, nullptr );
	gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, VelocityIndex, nullptr );
	gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, StartTimeIndex, nullptr );
	#endif 

	
}

void TransformFeedbackSmokeParticlesApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
	static float rotateRadians = 0.0f;
	rotateRadians += 0.01f;
	
	gl::ScopedVao			vaoScope( mPVao[1-mDrawBuff] );
	gl::ScopedGlslProg		glslScope( mPRenderGlsl );
	gl::ScopedTextureBind	texScope( mSmokeTexture );

	// no need to enable point size, automatically utilized in WebGL if you call gl_PointSize in shader.
#ifndef CINDER_EMSCRIPTEN
	gl::ScopedState			stateScope( GL_PROGRAM_POINT_SIZE, true );
#endif 

	gl::ScopedBlend			blendScope( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	gl::pushMatrices();
	gl::setMatrices( mCam );
	gl::multModelMatrix( rotate( rotateRadians, vec3( 0, 1, 0 ) ) );
	
	mPRenderGlsl->uniform( "Time", getElapsedFrames() / 60.0f );
	gl::setDefaultShaderVars();
	gl::drawArrays( GL_POINTS, 0, nParticles );
	
	gl::popMatrices();
	
}

CINDER_APP( TransformFeedbackSmokeParticlesApp, RendererGl )
