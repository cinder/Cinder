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

#include "cinder/app/AppImplMswRendererGdi.h"
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"
#include "cinder/msw/CinderMsw.h"
#include <windowsx.h>

namespace cinder { namespace app {

AppImplMswRendererGdi::AppImplMswRendererGdi( App *aApp, bool doubleBuffer )
	: AppImplMswRenderer( aApp ), mDoubleBuffer( doubleBuffer ), mDoubleBufferBitmap( 0 )
{
}

void AppImplMswRendererGdi::defaultResize() const
{}

void AppImplMswRendererGdi::swapBuffers() const
{
	if( mDoubleBuffer ) {
		::BitBlt( mPaintDc, 0, 0, mDoubleBufferBitmapSize.x, mDoubleBufferBitmapSize.y, mDoubleBufferDc, 0, 0, SRCCOPY );

		// Always select the old bitmap back into the device context
		::SelectObject( mDoubleBufferDc, mDoubleBufferOldBitmap );
		::DeleteDC( mDoubleBufferDc );		
	}

	if( mApp->getsWindowsPaintEvents() )
		::EndPaint( mWnd, &mPaintStruct );
	else
		::ReleaseDC( mWnd, mPaintDc );
}

void AppImplMswRendererGdi::makeCurrentContext()
{
	if( mApp->getsWindowsPaintEvents() )
		mPaintDc = ::BeginPaint( mWnd, &mPaintStruct );
	else
		mPaintDc = ::GetDC( mWnd );
	
	if( mDoubleBuffer ) {
		::RECT clientRect;
		::GetClientRect( mWnd, &clientRect );
		Vec2i windowSize( clientRect.right - clientRect.left, clientRect.bottom - clientRect.top );

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

bool AppImplMswRendererGdi::initialize( HWND wnd, HDC dc )
{
	mWnd = wnd;
	return true;
}

Surface8u AppImplMswRendererGdi::copyWindowContents( const Area &area )
{
	Surface8u result;

	// Warning - if you step through this code with a debugger, the image returned
	// will be of the foreground window (presumably your IDE) instead
	::RECT clientRect;
	::GetClientRect( mWnd, &clientRect );
	Area clippedArea = area.getClipBy( Area( clientRect.left, clientRect.top, clientRect.right, clientRect.bottom ) );
	::HDC hdc = ::GetDC( mWnd );
	::HDC memDC = ::CreateCompatibleDC( hdc );

	::HBITMAP copyBitmap = ::CreateCompatibleBitmap( hdc, clippedArea.getWidth(), clippedArea.getHeight() );
	if( ! copyBitmap )
		return result;
	
	::GdiFlush();

	// Copy the bits to our bitmap
	::HGDIOBJ oldBitmap = ::SelectObject( memDC, copyBitmap );
	if( ! ::BitBlt( memDC, 0, 0, clippedArea.getWidth(), clippedArea.getHeight(), hdc, clippedArea.x1, clippedArea.y1, SRCCOPY ) )
		return result;
	::SelectObject( memDC, oldBitmap );
	
	result = msw::convertHBitmap( copyBitmap );
	
	::DeleteObject( copyBitmap );

	::ReleaseDC( NULL, hdc );
	::DeleteDC( memDC );

	return result;
}

} } // namespace cinder::app
