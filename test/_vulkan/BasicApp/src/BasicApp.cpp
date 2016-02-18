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
class BasicApp : public App {
public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

private:
	vk::TextureRef	mTex;
};

void BasicApp::setup()
{
	mTex = vk::Texture::create( *Surface::create( loadImage( getAssetPath( "bloom.jpg" 	) ) ) );
}

void BasicApp::mouseDown( MouseEvent event )
{
}

void BasicApp::update()
{
}

void BasicApp::draw()
{
	//vk::cullMode( VK_CULL_MODE_NONE );
	vk::draw( mTex, getWindowBounds() );
}

CINDER_APP( BasicApp, RendererVk )
