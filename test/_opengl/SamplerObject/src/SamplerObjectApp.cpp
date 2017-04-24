#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ip/Checkerboard.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SamplerObjectApp : public App {
  public:
	void setup() override;
	void draw() override;
	
	gl::TextureRef		mTex;
	gl::Sampler			mSampler1;
	gl::SamplerRef		mSampler2;
};

void SamplerObjectApp::setup()
{
	mTex = gl::Texture::create( ip::checkerboard( 16, 16, 4 ), gl::Texture::Format().magFilter( GL_NEAREST ) );
	mSampler1 = gl::Sampler( gl::Sampler::Format().magFilter( GL_LINEAR ).minFilter( GL_LINEAR ) );
	mSampler2 = gl::Sampler::create();
	mSampler2->setMagFilter( GL_NEAREST );
}

void SamplerObjectApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	
	// left half should be nearest as dictated by sampler-less texture
	gl::draw( mTex, Rectf( 0, 0, 160, 160 ) );
	// left half should be linear as dictated by sampler
	{
		gl::ScopedSamplerBind sampler_( mSampler1 );
		gl::draw( mTex, Rectf( 160, 0, 320, 160 ) );
	}
}

CINDER_APP( SamplerObjectApp, RendererGl )
