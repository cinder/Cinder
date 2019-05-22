#include "cinder/app/AppScreenSaver.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Utilities.h"
#include "cinder/Color.h"
#include "cinder/gl/gl.h"
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

#define LOAD_LOGO_IN_CONSTRUCTOR 0

using namespace ci;
using namespace ci::app;

class ScreenSaverTestApp : public AppScreenSaver {
  public:
	ScreenSaverTestApp();

	void setup() override;
	void resize() override;
	void update() override;
	void draw() override;

	void loadLogo();

#if defined( CINDER_MAC )
	NSWindow* createMacConfigDialog() override
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


void prepareSettings( AppScreenSaver::Settings *settings )
{
//	settings->setFrameRate( 1 );
#if defined( CINDER_MAC )
	settings->setProvidesMacConfigDialog();
#elif defined( CINDER_MSW )
//	settings->enableDebug();
#endif
//	settings->enableSecondaryDisplayBlanking();
}

ScreenSaverTestApp::ScreenSaverTestApp()
{
#if LOAD_LOGO_IN_CONSTRUCTOR
	//log::manager()->enableSystemLogging();
	std::shared_ptr<log::LoggerSystem> sysLogger = log::makeLogger<log::LoggerSystem>();
	sysLogger->setLoggingLevel(log::LEVEL_WARNING);
	loadLogo();
#endif
}

void ScreenSaverTestApp::setup()
{
	//log::manager()->enableSystemLogging();
	std::shared_ptr<log::LoggerSystem> sysLogger = log::makeLogger<log::LoggerSystem>();
	sysLogger->setLoggingLevel(log::LEVEL_WARNING);

	CI_LOG_I( "called in " << getAppPath() );

#if defined( CINDER_MAC )
	loadConfigMac( this, &mConfig );
#else
	loadConfig( &mConfig );
#endif
	mColor = Color( 1.0f, 0.5f, 0.25f );
	mBackgroundColor = Color( 0.7f, 0.0f, 0.8f );

	getSignalCleanup().connect( [this] {
		CI_LOG_I( "shutting down" );
	} );

#if ! LOAD_LOGO_IN_CONSTRUCTOR
	loadLogo();
#endif
}

void ScreenSaverTestApp::loadLogo()
{
	try {
		mLogo = gl::Texture::create( loadImage( loadResource( RES_CINDER_LOGO ) ) );
		CI_LOG_I( "loaded." );
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
//	CI_LOG_I( "window size: " << getWindowSize() << ", num windows: " << getNumWindows() << ", preview: " << isPreview() );
//	CI_LOG_I( "Drawing" << *getDisplay() );

	gl::enableAlphaBlending();
		
	if( isPreview() )
		gl::clear( Color( 0, 0.95f, 1.0f ) );
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

CINDER_APP_SCREENSAVER( ScreenSaverTestApp, RendererGl, prepareSettings )
