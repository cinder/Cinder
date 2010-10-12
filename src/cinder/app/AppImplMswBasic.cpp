/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

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

#include "cinder/app/AppImplMswBasic.h"
#include "cinder/app/AppBasic.h"
#include "cinder/app/AppImplMswRenderer.h"
#include "cinder/app/Renderer.h"
#include "cinder/Utilities.h"

#include <windowsx.h>
#include <winuser.h>

using std::vector;
using std::string;

#define LOSHORT(l)           ((SHORT)(l))
#define HISHORT(l)           ((SHORT)(((DWORD)(l) >> 16) & 0xFFFF))

namespace cinder { namespace app {

static const wchar_t *WINDOWED_WIN_CLASS_NAME = TEXT("CinderWinClass");
static const wchar_t *FULLSCREEN_WIN_CLASS_NAME = TEXT("CinderWinFSClass");

AppImplMswBasic::AppImplMswBasic( AppBasic *aApp )
	: AppImplMsw( aApp ), mApp( aApp ), mHasBeenInitialized( false )
{
	mShouldQuit = false;
	mIsDragging = false;
}

void AppImplMswBasic::run()
{
	mDisplay = mApp->getSettings().getDisplay();
	if( ! mDisplay )
		mDisplay = cinder::Display::getMainDisplay().get();

	if( mApp->getSettings().isFullScreen() ) {
		mFullScreen = true;
		mWindowWidth = mApp->getSettings().getFullScreenWidth();
		mWindowHeight = mApp->getSettings().getFullScreenHeight();	
	}
	else {
		mFullScreen = false;
		mWindowWidth = mApp->getSettings().getWindowWidth();
		mWindowHeight = mApp->getSettings().getWindowHeight();
	}
	
	mFrameRate = mApp->getSettings().getFrameRate();
	
	createWindow( &mWindowWidth, &mWindowHeight );
	
	mApp->privateSetup__();
	mHasBeenInitialized = true;
	mApp->privateResize__( ResizeEvent( Vec2i( mWindowWidth, mWindowHeight ) ) );

	::ShowWindow( mWnd, SW_SHOW );
	::SetForegroundWindow( mWnd );
	::SetFocus( mWnd );

	// initialize our next frame time to be definitely now
	mNextFrameTime = -1;
	
	MSG msg;
	while( ! mShouldQuit ) {
		while( ::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
			::TranslateMessage( &msg );
			::DispatchMessage( &msg ); 
		}
		double currentSeconds = mApp->getElapsedSeconds();
		if( currentSeconds >= mNextFrameTime ) {
			mNextFrameTime = currentSeconds + 1.0 / mFrameRate;
			mApp->privateUpdate__();
			::RedrawWindow( mWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW );
		}
		else
			::Sleep( 1 );
	}

	killWindow( mFullScreen );
	mApp->privateShutdown__();
	delete mApp;
}

bool AppImplMswBasic::createWindow( int *width, int *height )
{
	int bits = 32;

	if( *width <= 0 ) {
		*width = ::GetSystemMetrics( SM_CXSCREEN );
		*height = ::GetSystemMetrics( SM_CYSCREEN );
	}

	WNDCLASS	wc;						// Windows Class Structure
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	if( mFullScreen ) {
		WindowRect.left = 0L;
		WindowRect.right = (long)*width;
		WindowRect.top = 0L;
		WindowRect.bottom = (long)*height;
	}
	else { // center the window on the display if windowed
		WindowRect.left = ( getDisplay()->getWidth() - *width ) / 2;
		WindowRect.right = ( getDisplay()->getWidth() - *width ) / 2 + *width;
		WindowRect.top = ( getDisplay()->getHeight() - *height ) / 2;
		WindowRect.bottom = ( getDisplay()->getHeight() - *height ) / 2 + *height;
	}

	mInstance			= ::GetModuleHandle( NULL );				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= WndProc;						// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= mInstance;							// Set The Instance
	wc.hIcon			= ::LoadIcon( NULL, IDI_WINLOGO );		// Load The Default Icon
	wc.hCursor			= ::LoadCursor( NULL, IDC_ARROW );		// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= ( mFullScreen ) ? FULLSCREEN_WIN_CLASS_NAME : WINDOWED_WIN_CLASS_NAME;

	if( ! ::RegisterClass( &wc ) ) {								// Attempt To Register The Window Class
		DWORD err = ::GetLastError();
		return false;											
	}

	if( mFullScreen ) {
		DEVMODE dmScreenSettings;								
		memset( &dmScreenSettings, 0, sizeof(dmScreenSettings) );	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize = sizeof( dmScreenSettings );
		dmScreenSettings.dmPelsWidth	= *width;
		dmScreenSettings.dmPelsHeight	= *height;
		dmScreenSettings.dmBitsPerPel	= bits;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_mFullscreen Gets Rid Of Start Bar.
		if( ::ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
			return false;

		mWindowExStyle = WS_EX_APPWINDOW;								// Window Extended Style
		mWindowStyle = WS_POPUP;										// Windows Style
		::ShowCursor( TRUE );										// Hide Mouse Pointer
	}
	else {
		mWindowExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		mWindowStyle = ( mApp->getSettings().isResizable() ) ? WS_OVERLAPPEDWINDOW
			:	( WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME );							// Windows Style
	}

	::AdjustWindowRectEx( &WindowRect, mWindowStyle, FALSE, mWindowExStyle );		// Adjust Window To True Requested Size

	std::wstring unicodeTitle = toUtf16( mApp->getSettings().getTitle() ); 

	// Create The Window
	if( ! ( mWnd = ::CreateWindowEx( mWindowExStyle,						// Extended Style For The Window
		( mFullScreen ) ? FULLSCREEN_WIN_CLASS_NAME : WINDOWED_WIN_CLASS_NAME,
		unicodeTitle.c_str(),						// Window Title
		mWindowStyle,					// Required Window Style
		WindowRect.left, WindowRect.top,								// Window Position
		WindowRect.right-WindowRect.left,	// Calculate Window Width
		WindowRect.bottom-WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		mInstance,							// Instance
		reinterpret_cast<LPVOID>( this ) )) )

	{
		//killWindow();							// Reset The Display
		return false;								
	}

	mDC = ::GetDC( mWnd );
	if( ! mDC ) {
		killWindow( mFullScreen );
		return false;
	}

	mApp->getRenderer()->setup( mApp, mWnd, mDC );

	::DragAcceptFiles( mWnd, TRUE );
	enableMultiTouch();

	return true;									// Success
}

void AppImplMswBasic::killWindow( bool wasFullScreen )
{
	mApp->getRenderer()->kill();

	if( wasFullScreen )
		::ChangeDisplaySettings( NULL, 0 );

	if( mDC )
		::ReleaseDC( mWnd, mDC );

	if( mWnd )
		::DestroyWindow( mWnd );

	if( wasFullScreen )
		::UnregisterClass( FULLSCREEN_WIN_CLASS_NAME, mInstance );
	else
		::UnregisterClass( WINDOWED_WIN_CLASS_NAME, mInstance );

	mWnd = 0;
}

void AppImplMswBasic::toggleFullScreen()
{
	bool prevFullScreen = mFullScreen;
	HDC oldDC = mDC;
	HWND oldWnd = mWnd;
	
	mFullScreen = ! mFullScreen;
	
	int windowWidth, windowHeight;
	if( mApp->isFullScreen() ) {
		windowWidth = mApp->getSettings().getFullScreenWidth();
		windowHeight = mApp->getSettings().getFullScreenHeight();	
	}
	else {
		windowWidth = mApp->getSettings().getWindowWidth();
		windowHeight = mApp->getSettings().getWindowHeight();
	}

	// prepare for a new one
	if( prevFullScreen ) {
		::ChangeDisplaySettings( NULL, 0 );
	}

	mApp->getRenderer()->prepareToggleFullScreen();
	createWindow( &windowWidth, &windowHeight );
	mApp->getRenderer()->finishToggleFullScreen();

	::ReleaseDC( oldWnd, oldDC );
	::DestroyWindow( oldWnd ); 
	if( prevFullScreen )
		::UnregisterClass( FULLSCREEN_WIN_CLASS_NAME, mInstance );
	else
		::UnregisterClass( WINDOWED_WIN_CLASS_NAME, mInstance );

	mWindowWidth = windowWidth;
	mWindowHeight = windowHeight;

	::ShowWindow( mWnd, SW_SHOW );
	::SetForegroundWindow( mWnd );
	::SetFocus( mWnd );
	::DragAcceptFiles( mWnd, TRUE );
	enableMultiTouch();
	
	mApp->privateResize__( ResizeEvent( Vec2i( mApp->getWindowWidth(), mApp->getWindowHeight() ) ) );
}

void AppImplMswBasic::enableMultiTouch()
{
	if( mApp->getSettings().isMultiTouchEnabled() ) {
		// we need to make sure this version of User32 even has MultiTouch symbols, so we'll do that with GetProcAddress
		BOOL (WINAPI *RegisterTouchWindow)( HWND, ULONG);
		*(DWORD *)&RegisterTouchWindow = (DWORD)::GetProcAddress( ::GetModuleHandle(TEXT("user32.dll")), "RegisterTouchWindow" );
		if( RegisterTouchWindow ) {
			(*RegisterTouchWindow)( mWnd, 0 );
		}
	}
}

void AppImplMswBasic::setWindowWidth( int aWindowWidth )
{
	int screenWidth, screenHeight;
	getScreenSize( aWindowWidth, mApp->getWindowHeight(), &screenWidth, &screenHeight );
	::SetWindowPos( mWnd, HWND_TOP, 0, 0, screenWidth, screenHeight, SWP_NOMOVE );
}

void AppImplMswBasic::setWindowHeight( int aWindowHeight )
{
	int screenWidth, screenHeight;
	getScreenSize( mApp->getWindowWidth(), aWindowHeight, &screenWidth, &screenHeight );
	::SetWindowPos( mWnd, HWND_TOP, 0, 0, screenWidth, screenHeight, SWP_NOMOVE );
}

void AppImplMswBasic::setWindowSize( int aWindowWidth, int aWindowHeight )
{
	int screenWidth, screenHeight;
	getScreenSize( aWindowWidth, aWindowHeight, &screenWidth, &screenHeight );
	::SetWindowPos( mWnd, HWND_TOP, 0, 0, screenWidth, screenHeight, SWP_NOMOVE );
}

float AppImplMswBasic::setFrameRate( float aFrameRate )
{
	return aFrameRate;
}

void AppImplMswBasic::getScreenSize( int clientWidth, int clientHeight, int *resultWidth, int *resultHeight )
{
	RECT windowRect;
	windowRect.left = windowRect.top = 0;
	windowRect.right = clientWidth;
	windowRect.bottom = clientHeight;
	::AdjustWindowRectEx( &windowRect, mWindowStyle, FALSE, mWindowExStyle );
	*resultWidth = windowRect.right - windowRect.left;
	*resultHeight = windowRect.bottom - windowRect.top;
}

void AppImplMswBasic::onTouch( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	// pull these symbols dynamically out of the user32.dll
	static BOOL (WINAPI *GetTouchInputInfo)( HTOUCHINPUT, UINT, PTOUCHINPUT, int ) = NULL;
	if( ! GetTouchInputInfo )
		*(size_t *)&GetTouchInputInfo = (size_t)::GetProcAddress( ::GetModuleHandle(TEXT("user32.dll")), "GetTouchInputInfo" );
	static BOOL (WINAPI *CloseTouchInputHandle)( HTOUCHINPUT ) = NULL;
	if( ! CloseTouchInputHandle )
		*(size_t *)&CloseTouchInputHandle = (size_t)::GetProcAddress( ::GetModuleHandle(TEXT("user32.dll")), "CloseTouchInputHandle" );

	bool handled = false;
	double currentTime = getApp()->getElapsedSeconds(); // we don't trust the device's sense of time
	unsigned int numInputs = LOWORD( wParam );
	shared_ptr<TOUCHINPUT> pInputs = shared_ptr<TOUCHINPUT>( new TOUCHINPUT[numInputs], checked_array_deleter<TOUCHINPUT>() );
	if( pInputs ) {
		vector<TouchEvent::Touch> beganTouches, movedTouches, endTouches, activeTouches;
		if( GetTouchInputInfo((HTOUCHINPUT)lParam, numInputs, pInputs.get(), sizeof(TOUCHINPUT) ) ) {
			for( unsigned int i = 0; i < numInputs; i++ ) {
				const TOUCHINPUT &ti = pInputs.get()[i];
				if( ti.dwID != 0 ) {
					POINT pt;
					// this has a small problem, which is that we lose the subpixel precision of the touch points.
					// However ScreenToClient doesn't support floating or fixed point either, so we're stuck 
					// unless we write our own ScreenToClient, which actually should be doable
					pt.x = TOUCH_COORD_TO_PIXEL( ti.x );
					pt.y = TOUCH_COORD_TO_PIXEL( ti.y );
					::ScreenToClient( hWnd, &pt );
					if( ti.dwFlags & 0x0004/*TOUCHEVENTF_UP*/ ) {
						Vec2f prevPos = mMultiTouchPrev[ti.dwID];
						endTouches.push_back( TouchEvent::Touch( Vec2f( (float)pt.x, (float)pt.y ), prevPos, ti.dwID, currentTime, &pInputs.get()[i] ) );
						mMultiTouchPrev.erase( ti.dwID );
					}
					else if( ti.dwFlags & 0x0002/*TOUCHEVENTF_DOWN*/ ) {
						beganTouches.push_back( TouchEvent::Touch( Vec2f( (float)pt.x, (float)pt.y ), Vec2f( (float)pt.x, (float)pt.y ), ti.dwID, currentTime, &pInputs.get()[i] ) );
						mMultiTouchPrev[ti.dwID] = Vec2f( (float)pt.x, (float)pt.y );
						activeTouches.push_back( beganTouches.back() );
					}
					else if( ti.dwFlags & 0x0001/*TOUCHEVENTF_MOVE*/ ) {
						movedTouches.push_back( TouchEvent::Touch( Vec2f( (float)pt.x, (float)pt.y ), mMultiTouchPrev[ti.dwID], ti.dwID, currentTime, &pInputs.get()[i] ) );
						activeTouches.push_back( movedTouches.back() );
						mMultiTouchPrev[ti.dwID] = Vec2f( (float)pt.x, (float)pt.y );
					}
				}
            }
            
            getApp()->privateSetActiveTouches__( activeTouches );
            
            // we need to post the event here so that our pInputs array is still valid since we've passed addresses into it as the native pointers
            if( ! beganTouches.empty() )
				getApp()->privateTouchesBegan__( beganTouches );
			if( ! movedTouches.empty() )
				getApp()->privateTouchesMoved__( movedTouches );
			if( ! endTouches.empty() )
				getApp()->privateTouchesEnded__( endTouches );
			
            handled = ( ! beganTouches.empty() ) || ( ! movedTouches.empty() ) || ( ! endTouches.empty() );
            CloseTouchInputHandle( (HTOUCHINPUT)lParam ); // this is exception-unsafe; we need some RAII goin' on there
        }
        else {
			// for now we'll just ignore an error
        }
	}

    if( ! handled ) {
        // if we didn't handle the message, let DefWindowProc handle it
        ::DefWindowProc( hWnd, WM_TOUCH, wParam, lParam );
    }
}

unsigned int prepMouseEventModifiers( WPARAM wParam )
{
	unsigned int result = 0;
	if( wParam & MK_CONTROL ) result |= MouseEvent::CTRL_DOWN;
	if( wParam & MK_LBUTTON ) result |= MouseEvent::LEFT_DOWN;
	if( wParam & MK_MBUTTON ) result |= MouseEvent::MIDDLE_DOWN;
	if( wParam & MK_RBUTTON ) result |= MouseEvent::RIGHT_DOWN;
	if( wParam & MK_SHIFT ) result |= MouseEvent::SHIFT_DOWN;
	if( ::GetKeyState( VK_MENU ) < 0 ) result |= MouseEvent::ALT_DOWN;	
	if( (::GetKeyState( VK_LWIN ) < 0) || (::GetKeyState( VK_RWIN ) < 0) ) result |= MouseEvent::META_DOWN;
	return result;
}

// Certain key codes need to be refined, for example VK_MENU needs to be
// converted into VK_LALT or VK_RALT
int prepNativeKeyCode( WPARAM wParam )
{
	unsigned int result = (int)wParam;
	if( wParam == VK_MENU ) {
		result = ( ::GetKeyState( VK_RMENU ) ) ? VK_RMENU : VK_LMENU;
	}
	else if( wParam == VK_SHIFT ) {
		result = ( ::GetKeyState( VK_RSHIFT ) ) ? VK_RSHIFT : VK_LSHIFT;	
	}
	else if( wParam == VK_CONTROL ) {
		result = ( ::GetKeyState( VK_RCONTROL ) ) ? VK_RCONTROL : VK_LCONTROL;		
	}
	return result;
}

char mapVirtualKey( WPARAM wParam )
{
	BYTE keyboardState[256];
	::GetKeyboardState( keyboardState );
	WORD result[4];
	
	int resultLength = ::ToAscii( wParam, ::MapVirtualKey( wParam, 0 ), keyboardState, result, 0 );
	if( resultLength == 1 )
		return (char)result[0];
	else
		return 0;
}

unsigned int prepKeyEventModifiers()
{
	unsigned int result = 0;
	if( ::GetKeyState( VK_CONTROL ) & 0x8000 ) result |= KeyEvent::CTRL_DOWN;
	if( ::GetKeyState( VK_SHIFT ) & 0x8000 ) result |= KeyEvent::SHIFT_DOWN;
	if( ( ::GetKeyState( VK_LMENU ) & 0x8000 ) || ( ::GetKeyState( VK_RMENU ) & 0x8000 ) ) result |= KeyEvent::ALT_DOWN;	
	if( ( ::GetKeyState( VK_LWIN ) < 0 ) || ( ::GetKeyState( VK_RWIN ) < 0 ) ) result |= KeyEvent::META_DOWN;
	return result;
}

extern "C" {
LRESULT CALLBACK WndProc(	HWND	mWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	AppImplMswBasic* impl = 0;
	
	// if the message is WM_NCCREATE we need to hide 'this' in the window long
	if( uMsg == WM_NCCREATE ) {
		impl = reinterpret_cast<AppImplMswBasic*>(((LPCREATESTRUCT)lParam)->lpCreateParams);
		::SetWindowLongPtr( mWnd, GWL_USERDATA, (__int3264)(LONG_PTR)impl ); 
	}
	else // the warning on this line is harmless
		impl = reinterpret_cast<AppImplMswBasic*>( ::GetWindowLongPtr( mWnd, GWL_USERDATA ) );

	if( ! impl )
		return DefWindowProc( mWnd, uMsg, wParam, lParam );		

	switch( uMsg ) {									// Check For Windows Messages
		case WM_SYSCOMMAND:							// Intercept System Commands
			switch( wParam ) {							// Check System Calls
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
					if( impl->getApp()->getSettings().getPowerManagement() )
						return 0;							// prevent
			}
		break;
		case WM_CLOSE:								// Did We Receive A Close Message?
			::PostQuitMessage(0);					// Send A Quit Message
			impl->quit();
			return 0;
		break;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:							// Is A Key Being Held Down? 
			impl->getApp()->privateKeyDown__( KeyEvent( KeyEvent::translateNativeKeyCode( prepNativeKeyCode( (int)wParam ) ), 
					mapVirtualKey( wParam ), prepKeyEventModifiers(), (int)wParam ) );
			return 0;								// Jump Back
		break;
		case WM_SYSKEYUP:
		case WM_KEYUP:								// Has A Key Been Released?
			impl->getApp()->privateKeyUp__( KeyEvent( KeyEvent::translateNativeKeyCode( prepNativeKeyCode( (int)wParam ) ), 
					mapVirtualKey( wParam ), prepKeyEventModifiers(), (int)wParam ) );
			return 0;								// Jump Back
		break;
		
		// mouse events
		case WM_LBUTTONDOWN:
			::SetCapture( mWnd );
			impl->mIsDragging = true;
			impl->getApp()->privateMouseDown__( MouseEvent( MouseEvent::LEFT_DOWN, LOSHORT(lParam), HISHORT(lParam), prepMouseEventModifiers( wParam ), 0.0f, static_cast<unsigned int>( wParam ) ) );
			return 0;
		break;
		case WM_RBUTTONDOWN:
			::SetCapture( mWnd );
			impl->mIsDragging = true;
			impl->getApp()->privateMouseDown__( MouseEvent( MouseEvent::RIGHT_DOWN, LOSHORT(lParam), HISHORT(lParam), prepMouseEventModifiers( wParam ), 0.0f, static_cast<unsigned int>( wParam ) ) );
			return 0;
		break;		
		case WM_MBUTTONDOWN:
			::SetCapture( mWnd );
			impl->mIsDragging = true;		
			impl->getApp()->privateMouseDown__( MouseEvent( MouseEvent::MIDDLE_DOWN, LOSHORT(lParam), HISHORT(lParam), prepMouseEventModifiers( wParam ), 0.0f, static_cast<unsigned int>( wParam ) ) );
			return 0;
		break;
		case WM_LBUTTONUP:
			::ReleaseCapture();
			impl->mIsDragging = false;		
			impl->getApp()->privateMouseUp__( MouseEvent( MouseEvent::LEFT_DOWN, LOSHORT(lParam), HISHORT(lParam), prepMouseEventModifiers( wParam ), 0.0f, static_cast<unsigned int>( wParam ) ) );
			return 0;
		break;
		case WM_RBUTTONUP:
			::ReleaseCapture();
			impl->mIsDragging = false;		
			impl->getApp()->privateMouseUp__( MouseEvent( MouseEvent::RIGHT_DOWN, LOSHORT(lParam), HISHORT(lParam), prepMouseEventModifiers( wParam ), 0.0f, static_cast<unsigned int>( wParam ) ) );
			return 0;
		break;		
		case WM_MBUTTONUP:
			::ReleaseCapture();
			impl->mIsDragging = false;	
			impl->getApp()->privateMouseUp__( MouseEvent( MouseEvent::MIDDLE_DOWN, LOSHORT(lParam), HISHORT(lParam), prepMouseEventModifiers( wParam ), 0.0f, static_cast<unsigned int>( wParam ) ) );
			return 0;
		break;
		case WM_MOUSEWHEEL:
			impl->getApp()->privateMouseWheel__( MouseEvent( 0, LOSHORT(lParam), HISHORT(lParam), prepMouseEventModifiers( wParam ),
					GET_WHEEL_DELTA_WPARAM( wParam ) / 120.0f, static_cast<unsigned int>( wParam ) ) );			
		break;
		case WM_KILLFOCUS:
			// if we lose capture during a drag, post a mouseup event as a notifier
			if( impl->mIsDragging ) {
				impl->getApp()->privateMouseUp__( MouseEvent( 0, LOSHORT(lParam), HISHORT(lParam), prepMouseEventModifiers( wParam ), 0.0f, static_cast<unsigned int>( wParam ) ) );
			}
			impl->mIsDragging = false;
		break;
		case WM_MOUSEMOVE:
			if( impl->mIsDragging ) {
				impl->getApp()->privateMouseDrag__( MouseEvent( 0, LOSHORT(lParam), HISHORT(lParam), prepMouseEventModifiers( wParam ),
						0.0f, static_cast<unsigned int>( wParam ) ) );						
}
			else
				impl->getApp()->privateMouseMove__( MouseEvent( 0, LOSHORT(lParam), HISHORT(lParam), prepMouseEventModifiers( wParam ),
						0.0f, static_cast<unsigned int>( wParam ) ) );						
		break;
		case WM_SIZE:
			if( impl->mHasBeenInitialized ) {
				impl->mWindowWidth = LOWORD(lParam);
				impl->mWindowHeight = HIWORD(lParam);
				impl->getApp()->privateResize__( ResizeEvent( Vec2i( impl->mWindowWidth, impl->mWindowHeight ) ) );
			}
			return 0;
		break;
		case WM_MOVE:
			if( impl->mHasBeenInitialized ) {
				impl->privateSetWindowOffset__( Vec2i( LOSHORT(lParam), HISHORT(lParam) ) );
			}
			return 0;
		break;
		case WM_DROPFILES: {
			HDROP dropH = (HDROP)wParam;
			POINT dropPoint;
			char fileName[8192];
			vector<string> files;
			
			int droppedFileCount = ::DragQueryFile( dropH, 0xFFFFFFFF, 0, 0 );
			for( int i = 0; i < droppedFileCount; ++i ) {
				::DragQueryFileA( dropH, i, fileName, 8192 );
				files.push_back( std::string( fileName ) );
			}

			::DragQueryPoint( dropH, &dropPoint );
			::DragFinish( dropH );

			FileDropEvent dropEvent( dropPoint.x, dropPoint.y, files );
			impl->getApp()->privateFileDrop__( dropEvent );
			return 0;
		}
		break;
		case WM_PAINT:
			impl->getApp()->getRenderer()->startDraw();
			impl->getApp()->draw();
			impl->getApp()->getRenderer()->finishDraw();
		break;
		case WM_TOUCH:
			impl->onTouch( mWnd, wParam, lParam );
		break;
	}

	// unhandled messages To DefWindowProc
	return DefWindowProc( mWnd, uMsg, wParam, lParam );
}
} // extern "C"

} } // namespace cinder::app
