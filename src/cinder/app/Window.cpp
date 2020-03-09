/*
 Copyright (c) 2010, The Cinder Project, All rights reserved.
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

#include "cinder/Cinder.h"
#include "cinder/app/Window.h"
#include "cinder/app/AppBase.h"

#if defined( CINDER_MSW_DESKTOP )
	#include "cinder/app/msw/AppImplMsw.h"
#elif defined( CINDER_UWP )
	#include "cinder/app/winrt/WindowImplWinRt.h"
#elif defined( CINDER_COCOA )
	#include <Foundation/Foundation.h>
#elif defined( CINDER_ANDROID )
	#include "cinder/app/android/WindowImplAndroid.h"
#elif defined( CINDER_LINUX )
	#include "cinder/app/linux/WindowImplLinux.h"
#endif

namespace cinder { namespace app {

bool Window::isFullScreen() const
{
	testValid();
	
#if defined( CINDER_COCOA )
	return [mImpl isFullScreen];
#else
	return mImpl->isFullScreen();
#endif
}

void Window::setFullScreen( bool fullScreen, const FullScreenOptions &options )
{
	testValid();
	
#if defined( CINDER_COCOA )
	[mImpl setFullScreen:fullScreen options:&options];
#else
	mImpl->setFullScreen( fullScreen, options );
#endif
}

ivec2 Window::getSize() const
{
	testValid();
	
#if defined( CINDER_COCOA )
	return [mImpl getSize];
#else
	return mImpl->getSize();
#endif
}

void Window::setSize( const ivec2 &size )
{
	testValid();
	
#if defined( CINDER_COCOA )
	[mImpl setSize:size];
#else
	mImpl->setSize( size );
#endif
}

ivec2 Window::getPos() const
{
	testValid();
	
#if defined( CINDER_COCOA )
	return [mImpl getPos];
#else
	return mImpl->getPos();
#endif
}

void Window::setPos( const ivec2 &pos ) const
{
	testValid();
	
#if defined( CINDER_COCOA )
	[mImpl setPos:pos];
#else
	mImpl->setPos( pos );
#endif
}

void Window::spanAllDisplays()
{
	Area spanning = Display::getSpanningArea();
	
	setSize( ivec2( spanning.getWidth(), spanning.getHeight() ) );	
	setPos( spanning.getUL() );
}

ivec2 Window::getMousePos() const
{
	return app::AppBase::get()->getMousePos() - getPos();
}

float Window::getContentScale() const
{
	testValid();
	
#if defined( CINDER_COCOA )
	return [mImpl getContentScale];
#elif defined( CINDER_MSW_DESKTOP )
	return mImpl->getContentScale();
#else
	return 1.0f;
#endif
}

void Window::close()
{
	testValid();
	
#if defined( CINDER_COCOA )
	[mImpl close];
#else
	mImpl->close();
#endif
}

std::string	Window::getTitle() const
{
	testValid();
	
#if defined( CINDER_COCOA )
	NSString *title = [mImpl getTitle];
	return title ? std::string( [title cStringUsingEncoding:NSUTF8StringEncoding] ) : std::string();
#else
	return mImpl->getTitle();
#endif
}

void Window::setTitle( const std::string &title )
{
	testValid();
	
#if defined( CINDER_COCOA )
	[mImpl setTitle:[NSString stringWithUTF8String:title.c_str()]];
#else
	mImpl->setTitle( title );
#endif
}

bool Window::isBorderless() const
{
	testValid();
	
#if defined( CINDER_COCOA )
	return [mImpl isBorderless];
#else
	return mImpl->isBorderless();
#endif
}

void Window::setBorderless( bool borderless )
{
	testValid();
	
#if defined( CINDER_COCOA )
	[mImpl setBorderless:borderless];
#else
	mImpl->setBorderless( borderless );
#endif
}

bool Window::isAlwaysOnTop() const
{
	testValid();
	
#if defined( CINDER_COCOA )
	return [mImpl isAlwaysOnTop];
#else
	return mImpl->isAlwaysOnTop();
#endif
}

void Window::setAlwaysOnTop( bool alwaysOnTop )
{
	testValid();
	
#if defined( CINDER_COCOA )
	[mImpl setAlwaysOnTop:alwaysOnTop];
#else
	mImpl->setAlwaysOnTop( alwaysOnTop );
#endif
}

void Window::hide()
{
	testValid();
	
#if defined( CINDER_COCOA )
	[mImpl hide];
#else
	mImpl->hide();
#endif
}

void Window::show()
{
	testValid();
	
#if defined( CINDER_COCOA )
	[mImpl show];
#else
	mImpl->show();
#endif
}

bool Window::isHidden() const
{
	testValid();
	
#if defined( CINDER_COCOA )
	return [mImpl isHidden];
#else
	return mImpl->isHidden();
#endif	
}

DisplayRef Window::getDisplay() const
{
	testValid();

#if defined( CINDER_COCOA )
	return [mImpl getDisplay];
#else
	return mImpl->getDisplay();
#endif	
}

RendererRef Window::getRenderer() const
{
	testValid();
	
#if defined( CINDER_COCOA )
	return [mImpl getRenderer];
#else
	return mImpl->getRenderer();
#endif
}

void* Window::getNative() const
{
	testValid();

#if defined( CINDER_COCOA )
	return [mImpl getNative];
#else
	return (void*)mImpl->getNative();
#endif
}

#if defined( CINDER_COCOA_TOUCH )
UIViewController* Window::getNativeViewController()
{
	testValid();

	return [mImpl getNativeViewController];
}
#endif


// Signal Emitters
void Window::emitClose()
{
	mSignalClose.emit();
}

void Window::emitMove()
{
	applyCurrentContext();

	mSignalMove.emit();
}

void Window::emitResize()
{
	applyCurrentContext();

	getRenderer()->defaultResize();
	mSignalResize.emit();
	getApp()->resize();
}

void Window::emitDisplayChange()
{
	applyCurrentContext();

	mSignalDisplayChange.emit();
}

void Window::emitMouseDown( MouseEvent *event )
{
	applyCurrentContext();

	CollectorEvent<MouseEvent> collector( event );
	mSignalMouseDown.emit( collector, *event );
	if( ! event->isHandled() )
		getApp()->mouseDown( *event );
}

void Window::emitMouseDrag( MouseEvent *event )
{
	applyCurrentContext();

	CollectorEvent<MouseEvent> collector( event );
	mSignalMouseDrag.emit( collector, *event );
	if( ! event->isHandled() )
		getApp()->mouseDrag( *event );
}

void Window::emitMouseUp( MouseEvent *event )
{
	applyCurrentContext();

	CollectorEvent<MouseEvent> collector( event );
	mSignalMouseUp.emit( collector, *event );
	if( ! event->isHandled() )
		getApp()->mouseUp( *event );
}

void Window::emitMouseWheel( MouseEvent *event )
{
	applyCurrentContext();

	CollectorEvent<MouseEvent> collector( event );
	mSignalMouseWheel.emit( collector, *event );
	if( ! event->isHandled() )
		getApp()->mouseWheel( *event );
}

void Window::emitMouseMove( MouseEvent *event )
{
	applyCurrentContext();

	CollectorEvent<MouseEvent> collector( event );
	mSignalMouseMove.emit( collector, *event );
	if( ! event->isHandled() )
		getApp()->mouseMove( *event );
}

void Window::emitTouchesBegan( TouchEvent *event )
{
	applyCurrentContext();

	CollectorEvent<TouchEvent> collector( event );
	mSignalTouchesBegan.emit( collector, *event );
	if( ! event->isHandled() )
		getApp()->touchesBegan( *event );
}

void Window::emitTouchesMoved( TouchEvent *event )
{
	applyCurrentContext();

	CollectorEvent<TouchEvent> collector( event );
	mSignalTouchesMoved.emit( collector, *event );
	if( ! event->isHandled() )
		getApp()->touchesMoved( *event );
}

void Window::emitTouchesEnded( TouchEvent *event )
{
	applyCurrentContext();

	CollectorEvent<TouchEvent> collector( event );
	mSignalTouchesEnded.emit( collector, *event );
	if( ! event->isHandled() )
		getApp()->touchesEnded( *event );
}


const std::vector<TouchEvent::Touch>& Window::getActiveTouches() const
{
	testValid();
	
#if defined( CINDER_COCOA )
	return [mImpl getActiveTouches];
#elif defined( CINDER_MSW_DESKTOP )
	return mImpl->getActiveTouches();
#elif defined( CINDER_UWP )
	return mImpl->getActiveTouches();
#elif defined( CINDER_ANDROID )
	return mImpl->getActiveTouches();	
#elif defined( CINDER_LINUX )
	return mImpl->getActiveTouches();	
#endif
}

void Window::emitKeyDown( KeyEvent *event )
{
	applyCurrentContext();

	CollectorEvent<KeyEvent> collector( event );
	mSignalKeyDown.emit( collector, *event );
	if( ! event->isHandled() )
		getApp()->keyDown( *event );
}

void Window::emitKeyUp( KeyEvent *event )
{
	applyCurrentContext();

	CollectorEvent<KeyEvent> collector( event );
	mSignalKeyUp.emit( collector, *event );
	if( ! event->isHandled() )
		getApp()->keyUp( *event );
}

void Window::emitDraw()
{
	applyCurrentContext();
	
	mSignalDraw.emit();
	getApp()->draw();
	mSignalPostDraw.emit();
}

void Window::emitFileDrop( FileDropEvent *event )
{
	applyCurrentContext();

	CollectorEvent<FileDropEvent> collector( event );
	mSignalFileDrop.emit( collector, *event );
	if( ! event->isHandled() )
		getApp()->fileDrop( *event );
}

void Window::applyCurrentContext()
{
	// On the Mac the active GL Context can change behind our back in some scenarios; forcing the context switch on other platforms is expensive though
#if defined( CINDER_MAC )
	getRenderer()->makeCurrentContext( true );
#else
	getRenderer()->makeCurrentContext( false );
#endif
}


} } // namespace cinder::app
