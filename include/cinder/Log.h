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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Filesystem.h"
#include "cinder/CurrentFunction.h"
#include "cinder/CinderAssert.h"
#include "cinder/System.h"

#include <sstream>
#include <fstream>
#include <vector>
#include <memory>
#include <mutex>

namespace cinder { namespace log {

typedef enum {
	LEVEL_VERBOSE,
	LEVEL_DEBUG,
	LEVEL_INFO,
	LEVEL_WARNING,
	LEVEL_ERROR,
	LEVEL_FATAL
} Level;

struct Location {
	Location() {}

	Location( const std::string &functionName, const std::string &fileName, const size_t &lineNumber )
		: mFunctionName( functionName ), mFileName( fileName ), mLineNumber( lineNumber )
	{}

	const std::string&	getFileName() const				{ return mFileName; }
	const std::string&	getFunctionName() const			{ return mFunctionName; }
	size_t				getLineNumber() const			{ return mLineNumber; }

private:
	std::string mFunctionName, mFileName;
	size_t		mLineNumber;
};

struct Metadata {

	std::string toString() const;

	Level		mLevel;
	Location	mLocation;
};

extern std::ostream& operator<<( std::ostream &os, const Location &rhs );
extern std::ostream& operator<<( std::ostream &lhs, const Level &rhs );

//! Logger is the base class all logging objects are derived from.
//!
//! \see LoggerConsole, LoggerFile, LoggerFileRotating
class Logger {
  public:
	virtual ~Logger()	{}

	virtual void write( const Metadata &meta, const std::string &text ) = 0;
	
	//! Setter for enabling or disabling timestamps inside this loggers messages.
	void setTimestampEnabled( bool enable = true )	{ mTimeStampEnabled = enable; }
	//! Getter for checking if timestamps are enabled in this loggers messages.
	bool isTimestampEnabled() const					{ return mTimeStampEnabled; }

  protected:
	Logger() : mTimeStampEnabled( false ) {}

	void writeDefault( std::ostream &stream, const Metadata &meta, const std::string &text );

  private:
	bool mTimeStampEnabled;
};

//! LoggerConsole will print log messages in the application console window.
class LoggerConsole : public Logger {
  public:
	void write( const Metadata &meta, const std::string &text ) override;
};

//! LoggerFile will write to a specified file, either appending to or overwriting that
//! file at application startup.
class LoggerFile : public Logger {
  public:
	//! LoggerFile constructor that creates or open a log file.  File appending is
	//! configurable via \p appendToExisting.  If \p filePath is empty, uses the
	//! default \a %cinder.log filename placed next to the application binary.
	//! If \p filePath does not exist, the file will be created.  If the folder structure
	//! containing \p filePath does not exist, the folder structure will be created as well.
	LoggerFile( const fs::path &filePath = fs::path(), bool appendToExisting = true );
	
	virtual ~LoggerFile();
	
	void write( const Metadata &meta, const std::string &text ) override;
	
	//! Returns the file path targeted by this logger.
	const fs::path&		getFilePath() const		{ return mFilePath; }

  protected:
	fs::path	getDefaultLogFilePath() const;
	void		ensureDirectoryExists();

	fs::path		mFilePath;
	bool			mAppend;
	std::ofstream	mStream;
};

//! LoggerFileRotating will write to a specified file, either appending to or overwriting that
//! file.  The filename will be re-evaluated during the first logging event occuring past midnight.
class LoggerFileRotating : public LoggerFile {
  public:
	//! LoggerFileRotating constructor that creates a rotating log file.  The filename will be
	//! re-evaluated when the first logging event occurs after midnight.
	//! File appending is configurable via \p appendToExisting.  \p formatStr will be passed to
	//! [strtime](http://www.cplusplus.com/reference/ctime/strftime/) to determine the file name.
	//! The log file will then be created in the folder defined in the \p folder parameter.
	//! If \p folder is empty, uses the folder where the binary is located.  If the folder structure
	//! defined in \p folder does not exist, it will be created.
	//! If \p formatStr is empty, an assertion will be thrown.
	LoggerFileRotating( const fs::path &folder, const std::string &formatStr, bool appendToExisting = true );

