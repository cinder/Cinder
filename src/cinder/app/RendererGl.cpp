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

#if defined( CINDER_COCOA )
	#if defined( CINDER_MAC )
		#import "cinder/app/AppImplCocoaRendererGl.h"
	#elif defined( CINDER_COCOA_TOUCH )
		#import "cinder/app/AppImplCocoaTouchRendererGl.h"
	#endif
#elif defined( CINDER_MSW )
	#if defined( CINDER_GL_ANGLE )
		#include "cinder/app/AppImplMswRendererAngle.h"
	#else
		#include "cinder/app/AppImplMswRendererGl.h"
	#endif
#endif

namespace cinder { namespace app {

const int RendererGl::sAntiAliasingSamples[] = { 0, 2, 4, 6, 8, 16, 32 };

RendererGl::RendererGl( const RendererGl::Options &options )
	: Renderer(), mImpl( 0 ), mOptions( options )
{}

RendererGl::RendererGl( const RendererGl &renderer )
	: Renderer( renderer ), mOptions( renderer.mOptions )
{
#if defined( CINDER_COCOA )
	mImpl = 0;
#elif defined( CINDER_MSW )
	mImpl = 0;
	mWnd = renderer.mWnd;
#endif
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
#if defined( CINDER_GL_ANGLE )
		mImpl = new AppImplMswRendererAngle( mApp, this );
#else
		mImpl = new AppImplMswRendererGl( mApp, this );
#endif
	if( ! mImpl->initialize( wnd, dc, sharedRenderer ) )
		throw ExcRendererAllocation();
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
#endif

} } // namespace cinder::app