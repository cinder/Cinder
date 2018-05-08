/*
 Copyright (c) 2018, The Cinder Project, All rights reserved.
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

#include "CinderClipper.h"
#include "clipper.hpp"

using namespace std;

namespace cinder {

template<typename T, typename U>
ClipperLib::Paths toClipper( const vector<PolyLineT<T>> &polys, U mult )
{
	ClipperLib::Paths result;
	result.resize( polys.size() );
	for( size_t c = 0; c < polys.size(); ++c ) {
		result[c].reserve( polys[c].size() );
		auto &points = polys[c].getPoints();
		for( size_t p = 0; p < points.size(); ++p ) {
			result[c].emplace_back( static_cast<int64_t>(points[p].x * mult), static_cast<int64_t>(points[p].y * mult) ); 
		}
	}
	
	return result;
} 

template<typename U>
vector<PolyLine2f> fromClipper( const ClipperLib::Paths &paths, U mult )
{
	vector<PolyLine2f> result;
	result.resize( paths.size() );
	
	const float invMult = 1.0f / mult;
	
	for( size_t c = 0; c < paths.size(); ++c ) {
		auto &path = paths[c];
		result[c].getPoints().reserve( path.size() );
		for( size_t p = 0; p < path.size(); ++p )
			result[c].getPoints().emplace_back( path[p].X * invMult, path[p].Y * invMult );
		result[c].setClosed();
	}
	
	return result;
}

vector<PolyLine2f> calcClipOp( ClipperLib::ClipType op, const vector<PolyLine2f> &subject, vector<PolyLine2f> &clip )
{
	const float SCALE = 2147483648.0f;
	auto aClipper = toClipper( subject, SCALE );
	auto bClipper = toClipper( clip, SCALE );
	ClipperLib::Paths solution;
	ClipperLib::Clipper c;
	c.AddPaths( aClipper, ClipperLib::ptSubject, true );
	c.AddPaths( bClipper, ClipperLib::ptClip, true );
	c.Execute( op, solution, ClipperLib::pftNonZero, ClipperLib::pftNonZero );
	return fromClipper( solution, SCALE );
}

vector<PolyLine2f> calcIntersection( const vector<PolyLine2f> &a, vector<PolyLine2f> &b )
{
	return calcClipOp( ClipperLib::ctIntersection, a, b );
}

vector<PolyLine2f> calcUnion( const vector<PolyLine2f> &a, vector<PolyLine2f> &b )
{
	return calcClipOp( ClipperLib::ctUnion, a, b );
}

vector<PolyLine2f> calcDifference( const vector<PolyLine2f> &subject, vector<PolyLine2f> &clip )
{
	return calcClipOp( ClipperLib::ctDifference, subject, clip );
}

vector<PolyLine2f> calcXor( const vector<PolyLine2f> &a, vector<PolyLine2f> &b )
{
	return calcClipOp( ClipperLib::ctXor, a, b );
}

vector<PolyLine2f> calcOffsetOp( ClipperLib::JoinType op, const vector<PolyLine2f> &a, float offset, double miterLimit, double arcTolerance )
{
	const float SCALE = 2147483648.0f;
	auto aClipper = toClipper( a, SCALE );
	ClipperLib::Paths solution;
	ClipperLib::ClipperOffset co( miterLimit, arcTolerance * SCALE ); 
	co.AddPaths( aClipper, op, ClipperLib::etClosedPolygon );
	co.Execute( solution, offset * SCALE );
	return fromClipper( solution, SCALE );
}

std::vector<cinder::PolyLine2f> calcRoundOffset( const std::vector<cinder::PolyLine2f> &poly, float offset, double arcTolerance )
{
	return calcOffsetOp( ClipperLib::jtRound, poly, offset, 0, arcTolerance );
}

std::vector<cinder::PolyLine2f> calcMiterOffset( const std::vector<cinder::PolyLine2f> &poly, float offset, double miterLimit )
{
	return calcOffsetOp( ClipperLib::jtMiter, poly, offset, miterLimit, 0 );
}

std::vector<cinder::PolyLine2f> calcSquareOffset( const std::vector<cinder::PolyLine2f> &poly, float offset )
{
	return calcOffsetOp( ClipperLib::jtSquare, poly, offset, 0, 0 );
}

} // namespace cinder