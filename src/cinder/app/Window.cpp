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
#include "cinder/app/App.h"

#if defined( CINDER_MSW )
	#include "cinder/app/AppImplMsw.h"
#elif defined( CINDER_WINRT )
	#include "cinder/app/AppImplWinRT.h"
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

Vec2i Window::getSize() const
{
	testValid();
	
#if defined( CINDER_COCOA )
	return [mImpl getSize];
#else
	return mImpl->getSize();
#endif
}

void Window::setSize( const Vec2i &size )
{
	testValid();
	
#if defined( CINDER_COCOA )
	[mImpl setSize:size];
#else
	mImpl->setSize( size );
#endif
}

Vec2i Window::getPos() const
{
	testValid();
	
#if defined( CINDER_COCOA )
	return [mImpl getPos];
#else
	return mImpl->getPos();
#endif
}

void Window::setPos( const Vec2i &pos ) const
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
	
	setSize( Vec2i( spanning.getWidth(), spanning.getHeight() ) );	
	setPos( spanning.getUL() );
}

float Window::getContentScale() const
{
	testValid();
	
#if defined( CINDER_COCOA )
	return [mImpl getContentScale];
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
	return mImpl->getNative();
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
	mSignalClose();
}

void Window::emitMove()
{
	getRenderer()->makeCurrentContext();
	mSignalMove();
}

void Window::emitResize()
{
	getRenderer()->makeCurrentContext();
	getRenderer()->defaultResize();
	mSignalResize();
	getApp()->resize();
}

void Window::emitDisplayChange()
{
	getRenderer()->makeCurrentContext();
	mSignalDisplayChange();
}

void Window::emitMouseDown( MouseEvent *event )
{
	getRenderer()->makeCurrentContext();
	mSignalMouseDown.set_combiner( EventCombiner<cinder::app::MouseEvent>( event ) );
	mSignalMouseDown( *event );
	if( ! event->isHandled() )
		getApp()->mouseDown( *event );
}

void Window::emitMouseDrag( MouseEvent *event )
{
	getRenderer()->makeCurrentContext();
	mSignalMouseDrag.set_combiner( EventCombiner<cinder::app::MouseEvent>( event ) );
	mSignalMouseDrag( *event );
	if( ! event->isHandled() )
		getApp()->mouseDrag( *event );
}

void Window::emitMouseUp( MouseEvent *event )
{
	getRenderer()->makeCurrentContext();
	mSignalMouseUp.set_combiner( EventCombiner<cinder::app::MouseEvent>( event ) );
	mSignalMouseUp( *event );
	if( ! event->isHandled() )
		getApp()->mouseUp( *event );
}

void Window::emitMouseWheel( MouseEvent *event )
{
	getRenderer()->makeCurrentContext();
	mSignalMouseWheel.set_combiner( EventCombiner<cinder::app::MouseEvent>( event ) );
	mSignalMouseWheel( *event );
	if( ! event->isHandled() )
		getApp()->mouseWheel( *event );
}

void Window::emitMouseMove( MouseEvent *event )
{
	getRenderer()->makeCurrentContext();
	mSignalMouseMove.set_combiner( EventCombiner<cinder::app::MouseEvent>( event ) );
	mSignalMouseMove( *event );
	if( ! event->isHandled() )
		getApp()->mouseMove( *event );
}

void Window::emitTouchesBegan( TouchEvent *event )
{
	getRenderer()->makeCurrentContext();
	mSignalTouchesBegan.set_combiner( EventCombiner<cinder::app::TouchEvent>( event ) );
	mSignalTouchesBegan( *event );
	if( ! event->isHandled() )
		getApp()->touchesBegan( *event );
}

void Window::emitTouchesMoved( TouchEvent *event )
{
	getRenderer()->makeCurrentContext();
	mSignalTouchesMoved.set_combiner( EventCombiner<cinder::app::TouchEvent>( event ) );
	mSignalTouchesMoved( *event );
	if( ! event->isHandled() )
		getApp()->touchesMoved( *event );
}

void Window::emitTouchesEnded( TouchEvent *event )
{
	getRenderer()->makeCurrentContext();
	mSignalTouchesEnded.set_combiner( EventCombiner<cinder::app::TouchEvent>( event ) );
	mSignalTouchesEnded( *event );
	if( ! event->isHandled() )
		getApp()->touchesEnded( *event );
}


const std::vector<TouchEvent::Touch>& Window::getActiveTouches() const
{
	testValid();
	
#if defined( CINDER_COCOA )
	return [mImpl getActiveTouches];
#elif defined( CINDER_MSW )
	return mImpl->getActiveTouches();
#elif defined( CINDER_WINRT )
	return mImpl->getActiveTouches();
#endif
}

void Window::emitKeyDown( KeyEvent *event )
{
	getRenderer()->makeCurrentContext();
	mSignalKeyDown.set_combiner( EventCombiner<cinder::app::KeyEvent>( event ) );
	mSignalKeyDown( *event );
	if( ! event->isHandled() )
		getApp()->keyDown( *event );
}

void Window::emitKeyUp( KeyEvent *event )
{
	getRenderer()->makeCurrentContext();
	mSignalKeyUp.set_combiner( EventCombiner<cinder::app::KeyEvent>( event ) );
	mSignalKeyUp( *event );
	if( ! event->isHandled() )
		getApp()->keyUp( *event );
}

void Window::emitDraw()
{
	mSignalDraw();
	getApp()->draw();
	mSignalPostDraw();
}

void Window::emitFileDrop( FileDropEvent *event )
{
	getRenderer()->makeCurrentContext();
	mSignalFileDrop.set_combiner( EventCombiner<cinder::app::FileDropEvent>( event ) );
	mSignalFileDrop( *event );
	if( ! event->isHandled() )
		getApp()->fileDrop( *event );
}


} } // namespace cinder::app
