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

#include "cinder/AxisAlignedBox.h"
#include "cinder/Sphere.h"

using std::vector;

namespace cinder {

bool Sphere::intersects( const AxisAlignedBox &box ) const
{
	return box.intersects( *this );
}

bool Sphere::intersects( const Ray &ray ) const
{
	float 		t;
	vec3		temp 	= ray.getOrigin() - mCenter;
	float 		a 		= dot( ray.getDirection(), ray.getDirection() );
	float 		b 		= 2.0f * dot( temp, ray.getDirection() );
	float 		c 		= dot( temp, temp ) - mRadius * mRadius;
	float 		disc	= b * b - 4.0f * a * c;

	if( disc < 0.0f ) {
		return false;
	}
	else { // this probably can be optimized
		float e = math<float>::sqrt( disc );
		float denom = 2.0f * a;
		t = (-b - e) / denom;    // smaller root

		if( t > EPSILON_VALUE ) {
			return true;
		} 

		t = (-b + e) / denom;    // larger root
		if( t > EPSILON_VALUE ) {
			return true;
		} 
	}

	return false;
}

int Sphere::intersect( const Ray &ray, float *intersection ) const
{
	float 		t;
	vec3		temp 	= ray.getOrigin() - mCenter;
	float 		a 		= dot( ray.getDirection(), ray.getDirection() );
	float 		b 		= 2 * dot( temp, ray.getDirection() );
	float 		c 		= dot( temp, temp ) - mRadius * mRadius;
	float 		disc	= b * b - 4 * a * c;

	if( disc < 0.0f ) {
		return 0;
	}
	else {
		float e = math<float>::sqrt( disc );
		float denom = 2.0f * a;
		t = (-b - e) / denom;    // smaller root

		if( t > EPSILON_VALUE ) {
			*intersection = t;
			return 1;
		} 

		t = (-b + e) / denom;    // larger root
		if( t > EPSILON_VALUE ) {
			*intersection = t;
			return 1;
		} 
	}

	return 0;
}

int Sphere::intersect( const Ray &ray, float *min, float *max ) const
{
	vec3		temp = ray.getOrigin() - mCenter;
	float 		a = dot( ray.getDirection(), ray.getDirection() );
	float 		b = 2 * dot( temp, ray.getDirection() );
	float 		c = dot( temp, temp ) - mRadius * mRadius;
	float 		disc = b * b - 4 * a * c;

	int count = 0;
	if( disc >= 0.0f ) {
		float t;

		float e = math<float>::sqrt( disc );
		float denom = 2.0f * a;

		t = ( -b - e ) / denom;    // smaller root
		if( t > EPSILON_VALUE ) {
			*min = t;
			count++;
		}

		t = ( -b + e ) / denom;    // larger root
		if( t > EPSILON_VALUE ) {
			*max = t;
			count++;
		}
	}

	return count;
}

vec3 Sphere::closestPoint( const Ray &ray ) const
{
	float 		t;
	vec3		diff 	= ray.getOrigin() - mCenter;
	float 		a 		= dot( ray.getDirection(), ray.getDirection() );
	float 		b 		= 2 * dot( diff, ray.getDirection() );
	float 		c 		= dot( diff, diff ) - mRadius * mRadius;
	float 		disc	= b * b - 4 * a * c;

	if( disc > 0 ) {
		float e = math<float>::sqrt( disc );
		float denom = 2 * a;
		t = (-b - e) / denom;    // smaller root

		if( t > EPSILON_VALUE )
			return ray.calcPosition( t );

		t = (-b + e) / denom;    // larger root
		if( t > EPSILON_VALUE )
			return ray.calcPosition( t );
	}
	
	// doesn't intersect; closest point on line
	t = dot( -diff, normalize(ray.getDirection()) );
	vec3 onRay = ray.calcPosition( t );
	return mCenter + normalize( onRay - mCenter ) * mRadius;
	
//	return ray.getDirection() * dot( ray.getDirection(), (mCenter - ray.getOrigin() ) );
}

Sphere Sphere::calculateBoundingSphere( const vector<vec3> &points )
{
	return calculateBoundingSphere( points.data(), points.size() );
}

Sphere Sphere::calculateBoundingSphere( const vec3 *points, size_t numPoints )
{
	if( ! numPoints )
		return Sphere( vec3( 0 ), 0 );
	
	// compute minimal and maximal bounds
	vec3 min(points[0]), max(points[0]);
	for( size_t i = 1; i < numPoints; ++i ) {
		if( points[i].x < min.x )
			min.x = points[i].x;
		else if( points[i].x > max.x )
			max.x = points[i].x;
		if( points[i].y < min.y )
			min.y = points[i].y;
		else if( points[i].y > max.y )
			max.y = points[i].y;
		if( points[i].z < min.z )
			min.z = points[i].z;
		else if( points[i].z > max.z )
			max.z = points[i].z;
	}
	// compute center and radius
	vec3 center = 0.5f * ( min + max );
	float maxDistance = distance2( center, points[0] );
	for( size_t i = 1; i < numPoints; ++i ) {
		float dist = distance2( center, points[i] );
		if( dist > maxDistance )
			maxDistance = dist;
	}
	return Sphere( center, math<float>::sqrt( maxDistance ) );
}

void Sphere::calcProjection( float focalLength, vec2 *outCenter, vec2 *outAxisA, vec2 *outAxisB ) const
{
	vec3 o( -mCenter.x, mCenter.y, mCenter.z );
	
    float r2 = mRadius * mRadius;
	float z2 = o.z * o.z;
	float l2 = dot( o, o );

	if( outCenter )
		*outCenter = focalLength * o.z * vec2( o ) / ( z2-r2 );
	if( fabs( z2 - l2 ) > 0.00001f ) {
		if( outAxisA )
			*outAxisA = focalLength * sqrtf( -r2*(r2-l2)/((l2-z2)*(r2-z2)*(r2-z2)) ) * vec2( o.x, o.y );
			
		if( outAxisB )
			*outAxisB = focalLength * sqrtf( fabs(-r2*(r2-l2)/((l2-z2)*(r2-z2)*(r2-l2))) ) * vec2( -o.y, o.x );
	}
	else { // approximate with circle
		float radius = focalLength * mRadius / sqrtf( z2 - r2 );
		if( outAxisA )
			*outAxisA = vec2( radius, 0 );
		if( outAxisB )
			*outAxisB = vec2( 0, radius );
	}
}

void Sphere::calcProjection( float focalLength, vec2 screenSizePixels, vec2 *outCenter, vec2 *outAxisA, vec2 *outAxisB ) const
{
	auto toScreenPixels = [=] ( vec2 v, const vec2 &windowSize ) {
		vec2 result = v;
		result.x *= 1 / ( windowSize.x / windowSize.y );
		result += vec2( 0.5f );
		result *= windowSize;
		return result;
	};
	
	vec2 center, axisA, axisB;
	calcProjection( focalLength, &center, &axisA, &axisB );
	if( outCenter )
		*outCenter = toScreenPixels( center, screenSizePixels );
	if( outAxisA )
		*outAxisA = toScreenPixels( center + axisA * 0.5f, screenSizePixels ) - toScreenPixels( center - axisA * 0.5f, screenSizePixels );
	if( outAxisB )
		*outAxisB = toScreenPixels( center + axisB * 0.5f, screenSizePixels ) - toScreenPixels( center - axisB * 0.5f, screenSizePixels );
}

float Sphere::calcProjectedArea( float focalLength, vec2 screenSizePixels ) const
{
	vec3 o( mCenter );
	
    float r2 = mRadius * mRadius;
	float z2 = o.z * o.z;
	float l2 = dot( o, o );
	
	float area = -float( M_PI ) * focalLength * focalLength * r2 * sqrt( fabs((l2-r2)/(r2-z2)) ) / (r2-z2);
	float aspectRatio = screenSizePixels.x / screenSizePixels.y;
	return area * screenSizePixels.x * screenSizePixels.y * 0.25f / aspectRatio;
}

Sphere Sphere::transformed( const mat4 &transform ) const
{
	vec4 center = transform * vec4( mCenter, 1 );
	vec4 radius = transform * vec4( mRadius, 0, 0, 0 );

	return Sphere( vec3( center ), glm::length( radius ) );
}

} // namespace cinder
