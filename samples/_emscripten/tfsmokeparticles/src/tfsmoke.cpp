
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Vao.h"
#include "cinder/Camera.h"
#include <vector>
#include "cinder/gl/TransformFeedbackObj.h"
#include "shaders.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#define STRINGIFY(A) #A

using namespace std;
using namespace ci;
using namespace ci::app;

const int nParticles			= 4000;
const int PositionIndex			= 0;
const int VelocityIndex			= 1;
const int StartTimeIndex		= 2;
const int InitialVelocityIndex	= 3;

class SimpleTransformFeedback : public App
{
public:
    void draw() override;
    void setup() override;
    void update() override;
    void setupBuffers();
    void setupShaders();

    gl::VboRef mPPositions[2], mPVelocities[2], mPStartTimes[2], mPInitVelocity;
    ci::gl::VaoRef mVaos[2];
    gl::GlslProgRef mUpdateShader,mRenderShader;
    uint32_t mDrawBuff = 0;
    ci::CameraPersp mCam;
    Rand mRand;
    gl::TextureRef mParticleTex;
};

void prepareSettings( SimpleTransformFeedback::Settings* settings )
{
    settings->setMultiTouchEnabled( false );
}

void SimpleTransformFeedback::setup()
{
    mCam.setPerspective( 60.0f, getWindowAspectRatio(), .01f, 1000.0f );
    mCam.lookAt( vec3( 0, 0, 10 ), vec3( 0, 0, 0 ) );

    mParticleTex = gl::Texture::create(loadImage(loadAsset("smoke_blur.png")));

    setupBuffers();
    setupShaders();
}

void SimpleTransformFeedback::setupBuffers() {
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
        mVaos[i] = ci::gl::Vao::create();

        // Bind the vao to capture index data for the glsl
        gl::ScopedVao vao(mVaos[i]);

        gl::ScopedBuffer pos(mPPositions[i]);
        //mPPositions[i]->bind();
        ci::gl::vertexAttribPointer( PositionIndex, 3, GL_FLOAT, GL_FALSE, 0, 0 );
        ci::gl::enableVertexAttribArray( PositionIndex );

        gl::ScopedBuffer vel(mPVelocities[i]);
        //mPVelocities[i]->bind();
        ci::gl::vertexAttribPointer( VelocityIndex, 3, GL_FLOAT, GL_FALSE, 0, 0 );
        ci::gl::enableVertexAttribArray( VelocityIndex );

        gl::ScopedBuffer st(mPStartTimes[i]);
        //mPStartTimes[i]->bind();
        ci::gl::vertexAttribPointer( StartTimeIndex, 1, GL_FLOAT, GL_FALSE, 0, 0 );
        ci::gl::enableVertexAttribArray( StartTimeIndex );

        gl::ScopedBuffer initVel(mPInitVelocity);
        ci::gl::vertexAttribPointer( InitialVelocityIndex, 3, GL_FLOAT, GL_FALSE, 0, 0 );
        ci::gl::enableVertexAttribArray( InitialVelocityIndex );
    }
}