	virtual ~LoggerFileRotating() { }
	
	void write( const Metadata &meta, const std::string &text ) override;
	
protected:
	fs::path		mFolderPath;
	std::string		mDailyFormatStr;
	int				mYearDay;
};

//! LoggerBreakpoint doesn't actually log anything, but triggers a breakpoint if a
//! log event occurs at or above the specified \p triggerLevel.
class LoggerBreakpoint : public Logger {
  public:
	//! Creates a LoggerBreakpoint with a specified \p triggerLevel.
	LoggerBreakpoint( Level triggerLevel = LEVEL_ERROR )
		: mTriggerLevel( triggerLevel )
	{}

	void write( const Metadata &meta, const std::string &text ) override;
	
	//! Sets the minimum logging level that will cause a breakpoint to trigger.
	void	setTriggerLevel( Level triggerLevel )	{ mTriggerLevel = triggerLevel; }
	//! Gets the minimum logging level that will cause a breakpoint to trigger.
	Level	getTriggerLevel() const					{ return mTriggerLevel; }
	
  private:
	Level	mTriggerLevel;
};

//! LoggerSystem provides 'system' logging support.  Uses syslog on Unix
//! based platforms, Event Logging on MSW platforms.
//! \note Does nothing on WinRT.
class LoggerSystem : public Logger {
public:
	LoggerSystem();
	virtual ~LoggerSystem();
	
	void write( const Metadata &meta, const std::string &text ) override;
	
	//! Sets the minimum logging level that will trigger a system log.
	//! \note This level can't be lower than \a CI_MIN_LOG_LEVEL.
	void setLoggingLevel( Level minLevel ) { mMinLevel = minLevel; }
	
protected:
	Level mMinLevel;
#if defined( CINDER_COCOA )
	class ImplSysLog;
	std::unique_ptr<ImplSysLog> mImpl;
#elif defined( CINDER_MSW )
	class ImplEventLog;
	std::unique_ptr<ImplEventLog> mImpl;
#endif
};

//! LoggerMulti is a logger that can log to multiple other Loggers.  This is
//! primarily used by LogManager as it's base Logger, when multiple log outputs
//! are enabled (ex. console and file)
class LoggerMulti : public Logger {
  public:
	void add( Logger *logger )							{ mLoggers.push_back( std::unique_ptr<Logger>( logger ) ); }
	void add( std::unique_ptr<Logger> &&logger )		{ mLoggers.emplace_back( move( logger ) ); }

	template <typename LoggerT>
	LoggerT* findType();

	void remove( Logger *logger );

	const std::vector<std::unique_ptr<Logger> >& getLoggers() const	{ return mLoggers; }

	void write( const Metadata &meta, const std::string &text ) override;

  private:
	std::vector<std::unique_ptr<Logger> >	mLoggers;
};

//! LogManager manages a stack of all active Loggers.  It can be used in one of two styles.
//! \a addLogger and \a removeLogger can be used to manually manage loggers, or helper
//! functions such as \a enableConsoleLogging and \a enableFileLogging can be used to
//! easily enable or disable an instance of a Logger internally managed by LogManager.
class LogManager {
public:
	//! Returns a pointer to the shared instance. To enable logging during shutdown, this instance is leaked at shutdown.
	static LogManager* instance()	{ return sInstance; }
	//! Destroys the shared instance. Useful to remove false positives with leak detectors like valgrind.
	static void destroyInstance()	{ delete sInstance; }
	//! Restores LogManager to its default state.
	void restoreToDefault();

	//! Resets the current Logger stack so only \p logger exists.
	void resetLogger( Logger *logger );
	//! Adds \p logger to the current stack of loggers.
	void addLogger( Logger *logger );
	//! Remove \p logger to the current stack of loggers.
	void removeLogger( Logger *logger );
	//! Returns a pointer to the current base Logger instance.
	Logger* getLogger()	{ return mLogger.get(); }
	//! Returns a logger of a specifc type, or nullptr if that type of Logger is currently not in use.
	template<typename LoggerT>
	LoggerT* getLogger();
	//! Returns a vector of all current loggers
	std::vector<Logger *> getAllLoggers();
	//! Returns the mutex used for thread safe loggers. Also used when adding or resetting new loggers.
	std::mutex& getMutex() const			{ return mMutex; }

