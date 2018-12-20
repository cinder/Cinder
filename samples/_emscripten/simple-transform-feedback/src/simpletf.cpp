
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

using namespace std;
using namespace ci;
using namespace ci::app;

#define NUM_PARTICLES 50000

/**
 * This demonstrates Transform Feedback with Emscripten - nothing interesting
 * in and of itself but this is more to show the slight difference you'll need to
 * keep in mind when using Transform Feedback via Cinder as well as provide a more basic example to
 * those who might not have worked with Transform Feedback before.
 *
 * The primary differences between Emscripten and Cinder is simply that you need to unbind your data buffer 
 * at the end of the transform feedback stage.
 *
 */
class SimpleTransformFeedback : public App
{
public:
    void draw() override;
    void setup() override;
    void update() override;

    ci::gl::VboRef mVbos[2];
    ci::gl::VaoRef mVaos[2];
    gl::GlslProgRef mUpdateShader,mRenderShader;
    uint32_t mDrawBuff = 0;
    ci::CameraPersp mCam;
};

void prepareSettings( SimpleTransformFeedback::Settings* settings )
{
    settings->setMultiTouchEnabled( false );
}

void SimpleTransformFeedback::setup()
{
    float fov = 45 * ( 3.14149 / 10 );
    float aspect = ci::app::getWindowAspectRatio();
    float near = 0.1;
    float far = 40000.0;
    ci::vec3 eye = ci::vec3( 0, 0, 600 );
    ci::vec3 target = ci::vec3( 0, 0, 0) ;

    mCam = CameraPersp( getWindowWidth(), getWindowHeight(), fov, near, far );
    mCam.lookAt( eye, target );

    vector<ci::vec3> positions;
    const int numParticles = NUM_PARTICLES;


    // this just makes a sphere
    const float azimuth = 256.0f * M_PI / numParticles;
    const float inclination = M_PI / numParticles;
    const float radius = 50.0f;
    vec3 center = vec3( getWindowCenter() + vec2( 0.0f, 40.0f ), 0.0f );
    for( int i = 0; i < numParticles; ++i )
    {	// assign starting values to particles.
        float x = radius * sin( inclination * i ) * cos( azimuth * i );
        float y = radius * cos( inclination * i );
        float z = radius * sin( inclination * i ) * sin( azimuth * i );
        positions.emplace_back(vec3(x,y,z));
    }

    mVbos[0] = gl::Vbo::create( GL_ARRAY_BUFFER,sizeof( vec3 ) * positions.size(),positions.data(),GL_STATIC_DRAW );
    mVbos[1] = gl::Vbo::create( GL_ARRAY_BUFFER,sizeof( vec3 ) * positions.size(),nullptr,GL_STATIC_DRAW );

    for(int i = 0; i < 2; ++i)
    {
        mVaos[i] = gl::Vao::create();
        gl::ScopedVao vao(mVaos[i]);
        gl::ScopedBuffer vbo(mVbos[i]);
        gl::enableVertexAttribArray( 0 );
        gl::vertexAttribPointer( 0, 3, GL_FLOAT,GL_FALSE,0,nullptr );
    }

    gl::GlslProg::Format ufmt,rfmt;
    ufmt.vertex( vertexTF );
    ufmt.fragment( fragmentTF );
    ufmt.feedbackFormat( GL_SEPARATE_ATTRIBS )
            .feedbackVaryings( { "oPosition" } )
            .attribLocation( "vPosition", 0 );

    rfmt.vertex( vertexR );
    rfmt.fragment( fragmentR );

    try {
        mUpdateShader = gl::GlslProg::create( ufmt );
    }
    catch( ci::Exception &e ) {
        CI_LOG_I( "Exception with update shader :" << e.what() );
    }

    try {
        mRenderShader = gl::GlslProg::create( rfmt );
    }
    catch( ci::Exception &e ){
        CI_LOG_I( "Exception with render shader :" << e.what() );
    }
}

void SimpleTransformFeedback::update()
{
    gl::ScopedGlslProg shader( mUpdateShader );
    gl::ScopedState	stateScope( GL_RASTERIZER_DISCARD, true );
    gl::ScopedVao  vao( mVaos[ mDrawBuff ] );
    gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, mVbos[ 1-mDrawBuff ] );
    gl::beginTransformFeedback( GL_POINTS );

    // Draw source into destination, performing our vertex transformations.
    gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );

    gl::endTransformFeedback();

    // IMPORTANT! must unbind buffer after we're done with TF, otherwise during draw, WebGL thinks
    // the buffer is still bound for TF and that you're trying to use it at the same time.
    gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER,0,nullptr );

    mDrawBuff = 1 - mDrawBuff;
}

void SimpleTransformFeedback::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    gl::setMatrices( mCam );
    gl::enableDepthRead();
    gl::enableDepthWrite();

    gl::ScopedGlslProg render( mRenderShader );
    gl::ScopedVao  vao( mVaos[ mDrawBuff ] );
    gl::context()->setDefaultShaderVars();
    gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );

}

CINDER_APP( SimpleTransformFeedback, RendererGl, prepareSettings )
