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

#pragma once

#include "cinder/PolyLine.h"

#include <vector>

namespace cinder {

std::vector<cinder::PolyLine2f> calcIntersection( const std::vector<cinder::PolyLine2f> &a, std::vector<cinder::PolyLine2f> &b );
//! Calculates the boolean union of \a a and \a b.
std::vector<cinder::PolyLine2f> calcUnion( const std::vector<cinder::PolyLine2f> &a, std::vector<cinder::PolyLine2f> &b );
//! Calculates the boolean difference of \a subject and \a clip.
std::vector<cinder::PolyLine2f> calcDifference( const std::vector<cinder::PolyLine2f> &subject, std::vector<cinder::PolyLine2f> &clip );
//! Calculates the boolean XOR of \a a and \a b.
std::vector<cinder::PolyLine2f> calcXor( const std::vector<cinder::PolyLine2f> &a, std::vector<cinder::PolyLine2f> &b );
//! Calculates the rounded offset curve of \a poly. Negative \a offset creates inset.
std::vector<cinder::PolyLine2f> calcRoundOffset( const std::vector<cinder::PolyLine2f> &poly, float offset, double arcTolerance = 0.25 );
//! Calculates the mitered offset curve of \a poly. Negative \a offset creates inset.
std::vector<cinder::PolyLine2f> calcMiterOffset( const std::vector<cinder::PolyLine2f> &poly, float offset, double miterLimit = 2.0 );
//! Calculates the squared offset curve of \a poly. Negative \a offset creates inset.
std::vector<cinder::PolyLine2f> calcSquareOffset( const std::vector<cinder::PolyLine2f> &poly, float offset );

} // namespace cinder