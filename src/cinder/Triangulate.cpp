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

#include "cinder/Triangulate.h"
#include "cinder/Shape2d.h"
#include "../libtess2/tesselator.h"

using namespace std;

namespace cinder {

void* stdAlloc( void* userData, unsigned int size )
{
	int *allocated = (int*)userData;
	*allocated += (int)size;
	return malloc( size );
}

void stdFree( void* /*userData*/, void* ptr )
{
	free( ptr );
}

Triangulator::Triangulator( const Path2d &path, float approximationScale )
{	
	allocate();
	addPath( path, approximationScale );
}

Triangulator::Triangulator( const Shape2d &shape, float approximationScale )
{	
	allocate();
	addShape( shape, approximationScale );
}

Triangulator::Triangulator( const PolyLine2f &polyLine )
{	
	allocate();
	addPolyLine( polyLine );
}

Triangulator::Triangulator()
{
	allocate();
}

void Triangulator::allocate()
{
	mAllocated = 0;
	
	TESSalloc ma;
	memset( &ma, 0, sizeof(ma) );
	ma.memalloc = stdAlloc;
	ma.memfree = stdFree;
	ma.userData = (void*)&mAllocated;
	ma.extraVertices = 2560; // realloc not provided, allow 256 extra vertices.

	mTess = shared_ptr<TESStesselator>( tessNewTess( &ma ), tessDeleteTess );
	if( ! mTess )
		throw Triangulator::Exception();
}

void Triangulator::addShape( const Shape2d &shape, float approximationScale )
{
	size_t numContours = shape.getContours().size();
	for( size_t p = 0; p < numContours; ++p ) {
		addPath( shape.getContour(p), approximationScale );
	}	
}

void Triangulator::addPath( const Path2d &path, float approximationScale )
{
	vector<vec2> subdivided = path.subdivide( approximationScale );
	tessAddContour( mTess.get(), 2, &subdivided[0], sizeof(float) * 2, (int)subdivided.size() );
}

void Triangulator::addPolyLine( const PolyLine2f &polyLine )
{
	if( polyLine.size() > 0 )
		tessAddContour( mTess.get(), 2, &polyLine.getPoints()[0], sizeof(float) * 2, (int)polyLine.size() );
}

void Triangulator::addPolyLine( const vec2 *points, size_t numPoints )
{
	if( numPoints > 0 )
		tessAddContour( mTess.get(), 2, &points, sizeof(vec2), (int)numPoints );
}

TriMesh Triangulator::calcMesh( Winding winding )
{
	TriMesh result( TriMesh::Format().positions( 2 ) );
	
	tessTesselate( mTess.get(), (int)winding, TESS_POLYGONS, 3, 2, 0 );
	result.appendPositions( (vec2*)tessGetVertices( mTess.get() ), tessGetVertexCount( mTess.get() ) );
	result.appendIndices( (uint32_t*)( tessGetElements( mTess.get() ) ), tessGetElementCount( mTess.get() ) * 3 );
	
	return result;
}

TriMeshRef Triangulator::createMesh( Winding winding )
{
	TriMeshRef result = make_shared<TriMesh>( TriMesh::Format().positions( 2 ) );
	
	tessTesselate( mTess.get(), (int)winding, TESS_POLYGONS, 3, 2, 0 );
	result->appendPositions( (vec2*)tessGetVertices( mTess.get() ), tessGetVertexCount( mTess.get() ) );
	result->appendIndices( (uint32_t*)( tessGetElements( mTess.get() ) ), tessGetElementCount( mTess.get() ) * 3 );
	
	return result;
}

} // namespace cinder
