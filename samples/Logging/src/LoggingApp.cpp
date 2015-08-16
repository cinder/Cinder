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
	
	// this routine will enable logging to a given file
	void enableFileLogging();
	// this routine will enable logging to a given file, and rotate that file daily
	// at the first logging event past midnight
	void enableRotatingFileLogging();
	// this routine will enable system logging via syslog (OS X) and EventLog (Windows).
	// WinRT is still being worked on.
	void enableSysLogging();
};

void LoggingApp::setup()
{
	// note: cinder will only log to one file at a time, so if you enableFileLogging and
	// enableRotatingFileLogging, it will just use whichever was called last.
	
	enableFileLogging();
	//enableRotatingFileLogging();
	enableSysLogging();

}

void LoggingApp::enableFileLogging()
{
	// cinder will create the folder structure for you, and defaults to appending to
	// the logging file
	log::manager()->enableFileLogging( "/tmp/logging/cinder.log" );
}

void LoggingApp::enableRotatingFileLogging()
{
	// cinder will create the folder structure for you, and when it creates the log
	// file it will use strtime to turn your formatted string into an actual string.
	// you can format it any way you like, but note that currently it will only roll
	// over the first time you log after midnight, independent of your string format.
	// this is obviously very useful for permanent installs
	log::manager()->enableFileLogging( "/tmp/logging", "cinder.%Y.%m.%d.log", false );
}

void LoggingApp::enableSysLogging()
{
	// this will enable system logging (currently syslog or eventlog), and will let
	// you set the minimum logging level independent of the console/file output
	// logging levels.  if you do not explicitly set the system logging level, it
	// defaults to the same as the global minimum logging level
	log::manager()->enableSystemLogging();
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
