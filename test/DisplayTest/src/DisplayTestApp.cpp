#include "cinder/app/AppBasic.h"
using namespace ci;
using namespace ci::app;

#include <vector>
#include <list>
using namespace std;

class DisplayTestApp : public AppBasic {
 public:
	virtual void prepareSettings( Settings *settings );

	void keyDown( KeyEvent event );
	void mouseDrag( MouseEvent event );
	void draw();
	
	// This will maintain a list of points which we will draw line segments between
	list<Vec2f>		mPoints;
};

void DisplayTestApp::prepareSettings( Settings *settings )
{
	console() << "Startup: " << getDisplay().getWidth() << endl;

	const vector<shared_ptr<Display> > &displays( Display::getDisplays() );

	for( vector<shared_ptr<Display> >::const_iterator it = displays.begin(); it != displays.end(); ++it ) {
		Area a = (*it)->getArea();
		console() << "Display: " << a << " size: " << a.getWidth() << " x " << a.getHeight() << " @ depth: " << (*it)->getBitsPerPixel() << endl;
	}

	if( displays.size() > 1 )
		settings->setDisplay( displays[1] );
}

void DisplayTestApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ) {
		setFullScreen( ! isFullScreen() );
	}
	
}

void DisplayTestApp::mouseDrag( MouseEvent event )
{
	// add wherever the user drags to the end of our list of points
	mPoints.push_back( event.getPos() );
}

void DisplayTestApp::draw()
{
	// this pair of lines is the standard way to clear the screen in OpenGL
	glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	// We'll set the color to an orange color
	glColor3f( 1.0f, 0.5f, 0.25f );
	
	// now tell OpenGL we've got a series of points it should draw lines between
	glBegin( GL_LINE_STRIP );
	// iterate across our list of points, and pass each one to OpenGL
	for( list<Vec2f>::iterator pointIter = mPoints.begin(); pointIter != mPoints.end(); ++pointIter ) {
		glVertex2f( pointIter->x, pointIter->y );
	}
	// tell OpenGL to actually draw the lines now
	glEnd();
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( DisplayTestApp, RendererGl )