	//! Enables LoggerConsole
	void enableConsoleLogging();
	//! Disables LoggerConsole
	void disableConsoleLogging();
	//! Enalbes or disables LoggerConsole
	void setConsoleLoggingEnabled( bool enable )		{ enable ? enableConsoleLogging() : disableConsoleLogging(); }
	//! Checks if a LogerConsole is active
	bool isConsoleLoggingEnabled() const				{ return mConsoleLoggingEnabled; }

	//! Enables an instance of LoggerFile.
	//! \note Removes the existing LoggerFile, if one exists. 
	void enableFileLogging( const fs::path &filePath = fs::path(), bool appendToExisting = true );
	//! Removes the current LoggerFile.
	void disableFileLogging();
	//! Enables or disables LoggerFile.
	void setFileLoggingEnabled( bool enable, const fs::path &filePath = fs::path(), bool appendToExisting = true );
	//! Checks if a LoggerFile is currently active.
	bool isFileLoggingEnabled() const					{ return mFileLoggingEnabled; }

	//! Enables an instance of LoggerFileRotating that rotates the target file.
	//! \note Removes the existing FileLoggerRotating, if one exists.
	void enableFileLoggingRotating( const fs::path &folder, const std::string& formatStr, bool appendToExisting = true);
	//! Removes the current LoggerFileRotating.
	void disableFileLoggingRotating();
	//! Enables or disables LoggerFileRotating.
	void setFileLoggingRotatingEnabled( bool enable, const fs::path &folder, const std::string &formatStr, bool appendToExisting = true );
	//! Checks if a LoggerFileRotating is currently active.
	bool isFileLoggingRotatingEnabled() const			{ return mFileLoggingRotatingEnabled; }
	
	//! Enables a platform specific instance of LoggerSystem.
	void enableSystemLogging();
	//! Removes the current instance of LoggerSystem.
	void disableSystemLogging();
	//! Enables or disables LoggerSystem.
	void setSystemLoggingEnabled( bool enable = true )		{ enable ? enableSystemLogging() : disableSystemLogging(); }
	//! Checks if LoggerSystem is enabled.
	bool isSystemLoggingEnabled() const					{ return mSystemLoggingEnabled; }
	//! Sets the mimimum logging level that will write to the system log.
	//! \note This level can't be lower than \a CI_MIN_LOG_LEVEL.
	void setSystemLoggingLevel( Level level );
	//! Gets the current minimum logging level that will write to the system log.
	Level getSystemLoggingLevel() const					{ return mSystemLoggingLevel; }

	//! Enables a breakpoint to be triggered when a log message happens at \a LEVEL_ERROR or higher
	void enableBreakOnError()							{ enableBreakOnLevel( LEVEL_ERROR ); }
	//! Enables a breakpoint to be triggered when a log message happens at \p trigerLevel or higher.
	void enableBreakOnLevel( Level trigerLevel );
	//! Disables any breakpoints set for logging.
	void disableBreakOnLog();

protected:
	LogManager();

	bool initFileLogging();
	bool initFileLoggingRotating();

	std::unique_ptr<Logger>	mLogger;
	LoggerMulti*			mLoggerMulti;
	mutable std::mutex		mMutex;
	bool					mConsoleLoggingEnabled, mFileLoggingEnabled, mFileLoggingRotatingEnabled;
	bool					mSystemLoggingEnabled, mBreakOnLogEnabled;
	Level					mSystemLoggingLevel;

	static LogManager *sInstance;
};

LogManager* manager();

struct Entry {
	// TODO: move &&location
	Entry( Level level, const Location &location )
		: mHasContent( false )
	{
		mMetaData.mLevel = level;
		mMetaData.mLocation = location;
	}

	~Entry()
	{
		if( mHasContent )
			writeToLog();
	}

	template <typename T>
	Entry& operator<<( const T &rhs )
	{
		mHasContent = true;
		mStream << rhs;
		return *this;
	}

	void writeToLog()
	{
		manager()->getLogger()->write( mMetaData, mStream.str() );
	}

	const Metadata&	getMetaData() const	{ return mMetaData; }

private:

