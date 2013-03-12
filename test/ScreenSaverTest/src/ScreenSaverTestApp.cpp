#include "cinder/Cinder.h"
#include "cinder/Utilities.h"
#include "cinder/app/AppScreenSaver.h"
#include "cinder/Color.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"

#include "Resources.h"
#include "Configuration.h"
#if defined( CINDER_MAC )
	#include "MacConfigDialog.h"
#elif defined( CINDER_MSW )
	#include "WindowsConfig.h"
#endif

using namespace ci;
using namespace ci::app;

class ScreenSaverTestApp : public AppScreenSaver {
  public:
	virtual void prepareSettings( Settings *settings );
	virtual void setup();
	virtual void resize();
	virtual void update();
	virtual void draw();
	virtual void shutdown();

#if defined( CINDER_MAC )
	virtual NSWindow* createMacConfigDialog() override {
		return getConfigDialogMac( this, &mConfig ); // defined in MacConfigDialog.cpp
	}
#elif defined( CINDER_MSW )
	static BOOL doConfigureDialog( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam ) {
		return mswDoConfigureDialog( hDlg, message, wParam, lParam ); // defined in WindowsConfig.cpp
	}
#endif

  protected:
	Configuration	mConfig;
	ci::Color		mColor, mBackgroundColor;
	float			mRadius;
	gl::Texture		mLogo;
};


void ScreenSaverTestApp::prepareSettings( Settings *settings )
{
console() << "prepareSettings() called in " << getAppPath() << std::endl;
//	settings->setFrameRate( 1 );
#if defined( CINDER_MAC )
	settings->setProvidesMacConfigDialog();
#endif
//	settings->enableSecondaryDisplayBlanking();
}

void ScreenSaverTestApp::setup()
{
	console() << "setup" << std::endl;
#if defined( CINDER_MAC )
	loadConfigMac( this, &mConfig );
#else
	loadConfig( &mConfig );
#endif
	mColor = Color( 1.0f, 0.5f, 0.25f );
	mBackgroundColor = Color( 0.7f, 0.0f, 0.8f );
	
	try {
		mLogo = loadImage( loadResource( RES_CINDER_LOGO ) );
	}
	catch( std::exception &e ) {
#if defined( CINDER_MAC )
NSLog( @"setup exc: %s", e.what() );
#endif
throw e;
	}
}

void windowClosedCallback()
{
#if defined( CINDER_MAC )
	NSLog( @"Closing window." );
#else
	console() << "Closing window: " << app::getWindow() << std::endl;
#endif
}

void ScreenSaverTestApp::resize()
{
	console() << "resizing: " << getWindowSize() << std::endl;
	getWindow()->getSignalClose().connect( windowClosedCallback );
}

void ScreenSaverTestApp::update()
{
#if defined( CINDER_MAC )
	NSLog( @"update " );
#endif
	mRadius = (float)abs( cos( getElapsedSeconds() ) * getWindowHeight() / 2 );
}

void ScreenSaverTestApp::draw()
{
//NSLog( @"draw: %dx%d (%d) [%d]", getWindowWidth(), getWindowHeight(), getNumWindows(), isPreview() );
	gl::enableAlphaBlending();
		
//console() << "Drawing: " << *getDisplay() << std::endl;
	if( isPreview() )
		gl::clear( Color( 0, 0.5f, 1.0f ) );
	else
		gl::clear( mBackgroundColor );
	gl::color( mColor );
	//gl::drawSolidCircle( getWindowCenter(), mRadius );
	gl::drawSolidRect( Rectf( getWindowCenter() - Vec2f( mRadius, mRadius ), getWindowCenter() + Vec2f( mRadius, mRadius ) ) );
	
	if( mConfig.mDrawCinderLogo ) {
		gl::color( Color::white() );
		gl::draw( mLogo, getWindowCenter() - mLogo.getSize() / 2 );
	}
}

void ScreenSaverTestApp::shutdown()
{
	console() << "shutting down" << std::endl;
}

CINDER_APP_SCREENSAVER( ScreenSaverTestApp, RendererGl )
