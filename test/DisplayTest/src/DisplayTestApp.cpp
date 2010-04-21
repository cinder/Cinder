#include "flint/app/AppBasic.h"
using namespace fli;
using namespace fli::app;

#include <vector>
using std::vector;
#include <list>
using std::list;
#include <sstream>

class DisplayTestApp : public AppBasic {
 public:
	virtual void prepareSettings( Settings *settings );

	void mouseDrag( MouseEvent event );
	void draw();
	
	// This will maintain a list of points which we will draw line segments between
	list<Vec2f>		mPoints;
};

void DisplayTestApp::prepareSettings( Settings *settings )
{
	std::cout << "Startup: " << getDisplay().getWidth() << std::endl;

	const vector<shared_ptr<Display> > &displays( Display::getDisplays() );
#if defined( FLI_MAC )
	std::ostream &os( std::cout );
#else
	std::stringstream os;
#endif
	for( vector<shared_ptr<Display> >::const_iterator it = displays.begin(); it != displays.end(); ++it ) {
		Area a = (*it)->getArea();
		os << "Display: " << a << " size: " << a.getWidth() << " x " << a.getHeight() << " @ depth: " << (*it)->getBitsPerPixel() << std::endl;
	}
#if defined( FLI_MSW )
	OutputDebugStringA( os.str().c_str() );
#endif

	if( displays.size() > 1 )
		settings->setDisplay( displays[1] );
}

void DisplayTestApp::mouseDrag( MouseEvent event )
{
	// add wherever the user drags to the end of our list of points
	mPoints.push_back( event.getVec2f() );
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

FLI_APP_BASIC( DisplayTestApp )
