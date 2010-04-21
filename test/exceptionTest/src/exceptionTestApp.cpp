#include "flint/app/AppBasic.h"
using namespace fli;
using namespace fli::app;

#if defined( FLI_MAC )
#	include <cxxabi.h>
#	include <execinfo.h>
#else
#	include "StackWalker.h"
#endif

class exceptionTestApp : public AppBasic {
 public:
	void	setup();
};

#if defined( FLI_MSW )
class CinderStackWalker : public StackWalker {
  public:
	CinderStackWalker()
		: StackWalker()
	{
		ShowCallstack();
	}
	virtual void OnSymInit(LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName) {}
	virtual void OnLoadModule(LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size, DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion) {}
	virtual void OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry)
	{
		CHAR buffer[STACKWALK_MAX_NAMELEN];
		if ( (eType != lastEntry) && (entry.offset != 0) && ( eType != firstEntry ) ) {
			if (entry.name[0] == 0)
				strcpy_s(entry.name, "(function-name not available)");
			if (entry.undName[0] != 0)
				strcpy_s(entry.name, entry.undName);
			if (entry.undFullName[0] != 0)
				strcpy_s(entry.name, entry.undFullName);
			if (entry.lineFileName[0] == 0) {
				strcpy_s(entry.lineFileName, "(filename not available)");
				if (entry.moduleName[0] == 0)
					strcpy_s(entry.moduleName, "(module-name not available)");
				_snprintf_s(buffer, STACKWALK_MAX_NAMELEN, "%p (%s): %s: %s", (LPVOID) entry.offset, entry.moduleName, entry.lineFileName, entry.name);
			}
			else
				_snprintf_s(buffer, STACKWALK_MAX_NAMELEN, "%s (%d): %s", entry.lineFileName, entry.lineNumber, entry.name);
			mEntries.push_back( std::string( buffer ) );
		}

	}
	virtual void OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr) {}
	virtual void OnOutput(LPCSTR szText) {}
	
	const std::vector<std::string>&	getEntries() { return mEntries; }
	
  protected:
	std::vector<std::string>	mEntries;
};
#endif

std::vector<std::string> stackTrace()
{
#if defined( FLI_MSW )
	CinderStackWalker csw;
	return csw.getEntries();
#else
	std::vector<std::string> result;
	static const int MAX_DEPTH = 128;
	void* callstack[MAX_DEPTH];
	int i, frames = backtrace( callstack, MAX_DEPTH );
	char** strs = backtrace_symbols( callstack, frames );
	for( i = 0; i < frames; ++i ) {
		// demangle any C++ names
		std::string mangled( strs[i] );
		// find the beginning and the end of the useful part of the trace
		std::string::size_type end = mangled.find_last_of( '+' ) - 1;
		std::string::size_type begin = mangled.rfind( ' ', end - 1 ) + 1;

		// if they were found, we'll go ahead and demangle
		if( ( begin != std::string::npos ) && ( end != std::string::npos ) ) {
			mangled = mangled.substr( begin, end - begin );

			size_t MAX_NAME = 1024;
			int demangleStatus;
			std::string name;
			char* demangledName = (char*)malloc( MAX_NAME );
			if( ( demangledName = abi::__cxa_demangle( mangled.c_str(), demangledName, &MAX_NAME, &demangleStatus ) ) && ( demangleStatus == 0 ) ) {
				name = demangledName; // the demangled name is now in our trace string
			}
			else
				name = mangled;
			free( demangledName );
			result.push_back( std::string( strs[i] ).substr( 0, begin ) + name + std::string( strs[i] ).substr( end, std::string::npos ) );
		}
		else
			result.push_back( std::string( strs[i] ) );
	}
	
	free( strs );
	
	return result;
#endif
}

class Exception : public std::exception {
  public:
	Exception() : std::exception(), mStackTrace( stackTrace() )
	{}		
	virtual ~Exception() throw() {}
	
	const std::vector<std::string>&		getStackTrace() const { return mStackTrace; }
	
  protected:
	std::vector<std::string>	mStackTrace;
};

class UnspecifiedException : public Exception {
  public:
	UnspecifiedException() : Exception() {}
	
	virtual const char *what() { return "Unspecified."; }
};

void exceptionTestApp::setup()
{
	try {
		// Program body
		throw UnspecifiedException();
	}
	catch( Exception &ce ) {
		console() << "Uncaught Cinder Exception" << std::endl;
		console() << " Exception: " << ce.what() << std::endl;
		console() << " Stack Trace: " << std::endl;
		const std::vector<std::string> &strackTrace( ce.getStackTrace() );
		for( std::vector<std::string>::const_iterator lineIter = strackTrace.begin(); lineIter != strackTrace.end(); ++lineIter ) {
			console() << "  " << *lineIter << std::endl;
		}
	}
	catch( std::exception &e ) {
		console() << "Uncaught std::exception" << std::endl;
		console() << " Exception: " << e.what() << std::endl;
	}
	catch( ... ) {
		console() << "Uncaught unknown exception" << std::endl;
	};
}

// This line tells Flint to actually create the application
FLI_APP_BASIC( exceptionTestApp, RendererGL )
