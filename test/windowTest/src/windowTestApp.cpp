#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"
#include "cinder/Log.h"

#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

class WindowData {
  public:
	~WindowData() { CI_LOG_V( "Destroying Window Data" ); };
	vector<vec2> 	mPoints;
};

class WindowTestApp : public App {
 public:
	// this is passed into the app intantiation macro at the bottom of the file
	static void prepareSettings( Settings *settings );

	void setup() override;
	void keyDown( KeyEvent event ) override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void fileDrop( FileDropEvent event ) override;
	void resize() override;
	void update() override;

	// Cinder calls this function 30 times per second by default
	void windowDraw();
	void anotherTest( MouseEvent event );
	void mouseDown1( MouseEvent &event );
	bool mouseDown2( MouseEvent event );
	void mouseDown3( MouseEvent &event );
	void windowMove();
	void windowClose();
	void windowMouseDown( MouseEvent &mouseEvt );
	void displayChange();

	bool shouldQuit();

	// This will maintain a list of points which we will draw line segments between
	WindowRef		mSecondWindow;
};

void WindowTestApp::mouseDrag( MouseEvent event )
{
	vec2 v = event.getPos();
	WindowRef w = event.getWindow();
	vector<vec2> &points = w->getUserData<WindowData>()->mPoints;
	points.push_back( v );
}

void WindowTestApp::prepareSettings( Settings *settings )
{
	settings->setPowerManagementEnabled( false );
	settings->setQuitOnLastWindowCloseEnabled( false );
//	settings->setFullScreen( true );
	settings->setWindowSize( 800, 500 );
	settings->setTitle( "title set from App::Settings" );
//	settings->prepareWindow( Window::Format().resizable( false ).renderer( RendererGl::create() ).fullScreen( true ) );
//	settings->prepareWindow( Window::Format().fullScreen().fullScreenButton() );
}

void WindowTestApp::setup()
{
	for( auto display : Display::getDisplays() )
		CI_LOG_V( "display name: '" << display->getName() << "', bounds: " << display->getBounds() );

	getWindow()->setUserData( new WindowData );

	getWindow()->getSignalMouseDown().connect( signals::slot( this, &WindowTestApp::anotherTest ) );
	getWindow()->getSignalMouseDown().connect( std::bind( &WindowTestApp::mouseDown1, this, std::placeholders::_1 ) );
//	getWindow()->getSignalMouseDown().connect( std::bind( &WindowTestApp::mouseDown2, this, std::placeholders::_1 ) ); // This shouldn't build because it has the wrong signature (<bool( MouseEvent )>) - to note, the reference is not the problem, just the return value is different.
	getWindow()->getSignalMouseDown().connect( std::bind( &WindowTestApp::mouseDown3, this, std::placeholders::_1 ) );
	getSignalShouldQuit().connect( std::bind( &WindowTestApp::shouldQuit, this ) );
	
	getWindow()->getSignalMove().connect( std::bind( &WindowTestApp::windowMove, this ) );
	getWindow()->getSignalDraw().connect( std::bind( &WindowTestApp::windowDraw, this ) );
	getWindow()->getSignalDisplayChange().connect( std::bind( &WindowTestApp::displayChange, this ) );
	getWindow()->getSignalClose().connect( std::bind( &WindowTestApp::windowClose, this ) );
	
	getSignalDidBecomeActive().connect( [] { CI_LOG_V( "App became active." ); } );
	getSignalWillResignActive().connect( [] { CI_LOG_V( "App will resign active." ); } );
}

bool WindowTestApp::shouldQuit()
{
	return true;
}

void WindowTestApp::update()
{
}

void WindowTestApp::anotherTest( MouseEvent event )
{
	CI_LOG_V( "Totes." );
}

void WindowTestApp::mouseDown( MouseEvent event )
{
	CI_LOG_V( "Main mouseDown" );
}

void WindowTestApp::mouseDown1( MouseEvent &event )
{
	enablePowerManagement( ! isPowerManagementEnabled() );
	CI_LOG_V( "Power mgmt: " << isPowerManagementEnabled() );
	CI_LOG_V( "window pos: " << getWindow()->getPos() );
	CI_LOG_V( "Title was: " << getWindow()->getTitle() << "." );
	getWindow()->setTitle( "Crunk " + toString( getWindow()->getPos().x ) );
	CI_LOG_V( "Window size from app: " << getWindowWidth() << " x " << getWindowHeight() << " from window: " << event.getWindow()->getSize() );
	CI_LOG_V( "Window pos from app: " << getWindowPos() << " from window: " << getWindow()->getPos() );
//	event.setHandled();
}

void WindowTestApp::windowMove()
{
	CI_LOG_V( "window pos: " << getWindow()->getPos() );
}

void WindowTestApp::displayChange()
{
	CI_LOG_V( "window display changed: " << getWindow()->getDisplay()->getBounds() );
}

bool WindowTestApp::mouseDown2( MouseEvent event )
{
	CI_ASSERT_MSG( 0, "getSignalMouseDown() called with wrong signature (should be <void( MouseEvent &event)>" );
	return false;
}

void WindowTestApp::mouseDown3( MouseEvent &event )
{
	CI_LOG_V( "It worked 3!" << getMousePos() );
	event.setHandled();
}

void WindowTestApp::windowClose()
{
	WindowRef win = getWindow();
	CI_LOG_V( "Closing " << getWindow() );
}

