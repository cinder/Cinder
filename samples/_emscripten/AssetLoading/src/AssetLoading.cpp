#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/emscripten/CinderEmscripten.h"
#include "cinder/Log.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/emscripten/EmscriptenVideo.h"

using namespace ci;
using namespace ci::app;
using namespace ci::em;

//TODO this is temporary - this function isn' getting bundled in for some reason currently. 
EMSCRIPTEN_BINDINGS( CinderEEmscripten ) {
		emscripten::class_<std::function<void( emscripten::val e )> >( "Lislback" )
		.constructor<>()
		.function( "onload", &std::function<void( emscripten::val e )>::operator());
	};
/**
 * This is just a simple test of loading images and a video while demonstrating 
 * examples of how loading assets would work. 
 * 
 * If you're loading from the resources folder, those items will get packaged into a binary file and things should work like normal. 
 * 
 * The difference being that, loading resources will be much faster as that will be ready and all loaded when 
 * your project starts. 
 * 
 * The downside is that bundling your assets with your build WASM file can result in a much larger bundle size resulting in a longer time to screeen.
 * 
 * When loading assets, unfortunately, that will require an http request which can slow things down a bit depending
 * on the size and where you're loading from. 
 * 
 * To make for an even greater headache - testing seems to indicate that bundling larger files into resources 
 * will not quite work(in addition to greatly increasing your project's load time.) though this 
 * may have changed in newer versions. 
 * 
 * It's recommended to just use an assets folder and use the various loadAsset methods, especially the async one.  
 * 
 */
class AssetLoadTest : public App 
{
  gl::TextureRef mAsyncTest,mSyncTest;
  EmscriptenVideoRef mVideo;

  public:
	void draw() override;
  	void setup() override;
};

void prepareSettings( AssetLoadTest::Settings* settings )
{
	settings->setWindowSize (1024,768 );
}

void AssetLoadTest::setup()
{
	// this is an example of syncronous loading - should work exactly like normal but may block other things while 
	// it's loading.
	mSyncTest = gl::Texture::create( loadImage( loadAsset( "loadSync.jpg" ) ) );

	// example of loading assets asyncronously. Pass in a lambda that will receive a DataSourceRef
	app::loadAssetAsync( "loadAsync.jpg",[=]( ci::DataSourceRef ref )->void {
		mAsyncTest = gl::Texture::create( loadImage( ref ) );
	});

	// example of loading something a bit larger like a video. Should work normally. This happens to be 
	// async as well technically because it's using Javascript under the hood to load the video. 
	mVideo = EmscriptenVideo::create( loadAsset( "sintel.ogv" ) );

}


void AssetLoadTest::draw()
{
	// Clear the contents of the window. This call will clear
	// both the color and depth buffers.
	gl::clear( Color::gray( 0.1f ) );

	if( mAsyncTest )
	{
		gl::draw( mAsyncTest,Rectf( 0,0,341,app::getWindowHeight() ) );
	}

	if( mVideo->hasVideoLoaded() ) 
	{
		if( mVideo->isPlaying() ) 
		{
			mVideo->updateTexture();
		}
		else { 
			mVideo->play();
		}

		gl::pushMatrices();
		gl::translate( vec2( 341,0) );
		gl::draw( mVideo->getTexture(),Rectf( 0,0,341,app::getWindowHeight() ) );
		gl::popMatrices();
	}

	if( mSyncTest )
	{
		gl::pushMatrices();
		gl::translate( vec2(341 * 2,0) );
		gl::draw( mSyncTest,Rectf( 0,0,341,app::getWindowHeight() ) );
		gl::popMatrices();
	}
}

// This line tells Cinder to actually create and run the application.
CINDER_APP( AssetLoadTest, RendererGl, prepareSettings )



