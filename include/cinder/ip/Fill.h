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

#include "cinder/Surface.h"
#include "cinder/Channel.h"
#include "cinder/Area.h"
#include "cinder/Color.h"

namespace cinder { namespace ip {

template<typename T, typename Y>
CI_API void fill( SurfaceT<T> *surface, const ColorT<Y> &color );
template<typename T, typename Y>
CI_API void fill( SurfaceT<T> *surface, const ColorT<Y> &color, const Area &area );
template<typename T, typename Y>
CI_API void fill( SurfaceT<T> *surface, const ColorAT<Y> &color );
template<typename T, typename Y>
CI_API void fill( SurfaceT<T> *surface, const ColorAT<Y> &color, const Area &area );

template<typename T>
CI_API void fill( ChannelT<T> *channel, T value, const Area &area );
template<typename T>
CI_API void fill( ChannelT<T> *channel, T value );

} } // namespace cinder::ip
