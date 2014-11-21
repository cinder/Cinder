#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;

#include <list>
using namespace std;

class WindowData {
  public:
	~WindowData() { CI_LOG_V( "Destroying Window Data" ); };
	vector<vec2> 	mPoints;
};

// We'll create a new Cinder Application by deriving from the BasicApp class
class BasicApp : public AppBasic {
 public:
	// Cinder will always call this function whenever the user drags the mouse
	void prepareSettings( Settings *settings ) override;
	void mouseDrag( MouseEvent event );
	void keyDown( KeyEvent event );
	// Cinder calls this function 30 times per second by default
	void windowDraw();
	void fileDrop( FileDropEvent event );
	void resize();

	void setup();
	void update();
	void anotherTest( MouseEvent event );
	void mouseDown( MouseEvent event );
	void mouseDown1( MouseEvent &event );
	bool mouseDown2( MouseEvent event );
	void mouseDown3( MouseEvent &event );
	void windowMove();
	void windowClose();
	void windowMouseDown( MouseEvent &mouseEvt );
	void displayChange();

	bool	shouldQuit();

	// This will maintain a list of points which we will draw line segments between
	WindowRef		mSecondWindow;
};

void BasicApp::mouseDrag( MouseEvent event )
{
	vec2 v = event.getPos();
	WindowRef w = event.getWindow();
	vector<vec2> &points = w->getUserData<WindowData>()->mPoints;
	points.push_back( v );
}

void BasicApp::prepareSettings( Settings *settings )
{
	settings->enablePowerManagement( false );
	settings->enableQuitOnLastWindowClose( false );
//	settings->setFullScreen( false );
	settings->setWindowSize( 800, 500 );
//	settings->prepareWindow( Window::Format().resizable( false ).renderer( RendererGl::create() ).fullScreen( true ) );
//	settings->prepareWindow( Window::Format().fullScreen().fullScreenButton() );
}

void BasicApp::setup()
{
	for( auto displayIt = Display::getDisplays().begin(); displayIt != Display::getDisplays().end(); ++displayIt )
		CI_LOG_V( "Resolution: " << (*displayIt)->getBounds() );

	getWindow()->setUserData( new WindowData );

	getWindow()->connectMouseDown( &BasicApp::anotherTest, this );
	getWindow()->getSignalMouseDown().connect( std::bind( &BasicApp::mouseDown1, this, std::placeholders::_1 ) );
	getWindow()->getSignalMouseDown().connect( std::bind( &BasicApp::mouseDown2, this, std::placeholders::_1 ) );
	getWindow()->getSignalMouseDown().connect( std::bind( &BasicApp::mouseDown3, this, std::placeholders::_1 ) );
	getSignalShouldQuit().connect( std::bind( &BasicApp::shouldQuit, this ) );
	
	getWindow()->getSignalMove().connect( std::bind( &BasicApp::windowMove, this ) );
	getWindow()->getSignalDraw().connect( std::bind( &BasicApp::windowDraw, this ) );
	getWindow()->getSignalDisplayChange().connect( std::bind( &BasicApp::displayChange, this ) );
	getWindow()->getSignalClose().connect( std::bind( &BasicApp::windowClose, this ) );
	
	getSignalDidBecomeActive().connect( [] { CI_LOG_V( "App became active." ); } );
	getSignalWillResignActive().connect( [] { CI_LOG_V( "App will resign active." ); } );
}

bool BasicApp::shouldQuit()
{
	return true;
}

void BasicApp::update()
{
}

void BasicApp::anotherTest( MouseEvent event )
{
	CI_LOG_V( "Totes." );
}

void BasicApp::mouseDown( MouseEvent event )
{
	CI_LOG_V( "Main mouseDown" );
}

void BasicApp::mouseDown1( MouseEvent &event )
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

void BasicApp::windowMove()
{
	CI_LOG_V( "window pos: " << getWindow()->getPos() );
}

void BasicApp::displayChange()
{
	CI_LOG_V( "window display changed: " << getWindow()->getDisplay()->getBounds() );
}

bool BasicApp::mouseDown2( MouseEvent event )
{
	CI_LOG_V( "It worked 2!" );
	return false;
}

void BasicApp::mouseDown3( MouseEvent &event )
{
	CI_LOG_V( "It worked 3!" << getMousePos() );
	event.setHandled();
}

void BasicApp::windowClose()
{
	WindowRef win = getWindow();
	CI_LOG_V( "Closing " << getWindow() );
}

void BasicApp::windowMouseDown( MouseEvent &mouseEvt )
{
	CI_LOG_V( "Mouse down in window" );
}

void BasicApp::keyDown( KeyEvent event )
{
	if( event.getCode() == KeyEvent::KEY_RSHIFT ) {
		CI_LOG_V( "Right shift down" );
	}
	else if( event.getCode() == KeyEvent::KEY_LCTRL ) {
		CI_LOG_V( "Left control down" );
	}
	if( event.getChar() == 'f' ) {
		CI_LOG_V( "Toggling from fullscreen: " << getWindow()->isFullScreen() );
		getWindow()->setFullScreen( ! getWindow()->isFullScreen(), FullScreenOptions().display( Display::getDisplays()[1] ) );
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
		format.setFullScreen( true );
		mSecondWindow = createWindow( format );
		mSecondWindow->getSignalClose().connect( std::bind( &BasicApp::windowClose, this ) );
		mSecondWindow->getSignalMouseDown().connect( std::bind( &BasicApp::windowMouseDown, this, std::placeholders::_1 ) );
		mSecondWindow->getSignalDraw().connect( std::bind( &BasicApp::windowDraw, this ) );
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

void BasicApp::fileDrop( FileDropEvent event )
{
	CI_LOG_V( "File drop: " << event );
}

void BasicApp::resize()
{
	CI_LOG_V( "Resized: " << getWindowSize() );
}

void BasicApp::windowDraw()
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

// This line tells Flint to actually create the application
CINDER_APP_BASIC( BasicApp, RendererGl )
