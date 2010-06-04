#include "cinder/app/AppBasic.h"
#include "cinder/qtime/MovieWriter.h"
#include "cinder/ip/Fill.h"

#include <list>
using namespace ci;
using namespace ci::app;
using namespace std;

// We'll create a new Cinder Application by deriving from the AppBasic class
class QuickTimeWriterApp : public AppBasic {
 public:
	void setup();
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void draw();
	
	// This will maintain a list of points which we will draw line segments between
	list<Vec2f>			mPoints;
	qtime::MovieWriter	mMovieWriter;
};

void QuickTimeWriterApp::setup()
{
	string path = getSaveFilePath();
	qtime::MovieWriter::Format format;
	Surface surface( 640, 480, false );
	ip::fill( &surface, Color8u( 255, 128, 64 ) );
	if( ! path.empty() && qtime::MovieWriter::getUserCompressionSettings( &format, loadImage( "C:\\Users\\Andrew\\Pictures\\2046-724573.jpg" ) ) )
		mMovieWriter = qtime::MovieWriter( path, getWindowWidth(), getWindowHeight(), format );
}

void QuickTimeWriterApp::mouseDown( MouseEvent event )
{
	if( event.isRight() ) {
		// add one second of white on right clicks
		Surface white( mMovieWriter.getWidth(), mMovieWriter.getHeight(), false );
		ip::fill( &white, Color8u( 255, 255, 255 ) );
		for( int i = 0; i < 30; ++i ) {
			ip::fill( &white, Color8u( 0, 0, 255 ), Area( 0, 400, mMovieWriter.getWidth() * i / 30, 480 ) );
			mMovieWriter.addFrame( white );
		}
	}
}

void QuickTimeWriterApp::mouseDrag( MouseEvent event )
{
	// add wherever the user drags to the end of our list of points
	mPoints.push_back( event.getPos() );
}

void QuickTimeWriterApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );

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
	
	if( mMovieWriter )
		mMovieWriter.addFrame( copyWindowSurface() );
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( QuickTimeWriterApp, RendererGl )
