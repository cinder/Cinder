#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ip/flip.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class imageProcessingApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void draw();
	
	gl::TextureRef		mCurTex;
	
	size_t										mCurFnIdx;
	vector<function<gl::TextureRef(void)>>		mTexGenFns;
};

void imageProcessingApp::setup()
{
	auto img = loadImage( loadAsset( "upside_down.png" ) );

	// 8 BIT FLIP VERTICAL
	{
	Surface8u rgb8( img );
	Surface8u rgba8( rgb8.getWidth(), rgb8.getHeight(), true );
	rgba8.copyFrom( rgb8, rgb8.getBounds() );

	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Flip Vertical: RGB -> RGB" << std::endl;
			Surface s8( rgb8.getWidth(), rgb8.getHeight(), false );
			ip::flipVertical( rgb8, &s8 );
			return gl::Texture::create( s8 );
		}
	);

	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Flip Vertical 8bit: RGBA -> RGB" << std::endl;
			Surface s8( rgba8.getWidth(), rgba8.getHeight(), false );
			ip::flipVertical( rgba8, &s8 );
			return gl::Texture::create( s8 );
		}
	);

	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Flip Vertical 8bit: RGBA -> RGBA" << std::endl;
			Surface s8( rgba8.getWidth(), rgba8.getHeight(), true );
			ip::flipVertical( rgba8, &s8 );
			return gl::Texture::create( s8 );
		}
	);

	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Flip Vertical 8bit: RGB -> RGBA" << std::endl;
			Surface s8( rgb8.getWidth(), rgb8.getHeight(), true );
			ip::flipVertical( rgb8, &s8 );
			return gl::Texture::create( s8 );
		}
	);
	}

	// 32 BIT FLIP VERTICAL
	{
	Surface32f rgb32f( img );
	Surface32f rgba32f( rgb32f.getWidth(), rgb32f.getHeight(), true );
	rgba32f.copyFrom( rgb32f, rgb32f.getBounds() );

	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Flip Vertical 32bit: RGB -> RGB" << std::endl;
			Surface32f s32f( rgb32f.getWidth(), rgb32f.getHeight(), false );
			ip::flipVertical( rgb32f, &s32f );
			return gl::Texture::create( s32f );
		}
	);

	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Flip Vertical 32bit: RGBA -> RGB" << std::endl;
			Surface32f s32f( rgba32f.getWidth(), rgba32f.getHeight(), false );
			ip::flipVertical( rgba32f, &s32f );
			return gl::Texture::create( s32f );
		}
	);

	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Flip Vertical 32bit: RGBA -> RGBA" << std::endl;
			Surface32f s32f( rgba32f.getWidth(), rgba32f.getHeight(), true );
			ip::flipVertical( rgba32f, &s32f );
			return gl::Texture::create( s32f );
		}
	);

	mTexGenFns.push_back( [=](void)->gl::TextureRef {
			app::console() << "Flip Vertical 32bit: RGB -> RGBA" << std::endl;
			Surface32f s32f( rgb32f.getWidth(), rgb32f.getHeight(), true );
			ip::flipVertical( rgb32f, &s32f );
			return gl::Texture::create( s32f );
		}
	);
	}
	
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

CINDER_APP_NATIVE( imageProcessingApp, RendererGl )
