#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/gl/Context.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/Vbo.h"

#include "cinder/TriMesh.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "cinder/ImageIo.h"

#include "cinder/gl/TransformFeedbackObj.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const int nParticles = 4000;

float mix( float x, float y, float a )
{
	return x * ( 1 - a ) + y * a;
}

class TransformFeedbackSmokeParticlesApp : public AppNative {
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
	gl::TransformFeedbackObjRef		mPFeedbackObj[2];
	gl::VboRef						mPPositions[2], mPVelocities[2], mPStartTimes[2], mPInitVelocity;
	
	gl::GlslProgRef					mPUpdateGlsl, mPRenderGlsl;
	gl::TextureRef					mSmokeTexture;
	
	Rand							mRand;
	CameraPersp						mCam;
	TriMeshRef						mTrimesh;
	uint32_t						mDrawBuff;
};

void TransformFeedbackSmokeParticlesApp::setup()
{
	mDrawBuff = 1;
	
	mCam.setPerspective( 60.0f, getWindowAspectRatio(), .01f, 1000.0f );
	mCam.lookAt( Vec3f( 0, 0, 10 ), Vec3f( 0, 0, 0 ) );
	
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
		std::vector<std::string> transformFeedbackVaryings;
		transformFeedbackVaryings.push_back( "Position" );
		transformFeedbackVaryings.push_back( "Velocity" );
		transformFeedbackVaryings.push_back( "StartTime" );
		
		ci::gl::GlslProg::Format mUpdateParticleGlslFormat;
		mUpdateParticleGlslFormat.vertex( loadAsset( "updateSmoke.vert" ) )
			.feedbackFormat( GL_SEPARATE_ATTRIBS )
			.feedbackVaryings( transformFeedbackVaryings )
			.attribLocation( "VertexPosition", 0 )
			.attribLocation( "VertexVelocity", 1 )
			.attribLocation( "VertexStartTime", 2 )
			.attribLocation( "VertexInitialVelocity", 3 );
		
		mPUpdateGlsl = ci::gl::GlslProg::create( mUpdateParticleGlslFormat );
	}
	catch( const ci::gl::GlslProgCompileExc &ex ) {
		console() << "PARTICLE UPDATE GLSL ERROR: " << ex.what() << std::endl;
	}
	
	mPUpdateGlsl->uniform( "H", 1.0f / 60.0f );
	mPUpdateGlsl->uniform( "Accel", Vec3f( 0, 0, 0 ) );
	mPUpdateGlsl->uniform( "ParticleLifetime", 3.0f );
	
	try {
		ci::gl::GlslProg::Format mRenderParticleGlslFormat;
		mRenderParticleGlslFormat.vertex( loadAsset( "renderSmoke.vert" ) )
			.fragment( loadAsset( "renderSmoke.frag" ) )
			.attribLocation("VertexPosition", 0 )
			.attribLocation( "VertexVelocity", 1 )
			.attribLocation( "VertexStartTime", 2 )
			.attribLocation( "VertexInitialVelocity", 3 );
		
		mPRenderGlsl = ci::gl::GlslProg::create( mRenderParticleGlslFormat );
	}
	catch( const ci::gl::GlslProgCompileExc &ex ) {
		console() << "PARTICLE RENDER GLSL ERROR: " << ex.what() << std::endl;
	}
	
	mPRenderGlsl->uniform( "ParticleTex", 0 );
	mPRenderGlsl->uniform( "MinParticleSize", 1.0f );
	mPRenderGlsl->uniform( "MaxParticleSize", 64.0f );
	mPRenderGlsl->uniform( "ParticleLifetime", 3.0f );}

