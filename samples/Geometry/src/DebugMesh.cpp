/*
 Copyright (c) 2014, Paul Houx - All rights reserved.
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

#include "DebugMesh.h"

using namespace ci;
using namespace ci::geom;
using namespace std;

DebugMesh::DebugMesh(void)
	: mColor( Color::white() )
{
	enable( Attrib::POSITION );
	enable( Attrib::COLOR );

	clear();
}

DebugMesh::DebugMesh(const TriMesh& mesh, const ColorA& color)
	: mColor( color )
{
	enable( Attrib::POSITION );
	enable( Attrib::COLOR );

	setMesh(mesh);
}

DebugMesh::~DebugMesh(void)
{
}

void DebugMesh::clear()
{
	mVertices.clear();
	mColors.clear();
	mIndices.clear();
}

void DebugMesh::setMesh(const TriMesh& mesh)
{
	clear();

	// check if mesh is valid and count vertices
	if(!mesh.hasNormals())
		return;

	size_t numVertices = mesh.getNumVertices();
	if(numVertices < 1)
		return;

	// reserve memory to prevent reallocations
	bool hasTangents = mesh.hasTangents();
	size_t numEntriesPerVertex = hasTangents ? 4 : 2;
	size_t numIndicesPerVertex = hasTangents ? 6 : 2;

	mVertices.reserve( numVertices * numEntriesPerVertex );
	mColors.reserve( numVertices * numEntriesPerVertex );
	mIndices.reserve( numVertices * numIndicesPerVertex );

	// determine the right scale, based on the bounding box
	AxisAlignedBox3f bbox = mesh.calcBoundingBox();
	vec3 size = bbox.getMax() - bbox.getMin();
	float scale = math<float>::max( math<float>::max( float(size.x), float(size.y) ), float(size.z) ) / 25.0f;

	// construct mesh
	for(size_t i=0;i<numVertices;++i) {
		uint32_t idx = mVertices.size();

		mVertices.push_back( mesh.getPositions<3>()[i] );
		mVertices.push_back( mesh.getPositions<3>()[i] + scale * mesh.getNormals()[i] );
		
		mColors.push_back( mColor );
		mColors.push_back( mColor );
		
		mIndices.push_back( idx );
		mIndices.push_back( idx + 1 );

		if(hasTangents)
		{
			mVertices.push_back( mesh.getPositions<3>()[i] + scale * mesh.getTangents()[i] );
			mVertices.push_back( mesh.getPositions<3>()[i] + scale * cross( mesh.getNormals()[i], mesh.getTangents()[i] ) );

			mColors.push_back( Color(1, 0, 0) );
			mColors.push_back( Color(0, 1, 0) );

			mIndices.push_back( idx );
			mIndices.push_back( idx + 2 );
			mIndices.push_back( idx );
			mIndices.push_back( idx + 3 );
		}
	}
}

uint8_t DebugMesh::getAttribDims( Attrib attr ) const
{
	switch( attr ) {
		case Attrib::POSITION: return 3;
		case Attrib::COLOR: return 3;
		default:
			return 0;
	}
}

void DebugMesh::loadInto( Target *target ) const
{
	target->copyAttrib( Attrib::POSITION, 3, 0, reinterpret_cast<const float*>(&mVertices.front()), mVertices.size() );
	target->copyAttrib( Attrib::COLOR, 3, 0, reinterpret_cast<const float*>(&mColors.front()), mColors.size() );
	target->copyIndices( Primitive::LINES, &mIndices.front(), mIndices.size(), 4 );
}
