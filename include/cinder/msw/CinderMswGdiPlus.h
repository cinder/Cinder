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
 ANY DIRECT, INDIRECT, cinder::, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "cinder/Surface.h"
#include <windows.h>

// forward declarations
namespace Gdiplus {
	class Bitmap;
	typedef INT PixelFormat;
}

namespace cinder { namespace msw {

/** Copies the contents of a Gdiplus::Bitmap. \a premultiplied determines whether the result should be premultiplied by alpha **/
Surface8u convertGdiplusBitmap( Gdiplus::Bitmap &bitmap, bool premultiplied );

/** \brief Translates a cinder::SurfaceChannelOrder into a Gdiplus::PixelFormat 
	Supports BGR, BGRX, BGRA. Returns PixelFormatUndefined on failure **/
Gdiplus::PixelFormat surfaceChannelOrderToGdiplusPixelFormat( const SurfaceChannelOrder &sco, bool premultiplied );

/** Creates a Gdiplus::Bitmap which wraps a cinder::Surface8u.
	Requires \a surface to confrom to SurfaceConstraintsNative and throw SurfaceConstraintsExc if it does not. Caller is responsible for deleting the result.**/
Gdiplus::Bitmap* createGdiplusBitmap( const Surface8u &surface, bool premultiplied );

} } // namespace cinder::msw