void SimpleTransformFeedback::setupShaders(){
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

        /**
         * Setup the update shader. Note that in the case of WebGL, even when
         * doing transform feedback, a fragment shader is still needed.
         * That being said, it doesn't have to do anything, it just has to
         * compile into a valid shader.
         */
        mUpdateParticleGlslFormat.vertex( loadAsset( "updateSmoke.vert" ) )
        .fragment(STRINGIFY(
            precision highp float;
            out vec4 glFragColor;
            void main(){
                glFragColor = vec4(1.0,1.0,0.0,1.0);
            }
        ))
        // This option will be either GL_SEPARATE_ATTRIBS or GL_INTERLEAVED_ATTRIBS,
        // depending on the structure of our data, below. We're using multiple
        // buffers. Therefore, we're using GL_SEPERATE_ATTRIBS
        // Note - there is a restriction on the number of buffers, which can be as little as 4
        .feedbackFormat( GL_SEPARATE_ATTRIBS )
        // Pass the feedbackVaryings to glsl
        .feedbackVaryings( transformFeedbackVaryings )
        .attribLocation( "VertexPosition",			PositionIndex )
        .attribLocation( "VertexVelocity",			VelocityIndex )
        .attribLocation( "VertexStartTime",			StartTimeIndex )
        .attribLocation( "VertexInitialVelocity",	InitialVelocityIndex );

        mUpdateShader = ci::gl::GlslProg::create( mUpdateParticleGlslFormat );
    }
    catch( const ci::Exception &ex ) {
        console() << "PARTICLE UPDATE GLSL ERROR: " << ex.what() << std::endl;
    }

    mUpdateShader->uniform( "H", 1.0f / 60.0f );
    mUpdateShader->uniform( "Accel", vec3( 0.0f ) );
    mUpdateShader->uniform( "ParticleLifetime", 3.0f );

    try {
        ci::gl::GlslProg::Format mRenderParticleGlslFormat;
        // This being the render glsl, we provide a fragment shader.
        mRenderParticleGlslFormat.vertex( loadAsset( "renderSmoke.vert" ) )
                .fragment( loadAsset( "renderSmoke.frag" ) )
                .attribLocation("VertexPosition",			PositionIndex )
                .attribLocation( "VertexStartTime",			StartTimeIndex );

        mRenderShader = ci::gl::GlslProg::create( mRenderParticleGlslFormat );
    }
    catch( const ci::Exception &ex ) {
        console() << "PARTICLE RENDER GLSL ERROR: " << ex.what() << std::endl;
    }

    mRenderShader->uniform( "ParticleTex", 0 );
    mRenderShader->uniform( "MinParticleSize", 1.0f );
    mRenderShader->uniform( "MaxParticleSize", 64.0f );
    mRenderShader->uniform( "ParticleLifetime", 3.0f );
}
void SimpleTransformFeedback::update()
{

    gl::ScopedGlslProg shader( mUpdateShader );
    gl::ScopedState	stateScope( GL_RASTERIZER_DISCARD, true );
    gl::ScopedVao  vao( mVaos[ mDrawBuff ] );

    mUpdateShader->uniform( "Time", (float)app::getElapsedSeconds() );

    // bind buffers
    gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, PositionIndex, mPPositions[ 1-mDrawBuff ] );
    gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, VelocityIndex, mPVelocities[ 1-mDrawBuff ] );
    gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, StartTimeIndex , mPStartTimes[ 1-mDrawBuff ] );
    //gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, InitialVelocityIndex, mPInitVelocity );


    gl::beginTransformFeedback( GL_POINTS );

    // Draw source into destination, performing our vertex transformations.
    gl::drawArrays( GL_POINTS, 0, nParticles );

    gl::endTransformFeedback();

    // IMPORTANT! must unbind buffer after we're done with TF, otherwise during draw, WebGL thinks
    // the buffer is still bound for TF and that you're trying to use it at the same time.
    gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER,0,nullptr );
    gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER,1,nullptr );
    gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER,2,nullptr );

    mDrawBuff = 1 - mDrawBuff;
}

void SimpleTransformFeedback::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    static float rotateRadians = 0.0f;
    rotateRadians += 0.01f;

    gl::ScopedVao			vaoScope( mVaos[mDrawBuff] );
    gl::ScopedGlslProg		glslScope( mRenderShader );
    gl::ScopedTextureBind	texScope( mParticleTex );
    gl::ScopedBlend			blendScope( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    gl::pushMatrices();
    gl::setMatrices( mCam );
    gl::multModelMatrix( rotate( rotateRadians, vec3( 0, 1, 0 ) ) );

    mRenderShader->uniform( "Time", (float)getElapsedSeconds());
    gl::setDefaultShaderVars();
    gl::drawArrays( GL_POINTS, 0, nParticles );

    gl::popMatrices();

}

CINDER_APP( SimpleTransformFeedback, RendererGl, prepareSettings )
