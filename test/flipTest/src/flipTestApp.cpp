#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ip/Flip.h"

using namespace ci;
using namespace ci::app;
using namespace std;

//click mouse on the window to see different tests

class flipTestApp : public App {
  public:
    static void prepare(Settings* aSettings);
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
    
    Surface             mSurfaces[12];
    Channel             mPlanarChannels[12];
    gl::TextureRef      mSurfaceTextures[12], mPlanarChannelTextures[12], mNonPlanarChannelTextures[12];
    
    int                 mImageCounter;
};

void flipTestApp::prepare(Settings* aSettings)
{
    aSettings->setWindowPos(200, 0);
}

void flipTestApp::setup()
{
    int counter = 0;
    mSurfaces[0] = loadImage(loadAsset("test_portrait.jpg"));
    mSurfaces[1] = loadImage(loadAsset("test_landscape.jpg"));
    mSurfaces[2] = loadImage(loadAsset("cinder_alpha.png"));
    for (int i=0; i<4; i++){
        mSurfaces[counter++] = loadImage(loadAsset("test_portrait.jpg"));
        mSurfaces[counter++] = loadImage(loadAsset("test_landscape.jpg"));
        mSurfaces[counter++] = loadImage(loadAsset("cinder_alpha.png"));
    }
    
    //planar channels
    counter = 0;
    for (int i=0; i<4; i++){
        mPlanarChannels[counter++] = loadImage(loadAsset("test_portrait.jpg"));
        mPlanarChannels[counter++] = loadImage(loadAsset("test_landscape.jpg"));
        mPlanarChannels[counter++] = loadImage(loadAsset("cinder_alpha.png"));
    }
    
    //surfaces for non-planar channels
    Surface surfaces[12];
    for (int i=0; i<12; i++){
        surfaces[i] = mSurfaces[i].clone();
    }
    
    /////////////////////
    for (int i=0; i<12; i++){
        if (i<3) {
            
        }
        else if (i<6) {
            ip::flipVertical(&mSurfaces[i]);
        }
        else if (i<9) {
            ip::flipHorizontal(&mSurfaces[i]);
        }
        else {
            ip::flipDiagonal(&mSurfaces[i]);
        }
        mSurfaceTextures[i] = gl::Texture::create(mSurfaces[i]);
    }
    
    /////////////////////
    for (int i=0; i<12; i++){
        if (i<3) {
            
        }
        else if (i<6) {
            ip::flipVertical(&mPlanarChannels[i]);
        }
        else if (i<9) {
            ip::flipHorizontal(&mPlanarChannels[i]);
        }
        else {
            ip::flipDiagonal(&mPlanarChannels[i]);
        }
        mPlanarChannelTextures[i] = gl::Texture::create(mPlanarChannels[i]);
    }
    
    /////////////////////
    for (int i=0; i<12; i++){
        Channel& nonPlanarChannel = surfaces[i].getChannel(0);
        if (i<3) {
            
        }
        else if (i<6) {
            ip::flipVertical(&nonPlanarChannel);
        }
        else if (i<9) {
            ip::flipHorizontal(&nonPlanarChannel);
        }
        else {
            ip::flipDiagonal(&nonPlanarChannel);
        }
        mNonPlanarChannelTextures[i] = gl::Texture::create(nonPlanarChannel);
    }
    
    /////////////
    mImageCounter=0;
    setWindowSize(mSurfaces[mImageCounter].getSize() * ivec2(3, 4));
}

void flipTestApp::mouseDown( MouseEvent event )
{
    mImageCounter++;
    mImageCounter %= 3;
    setWindowSize(mSurfaces[mImageCounter].getSize() * ivec2(3, 4));
}

void flipTestApp::update()
{
}

void flipTestApp::draw()
{
    gl::clear( Color( 1, 0, 0 ) );
    auto size = mSurfaceTextures[mImageCounter]->getSize();
    
    {
        const gl::ScopedModelMatrix scModelMatrix;
        for (int i=0; i<4; i++){
            auto& texture = mSurfaceTextures[i * 3 + mImageCounter];
            gl::draw(texture);
            gl::translate(vec2(0, size.y));
            
        }
    }
    
    {
        const gl::ScopedModelMatrix scModelMatrix;
        gl::translate(vec2(size.x, 0));
        for (int i=0; i<4; i++){
            auto& texture = mPlanarChannelTextures[i * 3 + mImageCounter];
            gl::draw(texture);
            gl::translate(vec2(0, texture->getHeight()));
            
        }
    }
    
    {
        const gl::ScopedModelMatrix scModelMatrix;
        gl::translate(vec2(2*size.x, 0));
        for (int i=0; i<4; i++){
            auto& texture = mNonPlanarChannelTextures[i * 3 + mImageCounter];
            gl::draw(texture);
            gl::translate(vec2(0, size.y));
            
        }
    }
}

CINDER_APP( flipTestApp, RendererGl, &flipTestApp::prepare )
