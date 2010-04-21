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

#include "cinder/ip/Flip.h"

namespace cinder { namespace ip {

template<typename T>
void flipVertical( SurfaceT<T> *surface )
{
	int32_t rowBytes = surface->getRowBytes();
	uint8_t *buffer = new uint8_t[rowBytes];
	
	int32_t lastRow = surface->getHeight() - 1;
	int32_t halfHeight = surface->getHeight() / 2;
	for( int32_t y = 0; y < halfHeight; ++y ) {
		memcpy( buffer, surface->getData( Vec2i( 0, y ) ), rowBytes );
		memcpy( surface->getData( Vec2i( 0, y ) ), surface->getData( Vec2i( 0, lastRow - y ) ), rowBytes );
		memcpy( surface->getData( Vec2i( 0, lastRow - y ) ), buffer, rowBytes );
	}	
	
	delete [] buffer;
}


#define flip_PROTOTYPES(r,data,T)\
	template void flipVertical<T>( SurfaceT<T> *surface );

BOOST_PP_SEQ_FOR_EACH( flip_PROTOTYPES, ~, CHANNEL_TYPES )

} } // namespace cinder::ip
