#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/CinderViewCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"
#include "cinder/System.h"
#include "cinder/Text.h"

#import <UIKit/UIKit.h>

#include <map>
#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

struct TouchPoint {
	TouchPoint() {}
	TouchPoint( const Vec2f &initialPt, const Color &color ) : mColor( color ), mTimeOfDeath( -1.0 ) 
	{
		mLine.push_back( initialPt ); 
	}
	
	void addPoint( const Vec2f &pt ) { mLine.push_back( pt ); }
	
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
	
	PolyLine<Vec2f>	mLine;
	Color			mColor;
	float			mTimeOfDeath;
};

struct TestCallbackOrder {
	TestCallbackOrder() : mState( TestCallbackOrder::VIRGIN ), mDone( false ) {}
	
	void	setState( int state ) {
		if( mDone ) return;
		
		if( mState != state - 1 )
			app::console() << "Fail at state: " << mState << "->" << state << std::endl;
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

class iosAppTestApp : public AppCocoaTouch {
  public:
	void 			prepareSettings( Settings *settings );
	virtual void	setup();
	virtual void	resize();
	virtual void	update();
	void 			draw();
	
	virtual void	mouseDown( MouseEvent event );
	virtual void	mouseUp( MouseEvent event );
	virtual void	mouseDrag( MouseEvent event );

	virtual void	touchesBegan( TouchEvent event );
	virtual void	touchesMoved( TouchEvent event );
	virtual void	touchesEnded( TouchEvent event );	

	void didEnterBackground();
	void willEnterForeground();
	void willResignActive();
	void didBecomeActive();
	void shuttingDown();
	
	void keyDown( KeyEvent event );
	void proximitySensor( bool isClose );
	void batteryStateChange( bool isUnplugged );
	void memoryWarning();

	uint32_t	supportAllOrientations();
	uint32_t	supportPortraitOrientations();
	void		willRotate();
	void		didRotate();

	TestCallbackOrder	tester;
					
	Matrix44f		mCubeRotation;
	gl::Texture 	mTex;
	gl::Texture		mTextTex;
	gl::TextureFontRef	mFont;
	CameraPersp		mCam;
	WindowRef		mSecondWindow;
	string			mSecondWindowMessage;
	
	map<uint32_t,TouchPoint>	mActivePoints;
	list<TouchPoint>			mDyingPoints;
	int							mMouseTouchId; // gives a unique ID to each click to emulate multitouch
};

void iosAppTestApp::prepareSettings( Settings *settings )
{
	tester.setState( TestCallbackOrder::PREPARESETTINGS );

	for( auto &display : Display::getDisplays() )
		console() << *display << std::endl;

	settings->enableMultiTouch( true );
	settings->enableHighDensityDisplay( true );
	settings->enablePowerManagement( false );
	settings->enableStatusBar( true );
	
	settings->prepareWindow( Window::Format() );
	if( Display::getDisplays().size() > 1 )
			settings->prepareWindow( Window::Format().display( Display::getDisplays()[1] ).size( 800, 600 ) );
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
	tester.setState( TestCallbackOrder::SETUP );

	getSignalProximitySensor().connect( std::bind( &iosAppTestApp::proximitySensor, this, std::placeholders::_1 ) );
	enableProximitySensor();

	getSignalBatteryState().connect( std::bind( &iosAppTestApp::batteryStateChange, this, std::placeholders::_1 ) );
	enableBatteryMonitoring();
	
	mCubeRotation.setToIdentity();

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

	mTex = gl::Texture( surface );
	
	std::cout << "Size: " << getWindowSize() << std::endl;

	getWindow()->getSignalDraw().connect( std::bind( &iosAppTestApp::draw, this ) );

	if( getNumWindows() > 1 ) {
		mSecondWindow = getWindowIndex( 1 );
		mSecondWindow->getSignalDraw().connect( std::bind( &iosAppTestApp::draw, this ) );
	}

	getSignalDidEnterBackground().connect( bind( &iosAppTestApp::didEnterBackground, this ) );
	getSignalWillEnterForeground().connect( bind( &iosAppTestApp::willEnterForeground, this ) );
	getSignalWillResignActive().connect( bind( &iosAppTestApp::willResignActive, this ) );
	getSignalDidBecomeActive().connect( bind( &iosAppTestApp::didBecomeActive, this ) );
	getSignalShutdown().connect( bind( &iosAppTestApp::shuttingDown, this ) );
	getSignalMemoryWarning().connect( bind( &iosAppTestApp::memoryWarning, this ) );

	getSignalSupportedOrientations().connect( std::bind( &iosAppTestApp::supportAllOrientations, this ) );
//	getSignalSupportedOrientations().connect( std::bind( &iosAppTestApp::supportPortraitOrientations, this ) ); // this one limits to only portrait, even though slot above says all
	getSignalWillRotate().connect( std::bind( &iosAppTestApp::willRotate, this ) );
	getSignalDidRotate().connect( std::bind( &iosAppTestApp::didRotate, this ) );

	console() << "Device is " << ( System::isDeviceIpad() ? "iPad" : "iPhone" ) << std::endl;

	console() << " ### num Windows: " << getNumWindows() << endl;

}

void iosAppTestApp::didEnterBackground()
{
	console() << "Did enter background!" << std::endl;
}

void iosAppTestApp::willEnterForeground()
{
	console() << "Will enter foreground!" << std::endl;
}

void iosAppTestApp::willResignActive()
{
	console() << "Will resign active!" << std::endl;
}

void iosAppTestApp::didBecomeActive()
{
	console() << "Did become active!" << std::endl;
}

void iosAppTestApp::shuttingDown()
{
	console() << "Shutting down!" << std::endl;
}

void iosAppTestApp::memoryWarning()
{
	console() << "Memory warning!" << std::endl;
}

uint32_t iosAppTestApp::supportAllOrientations()
{
	console() << "Supporting all orientations." << std::endl;
	return InterfaceOrientation::All;
}

uint32_t iosAppTestApp::supportPortraitOrientations()
{
	console() << "Supporting only portrait orientations." << std::endl;
	return InterfaceOrientation::PortraitAll;
}

void iosAppTestApp::willRotate()
{
	InterfaceOrientation orientation = getOrientation();
	console() << __PRETTY_FUNCTION__ << "orientation:" << orientation << std::endl;
}

void iosAppTestApp::didRotate()
{
	InterfaceOrientation orientation = getOrientation();
	console() << __PRETTY_FUNCTION__ << "orientation:" << orientation << std::endl;
}

void iosAppTestApp::resize()
{
	tester.setState( TestCallbackOrder::RESIZE );
	console() << "Resize!" << std::endl;
}

void iosAppTestApp::mouseDown( MouseEvent event )
{
	Color newColor( CM_HSV, Rand::randFloat(), 1, 1 );
std::cout << "Starting with: " << event.getPos() << std::endl;
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
	console() << "Width: " << getWindowWidth() << std::endl;
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
	
	/*if( isKeyboardVisible() )
		hideKeyboard();
	else {
		showKeyboard();
		mSecondWindowMessage.clear();
	}*/
	
	if( isStatusBarVisible() )
		hideStatusBar( StatusBarAnimation::FADE );
	else {
		showStatusBar( StatusBarAnimation::SLIDE );
	}
	console() << getWindowSize() << std::endl;
	
//	setFrameRate( 12.0f );
}

void iosAppTestApp::keyDown( KeyEvent event )
{
	std::cout << "Key: " << event.getCharUtf32() << std::endl;
	if( event.getCode() == KeyEvent::KEY_BACKSPACE )
		console() << "backspace!" << std::endl;
	else if( event.getCode() == KeyEvent::KEY_RETURN )
		console() << "return!" << std::endl;

	mSecondWindowMessage = getKeyboardString();
}

void iosAppTestApp::proximitySensor( bool isClose )
{
	std::cout << "Device is" << ( isClose ? " " : " NOT " ) << "close to your head!" << std::endl;
}

void iosAppTestApp::batteryStateChange( bool isUnplugged )
{
	std::cout << "Power is" << ( isUnplugged ? " " : " NOT " ) << "unplugged!" << std::endl;
}

void iosAppTestApp::update()
{
	tester.setState( TestCallbackOrder::UPDATE );

	mCubeRotation.rotate( Vec3f( 1, 1, 1 ), 0.03f );
}

void iosAppTestApp::draw()
{
	gl::enableAlphaBlending();
	gl::enableDepthRead();
	tester.setState( TestCallbackOrder::DRAW );
	CameraPersp mCam;
	mCam.lookAt( Vec3f( 3, 2, -3 ), Vec3f::zero() );
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );

	if( isUnplugged() )
		gl::clear( Color( 0.2f, 0.2f, 0.3f ) );
	else
		gl::clear( Color( 0.4f, 0.2f, 0.2f ) );		

	gl::enable( GL_TEXTURE_2D );
	gl::color( Color::white() );
	gl::enableDepthRead();
	mTex.bind();
	gl::setMatrices( mCam );
	glPushMatrix();
		gl::multModelView( mCubeRotation );
		gl::drawCube( Vec3f::zero(), Vec3f( 2.0f, 2.0f, 2.0f ) );
	glPopMatrix();
	gl::disable( GL_TEXTURE_2D );
	
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
		static gl::Texture messageTex;
		if( lastMessage != mSecondWindowMessage ) {
			TextBox box = TextBox().font( font ).text( mSecondWindowMessage );
			messageTex = box.render();
			lastMessage = mSecondWindowMessage;
		}
		if( messageTex ) {
			gl::color( Color::white() );
			gl::draw( messageTex, Vec2i( ( getWindowWidth() - messageTex.getCleanWidth() ) / 2, getWindowCenter().y ) );
		}
	}
//
	gl::disableDepthRead();
	gl::color( Color( 0.0f, 1.0f, 0.0f ) );
	mFont->drawString( orientationString( getWindowOrientation() ) + "@ " + toString( getWindowContentScale() ), Vec2f( 10.0f, 60.0f ) );
//	gl::drawStringCentered( "Orientation: " + orientationString( getInterfaceOrientation() ), Vec2f( getWindowCenter().x, 30.0f ), Color( 0.0f, 1.0f, 0.0f ), Font::getDefault() ); // ???: why not centered?

	mFont->drawString( toString( floor(getAverageFps()) ) + " fps", Vec2f( 10.0f, 90.0f ) );
}

CINDER_APP_COCOA_TOUCH( iosAppTestApp, RendererGl(RendererGl::AA_NONE) )
