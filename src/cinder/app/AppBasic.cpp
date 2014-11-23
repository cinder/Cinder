/*
 Copyright (c) 2012, The Cinder Project, All rights reserved.
 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

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

#include "cinder/app/AppBasic.h"
#include "cinder/app/Renderer.h"
#include "cinder/Unicode.h"

namespace cinder { namespace app {

// Global singleton
AppBasic*	AppBasic::sInstance;

AppBasic::AppBasic()
	: App()
{
}

AppBasic::~AppBasic()
{
}

#if defined( CINDER_MSW )
void AppBasic::executeLaunch( AppBasic *app, RendererRef renderer, const char *title )
{
	sInstance = app;

	// MSW sends it arguments as widestrings, so we'll convert them to utf8 array and pass that
	LPWSTR *szArglist;
	int nArgs;

	szArglist = ::CommandLineToArgvW( ::GetCommandLineW(), &nArgs );
	if( szArglist && nArgs ) {
		std::vector<std::string> utf8Args;
		char **utf8ArgPointers = (char **)malloc( sizeof(char*) * nArgs );
		for( int i = 0; i < nArgs; ++i )
			utf8Args.push_back( toUtf8( (char16_t*)szArglist[i] ) );
		for( int i = 0; i < nArgs; ++i )
			utf8ArgPointers[i] = const_cast<char *>( utf8Args[i].c_str() );
		App::executeLaunch( app, renderer, title, nArgs, utf8ArgPointers );
		free( utf8ArgPointers );
	}
	else	
		App::executeLaunch( app, renderer, title, 0, NULL );

	// Free memory allocated for CommandLineToArgvW arguments.
	::LocalFree( szArglist );
}
#elif defined( CINDER_WINRT )
void AppBasic::executeLaunch( AppBasic *app, RendererRef renderer, const char *title )
{
	sInstance = app;
	App::executeLaunch( app, renderer, title, 0, NULL );
}
#endif

void AppBasic::restoreWindowContext()
{
	getWindow()->getRenderer()->makeCurrentContext();
}

bool AppBasic::privateShouldQuit()
{
	return mSignalShouldQuit();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// AppBasic::Settings
AppBasic::Settings::Settings()
	: App::Settings(), mQuitOnLastWindowClose( true )
{
#if defined( CINDER_MSW )
	mEnableMswConsole = false;
#endif
}

void AppBasic::Settings::setShouldQuit( bool shouldQuit )
{
	mShouldQuit = shouldQuit;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// AppBasic::Window

} } // namespace cinder::app