void WindowTestApp::windowMouseDown( MouseEvent &mouseEvt )
{
	CI_LOG_V( "Mouse down in window" );
}

void WindowTestApp::keyDown( KeyEvent event )
{
	if( event.getCode() == KeyEvent::KEY_RSHIFT ) {
		CI_LOG_V( "Right shift down" );
	}
	else if( event.getCode() == KeyEvent::KEY_LCTRL ) {
		CI_LOG_V( "Left control down" );
	}
	if( event.getChar() == 'f' ) {
		CI_LOG_V( "Toggling from fullscreen: " << getWindow()->isFullScreen() );
		// This line forces fullscreen on the secondary display
		//getWindow()->setFullScreen( ! getWindow()->isFullScreen(), FullScreenOptions().display( Display::getDisplays()[1] ) );
		getWindow()->setFullScreen( ! getWindow()->isFullScreen() );
	}
	else if( event.getChar() == 'o' ) {
		CI_LOG_V( "(kiosk) Toggling from fullscreen: " << getWindow()->isFullScreen() );
		FullScreenOptions fullScreenOptions = FullScreenOptions().kioskMode();
		if( event.isControlDown() )
			fullScreenOptions.secondaryDisplayBlanking( false );
		else
			fullScreenOptions.secondaryDisplayBlanking( true );
		//if( event.isControlDown() )
		//	fullScreenOptions.exclusive();
		getWindow()->setFullScreen( ! getWindow()->isFullScreen(), fullScreenOptions );
	}
	else if( event.getCode() == KeyEvent::KEY_LEFT )
		getWindow()->setPos( getWindow()->getPos().x - 1, getWindow()->getPos().y );
	else if( event.getCode() == KeyEvent::KEY_RIGHT )
		getWindow()->setPos( getWindow()->getPos().x + 1, getWindow()->getPos().y );
	else if( event.getCode() == KeyEvent::KEY_UP )
		getWindow()->setSize( getWindow()->getSize().x + 1, getWindow()->getSize().y );
	else if( event.getCode() == KeyEvent::KEY_DOWN )
		getWindow()->setSize( getWindow()->getSize().x + 1, getWindow()->getSize().y + 1 );
	else if( event.getChar() == 'w' ) {
		Window::Format format( RendererGl::create() );
		//format.setFullScreen( true );
		mSecondWindow = createWindow( format );
		mSecondWindow->getSignalClose().connect( std::bind( &WindowTestApp::windowClose, this ) );
		mSecondWindow->getSignalMouseDown().connect( std::bind( &WindowTestApp::windowMouseDown, this, std::placeholders::_1 ) );
		mSecondWindow->getSignalDraw().connect( std::bind( &WindowTestApp::windowDraw, this ) );
		mSecondWindow->setUserData( new WindowData );
	}
	else if( event.getChar() == 'c' ) {
		getWindow()->close();
//		getWindow( getNumWindows() - 1 )->close();
	}
	else if( event.getChar() == 'h' ) {
		if( getWindowIndex(0)->isHidden() )
			getWindowIndex(0)->show();
		else
			getWindowIndex(0)->hide();
	}
	else if( event.getChar() == 'b' ) {
		getWindow()->setBorderless( ! getWindow()->isBorderless() );
	}
	else if( event.getChar() == 't' ) {
		getWindow()->setAlwaysOnTop( ! getWindow()->isAlwaysOnTop() );
	}
	else if( event.getChar() == 's' ) {
		getWindow()->setBorderless();
		getWindow()->spanAllDisplays();
		CI_LOG_V( "Spanning Area: " << Display::getSpanningArea() );
		CI_LOG_V( "Bounds: " << getWindow()->getBounds() );
		//getWindow()->setPos( ivec2( -1680 + 1, 0 + 1 ) );
//		getWindow()->setSize( 1440, 900 );
//		getWindow()->setPos( 0, 0 );
	}
}

void WindowTestApp::fileDrop( FileDropEvent event )
{
	CI_LOG_V( "File drop: " << event );
}

void WindowTestApp::resize()
{
	CI_LOG_V( "window pos: " << getWindowPos() << ", size: " << getWindowSize() );
}

void WindowTestApp::windowDraw()
{
	gl::enableAlphaBlending();
//	glEnable(GL_MULTISAMPLE_ARB);
	if( getWindow() == getForegroundWindow() )
		gl::clear( Color( 0.1f, 0.1f, 0.5f ) );
	else
		gl::clear( Color( 0.3f, 0.1f, 0.1f ) );

	// We'll set the color to an orange color
	gl::color( 1.0f, 0.5f, 0.25f );

	glEnable( GL_LINE_SMOOTH );
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	
	gl::begin( GL_LINE_STRIP );
	const vector<vec2> &points = getWindow()->getUserData<WindowData>()->mPoints;
	for( auto pointIter = points.begin(); pointIter != points.end(); ++pointIter ) {
		gl::vertex( *pointIter /*+ vec2( 0, getElapsedSeconds() )*/ );
	}
	gl::end();
	
	//if( window == mSecondWindow )
		gl::drawLine( vec2( 50, 50 ), vec2( 250, 250 ) );
	gl::pushMatrices();
		gl::color( 1.0f, 0.2f, 0.15f );
		gl::translate( getWindowCenter() );
		gl::rotate( getElapsedSeconds() );
		gl::drawSolidRect( Rectf( -100, -100, 100, 100 ) );
	gl::popMatrices();
}

CINDER_APP( WindowTestApp, RendererGl, WindowTestApp::prepareSettings )
