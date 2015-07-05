#include "cinder/app/cocoa/AppCocoaTouch.h"
#include "cinder/app/cocoa/CinderViewCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/gl/gl.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/gl/Shader.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"
#include "cinder/System.h"
#include "cinder/Text.h"
#include "cinder/Log.h"
#include "cinder/app/cocoa/PlatformCocoa.h"

#import <UIKit/UIKit.h>

#include <map>
#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

struct TouchPoint {
	TouchPoint() {}
	TouchPoint( const vec2 &initialPt, const Color &color ) : mColor( color ), mTimeOfDeath( -1.0 ) 
	{
		mLine.push_back( initialPt ); 
	}
	
	void addPoint( const vec2 &pt ) { mLine.push_back( pt ); }
	
	void draw() const
	{
		if( mTimeOfDeath > 0 ) // are we dying? then fade out
			gl::color( ColorA( mColor, ( mTimeOfDeath - getElapsedSeconds() ) / 2.0f ) );
		else
			gl::color( mColor );

		gl::draw( mLine );
	}
	
	void startDying() { mTimeOfDeath = getElapsedSeconds() + 2.0f; } // two seconds til dead
	
	bool isDead() const { return getElapsedSeconds() > mTimeOfDeath; }
	
	PolyLine2f	mLine;
	Color			mColor;
	float			mTimeOfDeath;
};

struct TestCallbackOrder {
	TestCallbackOrder() : mState( TestCallbackOrder::VIRGIN ), mDone( false ) {}
	
	void	setState( int state ) {
		if( mDone ) return;
		
		if( mState != state - 1 )
			CI_LOG_E( "Fail at state: " << mState << "->" << state );
		else
			mState = state;
		if( mState == DRAW )
			mDone = true;
	}
	
	enum { VIRGIN, PREPARESETTINGS, SETUP, RESIZE, UPDATE, DRAW };
	
	bool	mDone;
	int		mState;
};

static string orientationString( InterfaceOrientation orientation )
{
	switch (orientation) {
		case InterfaceOrientation::Portrait: return "Portrait";
		case InterfaceOrientation::PortraitUpsideDown: return "PortraitUpsideDown";
		case InterfaceOrientation::LandscapeLeft: return "LandscapeLeft";
		case InterfaceOrientation::LandscapeRight: return "LandscapeRight";
		default: return "Error";
	}
}

TestCallbackOrder	sOrderTester;

class iosAppTestApp : public AppCocoaTouch {
  public:
	static 	void prepareSettings( AppCocoaTouch::Settings *settings );

	iosAppTestApp();

	void	setup()								override;
	void	resize()							override;
	void	update()							override;
	void 	draw()								override;
	
	void	keyDown( KeyEvent event )			override;
	void	mouseDown( MouseEvent event )		override;
	void	mouseUp( MouseEvent event )			override;
	void	mouseDrag( MouseEvent event )		override;

	void	touchesBegan( TouchEvent event )	override;
	void	touchesMoved( TouchEvent event )	override;
	void	touchesEnded( TouchEvent event )	override;

	void didEnterBackground();
	void willEnterForeground();
	void willResignActive();
	void didBecomeActive();
	void shuttingDown();
	
	void proximitySensor( bool isClose );
	void batteryStateChange( bool isUnplugged );
	void memoryWarning();

	uint32_t	supportAllOrientations();
	uint32_t	supportPortraitOrientations();
	void		willRotate();
	void		didRotate();

	mat4				mCubeRotation;
	gl::TextureRef		mTex;
	gl::TextureRef		mTextTex;
	gl::TextureFontRef	mFont;
	CameraPersp			mCam;
	WindowRef			mSecondWindow;
	string				mSecondWindowMessage;
	
	map<uint32_t,TouchPoint>	mActivePoints;
	list<TouchPoint>			mDyingPoints;
	int							mMouseTouchId; // gives a unique ID to each click to emulate multitouch
	bool mMultipleDisplays = false;
};

