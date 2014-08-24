/*
 Copyright (c) 2009, The Barbarian Group
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
#include "cinder/Vector.h"

#include <vector>
#include <float.h>
#include <stdlib.h>
#include <algorithm>
#include <utility>

namespace cinder {

// KdTree Declarations
template<unsigned char K>
struct KdNode {
	void init( float p, uint32_t a) {
		splitPos = p;
		splitAxis = a;
		rightChild = ~0;
		hasLeftChild = 0;
	}
	void initLeaf() {
		splitAxis = K;
		rightChild = ~0;
		hasLeftChild = 0;
	}
	// KdNode Data
	float splitPos;
	uint32_t splitAxis:2;
	uint32_t hasLeftChild:1;
	uint32_t rightChild:29;
};

struct NullLookupProc {
 public:
	void process( uint32_t id, float distSqrd, float &maxDistSqrd ) {}
};

template <typename NodeData, unsigned char K=3, class LookupProc = NullLookupProc> class KdTree {
public:
	typedef std::pair<const NodeData*, uint32_t> NodeDataIndex;
	
	// KdTree Public Methods
	template<typename NodeDataVector>
	KdTree( const NodeDataVector &data );
	KdTree() {}
	template<typename NodeDataVector>
	void initialize( const NodeDataVector &d );
	~KdTree() {
		free( nodes );
		delete[] mNodeData;
	}
	void recursiveBuild( uint32_t nodeNum, uint32_t start, uint32_t end, std::vector<NodeDataIndex> &buildNodes );
	void lookup( const NodeData &p, const LookupProc &process, float maxDist ) const;
	void findNearest( float p[K], float result[K], uint32_t *resultIndex ) const;
	
private:
	// KdTree Private Methods
	void privateLookup(uint32_t nodeNum, float p[K], const LookupProc &process, float &maxDistSquared) const;
	void privateFindNearest( uint32_t nodeNum, float p[K], float &maxDistSquared, float result[K], uint32_t *resultIndex ) const;
	// KdTree Private Data
	KdNode<K> *nodes;
	NodeDataIndex *mNodeData;
	uint32_t nNodes, nextFreeNode;
};


// Shims
template<typename NDV>
struct NodeDataVectorTraits
{
	static uint32_t getSize( const NDV &ndv ) {
		return static_cast<uint32_t>( ndv.size() );
	}
};

template<typename NodeData>
struct NodeDataTraits
{
	static float getAxis( const NodeData &data, int axis ) {
		if( axis == 0 ) return data.x;
		else if( axis == 1 ) return data.y;
		else return (float)data.z;		
	}
	static float getAxis0( const NodeData &data ) { return static_cast<float>( data.x ); }
	static float getAxis1( const NodeData &data ) { return static_cast<float>( data.y ); }
	static float getAxis2( const NodeData &data ) { return static_cast<float>( data.z ); }
	static float distanceSquared( const NodeData &data, float k[3] ) {
		float result = ( data.x - k[0] ) * ( data.x - k[0] );
		result += ( data.y - k[1] ) * ( data.y - k[1] );
		result += ( data.z - k[2] ) * ( data.z - k[2] );
		return result;
	}
};

template<>
struct NodeDataTraits<vec2>
{
	static float getAxis( const vec2 &data, int axis ) {
		if( axis == 0 ) return data.x;
		else return data.y; 
	}
	static float getAxis0( const vec2 &data ) { return static_cast<float>( data.x ); }
	static float getAxis1( const vec2 &data ) { return static_cast<float>( data.y ); }
	static float distanceSquared( const vec2 &data, float k[2] ) {
		float result = ( data.x - k[0] ) * ( data.x - k[0] );
		result += ( data.y - k[1] ) * ( data.y - k[1] );
		return result;
	}
};

template<typename NodeData> struct CompareNode {
	CompareNode( int a ) { axis = a; }
	int axis;
	bool operator()(const std::pair<const NodeData*,uint32_t> &d1,
			const std::pair<const NodeData*,uint32_t> &d2) const {
		return NodeDataTraits<NodeData>::getAxis( *d1.first, axis ) == NodeDataTraits<NodeData>::getAxis( *d2.first, axis ) ? ( d1.first < d2.first ) :
			NodeDataTraits<NodeData>::getAxis( *d1.first, axis ) < NodeDataTraits<NodeData>::getAxis( *d2.first, axis );
	}
};

// KdTree Method Definitions
template<typename NodeData, unsigned char K, typename LookupProc>
 template<typename NodeDataVector>
KdTree<NodeData, K, LookupProc>::KdTree(const NodeDataVector &d)
{
	initialize( d );
}

template<typename NodeData, unsigned char K, typename LookupProc>
 template<typename NodeDataVector>
void KdTree<NodeData, K, LookupProc>::initialize( const NodeDataVector &d )
{
	nNodes = NodeDataVectorTraits<NodeDataVector>::getSize( d );
	nextFreeNode = 1;
	nodes = (KdNode<K> *)malloc(nNodes * sizeof(KdNode<K>));
	mNodeData = new NodeDataIndex[nNodes];
	std::vector<NodeDataIndex> buildNodes;
	buildNodes.reserve( nNodes );
	for( uint32_t i = 0; i < nNodes; ++i )
		buildNodes.push_back( std::make_pair( &d[i], i ) );
	// Begin the KdTree building process
	recursiveBuild( 0, 0, nNodes, buildNodes );
}

template<typename NodeData, unsigned char K, typename LookupProc>
void KdTree<NodeData, K, LookupProc>::recursiveBuild( uint32_t nodeNum, uint32_t start, uint32_t end, std::vector<NodeDataIndex> &buildNodes )
{
	// Create leaf node of kd-tree if we've reached the bottom
	if( start + 1 == end) {
		nodes[nodeNum].initLeaf();
		mNodeData[nodeNum] = buildNodes[start];
		return;
	}
	// Choose split direction and partition data
	// Compute bounds of data from _start_ to _end_
	float boundMin[K], boundMax[K];
	for( unsigned char k = 0; k < K; ++k ) {
		boundMin[k] = FLT_MAX;
		boundMax[k] = FLT_MIN;
	}
	
	for( uint32_t i = start; i < end; ++i ) {
		for( uint8_t axis = 0; axis < K; axis++ ) {
			// NOT Compiling? you should define NOMINMAX
			boundMin[axis] = std::min( boundMin[axis], NodeDataTraits<NodeData>::getAxis( *buildNodes[i].first, axis ) );
			boundMax[axis] = std::max( boundMax[axis], NodeDataTraits<NodeData>::getAxis( *buildNodes[i].first, axis ) );
		}
	}
	int splitAxis = 0;
	float maxExtent = boundMax[0] - boundMin[0];
	for( unsigned char k = 1; k < K; ++k ) {
		if( boundMax[k] - boundMin[k] > maxExtent ) {
			splitAxis = k;
			maxExtent = boundMax[k] - boundMin[k];
		}	
	}
	uint32_t splitPos = ( start + end ) / 2;
	std::nth_element( &buildNodes[start], &buildNodes[splitPos], &buildNodes[end-1] + 1, CompareNode<NodeData>(splitAxis) );
	// Allocate kd-tree node and continue recursively
	nodes[nodeNum].init( NodeDataTraits<NodeData>::getAxis( *buildNodes[splitPos].first, splitAxis ), splitAxis );
	mNodeData[nodeNum] = buildNodes[splitPos];
	if( start < splitPos ) {
		nodes[nodeNum].hasLeftChild = 1;
		uint32_t childNum = nextFreeNode++;
		recursiveBuild( childNum, start, splitPos, buildNodes );
	}
	if( splitPos + 1 < end ) {
		nodes[nodeNum].rightChild = nextFreeNode++;
		recursiveBuild( nodes[nodeNum].rightChild, splitPos + 1, end, buildNodes );
	}
}

template<typename NodeData, unsigned char K, typename LookupProc>
void KdTree<NodeData, K, LookupProc>::lookup( const NodeData &p, const LookupProc &proc, float maxDist ) const 
{
	float maxDistSqrd = maxDist * maxDist;
	float pt[K];
	for( unsigned char k = 0; k < K; ++k )
		pt[k] = NodeDataTraits<NodeData>::getAxis( p, k );

	privateLookup( 0, pt, proc, maxDistSqrd );
}

template<typename NodeData, unsigned char K, typename LookupProc>
void KdTree<NodeData, K, LookupProc>::privateLookup( uint32_t nodeNum, float p[K], const LookupProc &process, float &maxDistSquared ) const 
{
	KdNode<K> *node = &nodes[nodeNum];
	// process kd-tree node's children
	int axis = node->splitAxis;
	if( axis != K ) {
		float dist2 = ( p[axis] - node->splitPos ) * ( p[axis] - node->splitPos );
		if( p[axis] <= node->splitPos ) {
			if(node->hasLeftChild)
				privateLookup( nodeNum + 1, p, process, maxDistSquared );
			if( ( dist2 < maxDistSquared ) && ( node->rightChild < nNodes ) )
				privateLookup( node->rightChild, p, process, maxDistSquared );
		}
		else {
			if( node->rightChild < nNodes )
				privateLookup( node->rightChild, p, process, maxDistSquared );
			if( ( dist2 < maxDistSquared ) && node->hasLeftChild )
				privateLookup( nodeNum + 1, p, process, maxDistSquared );
		}
	}
	// Hand kd-tree node to processing function
	float distSqr = 0.0f;
	for( unsigned char k = 0; k < K; ++k ) {
		float v = NodeDataTraits<NodeData>::getAxis( *mNodeData[nodeNum].first, k ) - p[k];
		distSqr += v * v;
	}
	
	if( distSqr < maxDistSquared )
		process.process( mNodeData[nodeNum].second, distSqr, maxDistSquared );
}

// Find Nearest
template<typename NodeData, unsigned char K, typename LookupProc>
void KdTree<NodeData, K, LookupProc>::findNearest( float p[K], float result[K], uint32_t *resultIndex ) const
{
	float maxDist = FLT_MAX;
	*resultIndex = -1;
	privateFindNearest( 0, p, maxDist, result, resultIndex );
}

template<typename NodeData, unsigned char K, typename LookupProc>
void KdTree<NodeData, K, LookupProc>::privateFindNearest( uint32_t nodeNum, float p[K], float &maxDistSquared, float result[K], uint32_t *resultIndex ) const
{
	KdNode<K> *node = &nodes[nodeNum];
	// process kd-tree node's children
	int axis = node->splitAxis;
	if( axis != K ) {
		float dist2 = (p[axis] - node->splitPos) * (p[axis] - node->splitPos);
		if( p[axis] <= node->splitPos ) {
			if( node->hasLeftChild )
				privateFindNearest( nodeNum+1, p, maxDistSquared, result, resultIndex );
			if( ( dist2 < maxDistSquared ) && ( node->rightChild < nNodes) )
				privateFindNearest( node->rightChild, p, maxDistSquared, result, resultIndex );
		}
		else {
			if( node->rightChild < nNodes)
				privateFindNearest(node->rightChild,
				              p,
							  maxDistSquared, result, resultIndex );
			if( dist2 < maxDistSquared && node->hasLeftChild)
				privateFindNearest(nodeNum+1,
				              p,
							  maxDistSquared, result, resultIndex );
		}
	}
	
	float distSqr = NodeDataTraits<NodeData>::distanceSquared( *mNodeData[nodeNum].first, p );
	if( distSqr < maxDistSquared ) {
		maxDistSquared = distSqr;
		for( unsigned char k = 0; k < K; ++k )
			result[k] = NodeDataTraits<NodeData>::getAxis( *mNodeData[nodeNum].first, k );
		*resultIndex = mNodeData[nodeNum].second;
	}
}

} // namespace ci
