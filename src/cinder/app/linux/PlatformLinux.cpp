/*
 Copyright (c) 2012, The Cinder Project, All rights reserved.

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

#include "cinder/app/linux/PlatformLinux.h"
#include "cinder/ImageSourceFileRadiance.h"
#include "cinder/ImageSourceFileStbImage.h"
#include "cinder/ImageTargetFileStbImage.h"
#include "cinder/Utilities.h"

#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <pwd.h>

namespace cinder {

extern void FontManager_destroyStaticInstance();

} // namespace cinder

namespace cinder { namespace app {

namespace {

const size_t RESOURCE_SEARCH_DEPTH = 10;

} // anonymous namespace

PlatformLinux::PlatformLinux()
{
	ImageSourceFileRadiance::registerSelf();
	ImageSourceFileStbImage::registerSelf();
	ImageTargetFileStbImage::registerSelf();
}

PlatformLinux::~PlatformLinux()
{
}

PlatformLinux* PlatformLinux::get() 
{ 
	return reinterpret_cast<PlatformLinux*>( Platform::get() ); 
}

void PlatformLinux::cleanupLaunch()
{
	cinder::FontManager_destroyStaticInstance();
}

DataSourceRef PlatformLinux::loadResource( const fs::path &resourcePath ) 
{
	fs::path fullPath = getResourcePath( resourcePath );
	if( fullPath.empty() )
		throw ResourceLoadExc( std::string( "Could not resolve absolute path for: " ) + resourcePath.string() );
	else
		return DataSourcePath::create( fullPath );	
}

fs::path PlatformLinux::getResourceDirectory() const 
{
	// @TODO: Implement
	return fs::path();	
}

fs::path PlatformLinux::getResourcePath( const fs::path &rsrcRelativePath ) const
{
	if( ! mResourceDirsInitialized ) {
		mResourceDirsInitialized = true;
		// First search the local directory, then its parent, up to ASSET_SEARCH_DEPTH levels up
		// check at least the app path, even if it has no parent directory.
		auto execPath = getExecutablePath();
		size_t parentCt = 0;
		for( fs::path curPath = execPath; curPath.has_parent_path() || ( curPath == execPath ); curPath = curPath.parent_path(), ++parentCt ) {
			if( parentCt >= RESOURCE_SEARCH_DEPTH ) {
				break;
			}
			const fs::path curResourceDir = curPath / fs::path( "resources" );
			if( fs::exists( curResourceDir ) && fs::is_directory( curResourceDir ) ) {
				auto it = std::find( mResourceDirectories.begin(), mResourceDirectories.end(), curResourceDir );
				if( it == mResourceDirectories.end() ) {
					mResourceDirectories.push_back( curResourceDir );
				}
				break;
			}
		}

		// Next add the executable's directory in case the resource is specified with
		// a path that doesn't contain a 'resources' directory.
		mResourceDirectories.push_back( execPath.parent_path() );

		// Finally add the current working directory for the same reason as above.
		mResourceDirectories.push_back( fs::current_path() );
	}

	for( const auto &directory : mResourceDirectories ) {
		auto fullPath = directory / rsrcRelativePath;
		if( fs::exists( fullPath ) )
			return fullPath;
	}

	return fs::path(); // empty implies failure	
}

struct DialogHelper {
	// Zenkity and KDialog for now...maybe more in the future.
	enum ExecName { UNKNOWN, ZENITY, KDIALOG };

	ExecName	execName = DialogHelper::UNKNOWN;
	std::string	execPath;

	bool isUnknown() const { return DialogHelper::UNKNOWN == execName; }
	bool isZenity() const { return DialogHelper::ZENITY == execName; }
	bool isKDialog() const { return DialogHelper::KDIALOG == execName; }

	static DialogHelper get() {
		DialogHelper result;

		// Zenity gets priority
		if( fs::exists( "/usr/bin/zenity" ) ) {
			result.execName = DialogHelper::ZENITY;
			result.execPath = "/usr/bin/zenity";
		}
		else if( fs::exists( "/usr/bin/kdialog" ) ) {
			result.execName = DialogHelper::KDIALOG;
			result.execPath = "/usr/bin/kdialog";
		}

		return result;
	}

	static fs::path execCmd( const std::vector<std::string>& args ) {
		fs::path result;

		std::stringstream ss;
		for( size_t i = 0; i < args.size(); ++i ) {
			ss << args[i];
			if( i < ( args.size() - 1 ) ) {
				ss << " ";
			}
		}
		
		std::string cmd = ss.str();
		if( ! cmd.empty() ) {
			FILE* pipe = popen( cmd.c_str(), "r" );
			if( pipe ) {
				std::vector<char> buffer( 512 );
				std::string value;
				while( ! feof( pipe ) ) {
					std::memset( static_cast<void*>( buffer.data() ), 0, buffer.size() );
					if( nullptr != fgets( buffer.data(), buffer.size(), pipe ) )  {
						value += static_cast<const char *>( buffer.data() );
					}
				}
				if( ! value.empty() ) {
					// Zenity seems to add a new line character at the end of the path, so remove it if present.
					const auto newLineCharacterPos = std::strcspn( value.c_str(), "\n" );

					if( newLineCharacterPos !=  value.size() ) {
						value[ newLineCharacterPos ] = 0;
					}

					result = value;
				}
			}
			pclose(pipe);			
		}

		return result;
	}

	static std::string quote( const std::string& s ) {
		return "\"" + s + "\"";
	}

	static fs::path getOpenFilePath( const fs::path &initialPath, const std::vector<std::string> &extensions ) {
		fs::path result;
		auto dh = DialogHelper::get();
		if( ! dh.isUnknown() ) {
			std::vector<std::string> args;

			args.push_back( dh.execPath );
			if( dh.isZenity() ) {
				args.push_back( "--file-selection" );

				args.push_back( "--filename");
				args.push_back( quote( initialPath.string() ) );

				if( ! extensions.empty() ) {
					args.push_back( "--file-filter" );
					std::string allExts;
					for( size_t i = 0; i < extensions.size(); ++i ) {
						allExts += ( "*." + extensions[i] );
						if( i < ( extensions.size() - 1 ) ) {
							allExts += " ";
						}
					}
					args.push_back( quote( allExts ) );
				}
			}
			else if( dh.isKDialog() ) {
				args.push_back( "--getopenfilename" );
				// KDialog requires a starting dir
				args.push_back( quote( initialPath.string() ) );

				if( ! extensions.empty() ) {
					std::string allExts;
					for( size_t i = 0; i < extensions.size(); ++i ) {
						allExts += ( "*." + extensions[i] );
						if( i < ( extensions.size() - 1 ) ) {
							allExts += " ";
						}
					}
					args.push_back( quote( allExts ) );
				}
			}

			result = DialogHelper::execCmd( args );
		}
		return result;
	}

	static fs::path getFolderPath( const fs::path &initialPath ) {
		fs::path result;
		auto dh = DialogHelper::get();
		if( ! dh.isUnknown() ) {
			std::vector<std::string> args;

			args.push_back( dh.execPath );
			if( dh.isZenity() ) {
				args.push_back( "--file-selection" );

				args.push_back( "--directory" );

				args.push_back( "--filename");
				args.push_back( quote( initialPath.string() ) );
			}
			else if( dh.isKDialog() ) {
				args.push_back( "--getexistingdirectory" );
				// KDialog requires a starting dir
				args.push_back( quote( initialPath.string() ) );
			}

			result = DialogHelper::execCmd( args );
		}
		return result;
	}

	static fs::path getSaveFilePath( const fs::path &initialPath, const std::vector<std::string> &extensions ) {
		fs::path result;
		auto dh = DialogHelper::get();
		if( ! dh.isUnknown() ) {
			std::vector<std::string> args;

			args.push_back( dh.execPath );
			if( dh.isZenity() ) {
				args.push_back( "--file-selection" );
				args.push_back( "--save" );
				args.push_back( "--confirm-overwrite" );

				args.push_back( "--filename");
				args.push_back( quote( initialPath.string() ) );

				if( ! extensions.empty() ) {
					std::string allExts;
					for( size_t i = 0; i < extensions.size(); ++i ) {
						allExts += ( "*." + extensions[i] );
						if( i < ( extensions.size() - 1 ) ) {
							allExts += " ";
						}
					}
					args.push_back( quote( allExts ) );
				}
			}
			else if( dh.isKDialog() ) {
				args.push_back( "--getsavefilename" );
				// KDialog requires a starting dir
				args.push_back( quote( initialPath.string() ) );

				if( ! extensions.empty() ) {
					args.push_back( "--file-filter" );
					std::string allExts;
					for( size_t i = 0; i < extensions.size(); ++i ) {
						allExts += ( "*." + extensions[i] );
						if( i < ( extensions.size() - 1 ) ) {
							allExts += " ";
						}
					}
					args.push_back( quote( allExts ) );
				}
			}

			result = DialogHelper::execCmd( args );
		}
		return result;
	}
};


fs::path PlatformLinux::getOpenFilePath( const fs::path &initialPath, const std::vector<std::string> &extensions ) 
{
	return DialogHelper::getOpenFilePath( initialPath, extensions );
}

fs::path PlatformLinux::getFolderPath( const fs::path &initialPath ) 
{
	return DialogHelper::getFolderPath( initialPath );
}

fs::path PlatformLinux::getSaveFilePath( const fs::path &initialPath, const std::vector<std::string> &extensions ) 
{
	return DialogHelper::getSaveFilePath( initialPath, extensions );
}

std::map<std::string, std::string> PlatformLinux::getEnvironmentVariables() 
{
	std::map<std::string, std::string> result;

	char **envVars = environ;
	while( nullptr != *envVars ) {
		std::string var = std::string( *envVars );
		std::vector<std::string> kv = ci::split( var, '=' );
		if( 2 == kv.size() ) {
			result[kv[0]] = kv[1];
		}
		++envVars;
	}

	return result;
}

fs::path PlatformLinux::expandPath( const fs::path &path ) 
{
	fs::path filename = path.filename();

	char actualPath[PATH_MAX];
	if( ::realpath( path.parent_path().c_str(), actualPath ) ) { 
		fs::path expandedPath = fs::path( std::string( actualPath ) );
		expandedPath /= filename;
		return expandedPath;	
	}   

	return fs::path();  
}

fs::path PlatformLinux::getHomeDirectory() const 
{
	fs::path result;

	const char *homeDir = getenv( "HOME" );
	if( nullptr == homeDir ) {
		long int len = sysconf( _SC_GETPW_R_SIZE_MAX );
		if( -1 == len ) {
			len = 1024;
		}
		std::vector<char> buf( len );

		struct passwd pwd = {};
		struct passwd *pwdPtr = nullptr;
		int error = 0;

		while( ERANGE == ( error = getpwuid_r( getuid(), &pwd, buf.data(), buf.size(), &pwdPtr ) ) ) {
			buf.resize( 2*buf.size() );
			// Bail if the size becomes too big
			if( buf.size() >= 65536 ) {
				error = ERANGE;
				break;
			}
		}

		if( 0 == error ) {
			result = fs::path( pwd.pw_dir );
		}
	}
	else {
		result = fs::path( homeDir );
	}

	return result;
}

fs::path PlatformLinux::getDocumentsDirectory() const 
{
	fs::path result;

	auto homeDir = getHomeDirectory();
	if( ! homeDir.empty() ) {
		auto docsDir = homeDir / "Documents";
		if( fs::exists( docsDir ) && fs::is_directory( docsDir ) ) {
			result = docsDir;
		}
	}

	return result;
}

fs::path PlatformLinux::getDefaultExecutablePath() const 
{
	std::vector<char> buf( PATH_MAX );
	std::memset( &(buf[0]), 0, buf.size()  );
    ssize_t len = ::readlink("/proc/self/exe", &(buf[0]), buf.size() - 1 );
    if( ( -1 != len ) && ( len < buf.size() ) ) {
      buf[len] = '\0';
    }
 	return fs::path( std::string( &(buf[0]), len ) ).parent_path();
}

void PlatformLinux::sleep( float milliseconds ) 
{
	unsigned long sleepMicroSecs = milliseconds*1000L;
	usleep( sleepMicroSecs );	
}

void PlatformLinux::launchWebBrowser( const Url &url ) 
{
	pid_t pid = fork();

	if( 0 == pid ) {
		// 0 means we're in the child preocess

		const std::string exe = "/usr/bin/xdg-open";
		std::vector<char*> args;
		args.push_back( const_cast<char*>( exe.c_str() ) );
		args.push_back( const_cast<char*>( url.c_str() ) );
		args.push_back( nullptr );

		execvp( args[0], args.data() );

		_exit( 1 );
	}
}

std::vector<std::string> PlatformLinux::stackTrace() 
{
	// @TODO: Implement
	return std::vector<std::string>();	
}

const std::vector<DisplayRef>& PlatformLinux::getDisplays()
{
	if( ! mDisplaysInitialized ) {
		// @TODO: Implement fuller

		mDisplaysInitialized = true;
	}

	return mDisplays;
}

}} // namespace cinder::app
