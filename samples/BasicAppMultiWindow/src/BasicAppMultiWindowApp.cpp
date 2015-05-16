#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

// We'll create a new Cinder Application by deriving from the App class
class BasicAppMultiWindow : public App {
  public:
	void setup();
	void createNewWindow();
	
	void mouseDrag( MouseEvent event );
	void keyDown( KeyEvent event );
	void draw();
};

// The window-specific data for each window
class WindowData {
  public:
	WindowData()
		: mColor( Color( CM_HSV, randFloat(), 0.8f, 0.8f ) ) // a random color
	{}
  
	Color			mColor;
	list<vec2>		mPoints; // the points drawn into this window
};

void BasicAppMultiWindow::setup()
{
	// for the default window we need to provide an instance of WindowData
	getWindow()->setUserData( new WindowData );
	
	createNewWindow();
}

void BasicAppMultiWindow::createNewWindow()
{
	app::WindowRef newWindow = createWindow( Window::Format().size( 400, 400 ) );
	newWindow->setUserData( new WindowData );
	
	// for demonstration purposes, we'll connect a lambda unique to this window which fires on close
	int uniqueId = getNumWindows();
	newWindow->getSignalClose().connect(
			[uniqueId,this] { this->console() << "You closed window #" << uniqueId << std::endl; }
		);
}

void BasicAppMultiWindow::mouseDrag( MouseEvent event )
{
	WindowData *data = getWindow()->getUserData<WindowData>();
	
	// add this point to the list
	data->mPoints.push_back( event.getPos() );
}

void BasicAppMultiWindow::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
	else if( event.getChar() == 'w' )
		createNewWindow();
}

void BasicAppMultiWindow::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );

	WindowData *data = getWindow()->getUserData<WindowData>();

	gl::color( data->mColor );	
	gl::begin( GL_LINE_STRIP );
	for( auto pointIter = data->mPoints.begin(); pointIter != data->mPoints.end(); ++pointIter ) {
		gl::vertex( *pointIter );
	}
	gl::end();
}

// This line tells Cinder to actually create the application
CINDER_APP( BasicAppMultiWindow, RendererGl )