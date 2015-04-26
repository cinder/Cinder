#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "CinderFreenect.h"


#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Log.h"
#include "cinder/gl/Batch.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const int PARTICLES = 128;
const float PRESIMULATION_DELTA_TIME = 0.1;
const int BASE_LIFETIME = 2;
const float MAX_DELTA_TIME = 0.2;

const int SLICES = 60;

const int SORT_PASSES_PER_FRAME = 50;

const int OPACITY_TEXTURE_RESOLUTION = 1024;

gl::GlslProgRef loadShader( const std::string& filename )
{
    gl::GlslProgRef prog;
    try {
        prog = gl::GlslProg::create( loadAsset( filename+".vert"), loadAsset(filename+".frag") );
    } catch (const gl::GlslProgCompileExc& e) {
        console() << filename+" shader" << e.what() << endl;
    } catch (const gl::GlslProgLinkExc& e) {
        console() << filename+" shader" << e.what() << endl;
    }
    return prog;
}

using namespace ci;
using namespace ci::app;
using namespace std;

class VolumeKinectApp : public App {
  public:
    
    VolumeKinectApp();
    
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    gl::BatchRef    mVertices, mParticleQuad, mFullScreen, mFloor;
    gl::FboRef      mSimulationFbo[2], mOpacityFbo;
    gl::GlslProgRef mSimulationShader, mRenderShader, mSortShader, mOpacityShader, mFloorShader, mBackgroundShader;
    gl::TextureRef  mSpawnTexture;
    
    KinectRef       mKinect;
    
    CameraPersp     mCamera;
    
    int mCurrent = 0;
    int mMaxParticleCount = PARTICLES * PARTICLES;
    
    float mInitialTurbulance = 0.4f;
    float mPrevTime = 0.f;
    float mDeltaTime = 0.f;
    float mInitialSpeed = 1.0f;
    
    bool mFirstFrame = true;
    
    int mTotalSortSteps;
    int mSortStepsLeft;
    int mSortPass = -1;
    int mSortStage = -1;
    
    bool mFlipped = false;
    
    CameraOrtho mLightCamera;

    
};

