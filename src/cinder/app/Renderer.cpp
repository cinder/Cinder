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

#if !defined( CINDER_WINRT)
	#include "cinder/gl/gl.h"
#endif

#include "cinder/app/App.h"

#if defined( CINDER_COCOA )
	#include "cinder/cocoa/CinderCocoa.h"
	#if defined( CINDER_MAC )
		#import <Cocoa/Cocoa.h>
		#import "cinder/app/AppImplCocoaRendererGl.h"
		#import "cinder/app/AppImplCocoaRendererQuartz.h"
	#elif defined( CINDER_COCOA_TOUCH )
		#include "cinder/cocoa/CinderCocoaTouch.h"
		#import "cinder/app/AppImplCocoaTouchRendererGl.h"
		#import "cinder/app/AppImplCocoaTouchRendererQuartz.h"		
	#endif

#elif defined( CINDER_MSW )
	#include "cinder/app/AppImplMsw.h"
	#include "cinder/app/AppImplMswRendererGl.h"
	#include "cinder/app/AppImplMswRendererGdi.h"
#endif
#include "cinder/ip/Flip.h"


namespace cinder { namespace app {

const int RendererGl::sAntiAliasingSamples[] = { 0, 2, 4, 6, 8, 16, 32 };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Renderer
Renderer::Renderer( const Renderer &renderer )
	: mApp( renderer.mApp )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RendererGl
RendererGl::RendererGl( int aAntiAliasing )
	: Renderer(), mImpl( 0 ), mAntiAliasing( aAntiAliasing )
{}

RendererGl::RendererGl( const RendererGl &renderer )
	: Renderer( renderer ), mAntiAliasing( renderer.mAntiAliasing )
{
#if defined( CINDER_COCOA )
	mImpl = 0;
#elif defined( CINDER_MSW )
	mImpl = 0;
	mWnd = renderer.mWnd;
#endif
}

void RendererGl::setAntiAliasing( int aAntiAliasing )
{
	mAntiAliasing = aAntiAliasing;
}

#if defined( CINDER_MAC )
RendererGl::~RendererGl()
{
	if( mImpl )
		::CFRelease( mImpl );
}

void RendererGl::setup( App *aApp, CGRect frame, NSView *cinderView, RendererRef sharedRenderer, bool retinaEnabled )
{
	mApp = aApp;
	RendererGlRef sharedGl = std::dynamic_pointer_cast<RendererGl>( sharedRenderer );
	mImpl = [[AppImplCocoaRendererGl alloc] initWithFrame:NSRectFromCGRect(frame) cinderView:cinderView app:mApp renderer:this sharedRenderer:sharedGl withRetina:retinaEnabled];
	// This is necessary for Objective-C garbage collection to do the right thing
	::CFRetain( mImpl );
}

void RendererGl::startDraw()
{
	[mImpl makeCurrentContext];
}

void RendererGl::finishDraw()
{
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

Surface RendererGl::copyWindowSurface( const Area &area )
{
	Surface s( area.getWidth(), area.getHeight(), false );
	glFlush(); // there is some disagreement about whether this is necessary, but ideally performance-conscious users will use FBOs anyway
	GLint oldPackAlignment;
	glGetIntegerv( GL_PACK_ALIGNMENT, &oldPackAlignment ); 
	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glReadPixels( area.x1, mApp->getWindow()->toPixels( mApp->getWindowHeight() ) - area.y2, area.getWidth(), area.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, s.getData() );
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

void RendererGl::makeCurrentContext()
{
	[mImpl makeCurrentContext];
}

#elif defined( CINDER_COCOA_TOUCH ) 
RendererGl::~RendererGl()
{
}

void RendererGl::setup( App *aApp, const Area &frame, UIView *cinderView, RendererRef sharedRenderer )
{
	mApp = aApp;

	RendererGlRef sharedRendererGl = std::dynamic_pointer_cast<RendererGl>( sharedRenderer );
	mImpl = [[AppImplCocoaTouchRendererGl alloc] initWithFrame:cocoa::createCgRect( frame ) cinderView:(UIView*)cinderView app:mApp renderer:this sharedRenderer:sharedRendererGl];
}

EAGLContext* RendererGl::getEaglContext() const
{
	return [mImpl getEaglContext];
}

void RendererGl::startDraw()
{
	[mImpl makeCurrentContext];
}

void RendererGl::finishDraw()
{
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

void RendererGl::makeCurrentContext()
{
	[mImpl makeCurrentContext];
}

Surface	RendererGl::copyWindowSurface( const Area &area )
{
	Surface s( area.getWidth(), area.getHeight(), true );
	glFlush(); // there is some disagreement about whether this is necessary, but ideally performance-conscious users will use FBOs anyway
	GLint oldPackAlignment;
	glGetIntegerv( GL_PACK_ALIGNMENT, &oldPackAlignment ); 
	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glReadPixels( area.x1, mApp->getWindow()->toPixels( mApp->getWindowHeight() ) - area.y2, area.getWidth(), area.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, s.getData() );
	glPixelStorei( GL_PACK_ALIGNMENT, oldPackAlignment );	
	ip::flipVertical( &s );

	return s;
}

#elif defined( CINDER_MSW )
RendererGl::~RendererGl()
{
	delete mImpl;
}

void RendererGl::setup( App *aApp, HWND wnd, HDC dc, RendererRef sharedRenderer )
{
	mWnd = wnd;
	mApp = aApp;
	if( ! mImpl )
		mImpl = new AppImplMswRendererGl( mApp, this );

	mImpl->initialize( wnd, dc, sharedRenderer );
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
	mImpl->makeCurrentContext();
}

void RendererGl::makeCurrentContext()
{
	mImpl->makeCurrentContext();
}

void RendererGl::finishDraw()
{
	mImpl->swapBuffers();
}

void RendererGl::defaultResize()
{
	mImpl->defaultResize();
}

Surface	RendererGl::copyWindowSurface( const Area &area )
{
	Surface s( area.getWidth(), area.getHeight(), false );
	glFlush(); // there is some disagreement about whether this is necessary, but ideally performance-conscious users will use FBOs anyway
	GLint oldPackAlignment;
	glGetIntegerv( GL_PACK_ALIGNMENT, &oldPackAlignment ); 
	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glReadPixels( area.x1, mApp->getWindow()->toPixels( mApp->getWindowHeight() ) - area.y2, area.getWidth(), area.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, s.getData() );
	glPixelStorei( GL_PACK_ALIGNMENT, oldPackAlignment );	
	ip::flipVertical( &s );
	return s;
}

#endif // 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Renderer2d
Renderer2d::Renderer2d( const Renderer2d &renderer )
	: Renderer( renderer )
{
	mImpl = 0;
#if defined( CINDER_MSW )
	mDoubleBuffer = renderer.mDoubleBuffer;
#endif
}

#if defined( CINDER_COCOA )

Renderer2d::Renderer2d()
	: Renderer(), mImpl( 0 )
{
}

#if defined( CINDER_MAC )
Renderer2d::~Renderer2d()
{
	::CFRelease( mImpl );
}

void Renderer2d::setup( App *aApp, CGRect frame, NSView *cinderView, RendererRef /*sharedRenderer*/, bool retinaEnabled )
{
	mApp = aApp;
	mImpl = [[AppImplCocoaRendererQuartz alloc] initWithFrame:NSRectFromCGRect(frame) cinderView:cinderView app:mApp];
	// This is necessary for Objective-C garbage collection to do the right thing
	::CFRetain( mImpl );
}

#else

void Renderer2d::setup( App *aApp, const Area &frame, UIView *cinderView, RendererRef /*sharedRenderer*/ )
{
	mApp = aApp;
	mImpl = [[AppImplCocoaTouchRendererQuartz alloc] initWithFrame:cinder::cocoa::createCgRect(frame) cinderView:cinderView app:mApp];
}

#endif

CGContextRef Renderer2d::getCgContext()
{
	return [mImpl getCGContextRef];
}

void Renderer2d::startDraw()
{
	[mImpl makeCurrentContext];
}

void Renderer2d::finishDraw()
{
	[mImpl flushBuffer];
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

void Renderer2d::makeCurrentContext()
{
	[mImpl makeCurrentContext];
}

Surface Renderer2d::copyWindowSurface( const Area &area )
{
#if defined( CINDER_MAC )
	NSBitmapImageRep *rep = [mImpl getContents:area];
	if( rep )
		return cocoa::convertNsBitmapDataRep( rep, true );
	else
		return Surface();
#elif defined( CINDER_COCOA_TOUCH )
	UIImage *image = [mImpl getContents:area];
	if( image )
		return cocoa::convertUiImage( image, true );
	else
		return Surface();
#endif
}
#endif

#if defined( CINDER_MSW )

Renderer2d::Renderer2d( bool doubleBuffer )
	: Renderer(), mDoubleBuffer( doubleBuffer )
{
}

void Renderer2d::setup( App *app, HWND wnd, HDC dc, RendererRef /*sharedRenderer*/ )
{
	mApp = app;
	mWnd = wnd;
	mImpl = new AppImplMswRendererGdi( app, mDoubleBuffer );
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

Surface	Renderer2d::copyWindowSurface( const Area &area )
{
	return mImpl->copyWindowContents( area );
}

#endif // defined( CINDER_MSW )

} } // namespace cinder::app
