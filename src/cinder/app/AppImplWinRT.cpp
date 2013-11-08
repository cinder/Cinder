// The copyright in this software is being made available under the BSD License, included below. 
// This software may be subject to other third party and contributor rights, including patent rights, 
// and no such rights are granted under this license.
//
// Copyright (c) 2013, Microsoft Open Technologies, Inc. 
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice, 
//   this list of conditions and the following disclaimer in the documentation 
//   and/or other materials provided with the distribution.
// - Neither the name of Microsoft Open Technologies, Inc. nor the names of its contributors 
//   may be used to endorse or promote products derived from this software 
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



#include "cinder/app/AppImplWinRT.h"
#include "cinder/app/App.h"
#include "cinder/Utilities.h"
#include "cinder/Display.h"
#include "cinder/WinRTUtils.h"
#include "cinder/Utilities.h"

#include <Windows.h>
#include <CommDlg.h>
#include <ShellAPI.h>
#include <collection.h>
#include <ppltasks.h>


#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define LOSHORT(l)           ((SHORT)(l))
#define HISHORT(l)           ((SHORT)(((DWORD)(l) >> 16) & 0xFFFF))
#undef min
#undef max

using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace concurrency;
using namespace Platform;
using namespace cinder::winrt;



using std::string;
using std::wstring;
using std::vector;
using std::pair;

