#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Pbo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CompressedTextureApp : public App {
  public:
	void prepareSettings( Settings *settings ) { settings->setMultiTouchEnabled( false ); }
	void setup();
	void mouseDown( MouseEvent event );	
	void keyDown( KeyEvent event );
	void draw();

	size_t								mIndex;
	float								mZoom;
	vector<pair<string,gl::TextureRef>> mTextures;
};

void CompressedTextureApp::setup()
{
	mIndex = 0;
	mZoom = 2.0f;
	mTextures.push_back( make_pair( "Original", gl::Texture::create( loadImage( loadAsset( "compression_test.png" ) ) ) ) );
	ivec2 textureSize = mTextures.back().second->getSize();

	gl::Texture::Format		format;
	format.loadTopDown();
#if ! defined( CINDER_GLES ) // use an intermediate PBO for non-ES
	gl::PboRef pbo = gl::Pbo::create( GL_PIXEL_UNPACK_BUFFER, textureSize.x * textureSize.y * 4, nullptr, GL_STREAM_DRAW );
	format.intermediatePbo( pbo );
#endif

#if ! defined( CINDER_GLES ) || defined( CINDER_GL_ANGLE )
	mTextures.push_back( make_pair( "DXT1", gl::Texture::createFromDds( loadAsset( "compression_test_dxt1.dds" ), format ) ) );
	mTextures.push_back( make_pair( "DXT3", gl::Texture::createFromDds( loadAsset( "compression_test_dxt3.dds" ), format ) ) );
	mTextures.push_back( make_pair( "DXT5", gl::Texture::createFromDds( loadAsset( "compression_test_dxt5.dds" ), format ) ) );

	if( gl::isExtensionAvailable( "GL_ARB_texture_compression_bptc" ) )
		mTextures.push_back( make_pair( "BC7", gl::Texture::createFromDds( loadAsset( "compression_test_bc7.dds" ), format ) ) );
	else
		console() << "This GL implementation doesn't support BC7 textures" << std::endl;

	if( gl::isExtensionAvailable( "GL_ARB_texture_compression_rgtc" ) ) {
		mTextures.push_back( make_pair( "Luminance DXT5A/BC4", gl::Texture::createFromDds( loadAsset( "compression_test_lum_dxt5a.dds" ), gl::Texture::Format().swizzleMask( GL_RED, GL_RED, GL_RED, GL_ONE ) ) ) );
		mTextures.push_back( make_pair( "Luminance+Alpha 3DC/ATI2/BC5", gl::Texture::createFromDds( loadAsset( "compression_test_lum_3dc.dds" ), gl::Texture::Format().swizzleMask( GL_RED, GL_RED, GL_RED, GL_GREEN ) ) ) );
	}
	
	if( gl::isExtensionAvailable( "GL_OES_compressed_ETC1_RGB8_texture" ) ) {
		mTextures.push_back( make_pair( "ETC1", gl::Texture::createFromKtx( loadAsset( "compression_test_etc1.ktx" ), format ) ) );
	}
	else
		console() << "This GL implementation doesn't support ETC1 textures" << std::endl;

	if( gl::getVersion() >= make_pair(4,3) ) {
		mTextures.push_back( make_pair( "ETC2", gl::Texture::createFromKtx( loadAsset( "compression_test_etc2.ktx" ), format ) ) );
	}
	else
		console() << "This GL implementation doesn't support ETC2 textures" << std::endl;

#elif defined( CINDER_GLES )
	if( gl::isExtensionAvailable( "GL_IMG_texture_compression_pvrtc" ) ) {
		mTextures.push_back( make_pair( "PVRTC: 2bit", gl::Texture::createFromKtx( loadAsset( "compression_test_pvrtc_2bit.ktx" ) ) ) );
		mTextures.push_back( make_pair( "PVRTC: 4bit", gl::Texture::createFromKtx( loadAsset( "compression_test_pvrtc_4bit.ktx" ) ) ) );
	}
#endif
}

void CompressedTextureApp::mouseDown( MouseEvent event )
{
	mIndex = ( mIndex + 1 ) % mTextures.size();
	console() << "Showing " << mTextures[mIndex].first << std::endl;
}

void CompressedTextureApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ) {
		case KeyEvent::KEY_EQUALS:
			mZoom *= 2.0f;
		break;
		case KeyEvent::KEY_MINUS:
			mZoom /= 2.0f;
		break;
	}
}

void CompressedTextureApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::enableAlphaBlending( true );

	gl::draw( mTextures[mIndex].second, Rectf( mTextures[mIndex].second->getBounds() ) * mZoom );
}

#if defined( CINDER_MSW )
CINDER_APP( CompressedTextureApp, RendererGl( RendererGl::Options().version( 4, 3 ) ) )
#else
CINDER_APP( CompressedTextureApp, RendererGl )
#endif