	Metadata			mMetaData;
	bool				mHasContent;
	std::stringstream	mStream;
};


template<class LoggerT>
class ThreadSafeT : public LoggerT {
  public:
	template <typename... Args>
	ThreadSafeT( Args &&... args )
	: LoggerT( std::forward<Args>( args )... )
	{}

	void write( const Metadata &meta, const std::string &text ) override
	{
		std::lock_guard<std::mutex> lock( manager()->getMutex() );
		LoggerT::write( meta, text );
	}
};

typedef ThreadSafeT<LoggerConsole>		LoggerConsoleThreadSafe;
typedef ThreadSafeT<LoggerFile>			LoggerFileThreadSafe;
typedef ThreadSafeT<LoggerFileRotating> LoggerFileRotatingThreadSafe;

// ----------------------------------------------------------------------------------
// Template method implementations

template <typename LoggerT>
LoggerT* LoggerMulti::findType()
{
	for( const auto &logger : mLoggers ) {
		auto result = dynamic_cast<LoggerT *>( logger.get() );
		if( result )
			return result;
	}

	return nullptr;
}

template<typename LoggerT>
LoggerT* LogManager::getLogger()
{
	auto loggerMulti = dynamic_cast<LoggerMulti *>( mLogger.get() );
	if( loggerMulti ) {
		return loggerMulti->findType<LoggerT>();
	}
	else {
		return dynamic_cast<LoggerT *>( mLogger.get() );
	}
}

} } // namespace cinder::log

// ----------------------------------------------------------------------------------
// Logging macros

#define CINDER_LOG_STREAM( level, stream ) ::cinder::log::Entry( level, ::cinder::log::Location( CINDER_CURRENT_FUNCTION, __FILE__, __LINE__ ) ) << stream

// CI_MIN_LOG_LEVEL is designed so that if you set it to 7 : nothing logs, 6 : only fatal, 5 : fatal + error, ..., 1 : everything

#if ! defined( CI_MIN_LOG_LEVEL )
	#if ! defined( NDEBUG )
		#define CI_MIN_LOG_LEVEL 0	// debug mode default is LEVEL_VERBOSE
	#else
		#define CI_MIN_LOG_LEVEL 2	// release mode default is LEVEL_INFO
	#endif
#endif

#if( CI_MIN_LOG_LEVEL <= 0 )
	#define CI_LOG_V( stream )	CINDER_LOG_STREAM( ::cinder::log::LEVEL_VERBOSE, stream )
#else
	#define CI_LOG_V( stream )	((void)0)
#endif

#if( CI_MIN_LOG_LEVEL <= 1 )
#define CI_LOG_D( stream )	CINDER_LOG_STREAM( ::cinder::log::LEVEL_DEBUG, stream )
#else
#define CI_LOG_D( stream )	((void)0)
#endif

#if( CI_MIN_LOG_LEVEL <= 2 )
	#define CI_LOG_I( stream )	CINDER_LOG_STREAM( ::cinder::log::LEVEL_INFO, stream )
#else
	#define CI_LOG_I( stream )	((void)0)
#endif

#if( CI_MIN_LOG_LEVEL <= 3 )
	#define CI_LOG_W( stream )	CINDER_LOG_STREAM( ::cinder::log::LEVEL_WARNING, stream )
#else
	#define CI_LOG_W( stream )	((void)0)
#endif

#if( CI_MIN_LOG_LEVEL <= 4 )
	#define CI_LOG_E( stream )	CINDER_LOG_STREAM( ::cinder::log::LEVEL_ERROR, stream )
#else
	#define CI_LOG_E( stream )	((void)0)
#endif

#if( CI_MIN_LOG_LEVEL <= 5 )
	#define CI_LOG_F( stream )	CINDER_LOG_STREAM( ::cinder::log::LEVEL_FATAL, stream )
#else
	#define CI_LOG_F( stream )	((void)0)
#endif

//! Debug macro to simplify logging an exception, which also prints the exception type
#define CI_LOG_EXCEPTION( str, exc )	\
{										\
	CI_LOG_E( str << ", exception type: " << cinder::System::demangleTypeName( typeid( exc ).name() ) << ", what: " << exc.what() );	\
}

