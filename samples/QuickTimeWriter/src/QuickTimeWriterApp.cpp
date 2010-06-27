#include "cinder/app/AppBasic.h"
#include "cinder/qtime/MovieWriter.h"
#include "cinder/ip/Fill.h"

#include "Resources.h"

#include <list>
using namespace ci;
using namespace ci::app;
using namespace std;

// We'll create a new Cinder Application by deriving from the AppBasic class
class QuickTimeWriterApp : public AppBasic {
 public:
	void setup();
	void mouseDrag( MouseEvent event );
	void draw();
	
	// This will maintain a list of points which we will draw line segments between
	list<Vec2f>			mPoints;
	qtime::MovieWriter	mMovieWriter;
};

void QuickTimeWriterApp::setup()
{
	string path = getSaveFilePath();
	if( path.empty() )
		return; // user cancelled save

	// The preview image below is entitled "Lava" by "Z T Jackson"
	// http://www.flickr.com/photos/ztjackson/3241111818/

	qtime::MovieWriter::Format format;
	if( qtime::MovieWriter::getUserCompressionSettings( &format, loadImage( loadResource( RES_PREVIEW_IMAGE ) ) ) ) {
		mMovieWriter = qtime::MovieWriter( path, getWindowWidth(), getWindowHeight(), format );
	}
}

void QuickTimeWriterApp::mouseDrag( MouseEvent event )
{
	// add wherever the user drags to the end of our list of points
	mPoints.push_back( event.getPos() );
}

void QuickTimeWriterApp::draw()
{
	gl::clear( Color( 0.05f, 0.1f, 0.2f ) );

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
	
	// add this frame to our movie
	if( mMovieWriter )
		mMovieWriter.addFrame( copyWindowSurface() );
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( QuickTimeWriterApp, RendererGl )