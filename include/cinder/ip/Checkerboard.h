/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.
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

#pragma once

#include "cinder/Surface.h"
#include "cinder/Color.h"

namespace cinder { namespace ip {

CI_API Surface8u	checkerboard( int32_t width, int32_t height, int32_t tileSize = 16, const Color8u &evenColor = Color8u( 0, 0, 0 ), const Color8u &oddColor = Color8u( 255, 255, 255 ) );
CI_API Surface16u	checkerboard16u( int32_t width, int32_t height, int32_t tileSize = 16, const ColorT<uint16_t> &evenColor = ColorT<uint16_t>( 0, 0, 0 ), const ColorT<uint16_t> &oddColor = ColorT<uint16_t>( 65535, 65535, 65535 ) );
CI_API Surface32f	checkerboard32f( int32_t width, int32_t height, int32_t tileSize = 16, const Colorf &evenColor = Colorf( 0, 0, 0 ), const Colorf &oddColor = Colorf( 1, 1, 1 ) );

CI_API void	checkerboard( Surface8u *surface, const Area &area, int32_t tileSize = 16, const ColorA8u &evenColor = ColorA8u( 0, 0, 0, 255 ), const ColorA8u &oddColor = ColorA8u( 255, 255, 255, 255 ) );
CI_API void	checkerboard( Surface16u *surface, const Area &area, int32_t tileSize = 16, const ColorAT<uint16_t> &evenColor = ColorAT<uint16_t>( 0, 0, 0, 65535 ), const ColorAT<uint16_t> &oddColor = ColorAT<uint16_t>( 65535, 65535, 65535, 65535 ) );
CI_API void	checkerboard( Surface32f *surface, const Area &area, int32_t tileSize = 16, const ColorAf &evenColor = ColorAf( 0, 0, 0, 1 ), const ColorAf &oddColor = ColorAf( 1, 1, 1, 1 ) );

} } // namespace cinder::ip
