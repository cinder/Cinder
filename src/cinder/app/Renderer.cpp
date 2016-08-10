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

#include "cinder/app/Renderer.h"

#if !defined( CINDER_UWP )
	#include "cinder/gl/platform.h"
#endif

#include "cinder/app/AppBase.h"

#if defined( CINDER_COCOA )
	#include "cinder/cocoa/CinderCocoa.h"
	#if defined( CINDER_MAC )
		#include <ApplicationServices/ApplicationServices.h>
		#import <Cocoa/Cocoa.h>
		#import "cinder/app/cocoa/RendererImpl2dMacQuartz.h"
	#elif defined( CINDER_COCOA_TOUCH )
		#include "cinder/cocoa/CinderCocoaTouch.h"
		#import "cinder/app/cocoa/RendererImpl2dCocoaTouchQuartz.h"		
	#endif

#elif defined( CINDER_MSW_DESKTOP )
	#include "cinder/app/msw/AppImplMsw.h"
	#include "cinder/app/msw/RendererImpl2dGdi.h"
#elif defined( CINDER_ANDROID )
    #include "cinder/app/android/Renderer2dAndroid.h"
#endif

namespace cinder { namespace app {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Renderer
Renderer::Renderer( const Renderer &renderer )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Renderer2d
#if defined( CINDER_COCOA )

Renderer2d::Renderer2d( const Renderer2d &renderer )
	: Renderer( renderer ), mImpl( nullptr )
{
}

Renderer2d::Renderer2d()
	: Renderer(), mImpl( nullptr )
{
}

#if defined( CINDER_MAC )
Renderer2d::~Renderer2d()
{
	if( mImpl )
		::CFRelease( mImpl );
}

void Renderer2d::setup( CGRect frame, NSView *cinderView, RendererRef /*sharedRenderer*/, bool retinaEnabled )
{
	mImpl = [[RendererImpl2dMacQuartz alloc] initWithFrame:NSRectFromCGRect(frame) cinderView:cinderView];
	// This is necessary for Objective-C garbage collection to do the right thing
	::CFRetain( mImpl );
}

#else

void Renderer2d::setup( const Area &frame, UIView *cinderView, RendererRef /*sharedRenderer*/ )
{
	mImpl = [[RendererImpl2dCocoaTouchQuartz alloc] initWithFrame:cinder::cocoa::createCgRect(frame) cinderView:cinderView];
}

#endif

CGContextRef Renderer2d::getCgContext()
{
	return [mImpl getCGContextRef];
}

void Renderer2d::startDraw()
{
	[mImpl startDraw];
}

void Renderer2d::finishDraw()
{
	[mImpl finishDraw];
}

void Renderer2d::setFrameSize( int width, int height )
{
#if defined( CINDER_MAC )
	[mImpl setFrameSize:NSSizeToCGSize(NSMakeSize( width, height ))];
#endif
}

void Renderer2d::defaultResize()
{
	[mImpl defaultResize];
}

void Renderer2d::makeCurrentContext( bool /*force*/ )
{
}

Surface Renderer2d::copyWindowSurface( const Area &area, int32_t windowHeightPixels )
{
#if defined( CINDER_MAC )
	NSBitmapImageRep *rep = [mImpl getContents:area];
	if( rep )
		return *cocoa::convertNsBitmapDataRep( rep, true );
	else
		return Surface( 0, 0, false );
#elif defined( CINDER_COCOA_TOUCH )
	UIImage *image = [mImpl getContents:area];
	if( image )
		return *cocoa::convertUiImage( image, true );
	else
		return Surface( 0, 0, false );
#endif
}
#endif

#if defined( CINDER_MSW_DESKTOP )

Renderer2d::Renderer2d( const Renderer2d &renderer )
	: Renderer( renderer )
{
	mImpl = 0;
	mDoubleBuffer = renderer.mDoubleBuffer;
}

Renderer2d::Renderer2d( bool doubleBuffer, bool paintEvents )
	: Renderer(), mDoubleBuffer(doubleBuffer), mPaintEvents( paintEvents )
{
}

void Renderer2d::setup( HWND wnd, HDC dc, RendererRef /*sharedRenderer*/ )
{
	mWnd = wnd;
	mImpl = new RendererImpl2dGdi( mDoubleBuffer, mPaintEvents );
	mImpl->initialize( wnd, dc, RendererRef() /* we don't use shared renderers on GDI */ );
}

void Renderer2d::kill()
{
	mImpl->kill();
}

HDC Renderer2d::getDc()
{
	return mImpl->getDc();
}

void Renderer2d::prepareToggleFullScreen()
{
	mImpl->prepareToggleFullScreen();
}

void Renderer2d::finishToggleFullScreen()
{
	mImpl->finishToggleFullScreen();
}

void Renderer2d::startDraw()
{
	mImpl->makeCurrentContext();
}

void Renderer2d::finishDraw()
{
	mImpl->swapBuffers();
}

void Renderer2d::defaultResize()
{
	mImpl->defaultResize();
}

Surface	Renderer2d::copyWindowSurface( const Area &area, int32_t windowHeightPixels )
{
	return mImpl->copyWindowContents( area );
}

#endif // defined( CINDER_MSW_DESKTOP )

} } // namespace cinder::app
