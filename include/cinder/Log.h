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
#include "cinder/Noncopyable.h"
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

struct CI_API Location {
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

struct CI_API Metadata {

	std::string toString() const;

	Level		mLevel;
	Location	mLocation;
};

CI_API extern std::ostream& operator<<( std::ostream &os, const Location &rhs );
CI_API extern std::ostream& operator<<( std::ostream &lhs, const Level &rhs );

//! Logger is the base class all logging objects are derived from.
//!
//! \see LoggerConsole, LoggerFile, LoggerFileRotating
class CI_API Logger : private Noncopyable {
  public:
	virtual ~Logger()	{}

	virtual void write( const Metadata &meta, const std::string &text ) = 0;

	void setTimestampEnabled( bool enable = true )	{ mTimeStampEnabled = enable; }
	bool isTimestampEnabled() const					{ return mTimeStampEnabled; }
	
  protected:
	Logger() : mTimeStampEnabled( false ) {}

	void writeDefault( std::ostream &stream, const Metadata &meta, const std::string &text );

  private:
	bool mTimeStampEnabled;
};
	
typedef std::shared_ptr<Logger>	LoggerRef;

//! LoggerConsole prints log messages in the application console window.
class CI_API LoggerConsole : public Logger {
  public:
	void write( const Metadata &meta, const std::string &text ) override;
};

//! \brief LoggerFile will write log messages to a specified file.
//!
//! LoggerFile will write to a specified file, either appending to or overwriting that file at application startup.
class CI_API LoggerFile : public Logger {
  public:
	//! LoggerFile writes to a single log file.  File appending is configurable.
	//! If \p filePath is empty, uses the default ('%cinder.log' next to app binary)
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

//! LoggerFileRotating will write log messages to a file that is rotated at midnight.
class CI_API LoggerFileRotating : public LoggerFile {
public:
	
	//! Creates a rotating log file that will rotate when the first logging event occurs after midnight.
	//! \p formatStr will be passed to strftime to determine the file name.
	LoggerFileRotating( const fs::path &folder, const std::string &formatStr, bool appendToExisting = true );
	
	virtual ~LoggerFileRotating() { }
	
	void write( const Metadata &meta, const std::string &text ) override;
	
protected:
	fs::path		mFolderPath;
	std::string		mDailyFormatStr;
	int				mYearDay;
};
	
//! LoggerBreakpoint doesn't actually print anything, but triggers a breakpoint on log events above a specified threshold.
class CI_API LoggerBreakpoint : public Logger {
  public:
	LoggerBreakpoint( Level triggerLevel = LEVEL_ERROR )
		: mTriggerLevel( triggerLevel )
	{}

	void write( const Metadata &meta, const std::string &text ) override;

	void	setTriggerLevel( Level triggerLevel )	{ mTriggerLevel = triggerLevel; }
	Level	getTriggerLevel() const					{ return mTriggerLevel; }
	
  private:
	Level	mTriggerLevel;
};

//! LoggerSystem rovides 'system' logging support. Uses syslog on platforms that have it, on MSW uses Windows Event Logging.
//! \note Does nothing on WinRT.
class CI_API LoggerSystem : public Logger {
public:
	LoggerSystem();
	virtual ~LoggerSystem();
	
	void write( const Metadata &meta, const std::string &text ) override;
	//! Sets the minimum logging level that will trigger a system log.
	//! \note Setting \p minLevel below CI_MIN_LOG_LEVEL is pointless; minLevel will act like CI_MIN_LOG_LEVEL.
	void setLoggingLevel( Level minLevel ) { mMinLevel = minLevel; }
	
protected:
	Level mMinLevel;
#if defined( CINDER_COCOA ) || defined( CINDER_LINUX )
	class ImplSysLog;
	std::unique_ptr<ImplSysLog> mImpl;
#elif defined( CINDER_MSW_DESKTOP )
	class ImplEventLog;
	std::unique_ptr<ImplEventLog> mImpl;
#elif defined( CINDER_ANDROID )
	class ImplLogCat;
	std::unique_ptr<ImplLogCat> mImpl;
#endif
};

//! \brief LogManager manages a stack of all active Loggers.
//!
//! LogManager's default state contains a single LoggerConsole.  LogManager allows for adding and removing Loggers via their pointer values.
class CI_API LogManager {
public:
	// Returns a pointer to the shared instance. To enable logging during shutdown, this instance is leaked at shutdown.
	static LogManager* instance()	{ return sInstance; }
	//! Destroys the shared instance. Useful to remove false positives with leak detectors like valgrind.
	static void destroyInstance()	{ delete sInstance; }
	//! Restores LogManager to its default state - a single LoggerConsole.
	void restoreToDefault();