namespace cinder { namespace app {

AppImplWinRT::AppImplWinRT( App *aApp )
	: mApp( aApp ), mSetupHasBeenCalled( false )
{
	mControlKey = false;
	mAltKey = false;
	mShiftKey = false;

}

AppImplWinRT::~AppImplWinRT()
{

}

void AppImplWinRT::hideCursor()
{

}

void AppImplWinRT::showCursor()
{
	//getWindow()->getNativeCoreWindow()->Get()->PointerCursor = ref new CoreCursor(CoreCursorType::Arrow, 0);

}

Buffer AppImplWinRT::loadResource( int id, const std::string &type )
{
#if 0
	HRSRC resInfoHandle;
	HGLOBAL resHandle;
	void *dataPtr;
	size_t dataSize;

	wchar_t unicodeType[1024]; 
	wsprintfW( unicodeType, L"%S", type.c_str() );
	resInfoHandle = ::FindResource( NULL, MAKEINTRESOURCE(id), unicodeType );
	if( resInfoHandle == NULL ) {
		throw ResourceLoadExc( id, type );
	}
	resHandle = ::LoadResource( NULL, resInfoHandle );
	if( resHandle == NULL ) {
		throw ResourceLoadExc( id, type );
	}
	// it's not necessary to unlock resources because the system automatically deletes them when the process
	// that created them terminates.
	dataPtr = ::LockResource( resHandle );
	if( dataPtr == 0 ) {
		throw ResourceLoadExc( id, type );
	}
	dataSize = ::SizeofResource( NULL, resInfoHandle );
	return Buffer( dataPtr, dataSize );
#endif
	return NULL;
}

fs::path AppImplWinRT::getAppPath()
{
	Windows::ApplicationModel::Package^ package = Windows::ApplicationModel::Package::Current;
	Windows::Storage::StorageFolder^ installedLocation = package->InstalledLocation;
	Platform::String^ output = installedLocation->Path;
	std::wstring t = std::wstring(output->Data());
	return fs::path(PlatformStringToString(output));
}

void AppImplWinRT::getFolderPath( const fs::path &initialPath,  std::vector<std::string> extensions, std::function<void (fs::path)> f)
{
	if(extensions.size() == 0) {
		throw std::exception( "Must specify at least one file extension in extensions argument" );
	}

	// FilePicker APIs will not work if the application is in a snapped state.
    // If an app wants to show a FilePicker while snapped, it must attempt to unsnap first
	if(!ensureUnsnapped()) {
		return;
	}
    FolderPicker^ folderPicker = ref new FolderPicker();
    folderPicker->SuggestedStartLocation = PickerLocationId::Desktop;
 
	for( auto iter = extensions.begin(); iter != extensions.end(); ++iter ) {
		std::wstring temp(iter->begin(), iter->end());
		folderPicker->FileTypeFilter->Append( ref new Platform::String(temp.c_str()));
	}

    create_task(folderPicker->PickSingleFolderAsync()).then([f](StorageFolder^ folder)
    {
        if (folder)
        {
			f(fs::path(toUtf8(folder->Path->Data())));
        }
        else
        {
			f(fs::path(""));
        }
    });
}

void AppImplWinRT::getOpenFilePath( const fs::path &initialPath,  std::vector<std::string> extensions, std::function<void (fs::path)> f)
{
	if(extensions.size() == 0) {
		throw std::exception( "Must specify at least one file extension in extensions argument" );
	}

	// FilePicker APIs will not work if the application is in a snapped state.
    // If an app wants to show a FilePicker while snapped, it must attempt to unsnap first
	if(!ensureUnsnapped()) {
		return;
	}
    FileOpenPicker^ picker = ref new FileOpenPicker();
    picker->SuggestedStartLocation = PickerLocationId::Desktop;
 
	for( auto iter = extensions.begin(); iter != extensions.end(); ++iter ) {
		std::wstring temp(iter->begin(), iter->end());
		picker->FileTypeFilter->Append( ref new Platform::String(temp.c_str()));
	}

    create_task(picker->PickSingleFileAsync()).then([f](StorageFile^ file)
    {
        if (file)
        {
			f(fs::path(toUtf8(file->Path->Data())));
        }
        else
        {
			f(fs::path(""));
        }
    });
}

void AppImplWinRT::getSaveFilePath( const fs::path &initialPath,std::vector<std::string> extensions,std::function<void (fs::path)> f)
{
	if(initialPath.empty() && extensions.size() == 0) {
		throw std::exception( "Must specify initialPath or at least one file extension" );
	}

    // FilePicker APIs will not work if the application is in a snapped state.
    // If an app wants to show a FilePicker while snapped, it must attempt to unsnap first
	if(!ensureUnsnapped()) {
		return;
	}

    FileSavePicker^ savePicker = ref new FileSavePicker();
	savePicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;

    auto plainTextExtensions = ref new Platform::Collections::Vector<String^>();
	
	if(extensions.size() > 0) {

		for( auto iter = extensions.begin(); iter != extensions.end(); ++iter ) {
			std::wstring temp(iter->begin(), iter->end());
			plainTextExtensions->Append( ref new Platform::String(temp.c_str()));
		}
	} else if(! initialPath.empty() ) {
		plainTextExtensions->Append(ref new Platform::String(toUtf16(initialPath.extension()).c_str()));
	} 

    savePicker->FileTypeChoices->Insert("", plainTextExtensions);

	if(! initialPath.empty() ) {
		savePicker->SuggestedFileName = ref new Platform::String(toUtf16(initialPath.filename()).c_str());
	} else {
		savePicker->SuggestedFileName = "New Document";
	}

    create_task(savePicker->PickSaveFileAsync()).then([f](StorageFile^ file)
    {
        if (file != nullptr)
        {
			f(fs::path(toUtf8(file->Path->Data())));
        }
        else
        {
			f(fs::path(""));
        }
    });
}


///////////////////////////////////////////////////////////////////////////////
// WindowImplWinRT
WindowImplWinRT::WindowImplWinRT( const Window::Format &format, AppImplWinRT *appImpl )
	: mWindowOffset( 0, 0 ), mAppImpl( appImpl ), mIsDragging( false ), mHidden( false ), mTouchId(0), mIsMultiTouchEnabled(false)
{	
	mTitle = "";
	mFullScreen = format.isFullScreen();
	mDisplay = format.getDisplay();
	mRenderer = format.getRenderer();
	mResizable = format.isResizable();
	mAlwaysOnTop = format.isAlwaysOnTop();
	mBorderless = format.isBorderless();

	mWindowedSize = format.getSize();
	mWindowWidth = mWindowedSize.x;
	mWindowHeight = mWindowedSize.y;
	if( format.isPosSpecified() )
		mWindowOffset = mWindowedPos = format.getPos();
	else {
		Vec2i displaySize = mDisplay->getSize();
		mWindowOffset = mWindowedPos = ( displaySize - mWindowedSize ) / 2;
	}

	mRenderer->setup( mAppImpl->getApp(), mWnd);
	// set WindowRef and its impl pointer to this
	mWindowRef = Window::privateCreate__( this, mAppImpl->getApp() );
	
	completeCreation();
}

WindowImplWinRT::WindowImplWinRT( DX_WINDOW_TYPE hwnd, RendererRef renderer, AppImplWinRT *appImpl )
	: mWnd( hwnd ), mRenderer( renderer ), mAppImpl( appImpl ), mIsDragging( false ), mTouchId(0), mIsMultiTouchEnabled(false)
{
	mTitle = "";

	float width, height;
	GetPlatformWindowDimensions(hwnd.Get(), &width, &height);
	mWindowOffset = Vec2i( 0, 0);
	mWindowWidth = static_cast<int>(width);
	mWindowHeight = static_cast<int>(height);

	mDisplay = Display::getMainDisplay();

	mRenderer->setup( mAppImpl->getApp(), mWnd);

	mWindowRef = Window::privateCreate__( this, mAppImpl->getApp() );
}

void WindowImplWinRT::createWindow( const Vec2i &windowSize, const std::string &title )
{

}

void WindowImplWinRT::completeCreation()
{
#if 0
	::DragAcceptFiles( mWnd, TRUE );
	enableMultiTouch();

	::ShowWindow( mWnd, SW_SHOW );
	::SetForegroundWindow( mWnd );
	::SetFocus( mWnd );
#endif
}


void WindowImplWinRT::setFullScreen( bool fullScreen, const FullScreenOptions &options )
{
	if( mFullScreen != fullScreen )
		toggleFullScreen();
}

void WindowImplWinRT::toggleFullScreen()
{
	mFullScreen = ! mFullScreen;
}

void WindowImplWinRT::getScreenSize( int clientWidth, int clientHeight, int *resultWidth, int *resultHeight )
{
#if 0
	RECT windowRect;
	windowRect.left = windowRect.top = 0;
	windowRect.right = clientWidth;
	windowRect.bottom = clientHeight;
	::AdjustWindowRectEx( &windowRect, mWindowStyle, FALSE, mWindowExStyle );
	*resultWidth = windowRect.right - windowRect.left;
	*resultHeight = windowRect.bottom - windowRect.top;
#endif
}

void WindowImplWinRT::setPos( const Vec2i &windowPos )
{
#if 0
	RECT clientArea;
	clientArea.left = windowPos.x; clientArea.top = windowPos.y;
	clientArea.right = windowPos.x + 1; clientArea.bottom = windowPos.y + 1; // we don't actually care about the lower-right
	::AdjustWindowRectEx( &clientArea, mWindowStyle, FALSE, mWindowExStyle );
	::SetWindowPos( mWnd, HWND_TOP, clientArea.left, clientArea.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
	
	POINT upperLeft;
	upperLeft.x = upperLeft.y = 0;
	::ClientToScreen( mWnd, &upperLeft );
	mWindowOffset.x = upperLeft.x;
	mWindowOffset.y = upperLeft.y;
#endif
}

void WindowImplWinRT::hide()
{
	//::ShowWindow( mWnd, SW_HIDE );
	mHidden = true;
}

void WindowImplWinRT::show()
{
	//::ShowWindow( mWnd, SW_SHOW );
	mHidden = false;
}

bool WindowImplWinRT::isHidden() const
{
	return mHidden;
}

std::string	WindowImplWinRT::getTitle() const
{
	return std::string("Not Implemented");
}

void WindowImplWinRT::setTitle( const std::string &title )
{
	mTitle = title;
}

void WindowImplWinRT::setSize( const Vec2i &windowSize )
{
	mWindowWidth = windowSize.x;
	mWindowHeight = windowSize.y;
}

void WindowImplWinRT::close()
{
	getAppImpl()->closeWindow( this );
	// at this point 'this' is invalid so do nothing after this line
}


void WindowImplWinRT::enableMultiTouch()
{
	mIsMultiTouchEnabled = true;
}

#if 0

void WindowImplWinRT::onTouch( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	// pull these symbols dynamically out of the user32.dll
	static BOOL (WINAPI *GetTouchInputInfo)( HTOUCHINPUT, UINT, PTOUCHINPUT, int ) = NULL;
	if( ! GetTouchInputInfo )
		*(size_t *)&GetTouchInputInfo = (size_t)::GetProcAddress( ::GetModuleHandle(TEXT("user32.dll")), "GetTouchInputInfo" );
	static BOOL (WINAPI *CloseTouchInputHandle)( HTOUCHINPUT ) = NULL;
	if( ! CloseTouchInputHandle )
		*(size_t *)&CloseTouchInputHandle = (size_t)::GetProcAddress( ::GetModuleHandle(TEXT("user32.dll")), "CloseTouchInputHandle" );

	bool handled = false;
	double currentTime = app::getElapsedSeconds(); // we don't trust the device's sense of time
	unsigned int numInputs = LOWORD( wParam );
	std::shared_ptr<TOUCHINPUT> pInputs = std::shared_ptr<TOUCHINPUT>( new TOUCHINPUT[numInputs], checked_array_deleter<TOUCHINPUT>() );
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
            
			mActiveTouches = activeTouches;
            
            // we need to post the event here so that our pInputs array is still valid since we've passed addresses into it as the native pointers
            if( ! beganTouches.empty() ) {
				TouchEvent event( getWindow(), beganTouches );
				getWindow()->emitTouchesBegan( &event );
			}
			if( ! movedTouches.empty() ) {
				TouchEvent event( getWindow(), movedTouches );
				getWindow()->emitTouchesMoved( &event );
			}
			if( ! endTouches.empty() ) {
				TouchEvent event( getWindow(), endTouches );
				getWindow()->emitTouchesEnded( &event );
			}
			
            handled = ( ! beganTouches.empty() ) || ( ! movedTouches.empty() ) || ( ! endTouches.empty() );
            CloseTouchInputHandle( (HTOUCHINPUT)lParam ); // this is exception-unsafe; we need some RAII goin' on there*/
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

WCHAR mapVirtualKey( WPARAM wParam )
{
	BYTE keyboardState[256];
	::GetKeyboardState( keyboardState );
	WCHAR result[4];

	// the control key messes up the ToAscii result, so we zero it out
	keyboardState[VK_CONTROL] = 0;
	
	int resultLength = ::ToUnicode( wParam, ::MapVirtualKey( wParam, 0 ), keyboardState, result, 4, 0 );
	if( resultLength >= 1 )
		return result[0];
	else
		return 0;
}


#endif

void WindowImplWinRT::setBorderless( bool borderless )
{

}

unsigned int AppImplWinRT::prepKeyEventModifiers()
{
	unsigned int result = 0;
	if( mControlKey ) result |= KeyEvent::CTRL_DOWN;
	if( mShiftKey ) result |= KeyEvent::SHIFT_DOWN;
	if( mAltKey ) result |= KeyEvent::ALT_DOWN;
	//if( ( ::GetKeyState( VK_LMENU ) & 0x8000 ) || ( ::GetKeyState( VK_RMENU ) & 0x8000 ) ) result |= KeyEvent::ALT_DOWN;	
	//if( ( ::GetKeyState( VK_LWIN ) < 0 ) || ( ::GetKeyState( VK_RWIN ) < 0 ) ) result |= KeyEvent::META_DOWN;
	return result;
}



void AppImplWinRT::handleKeyDown(KeyEventArgs^ e) 
{
	switch(e->VirtualKey) {
	case VirtualKey::Control:
		mControlKey = true;
		break;

	case VirtualKey::Shift:
		mShiftKey = true;
		break;

	default:
		KeyEvent event( getWindow(), 
				KeyEvent::translateNativeKeyCode((int)e->VirtualKey), 
				(int)e->VirtualKey, 
				(int)e->VirtualKey, 
				prepKeyEventModifiers(), 
				(int)e->VirtualKey );

		getWindow()->emitKeyDown( &event );
		break;
	}
}

void AppImplWinRT::handleKeyUp(KeyEventArgs^ e) 
{
	switch(e->VirtualKey) {
	case VirtualKey::Control:
		mControlKey = false;
		break;

	case VirtualKey::Shift:
		mShiftKey = false;
		break;

	default:
		KeyEvent event( getWindow(), 
				KeyEvent::translateNativeKeyCode((int)e->VirtualKey), 
				(int)e->VirtualKey, 
				(int)e->VirtualKey, 
				prepKeyEventModifiers(), 
				(int)e->VirtualKey );

		getWindow()->emitKeyUp( &event );
		break;
	}
}

void WindowImplWinRT::setAlwaysOnTop( bool alwaysOnTop )
{

}

unsigned int prepPointerEventModifiers(PointerEventArgs^ args)
{
	VirtualKeyModifiers modifiers = args->KeyModifiers;
	PointerPoint^ p = args->CurrentPoint;
	PointerPointProperties^ properties = p->Properties;

	unsigned int result = 0;
	if(( modifiers & VirtualKeyModifiers::Control ) == VirtualKeyModifiers::Control)
		result |= MouseEvent::CTRL_DOWN;
	if(( modifiers & VirtualKeyModifiers::Shift ) == VirtualKeyModifiers::Shift)
		result |= MouseEvent::SHIFT_DOWN;
	if(( modifiers & VirtualKeyModifiers::Menu ) == VirtualKeyModifiers::Menu)
		result |= MouseEvent::ALT_DOWN;
	if(( modifiers & VirtualKeyModifiers::Windows ) == VirtualKeyModifiers::Windows)
		result |= MouseEvent::META_DOWN;

	if(properties->IsLeftButtonPressed)
		result |= MouseEvent::LEFT_DOWN;
	if(properties->IsMiddleButtonPressed)
		result |= MouseEvent::MIDDLE_DOWN;
	if(properties->IsRightButtonPressed)
		result |= MouseEvent::RIGHT_DOWN;

	return result;
}





void WindowImplWinRT::handlePointerDown(PointerEventArgs^ args) 
{
	mIsMultiTouchEnabled ? handleTouchDown(args) : handleMouseDown(args);
}

void WindowImplWinRT::handlePointerMoved(PointerEventArgs^ args) 
{
	mIsMultiTouchEnabled ? handleTouchMoved(args) : handleMouseMoved(args);
}

void WindowImplWinRT::handlePointerUp(PointerEventArgs^ args) 
{
	mIsMultiTouchEnabled ? handleTouchUp(args) : handleMouseUp(args);
}

void WindowImplWinRT::handleTouchDown(PointerEventArgs^ args) 
{
	PointerPoint^ p = args->CurrentPoint;
	vector<TouchEvent::Touch> touches;
	float x = getScaledDPIValue(p->Position.X);
	float y = getScaledDPIValue(p->Position.Y);

	auto id = mTouchId++;
	mTouchIds[p->PointerId] = id;

	mMultiTouchPrev[id] = Vec2f(x, y);
	TouchEvent::Touch e( Vec2f(x, y ), Vec2f(x, y), id, app::getElapsedSeconds(), nullptr);
	touches.push_back(e);
	mActiveTouches.push_back(e);

	TouchEvent event( getWindow(), touches );
	getWindow()->emitTouchesBegan( &event );
}

void WindowImplWinRT::handleMouseDown(PointerEventArgs^ args) 
{
	PointerPoint^ p = args->CurrentPoint;
	mIsDragging = true;		
	float x = getScaledDPIValue(p->Position.X);
	float y = getScaledDPIValue(p->Position.Y);
	MouseEvent event( getWindow(), prepPointerEventModifiers( args ), static_cast<int>(x), static_cast<int>(y), prepPointerEventModifiers( args ), 0.0f, 0L);
	getWindow()->emitMouseDown( &event );
}


void WindowImplWinRT::handleTouchMoved(PointerEventArgs^ args) 
{
	PointerPoint^ p = args->CurrentPoint;
	vector<TouchEvent::Touch> touches;
	float x = getScaledDPIValue(p->Position.X);
	float y = getScaledDPIValue(p->Position.Y);
	auto id = mTouchIds[p->PointerId];
	
	if(mMultiTouchPrev.find(id) != mMultiTouchPrev.end()) {
		mMultiTouchPrev[id] = Vec2f(x, y);
		TouchEvent::Touch e(  Vec2f(x, y ), mMultiTouchPrev[id], id, app::getElapsedSeconds(), nullptr);
		mActiveTouches.erase(std::find_if(mActiveTouches.begin(), mActiveTouches.end(),[id](const TouchEvent::Touch & m) -> bool { return m.getId() == id; }));
		mActiveTouches.push_back(e);
		touches.push_back(e);
		TouchEvent event( getWindow(), touches );
		getWindow()->emitTouchesMoved( &event );
	}
}

void WindowImplWinRT::handleMouseMoved(PointerEventArgs^ args) 
{
	PointerPoint^ p = args->CurrentPoint;
	float x = getScaledDPIValue(p->Position.X);
	float y = getScaledDPIValue(p->Position.Y);
	MouseEvent event( getWindow(), prepPointerEventModifiers( args ), static_cast<int>(x), static_cast<int>(y), prepPointerEventModifiers( args ), 0.0f, 0L);
	if(mIsDragging)		
		getWindow()->emitMouseDrag( &event );
	else
		getWindow()->emitMouseMove( &event );
}

void WindowImplWinRT::handleTouchUp(PointerEventArgs^ args) 
{
	PointerPoint^ p = args->CurrentPoint;
	vector<TouchEvent::Touch> touches;
	float x = getScaledDPIValue(p->Position.X);
	float y = getScaledDPIValue(p->Position.Y);
	auto id = mTouchIds[p->PointerId];
	touches.push_back( TouchEvent::Touch(  Vec2f(x, y ), mMultiTouchPrev[id], id, app::getElapsedSeconds(), nullptr) );
	TouchEvent event( getWindow(), touches );
	getWindow()->emitTouchesEnded( &event );
	mActiveTouches.erase(std::find_if(mActiveTouches.begin(), mActiveTouches.end(),[id](const TouchEvent::Touch & m) -> bool { return m.getId() == id; }));
	mMultiTouchPrev.erase(id);
	mTouchIds.erase(p->PointerId);
}

void WindowImplWinRT::handleMouseUp(PointerEventArgs^ args) 
{
	PointerPoint^ p = args->CurrentPoint;
	float x = getScaledDPIValue(p->Position.X);
	float y = getScaledDPIValue(p->Position.Y);
	mIsDragging = false;		
	MouseEvent event( getWindow(), prepPointerEventModifiers( args ), static_cast<int>(x), static_cast<int>(y), prepPointerEventModifiers( args ), 0.0f, 0L);
	getWindow()->emitMouseUp( &event );
}

void WindowImplWinRT::keyDown( const KeyEvent &event )
{
	KeyEvent localEvent( event );
	mAppImpl->setWindow( mWindowRef );
	mWindowRef->emitKeyDown( &localEvent );
}
void WindowImplWinRT::draw()
{
	mAppImpl->setWindow( mWindowRef );
	mRenderer->startDraw();
	mWindowRef->emitDraw();
	mRenderer->finishDraw();
}

void WindowImplWinRT::resize()
{
	mAppImpl->setWindow( mWindowRef );
	mWindowRef->emitResize();
}

void WindowImplWinRT::redraw()
{
	//::RedrawWindow( mWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW );
}

void WindowImplWinRT::privateClose()
{
	mRenderer->kill();

	//mWnd = 0;
}




} } // namespace cinder::app