void TransformFeedbackSmokeParticlesApp::loadBuffers()
{
	ci::TriMesh::Format mTrimeshFormat;
	mTrimeshFormat.positions(3).normals();
	mTrimesh = ci::TriMesh::create( mTrimeshFormat );
	
	// BUFFER FIRST POSITION DATA
	for( int i = 0; i < nParticles; i++ )
		mTrimesh->appendVertex( ci::Vec3f( 0.0f, 0.0f, 0.0f ) );
	
	mPPositions[0] = ci::gl::Vbo::create( GL_ARRAY_BUFFER, mTrimesh->getNumVertices() * sizeof(ci::Vec3f), mTrimesh->getVertices<3>(), GL_DYNAMIC_COPY );
	mPPositions[1] = ci::gl::Vbo::create( GL_ARRAY_BUFFER, mTrimesh->getNumVertices() * sizeof(ci::Vec3f), nullptr, GL_DYNAMIC_COPY );
	
	// BUFFER FIRST VELOCITY DATA
	ci::Vec3f v(0.0f, 0.0f, 0.0f);
	float velocity, theta, phi;
	
	for( int i = 0; i < nParticles; i++ ) {
		theta = mix( 0.0f, (float)M_PI / 6.0f, mRand.nextFloat() );
		phi = mix( 0.0f, (float)(2 * M_PI), mRand.nextFloat() );
		
		float angle = mRand.nextFloat( 0.0f, M_PI * 2.0f );
		v.z = mRand.nextFloat( -1.0f, 1.0 );
		v.x = sqrt( 1 - v.z * v.z ) * cos( angle );
		v.y = sqrt( 1 - v.z * v.z ) * sin( angle );
		
		velocity = mix( 0.0f, 1.5f, mRand.nextFloat() );
		v = v.normalized() * velocity;
		mTrimesh->appendNormal( v );
	}
	
	mPVelocities[0] = ci::gl::Vbo::create( GL_ARRAY_BUFFER, mTrimesh->getNormals().size() * sizeof(ci::Vec3f), mTrimesh->getNormals().data(), GL_DYNAMIC_COPY );
	mPVelocities[1] = ci::gl::Vbo::create( GL_ARRAY_BUFFER, mTrimesh->getNormals().size() * sizeof(ci::Vec3f), nullptr, GL_DYNAMIC_COPY );
	mPInitVelocity = ci::gl::Vbo::create( GL_ARRAY_BUFFER, mTrimesh->getNormals().size() * sizeof(ci::Vec3f), mTrimesh->getNormals().data(), GL_STATIC_DRAW );
	
	// BUFFER START TIMES
	GLfloat * timeData = new GLfloat[nParticles];
	float time = 0.0f;
	float rate = 0.001f;
	for( int i = 0; i < nParticles; i++ ) {
		timeData[i] = time;
		time += rate;
	}
	mPStartTimes[0] = ci::gl::Vbo::create( GL_ARRAY_BUFFER, nParticles * sizeof( float ), timeData, GL_DYNAMIC_COPY );
	mPStartTimes[1] = ci::gl::Vbo::create( GL_ARRAY_BUFFER, nParticles * sizeof( float ), nullptr, GL_DYNAMIC_COPY );
	
	delete [] timeData;
	
	mPVao[0] = ci::gl::Vao::create();
	mPVao[1] = ci::gl::Vao::create();
	
	// INITIALIZE THE FIRST VAO
	mPVao[0]->bind();
	mPPositions[0]->bind();
	ci::gl::vertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
	ci::gl::enableVertexAttribArray( 0 );
	
	mPVelocities[0]->bind();
	ci::gl::vertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );
	ci::gl::enableVertexAttribArray( 1 );
	
	mPStartTimes[0]->bind();
	ci::gl::vertexAttribPointer( 2, 1, GL_FLOAT, GL_FALSE, 0, 0 );
	ci::gl::enableVertexAttribArray( 2 );
	
	mPInitVelocity->bind();
	ci::gl::vertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, 0, 0 );
	ci::gl::enableVertexAttribArray( 3 );
	
	// INITIALIZE THE SECOND VAO
	mPVao[1]->bind();
	mPPositions[1]->bind();
	ci::gl::vertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
	ci::gl::enableVertexAttribArray( 0 );
	
	mPVelocities[1]->bind();
	ci::gl::vertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );
	ci::gl::enableVertexAttribArray( 1 );
	
	mPStartTimes[1]->bind();
	ci::gl::vertexAttribPointer( 2, 1, GL_FLOAT, GL_FALSE, 0, 0 );
	ci::gl::enableVertexAttribArray( 2 );
	
	mPInitVelocity->bind();
	ci::gl::vertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, 0, 0 );
	ci::gl::enableVertexAttribArray( 3 );
	
	// Creating the TransformFeedbackObj's
	mPFeedbackObj[0] = gl::TransformFeedbackObj::create();
	mPFeedbackObj[1] = gl::TransformFeedbackObj::create();
	
	mPFeedbackObj[0]->bind();
	gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, mPPositions[0] );
	gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 1, mPVelocities[0] );
	gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 2, mPStartTimes[0] );
	mPFeedbackObj[0]->unbind();
	
	mPFeedbackObj[1]->bind();
	gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, mPPositions[1] );
	gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 1, mPVelocities[1] );
	gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 2, mPStartTimes[1] );
	mPFeedbackObj[1]->unbind();
}

void TransformFeedbackSmokeParticlesApp::mouseDown( MouseEvent event )
{
}

void TransformFeedbackSmokeParticlesApp::update()
{
	mDrawBuff = 1 - mDrawBuff;
	
	gl::ScopedGlslProg	mGlslScope( mPUpdateGlsl );
	gl::ScopedVao		mVaoScope( mPVao[mDrawBuff] );
	gl::ScopedState		mStateScope( GL_RASTERIZER_DISCARD, true );
	
	mPUpdateGlsl->uniform( "Time", getElapsedFrames() / 60.0f );
	
	mPFeedbackObj[1-mDrawBuff]->bind();
	
	gl::beginTransformFeedback( GL_POINTS );
	gl::drawArrays( GL_POINTS, 0, nParticles );
	gl::endTransformFeedback();	
}

void TransformFeedbackSmokeParticlesApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
	static float rotateRadians = 0.0f;
	rotateRadians += 0.01f;
	
	gl::ScopedVao mVaoScope( mPVao[1-mDrawBuff] );
	gl::ScopedGlslProg mGlslScope( mPRenderGlsl );
	gl::ScopedTextureBind mTexScope( mSmokeTexture );
	gl::ScopedState mStateScope( GL_PROGRAM_POINT_SIZE, true );
	gl::ScopedBlend mBlendScope( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	gl::pushMatrices();
	gl::setMatrices( mCam );
	gl::multModelMatrix( Matrix44f::createRotation( Vec3f( 0, 1, 0 ), rotateRadians ) );
	
	mPRenderGlsl->uniform( "Time", getElapsedFrames() / 60.0f );
	gl::setDefaultShaderVars();
	gl::drawArrays( GL_POINTS, 0, nParticles );
	
	gl::popMatrices();
}

CINDER_APP_NATIVE( TransformFeedbackSmokeParticlesApp, RendererGl )
