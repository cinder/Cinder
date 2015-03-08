#include "cinder/app/AppBasic.h"

#include "cinder/Log.h"

#define CI_ASSERT_DEBUG_BREAK
#include "cinder/CinderAssert.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class DebugTestApp : public AppBasic {
	void setup();

	void testEnableFile();
	void testRotatingFile();
	void testEnableDisable();
	void testSystemLevel();
	void testAddFile();
	void testAddRemove();
	void testAsserts();

	void keyDown( KeyEvent event );

};

void DebugTestApp::setup()
{

	//testEnableFile();
	testRotatingFile();
	//testEnableDisable();
	testSystemLevel();
	//testAddFile();
	//testAddRemove();
	//testAsserts();
	
}

void DebugTestApp::testAsserts()
{
	int i = 0;
	CI_VERIFY( ( i += 1 ) );

	CI_LOG_I( "i = " << i ); // should be 1 in both debug and release

//	CI_ASSERT( false );
//	CI_ASSERT_MSG( false, "blarg" );
}

void DebugTestApp::testEnableFile()
{
	log::manager()->enableFileLogging();
//	log::manager()->enableFileLogging( "/tmp/blarg/cinder.log" );  // FIXME: writing to a non-existent folder path is broken.


	CI_LOG_I( "enabled file logger" );
}

void DebugTestApp::testRotatingFile()
{
	log::manager()->enableFileLogging( "/tmp", "loggingTests.%Y.%m.%d.log", false );
	// also a bug
	//log::manager()->enableFileLogging( "/tmp/cinder", "loggingTests.%Y.%m.%d.log", false );
}

void DebugTestApp::testSystemLevel()
{
	log::manager()->enableSystemLogging();
	log::manager()->setSystemLoggingLevel(log::LEVEL_ERROR);
	CI_LOG_I( "This should not show up in the sys log." );
	CI_LOG_E( "This should show up in the sys log." );
	log::manager()->setSystemLoggingLevel(log::LEVEL_VERBOSE);
	CI_LOG_V( "This should show up in the sys log as well." );
	
}

void DebugTestApp::testEnableDisable()
{
	log::manager()->enableFileLogging();
	CI_LOG_I( "enabled file logger" );

	log::manager()->disableFileLogging();
	CI_LOG_I( "disable file logger" );
}

void DebugTestApp::testAddFile()
{
	log::manager()->addLogger( new log::LoggerFileThreadSafe );

	CI_LOG_I( "added file logger" );
}

void DebugTestApp::testAddRemove()
{
	auto logger = new log::LoggerSysLog;
	log::manager()->addLogger( logger );
	CI_LOG_I( "added LoggerSysLog" );
	log::manager()->removeLogger( logger );
	CI_LOG_I( "removed LoggerSysLog" );
}

void DebugTestApp::keyDown( KeyEvent event )
{
	auto loggers = log::manager()->getAllLoggers();
	CI_LOG_I( "event char: " << event.getChar() << ", code: " << event.getCode() );
}

CINDER_APP_BASIC( DebugTestApp, Renderer2d )
