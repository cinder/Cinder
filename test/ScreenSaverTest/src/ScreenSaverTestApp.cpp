#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Utilities.h"
#include "cinder/Color.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Log.h"
#include "cinder/System.h"

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
	virtual void prepareSettings( Settings *settings ) override;
	virtual void setup() override;
	virtual void resize() override;
	virtual void update() override;
	virtual void draw() override;
	virtual void shutdown() override;

#if defined( CINDER_MAC )
	virtual NSWindow* createMacConfigDialog() override
	{
		return getConfigDialogMac( this, &mConfig ); // defined in MacConfigDialog.cpp
	}
#elif defined( CINDER_MSW )
	static BOOL doConfigureDialog( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
	{
		return mswDoConfigureDialog( hDlg, message, wParam, lParam ); // defined in WindowsConfig.cpp
	}
#endif

  protected:
	Configuration	mConfig;
	ci::Color		mColor, mBackgroundColor;
	float			mRadius;
	gl::TextureRef	mLogo;
};


void ScreenSaverTestApp::prepareSettings( Settings *settings )
{
	CI_LOG_I( "called in " << getAppPath() );
//	settings->setFrameRate( 1 );
#if defined( CINDER_MAC )
	settings->setProvidesMacConfigDialog();
#endif
//	settings->enableSecondaryDisplayBlanking();
}

void ScreenSaverTestApp::setup()
{
	CI_LOG_I( "bang" );

#if defined( CINDER_MAC )
	loadConfigMac( this, &mConfig );
#else
	loadConfig( &mConfig );
#endif
	mColor = Color( 1.0f, 0.5f, 0.25f );
	mBackgroundColor = Color( 0.7f, 0.0f, 0.8f );
	
	try {
		mLogo = gl::Texture::create( loadImage( loadResource( RES_CINDER_LOGO ) ) );
	}
	catch( std::exception &exc ) {
		CI_LOG_E( "exception caught, type: " << System::demangleTypeName( typeid( exc ).name() ) << ", what: " << exc.what() );
	}
}

void windowClosedCallback()
{
	CI_LOG_I( "closing window: " << app::getWindow() );
}

void ScreenSaverTestApp::resize()
{
	console() << "resizing: " << getWindowSize() << std::endl;
	getWindow()->getSignalClose().connect( windowClosedCallback );
}

void ScreenSaverTestApp::update()
{
	mRadius = (float)abs( cos( getElapsedSeconds() ) * getWindowHeight() / 2 );
}

void ScreenSaverTestApp::draw()
{
//NSLog( @"draw: %dx%d (%d) [%d]", getWindowWidth(), getWindowHeight(), getNumWindows(), isPreview() ); // TODO: update
	gl::enableAlphaBlending();
		
//console() << "Drawing: " << *getDisplay() << std::endl; // TODO: update
	if( isPreview() )
		gl::clear( Color( 0, 0.5f, 1.0f ) );
	else
		gl::clear( mBackgroundColor );

	gl::color( mColor );
	//gl::drawSolidCircle( getWindowCenter(), mRadius );
	gl::drawSolidRect( Rectf( getWindowCenter() - vec2( mRadius, mRadius ), getWindowCenter() + vec2( mRadius, mRadius ) ) );
	
	if( mConfig.mDrawCinderLogo ) {
		gl::color( Color::white() );
		gl::draw( mLogo, getWindowCenter() - vec2( mLogo->getSize() / 2 ) );
	}
}

void ScreenSaverTestApp::shutdown()
{
	console() << "shutting down" << std::endl;
}

CINDER_APP_SCREENSAVER( ScreenSaverTestApp, RendererGl )
