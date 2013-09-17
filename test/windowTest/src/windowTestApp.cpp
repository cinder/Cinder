#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;

#include <list>
using namespace std;

class WindowData {
  public:
  	~WindowData() { std::cout << "Destroying Window Data" << std::endl; };
	vector<Vec2f> 	mPoints;
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
	Vec2f v = event.getPos();
	WindowRef w = event.getWindow();
	vector<Vec2f> &points = w->getUserData<WindowData>()->mPoints;
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
		console() << "Resolution: " << (*displayIt)->getBounds() << std::endl;

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
	console() << "Totes." << std::endl;
}

void BasicApp::mouseDown( MouseEvent event )
{
	console() << "Main mouseDown" << std::endl;
	vector<int> v;
	for( auto it = v.begin(); it != v.end(); ++it )
		console() << *it;
}

void BasicApp::mouseDown1( MouseEvent &event )
{
	enablePowerManagement( ! isPowerManagementEnabled() );
console() << "Power mgmt: " << isPowerManagementEnabled() << std::endl;
	console() << "window pos: " << getWindow()->getPos() << std::endl;
	console() << "Title was: " << getWindow()->getTitle() << "." << std::endl;
	getWindow()->setTitle( "Crunk " + toString( getWindow()->getPos().x ) );
	console() << "Window size from app: " << getWindowWidth() << " x " << getWindowHeight() << " from window: " << event.getWindow()->getSize() << std::endl;
	console() << "Window pos from app: " << getWindowPos() << " from window: " << getWindow()->getPos() << std::endl;
//	event.setHandled();
}

void BasicApp::windowMove()
{
	console() << "window pos: " << getWindow()->getPos() << std::endl;
}

void BasicApp::displayChange()
{
	console() << "window display changed: " << getWindow()->getDisplay()->getBounds() << std::endl;
}

bool BasicApp::mouseDown2( MouseEvent event )
{
	console() << "It worked 2!" << std::endl;
	return false;
}

void BasicApp::mouseDown3( MouseEvent &event )
{
	console() << "It worked 3!" << getMousePos() << std::endl;
	event.setHandled();
}

void BasicApp::windowClose()
{
	WindowRef win = getWindow();
	console() << "Closing " << getWindow() << std::endl;
}

void BasicApp::windowMouseDown( MouseEvent &mouseEvt )
{
	console() << "Mouse down in window" << std::endl;
}

void BasicApp::keyDown( KeyEvent event )
{
	if( event.getCode() == KeyEvent::KEY_RSHIFT ) {
		console() << "Right shift down" << std::endl;
	}
	else if( event.getCode() == KeyEvent::KEY_LCTRL ) {
		console() << "Left control down" << std::endl;
	}
	if( event.getChar() == 'f' ) {
		console() << "Toggling from fullscreen: " << getWindow()->isFullScreen() << std::endl;
		getWindow()->setFullScreen( ! getWindow()->isFullScreen(), FullScreenOptions().display( Display::getDisplays()[1] ) );
	}
	else if( event.getChar() == 'o' ) {
		console() << "(kiosk) Toggling from fullscreen: " << getWindow()->isFullScreen() << std::endl;
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
		console() << "Spanning Area: " << Display::getSpanningArea() << std::endl;
		console() << "Bounds: " << getWindow()->getBounds() << std::endl;
		//getWindow()->setPos( Vec2i( -1680 + 1, 0 + 1 ) );
//		getWindow()->setSize( 1440, 900 );
//		getWindow()->setPos( 0, 0 );
	}
}

void BasicApp::fileDrop( FileDropEvent event )
{
	console() << "File drop: " << event << std::endl;
}

void BasicApp::resize()
{
	console() << "Resized: " << getWindowSize() << std::endl;
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
	glColor3f( 1.0f, 0.5f, 0.25f );

	glEnable( GL_LINE_SMOOTH );
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	
	glBegin( GL_LINE_STRIP );
	const vector<Vec2f> &points = getWindow()->getUserData<WindowData>()->mPoints;
	for( auto pointIter = points.begin(); pointIter != points.end(); ++pointIter ) {
		gl::vertex( *pointIter /*+ Vec2f( 0, getElapsedSeconds() )*/ );
	}
	glEnd();
	
	//if( window == mSecondWindow )
		gl::drawLine( Vec2f( 50, 50 ), Vec2f( 250, 250 ) );
	gl::pushMatrices();
		glColor3f( 1.0f, 0.2f, 0.15f );
		gl::translate( getWindowCenter() );
		gl::rotate( getElapsedSeconds() * 5 );
		gl::drawSolidRect( Rectf( -100, -100, 100, 100 ) );
	gl::popMatrices();
}

// This line tells Flint to actually create the application
CINDER_APP_BASIC( BasicApp, RendererGl(RendererGl::AA_MSAA_16) )