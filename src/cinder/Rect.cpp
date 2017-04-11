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

#include "cinder/Rect.h"
#include "cinder/Area.h"

#include <limits>
using std::numeric_limits;

namespace cinder {

template<typename T>
RectT<T>::RectT( const Area &area )
{
	set( (T)area.getX1(), (T)area.getY1(), (T)( area.getX2()), (T)( area.getY2()) );
}

template<typename T>
RectT<T>::RectT( const std::vector<Vec2T> &points )
{
	x1 = numeric_limits<T>::max();
	x2 = -numeric_limits<T>::max();
	y1 = numeric_limits<T>::max();
	y2 = -numeric_limits<T>::max();
	for( size_t s = 0; s < points.size(); ++s )
		include( points[s] );
}

template<typename T>
void RectT<T>::set( T aX1, T aY1, T aX2, T aY2 )
{
	x1 = aX1;
	y1 = aY1;
	x2 = aX2;
	y2 = aY2;
}

template<typename T>
void RectT<T>::canonicalize()
{
	if ( x1 > x2 ) {
		T temp = x1;
		x1 = x2;
		x2 = temp;
	}

	if ( y1 > y2 ) {
		T temp = y1;
		y1 = y2;
		y2 = temp;
	}
}

template<typename T>
RectT<T> RectT<T>::canonicalized() const
{
	RectT<T> result( *this );
	result.canonicalize();
	return result;
}

template<typename T>
void RectT<T>::clipBy( const RectT &clip )
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
RectT<T> RectT<T>::getClipBy( const RectT &clip ) const
{
	RectT<T> result( *this );
	result.clipBy( RectT<T>( clip ) );
	return result;
}

template<typename T>
void RectT<T>::offset( const Vec2T &offset )
{
	x1 += offset.x;
	x2 += offset.x;
	y1 += offset.y;
	y2 += offset.y;
}

template<typename T>
RectT<T> RectT<T>::getOffset( const Vec2T &off ) const
{
	RectT result( *this ); result.offset( off ); return result;
}

template<typename T>
void RectT<T>::moveULTo( const Vec2T &newUL )
{
	set( newUL.x, newUL.y, newUL.x + getWidth(), newUL.y + getHeight() );
}

template<typename T>
RectT<T> RectT<T>::getMoveULTo( const Vec2T &newUL ) const
{
	RectT result( *this );
	result.moveULTo( newUL );
	return result;
}

template<typename T>
void RectT<T>::inflate( const Vec2T &amount )
{
	x1 -= amount.x;
	x2 += amount.x;
	y1 -= amount.y; // assume canonical rect has y1 < y2
	y2 += amount.y;
}

template<typename T>
RectT<T> RectT<T>::inflated( const Vec2T &amount ) const
{
	RectT<T> result( *this );
	result.inflate( amount );
	return result;
}

template<typename T>
void RectT<T>::scaleCentered( const Vec2T &scale )
{
	const T halfWidth = getWidth() * scale.x / 2.0f;
	const T halfHeight = getHeight() * scale.y / 2.0f;
	const auto center = getCenter();
	x1 = center.x - halfWidth;
	x2 = center.x + halfWidth;
	y1 = center.y - halfHeight;
	y2 = center.y + halfHeight;
}

template<typename T>
void RectT<T>::scaleCentered( T scale )
{
	const T halfWidth = getWidth() * scale / 2;
	const T halfHeight = getHeight() * scale / 2;
	const auto center = getCenter();
	x1 = center.x - halfWidth;
	x2 = center.x + halfWidth;
	y1 = center.y - halfHeight;
	y2 = center.y + halfHeight;
}

template<typename T>
RectT<T> RectT<T>::scaledCentered( const Vec2T &scale ) const
{
	const T halfWidth = getWidth() * scale.x / 2;
	const T halfHeight = getHeight() * scale.y / 2;
	const auto center = getCenter();
	return RectT<T>( center.x - halfWidth, center.y - halfHeight, center.x + halfWidth, center.y + halfHeight );
}

template<typename T>
RectT<T> RectT<T>::scaledCentered( T scale ) const
{
	const T halfWidth = getWidth() * scale / 2;
	const T halfHeight = getHeight() * scale / 2;
	const auto center = getCenter();
	return RectT<T>( center.x - halfWidth, center.y - halfHeight, center.x + halfWidth, center.y + halfHeight );
}

template<typename T>
void RectT<T>::scale( T s )
{
	x1 *= s;
	x2 *= s;
	y1 *= s;
	y2 *= s;
}

template<typename T>
void RectT<T>::scale( const Vec2T &scale )
{
	x1 *= scale.x;
	y1 *= scale.y;
	x2 *= scale.x;
	y2 *= scale.y;
}

template<typename T>
RectT<T> RectT<T>::scaled( T s ) const
{
	return RectT<T>( x1 * s, y1 * s, x2 * s, y2 * s );
}

template<typename T>
RectT<T> RectT<T>::scaled( const Vec2T &scale ) const
{
	return RectT<T>( x1 * scale.x, y1 * scale.y, x2 * scale.x, y2 * scale.y );
}

template<typename T>
RectT<T> RectT<T>::transformed( const Mat3T& matrix ) const
{
	Vec2T center = Vec2T( x1 + x2, y1 + y2 ) / (T) 2;
	Vec2T extents = glm::abs( Vec2T( x2, y2 ) - center );

	Vec3T x = matrix * Vec3T( extents.x, 0, 0 );
	Vec3T y = matrix * Vec3T( 0, extents.y, 0 );

	extents = Vec2T( glm::abs( x ) + glm::abs( y ) );
	center = Vec2T( matrix * Vec3T( center, 1 ) );

	return RectT<T>( center.x - extents.x, center.y - extents.y, center.x + extents.x, center.y + extents.y );
}

template<typename T>
void RectT<T>::transform( const Mat3T &matrix )
{
	Vec2T center = Vec2T( x1 + x2, y1 + y2 ) / (T) 2;
	Vec2T extents = glm::abs( Vec2T( x2, y2 ) - center );

	Vec3T x = matrix * Vec3T( extents.x, 0, 0 );
	Vec3T y = matrix * Vec3T( 0, extents.y, 0 );

	extents = Vec2T( glm::abs( x ) + glm::abs( y ) );
	center = Vec2T( matrix * Vec3T( center, 1 ) );

	x1 = center.x - extents.x;
	y1 = center.y - extents.y;
	x2 = center.x + extents.x;
	y2 = center.y + extents.y;
}

template<typename T>
bool RectT<T>::intersects( const RectT<T> &rect ) const
{
	if( ( x1 > rect.x2 ) || ( x2 < rect.x1 ) || ( y1 > rect.y2 ) || ( y2 < rect.y1 ) )
		return false;
	else
		return true;
}

template<typename T>
T RectT<T>::distance( const Vec2T &pt ) const
{
	T squaredDistance = 0;
	if( pt.x < x1 ) squaredDistance += ( x1 - pt.x ) * ( x1 - pt.x );
	else if( pt.x > x2 ) squaredDistance += ( pt.x - x2 ) * ( pt.x - x2 );
	if( pt.y < y1 ) squaredDistance += ( y1 - pt.y ) * ( y1 - pt.y );
	else if( pt.y > y2 ) squaredDistance += ( pt.y - y2 ) * ( pt.y - y2 );
	
	if( squaredDistance > 0 )
		return math<T>::sqrt( squaredDistance );
	else
		return 0;
}

template<typename T>
T RectT<T>::distanceSquared( const Vec2T &pt ) const
{
	T squaredDistance = 0;
	if( pt.x < x1 ) squaredDistance += ( x1 - pt.x ) * ( x1 - pt.x );
	else if( pt.x > x2 ) squaredDistance += ( pt.x - x2 ) * ( pt.x - x2 );
	if( pt.y < y1 ) squaredDistance += ( y1 - pt.y ) * ( y1 - pt.y );
	else if( pt.y > y2 ) squaredDistance += ( pt.y - y2 ) * ( pt.y - y2 );
	
	return squaredDistance;
}

template<typename T>
typename RectT<T>::Vec2T RectT<T>::closestPoint( const Vec2T &pt ) const
{
	auto result = pt;
	if( pt.x < x1 ) result.x = x1;
	else if( pt.x > x2 ) result.x = x2;
	if( pt.y < y1 ) result.y = y1;
	else if( pt.y > y2 ) result.y = y2;
	return result;
}

template<typename T>
void RectT<T>::include( const Vec2T &point )
{
	if( x1 > point.x ) x1 = point.x;
	if( x2 < point.x ) x2 = point.x;
	if( y1 > point.y ) y1 = point.y;
	if( y2 < point.y ) y2 = point.y;
}

template<typename T>
void RectT<T>::include( const std::vector<Vec2T> &points )
{
	for( size_t s = 0; s < points.size(); ++s )
		include( points[s] );
}

template<typename T>
void RectT<T>::include( const RectT<T> &rect )
{
	include( Vec2T( rect.x1, rect.y1 ) );
	include( Vec2T( rect.x2, rect.y2 ) );
}

template<typename T>
Area RectT<T>::getInteriorArea() const
{
	RectT<T> canon = canonicalized();
	
	return Area( static_cast<int32_t>( ceil( canon.x1 ) ), static_cast<int32_t>( ceil( canon.y1 ) ), 
		static_cast<int32_t>( floor( canon.x2 ) ) + 1, static_cast<int32_t>( floor( canon.y2 ) ) + 1 );
}

template<typename T>
RectT<T> RectT<T>::getCenteredFit( const RectT<T> &other, bool expand ) const
{
	RectT<T> result = *this;
	result.offset( other.getCenter() - result.getCenter() );
	
	bool isInside = ( ( result.getWidth() < other.getWidth() ) && ( result.getHeight() < other.getHeight() ) );
	if( expand || ( ! isInside ) ) { // need to do some scaling
		T aspectAspect = result.getAspectRatio() / other.getAspectRatio();
		if( aspectAspect >= 1.0f ) { // result is proportionally wider so we need to fit its x-axis
			T scaleBy = other.getWidth() / result.getWidth();
			result.scaleCentered( scaleBy );
		}
		else { // result is proportionally wider so we need to fit its y-axis
			T scaleBy = other.getHeight() / result.getHeight();
			result.scaleCentered( scaleBy );
		}
	}
	
	return result;
}
	
template<typename T>
RectT<T> RectT<T>::getCenteredFill( const RectT<T> &other, bool contract ) const
{
	RectT<T> result = *this;
	result.offset( other.getCenter() - result.getCenter() );
	
	bool otherIsInside = ( ( result.getWidth() > other.getWidth() ) && ( result.getHeight() > other.getHeight() ) );
	if( contract || ( ! otherIsInside ) ) { // need to do some scaling
		T aspectAspect = result.getAspectRatio() / other.getAspectRatio();
		if( aspectAspect <= 1.0f ) { // result is proportionally wider so we need to fit its x-axis
			T scaleBy = other.getWidth() / result.getWidth();
			result.scaleCentered( scaleBy );
		}
		else { // result is proportionally wider so we need to fit its y-axis
			T scaleBy = other.getHeight() / result.getHeight();
			result.scaleCentered( scaleBy );
		}
	}
	
	return result;
}

RectMapping::RectMapping( const Rectf &aSrcRect, const Rectf &aDstRect, bool preserveSrcAspect )
	: mSrcRect( aSrcRect ), mDstRect( aDstRect )
{
	if( preserveSrcAspect ) {
		float srcAspect = aSrcRect.getWidth() / (float)aSrcRect.getHeight();
		float dstAspect = aDstRect.getWidth() / (float)aDstRect.getHeight();
		if( srcAspect < dstAspect ) { // src is narrower aspect
			float heightRatio = mDstRect.getHeight() / mSrcRect.getHeight();
			float effectiveWidth = mSrcRect.getWidth() * heightRatio;
			float offsetX = ( mDstRect.getWidth() - effectiveWidth ) / 2.0f;
			float dstWidth = mDstRect.getHeight() * srcAspect;
			mDstRect.set( mDstRect.x1 + offsetX, mDstRect.getY1(), mDstRect.x1 + offsetX + dstWidth, mDstRect.getY2() );
		}
		else { // src is wider aspect
			float effectiveHeight = mSrcRect.getHeight() * ( mDstRect.getWidth() / mSrcRect.getWidth() );
			float offsetY = ( mDstRect.getHeight() - effectiveHeight ) / 2.0f;
			float dstHeight = mDstRect.getWidth() / srcAspect;
			mDstRect.set( mDstRect.x1, mDstRect.getY1() + offsetY, mDstRect.x2, mDstRect.getY1() + offsetY + dstHeight );
		}
	}
}

vec2 RectMapping::map( const vec2 &srcPoint ) const
{
	float x = ( srcPoint.x - mSrcRect.getX1() ) / mSrcRect.getWidth() * mDstRect.getWidth() + mDstRect.getX1();
	float y = ( srcPoint.y - mSrcRect.getY1() ) / mSrcRect.getHeight() * mDstRect.getHeight() + mDstRect.getY1();
	
	return vec2( x, y );
}

Rectf RectMapping::map( const Rectf &srcRect ) const
{
	vec2 ul = map( vec2( srcRect.x1, srcRect.y1 ) );
	vec2 lr = map( vec2( srcRect.x2, srcRect.y2 ) );
	
	return Rectf( ul.x, ul.y, lr.x, lr.y );
}

// This routine assumes that srcRect maps into dstArea
// and trims both the dst and the src proportionally to accommodate any clipping
void getClippedScaledRects( const Area &srcSurfaceBounds, const Rectf &srcRect, const Area &dstSurfaceBounds, const Area &dstArea, Rectf *resultSrcRect, Area *resultDstArea )
{
	// clip the destination, create newSrcRect from mapping
	Area clippedDst = dstArea.getClipBy( dstSurfaceBounds );
	Rectf newSrcRect = RectMapping( Rectf( dstArea ), srcRect ).map( Rectf( clippedDst ) );

	// clip the src, create newDstRect from mapping
	newSrcRect.clipBy( Rectf( srcSurfaceBounds ) );
	Rectf newDstRect = RectMapping( srcRect, Rectf( dstArea ) ).map( Rectf( newSrcRect ) );

	// discretize and clip the destinaton
	// the discretization is done to minimize the area
	*resultDstArea = newDstRect.getInteriorArea();
	resultDstArea->clipBy( dstSurfaceBounds );
	
	// now map the discretized, clipped destination back once more to get the final source
	*resultSrcRect = RectMapping( Rectf( dstArea ), srcRect ).map( Rectf( *resultDstArea ) );
	
	// this is not kosher, but sometimes we need to fudge things on the src to keep it in bounds
	resultSrcRect->clipBy( Rectf( srcSurfaceBounds ) );
}

template<typename T>
std::ostream& operator<< ( std::ostream& o, const RectT<T>& rect )
{
	return o << "(" << rect.x1 << ", " << rect.y1 << ")-(" << rect.x2 << ", " << rect.y2 << ")";
}

template class CI_API RectT<float>;
template class CI_API RectT<double>;

} // namespace cinder
