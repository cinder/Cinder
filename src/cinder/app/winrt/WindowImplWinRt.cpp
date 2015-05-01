/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.
 
 Portions Copyright (c) 2013, Microsoft Open Technologies, Inc. 
 All rights reserved.

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

#include "cinder/app/winrt/WindowImplWinRt.h"
#include "cinder/app/winrt/AppWinRt.h"
#include "cinder/winrt/WinRtUtils.h"

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace cinder::winrt;
using namespace std;

namespace cinder { namespace app {

/*WindowImplWinRt::WindowImplWinRt( const Window::Format &format, AppWinRt *app )
	: mWindowOffset( 0, 0 ), mApp( app ), mIsDragging( false ), mHidden( false ), mTouchId( 0 ), mIsMultiTouchEnabled( false )
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
		ivec2 displaySize = mDisplay->getSize();
		mWindowOffset = mWindowedPos = ( displaySize - mWindowedSize ) / 2;
	}

	mRenderer->setup( mWnd, nullptr );
	// set WindowRef and its impl pointer to this
	mWindowRef = Window::privateCreate__( this, mAppImpl );
	
	completeCreation();
}*/

WindowImplWinRt::WindowImplWinRt( Windows::UI::Core::CoreWindow^ wnd, RendererRef renderer, AppWinRt *app )
	: mWnd( wnd ), mRenderer( renderer ), mApp( app ), mIsDragging( false ), mTouchId( 0 ), mIsMultiTouchEnabled( false ),
		mControlKeyPressed( false ), mAltKeyPressed( false ), mShiftKeyPressed( false )
{
	mTitle = "";

	float width, height;
	GetPlatformWindowDimensions( mWnd.Get(), &width, &height);
	mWindowOffset = ivec2( 0, 0);
	mWindowWidth = static_cast<int>(width);
	mWindowHeight = static_cast<int>(height);

	mDisplay = Display::getMainDisplay();

	mRenderer->setup( mWnd, nullptr );

	mWindowRef = Window::privateCreate__( this, mApp );
}

void WindowImplWinRt::completeCreation()
{
#if 0
	::DragAcceptFiles( mWnd, TRUE );
	enableMultiTouch();

	::ShowWindow( mWnd, SW_SHOW );
	::SetForegroundWindow( mWnd );
	::SetFocus( mWnd );
#endif
}

void WindowImplWinRt::setFullScreen( bool fullScreen, const FullScreenOptions &options )
{
	if( mFullScreen != fullScreen )
		toggleFullScreen();
}

void WindowImplWinRt::toggleFullScreen()
{
	mFullScreen = ! mFullScreen;
}

void WindowImplWinRt::getScreenSize( int clientWidth, int clientHeight, int *resultWidth, int *resultHeight )
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

void WindowImplWinRt::setPos( const ivec2 &windowPos )
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

void WindowImplWinRt::hide()
{
	//::ShowWindow( mWnd, SW_HIDE );
	mHidden = true;
}

void WindowImplWinRt::show()
{
	//::ShowWindow( mWnd, SW_SHOW );
	mHidden = false;
}

bool WindowImplWinRt::isHidden() const
{
	return mHidden;
}

std::string	WindowImplWinRt::getTitle() const
{
	return std::string( "Not Implemented" );
}

void WindowImplWinRt::setTitle( const std::string &title )
{
	mTitle = title;
}

void WindowImplWinRt::setSize( const ivec2 &windowSize )
{
	mWindowWidth = windowSize.x;
	mWindowHeight = windowSize.y;
}

void WindowImplWinRt::close()
{
//	mApp->closeWindow( this );
	// at this point 'this' is invalid so do nothing after this line
}

void WindowImplWinRt::enableMultiTouch()
{
	mIsMultiTouchEnabled = true;
}

void WindowImplWinRt::setBorderless( bool borderless )
{
}

void WindowImplWinRt::setAlwaysOnTop( bool alwaysOnTop )
{
}

void WindowImplWinRt::sizeChanged()
{
	float width, height;
	GetPlatformWindowDimensions( mWnd.Get(), &width, &height );
	mWindowWidth = (int)width;
	mWindowHeight = (int)height;

	getWindow()->emitResize();
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

void WindowImplWinRt::handlePointerDown( PointerEventArgs^ args ) 
{
	mIsMultiTouchEnabled ? handleTouchDown( args ) : handleMouseDown( args );
}

void WindowImplWinRt::handlePointerMoved( PointerEventArgs^ args ) 
{
	mIsMultiTouchEnabled ? handleTouchMoved( args ) : handleMouseMoved( args );
}

void WindowImplWinRt::handlePointerUp( PointerEventArgs^ args ) 
{
	mIsMultiTouchEnabled ? handleTouchUp( args ) : handleMouseUp( args );
}

void WindowImplWinRt::handleTouchDown( PointerEventArgs^ args ) 
{
	PointerPoint^ p = args->CurrentPoint;
	vector<TouchEvent::Touch> touches;
	float x = getScaledDPIValue(p->Position.X);
	float y = getScaledDPIValue(p->Position.Y);

	auto id = mTouchId++;
	mTouchIds[p->PointerId] = id;

	mMultiTouchPrev[id] = vec2(x, y);
	TouchEvent::Touch e( vec2(x, y ), vec2(x, y), id, app::getElapsedSeconds(), nullptr);
	touches.push_back(e);
	mActiveTouches.push_back(e);

	TouchEvent event( getWindow(), touches );
	getWindow()->emitTouchesBegan( &event );
}

void WindowImplWinRt::handleMouseDown( PointerEventArgs^ args ) 
{
	PointerPoint^ p = args->CurrentPoint;
	mIsDragging = true;		
	float x = getScaledDPIValue(p->Position.X);
	float y = getScaledDPIValue(p->Position.Y);
	MouseEvent event( getWindow(), prepPointerEventModifiers( args ), static_cast<int>(x), static_cast<int>(y), prepPointerEventModifiers( args ), 0.0f, 0L);
	getWindow()->emitMouseDown( &event );
}

void WindowImplWinRt::handleTouchMoved( PointerEventArgs^ args ) 
{
	PointerPoint^ p = args->CurrentPoint;
	vector<TouchEvent::Touch> touches;
	float x = getScaledDPIValue( p->Position.X );
	float y = getScaledDPIValue( p->Position.Y );
	auto id = mTouchIds[p->PointerId];
	
	if( mMultiTouchPrev.find(id) != mMultiTouchPrev.end() ) {
		mMultiTouchPrev[id] = vec2(x, y);
		TouchEvent::Touch e( vec2(x, y ), mMultiTouchPrev[id], id, app::getElapsedSeconds(), nullptr );
		mActiveTouches.erase( find_if( mActiveTouches.begin(), mActiveTouches.end(), [id](const TouchEvent::Touch & m) -> bool { return m.getId() == id; }) );
		mActiveTouches.push_back( e );
		touches.push_back( e );
		TouchEvent event( getWindow(), touches );
		getWindow()->emitTouchesMoved( &event );
	}
}

void WindowImplWinRt::handleMouseMoved( PointerEventArgs^ args )
{
	PointerPoint^ p = args->CurrentPoint;
	float x = getScaledDPIValue( p->Position.X );
	float y = getScaledDPIValue( p->Position.Y );
	MouseEvent event( getWindow(), prepPointerEventModifiers( args ), static_cast<int>(x), static_cast<int>(y), prepPointerEventModifiers( args ), 0.0f, 0L );
	if( mIsDragging )
		getWindow()->emitMouseDrag( &event );
	else
		getWindow()->emitMouseMove( &event );
}

void WindowImplWinRt::handleTouchUp( PointerEventArgs^ args )
{
	PointerPoint^ p = args->CurrentPoint;
	vector<TouchEvent::Touch> touches;
	float x = getScaledDPIValue( p->Position.X );
	float y = getScaledDPIValue( p->Position.Y );
	auto id = mTouchIds[p->PointerId];
	touches.push_back( TouchEvent::Touch( vec2(x, y ), mMultiTouchPrev[id], id, app::getElapsedSeconds(), nullptr ) );
	TouchEvent event( getWindow(), touches );
	getWindow()->emitTouchesEnded( &event );
	mActiveTouches.erase( find_if( mActiveTouches.begin(), mActiveTouches.end(),[id](const TouchEvent::Touch & m) -> bool { return m.getId() == id; }) );
	mMultiTouchPrev.erase( id );
	mTouchIds.erase( p->PointerId );
}

void WindowImplWinRt::handleMouseUp( PointerEventArgs^ args )
{
	PointerPoint^ p = args->CurrentPoint;
	float x = getScaledDPIValue( p->Position.X );
	float y = getScaledDPIValue( p->Position.Y );
	mIsDragging = false;		
	MouseEvent event( getWindow(), prepPointerEventModifiers( args ), static_cast<int>(x), static_cast<int>(y), prepPointerEventModifiers( args ), 0.0f, 0L );
	getWindow()->emitMouseUp( &event );
}

unsigned int WindowImplWinRt::prepKeyEventModifiers() const
{
	unsigned int result = 0;
	if( mControlKeyPressed ) result |= KeyEvent::CTRL_DOWN;
	if( mShiftKeyPressed ) result |= KeyEvent::SHIFT_DOWN;
	if( mAltKeyPressed ) result |= KeyEvent::ALT_DOWN;
	//if( ( ::GetKeyState( VK_LMENU ) & 0x8000 ) || ( ::GetKeyState( VK_RMENU ) & 0x8000 ) ) result |= KeyEvent::ALT_DOWN;	
	//if( ( ::GetKeyState( VK_LWIN ) < 0 ) || ( ::GetKeyState( VK_RWIN ) < 0 ) ) result |= KeyEvent::META_DOWN;
	return result;
}

void WindowImplWinRt::handleKeyDown( KeyEventArgs^ args )
{
	switch( args->VirtualKey ) {
		case VirtualKey::Control:
			mControlKeyPressed = true;
		break;
		case VirtualKey::Shift:
			mShiftKeyPressed = true;
		break;
		default:
			KeyEvent event( getWindow(), KeyEvent::translateNativeKeyCode((int)args->VirtualKey), (int)args->VirtualKey, (int)args->VirtualKey, prepKeyEventModifiers(), (int)args->VirtualKey );
			mApp->setWindow( mWindowRef );
			getWindow()->emitKeyDown( &event );
		break;
	}
}

void WindowImplWinRt::handleKeyUp( KeyEventArgs^ args )
{
	switch( args->VirtualKey ) {
		case VirtualKey::Control:
			mControlKeyPressed = false;
		break;
		case VirtualKey::Shift:
			mShiftKeyPressed = false;
		break;
		default:
			KeyEvent event( getWindow(), KeyEvent::translateNativeKeyCode((int)args->VirtualKey), (int)args->VirtualKey, (int)args->VirtualKey, prepKeyEventModifiers(), (int)args->VirtualKey );
			mApp->setWindow( mWindowRef );
			getWindow()->emitKeyUp( &event );
		break;
	}
}

void WindowImplWinRt::keyDown( const KeyEvent &event )
{
	KeyEvent localEvent( event );
	mApp->setWindow( mWindowRef );
	mWindowRef->emitKeyDown( &localEvent );
}

void WindowImplWinRt::draw()
{
	mApp->setWindow( mWindowRef );
	mRenderer->startDraw();
	mWindowRef->emitDraw();
	mRenderer->finishDraw();
}

void WindowImplWinRt::resize()
{
	mApp->setWindow( mWindowRef );
	mWindowRef->emitResize();
}

void WindowImplWinRt::redraw()
{
	//::RedrawWindow( mWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW );
}

void WindowImplWinRt::privateClose()
{
//	mRenderer->kill();
	//mWnd = 0;
}

} } // namespace cinder::app
