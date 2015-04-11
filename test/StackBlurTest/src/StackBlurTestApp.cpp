#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ip/Blur.h"

using namespace ci;
using namespace ci::app;
using namespace std;

typedef uint16_t T;

class StackBlurTestApp : public App {
  public:
	void setup() override;
	void mouseDrag( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;
	void draw() override;

	void profile();
	
	SurfaceT<T>		mSourceImage, mBlurredImage;
	ChannelT<T>		mSourceChannel, mBlurredChannel;
	gl::TextureRef	mBlurredTex;
};

void StackBlurTestApp::setup()
{
	mSourceImage = SurfaceT<T>( loadImage( loadAsset( "great_wall.jpg" ) ) );
	setWindowSize( mSourceImage.getSize() );
	mBlurredImage = SurfaceT<T>( mSourceImage.getWidth(), mSourceImage.getHeight(), true, SurfaceChannelOrder::RGBA );
	mBlurredImage.copyFrom( mSourceImage, mSourceImage.getBounds() );
	ip::stackBlur( &mBlurredImage, Area( 240, 150, 500, 310 ), 35 );
	mBlurredTex = gl::Texture::create( mBlurredImage );
}

void StackBlurTestApp::mouseDrag( MouseEvent event )
{
	mBlurredImage.copyFrom( mSourceImage, mSourceImage.getBounds() );
	ip::stackBlur( &mBlurredImage, event.getPos().x / 2 );
	mBlurredTex->update( mBlurredImage );
}

void StackBlurTestApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'p' )
		profile();
}

void StackBlurTestApp::profile()
{
	mBlurredImage.copyFrom( mSourceImage, mSourceImage.getBounds() );

	Timer timer( true );
	const int maxRadius = 777;
	const int iterations = 1;
	for( int radius = 0; radius < maxRadius; radius += 1 )
		for( int i = 0; i < iterations; ++i )
			ip::stackBlur( &mBlurredImage, radius );
	
	timer.stop();
	console() << iterations * maxRadius << " iterations in " << timer.getSeconds() << std::endl;
}

void StackBlurTestApp::draw()
{
	gl::clear(); 
	gl::enableAlphaBlending();
	gl::draw( mBlurredTex );
}

CINDER_APP( StackBlurTestApp, RendererGl, []( App::Settings *settings ) {
	settings->setMultiTouchEnabled( false );
} )
