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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Surface.h"

namespace cinder { namespace ip {

//! Flips the contents of \a surface vertically (bottom becomes top)
template<typename T>
CI_API void flipVertical( SurfaceT<T> *surface );

//! Flips the contents of \a channel vertically (bottom becomes top)
template<typename T>
CI_API void flipVertical( ChannelT<T> *channel );

//! Copies the contents of \a srcSurface into \a destSurface, flipping them vertically
template<typename T>
CI_API void flipVertical( const SurfaceT<T> &srcSurface, SurfaceT<T> *destSurface );

//! Copies the contents of \a srcChannel into \a destChannel, flipping them vertically
template<typename T>
CI_API void flipVertical( const ChannelT<T> &srcChannel, ChannelT<T> *destChannel );

//! Flips the contents of \a surface horizontally (left becomes right)
template<typename T>
CI_API void flipHorizontal( SurfaceT<T> *surface );

//! Flips the contents of \a channel horizontally (left becomes right)
template<typename T>
CI_API void flipHorizontal( ChannelT<T> *channel );

//! Flips the contents of \a surface both vertically and horizontally (left becomes right, bottom becomes top)
template<typename T>
CI_API void flipDiagonal( SurfaceT<T> *surface );
    
//! Flips the contents of \a channel both vertically and horizontally (left becomes right, bottom becomes top)
template<typename T>
CI_API void flipDiagonal( ChannelT<T> *channel );

} } // namespace cinder::ip
