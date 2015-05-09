#define USE_DIRECTX

#include "cinder/app/AppNative.h"
#if defined( USE_DIRECTX )
  #include "cinder/app/RendererDx.h"
  #include "cinder/dx/DxTexture.h"
#else 
  #include "cinder/gl/gl.h"
  #include "cinder/gl/Texture.h"
#endif
#include "cinder/ImageIo.h"
#include "cinder/Surface.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#pragma comment( lib, "D3D11" )

#if defined( USE_DIRECTX )
  #define grfx dx
#else
  #define grfx gl
#endif

enum Mode {
	CHANNEL8U,
	CHANNEL32F,
	SURFACE8U,
	SURFACE32F
};

class TexturesApp : public AppNative {
public:
	void setup();
	void keyDown( KeyEvent event );
	void mouseDown( MouseEvent event );	
	void mouseDrag( MouseEvent event );	
	void mouseUp( MouseEvent event );	
	void update();
	void draw();

private:
	Surface8u			mSurfaces[2];
	int					mSurfaceIndex;
	Surface8u			getSurface();

	Mode				mMode;
	grfx::TextureRef	mTex;
	std::string			mMsg;

	Vec2i				mP0;
	Vec2i				mP1;
	bool				mDrawRect;
	bool				mDoUpdate;
};

Surface8u TexturesApp::getSurface() 
{
	++mSurfaceIndex;
	mSurfaceIndex %= 2;
	return mSurfaces[mSurfaceIndex];
}

void TexturesApp::setup()
{
	mDrawRect = false;
	mDoUpdate = false;

	mSurfaceIndex = 0;
	mSurfaces[0] = loadImage( loadAsset( "PurpleFlower.jpg" ) );
	mSurfaces[1] = loadImage( loadAsset( "VegasMall.jpg" ) );
		
	mTex = grfx::Texture::create( getSurface() );
	mMode = SURFACE8U;
}

void TexturesApp::keyDown( KeyEvent event )
{
	switch( event.getCode() ) {
	case KeyEvent::KEY_1:
	{
		Channel8u chan8u = getSurface().getChannel( 0 );
		if( mTex && (DXGI_FORMAT_R8_UNORM == mTex ->getInternalFormat()) ) {
			mTex->update( chan8u );
			mMsg = "Updated using Channel8u";
		}
		else {
			mTex = grfx::Texture::create( chan8u );
			mMsg = "Created using Channel8u";
		}

		mMode = CHANNEL8U;
	}
	break;

	case KeyEvent::KEY_2:
	{
		Channel8u chan8u = getSurface().getChannel( 1 );
		Channel32f chan32f = Channel32f( chan8u );
		if( mTex && (DXGI_FORMAT_R32_FLOAT == mTex ->getInternalFormat()) ) {
			mTex->update( chan32f );
			mMsg = "Updated using Channel32f";
		}
		else {
			mTex = grfx::Texture::create( chan32f );
			mMsg = "Created using Channel32f";
		}

		mMode = CHANNEL32F;
	}
	break;

	case KeyEvent::KEY_3:
	{
		Surface8u surf8u = getSurface();
		if( mTex && (DXGI_FORMAT_R8G8B8A8_UNORM == mTex ->getInternalFormat()) ) {
			mTex->update( surf8u );
			mMsg = "Updated using Surface8u";
		}
		else {
			mTex = grfx::Texture::create( surf8u );
			mMsg = "Created using Surface8u";
		}

		mMode = SURFACE8U;
	}
	break;

	case KeyEvent::KEY_4:
	{
		Surface8u surf8u = getSurface();
		Surface32f surf32f = Surface32f( surf8u );
		if( mTex && (DXGI_FORMAT_R32G32B32A32_FLOAT == mTex ->getInternalFormat()) ) {
			mTex->update( surf32f );
			mMsg = "Updated using Surface32f";
		}
		else {
			mTex = grfx::Texture::create( surf32f );
			mMsg = "Created using Surface32f";
		}

		mMode = SURFACE32F;
	}
	break;
	}
}

void TexturesApp::mouseDown( MouseEvent event )
{
	mP1 = mP0 = event.getPos();
	mDrawRect = true;
}

void TexturesApp::mouseDrag( MouseEvent event )
{
	mP1 = event.getPos();
}

void TexturesApp::mouseUp( MouseEvent event )
{
	mP1 = event.getPos();
	mDrawRect = false;
	mDoUpdate = true;
}

void TexturesApp::update()
{
	if( mDoUpdate ) {
		Area area;
		area.x1 = std::min( mP0.x, mP1.x );
		area.y1 = std::min( mP0.y, mP1.y );
		area.x2 = std::max( mP0.x, mP1.x );
		area.y2 = std::max( mP0.y, mP1.y );

		if( CHANNEL8U == mMode ) {
			Channel8u chan8u = getSurface().getChannel( 0 );
			if( mTex && (DXGI_FORMAT_R8_UNORM == mTex ->getInternalFormat()) ) {
				mTex->update( chan8u, area );
				mMsg = "Updated using Channel8u : Area";
			}
		}
		else if( CHANNEL32F == mMode ) {
			//Channel8u chan8u = getSurface().getChannel( 1 );
			//Channel32f chan32f = Channel32f( chan8u );
			//if( mTex && (DXGI_FORMAT_R32_FLOAT == mTex ->getInternalFormat()) ) {
			//	mTex->update( chan32f, area );
			//	mMsg = "Updated using Channel32f : Area";
			//}
		}
		else if( SURFACE8U == mMode ) {
			Surface8u surf8u = getSurface();
			if( mTex && (DXGI_FORMAT_R8G8B8A8_UNORM == mTex ->getInternalFormat()) ) {
				mTex->update( surf8u, area );
				mMsg = "Updated using Surface8u : Area";
			}			
		}
		else if( SURFACE32F == mMode ) {
			//Surface8u surf8u = getSurface();
			//Surface32f surf32f = Surface32f( surf8u );
			//if( mTex && (DXGI_FORMAT_R32G32B32A32_FLOAT == mTex ->getInternalFormat()) ) {
			//	mTex->update( surf32f, area );
			//	mMsg = "Updated using Surface32f : Area";
			//}
		}

		mDoUpdate = false;
	}
}

void TexturesApp::draw()
{
	grfx::clear( Color( 0, 0, 0 ) );

	grfx::disableAlphaBlending();
	grfx::color( Color( 1, 1, 1  ) );
	grfx::draw( mTex );

	grfx::enableAlphaBlending();
	grfx::color( ColorA( 0, 0, 0, 0.6f ) );
	grfx::drawSolidRect( Rectf( 0, 0, getWindowWidth(), 50.0f ) );

	grfx::drawString( mMsg, Vec2f( 2, 2 ), ci::Color( 1, 1, 1 ), ci::Font( "Arial", 24.0f ) );

	if( mDrawRect ) {
		grfx::color( Color( 0, 1, 0 ) );

		Rectf r;
		r.x1 = std::min( mP0.x, mP1.x );
		r.y1 = std::min( mP0.y, mP1.y );
		r.x2 = std::max( mP0.x, mP1.x );
		r.y2 = std::max( mP0.y, mP1.y );
		grfx::drawStrokedRect( r );
	}
}

#if defined( USE_DIRECTX )
  CINDER_APP_NATIVE( TexturesApp, RendererDx )
#else 
  CINDER_APP_NATIVE( TexturesApp, RendererGl )
#endif 
