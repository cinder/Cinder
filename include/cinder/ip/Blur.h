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

#include "cinder/Surface.h"

namespace cinder { namespace ip {

//! Blur \a surface in-place using "stackBlur", a Gaussian-approximating algorithm by Mario Klingemann.
CI_API void			stackBlur( Surface8u *surface, int radius );
//! Blur \a surface in-place in \a area using "stackBlur", a Gaussian-approximating algorithm by Mario Klingemann.
CI_API void			stackBlur( Surface8u *surface, const Area &area, int radius );
//! Create a blurred copy of \a surface using "stackBlur", a Gaussian-approximating algorithm by Mario Klingemann.
CI_API Surface8u	stackBlurCopy( const Surface8u &surface, int radius );

//! Blur \a channel in-place using "stackBlur", a Gaussian-approximating algorithm by Mario Klingemann.
CI_API void			stackBlur( Channel8u *channel, int radius );
//! Blur \a channel in-place in \a area using "stackBlur", a Gaussian-approximating algorithm by Mario Klingemann.
CI_API void			stackBlur( Channel8u *channel, const Area &area, int radius );
//! Create a blurred copy of \a channel using "stackBlur", a Gaussian-approximating algorithm by Mario Klingemann.
CI_API Channel8u	stackBlurCopy( const Channel8u &channel, int radius );

//! Blur \a surface in-place using "stackBlur", a Gaussian-approximating algorithm by Mario Klingemann.
CI_API void			stackBlur( Surface16u *surface, int radius );
//! Blur \a surface in-place in \a area using "stackBlur", a Gaussian-approximating algorithm by Mario Klingemann.
CI_API void			stackBlur( Surface16u *surface, const Area &area, int radius );
//! Create a blurred copy of \a surface using "stackBlur", a Gaussian-approximating algorithm by Mario Klingemann.
CI_API Surface16u	stackBlurCopy( const Surface16u &surface, int radius );

//! Blur \a channel in-place using "stackBlur", a Gaussian-approximating algorithm by Mario Klingemann.
CI_API void			stackBlur( Channel16u *channel, int radius );
//! Blur \a channel in-place in \a area using "stackBlur", a Gaussian-approximating algorithm by Mario Klingemann.
CI_API void			stackBlur( Channel16u *channel, const Area &area, int radius );
//! Create a blurred copy of \a channel using "stackBlur", a Gaussian-approximating algorithm by Mario Klingemann.
CI_API Channel16u	stackBlurCopy( const Channel16u &channel, int radius );

//! Blur \a surface in-place using "stackBlur", a Gaussian-approximating algorithm by Mario Klingemann.
CI_API void			stackBlur( Surface32f *surface, int radius );
//! Blur \a surface in-place in \a area using "stackBlur", a Gaussian-approximating algorithm by Mario Klingemann.
CI_API void			stackBlur( Surface32f *surface, const Area &area, int radius );
//! Create a blurred copy of \a surface using "stackBlur", a Gaussian-approximating algorithm by Mario Klingemann.
CI_API Surface32f	stackBlurCopy( const Surface32f &surface, int radius );

//! Blur \a channel in-place using "stackBlur", a Gaussian-approximating algorithm by Mario Klingemann.
CI_API void			stackBlur( Channel32f *channel, int radius );
//! Blur \a channel in-place in \a area using "stackBlur", a Gaussian-approximating algorithm by Mario Klingemann.
CI_API void			stackBlur( Channel32f *channel, const Area &area, int radius );
//! Create a blurred copy of \a channel using "stackBlur", a Gaussian-approximating algorithm by Mario Klingemann.
CI_API Channel32f	stackBlurCopy( const Channel32f &channel, int radius );

} } // namespace cinder::ip
