#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/vk/vk.h"
#include "cinder/GeomIo.h"
#include "cinder/ImageIo.h"
using namespace ci;
using namespace ci::app;
using namespace std;

/** \class BasicApp
 *
 */
class Float32TextureApp : public App {
public:
	void setup() override;
	void update() override;
	void draw() override;

private:
	vk::TextureRef	mTex8u;
	vk::TextureRef	mTex32f;

	void drawBlendingTests();
};

void Float32TextureApp::setup()
{
	Surface surf8u = Surface( loadImage( getAssetPath( "bloom.jpg" ) ) );
	mTex8u = vk::Texture::create( surf8u );

	Surface32f surf32f = Surface32f( surf8u );
	mTex32f = vk::Texture::create( surf32f );
}

void Float32TextureApp::update()
{
}

void Float32TextureApp::draw()
{
	Rectf w = Rectf( 0, 0, getWindowWidth()/2.0f, getWindowHeight() );
	
	Rectf r0 = Rectf( mTex8u->getBounds() ).getCenteredFit( w, false ).scaledCentered( 0.9f );
	vk::draw( mTex8u, r0 );

	Rectf r1 = Rectf( mTex32f->getBounds() ).getCenteredFit( w, false ).scaledCentered( 0.9f );
	vk::draw( mTex32f, r1 + vec2( w.getWidth(), 0.0f ) );
}

CINDER_APP( Float32TextureApp, RendererVk )