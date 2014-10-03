#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/GlslProg.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// output = ((input*exposure) + offset)^(1/gamma)

class HighDynamicRangeApp : public AppNative {
  public:
	void setup() override;
	void mouseDrag( MouseEvent event ) override;	
	void draw() override;
	void fileDrop( FileDropEvent event ) override;

	void loadHdr( const fs::path &path );

	float				mExposure;
	gl::TextureRef		mHdrTexture;
	gl::GlslProgRef		mShader;
};

void HighDynamicRangeApp::loadHdr( const fs::path &path )
{
	Surface32f s( loadImage( path ) );
	mHdrTexture = gl::Texture::create( s, gl::Texture::Format().internalFormat( GL_RGB32F ) );
	
	mExposure = 1.0f;
}

void HighDynamicRangeApp::setup()
{
	loadHdr( getAssetPath( "Desk_oBA2_scaled.hdr" ) );
			
	mShader = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "shader.vert" ) )
																.fragment( loadAsset( "shader.frag" ) ) );
}

void HighDynamicRangeApp::fileDrop( FileDropEvent event )
{
	loadHdr( event.getFile( 0 ) );
}

void HighDynamicRangeApp::mouseDrag( MouseEvent event )
{
	mExposure = powf( event.getPos().x / (float)getWindowWidth() * 2, 4 );
	console() << "Exposure: " << mExposure << std::endl;
}

void HighDynamicRangeApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );

	gl::ScopedGlslProg shaderScp( mShader );
	gl::ScopedTextureBind texBindScp( mHdrTexture );
	mShader->uniform( "uExposure", mExposure );
	gl::drawSolidRect( mHdrTexture->getBounds() );
}

CINDER_APP_NATIVE( HighDynamicRangeApp, RendererGl( RendererGl::Options() ) )