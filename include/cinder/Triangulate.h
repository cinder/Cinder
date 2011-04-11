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

class Triangulator {
  public:
	typedef enum Winding { WINDING_ODD, WINDING_NONZERO, WINDING_POSITIVE, WINDING_NEGATIVE, WINDING_ABS_GEQ_TWO } Winding;
  
	Triangulator();
	Triangulator( const Shape2d &shape, float approximationScale = 1.0f );
	
//	void		addEdge( const PolyLine &polyLine );
	void		addPath( const Path2d &path, float approximationScale = 1.0f );
//	void		addHole( const PolyLine &polyLine );
	
	TriMesh2d		calcMesh( Winding winding = WINDING_ODD );
	
	class Exception : public ci::Exception {
	};
	
  protected:	
	void			allocate();
	
	int									mAllocated;
	std::shared_ptr<TESStesselator>		mTess;
};

} // namespace cinder