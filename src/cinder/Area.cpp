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

#include "cinder/Area.h"
#include <algorithm>

using std::pair;

namespace cinder {

template<typename T>
AreaT<T>::AreaT( const Vec2<T> &UL, const Vec2<T> &LR )
{
	set( UL.x, UL.y, LR.x, LR.y );
}


template<>
 template<>
AreaT<int32_t>::AreaT( const AreaT<boost::rational<int32_t> > &aAreaBase )
{
	x1 = aAreaBase.x1.numerator() / aAreaBase.x1.denominator();
	y1 = aAreaBase.y1.numerator() / aAreaBase.y1.denominator();
	x2 = aAreaBase.x2.numerator() / aAreaBase.x2.denominator();
	y2 = aAreaBase.y2.numerator() / aAreaBase.y2.denominator();
}

template<>
 template<>
AreaT<boost::rational<int32_t> >::AreaT( const AreaT<int32_t> &aAreaBase )
{
	x1 = aAreaBase.x1;
	y1 = aAreaBase.y1;
	x2 = aAreaBase.x2;
	y2 = aAreaBase.y2;
}

template<typename T>
void AreaT<T>::set( T aX1, T aY1, T aX2, T aY2 )
{
	if ( aX1 <= aX2 ) {
		x1 = aX1;
		x2 = aX2;
	}
	else {
		x1 = aX2;
		x2 = aX1;
	}

	if ( aY1 <= aY2 ) {
		y1 = aY1;
		y2 = aY2;
	}
	else {
		y1 = aY2;
		y2 = aY1;
	}
}

template<typename T>
void AreaT<T>::clipBy( const AreaT<T> &clip )
{
	if ( x1 < clip.x1 )
		x1 = clip.x1;
	if ( x2 < clip.x1 )
		x2 = clip.x1;
	if ( x1 > clip.x2 )
		x1 = clip.x2;
	if ( x2 > clip.x2 )
		x2 = clip.x2;

	if ( y1 < clip.y1 )
		y1 = clip.y1;
	if ( y2 < clip.y1 )
		y2 = clip.y1;
	if ( y1 > clip.y2 )
		y1 = clip.y2;
	if ( y2 > clip.y2 )
		y2 = clip.y2;
}

template<typename T>
AreaT<T> AreaT<T>::getClipBy( const AreaT<T> &clip ) const
{
	AreaT<T> result( *this );
	result.clipBy( clip );
	return result;
}

template<typename T>
void AreaT<T>::offset( const Vec2<T> &o )
{
	x1 += o.x;
	x2 += o.x;
	y1 += o.y;
	y2 += o.y;
}

template<typename T>
AreaT<T> AreaT<T>::getOffset( const Vec2<T> &offset ) const
{
	return AreaT<T>( x1 + offset.x, y1 + offset.y, x2 + offset.x, y2 + offset.y );
}

template<typename T>
void AreaT<T>::moveULTo( const Vec2<T> &newUL )
{
	set( newUL.x, newUL.y, newUL.x + getWidth(), newUL.y + getHeight() );
}

template<typename T>
AreaT<T> AreaT<T>::getMoveULTo( const Vec2<T> &newUL ) const
{
	return AreaT<T>( newUL.x, newUL.y, newUL.x + getWidth(), newUL.y + getHeight() );
}

template<typename T>
bool AreaT<T>::contains( const Vec2<T> &offset ) const
{
	return ( ( offset.x >= x1 ) && ( offset.x < x2 ) && ( offset.y >= y1 ) && ( offset.y < y2 ) );
}

template<typename T>
bool AreaT<T>::intersects( const AreaT<T> &area ) const
{
	if( ( x1 >= area.x2 ) || ( x2 < area.x1 ) || ( y1 >= area.y2 ) || ( y2 < area.y1 ) )
		return false;
	else
		return true;
}

template<typename T>
bool AreaT<T>::operator<( const AreaT<T> &aArea ) const
{
	if ( x1 != aArea.x1 ) return x1 < x1;
	if ( y1 != aArea.y1 ) return y1 < y1;
	if ( x2 != aArea.x2 ) return x2 < x2;
	if ( y2 != aArea.y2 ) return y2 < y2;
	
	return false;
}


template<typename T>
AreaT<T> AreaT<T>::proportionalFit( const AreaT<T> &srcArea, const AreaT<T> &dstArea, bool center, bool expand )
{
	T resultWidth, resultHeight;
	if( srcArea.getWidth() >= srcArea.getHeight() ) { // wider than tall
		resultWidth = ( expand ) ? dstArea.getWidth() : std::min( dstArea.getWidth(), srcArea.getWidth() );
		resultHeight = resultWidth * srcArea.getHeight() / srcArea.getWidth();
		if( resultHeight > dstArea.getHeight() ) { // maximized proportional would be too tall
			resultHeight = dstArea.getHeight();
			resultWidth = resultHeight * srcArea.getWidth() / srcArea.getHeight();
		}
	}
	else { // taller than wide
		resultHeight = ( expand ) ? dstArea.getHeight() : std::min( dstArea.getHeight(), srcArea.getHeight() );
		resultWidth = resultHeight * srcArea.getWidth() / srcArea.getHeight();
		if( resultWidth > dstArea.getWidth() ) { // maximized proportional would be too wide
			resultWidth = dstArea.getWidth();
			resultHeight = resultWidth * srcArea.getHeight() / srcArea.getWidth();
		}
	}
	
	AreaT<T> resultArea( 0, 0, resultWidth, resultHeight );
	if ( center )
		resultArea.offset( Vec2<T>( ( dstArea.getWidth() - resultWidth ) / 2, ( dstArea.getHeight() - resultHeight ) / 2 ) );
	resultArea.offset( dstArea.getUL() );
	return resultArea;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*** Returns a pair composed of the source area and the destination absolute offset ***/
pair<Area,Vec2i> clippedSrcDst( const Area &srcSurfaceBounds, const Area &srcArea, const Area &dstSurfaceBounds, const Vec2i &dstLT )
{
	Area clippedSrc = srcArea.getClipBy( srcSurfaceBounds );
	Vec2i newDstLT = dstLT + ( clippedSrc.getUL() - srcArea.getUL() );
	Vec2i oldSrcLT = clippedSrc.getUL();
	clippedSrc.moveULTo( newDstLT );
	Area oldClippedDst = clippedSrc;
	clippedSrc.clipBy( dstSurfaceBounds );
	newDstLT = clippedSrc.getUL();
	clippedSrc.moveULTo( oldSrcLT + ( clippedSrc.getUL() - oldClippedDst.getUL() ) );
	clippedSrc.clipBy( srcSurfaceBounds );
	return std::make_pair( clippedSrc, newDstLT );
}

template class AreaT<int32_t>;
template class AreaT<boost::rational<int32_t> >;

} // namespace cinder
