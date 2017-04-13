/*
 Copyright (c) 2011, The Cinder Project: http://libcinder.org
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

#include "cinder/Cinder.h"
#include "cinder/TriMesh.h"
#include "cinder/PolyLine.h"
#include "cinder/Shape2d.h"
#include "cinder/Path2d.h"

struct TESStesselator;

namespace cinder {

//! Converts an arbitrary Shape2d into a TriMesh2d
class CI_API Triangulator {
  public:
	typedef enum Winding { WINDING_ODD, WINDING_NONZERO, WINDING_POSITIVE, WINDING_NEGATIVE, WINDING_ABS_GEQ_TWO } Winding;

	//! Default constructor
	Triangulator();
	//! Constructs using a Path2d. \a approximationScale represents how smooth the tesselation is, with 1.0 corresponding to 1:1 with screen space
	Triangulator( const Path2d &path, float approximationScale = 1.0f );
	//! Constructs using a Shape2d. \a approximationScale represents how smooth the tesselation is, with 1.0 corresponding to 1:1 with screen space
	Triangulator( const Shape2d &shape, float approximationScale = 1.0f );
	//! Constructs using a PolyLine2f.
	Triangulator( const PolyLine2f &polyLine );

	//! Adds a Shape2d to the tesselation. \a approximationScale represents how smooth the tesselation is, with 1.0 corresponding to 1:1 with screen space	
	void		addShape( const Shape2d &path, float approximationScale = 1.0f );
	//! Adds a Path2d to the tesselation. \a approximationScale represents how smooth the tesselation is, with 1.0 corresponding to 1:1 with screen space	
	void		addPath( const Path2d &path, float approximationScale = 1.0f );
	//! Adds a PolyLine2f to the tesselation.
	void		addPolyLine( const PolyLine2f &polyLine );
	//! Adds a PolyLine defined as a series of vec2's
	void		addPolyLine( const vec2 *points, size_t numPoints );

	//! Performs the tesselation, returning a TriMesh2d
	TriMesh		calcMesh( Winding winding = WINDING_ODD );
	//! Performs the tesselation, returning a TriMesh2d
	TriMeshRef	createMesh( Winding winding = WINDING_ODD );
	
	class CI_API Exception : public cinder::Exception {
	};
	
  protected:	
	void			allocate();
	
	int									mAllocated;
	std::shared_ptr<TESStesselator>		mTess;
};

} // namespace cinder