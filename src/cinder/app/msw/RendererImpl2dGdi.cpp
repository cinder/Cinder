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

#include "cinder/app/msw/RendererImpl2dGdi.h"
#include "cinder/app/AppBase.h"
#include "cinder/app/msw/AppImplMsw.h"
#include "cinder/msw/CinderMsw.h"
#include "cinder/msw/CinderMswGdiPlus.h"
#include "cinder/Log.h"
#include <windowsx.h>

namespace cinder { namespace app {

RendererImpl2dGdi::RendererImpl2dGdi( bool doubleBuffer, bool paintEvents )
	: mDoubleBuffer( doubleBuffer ), mPaintEvents( paintEvents ), mDoubleBufferBitmap( 0 )
{
}

void RendererImpl2dGdi::defaultResize() const
{}

void RendererImpl2dGdi::swapBuffers() const
{
	if( mDoubleBuffer ) {
		::BitBlt( mPaintDc, 0, 0, mDoubleBufferBitmapSize.x, mDoubleBufferBitmapSize.y, mDoubleBufferDc, 0, 0, SRCCOPY );

		// Always select the old bitmap back into the device context
		::SelectObject( mDoubleBufferDc, mDoubleBufferOldBitmap );
		::DeleteDC( mDoubleBufferDc );		
	}

	if( mPaintEvents )
		::EndPaint( mWindowImpl->getHwnd(), &mPaintStruct );
	else
		::ReleaseDC( mWindowImpl->getHwnd(), mPaintDc );
}

void RendererImpl2dGdi::makeCurrentContext( bool /*force*/ )
{
	if( mPaintEvents )
		mPaintDc = ::BeginPaint( mWindowImpl->getHwnd(), &mPaintStruct );
	else
		mPaintDc = ::GetDC( mWindowImpl->getHwnd() );
	
	if( mDoubleBuffer ) {
		::RECT clientRect;
		::GetClientRect( mWindowImpl->getHwnd(), &clientRect );
		ivec2 windowSize( clientRect.right - clientRect.left, clientRect.bottom - clientRect.top );

		mDoubleBufferDc = ::CreateCompatibleDC( mPaintDc );
		if(( mDoubleBufferBitmap == 0 ) || ( mDoubleBufferBitmapSize != windowSize )) {
			if( mDoubleBufferBitmap ) {
				::DeleteObject( mDoubleBufferBitmap );
			}
			
			mDoubleBufferBitmap = ::CreateCompatibleBitmap( mPaintDc, windowSize.x, windowSize.y );
			mDoubleBufferBitmapSize = windowSize;
		}
		mDoubleBufferOldBitmap = (::HBITMAP)::SelectObject( mDoubleBufferDc, mDoubleBufferBitmap );
	}
}

bool RendererImpl2dGdi::initialize( WindowImplMsw *windowImpl, RendererRef /*sharedRenderer*/ )
{
	mWindowImpl = windowImpl;
	return true;
}

Surface8u RendererImpl2dGdi::copyWindowContents( const Area &area )
{
	// Warning - if you step through this code with a debugger, the image returned
	// will be of the foreground window (presumably your IDE) instead
	::RECT clientRect;
	::GetClientRect( mWindowImpl->getHwnd(), &clientRect );
	Area clippedArea = area.getClipBy( Area( clientRect.left, clientRect.top, clientRect.right, clientRect.bottom ) );
	::HDC hdc = ::GetDC( mWindowImpl->getHwnd() );
	::HDC memDC = ::CreateCompatibleDC( hdc );

	::HBITMAP copyBitmap = ::CreateCompatibleBitmap( hdc, clippedArea.getWidth(), clippedArea.getHeight() );
	if( ! copyBitmap ) {
		CI_LOG_E( "::CreateCompatibleBitmap() failed on copyWindowContents()" );
		return Surface8u();
	}

	::GdiFlush();

	// Copy the bits to our bitmap
	::HGDIOBJ oldBitmap = ::SelectObject( memDC, copyBitmap );
	if( ! ::BitBlt( memDC, 0, 0, clippedArea.getWidth(), clippedArea.getHeight(), hdc, clippedArea.x1, clippedArea.y1, SRCCOPY ) ){
		CI_LOG_E( "BitBlt() failed on copyWindowContents()" );
		return Surface8u();
	}
	::SelectObject( memDC, oldBitmap );
	
	auto tempSurface = msw::convertHBitmap( copyBitmap );
	Surface8u result( *tempSurface );

	::DeleteObject( copyBitmap );

	::ReleaseDC( NULL, hdc );
	::DeleteDC( memDC );

	return result;
}

} } // namespace cinder::app
