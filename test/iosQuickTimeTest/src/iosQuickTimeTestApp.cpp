#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/gl/Texture.h"
#include "cinder/qtime/QuickTimeGl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class iosQuickTimeTestApp : public App {
  public:
	void prepareSettings( Settings *settings ) { settings->setMultiTouchEnabled( false ); }
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void update() override;
	void draw() override;
	
	qtime::MovieGlRef		mMovie;
	gl::TextureRef			mFrameTexture;
	vector<gl::TextureRef>	mSavedTextures;
};

void iosQuickTimeTestApp::setup()
{
	auto filePath = getAssetPath( "test.mov" );
	if( filePath.empty() || ( ! fs::exists( filePath ) ) ) {
		CI_LOG_E( "This test requires there to be a file test.mov in the app's assets directory." );
	}
	else {
		mMovie = qtime::MovieGl::create( filePath );
		mMovie->setLoop();
		mMovie->play();
	}
}

void iosQuickTimeTestApp::mouseDrag( MouseEvent event )
{
	if( mFrameTexture )
		mSavedTextures.push_back( mFrameTexture );
}

void iosQuickTimeTestApp::mouseDown( MouseEvent event )
{
	mSavedTextures.clear();
}

void iosQuickTimeTestApp::update()
{
	if( mMovie )
		mFrameTexture = mMovie->getTexture();
}

void iosQuickTimeTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	if( mFrameTexture ) {
		Rectf centeredRect = Rectf( mFrameTexture->getBounds() ).getCenteredFit( getWindowBounds(), true );
		gl::draw( mFrameTexture, centeredRect - vec2( 0, centeredRect.y1 ) );
	}
	
	// draw saved textures
	float savedWidth = getWindowWidth() / (float)mSavedTextures.size();
	float savedHeight = savedWidth / mMovie->getAspectRatio();
	for( auto savedIt = mSavedTextures.begin(); savedIt != mSavedTextures.end(); ++savedIt ) {
		float left = (savedIt - mSavedTextures.begin()) * savedWidth;
		gl::draw( *savedIt, Rectf( left, getWindowHeight() - savedHeight, left + savedWidth, getWindowHeight() ) );
	}
}

CINDER_APP( iosQuickTimeTestApp, RendererGl )
