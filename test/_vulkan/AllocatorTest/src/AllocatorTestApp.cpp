#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/vk/vk.h"
#include "cinder/GeomIo.h"
#include "cinder/ImageIo.h"
using namespace ci;
using namespace ci::app;
using namespace std;

/** \class AllocatorTestApp
 *
 */
class AllocatorTestApp : public App {
public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

private:
	vk::TextureRef	mTex;

	void drawBlendingTests();
};

void AllocatorTestApp::setup()
{
	mTex = vk::Texture::create( *Surface::create( loadImage( getAssetPath( "bloom.jpg" 	) ) ) );
}

void AllocatorTestApp::mouseDown( MouseEvent event )
{
}

void AllocatorTestApp::update()
{
}

void AllocatorTestApp::drawBlendingTests()
{
	vk::enableAlphaBlending();
	{
		vk::ScopedColor color( ColorA( 1, 0, 0, 0.5f ) );
		vk::drawSolidRect( Rectf( 50, 50, 250, 250 ) );
	}

	{
		vk::ScopedColor color( ColorA( 0, 1, 0, 0.5f ) );
		vk::drawSolidRect( Rectf( 50, 50, 250, 250 ) + vec2( 50, 50 ) );
	}

	{
		vk::ScopedColor color( ColorA( 0, 0, 1, 0.5f ) );
		vk::drawSolidRect( Rectf( 50, 50, 250, 250 ) + vec2( 100, 100 ) );
	}

	vk::enableAdditiveBlending();
	{
		vk::ScopedColor color( ColorA( 1, 0, 0, 0.5f ) );
		vk::drawSolidRect( Rectf( 350, 50, 550, 250 ) );
	}

	{
		vk::ScopedColor color( ColorA( 0, 1, 0, 0.5f ) );
		vk::drawSolidRect( Rectf( 350, 50, 550, 250 ) + vec2( 50, 50 ) );
	}

	{
		vk::ScopedColor color( ColorA( 0, 0, 1, 0.5f ) );
		vk::drawSolidRect( Rectf( 350, 50, 550, 250 ) + vec2( 100, 100 ) );
	}
}

void AllocatorTestApp::draw()
{
	vk::cullMode( VK_CULL_MODE_BACK_BIT );
	vk::disableBlending();
	vk::draw( mTex, getWindowBounds() );

	drawBlendingTests();
}

CINDER_APP( AllocatorTestApp, RendererVk )