	//! Removes all loggers from the stack.
	void clearLoggers();
	//! Resets the current Logger stack so only \p logger exists.
	void resetLogger( const LoggerRef& logger );
	//! Adds \a logger to the current stack of loggers.
	void addLogger( const LoggerRef& logger );
	//! Remove \a logger to the current stack of loggers.
	void removeLogger( const LoggerRef& logger );
	//! Returns a vector of Loggers of a specifc type.
	template<typename LoggerT>
	std::vector<std::shared_ptr<LoggerT>> getLoggers();
	//! Returns a vector of LoggerRef that contains all active loggers
	std::vector<LoggerRef> getAllLoggers();
	//! Returns the mutex used for thread safe logging.
	std::mutex& getMutex() const			{ return mMutex; }
	
	void write( const Metadata &meta, const std::string &text );
	
	template<typename LoggerT, typename... Args>
	std::shared_ptr<LoggerT> makeLogger( Args&&... args );

	template<typename LoggerT, typename... Args>
	std::shared_ptr<LoggerT> makeOrGetLogger( Args&&... args );
	
protected:
	LogManager();

	std::vector<LoggerRef>			mLoggers;
	
	mutable std::mutex				mMutex;
	
	static LogManager 				*sInstance;
};
	
struct CI_API Entry {
	// TODO: move &&location
	Entry( Level level, const Location &location );
	~Entry();

	template <typename T>
	Entry& operator<<( const T &rhs )
	{
		mHasContent = true;
		mStream << rhs;
		return *this;
	}

	void writeToLog();
	const Metadata&	getMetaData() const	{ return mMetaData; }

private:

	Metadata			mMetaData;
	bool				mHasContent;
	std::stringstream	mStream;
};

// ----------------------------------------------------------------------------------
// Freestanding functions

//! The global manager for logging, used to manipulate the Logger stack. Provides thread safety amongst the Loggers.
CI_API LogManager* manager();

//! Creates and returns a new logger of type LoggerT, adding it to the current Logger stack.
template<typename LoggerT, typename... Args>
std::shared_ptr<LoggerT> makeLogger( Args&&... args )
{
	return manager()->makeLogger<LoggerT>( std::forward<Args>( args )... );
}

//! If a logger of type LoggerT exists, it will return that logger.  Otherwise creates and
//! returns a new logger of type LoggerT, adding it to the current Logger stack.
template<typename LoggerT, typename... Args>
std::shared_ptr<LoggerT> makeOrGetLogger( Args&&... args )
{
	return manager()->makeOrGetLogger<LoggerT>( std::forward<Args>( args )... );
}

// ----------------------------------------------------------------------------------
// Template method implementations

template<typename LoggerT, typename... Args>
std::shared_ptr<LoggerT> LogManager::makeLogger( Args&&... args )
{
	static_assert( std::is_base_of<Logger, LoggerT>::value, "LoggerT must inherit from log::Logger" );
	
	std::shared_ptr<LoggerT> result = std::make_shared<LoggerT>( std::forward<Args>( args )... );
	addLogger( result );
	return result;
}

template<typename LoggerT, typename... Args>
std::shared_ptr<LoggerT> LogManager::makeOrGetLogger( Args&&... args )
{
	static_assert( std::is_base_of<Logger, LoggerT>::value, "LoggerT must inherit from log::Logger" );
	
	auto vector = getLoggers<LoggerT>();
	if( ! vector.empty() ) {
		return vector.front();
	}
	
	std::shared_ptr<LoggerT> result = std::make_shared<LoggerT>( std::forward<Args>( args )... );
	addLogger( result );
	return result;
}
	
template<typename LoggerT>
std::vector<std::shared_ptr<LoggerT>> LogManager::getLoggers()
{
	std::vector<std::shared_ptr<LoggerT>> result;

	std::lock_guard<std::mutex> lock( manager()->getMutex() );
	for( const auto &logger : mLoggers ) {
		auto loggerCasted = std::dynamic_pointer_cast<LoggerT>( logger );
		if( loggerCasted ) {
			result.push_back( loggerCasted );
		}
	}

	return result;
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