// static
void iosAppTestApp::prepareSettings( AppCocoaTouch::Settings *settings )
{
	sOrderTester.setState( TestCallbackOrder::PREPARESETTINGS );

	// THIS DOES NOT WORK ON iOS - Can't query displays in prepareSettings
//	for( auto &display : Display::getDisplays() )
//		CI_LOG_V( *display );

//	settings->setMultiTouchEnabled( false );
//	settings->enableHighDensityDisplay( false ); // FIXME: currently doesn't do anything
	settings->setPowerManagementEnabled( false );
//	settings->setStatusBarEnabled( true ); // disabled by default
}

iosAppTestApp::iosAppTestApp()
{
	CI_LOG_V( "Displays" );
	for( auto &display : Display::getDisplays() )
		CI_LOG_V( *display );
}

void iosAppTestApp::setup()
{
	UIButton *btn = [UIButton buttonWithType:UIButtonTypeRoundedRect];
	btn.bounds = CGRectMake(0, 100, 200, 26);
	btn.center = CGPointMake( getWindowCenter().x, getWindowHeight() - 50 );
	btn.autoresizingMask = UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleBottomMargin | UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleRightMargin;
	btn.backgroundColor = [UIColor clearColor];
	[btn setTitle:@"An Exciting New Button" forState:UIControlStateNormal];
	[btn sizeToFit];
	CinderViewCocoaTouch *cinderView = reinterpret_cast<CinderViewCocoaTouch *>( getWindow()->getNative() ) ;
	[cinderView addSubview:btn];

	mMouseTouchId = 0;
	sOrderTester.setState( TestCallbackOrder::SETUP );

	getSignalProximitySensor().connect( std::bind( &iosAppTestApp::proximitySensor, this, std::placeholders::_1 ) );
	enableProximitySensor();

	getSignalBatteryState().connect( std::bind( &iosAppTestApp::batteryStateChange, this, std::placeholders::_1 ) );
	enableBatteryMonitoring();
	
	mFont = gl::TextureFont::create( Font( "Helvetica", 16 ) );

	// Create a blue-green gradient as an OpenGL texture
	Surface8u surface( 256, 256, false );
	Surface8u::Iter iter = surface.getIter();
	while( iter.line() ) {
		while( iter.pixel() ) {
			iter.r() = 0;
			iter.g() = iter.x();
			iter.b() = iter.y();
		}
	}

	mTex = gl::Texture::create( surface );

	CI_LOG_V( "window size: " << getWindowSize() << ", window content scale: " << getWindowContentScale() );

	getWindow()->getSignalDraw().connect( std::bind( &iosAppTestApp::draw, this ) );

	auto displays = Display::getDisplays();
	if( displays.size() > 1 )
		createWindow( Window::Format().display( displays[1] ) );

	if( getNumWindows() > 1 ) {
		mSecondWindow = getWindowIndex( 1 );
		mSecondWindow->getSignalDraw().connect( std::bind( &iosAppTestApp::draw, this ) );
	}

	getSignalDidEnterBackground().connect( bind( &iosAppTestApp::didEnterBackground, this ) );
	getSignalWillEnterForeground().connect( bind( &iosAppTestApp::willEnterForeground, this ) );
	getSignalWillResignActive().connect( bind( &iosAppTestApp::willResignActive, this ) );
	getSignalDidBecomeActive().connect( bind( &iosAppTestApp::didBecomeActive, this ) );
	getSignalCleanup().connect( bind( &iosAppTestApp::shuttingDown, this ) );
	getSignalMemoryWarning().connect( bind( &iosAppTestApp::memoryWarning, this ) );

	getSignalSupportedOrientations().connect( std::bind( &iosAppTestApp::supportAllOrientations, this ) );
//	getSignalSupportedOrientations().connect( std::bind( &iosAppTestApp::supportPortraitOrientations, this ) ); // this one limits to only portrait, even though slot above says all
	getSignalWillRotate().connect( std::bind( &iosAppTestApp::willRotate, this ) );
	getSignalDidRotate().connect( std::bind( &iosAppTestApp::didRotate, this ) );

	CI_LOG_V( "Device is " << ( System::isDeviceIpad() ? "iPad" : "iPhone" ) );

	CI_LOG_V( " ### num Windows: " << getNumWindows() );

}

