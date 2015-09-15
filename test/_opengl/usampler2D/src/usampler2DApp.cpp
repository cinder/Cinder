// Should appear as alternating black & white horizontal lines

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/Texture.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class usample2D_testApp : public App {
  public:
    void setup() override;
    void draw() override;
    
    gl::BatchRef    mBatch;
    gl::GlslProgRef mShader;
    gl::TextureRef  mUSampler;
};

void usample2D_testApp::setup()
{
    mShader = gl::GlslProg::create(loadAsset("usampler_test.vert"), loadAsset("usampler_test.frag"));
    mBatch = gl::Batch::create(geom::Plane().size(getWindowSize()).origin(vec3(getWindowCenter(),0)).normal(vec3(0,0,1)), mShader);
	
	auto fmt = gl::Texture::Format().internalFormat( GL_R16UI ).dataType( GL_UNSIGNED_SHORT ).minFilter( GL_NEAREST ).magFilter( GL_NEAREST );
    mUSampler = gl::Texture::create(getWindowWidth(), getWindowHeight(), fmt );
    
    mShader->uniform("test", 0);
    
    vector<uint16_t> shorts;
	for( int y = 0; y < getWindowHeight(); ++y ) {
		for( int x = 0; x < getWindowWidth(); x++ ) {
			if( y % 2 == 0 )
				shorts.push_back( 65000 );
			else
				shorts.push_back( 0 );
		}
	}
    
    mUSampler->update( Channel16u( getWindowWidth(), getWindowHeight(), getWindowWidth()*sizeof(uint16_t), 1, shorts.data() ));
}

void usample2D_testApp::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    
    gl::ScopedTextureBind usampler( mUSampler, 0 );
    mBatch->draw();
}

CINDER_APP( usample2D_testApp, RendererGl )