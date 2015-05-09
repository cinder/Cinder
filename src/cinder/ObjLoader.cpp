/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

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

#include "cinder/ObjLoader.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;
#include <sstream>
using std::ostringstream;

#include <sstream>
using namespace std;
using boost::make_tuple;

namespace cinder {

ObjLoader::ObjLoader( shared_ptr<IStreamCinder> stream, bool includeUVs )
	: mStream( stream )
{
	parse( includeUVs );
}

ObjLoader::ObjLoader( DataSourceRef dataSource, bool includeUVs )
	: mStream( dataSource->createStream() )
{
	parse( includeUVs );
}

ObjLoader::ObjLoader( DataSourceRef dataSource, DataSourceRef materialSource, bool includeUVs )
    : mStream( dataSource->createStream() )
{
    parseMaterial( materialSource->createStream() );
    parse( includeUVs );
}
    
ObjLoader::~ObjLoader()
{
}
    
void ObjLoader::parseMaterial( std::shared_ptr<IStreamCinder> material )
{
    Material m;
    m.Ka[0] = m.Ka[1] = m.Ka[2] = 1.0f;
    m.Kd[0] = m.Kd[1] = m.Kd[2] = 1.0f;

    while( ! material->isEof() ) {
        string line = material->readLine();
        if( line.empty() || line[0] == '#' )
            continue;

        string tag;
        stringstream ss( line );
        ss >> tag;
        if( tag == "newmtl" ) {
            if( m.mName.length() > 0 )
                mMaterials[m.mName] = m;
            
            ss >> m.mName;
            m.Ka[0] = m.Ka[1] = m.Ka[2] = 1.0f;
            m.Kd[0] = m.Kd[1] = m.Kd[2] = 1.0f;
        }
        else if( tag == "Ka" ) {
            ss >> m.Ka[0] >> m.Ka[1] >> m.Ka[2];
        }
        else if( tag == "Kd" ) {
            ss >> m.Kd[0] >> m.Kd[1] >> m.Kd[2];
        }
    }
    if( m.mName.length() > 0 )
        mMaterials[m.mName] = m;
}

void ObjLoader::parse( bool includeUVs )
{
	Group *currentGroup;
	mGroups.push_back( Group() );
	currentGroup = &mGroups[mGroups.size()-1];
	currentGroup->mBaseVertexOffset = currentGroup->mBaseTexCoordOffset = currentGroup->mBaseNormalOffset = 0;

    const Material *currentMaterial = 0;
    
	size_t lineNumber = 0;
	while( ! mStream->isEof() ) {
		lineNumber++;
		string line = mStream->readLine(), tag;
        if( line.empty() || line[0] == '#' )
            continue;
        
		stringstream ss( line );
		ss >> tag;
		if( tag == "v" ) { // vertex
			Vec3f v;
			ss >> v.x >> v.y >> v.z;
			mVertices.push_back( v );
		}
		else if( tag == "vt" ) { // vertex texture coordinates
			if( includeUVs ) {
				Vec2f tex;
				ss >> tex.x >> tex.y;
				mTexCoords.push_back( tex );
			}
		}
		else if( tag == "vn" ) { // vertex normals
			Vec3f v;
			ss >> v.x >> v.y >> v.z;
			mNormals.push_back( v.normalized() );
		}
		else if( tag == "f" ) { // face
			parseFace( currentGroup, currentMaterial, line, includeUVs );
		}
		else if( tag == "g" ) { // group
			if( ! currentGroup->mFaces.empty() )
				mGroups.push_back( Group() );
			currentGroup = &mGroups[mGroups.size()-1];
			currentGroup->mBaseVertexOffset = mVertices.size();
			currentGroup->mBaseTexCoordOffset = mTexCoords.size();
			currentGroup->mBaseNormalOffset = mNormals.size();
			currentGroup->mName = line.substr( line.find( ' ' ) + 1 );
		}
        else if( tag == "usemtl") { // material
            string tag;
            ss >> tag;
            std::map<std::string, Material>::const_iterator m = mMaterials.find(tag);
            if( m != mMaterials.end() ) {
                currentMaterial = &m->second;
            }
        }
	}
}

void ObjLoader::parseFace( Group *group, const Material *material, const std::string &s, bool includeUVs )
{
	ObjLoader::Face result;
	result.mNumVertices = 0;
    result.mMaterial = material;

	size_t offset = 2; // account for "f "
	size_t length = s.length();
	while( offset < length ) {
		size_t endOfTriple, firstSlashOffset, secondSlashOffset;
	
		while( s[offset] == ' ' )
			++offset;
	
		// find the end of this triple "v/vt/vn"
		endOfTriple = s.find( ' ', offset );
		if( endOfTriple == string::npos ) endOfTriple = length;
		firstSlashOffset = s.find( '/', offset );
		if( firstSlashOffset != string::npos ) {
			secondSlashOffset = s.find( '/', firstSlashOffset + 1 );
			if( secondSlashOffset > endOfTriple ) secondSlashOffset = string::npos;
		}
		else
			secondSlashOffset = string::npos;
		
		// process the vertex index
		int vertexIndex = (firstSlashOffset != string::npos) ? 
            lexical_cast<int>( s.substr( offset, firstSlashOffset - offset ) ) : 
            lexical_cast<int>( s.substr( offset, endOfTriple - offset));
        
		if( vertexIndex < 0 )
			result.mVertexIndices.push_back( group->mBaseVertexOffset + vertexIndex );
		else
			result.mVertexIndices.push_back( vertexIndex - 1 );
			
		// process the tex coord index
		if( includeUVs && ( firstSlashOffset != string::npos ) ) {
			size_t numSize = ( secondSlashOffset == string::npos ) ? ( endOfTriple - firstSlashOffset - 1 ) : secondSlashOffset - firstSlashOffset - 1;
			if( numSize > 0 ) {
				int texCoordIndex = lexical_cast<int>( s.substr( firstSlashOffset + 1, numSize ) );
				if( texCoordIndex < 0 )
					result.mTexCoordIndices.push_back( group->mBaseTexCoordOffset + texCoordIndex );
				else
					result.mTexCoordIndices.push_back( texCoordIndex - 1 );
				if( group->mFaces.empty() )
					group->mHasTexCoords = true;
			}
			else
				group->mHasTexCoords = false;
		}
		else if( group->mFaces.empty() ) // if this is the first face, let's note that this group has no tex coords
			group->mHasTexCoords = false;
			
		// process the normal index
		if( secondSlashOffset != string::npos ) {
			int normalIndex = lexical_cast<int>( s.substr( secondSlashOffset + 1, endOfTriple - secondSlashOffset - 1 ) );
			if( normalIndex < 0 )
				result.mNormalIndices.push_back( group->mBaseNormalOffset + normalIndex );
			else
				result.mNormalIndices.push_back( normalIndex - 1 );
			group->mHasNormals = true;
		}
		else if( group->mFaces.empty() ) // if this is the first face, let's note that this group has no normals
			group->mHasNormals = false;
		
		offset = endOfTriple + 1;
		result.mNumVertices++;
	}
	
	group->mFaces.push_back( result );
}

void ObjLoader::load( size_t groupIndex, TriMesh *destTriMesh, boost::tribool loadNormals, boost::tribool loadTexCoords, bool optimizeVertices )
{
	destTriMesh->clear();

	bool texCoords;
	if( loadTexCoords ) texCoords = true;
	else if( ! loadTexCoords ) texCoords = false;
	else texCoords = mGroups[groupIndex].mHasTexCoords;

	bool normals;
	if( loadNormals ) normals = true;
	else if( ! loadNormals ) normals = false;
	else normals = mGroups[groupIndex].mHasNormals;

	if( ! optimizeVertices ) {
		loadInternalNoOptimize( mGroups[groupIndex], destTriMesh, texCoords, normals );
	}
	else if( normals && texCoords ) {
		map<VertexTriple,int> uniqueVerts;
		loadInternalNormalsTextures( mGroups[groupIndex], uniqueVerts, destTriMesh );
	}
	else if( normals ) {
		map<VertexPair,int> uniqueVerts;
		loadInternalNormals( mGroups[groupIndex], uniqueVerts, destTriMesh );
	}
	else if( texCoords ) {
		map<VertexPair,int> uniqueVerts;
		loadInternalTextures( mGroups[groupIndex], uniqueVerts, destTriMesh );
	}
	else {
		map<int,int> uniqueVerts;
		loadInternal( mGroups[groupIndex], uniqueVerts, destTriMesh );
	}

}

void ObjLoader::load( TriMesh *destTriMesh, boost::tribool loadNormals, boost::tribool loadTexCoords, bool optimizeVertices )
{
	destTriMesh->clear();

	// sort out if we're loading texCoords
	bool texCoords, normals;
	if( loadTexCoords ) texCoords = true;
	else if( ! loadTexCoords ) texCoords = false;
	else { // determine if any groups have texCoords
		texCoords = false;
		for( vector<Group>::const_iterator groupIt = mGroups.begin(); groupIt != mGroups.end(); ++groupIt ) {
			if( groupIt->mHasTexCoords )
				texCoords = true;
		}
	
	}

	// sort out if we're loading normals
	if( loadNormals ) normals = true;
	else if( ! loadNormals ) normals = false;
	else { // determine if any groups have normals
		normals = false;
		for( vector<Group>::const_iterator groupIt = mGroups.begin(); groupIt != mGroups.end(); ++groupIt ) {
			if( groupIt->mHasNormals )
				normals = true;
		}
	
	}

	if( ! optimizeVertices ) {
		for( vector<Group>::const_iterator groupIt = mGroups.begin(); groupIt != mGroups.end(); ++groupIt ) {
			loadInternalNoOptimize( *groupIt, destTriMesh, texCoords, normals );
		}	
	}
	else if( normals && texCoords ) {
		map<VertexTriple,int> uniqueVerts;
		for( vector<Group>::const_iterator groupIt = mGroups.begin(); groupIt != mGroups.end(); ++groupIt )
			loadInternalNormalsTextures( *groupIt, uniqueVerts, destTriMesh );
	}
	else if( normals ) {
		map<VertexPair,int> uniqueVerts;
		for( vector<Group>::const_iterator groupIt = mGroups.begin(); groupIt != mGroups.end(); ++groupIt )
			loadInternalNormals( *groupIt, uniqueVerts, destTriMesh );
	}
	else if( texCoords ) {
		map<VertexPair,int> uniqueVerts;
		for( vector<Group>::const_iterator groupIt = mGroups.begin(); groupIt != mGroups.end(); ++groupIt )
			loadInternalTextures( *groupIt, uniqueVerts, destTriMesh );
	}
	else {
		map<int,int> uniqueVerts;
		for( vector<Group>::const_iterator groupIt = mGroups.begin(); groupIt != mGroups.end(); ++groupIt )
			loadInternal( *groupIt, uniqueVerts, destTriMesh );
	}
}

void ObjLoader::loadInternalNoOptimize( const Group &group, TriMesh *destTriMesh, bool texCoords, bool normals )
{
    bool hasColors = mMaterials.size() > 0;
	size_t offset = destTriMesh->getNumVertices();
	for( size_t f = 0; f < group.mFaces.size(); ++f ) {
		Vec3f normal;
		if( normals && ( ! group.mHasNormals ) ) { // we'll have to derive it from two edges
			Vec3f edge1 = mVertices[group.mFaces[f].mVertexIndices[1]] - mVertices[group.mFaces[f].mVertexIndices[0]];
			Vec3f edge2 = mVertices[group.mFaces[f].mVertexIndices[2]] - mVertices[group.mFaces[f].mVertexIndices[0]];
			normal = edge1.cross( edge2 ).normalized();
		}
		for( int v = 0; v < group.mFaces[f].mNumVertices; ++v ) {
			destTriMesh->appendVertex( mVertices[group.mFaces[f].mVertexIndices[v]] );
			if( normals && group.mHasNormals )
				destTriMesh->appendNormal( mNormals[group.mFaces[f].mNormalIndices[v]] );
			else if( normals && ( ! group.mHasNormals ) ) { // we'll have to use the one derived from two edges
				destTriMesh->appendNormal( normal );
			}
			if( texCoords && group.mHasTexCoords ) {
				Vec2f texCoord = mTexCoords[group.mFaces[f].mTexCoordIndices[v]];
				texCoord.y = 1.0f - texCoord.y;
				destTriMesh->appendTexCoord( texCoord );	
			}
			else if( texCoords && ( ! group.mHasTexCoords ) ) // we'll have to make some up
				destTriMesh->appendTexCoord( Vec2f::zero() );
            if( hasColors ) {
                if( group.mFaces[f].mMaterial ) {
                    const Material *m = group.mFaces[f].mMaterial;
                    Color rgb(m->Kd[0], m->Kd[1], m->Kd[2]);
                    destTriMesh->appendColorRgb( rgb );
                }
                else {
                    Color rgb(1, 1, 1);
                    destTriMesh->appendColorRgb( rgb );
                }
            }
		}

		int triangles = group.mFaces[f].mNumVertices - 2;
		for( int t = 0; t < triangles; ++t ) {
			destTriMesh->appendTriangle( offset + 0, offset + t + 1, offset + t + 2 );
		}
		offset += group.mFaces[f].mNumVertices;
	}	
}

void ObjLoader::loadInternalNormalsTextures( const Group &group, map<VertexTriple,int> &uniqueVerts, TriMesh *destTriMesh )
{
    bool hasColors = mMaterials.size() > 0;
	for( size_t f = 0; f < group.mFaces.size(); ++f ) {
		Vec3f inferredNormal;
		bool forceUnique = false;
        Color rgb;
        if( hasColors ) {
            const Material *m = group.mFaces[f].mMaterial;
            if( m ) {
                rgb.r = m->Kd[0];
                rgb.g = m->Kd[1];
                rgb.b = m->Kd[2];
            }
            else {
                rgb.r = 1;
                rgb.g = 1;
                rgb.b = 1;
            }
        }
		if( group.mFaces[f].mNormalIndices.empty() ) { // we'll have to derive it from two edges
			Vec3f edge1 = mVertices[group.mFaces[f].mVertexIndices[1]] - mVertices[group.mFaces[f].mVertexIndices[0]];
			Vec3f edge2 = mVertices[group.mFaces[f].mVertexIndices[2]] - mVertices[group.mFaces[f].mVertexIndices[0]];
			inferredNormal = edge1.cross( edge2 ).normalized();
			forceUnique = true;
		}
		
		if( group.mFaces[f].mTexCoordIndices.empty() )
			forceUnique = true;
		
		vector<int> faceIndices;
		faceIndices.reserve( group.mFaces[f].mNumVertices );
		for( int v = 0; v < group.mFaces[f].mNumVertices; ++v ) {
			if( ! forceUnique ) {
				VertexTriple triple = make_tuple( group.mFaces[f].mVertexIndices[v], group.mFaces[f].mTexCoordIndices[v], group.mFaces[f].mNormalIndices[v] );
				pair<map<VertexTriple,int>::iterator,bool> result = uniqueVerts.insert( make_pair( triple, destTriMesh->getVertices().size() ) );
				if( result.second ) { // we've got a new, unique vertex here, so let's append it
					destTriMesh->appendVertex( mVertices[group.mFaces[f].mVertexIndices[v]] );
					destTriMesh->appendNormal( mNormals[group.mFaces[f].mNormalIndices[v]] );
					destTriMesh->appendTexCoord( mTexCoords[group.mFaces[f].mTexCoordIndices[v]] );
                    if( hasColors )
                        destTriMesh->appendColorRgb( rgb );
				}
				// the unique ID of the vertex is appended for this vert
				faceIndices.push_back( result.first->second );
			}
			else { // have to force unique because this group lacks either normals or texCoords
				faceIndices.push_back( destTriMesh->getVertices().size() );

				destTriMesh->appendVertex( mVertices[group.mFaces[f].mVertexIndices[v]] );
				if( ! group.mHasNormals )
					destTriMesh->appendNormal( inferredNormal );
				else
					destTriMesh->appendNormal( mNormals[group.mFaces[f].mNormalIndices[v]] );
				if( ! group.mHasTexCoords )
					destTriMesh->appendTexCoord( Vec2f::zero() );
				else
					destTriMesh->appendTexCoord( mTexCoords[group.mFaces[f].mTexCoordIndices[v]] );
                if( hasColors )
                    destTriMesh->appendColorRgb( rgb );
			}
		}

		int triangles = faceIndices.size() - 2;
		for( int t = 0; t < triangles; ++t ) {
			destTriMesh->appendTriangle( faceIndices[0], faceIndices[t + 1], faceIndices[t + 2] );
		}
	}	
}

void ObjLoader::loadInternalNormals( const Group &group, map<VertexPair,int> &uniqueVerts, TriMesh *destTriMesh )
{
    bool hasColors = mMaterials.size() > 0;
	for( size_t f = 0; f < group.mFaces.size(); ++f ) {
        Color rgb;
        if( hasColors ) {
            const Material *m = group.mFaces[f].mMaterial;
            if( m ) {
                rgb.r = m->Kd[0];
                rgb.g = m->Kd[1];
                rgb.b = m->Kd[2];
            }
            else {
                rgb.r = 1;
                rgb.g = 1;
                rgb.b = 1;
            }
        }
		Vec3f inferredNormal;
		bool forceUnique = false;
		if( group.mFaces[f].mNormalIndices.empty() ) { // we'll have to derive it from two edges
			Vec3f edge1 = mVertices[group.mFaces[f].mVertexIndices[1]] - mVertices[group.mFaces[f].mVertexIndices[0]];
			Vec3f edge2 = mVertices[group.mFaces[f].mVertexIndices[2]] - mVertices[group.mFaces[f].mVertexIndices[0]];
			inferredNormal = edge1.cross( edge2 ).normalized();
			forceUnique = true;
		}
		
		vector<int> faceIndices;
		faceIndices.reserve( group.mFaces[f].mNumVertices );
		for( int v = 0; v < group.mFaces[f].mNumVertices; ++v ) {
			if( ! forceUnique ) {
				VertexPair triple = make_tuple( group.mFaces[f].mVertexIndices[v], group.mFaces[f].mNormalIndices[v] );
				pair<map<VertexPair,int>::iterator,bool> result = uniqueVerts.insert( make_pair( triple, destTriMesh->getVertices().size() ) );
				if( result.second ) { // we've got a new, unique vertex here, so let's append it
					destTriMesh->appendVertex( mVertices[group.mFaces[f].mVertexIndices[v]] );
					destTriMesh->appendNormal( mNormals[group.mFaces[f].mNormalIndices[v]] );
                    if( hasColors )
                        destTriMesh->appendColorRgb( rgb );
				}
				// the unique ID of the vertex is appended for this vert
				faceIndices.push_back( result.first->second );
			}
			else { // have to force unique because this group lacks normals
				faceIndices.push_back( destTriMesh->getVertices().size() );

				destTriMesh->appendVertex( mVertices[group.mFaces[f].mVertexIndices[v]] );
				if( ! group.mHasNormals )
					destTriMesh->appendNormal( inferredNormal );
				else
					destTriMesh->appendNormal( mNormals[group.mFaces[f].mNormalIndices[v]] );
                if( hasColors )
                    destTriMesh->appendColorRgb( rgb );
			}
		}

		int triangles = faceIndices.size() - 2;
		for( int t = 0; t < triangles; ++t ) {
			destTriMesh->appendTriangle( faceIndices[0], faceIndices[t + 1], faceIndices[t + 2] );
		}
	}	
}

void ObjLoader::loadInternalTextures( const Group &group, map<VertexPair,int> &uniqueVerts, TriMesh *destTriMesh )
{
    bool hasColors = mMaterials.size() > 0;
	for( size_t f = 0; f < group.mFaces.size(); ++f ) {
        Color rgb;
        if( hasColors ) {
            const Material *m = group.mFaces[f].mMaterial;
            if( m ) {
                rgb.r = m->Kd[0];
                rgb.g = m->Kd[1];
                rgb.b = m->Kd[2];
            }
            else {
                rgb.r = 1;
                rgb.g = 1;
                rgb.b = 1;
            }
        }
		bool forceUnique = false;
		if( group.mFaces[f].mTexCoordIndices.empty() )
			forceUnique = true;
		
		vector<int> faceIndices;
		faceIndices.reserve( group.mFaces[f].mNumVertices );
		for( int v = 0; v < group.mFaces[f].mNumVertices; ++v ) {
			if( ! forceUnique ) {
				VertexPair triple = make_tuple( group.mFaces[f].mVertexIndices[v], group.mFaces[f].mTexCoordIndices[v] );
				pair<map<VertexPair,int>::iterator,bool> result = uniqueVerts.insert( make_pair( triple, destTriMesh->getVertices().size() ) );
				if( result.second ) { // we've got a new, unique vertex here, so let's append it
					destTriMesh->appendVertex( mVertices[group.mFaces[f].mVertexIndices[v]] );
					destTriMesh->appendTexCoord( mTexCoords[group.mFaces[f].mTexCoordIndices[v]] );
                    if( hasColors )
                        destTriMesh->appendColorRgb( rgb );
				}
				// the unique ID of the vertex is appended for this vert
				faceIndices.push_back( result.first->second );
			}
			else { // have to force unique because this group lacks texCoords
				faceIndices.push_back( destTriMesh->getVertices().size() );

				destTriMesh->appendVertex( mVertices[group.mFaces[f].mVertexIndices[v]] );
				if( ! group.mHasTexCoords )
					destTriMesh->appendTexCoord( Vec2f::zero() );
				else
					destTriMesh->appendTexCoord( mTexCoords[group.mFaces[f].mTexCoordIndices[v]] );
                if( hasColors )
                    destTriMesh->appendColorRgb( rgb );
			}
		}

		int triangles = faceIndices.size() - 2;
		for( int t = 0; t < triangles; ++t ) {
			destTriMesh->appendTriangle( faceIndices[0], faceIndices[t + 1], faceIndices[t + 2] );
		}
	}	
}

void ObjLoader::loadInternal( const Group &group, map<int,int> &uniqueVerts, TriMesh *destTriMesh )
{
    bool hasColors = mMaterials.size() > 0;
	for( size_t f = 0; f < group.mFaces.size(); ++f ) {
        Color rgb;
        if( hasColors ) {
            const Material *m = group.mFaces[f].mMaterial;
            if( m ) {
                rgb.r = m->Kd[0];
                rgb.g = m->Kd[1];
                rgb.b = m->Kd[2];
            }
            else {
                rgb.r = 1;
                rgb.g = 1;
                rgb.b = 1;
            }
        }
		vector<int> faceIndices;
		faceIndices.reserve( group.mFaces[f].mNumVertices );
		for( int v = 0; v < group.mFaces[f].mNumVertices; ++v ) {
			pair<map<int,int>::iterator,bool> result = uniqueVerts.insert( make_pair( group.mFaces[f].mVertexIndices[v], destTriMesh->getVertices().size() ) );
			if( result.second ) { // we've got a new, unique vertex here, so let's append it
				destTriMesh->appendVertex( mVertices[group.mFaces[f].mVertexIndices[v]] );
                if( hasColors )
                    destTriMesh->appendColorRgb( rgb );
			}
			// the unique ID of the vertex is appended for this vert
			faceIndices.push_back( result.first->second );
		}

		int triangles = faceIndices.size() - 2;
		for( int t = 0; t < triangles; ++t ) {
			destTriMesh->appendTriangle( faceIndices[0], faceIndices[t + 1], faceIndices[t + 2] );
		}
	}	
}

void ObjLoader::write( DataTargetRef dataTarget, const TriMesh &mesh, bool writeNormals, bool includeUVs )
{
	OStreamRef stream = dataTarget->getStream();
	const size_t numVerts = mesh.getNumVertices();
	for( size_t p = 0; p < numVerts; ++p ) {
		ostringstream os;
		os << "v " << mesh.getVertices()[p].x << " " << mesh.getVertices()[p].y << " " << mesh.getVertices()[p].z << std::endl;
		stream->writeData( os.str().c_str(), os.str().length() );
	}

	const bool processTexCoords = mesh.hasTexCoords() && includeUVs;
	if( processTexCoords ) {
		for( size_t p = 0; p < numVerts; ++p ) {
			ostringstream os;
			os << "vt " << mesh.getTexCoords()[p].x << " " << mesh.getTexCoords()[p].y << std::endl;
			stream->writeData( os.str().c_str(), os.str().length() );
		}
	}
	
	const bool processNormals = mesh.hasNormals() && writeNormals;
	if( processNormals ) {
		for( size_t p = 0; p < numVerts; ++p ) {
			ostringstream os;
			os << "vn " << mesh.getNormals()[p].x << " " << mesh.getNormals()[p].y << " " << mesh.getNormals()[p].z << std::endl;
			stream->writeData( os.str().c_str(), os.str().length() );
		}
	}
	
	const size_t numTriangles = mesh.getNumTriangles();
	const std::vector<uint32_t>& indices( mesh.getIndices() );
	for( size_t t = 0; t < numTriangles; ++t ) {
		ostringstream os;
		os << "f ";
		if( processNormals && processTexCoords ) {
			os << indices[t*3+0]+1 << "/" << indices[t*3+0]+1 << "/" << indices[t*3+0]+1 << " ";
			os << indices[t*3+1]+1 << "/" << indices[t*3+1]+1 << "/" << indices[t*3+1]+1 << " ";
			os << indices[t*3+2]+1 << "/" << indices[t*3+2]+1 << "/" << indices[t*3+2]+1 << " ";
		}
		else if ( processNormals ) {
			os << indices[t*3+0]+1 << "//" << indices[t*3+0]+1 << " ";
			os << indices[t*3+1]+1 << "//" << indices[t*3+1]+1 << " ";
			os << indices[t*3+2]+1 << "//" << indices[t*3+2]+1 << " ";
		}
		else if( processTexCoords ) {
			os << indices[t*3+0]+1 << "/" << indices[t*3+0]+1 << " ";
			os << indices[t*3+1]+1 << "/" << indices[t*3+1]+1 << " ";
			os << indices[t*3+2]+1 << "/" << indices[t*3+2]+1 << " ";
		}
		else { // just verts
			os << indices[t*3+0]+1 << " ";
			os << indices[t*3+1]+1 << " ";
			os << indices[t*3+2]+1 << " ";			
		}
		os << std::endl;
		stream->writeData( os.str().c_str(), os.str().length() );
	}
}

} // namespace cinder
