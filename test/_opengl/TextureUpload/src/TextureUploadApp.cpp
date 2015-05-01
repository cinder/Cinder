#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ip/Fill.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TextureUploadApp : public App {
  public:
	void prepareSettings( Settings *settings ) override { settings->enableMultiTouch( false ); }
	void setup();
	template<typename T>
	void setupTexConstructor();
	template<typename T>
	void setupTexUpdate();

	void mouseDown( MouseEvent event ) override;
	void draw() override;

	gl::TextureRef		mCurTex;
	ImageSourceRef		mImg, mImg2, mImgAlpha, mImg2Alpha;

	template<typename T>
	gl::TextureRef genTexConstructor( bool color, bool alpha );
	template<typename T>
	gl::TextureRef genTexUpdate( bool color, bool alpha );

	size_t										mCurFnIdx;
	vector<function<gl::TextureRef(void)>>		mTexGenFns;
};

template<typename T>
gl::TextureRef TextureUploadApp::genTexConstructor( bool color, bool alpha )
{
	if( color && alpha ) {
		SurfaceT<T> s( mImgAlpha );
		return gl::Texture::create( s );
	}
	else if( color ) {
		SurfaceT<T> s( mImg );
		return gl::Texture::create( s );
	}
	else { // grayscale
		ChannelT<T> s( mImg );
		return gl::Texture::create( s );
	}
}

template<typename T>
gl::TextureRef TextureUploadApp::genTexUpdate( bool color, bool alpha )
{
	if( color && alpha ) {
		SurfaceT<T> s0( mImgAlpha );
		gl::TextureRef result = gl::Texture::create( s0 );
		result->update( SurfaceT<T>( mImg2Alpha ) );
		return result;
	}
	else if( color ) {
		SurfaceT<T> s0( mImg );
		gl::TextureRef result = gl::Texture::create( s0 );
		result->update( SurfaceT<T>( mImg2 ) );
		return result;
	}
	else { // grayscale
		ChannelT<T> s0( mImg );
		gl::TextureRef result = gl::Texture::create( s0 );
		result->update( ChannelT<T>( mImg2 ) );
		return result;
	}
}

template<typename T>
void TextureUploadApp::setupTexConstructor()
{
	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Constructor Surface RGB " << sizeof(T) * 8 << "-bit" << std::endl;
			return genTexConstructor<T>( true, false );
		}
	);

	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Constructor Surface RGBA " << sizeof(T) * 8 << "-bit" << std::endl;
			return genTexConstructor<T>( true, true );
		}
	);

	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Constructor Channel " << sizeof(T) * 8 << "-bit" << std::endl;
			return genTexConstructor<T>( false, false );
		}
	);
}

template<typename T>
void TextureUploadApp::setupTexUpdate()
{
	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Update Surface RGB " << sizeof(T) * 8 << "-bit" << std::endl;
			return genTexUpdate<T>( true, false );
		}
	);

	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Update Surface RGBA " << sizeof(T) * 8 << "-bit" << std::endl;
			return genTexUpdate<T>( true, true );
		}
	);

	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Update Channel " << sizeof(T) * 8 << "-bit" << std::endl;
			return genTexUpdate<T>( false, false );
		}
	);
	
	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Update Subregion RGB " << sizeof(T) * 8 << "-bit" << std::endl;
			SurfaceT<T> srcImg( mImg );			
			SurfaceT<T> square( 100, 100, false );
			gl::TextureRef result = gl::Texture::create( mImg2 );
			square.copyFrom( srcImg, Area( 28, 128, 28 + 100, 128 + 100 ), ivec2( -28, -128 ) );
			for( int y = 0; y < result->getHeight() / square.getHeight(); y += 2 ) {
				for( int x = y%2; x < result->getWidth() / square.getWidth(); x += 2 ) {
					result->update( square, 0, ivec2( x * square.getWidth(), y * square.getHeight() ) );
				}
			}
			return result;
		}
	);

	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Update Subregion Channel " << sizeof(T) * 8 << "-bit" << std::endl;
			ChannelT<T> srcImg( mImg );			
			ChannelT<T> square( 100, 100 );
			gl::TextureRef result = gl::Texture::create( ChannelT<T>( mImg2 ) );
			square.copyFrom( srcImg, Area( 28, 128, 28 + 100, 128 + 100 ), ivec2( -28, -128 ) );
			for( int y = 0; y < result->getHeight() / square.getHeight(); y += 2 ) {
				for( int x = y%2; x < result->getWidth() / square.getWidth(); x += 2 ) {
					result->update( square, 0, ivec2( x * square.getWidth(), y * square.getHeight() ) );
				}
			}
			return result;
		}
	);
}

void TextureUploadApp::setup()
{
	mImg = loadImage( loadAsset( "fish.jpg" ) );
	mImgAlpha = loadImage( loadAsset( "fish_alpha.png" ) );
	mImg2 = loadImage( loadAsset( "fish2.jpg" ) );
	mImg2Alpha = loadImage( loadAsset( "fish2_alpha.png" ) );
	
	// 8 bit
	{
		setupTexConstructor<uint8_t>();
		setupTexUpdate<uint8_t>();
	}

	// 16 bit
	{
		setupTexConstructor<uint16_t>();
		setupTexUpdate<uint16_t>();
	}
	
	// 32 bit
	{
		setupTexConstructor<uint8_t>();
		setupTexUpdate<uint8_t>();
	}

	mCurFnIdx = 0;
	mCurTex = mTexGenFns[mCurFnIdx]();
}

void TextureUploadApp::mouseDown( MouseEvent event )
{
	mCurFnIdx = ( mCurFnIdx + 1 ) % mTexGenFns.size();
	mCurTex = mTexGenFns[mCurFnIdx]();
	console() << *mCurTex << std::endl;
}

void TextureUploadApp::draw()
{
	gl::clear( Color( 0.5f, 0.5f, 0.5f ) );
	gl::enableAlphaBlending();
	gl::draw( mCurTex );
}

CINDER_APP( TextureUploadApp, RendererGl )
