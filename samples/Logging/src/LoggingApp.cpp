#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

/*
 This is a basic example of how cinder logging can be used.  It's fairly feature rich,
 so you can see some of the more advanced usages in the testing app found in
 tests/DebugTest, and of course, by looking into the source code itself.

 To use, enable or disable the various logging methods, and then use a mouse event
 to cause a logging message to occur.
*/

/*
 DEFAULT MINIMUM LOGGING LEVEL
 
 ~ For a full understanding of the logging levels available, check "cinder/Log.h"
 
 When compiling cinder in DEBUG, it defaults to a minimum logging level of LEVEL_VERBOSE
 When compiling cinder in RELEASE, it defaults to a minimum logging level of LEVEL_INFO
 
 The minimum logging level is set at compile time and we optimize out all logging calls
 below the min level which will prevent burning CPU on non necessary log calls.
 
 The CI_MIN_LOG_LEVEL define can be used to override this.  So, if you wanted
 VERBOSE logging in RELEASE, you would include the following define BEFORE you include
 the logging header.
 	
 #define CI_MIN_LOG_LEVEL 0
 
*/

// as noted above, this sets the logging to LEVEL_VERBOSE independent of compilation mode
//#define CI_MIN_LOG_LEVEL 0

#include "cinder/Log.h"

class LoggingApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
	
	// This routine will enable logging to a given file.  This file will not rotate.
	void enableFileLogging();
	// This routine will enable logging to a given file, and rotate that file daily
	// at the first logging event past midnight
	void enableRotatingFileLogging();
	// This routine will enable system logging via syslog (OS X) and EventLog (Windows).
	// WinRT is still being worked on.
	void enableSysLogging();
};

void LoggingApp::setup()
{
	// Note: cinder will only log to one file at a time, so if you enableFileLogging and
	// enableRotatingFileLogging, it will just use whichever was called last.
	
	enableFileLogging();
	//enableRotatingFileLogging();
	enableSysLogging();

}

void LoggingApp::enableFileLogging()
{
	// Cinder will create the folder structure for you, and defaults to appending to
	// the logging file.
	log::manager()->enableFileLogging( "/tmp/logging/cinder.log" );
    
    // If you wanted to overwrite the file rather than append, you'd call this.
    //log::manager()->enableFileLogging( "/tmp/logging/cinder.log", false );
}

void LoggingApp::enableRotatingFileLogging()
{
	// Cinder will create the folder structure for you, and when it creates the log
	// file it will use strtime to turn your formatted string into an actual string.
	// You can format it any way you like, but note that currently it will only roll
	// over the first time you log after midnight, independent of your string format.
	// This is obviously very useful for permanent installs.
	log::manager()->enableFileLogging( "/tmp/logging", "cinder.%Y.%m.%d.log" );
    
    // If for some reason you wanted to overwrite the file rather than append,
    // you'd call this.
    //log::manager()->enableFileLogging( "/tmp/logging", "cinder.%Y.%m.%d.log", false );
}

void LoggingApp::enableSysLogging()
{
	// This will enable system logging (currently syslog or eventlog), and will let
	// you set the minimum logging level independent of the global (console/file output)
	// logging levels.  if you do not explicitly set the system logging level, it
	// defaults to the same as the global minimum logging level.
	log::manager()->enableSystemLogging();
    
    // The system logging level is independent of the minimum logging level,
    // and you can set it via the call below.  Keep in mind that setting the system
    // logging level to something lower than the CI_MIN_LOG_LEVEL would not work
    // as expected, as the system logging level can never be lower than the
    // minimum logging level.  This is because we optimize out logging calls
    // below CI_MIN_LOG_LEVEL.
	log::manager()->setSystemLoggingLevel(log::LEVEL_WARNING);
}

void LoggingApp::mouseDown( MouseEvent event )
{
	CI_LOG_V( "VERBOSE log at " << getElapsedSeconds() << " seconds" );
	CI_LOG_D( "DEBUG log at " 	<< getElapsedSeconds() << " seconds" );
	CI_LOG_I( "INFO log at "	<< getElapsedSeconds() << " seconds" );
	CI_LOG_W( "WARNING log at " << getElapsedSeconds() << " seconds" );
	CI_LOG_E( "ERROR log at " 	<< getElapsedSeconds() << " seconds" );
	CI_LOG_F( "FATAL log at " 	<< getElapsedSeconds() << " seconds" );
}

void LoggingApp::update()
{
}

void LoggingApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( LoggingApp, RendererGl )
