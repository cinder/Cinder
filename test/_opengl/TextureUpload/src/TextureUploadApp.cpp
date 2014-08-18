#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TextureUploadApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();

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

void TextureUploadApp::setup()
{
	mImg = loadImage( loadAsset( "fish.jpg" ) );
	mImgAlpha = loadImage( loadAsset( "fish_alpha.png" ) );
	mImg2 = loadImage( loadAsset( "fish2.jpg" ) );
	mImg2Alpha = loadImage( loadAsset( "fish2_alpha.png" ) );
	
	// 8 bit
	{
		mTexGenFns.push_back( [=](void)->gl::TextureRef {
				app::console() << "Constructor Surface8u RGB" << std::endl;
				return genTexConstructor<uint8_t>( true, false );
			}
		);

		mTexGenFns.push_back( [=](void)->gl::TextureRef {
				app::console() << "Constructor Surface8u RGBA" << std::endl;
				return genTexConstructor<uint8_t>( true, true );
			}
		);

		mTexGenFns.push_back( [=](void)->gl::TextureRef {
				app::console() << "Constructor Channel8u" << std::endl;
				return genTexConstructor<uint8_t>( false, false );
			}
		);

		mTexGenFns.push_back( [=](void)->gl::TextureRef {
				app::console() << "Update Surface8u RGB" << std::endl;
				return genTexUpdate<uint8_t>( true, false );
			}
		);

		mTexGenFns.push_back( [=](void)->gl::TextureRef {
				app::console() << "Update Surface8u RGBA" << std::endl;
				return genTexUpdate<uint8_t>( true, true );
			}
		);

		mTexGenFns.push_back( [=](void)->gl::TextureRef {
				app::console() << "Update Channel8u" << std::endl;
				return genTexUpdate<uint8_t>( false, false );
			}
		);
	}
	
	// 32 bit
	{
		mTexGenFns.push_back( [=](void)->gl::TextureRef {
				app::console() << "Constructor Surface32f RGB" << std::endl;
				return genTexConstructor<float>( true, false );
			}
		);

		mTexGenFns.push_back( [=](void)->gl::TextureRef {
				app::console() << "Constructor Surface32f RGBA" << std::endl;
				return genTexConstructor<float>( true, true );
			}
		);

		mTexGenFns.push_back( [=](void)->gl::TextureRef {
				app::console() << "Constructor Channel32f" << std::endl;
				return genTexConstructor<float>( false, false );
			}
		);

		mTexGenFns.push_back( [=](void)->gl::TextureRef {
				app::console() << "Update Surface32f RGB" << std::endl;
				return genTexUpdate<float>( true, false );
			}
		);

		mTexGenFns.push_back( [=](void)->gl::TextureRef {
				app::console() << "Update Surface32f RGBA" << std::endl;
				return genTexUpdate<float>( true, true );
			}
		);

		mTexGenFns.push_back( [=](void)->gl::TextureRef {
				app::console() << "Update Channel32f" << std::endl;
				return genTexUpdate<float>( false, false );
			}
		);
	}

	mCurFnIdx = 0;
	mCurTex = mTexGenFns[mCurFnIdx]();
}

void TextureUploadApp::mouseDown( MouseEvent event )
{
	mCurFnIdx = ( mCurFnIdx + 1 ) % mTexGenFns.size();
	mCurTex = mTexGenFns[mCurFnIdx]();
}

void TextureUploadApp::update()
{
}

void TextureUploadApp::draw()
{
	gl::clear( Color( 0.5f, 0.5f, 0.5f ) );
	gl::enableAlphaBlending();
	gl::draw( mCurTex );
}

CINDER_APP_NATIVE( TextureUploadApp, RendererGl )
