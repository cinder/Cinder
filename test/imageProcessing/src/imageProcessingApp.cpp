#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ip/flip.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class imageProcessingApp : public App {
  public:
	void setup();
	template<typename T>
	void setupFlipVertical();
	
	void mouseDown( MouseEvent event );	
	void draw();
	
	gl::TextureRef		mCurTex;
	
	size_t										mCurFnIdx;
	vector<function<gl::TextureRef(void)>>		mTexGenFns;
};

template<typename T>
void imageProcessingApp::setupFlipVertical()
{
	auto img = loadImage( loadAsset( "upside_down.png" ) );

	SurfaceT<T> rgb8( img );
	SurfaceT<T> rgba8( rgb8.getWidth(), rgb8.getHeight(), true );
	rgba8.copyFrom( rgb8, rgb8.getBounds() );
	ChannelT<T> y8( img );

	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Flip Vertical " << sizeof(T) * 8 << "-bit : RGB -> RGB" << std::endl;
			SurfaceT<T> s8( rgb8.getWidth(), rgb8.getHeight(), false );
			ip::flipVertical( rgb8, &s8 );
			return gl::Texture::create( s8 );
		}
	);

	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Flip Vertical " << sizeof(T) * 8 << "-bit : RGBA -> RGB" << std::endl;
			SurfaceT<T> s8( rgba8.getWidth(), rgba8.getHeight(), false );
			ip::flipVertical( rgba8, &s8 );
			return gl::Texture::create( s8 );
		}
	);

	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Flip Vertical " << sizeof(T) * 8 << "-bit : RGBA -> RGBA" << std::endl;
			SurfaceT<T> s8( rgba8.getWidth(), rgba8.getHeight(), true );
			ip::flipVertical( rgba8, &s8 );
			return gl::Texture::create( s8 );
		}
	);

	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Flip Vertical " << sizeof(T) * 8 <<  "-bit : RGB -> RGBA" << std::endl;
			SurfaceT<T> s8( rgb8.getWidth(), rgb8.getHeight(), true );
			ip::flipVertical( rgb8, &s8 );
			return gl::Texture::create( s8 );
		}
	);

	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Flip Vertical " << sizeof(T) * 8 << "-bit : Y -> Y" << std::endl;
			ChannelT<T> s8( rgb8.getWidth(), rgb8.getHeight() );
			ip::flipVertical( y8, &s8 );
			return gl::Texture::create( s8 );
		}
	);

	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Flip Vertical " << sizeof(T) * 8 << "-bit : nonplanar Y -> Y" << std::endl;
			ChannelT<T> s8( rgb8.getWidth(), rgb8.getHeight() );
			ip::flipVertical( rgb8.getChannelRed(), &s8 );
			return gl::Texture::create( s8 );
		}
	);
}

void imageProcessingApp::setup()
{
	// 8 BIT FLIP VERTICAL
	setupFlipVertical<uint8_t>();

	// 16 BIT FLIP VERTICAL
	setupFlipVertical<uint16_t>();

	// 32 BIT FLIP VERTICAL
	setupFlipVertical<float>();
	
	mCurFnIdx = 0;
	mCurTex = mTexGenFns[mCurFnIdx]();
}

void imageProcessingApp::mouseDown( MouseEvent event )
{
	mCurFnIdx = ( mCurFnIdx + 1 ) % mTexGenFns.size();
	mCurTex = mTexGenFns[mCurFnIdx]();
}

void imageProcessingApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 

	gl::draw( mCurTex );
}

CINDER_APP( imageProcessingApp, RendererGl )