void iosAppTestApp::didEnterBackground()
{
	CI_LOG_V( "Did enter background!" );
}

void iosAppTestApp::willEnterForeground()
{
	CI_LOG_V( "Will enter foreground!" );
}

void iosAppTestApp::willResignActive()
{
	CI_LOG_V( "Will resign active!" );
}

void iosAppTestApp::didBecomeActive()
{
	CI_LOG_V( "Did become active!" );
}

void iosAppTestApp::shuttingDown()
{
	CI_LOG_V( "Shutting down!" );
}

void iosAppTestApp::memoryWarning()
{
	CI_LOG_V( "Memory warning!" );
}

uint32_t iosAppTestApp::supportAllOrientations()
{
	CI_LOG_V( "Supporting all orientations." );
	return InterfaceOrientation::All;
}

uint32_t iosAppTestApp::supportPortraitOrientations()
{
	CI_LOG_V( "Supporting only portrait orientations." );
	return InterfaceOrientation::PortraitAll;
}

void iosAppTestApp::willRotate()
{
	InterfaceOrientation orientation = getOrientation();
	CI_LOG_V( __PRETTY_FUNCTION__ << "orientation:" << orientation );
}

void iosAppTestApp::didRotate()
{
	InterfaceOrientation orientation = getOrientation();
	CI_LOG_V( __PRETTY_FUNCTION__ << "orientation:" << orientation );
}

void iosAppTestApp::resize()
{
	sOrderTester.setState( TestCallbackOrder::RESIZE );
	CI_LOG_V( "Resize!" );

	CI_LOG_V( "window size: " << getWindowSize() );
}

void iosAppTestApp::mouseDown( MouseEvent event )
{
	Color newColor( CM_HSV, Rand::randFloat(), 1, 1 );
	CI_LOG_V( "Starting with: " << event.getPos() );
	mActivePoints.insert( make_pair( ++mMouseTouchId, TouchPoint( event.getPos(), newColor ) ) );
}

void iosAppTestApp::mouseUp( MouseEvent event )
{
	mActivePoints[mMouseTouchId].startDying();
	mDyingPoints.push_back( mActivePoints[mMouseTouchId] );
	mActivePoints.erase( mMouseTouchId );
}

void iosAppTestApp::mouseDrag( MouseEvent event )
{
	mActivePoints[mMouseTouchId].addPoint( event.getPos() );
}

void iosAppTestApp::touchesBegan( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		Color newColor( CM_HSV, Rand::randFloat(), 1, 1 );
		mActivePoints.insert( make_pair( touchIt->getId(), TouchPoint( touchIt->getPos(), newColor ) ) );
	}
	CI_LOG_V( "Width: " << getWindowWidth() );
}

void iosAppTestApp::touchesMoved( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt )
		mActivePoints[touchIt->getId()].addPoint( touchIt->getPos() );
}

void iosAppTestApp::touchesEnded( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		mActivePoints[touchIt->getId()].startDying();
		mDyingPoints.push_back( mActivePoints[touchIt->getId()] );
		mActivePoints.erase( touchIt->getId() );
	}
	
	if( isKeyboardVisible() )
		hideKeyboard();
	else {
		showKeyboard();
		mSecondWindowMessage.clear();
	}
	
	if( isStatusBarVisible() )
		hideStatusBar( StatusBarAnimation::FADE );
	else {
		showStatusBar( StatusBarAnimation::SLIDE );
	}
	CI_LOG_V( getWindowSize() );
	
//	setFrameRate( 12.0f );
}

void iosAppTestApp::keyDown( KeyEvent event )
{
	CI_LOG_V( "Key: " << event.getCharUtf32() );
	if( event.getCode() == KeyEvent::KEY_BACKSPACE )
		CI_LOG_V( "backspace!" );
	else if( event.getCode() == KeyEvent::KEY_RETURN )
		CI_LOG_V( "return!" );

	mSecondWindowMessage = getKeyboardString();
}

