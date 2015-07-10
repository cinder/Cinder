#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageFileTinyExr.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// output = ((input*exposure) + offset)^(1/gamma)

class HighDynamicRangeApp : public App {
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
#if defined( CINDER_GL_ES_2 )
	mHdrTexture = gl::Texture::create( s, gl::Texture::Format().internalFormat( GL_RGB ).dataType( GL_FLOAT ) );
#else
	mHdrTexture = gl::Texture::create( s, gl::Texture::Format() );
#endif

	mExposure = 1.0f;
}

void HighDynamicRangeApp::setup()
{
	ImageSourceFileTinyExr::registerSelf();
	ImageTargetFileTinyExr::registerSelf();

	loadHdr( getAssetPath( "Desk_oBA2_scaled.hdr" ) );

#if ! defined( CINDER_GL_ES )
	mShader = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "shader.vert" ) )
																.fragment( loadAsset( "shader.frag" ) ) );
#else
	mShader = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "shader_es2.vert" ) )
																.fragment( loadAsset( "shader_es2.frag" ) ) );
#endif
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

CINDER_APP( HighDynamicRangeApp, RendererGl, []( App::Settings *settings ) {
	settings->setMultiTouchEnabled( false );
} )