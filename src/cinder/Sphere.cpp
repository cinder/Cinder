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

#include "cinder/Sphere.h"

using std::vector;

namespace cinder {

bool Sphere::intersects( const Ray &ray )
{
	float 		t;
	Vec3f		temp 	= ray.getOrigin() - mCenter;
	float 		a 		= ray.getDirection().dot( ray.getDirection() );
	float 		b 		= 2.0f * temp.dot( ray.getDirection() );
	float 		c 		= temp.dot( temp ) - mRadius * mRadius;
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

int Sphere::intersect( const Ray &ray, float *intersection )
{
	float 		t;
	Vec3f		temp 	= ray.getOrigin() - mCenter;
	float 		a 		= ray.getDirection().dot( ray.getDirection() );
	float 		b 		= 2.0f * temp.dot( ray.getDirection() );
	float 		c 		= temp.dot( temp ) - mRadius * mRadius;
	float 		disc	= b * b - 4.0f * a * c;

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

Sphere Sphere::calculateBoundingSphere( const vector<Vec3f> &points )
{
	// compute minimal and maximal bounds
	Vec3f min(points[0]), max(points[0]);
	for( size_t i = 1; i < points.size(); ++i ) {
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
	Vec3f center = 0.5f * ( min + max );
	float maxDistance = center.distanceSquared( points[0] );
	for( size_t i = 1; i < points.size(); ++i ) {
		float dist = center.distanceSquared( points[i] );
		if( dist > maxDistance )
			maxDistance = dist;
	}
	return Sphere( center, math<float>::sqrt( maxDistance ) );
}

} // namespace cinder