void iosAppTestApp::proximitySensor( bool isClose )
{
	CI_LOG_V( "Device is" << ( isClose ? " " : " NOT " ) << "close to your head!" );
}

void iosAppTestApp::batteryStateChange( bool isUnplugged )
{
	CI_LOG_V( "Power is" << ( isUnplugged ? " " : " NOT " ) << "unplugged!" );
}

void iosAppTestApp::update()
{
	sOrderTester.setState( TestCallbackOrder::UPDATE );

	mCubeRotation *= rotate( 0.03f, vec3( 1 ) );
}

void iosAppTestApp::draw()
{
	gl::enableAlphaBlending();
	gl::enableDepthRead();
	sOrderTester.setState( TestCallbackOrder::DRAW );
	CameraPersp mCam;
	mCam.lookAt( vec3( 3, 2, -3 ), vec3( 0 ) );
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );

	if( getDisplay() == Display::getMainDisplay() )
		gl::clear( Color( 1.2f, 0.2f, 0.3f ) );
	else
		gl::clear( Color( 0.4f, 0.2f, 0.2f ) );		

	gl::color( Color::white() );

	gl::bindStockShader( gl::ShaderDef().color().texture() );
	mTex->bind();

	gl::setMatrices( mCam );
	{
		gl::ScopedModelMatrix modelScope;
		gl::multModelMatrix( mCubeRotation );
		gl::drawCube( vec3( 0 ), vec3( 2 ) );
	}

	gl::setMatricesWindow( getWindowSize() );
	for( map<uint32_t,TouchPoint>::const_iterator activeIt = mActivePoints.begin(); activeIt != mActivePoints.end(); ++activeIt ) {
		activeIt->second.draw();
	}

	for( list<TouchPoint>::iterator dyingIt = mDyingPoints.begin(); dyingIt != mDyingPoints.end(); ) {
		dyingIt->draw();
		if( dyingIt->isDead() )
			dyingIt = mDyingPoints.erase( dyingIt );
		else
			++dyingIt;
	}
	
	// draw yellow circles at the active touch points
	gl::color( Color( 1, 1, 0 ) );
	for( auto touch : getActiveTouches() )
		gl::drawStrokedCircle( touch.getPos(), 20.0f );
			
	if( getWindow() == mSecondWindow || Display::getDisplays().size() < 2 ) {
		static Font font = Font( "Arial", 48 );
		static std::string lastMessage;
		static gl::TextureRef messageTex;
		if( lastMessage != mSecondWindowMessage ) {
			TextBox box = TextBox().font( font ).text( mSecondWindowMessage );
			messageTex = gl::Texture::create( box.render() );
			lastMessage = mSecondWindowMessage;
		}
		if( messageTex ) {
			gl::color( Color::white() );
			gl::draw( messageTex, ivec2( ( getWindowWidth() - messageTex->getWidth() ) / 2, getWindowCenter().y ) );
		}
	}

	gl::disableDepthRead();
	gl::color( Color( 0.0f, 1.0f, 0.0f ) );
	mFont->drawString( orientationString( getWindowOrientation() ) + "@ " + toString( getWindowContentScale() ), vec2( 10.0f, 60.0f ) );
//	gl::drawStringCentered( "Orientation: " + orientationString( getInterfaceOrientation() ), vec2( getWindowCenter().x, 30.0f ), Color( 0.0f, 1.0f, 0.0f ), Font::getDefault() ); // ???: why not centered?

	mFont->drawString( toString( floor(getAverageFps()) ) + " fps", vec2( 10.0f, 90.0f ) );
	mFont->drawString( "Displays " + toString( Display::getDisplays().size() ), vec2( 10.0f, 140.0f ) );	
}

CINDER_APP_COCOA_TOUCH( iosAppTestApp, RendererGl( RendererGl::Options().msaa( 0 ) ), iosAppTestApp::prepareSettings )
