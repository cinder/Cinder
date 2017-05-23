#include "cinder/app/App.h"

//#define CI_MIN_LOG_LEVEL 1

#include "cinder/Log.h"
#include <sstream>

//#define CI_ASSERT_DEBUG_BREAK
#include "cinder/CinderAssert.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class DebugTestApp : public App {
	void setup();

    void testLevels();
	void testFileLogging();
	void testSystemLogging();
	void testFindLoggers();
	void testAddRemove();
	void testThreadSafety();
	void testAsserts();
	void testPerformance();
	void testBreakOnLog();
	void testMakeOrGet();

	void keyDown( KeyEvent event );

};

void DebugTestApp::setup()
{
	
	//testLevels();
	//testFileLogging();
	//testSystemLogging();

	//testAsserts();
	
	//testFindLoggers();
	//testAddRemove();
	
	testMakeOrGet();
	
	// WARNING: don't run these two unless you have 500 MB free disk space
	//testThreadSafety();
    //testPerformance();
	
	//testBreakOnLog();
}

void DebugTestApp::testLevels()
{
	log::manager()->restoreToDefault();
	CI_LOG_I( "\n--- TESTING LEVELS START----");
	
	CI_LOG_V( "verbose" );
	CI_LOG_D( "debug" );
	CI_LOG_I( "info" );
	CI_LOG_W( "warning" );
	CI_LOG_E( "error" );
	CI_LOG_F( "fatal" );
	
	CI_LOG_I( "\n--- TESTING LEVELS END----\n\n");
}

void DebugTestApp::testFileLogging()
{
	log::manager()->restoreToDefault();
	CI_LOG_I( "\n--- TESTING FILES START----");
	
	
	log::makeLogger<log::LoggerFile>( "/tmp/cinder/cinder.log" );
	log::makeLogger<log::LoggerFile>( "/tmp/cinder/ggg__Idontexist___/cinder.log" );
	log::makeLogger<log::LoggerFileRotating>( "/tmp/cinder", "loggingTests.%Y.%m.%d.log", false );

	CI_LOG_I( "This output should be in three logs." );
	CI_LOG_I( "\n--- TESTING FILES END----\n\n");
}

void DebugTestApp::testSystemLogging()
{
	log::manager()->restoreToDefault();
	CI_LOG_I( "\n--- TESTING SYSTEM START----");
	
	auto sysLog = log::makeLogger<log::LoggerSystem>();
    sysLog->setLoggingLevel( log::LEVEL_ERROR );
    CI_LOG_E( "Error log shows in system logs" );
    CI_LOG_D( "Debug log doesn't show in system logs.");
	
    log::manager()->restoreToDefault();
    log::makeLogger<log::LoggerSystem>();
    CI_LOG_E( "Error log shows in system logs" );
    CI_LOG_D( "Debug log shows in system logs.");
	
	CI_LOG_I( "\n--- TESTING SYSTEM END----\n\n");
}

void DebugTestApp::testFindLoggers()
{
	log::manager()->restoreToDefault();
	CI_LOG_I( "\n--- TESTING FIND START----");
	
	log::makeLogger<log::LoggerConsole>();
	log::makeLogger<log::LoggerConsole>();
	
	CI_ASSERT( 3 == log::manager()->getLoggers<log::LoggerConsole>().size() );
	CI_ASSERT( 3 == log::manager()->getAllLoggers().size() );
	
	auto logger = log::makeLogger<log::LoggerFile>( "/tmp/cinder/cinder.log" );
	log::makeLogger<log::LoggerFile>( "/tmp/cinder/cinder.log" );
	
	CI_ASSERT( 3 == log::manager()->getLoggers<log::LoggerConsole>().size() );
    console() << log::manager()->getLoggers<log::LoggerFile>().size() << " loggers." << endl;
	CI_ASSERT( 2 == log::manager()->getLoggers<log::LoggerFile>().size() );
	CI_ASSERT( 0 == log::manager()->getLoggers<log::LoggerBreakpoint>().size() );
	CI_ASSERT( 5 == log::manager()->getAllLoggers().size() );
	
	log::manager()->restoreToDefault();
	CI_LOG_I( "Logger searching passed." );
	CI_LOG_I( "\n--- TESTING FIND END----\n\n");
}

void DebugTestApp::testAddRemove()
{
	log::manager()->restoreToDefault();
	CI_LOG_I( "\n--- TESTING ADD/REMOVE START----");
	
	log::manager()->clearLoggers();
	
	auto logger1 = log::makeLogger<log::LoggerConsole>();
	auto logger2 = log::makeLogger<log::LoggerConsole>();

	CI_LOG_I( "This should  show up in the console twice." );
	
	auto logger3 = log::makeLogger<log::LoggerConsole>();
	auto logger4 = log::makeLogger<log::LoggerConsole>();
	auto logger5 = log::makeLogger<log::LoggerConsole>();
	
	log::manager()->removeLogger( logger1 );
	log::manager()->removeLogger( logger1 );
	log::manager()->removeLogger( logger1 );

	log::manager()->removeLogger( logger2 );
	log::manager()->removeLogger( logger3 );
	
	log::manager()->addLogger( logger2 );
	
	log::manager()->removeLogger( logger4 );
	log::manager()->removeLogger( logger5 );
	log::manager()->removeLogger( nullptr );
	
	CI_LOG_I( "This should only show up in the console once." );
	
	log::manager()->restoreToDefault();
	CI_LOG_I( "\n--- TESTING ADD/REMOVE END----\n\n");
}

