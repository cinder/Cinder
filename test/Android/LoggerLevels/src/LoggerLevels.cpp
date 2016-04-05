#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// We'll create a new Cinder Application by deriving from the App class
class LoggerLevels : public App {
  public:
	void setup();
	void draw();
};

void LoggerLevels::setup()
{
	CI_LOG_V( "Verbose Log Statement" );
	CI_LOG_I( "Info Log Statement" );
	CI_LOG_D( "Debug Log Statement" );
	CI_LOG_W( "Warning Log Statement" );
	CI_LOG_E( "Error Log Statement" );
	CI_LOG_F( "Fatal Log Statement" );
}

void LoggerLevels::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );
}

// This line tells Cinder to actually create the application
CINDER_APP( LoggerLevels, RendererGl )
