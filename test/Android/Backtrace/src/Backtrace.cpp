#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include <list>
using namespace ci;
using namespace ci::app;
using namespace std;

#include "OtherClass.h"

void printStackTrace( const std::vector<std::string>& bt )
{
	console() << "StackTrace:" << std::endl;
	for( size_t i = 0; i < bt.size(); ++i ) {
		console() << "   " << i << ": " << bt[i] << std::endl;
	}
}

void throwFunction()
{
	throw std::bad_alloc();
}

void myThrowFunction()
{
	try {
		throwFunction();
	}
	catch( const std::exception& e ) {
		console() << "Exception occurred: " << e.what() << std::endl;
		printStackTrace( ci::stackTrace() );
	}
}

void myFunction()
{
	printStackTrace( ci::stackTrace() );
}

// We'll create a new Cinder Application by deriving from the App class
class Backtrace : public App {
  public:
	void setup();
	void mouseDrag( MouseEvent event );
#if defined( CINDER_ANDROID )
    void touchesBegan( TouchEvent event );
#endif
	void keyDown( KeyEvent event );
	void draw();

	// This will maintain a list of points which we will draw line segments between
	list<vec2>		mPoints;

	void callOtherClass() {
		OtherClass oc;

		std::string s;
		oc.functionA( s );
	}
};

void Backtrace::setup()
{
	console() << "-" << std::endl;
	console() << "-" << std::endl;

	myFunction();

	console() << "-" << std::endl;
	console() << "-" << std::endl;

	myThrowFunction();

	console() << "-" << std::endl;
	console() << "-" << std::endl;

	callOtherClass();
}

void Backtrace::mouseDrag( MouseEvent event )
{
	mPoints.push_back( event.getPos() );
}

#if defined( CINDER_ANDROID )
void Backtrace::touchesBegan( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
        mPoints.push_back( touchIt->getPos() );
	}
}
#endif

void Backtrace::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
}

void Backtrace::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );

	gl::color( 1.0f, 0.5f, 0.25f );	
	gl::begin( GL_LINE_STRIP );
	for( auto pointIter = mPoints.begin(); pointIter != mPoints.end(); ++pointIter ) {
		gl::vertex( *pointIter );
	}
	gl::end();
}

// This line tells Cinder to actually create the application
CINDER_APP( Backtrace, RendererGl )