void DebugTestApp::testThreadSafety()
{

	log::manager()->restoreToDefault();
	CI_LOG_I( "\n--- TESTING THREADS START----");
	CI_LOG_I( "... this may take a while, don't fret." );
	
	log::manager()->clearLoggers();
	
	auto looper = [](std::string log) {
		auto logger = log::makeLogger<log::LoggerFile>( log, false );
		for( int i=0; i<500; ++i ) {
			for( int j=0; j<500; ++j ) {
				CI_LOG_D( i * j );
			}
			log::manager()->removeLogger( logger );
			log::manager()->addLogger( logger );
		}
		
		// clean up our mess
		auto path = logger->getFilePath();
		log::manager()->removeLogger( logger );
		logger.reset();
		fs::remove( path );
	};
	
	std::thread t1( [looper] () { looper( "/tmp/thread1.log" ); } );
	std::thread t2( [looper] () { looper( "/tmp/thread2.log" ); } );
	std::thread t3( [looper] () { looper( "/tmp/thread3.log" ); } );
	
	t1.join();
	t2.join();
	t3.join();

	log::manager()->clearLoggers();
	log::makeLogger<log::LoggerConsole>();
	
	CI_LOG_I( "Threading didn't crash." );
	CI_LOG_I( "\n--- TESTING THREADS END----\n\n");
}

void DebugTestApp::testAsserts()
{
	int i = 0;
	CI_VERIFY( ( i += 1 ) );

	CI_LOG_I( "i = " << i ); // should be 1 in both debug and release

//	CI_ASSERT( false );
//	CI_ASSERT_MSG( false, "blarg" );
}

void DebugTestApp::testPerformance()
{
	
	log::manager()->restoreToDefault();
	CI_LOG_I( "\n--- TESTING PERFORMANCE START----");
	CI_LOG_I( "... this may take a while, don't fret." );
	
	log::manager()->clearLoggers();
	
	typedef struct {
		double duration;
		int hits;
	} LogMeasure;
	
	auto looper = [](std::string log, LogMeasure &stats ) {
		
		shared_ptr<log::LoggerFile> logger = nullptr;
		if( !log.empty() ) {
			logger = log::makeLogger<log::LoggerFile>( log, false );
		}
		int logHits = 0;
		ci::Timer timer;
		timer.start();
		for( int i=0; i<1000; ++i ) {
			for( int j=0; j<1000; ++j ) {
				CI_LOG_D( ++logHits );
			}
		}
		timer.stop();
		
		if( logger ) {
			// clean up our mess
			auto path = logger->getFilePath();
			log::manager()->removeLogger( logger );
			logger.reset();
			fs::remove( path );
		}

		stats.hits = logHits;
		stats.duration = timer.getSeconds();
	};
	
	LogMeasure run1, run2, run3, run4, run5, run6;
	
	// test single log single thread
	looper( "/tmp/performance.log", run1 );
	
	// test two threads single log
	std::thread t1( [ looper, &run2 ] () { looper( "/tmp/performance.log", run2 ); } );
	std::thread t2( [ looper, &run3 ] () { looper( "", run3 ); } );
	
	t1.join();
	t2.join();

	// test three threads single log
	std::thread t3( [ looper, &run4 ] () { looper( "/tmp/performance.log", run4 ); } );
	std::thread t4( [ looper, &run5 ] () { looper( "", run5 ); } );
	std::thread t5( [ looper, &run6 ] () { looper( "", run6 ); } );
	
	t3.join();
	t4.join();
	t5.join();
	
	log::manager()->clearLoggers();
	log::makeLogger<log::LoggerConsole>();
	
	
	stringstream ss;

	ss << std::fixed << setprecision(5) << (run1.duration * 1000 / run1.hits) ;
	CI_LOG_D( "Single logging took " << ss.str() << " ms / log." );

	ss = stringstream();
	ss << std::fixed << setprecision(5) << std::max( run2.duration, run3.duration ) * 1000 / ( run2.hits + run3.hits );
	CI_LOG_D( "2 threads logging took " << ss.str() << " ms / log." );

	ss = stringstream();
	ss << std::fixed << setprecision(5) << std::max( std::max( run4.duration, run5.duration ), run6.duration ) * 1000 / ( run4.hits + run5.hits + run6.hits );
	CI_LOG_D( "3 threads logging took " << ss.str() << " ms / log." );
	
	CI_LOG_I( "\n--- TESTING PERFORMANCE END----\n\n");
}

void DebugTestApp::testBreakOnLog()
{
	log::manager()->restoreToDefault();
	CI_LOG_I( "\n--- TESTING BREAKPOINTS START----");
	
	auto bLog = log::makeLogger<log::LoggerBreakpoint>();
    bLog->setTriggerLevel( log::LEVEL_DEBUG );

	CI_LOG_V( "bang" );
	CI_LOG_D( "bang" );
	CI_LOG_I( "bang" );
	CI_LOG_W( "bang" );
	CI_LOG_E( "bang" );
	CI_LOG_F( "bang" );
}

void DebugTestApp::testMakeOrGet()
{
	auto consoleLoggers = log::manager()->getLoggers<log::LoggerConsole>();
	for( auto i : consoleLoggers ) {
		log::manager()->removeLogger( i );
	}
	
	CI_ASSERT( 0 == log::manager()->getLoggers<log::LoggerConsole>().size() );
	log::makeOrGetLogger<log::LoggerConsole>();
	CI_ASSERT( 1 == log::manager()->getLoggers<log::LoggerConsole>().size() );
	log::makeOrGetLogger<log::LoggerConsole>();
	CI_ASSERT( 1 == log::manager()->getLoggers<log::LoggerConsole>().size() );
	
	CI_LOG_D( "makeOrGet is functioning." );
}

void DebugTestApp::keyDown( KeyEvent event )
{
	CI_LOG_I( "event char: " << event.getChar() << ", code: " << event.getCode() );
}

CINDER_APP( DebugTestApp, Renderer2d )
