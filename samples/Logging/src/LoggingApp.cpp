#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#include "cinder/Log.h"

class LoggingApp : public App {

	//! This sample shows three basic logging use cases:
	//!  - File Logging
	//!  - Rotating File Logging
	//!  - System Logging

  public:
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

	//! enableFileLogging will enable logging to a given file.
	//! This file will not rotate, but you can control the file appending.
	void enableFileLogging();

	//! enableFileLoggingRotating will enable logging to a daily rotating file.
	//! The file will rotate daily at the first logging event past midnight.
	void enableFileLoggingRotating();

	//! enableSysLogging will enable platform specific system logging.
	//! Currently this means syslog (OS X) and EventLog (Windows).
	void enableSysLogging();

};

void LoggingApp::setup()
{
	enableFileLogging();
	enableFileLoggingRotating();
	enableSysLogging();
}

void LoggingApp::keyDown( KeyEvent event )
{
	//! Change log trigger levels for all active loggers with keys 0-5.
	if( event.isShiftDown() && event.isAltDown() ) {
		switch( event.getCode() ) {
		case KeyEvent::KEY_0: log::manager()->setLevel( log::LEVEL_VERBOSE ); break;
		case KeyEvent::KEY_1: log::manager()->setLevel( log::LEVEL_DEBUG ); break;
		case KeyEvent::KEY_2: log::manager()->setLevel( log::LEVEL_INFO ); break;
		case KeyEvent::KEY_3: log::manager()->setLevel( log::LEVEL_WARNING ); break;
		case KeyEvent::KEY_4: log::manager()->setLevel( log::LEVEL_ERROR ); break;
		case KeyEvent::KEY_5: log::manager()->setLevel( log::LEVEL_FATAL ); break;
		default: break;
		}
	}
}

void LoggingApp::enableFileLogging()
{
	//! This call will append log messages to the file `cinder.log` in the folder `/tmp/logging`.
	//! If the folder path `/tmp/logging` does not exist, it will be created for you.

	log::makeLogger<log::LoggerFile>( app::getAppPath() / "cinder.log", true );
}

void LoggingApp::enableFileLoggingRotating()
{
	//! This call will append log messages to a rotating file in the folder `/tmp/logging`.
	//! The filename `cinder.%Y.%m.%d.log` will be evaluated by strftime.  For the example
	//! below, the filename could evaluate to `cinder.2015.08.28.log`.
	log::makeLogger<log::LoggerFileRotating>( app::getAppPath() / "logs", "cinder.%Y.%m.%d.log", true, []( const fs::path& path ) {
		// Only preserve logs from the last 10 days
		ci::limitDirectoryFileCount( path.parent_path(), 10 );
	} );
}

void LoggingApp::enableSysLogging()
{
	//! This call will enable system logging, which will default to the same logging
	//! level as console/file logging.

	auto sysLogger = log::makeLogger<log::LoggerSystem>();

	//! This call will set the system logging level independent of the file/console logging level.
	//! The system logging level can not be lower than the preprocessor-defined level.
	sysLogger->setLevel( log::LEVEL_WARNING );
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
