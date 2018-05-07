/*
 Copyright (c) 2013, The Cinder Project, All rights reserved.

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

#include "cinder/app/RendererGl.h"
#include "cinder/ip/Flip.h"

#if defined( CINDER_COCOA ) && ( ! defined( __OBJC__ ) )
	#error "This file must be compiled as Objective-C++ on the Mac"
#endif

#if defined( CINDER_COCOA )
	#if defined( CINDER_MAC )
		#import "cinder/app/cocoa/RendererImplGlMac.h"
	#elif defined( CINDER_COCOA_TOUCH )
		#import "cinder/app/cocoa/RendererImplGlCocoaTouch.h"
	#endif
#elif defined( CINDER_MSW_DESKTOP )
	#include "cinder/app/msw/AppImplMsw.h"
	#if defined( CINDER_GL_ANGLE )
		#include "cinder/app/msw/RendererImplGlAngle.h"
	#else
		#include "cinder/app/msw/RendererImplGlMsw.h"
	#endif
#elif defined( CINDER_UWP )
	#include "cinder/app/msw/RendererImplGlAngle.h"
#elif defined( CINDER_ANDROID )
	#include "cinder/app/android/RendererGlAndroid.h"
#elif defined( CINDER_LINUX )
	#include "cinder/app/linux/RendererGlLinux.h"
#endif

namespace cinder { namespace app {

RendererGl::RendererGl( const RendererGl::Options &options )
	: Renderer(), mImpl( nullptr ), mOptions( options )
{}

RendererGl::RendererGl( const RendererGl &renderer )
	: Renderer( renderer ), mImpl( nullptr ), mOptions( renderer.mOptions )
{
#if defined( CINDER_MSW_DESKTOP )
#elif defined( CINDER_ANDROID )
	mImpl = 0;
#endif
}

#if defined( CINDER_MAC )
RendererGl::~RendererGl()
{
	if( mImpl )
		::CFRelease( mImpl );
}

void RendererGl::setup( CGRect frame, NSView *cinderView, RendererRef sharedRenderer, bool retinaEnabled )
{
	RendererGlRef sharedGl = std::dynamic_pointer_cast<RendererGl>( sharedRenderer );
	mImpl = [[RendererImplGlMac alloc] initWithFrame:NSRectFromCGRect(frame) cinderView:cinderView renderer:this sharedRenderer:sharedGl withRetina:retinaEnabled];
	// This is necessary for Objective-C garbage collection to do the right thing
	::CFRetain( mImpl );
}

void RendererGl::startDraw()
{
	if( mStartDrawFn )
		mStartDrawFn( this );
	else
		[mImpl makeCurrentContext:false];
}

void RendererGl::finishDraw()
{
	if( mFinishDrawFn )
		mFinishDrawFn( this );
	else
		[mImpl flushBuffer];
}

void RendererGl::setFrameSize( int width, int height )
{
	[mImpl setFrameSize:NSSizeToCGSize(NSMakeSize( width, height ))];
}

void RendererGl::defaultResize()
{
	[mImpl defaultResize];
}

Surface RendererGl::copyWindowSurface( const Area &area, int32_t windowHeightPixels )
{
	Surface s( area.getWidth(), area.getHeight(), false );
	glFlush(); // there is some disagreement about whether this is necessary, but ideally performance-conscious users will use FBOs anyway
	GLint oldPackAlignment;
	glGetIntegerv( GL_PACK_ALIGNMENT, &oldPackAlignment );
	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glReadPixels( area.x1, windowHeightPixels - area.y2, area.getWidth(), area.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, s.getData() );
	glPixelStorei( GL_PACK_ALIGNMENT, oldPackAlignment );
	ip::flipVertical( &s );
	return s;
}

CGLContextObj RendererGl::getCglContext()
{
	return [mImpl getCglContext];
}

CGLPixelFormatObj RendererGl::getCglPixelFormat()
{
	return [mImpl getCglPixelFormat];
}

NSOpenGLContext* RendererGl::getNsOpenGlContext()
{
	return [mImpl getNsOpenGlContext];
}

void RendererGl::makeCurrentContext( bool force )
{
	[mImpl makeCurrentContext:force];
}

void RendererGl::swapBuffers()
{
	[mImpl flushBuffer];
}

#elif defined( CINDER_COCOA_TOUCH )
RendererGl::~RendererGl()
{
}

void RendererGl::setup( const Area &frame, UIView *cinderView, RendererRef sharedRenderer )
{
	RendererGlRef sharedRendererGl = std::dynamic_pointer_cast<RendererGl>( sharedRenderer );
	mImpl = [[RendererImplGlCocoaTouch alloc] initWithFrame:cocoa::createCgRect( frame ) cinderView:(UIView*)cinderView renderer:this sharedRenderer:sharedRendererGl];
}

EAGLContext* RendererGl::getEaglContext() const
{
	return [mImpl getEaglContext];
}

void RendererGl::startDraw()
{
	if( mStartDrawFn )
		mStartDrawFn( this );
	else
		[mImpl makeCurrentContext:false];
}

void RendererGl::finishDraw()
{
	if( mFinishDrawFn )
		mFinishDrawFn( this );
	else
		[mImpl flushBuffer];
}

void RendererGl::setFrameSize( int width, int height )
{
	[mImpl setFrameSize:CGSizeMake( width, height )];
}

void RendererGl::defaultResize()
{
	[mImpl defaultResize];
}

void RendererGl::makeCurrentContext( bool force)
{
	[mImpl makeCurrentContext:force];
}

void RendererGl::swapBuffers()
{
	[mImpl flushBuffer];
}

Surface	RendererGl::copyWindowSurface( const Area &area, int32_t windowHeightPixels )
{
	Surface s( area.getWidth(), area.getHeight(), true );
	glFlush(); // there is some disagreement about whether this is necessary, but ideally performance-conscious users will use FBOs anyway
	GLint oldPackAlignment;
	glGetIntegerv( GL_PACK_ALIGNMENT, &oldPackAlignment );
	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glReadPixels( area.x1, windowHeightPixels - area.y2, area.getWidth(), area.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, s.getData() );
	glPixelStorei( GL_PACK_ALIGNMENT, oldPackAlignment );
	ip::flipVertical( &s );

	return s;
}

#elif defined( CINDER_MSW_DESKTOP )
RendererGl::~RendererGl()
{
	delete mImpl;
}

void RendererGl::setup( WindowImplMsw *windowImpl, RendererRef sharedRenderer )
{
	mWindowImpl = windowImpl;

	if( ! mImpl )
#if defined( CINDER_GL_ANGLE )
		mImpl = new RendererImplGlAngle( this );
#else
		mImpl = new RendererImplGlMsw( this );
#endif
	if( ! mImpl->initialize( windowImpl, sharedRenderer ) )
		throw ExcRendererAllocation( "RendererImplGlMsw initialization failed." );
}

HWND RendererGl::getHwnd() const
{
	return mWindowImpl->getHwnd();
}

HDC RendererGl::getDc() const
{
	return mWindowImpl->getDc();
}

void RendererGl::kill()
{
	mImpl->kill();
}

void RendererGl::prepareToggleFullScreen()
{
	mImpl->prepareToggleFullScreen();
}

void RendererGl::finishToggleFullScreen()
{
	mImpl->finishToggleFullScreen();
}

void RendererGl::startDraw()
{
	if( mStartDrawFn )
		mStartDrawFn( this );
	else
		mImpl->makeCurrentContext();
}

void RendererGl::makeCurrentContext( bool force )
{
	mImpl->makeCurrentContext( force );
}

void RendererGl::swapBuffers()
{
	mImpl->swapBuffers();
}

void RendererGl::finishDraw()
{
	if( mFinishDrawFn )
		mFinishDrawFn( this );
	else
		mImpl->swapBuffers();
}

void RendererGl::defaultResize()
{
	mImpl->defaultResize();
}

Surface	RendererGl::copyWindowSurface( const Area &area, int32_t windowHeightPixels )
{
	Surface s( area.getWidth(), area.getHeight(), false );
	glFlush(); // there is some disagreement about whether this is necessary, but ideally performance-conscious users will use FBOs anyway
	GLint oldPackAlignment;
	glGetIntegerv( GL_PACK_ALIGNMENT, &oldPackAlignment );
	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glReadPixels( area.x1, windowHeightPixels - area.y2, area.getWidth(), area.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, s.getData() );
	glPixelStorei( GL_PACK_ALIGNMENT, oldPackAlignment );
	ip::flipVertical( &s );
	return s;
}
#elif defined( CINDER_UWP )
RendererGl::~RendererGl()
{
	delete mImpl;
}

void RendererGl::setup( ::Platform::Agile<Windows::UI::Core::CoreWindow> wnd, RendererRef sharedRenderer )
{
	mWnd = wnd;
	if( ! mImpl )
		mImpl = new RendererImplGlAngle( this );
	if( ! mImpl->initialize( wnd, sharedRenderer ) )
		throw ExcRendererAllocation( "RendererImplGlMsw initialization failed." );
}

void RendererGl::prepareToggleFullScreen()
{
	mImpl->prepareToggleFullScreen();
}

void RendererGl::finishToggleFullScreen()
{
	mImpl->finishToggleFullScreen();
}

void RendererGl::startDraw()
{
	if( mStartDrawFn )
		mStartDrawFn( this );
	else
		mImpl->makeCurrentContext( false );
}

void RendererGl::makeCurrentContext( bool force )
{
	mImpl->makeCurrentContext( force );
}

void RendererGl::swapBuffers()
{
	mImpl->swapBuffers();
}

void RendererGl::finishDraw()
{
	if( mFinishDrawFn )
		mFinishDrawFn( this );
	else
		mImpl->swapBuffers();
}

void RendererGl::defaultResize()
{
	mImpl->defaultResize();
}

Surface	RendererGl::copyWindowSurface( const Area &area, int32_t windowHeightPixels )
{
	Surface s( area.getWidth(), area.getHeight(), false );
	glFlush(); // there is some disagreement about whether this is necessary, but ideally performance-conscious users will use FBOs anyway
	GLint oldPackAlignment;
	glGetIntegerv( GL_PACK_ALIGNMENT, &oldPackAlignment );
	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glReadPixels( area.x1, windowHeightPixels - area.y2, area.getWidth(), area.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, s.getData() );
	glPixelStorei( GL_PACK_ALIGNMENT, oldPackAlignment );
	ip::flipVertical( &s );
	return s;
}

#elif defined( CINDER_ANDROID )
RendererGl::~RendererGl()
{
	delete mImpl;
}

void RendererGl::setup( ANativeWindow *nativeWindow, RendererRef sharedRenderer )
{
	if( ! mImpl ) {
		mImpl = new RendererGlAndroid( this );
	}

	if( ! mImpl->initialize( nativeWindow, sharedRenderer ) ) {
		throw ExcRendererAllocation( "RendererGlAndroid initialization failed." );
	}
}

void RendererGl::startDraw()
{
	if( mStartDrawFn )
		mStartDrawFn( this );
	else
		mImpl->makeCurrentContext( false );
}

void RendererGl::makeCurrentContext( bool force )
{
	mImpl->makeCurrentContext( force );
}

void RendererGl::swapBuffers()
{
	mImpl->swapBuffers();
}

void RendererGl::finishDraw()
{
	if( mFinishDrawFn )
		mFinishDrawFn( this );
	else
		mImpl->swapBuffers();
}

void RendererGl::defaultResize()
{
	mImpl->defaultResize();
}

Surface	RendererGl::copyWindowSurface( const Area &area, int32_t windowHeightPixels )
{
	Surface s( area.getWidth(), area.getHeight(), false );
	glFlush(); // there is some disagreement about whether this is necessary, but ideally performance-conscious users will use FBOs anyway
	GLint oldPackAlignment;
	glGetIntegerv( GL_PACK_ALIGNMENT, &oldPackAlignment );
	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glReadPixels( area.x1, windowHeightPixels - area.y2, area.getWidth(), area.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, s.getData() );
	glPixelStorei( GL_PACK_ALIGNMENT, oldPackAlignment );
	ip::flipVertical( &s );
	return s;
}

#elif defined( CINDER_LINUX )
RendererGl::~RendererGl()
{
	delete mImpl;
}

#if defined( CINDER_HEADLESS )
void RendererGl::setup( ci::ivec2 renderSize, RendererRef sharedRenderer )
#else
void RendererGl::setup( void* nativeWindow, RendererRef sharedRenderer )
#endif
{
	if( ! mImpl ) {
		mImpl = new RendererGlLinux( this );
	}

#if defined( CINDER_HEADLESS )
	if( ! mImpl->initialize( renderSize, sharedRenderer ) ) {
#else
	if( ! mImpl->initialize( nativeWindow, sharedRenderer ) ) {
#endif
		throw ExcRendererAllocation( "RendererGlLinux initialization failed." );
	}
}

void RendererGl::startDraw()
{
	if( mStartDrawFn )
		mStartDrawFn( this );
	else
		mImpl->makeCurrentContext( false );
}

void RendererGl::makeCurrentContext( bool force )
{
	mImpl->makeCurrentContext( force );
}

void RendererGl::swapBuffers()
{
	mImpl->swapBuffers();
}

void RendererGl::finishDraw()
{
	if( mFinishDrawFn )
		mFinishDrawFn( this );
	else
		mImpl->swapBuffers();
}

void RendererGl::defaultResize()
{
	mImpl->defaultResize();
}

Surface	RendererGl::copyWindowSurface( const Area &area, int32_t windowHeightPixels )
{
	Surface s( area.getWidth(), area.getHeight(), false );
	glFlush(); // there is some disagreement about whether this is necessary, but ideally performance-conscious users will use FBOs anyway
	GLint oldPackAlignment;
	glGetIntegerv( GL_PACK_ALIGNMENT, &oldPackAlignment );
	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glReadPixels( area.x1, windowHeightPixels - area.y2, area.getWidth(), area.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, s.getData() );
	glPixelStorei( GL_PACK_ALIGNMENT, oldPackAlignment );
	ip::flipVertical( &s );
	return s;
}
#endif

} } // namespace cinder::app
