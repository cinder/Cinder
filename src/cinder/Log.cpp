/*
 Copyright (c) 2014, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "cinder/Log.h"
#include "cinder/CinderAssert.h"
#include "cinder/Utilities.h"
#include "cinder/Breakpoint.h"
#include "cinder/app/Platform.h"

#if defined( CINDER_COCOA )
	#include "cinder/app/cocoa/PlatformCocoa.h"
	#import <Foundation/Foundation.h>
	#include <syslog.h>
#elif defined( CINDER_MSW )
	#include <Windows.h>
	#include <codecvt>
#endif

#if defined( CINDER_COCOA ) && ( ! defined( __OBJC__ ) )
	#error "This file must be compiled as Objective-C++ on the Mac"
#endif

#include <mutex>
#include <time.h>

using namespace std;

namespace cinder { namespace log {

class LoggerImplMulti : public Logger {
public:

	void add( Logger *logger )							{ mLoggers.push_back( std::unique_ptr<Logger>( logger ) ); }
	void add( std::unique_ptr<Logger> &&logger )		{ mLoggers.emplace_back( move( logger ) ); }

	template <typename LoggerT>
	LoggerT* findType();

	void remove( Logger *logger );

	const vector<unique_ptr<Logger> >& getLoggers() const	{ return mLoggers; }

	void write( const Metadata &meta, const std::string &text ) override;

private:
	vector<unique_ptr<Logger> >	mLoggers; // TODO: make set? don't want duplicates
};

namespace  {

// output format is YYYY-MM-DD.HH:mm:ss
const std::string getCurrentDateTimeString()
{
	time_t timeSinceEpoch = time( NULL );
	struct tm *now = localtime( &timeSinceEpoch );

	char result[100];
	strftime( result, sizeof( result ), "%Y-%m-%d.%X", now );

	return result;
}

int getCurrentYearDay()
{
	time_t timeSinceEpoch = time( NULL );
	struct tm *now = localtime( &timeSinceEpoch );

	return now->tm_year * 1000 + now->tm_yday;
}

const std::string getDailyLogString( const std::string& format )
{
	time_t timeSinceEpoch = time( NULL );
	struct tm *now = localtime( &timeSinceEpoch );

	char result[100];
	strftime( result, sizeof( result ), format.c_str(), now );

	return result;
}

} // anonymous namespace

// ----------------------------------------------------------------------------------------------------
// MARK: - LogManager
// ----------------------------------------------------------------------------------------------------

LogManager*	LogManager::sInstance = new LogManager;	// note: leaks to enable logging during shutdown

LogManager* manager()
{
	return LogManager::instance();
}

LogManager::LogManager()
{
	restoreToDefault();
}

void LogManager::resetLogger( Logger *logger )
{
	lock_guard<mutex> lock( mMutex );

	mLogger.reset( logger );

	LoggerImplMulti *multi = dynamic_cast<LoggerImplMulti *>( logger );
	mLoggerMulti = multi ? multi : nullptr;

	mConsoleLoggingEnabled = mFileLoggingEnabled = mSystemLoggingEnabled = false;
}

void LogManager::addLogger( Logger *logger )
{
	lock_guard<mutex> lock( mMutex );

	if( ! mLoggerMulti ) {
		auto loggerMulti = unique_ptr<LoggerImplMulti>( new LoggerImplMulti );
		loggerMulti->add( move( mLogger ) );
		mLoggerMulti = loggerMulti.get();
		mLogger = move( loggerMulti );
	}

	mLoggerMulti->add( logger );
}


void LogManager::removeLogger( Logger *logger )
{
	CI_ASSERT( mLoggerMulti );

	mLoggerMulti->remove( logger );
}

void LogManager::restoreToDefault()
{
	lock_guard<mutex> lock( mMutex );

	mLogger.reset( new LoggerConsoleThreadSafe );
	mLoggerMulti = nullptr;
	mConsoleLoggingEnabled = true;
	mFileLoggingEnabled = false;
	mSystemLoggingEnabled = false;
	mBreakOnLogEnabled = false;

	switch( CI_MIN_LOG_LEVEL ) {
		case 5: mSystemLoggingLevel = LEVEL_FATAL;      break;
		case 4: mSystemLoggingLevel = LEVEL_ERROR;      break;
		case 3: mSystemLoggingLevel = LEVEL_WARNING;	break;
		case 2: mSystemLoggingLevel = LEVEL_INFO;       break;
		case 1: mSystemLoggingLevel = LEVEL_DEBUG;		break;
		case 0: mSystemLoggingLevel = LEVEL_VERBOSE;	break;
		default: CI_ASSERT_NOT_REACHABLE();
	}
}

vector<Logger *> LogManager::getAllLoggers()
{
	vector<Logger *> result;

	if( mLoggerMulti ) {
		for( const auto &logger : mLoggerMulti->getLoggers() )
			result.push_back( logger.get() );
	}
	else
		result.push_back( mLogger.get() );

	return result;
}

void LogManager::enableConsoleLogging()
{
	if( mConsoleLoggingEnabled )
		return;

	addLogger( new LoggerConsoleThreadSafe );
	mConsoleLoggingEnabled = true;
}

bool LogManager::initFileLogging()
{
	if( mFileLoggingEnabled ) {
		// destroys previous file logger to prepare for changes
		auto logger = mLoggerMulti->findType<LoggerFile>();
		if( logger )
			disableFileLogging();
		else
			return false;
	}
	return true;
}

void LogManager::enableFileLogging( const fs::path &path, bool appendToExisting )
{
	if( ! initFileLogging() ) {
		return;
	}

	addLogger( new LoggerFileThreadSafe( path, appendToExisting ) );
	mFileLoggingEnabled = true;
}

void LogManager::enableFileLogging( const fs::path &folder, const std::string &formatStr, bool appendToExisting )
{
	if( ! initFileLogging() ) {
		return;
	}

	addLogger( new LoggerFileThreadSafe( folder, formatStr, appendToExisting ) );
	mFileLoggingEnabled = true;
}

void LogManager::setFileLoggingEnabled( bool enable, const fs::path &filePath, bool appendToExisting )
{
	if( enable )
		enableFileLogging( filePath, appendToExisting );
	else
		disableFileLogging();
}


void LogManager::setFileLoggingEnabled( bool enable, const fs::path &folder, const string &formatStr, bool appendToExisting )
{
	if( enable )
		enableFileLogging( folder, formatStr, appendToExisting );
	else
		disableFileLogging();
}

void LogManager::enableSystemLogging()
{
	if( mSystemLoggingEnabled )
		return;

	addLogger( new LoggerSystem );
	setSystemLoggingLevel( mSystemLoggingLevel );
	mSystemLoggingEnabled = true;
}

void LogManager::setSystemLoggingLevel( Level level )
{
	mSystemLoggingLevel = level;
	
	auto logger = mLoggerMulti->findType<LoggerSystem>();
	logger->setLoggingLevel( level );
}

void LogManager::disableConsoleLogging()
{
	if( ! mConsoleLoggingEnabled || ! mLoggerMulti )
		return;

	auto logger = mLoggerMulti->findType<LoggerConsole>();
	mLoggerMulti->remove( logger );

	mConsoleLoggingEnabled = false;
}

void LogManager::disableFileLogging()
{
	if( ! mFileLoggingEnabled || ! mLoggerMulti )
		return;

	auto logger = mLoggerMulti->findType<LoggerFile>();
	mLoggerMulti->remove( logger );

	mFileLoggingEnabled = false;
}

void LogManager::disableSystemLogging()
{
	if( ! mSystemLoggingEnabled || ! mLoggerMulti )
		return;

	auto logger = mLoggerMulti->findType<LoggerSystem>();
	mLoggerMulti->remove( logger );
	mSystemLoggingEnabled = false;
}

void LogManager::enableBreakOnLevel( Level triggerLevel )
{
	if( mBreakOnLogEnabled ) {
		auto logger = mLoggerMulti->findType<LoggerBreakpoint>();
		logger->setTriggerLevel( triggerLevel );
	}
	else {
		addLogger( new LoggerBreakpoint( triggerLevel ) );
		mBreakOnLogEnabled = true;
	}
}

void LogManager::disableBreakOnLog()
{
	if( ! mBreakOnLogEnabled )
		return;
	
	auto logger = mLoggerMulti->findType<LoggerBreakpoint>();
	if( logger )
		mLoggerMulti->remove( logger );

	mBreakOnLogEnabled = false;
}

// ----------------------------------------------------------------------------------------------------
// MARK: - Logger
// ----------------------------------------------------------------------------------------------------

void Logger::writeDefault( std::ostream &stream, const Metadata &meta, const std::string &text )
{
	stream << meta.mLevel << " ";

	if( isTimestampEnabled() )
		stream << getCurrentDateTimeString() << " ";

	stream << meta.mLocation << " " << text << endl;
}

// ----------------------------------------------------------------------------------------------------
// MARK: - LoggerConsole
// ----------------------------------------------------------------------------------------------------

void LoggerConsole::write( const Metadata &meta, const string &text )
{
	writeDefault( app::Platform::get()->console(), meta, text );
}

// ----------------------------------------------------------------------------------------------------
// MARK: - LoggerImplMulti
// ----------------------------------------------------------------------------------------------------

template <typename LoggerT>
LoggerT* LoggerImplMulti::findType()
{
	for( const auto &logger : mLoggers ) {
		auto result = dynamic_cast<LoggerT *>( logger.get() );
		if( result )
			return result;
	}

	return nullptr;
}

void LoggerImplMulti::remove( Logger *logger )
{
	mLoggers.erase( remove_if( mLoggers.begin(), mLoggers.end(),
							  [logger]( const std::unique_ptr<Logger> &o ) {
								  return o.get() == logger;
							  } ),
				   mLoggers.end() );
}

void LoggerImplMulti::write( const Metadata &meta, const string &text )
{
	for( auto &logger : mLoggers )
		logger->write( meta, text );
}

// ----------------------------------------------------------------------------------------------------
// MARK: - LoggerFile
// ----------------------------------------------------------------------------------------------------

LoggerFile::LoggerFile( const fs::path &filePath, bool appendToExisting )
	: mFilePath( filePath ), mAppend( appendToExisting ), mRotating( false )
{
	if( mFilePath.empty() )
		mFilePath = getDefaultLogFilePath();

	setTimestampEnabled();
}

LoggerFile::LoggerFile( const fs::path &folder, const std::string &formatStr, bool appendToExisting )
	: mFolderPath( folder ), mDailyFormatStr( formatStr ), mAppend( appendToExisting ), mRotating( true )
{
	CI_ASSERT_MSG( ! formatStr.empty(), "cannot provide empty formatStr" );
	
	if( mFolderPath.empty() )
		mFolderPath = getDefaultLogFilePath().parent_path();

	mYearDay = getCurrentYearDay();
	mFilePath = mFolderPath / fs::path( getDailyLogString( mDailyFormatStr ) );

	setTimestampEnabled();
}

LoggerFile::~LoggerFile()
{
	if( mStream.is_open() )
		mStream.close();
}

void LoggerFile::write( const Metadata &meta, const string &text )
{
	if( mRotating && mYearDay != getCurrentYearDay() ) {
		mFilePath = mFolderPath / fs::path( getDailyLogString( mDailyFormatStr ) );
		mYearDay = getCurrentYearDay();

		if( mStream.is_open() )
			mStream.close();
	}

	if( ! mStream.is_open() ) {
		ensureDirectoryExists();
		mAppend ? mStream.open( mFilePath.string(), std::ofstream::app ) : mStream.open( mFilePath.string() );
	}
	
	writeDefault( mStream, meta, text );
}

fs::path LoggerFile::getDefaultLogFilePath() const
{
	return app::Platform::get()->getExecutablePath() / fs::path( "cinder.log" );
}

void LoggerFile::ensureDirectoryExists()
{
	fs::path dir = mFilePath.parent_path();
	if( dir.empty() ) {
		// make single file path names explicitly next to executable
		mFilePath = app::Platform::get()->getExecutablePath() / mFilePath;
	}
	else if( ! fs::is_directory( dir ) ) {
		bool success = fs::create_directories( dir );
		if( ! success ) {
			// not using CI_LOG_E since it could lead to recursion
			cerr << "ci::log::LoggerFile error: Unable to create folder \"" << dir.string() << "\"" << endl;
		}
	}
}

// ----------------------------------------------------------------------------------------------------
// MARK: - LoggerBreakpoint
// ----------------------------------------------------------------------------------------------------

void LoggerBreakpoint::write( const Metadata &meta, const string &text )
{
	if( meta.mLevel >= mTriggerLevel ) {
		CI_BREAKPOINT();
	}
}

#if defined( CINDER_COCOA )

// ----------------------------------------------------------------------------------------------------
// MARK: - ImplSysLog
// ----------------------------------------------------------------------------------------------------

class LoggerSystem::ImplSysLog : public Logger {
public:
	ImplSysLog()
	{
		// determine app name from it's NSBundle. https://developer.apple.com/library/mac/qa/qa1544/_index.html
		NSBundle *bundle = app::PlatformCocoa::get()->getBundle();
		NSString *bundlePath = [bundle bundlePath];
		NSString *appName = [[NSFileManager defaultManager] displayNameAtPath: bundlePath];
		
		const char *cAppName = [appName UTF8String];
		openlog( cAppName, ( LOG_CONS | LOG_PID ), LOG_USER );
	}
	
	virtual ~ImplSysLog()
	{
		closelog();
	}
	
	void write( const Metadata &meta, const std::string &text ) override
	{
		int sysLevel = cinderLogLevelToSysLogLevel( meta.mLevel );
		syslog( sysLevel , "%s %s", meta.toString().c_str(), text.c_str() );
	};
	
protected:
	int cinderLogLevelToSysLogLevel( Level cinderLogLevel )
	{
		switch( cinderLogLevel ) {
			case LEVEL_FATAL:	return LOG_CRIT;
			case LEVEL_ERROR:	return LOG_ERR;
			case LEVEL_WARNING:	return LOG_WARNING;
				// We never return lower than LOG_NOTICE for OS X SysLog to ensure the message arrives
				// http://apple.stackexchange.com/questions/13484/messages-issued-by-syslog-not-showing-up-in-system-logs
			case LEVEL_INFO:	return LOG_NOTICE;
			case LEVEL_DEBUG:	return LOG_NOTICE;
			case LEVEL_VERBOSE:	return LOG_NOTICE;
			default: CI_ASSERT_NOT_REACHABLE();
		}
	}
};
	
#elif defined( CINDER_MSW )

// ----------------------------------------------------------------------------------------------------
// MARK: - ImplEventLog
// ----------------------------------------------------------------------------------------------------

class LoggerSystem::ImplEventLog : public Logger {
public:
	ImplEventLog()
	{
		char filename[MAX_PATH];
		wchar_t wFilename[MAX_PATH];
		string stem;
		
		DWORD size = ::GetModuleFileNameA( NULL, filename, MAX_PATH );
		if( size ) {
			fs::path exePath( filename );
			stem = exePath.stem().string();
		} else {
			app::Platform::get()->console() << CINDER_CURRENT_FUNCTION << "[" << __LINE__
			<< "] could not determine application name, defaulting to 'CinderApp'" << endl;
			stem = "CinderApp";
		}
		
		::mbstowcs( wFilename, stem.c_str(), stem.size() + 1 );
		mHLog = ::RegisterEventSourceW( 0, wFilename );
		
		if( ! mHLog ) {
			app::Platform::get()->console() << CINDER_CURRENT_FUNCTION << "[" << __LINE__
			<< "] RegisterEventSourceW() failed with " << GetLastError() << endl;
		}
	}
	
	virtual ~ImplEventLog()
	{
		if( mHLog ) {
			::CloseEventLog( mHLog );
		}
	}
	
	void write( const Metadata& meta, const std::string& text ) override
	{
		int eventLevel = cinderLogLevelToEventLogLevel( meta.mLevel );
	
		std::wstring wMeta = mConverter.from_bytes( meta.toString() );
		std::wstring wText = mConverter.from_bytes( text );
	
		LPCTSTR wStrings[2];
		wStrings[0] = wMeta.c_str();
		wStrings[1] = wText.c_str();
	
		::ReportEventW( mHLog, eventLevel, 0, 0, 0, 2, 0, wStrings, 0 );
	}

protected:
	int cinderLogLevelToEventLogLevel( Level cinderLogLevel )
	{
		switch( cinderLogLevel ) {
				// MSDN Event Types don't seem particularly granular, hence the repeats
			case LEVEL_FATAL:	return EVENTLOG_ERROR_TYPE;
			case LEVEL_ERROR:	return EVENTLOG_ERROR_TYPE;
			case LEVEL_WARNING:	return EVENTLOG_WARNING_TYPE;
			case LEVEL_INFO:	return EVENTLOG_INFORMATION_TYPE;
			case LEVEL_DEBUG:	return EVENTLOG_INFORMATION_TYPE;
			case LEVEL_VERBOSE:	return EVENTLOG_INFORMATION_TYPE;
			default: CI_ASSERT_NOT_REACHABLE();
		}
	}
	
	HANDLE			mHLog;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> mConverter;
};

#endif

	
// ----------------------------------------------------------------------------------------------------
// MARK: - LoggerSystem
// ----------------------------------------------------------------------------------------------------

LoggerSystem::LoggerSystem()
{
	mMinLevel = LEVEL_VERBOSE;
#if defined( CINDER_COCOA )
	LoggerSystem::mImpl = std::unique_ptr<ImplSysLog>( new ImplSysLog() );
#elif defined( CINDER_MSW )
	LoggerSystem::mImpl = std::unique_ptr<ImplEventLog>( new ImplEventLog() );
#endif
}

LoggerSystem::~LoggerSystem()
{
}

void LoggerSystem::write( const Metadata &meta, const std::string &text )
{
#if ! defined( CINDER_WINRT ) // Currently no system logging support on WinRT
	if( meta.mLevel >= mMinLevel ) {
		mImpl->write( meta, text );
	}
#endif
}
	
// ----------------------------------------------------------------------------------------------------
// MARK: - Helper Classes
// ----------------------------------------------------------------------------------------------------

string Metadata::toString() const
{
	stringstream ss;
	ss << mLevel << " " << mLocation;
	return ss.str();
}

ostream& operator<<( ostream &os, const Location &rhs )
{
	os << rhs.getFunctionName() << "[" << rhs.getLineNumber() << "]";
	return os;
}

ostream& operator<<( ostream &lhs, const Level &rhs )
{
	switch( rhs ) {
		case LEVEL_VERBOSE:		lhs << "|verbose|";	break;
		case LEVEL_DEBUG:		lhs << "|debug  |";	break;
		case LEVEL_INFO:		lhs << "|info   |";	break;
		case LEVEL_WARNING:		lhs << "|warning|";	break;
		case LEVEL_ERROR:		lhs << "|error  |";	break;
		case LEVEL_FATAL:		lhs << "|fatal  |";	break;
		default: CI_ASSERT_NOT_REACHABLE();
	}
	return lhs;
}

} } // namespace cinder::log
