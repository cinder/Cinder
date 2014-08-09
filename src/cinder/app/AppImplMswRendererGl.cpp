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

#if ! defined( CINDER_GL_ANGLE )
#include "cinder/app/AppImplMswRendererGl.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Environment.h"
#include "glload/wgl_all.h"
#include "glload/wgl_load.h"
#include "cinder/app/App.h"
#include "cinder/Camera.h"
#include "cinder/Log.h"
#include <windowsx.h>

namespace cinder { namespace app {

bool sMultisampleSupported = false;
int sArbMultisampleFormat;
typedef HGLRC (__stdcall * PFNWGLCREATECONTEXTATTRIBSARB) (HDC hDC, HGLRC hShareContext, const int *attribList);


AppImplMswRendererGl::AppImplMswRendererGl( App *aApp, RendererGl *aRenderer )
	: AppImplMswRenderer( aApp ), mRenderer( aRenderer )
{
	mPrevRC = 0;
	mRC = 0;
}

void AppImplMswRendererGl::prepareToggleFullScreen()
{
	mPrevRC = mRC;
	mWasVerticalSynced = gl::isVerticalSyncEnabled();
}

void AppImplMswRendererGl::finishToggleFullScreen()
{
	if( mPrevRC ) {
		::wglDeleteContext( mPrevRC );
	}

	gl::enableVerticalSync( mWasVerticalSynced );
}

void AppImplMswRendererGl::defaultResize() const
{
	::RECT clientRect;
	::GetClientRect( mWnd, &clientRect );
	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;

	gl::viewport( 0, 0, width, height );
	gl::setMatricesWindow( width, height );
}

void AppImplMswRendererGl::swapBuffers() const
{
	::SwapBuffers( mDC );
}

void AppImplMswRendererGl::makeCurrentContext()
{
	mCinderContext->makeCurrent();
}

HWND createDummyWindow( int *width, int *height, bool fullscreen )
{
	DWORD windowExStyle, windowStyle;
	HWND wnd;
	const char *title = "flint";
	int bits = 32;

	if( *width <= 0 ) {
		*width = ::GetSystemMetrics( SM_CXSCREEN );
		*height = ::GetSystemMetrics( SM_CYSCREEN );
	}

	WNDCLASS	wc;						// Windows Class Structure
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = 0L;
	WindowRect.right = (long)*width;
	WindowRect.top = 0L;
	WindowRect.bottom = (long)*height;

	HINSTANCE instance	= ::GetModuleHandle( NULL );				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= DefWindowProc;						// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= instance;
	wc.hIcon			= ::LoadIcon( NULL, IDI_WINLOGO );		// Load The Default Icon
	wc.hCursor			= ::LoadCursor( NULL, IDC_ARROW );		// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= TEXT("FLINTDUMMY");

	if( ! ::RegisterClass( &wc ) ) {								// Attempt To Register The Window Class
		DWORD err = ::GetLastError();
		return 0;											
	}

	if( fullscreen ) {
		windowExStyle = WS_EX_APPWINDOW;
		windowStyle = WS_POPUP;
		::ShowCursor( TRUE );
	}
	else {
		windowExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		windowStyle = ( WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME );
	}

	::AdjustWindowRectEx( &WindowRect, windowStyle, FALSE, windowExStyle );		// Adjust Window To True Requested Size

	wchar_t unicodeTitle[1024]; 
	wsprintfW( unicodeTitle, L"%S", title );

	// Create The Window
	if( ! ( wnd = ::CreateWindowEx( windowExStyle,						// Extended Style For The Window
		TEXT("FLINTDUMMY"),
		unicodeTitle,						// Window Title
		windowStyle,					// Required Window Style
		0, 0,								// Window Position
		WindowRect.right-WindowRect.left,	// Calculate Window Width
		WindowRect.bottom-WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		instance,							// Instance
		0 )) )

	{
		return 0;								
	}

	return wnd;
}

bool AppImplMswRendererGl::initialize( HWND wnd, HDC dc, RendererRef sharedRenderer )
{
	if( ( ! sMultisampleSupported ) && mRenderer->getOptions().getAntiAliasing() ) {
		// first create a dummy window and use it to determine if we can do antialiasing
		int width = 640;
		int height = 480;
		HWND dummyWnd = createDummyWindow( &width, &height, false );
		HDC dummyDC = ::GetDC( dummyWnd );
		
		bool result = initializeInternal( dummyWnd, dummyDC, NULL );
		
		::ReleaseDC( dummyWnd, dummyDC );
		::DestroyWindow( dummyWnd );
		::UnregisterClass( TEXT("FLINTDUMMY"), ::GetModuleHandle( NULL ) );
		
		if( ! result )
			return false;
		// now do it again but with newly created multisample settings
	}
	
	RendererGl *sharedRendererGl = dynamic_cast<RendererGl*>( sharedRenderer.get() );
	HGLRC sharedRC = ( sharedRenderer ) ? sharedRendererGl->mImpl->mRC : NULL;

	if( ! initializeInternal( wnd, dc, sharedRC ) ) {
		return false;
	}

	gl::Environment::setCore();
	auto platformData = std::shared_ptr<gl::Context::PlatformData>( new gl::PlatformDataMsw( mRC, mDC ) );
	platformData->mDebug = mRenderer->getOptions().getDebug();
	platformData->mDebugLogSeverity = mRenderer->getOptions().getDebugLogSeverity();
	platformData->mDebugBreakSeverity = mRenderer->getOptions().getDebugBreakSeverity();
	platformData->mObjectTracking = mRenderer->getOptions().getObjectTracking();
	mCinderContext = gl::Context::createFromExisting( platformData );
	mCinderContext->makeCurrent();

	return true;
}

// We can't use the normal mechanism for this test because we don't have a context yet
namespace {
bool getCreateContextAttribsPtr( HDC dc, PFNWGLCREATECONTEXTATTRIBSARB *resultFnPtr )
{
	static PFNWGLCREATECONTEXTATTRIBSARB cachedFnPtr = NULL;
	if( ! cachedFnPtr ) {
		auto temp = ::wglCreateContext( dc ); 
		::wglMakeCurrent( dc, temp ); 

		cachedFnPtr = (PFNWGLCREATECONTEXTATTRIBSARB)::wglGetProcAddress( "wglCreateContextAttribsARB" );
		*resultFnPtr = cachedFnPtr;
		::wglMakeCurrent( NULL, NULL );
		::wglDeleteContext( temp );
		if( cachedFnPtr == NULL ) { 
			return false;
		}
		else
			return true;
	}
	else {
		*resultFnPtr = cachedFnPtr;
		return cachedFnPtr != NULL;
	}
}

HGLRC createContext( HDC dc, bool coreProfile, bool debug, int majorVersion, int minorVersion )
{
	HGLRC result = 0;
	static bool initializedLoadOGL = false;

	bool needsCreateContextAttribsARB = false;
	if( coreProfile || majorVersion > 2 )
		needsCreateContextAttribsARB = true;

	PFNWGLCREATECONTEXTATTRIBSARB wglCreateContextAttribsARBPtr = NULL;
	if( needsCreateContextAttribsARB && getCreateContextAttribsPtr( dc, &wglCreateContextAttribsARBPtr ) ) {
		int attribList[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, majorVersion,
			WGL_CONTEXT_MINOR_VERSION_ARB, minorVersion,
			WGL_CONTEXT_FLAGS_ARB, (debug) ? WGL_CONTEXT_DEBUG_BIT_ARB : 0,
			WGL_CONTEXT_PROFILE_MASK_ARB, (coreProfile) ? WGL_CONTEXT_CORE_PROFILE_BIT_ARB : WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
			0, 0
		};
 
		result = (*wglCreateContextAttribsARBPtr)( dc, 0, attribList );
		return result;
	}
	else {
		return ::wglCreateContext( dc );
	}
}
} // anonymous namespace

bool AppImplMswRendererGl::initializeInternal( HWND wnd, HDC dc, HGLRC sharedRC )
{
	int pixelFormat;
	mWnd = wnd;
	mDC = dc;

	BYTE stencilBits = mRenderer->getOptions().getStencil() ? 8 : 0;
	BYTE depthBits = mRenderer->getOptions().getDepthBufferDepth();

	static PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		32,											// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		depthBits,									// 32Bit Z-Buffer (Depth Buffer)  
		stencilBits,								// Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	/*
	Our first pass, Multisampling hasn't been created yet, so we create a window normally
	If it is supported, then we're on our second pass
	that means we want to use our pixel format for sampling
	so set PixelFormat to arbMultiSampleformat instead
	*/
	if( ! sMultisampleSupported ) {
		pixelFormat = ::ChoosePixelFormat( dc, &pfd );				// Find A Compatible Pixel Format
		if( pixelFormat == 0 ) {												// Did We Find A Compatible Format?
			return false;										
		}
	}
	else {
		pixelFormat = sArbMultisampleFormat;
	}

	if( ! ::SetPixelFormat( dc, pixelFormat, &pfd ) ) {		// Are We Able To Set The Pixel Format?
		return false;								
	}

	if( ! ( mRC = createContext( dc, mRenderer->getOptions().getDebug(), mRenderer->getOptions().getCoreProfile(),
			mRenderer->getOptions().getVersion().first, mRenderer->getOptions().getVersion().second ) ) )	{			// Are We Able To Get A Rendering Context?
		return false;								
	}

	if( ! ::wglMakeCurrent( dc, mRC ) ){					// Try To Activate The Rendering Context
		return false;								
	}

	gl::Environment::setCore();
	gl::env()->initializeFunctionPointers();

	wgl_LoadFunctions( dc );								// Initialize WGL function pointers

	if( ( ! sMultisampleSupported ) && ( mRenderer->getOptions().getAntiAliasing() > RendererGl::AA_NONE ) )  {
		int level = initMultisample( pfd, mRenderer->getOptions().getAntiAliasing(), dc );
		if( level > 0 ) {
			// kill the current context and relaunch
			::wglMakeCurrent( NULL, NULL );
			::wglDeleteContext( mRC );
			return true; // our caller will run us again
		}
	}

	::wglMakeCurrent( NULL, NULL );
	if( mPrevRC ) {
		if( ! ::wglCopyContext( mPrevRC, mRC, 0xFFFFFFFF /*GL_ALL_ATTRIB_BITS*/ ) )
			CI_LOG_E( "Unable to copy GL context attributes." );
	}

	if( mPrevRC )
		::wglShareLists( mPrevRC, mRC );
	
	if( sharedRC )
		::wglShareLists( sharedRC, mRC );

	::wglMakeCurrent( mDC, mRC );

	return true;									// Success
}

int AppImplMswRendererGl::initMultisample( PIXELFORMATDESCRIPTOR pfd, int requestedLevelIdx, HDC dc )
{
	// this is an array that corresponds to AppSettings::AA_NONE through AA_MSAA_16
	if( ( ! wglext_ARB_multisample ) || ( ! wglChoosePixelFormatARB ) ) {
		sMultisampleSupported = false;
		return 0;
	}

	// Get Our Current Device Context
//	mDC = ::GetDC( mWnd );
	int	pixelFormat;
	int	valid;
	UINT numFormats;
	float fAttributes[] = {0,0};

	// These Attributes Are The Bits We Want To Test For In Our Sample
	// Everything Is Pretty Standard, The Only One We Want To 
	// Really Focus On Is The SAMPLE BUFFERS ARB And WGL SAMPLES
	// These Two Are Going To Do The Main Testing For Whether Or Not
	// We Support Multisampling On This Hardware.
	int iAttributes[] =
	{
		WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
		WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB,24,
		WGL_ALPHA_BITS_ARB,8,
		WGL_DEPTH_BITS_ARB,16,
		WGL_STENCIL_BITS_ARB,0,
		WGL_DOUBLE_BUFFER_ARB,GL_TRUE,
		WGL_SAMPLE_BUFFERS_ARB,GL_TRUE,
		WGL_SAMPLES_ARB, 0,
		0,0
	};

	// iterate down the levels until one sticks
	for( int levelIdx = requestedLevelIdx; levelIdx > RendererGl::AA_NONE; --levelIdx ) {
		iAttributes[19] = RendererGl::sAntiAliasingSamples[levelIdx];

		// First We Check To See If We Can Get A Pixel Format For 4 Samples
		valid = ::wglChoosePixelFormatARB( dc, iAttributes, fAttributes, 1, &pixelFormat, &numFormats );
		// If We Returned True, And Our Format Count Is Greater Than 1
		if( valid && ( numFormats >= 1 ) ) {
			sMultisampleSupported = true;
			sArbMultisampleFormat = pixelFormat;	
			return levelIdx;
		}
	}

	// Return failure if we hit level 0
	return 0;
}

void AppImplMswRendererGl::kill()
{
	if( mRC ) {											// Do We Have A Rendering Context?
		::wglMakeCurrent( NULL, NULL );					// release The DC And RC Contexts
		::wglDeleteContext( mRC );						// delete The RC
	}

	mRC = 0;
}

} } // namespace cinder::app
#endif // ! defined( CINDER_GL_ANGLE )
