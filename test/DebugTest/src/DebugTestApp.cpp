#include "cinder/app/App.h"

#include "cinder/Log.h"

#define CI_ASSERT_DEBUG_BREAK
#include "cinder/CinderAssert.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class DebugTestApp : public App {
	void setup();

	void testEnableFileLogger();
	void testEnableSysLogger();
	void testEnableBadFilePath();
	void testRotatingFile();
	void testEnableDisable();
	void testSystemLevel();
	void testAddFile();
	void testAddRemove();
	void testAsserts();
	void testBreakOnLog();


	void keyDown( KeyEvent event );

};

void DebugTestApp::setup()
{
	//testEnableFileLogger();
//	testEnableSysLogger();
//	testEnableBadFilePath();
//	testEnableDisable();
//	testAddRemove();
	//testRotatingFile();
	//testEnableDisable();
//	testSystemLevel();
	//testAddFile();
	//testAddRemove();
	//testAsserts();
	testBreakOnLog();
}

void DebugTestApp::testAsserts()
{
	int i = 0;
	CI_VERIFY( ( i += 1 ) );

	CI_LOG_I( "i = " << i ); // should be 1 in both debug and release

//	CI_ASSERT( false );
//	CI_ASSERT_MSG( false, "blarg" );
}

void DebugTestApp::testBreakOnLog()
{
	log::manager()->enableBreakOnError();
	//log::manager()->enableBreakOnLevel( log::LEVEL_WARNING );

	CI_LOG_V( "bang" );
	CI_LOG_I( "bang" );
	CI_LOG_W( "bang" );
	CI_LOG_E( "bang" );
	CI_LOG_F( "bang" );
}

void DebugTestApp::testEnableFileLogger()
{
	log::manager()->enableFileLogging();

//	log::manager()->enableFileLogging( "/tmp/blarg/cinder.log" );
//	log::manager()->enableFileLogging( "/tmp", "loggingTests.%Y.%m.%d.log", false );
//	log::manager()->enableFileLogging( "/tmp/cinder", "loggingTests.%Y.%m.%d.log", false );

	CI_LOG_I( "enabled file logging" );
}

void DebugTestApp::testRotatingFile()
{
	log::manager()->enableFileLogging( "/tmp", "loggingTests.%Y.%m.%d.log", false );

	// empty formatStr causes an assertion failure:
//	log::manager()->enableFileLogging( "", "", false );

	// test nested folder path:
//	log::manager()->enableFileLogging( "/tmp/cinder", "loggingTests.%Y.%m.%d.log", false );
}

void DebugTestApp::testEnableSysLogger()
{
	log::manager()->enableSystemLogging();
	CI_LOG_I( "enabled system logging" );
	CI_LOG_V( "verbose message" );
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
	// TEMPORARY: commented out to build on windows, until LoggerSysLog is implemented there.
// 	auto logger = new log::LoggerSysLog;
// 	log::manager()->addLogger( logger );
// 	CI_LOG_I( "added LoggerSysLog" );
// 	log::manager()->removeLogger( logger );
// 	CI_LOG_I( "removed LoggerSysLog" );
}

void DebugTestApp::testEnableBadFilePath()
{
	log::manager()->enableFileLogging( "ABCDE:/Volumes/__THISDOESNOTEXIST_gf5jk313__/tmp/cinder.log" );
	
	CI_LOG_I( "This should hit the console but not deal with the bad files." );
}


void DebugTestApp::keyDown( KeyEvent event )
{
	auto loggers = log::manager()->getAllLoggers();
	CI_LOG_I( "event char: " << event.getChar() << ", code: " << event.getCode() );
}

CINDER_APP( DebugTestApp, Renderer2d )
