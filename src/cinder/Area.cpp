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
#include "cinder/Rect.h"
#include <algorithm>

using std::pair;

namespace cinder {
Area::Area( const Vec2i &UL, const Vec2i &LR )
{
	set( UL.x, UL.y, LR.x, LR.y );
}

Area::Area( const RectT<float> &r )
{
	set( (int32_t)r.x1, (int32_t)r.y1, (int32_t)r.x2, (int32_t)r.y2 );
}

void Area::set( int32_t aX1, int32_t aY1, int32_t aX2, int32_t aY2 )
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

void Area::clipBy( const Area &clip )
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

Area Area::getClipBy( const Area &clip ) const
{
	Area result( *this );
	result.clipBy( clip );
	return result;
}

void Area::offset( const Vec2i &o )
{
	x1 += o.x;
	x2 += o.x;
	y1 += o.y;
	y2 += o.y;
}

Area Area::getOffset( const Vec2i &offset ) const
{
	return Area( x1 + offset.x, y1 + offset.y, x2 + offset.x, y2 + offset.y );
}

void Area::moveULTo( const Vec2i &newUL )
{
	set( newUL.x, newUL.y, newUL.x + getWidth(), newUL.y + getHeight() );
}

Area Area::getMoveULTo( const Vec2i &newUL ) const
{
	return Area( newUL.x, newUL.y, newUL.x + getWidth(), newUL.y + getHeight() );
}

bool Area::contains( const Vec2i &offset ) const
{
	return ( ( offset.x >= x1 ) && ( offset.x < x2 ) && ( offset.y >= y1 ) && ( offset.y < y2 ) );
}

bool Area::intersects( const Area &area ) const
{
	if( ( x1 >= area.x2 ) || ( x2 < area.x1 ) || ( y1 >= area.y2 ) || ( y2 < area.y1 ) )
		return false;
	else
		return true;
}

void Area::include( const Vec2i &point )
{
	if( x1 > point.x ) x1 = point.x;
	if( x2 < point.x ) x2 = point.x;
	if( y1 > point.y ) y1 = point.y;
	if( y2 < point.y ) y2 = point.y;
}

void Area::include( const std::vector<Vec2i > &points )
{
	for( size_t s = 0; s < points.size(); ++s )
		include( points[s] );
}

void Area::include( const Area &area )
{
	include( Vec2i( area.x1, area.y1 ) );
	include( Vec2i( area.x2, area.y2 ) );
}

template<typename Y>
float Area::distance( const Vec2<Y> &pt ) const
{
	float squaredDistance = 0;
	if( pt.x < x1 ) squaredDistance += ( x1 - pt.x ) * ( x1 - pt.x );
	else if( pt.x > x2 ) squaredDistance += ( pt.x - x2 ) * ( pt.x - x2 );
	if( pt.y < y1 ) squaredDistance += ( y1 - pt.y ) * ( y1 - pt.y );
	else if( pt.y > y2 ) squaredDistance += ( pt.y - y2 ) * ( pt.y - y2 );
	
	if( squaredDistance > 0 )
		return math<float>::sqrt( squaredDistance );
	else
		return 0;
}

template<typename Y>
float Area::distanceSquared( const Vec2<Y> &pt ) const
{
	float squaredDistance = 0;
	if( pt.x < x1 ) squaredDistance += ( x1 - pt.x ) * ( x1 - pt.x );
	else if( pt.x > x2 ) squaredDistance += ( pt.x - x2 ) * ( pt.x - x2 );
	if( pt.y < y1 ) squaredDistance += ( y1 - pt.y ) * ( y1 - pt.y );
	else if( pt.y > y2 ) squaredDistance += ( pt.y - y2 ) * ( pt.y - y2 );
	
	return squaredDistance;
}

template<typename Y>
Vec2<Y>	Area::closestPoint( const Vec2<Y> &pt ) const
{
	Vec2<Y> result = pt;
	if( pt.x < (Y)x1 ) result.x = (Y)x1;
	else if( pt.x > (Y)x2 ) result.x = (Y)x2;
	if( pt.y < (Y)y1 ) result.y = (Y)y1;
	else if( pt.y > (Y)y2 ) result.y = (Y)y2;
	return result;
}

bool Area::operator<( const Area &aArea ) const
{
	if ( x1 != aArea.x1 ) return x1 < aArea.x1;
	if ( y1 != aArea.y1 ) return y1 < aArea.y1;
	if ( x2 != aArea.x2 ) return x2 < aArea.x2;
	if ( y2 != aArea.y2 ) return y2 < aArea.y2;
	
	return false;
}

Area Area::proportionalFit( const Area &srcArea, const Area &dstArea, bool center, bool expand )
{
	int32_t resultWidth, resultHeight;
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
	
	Area resultArea( 0, 0, resultWidth, resultHeight );
	if ( center )
		resultArea.offset( Vec2i( ( dstArea.getWidth() - resultWidth ) / 2, ( dstArea.getHeight() - resultHeight ) / 2 ) );
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

template float Area::distance( const Vec2i &pt ) const;
template float Area::distance( const Vec2f &pt ) const;
template float Area::distanceSquared( const Vec2i &pt ) const;
template float Area::distanceSquared( const Vec2f &pt ) const;
template Vec2i Area::closestPoint( const Vec2i &pt ) const;
template Vec2f Area::closestPoint( const Vec2f &pt ) const;

} // namespace cinder