VolumeKinectApp::VolumeKinectApp()
{
    
    
    mSimulationShader = loadShader("simulation");
    mRenderShader = loadShader("render");
    mSortShader = loadShader("sort");
    mOpacityShader = loadShader("opacity");
    mBackgroundShader = loadShader("background");
    mFloorShader = loadShader("floor");
    
    gl::Texture::Format fmt;
    fmt.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
    fmt.setMinFilter( GL_NEAREST );
    fmt.setMagFilter( GL_NEAREST );
    fmt.setInternalFormat(GL_RGBA32F);
    fmt.setDataType(GL_FLOAT);
    
    mSimulationFbo[0] = gl::Fbo::create(PARTICLES, PARTICLES, gl::Fbo::Format().colorTexture(fmt).disableDepth() );
    mSimulationFbo[1] = gl::Fbo::create(PARTICLES, PARTICLES, gl::Fbo::Format().colorTexture(fmt).disableDepth() );
    
    {
        gl::ScopedFramebuffer fbo( mSimulationFbo[0] );
        gl::clear();
    }
    {
        gl::ScopedFramebuffer fbo( mSimulationFbo[1] );
        gl::clear();
    }
    
    mParticleQuad = gl::Batch::create(geom::Plane().size(vec2(PARTICLES)).origin(vec3(PARTICLES/2, PARTICLES/2, 0.)).normal(vec3(0,0,1)), mSimulationShader);
    
    
    gl::Texture::Format opacity_fmt;
    opacity_fmt.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
    opacity_fmt.setMagFilter( GL_LINEAR );
    opacity_fmt.setMagFilter( GL_LINEAR );
    opacity_fmt.setDataType(GL_UNSIGNED_BYTE);
    opacity_fmt.setInternalFormat(GL_RGBA);
    
    mOpacityFbo = gl::Fbo::create( OPACITY_TEXTURE_RESOLUTION, OPACITY_TEXTURE_RESOLUTION, gl::Fbo::Format().disableDepth().colorTexture(opacity_fmt) );
    
    vector<vec4> positions;
    vector<vec2> texCoords;
    vector<vec4> spawn;
    
    for( int y = 0; y < PARTICLES; y++ ){
        for( int x = 0; x < PARTICLES; x++ ){
            
            auto sphere_pt = randVec3f();
            
            auto tc = vec2( (x + 0.5) / PARTICLES, (y + 0.5) / PARTICLES );
            auto spawndata = vec4( sphere_pt*.1f, (float)BASE_LIFETIME + randFloat() * 2.f );
            
            spawn.push_back(spawndata);
            positions.push_back(vec4(tc,0.,1.));
            texCoords.push_back(tc);
            
        }
    }
    
    geom::BufferLayout pos_layout, tc_layout;
    
    auto pos_buffer = gl::Vbo::create(GL_ARRAY_BUFFER, sizeof(vec4)*positions.size(), positions.data(), GL_DYNAMIC_DRAW);
    
    pos_layout.append(geom::POSITION, 4, 0, 0);
    
    auto tc_buffer = gl::Vbo::create(GL_ARRAY_BUFFER, sizeof(vec2)*texCoords.size(), texCoords.data(), GL_STATIC_DRAW);
    
    tc_layout.append(geom::TEX_COORD_0, 2, 0, 0);
    
    vector<pair<geom::BufferLayout, gl::VboRef>> buffers;
    
    buffers.push_back(make_pair(pos_layout, pos_buffer));
    buffers.push_back(make_pair(tc_layout, tc_buffer));
    
    auto mesh = gl::VboMesh::create( PARTICLES * PARTICLES, GL_POINTS, buffers );
    
    mVertices = gl::Batch::create( mesh, mRenderShader);
    
    
    mCamera.setPerspective(60, getWindowAspectRatio(), .1, 10000);
    mCamera.lookAt(vec3(-1,0,.5),vec3(0));
    
    mLightCamera.setOrtho(-5.0, 5.0, -5.0, 5.0, -50.0, 50.0);
    // mLightCamera.setViewDirection(vec3(0,-1,0));
    //mLightCamera.setWorldUp(vec3(0,1,0));
    mLightCamera.lookAt(vec3(0,5,0), vec3(0,0,0));
    
    
    mFullScreen = gl::Batch::create(geom::Plane().size(getWindowSize()).origin(vec3(getWindowCenter(),0.)).normal(vec3(0,0,1)), mBackgroundShader);
    
    mFloor = gl::Batch::create(geom::Plane().size(vec2(100,100)).origin(vec3(0.,-.25, 0. )).normal(vec3(0,1,0)), mFloorShader);
    
    mTotalSortSteps = (log2(mMaxParticleCount) * (log2(mMaxParticleCount) + 1)) / 2;
    mSortStepsLeft = mTotalSortSteps;
    mSortPass = -1;
    mSortStage = -1;
    
    gl::enable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);

    
    mKinect = Kinect::create();
    
    vector<uint16_t> tmp( mKinect->getHeight()*mKinect->getWidth(), 0 );
    
    mSpawnTexture = gl::Texture::create( Channel16u( mKinect->getWidth(), mKinect->getHeight(), mKinect->getWidth() * sizeof(uint16_t),  sizeof(uint16_t), tmp.data() ), gl::Texture::Format().internalFormat(GL_R16UI).dataType(GL_UNSIGNED_SHORT).minFilter(GL_NEAREST).magFilter(GL_NEAREST) );
    
    
}

void VolumeKinectApp::mouseDown( MouseEvent event )
{
}

void VolumeKinectApp::update()
{
    if( mKinect->checkNewDepthFrame() )
        mSpawnTexture->update( Channel16u( mKinect->getDepthImage() ) );
    
    
    
}

void VolumeKinectApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( VolumeKinectApp, RendererGl )
