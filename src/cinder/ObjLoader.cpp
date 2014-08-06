/*
 Copyright (c) 2013, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

geom::SourceRef	loadGeom( const fs::path &path )
{
return geom::SourceRef();
}

ObjLoader::ObjLoader( shared_ptr<IStreamCinder> stream, bool includeUVs )
	: mStream( stream )
{
	parse( includeUVs );
	load();
}

ObjLoader::ObjLoader( DataSourceRef dataSource, bool includeUVs )
	: mStream( dataSource->createStream() )
{
	parse( includeUVs );
	load();
}

ObjLoader::ObjLoader( DataSourceRef dataSource, DataSourceRef materialSource, bool includeUVs )
    : mStream( dataSource->createStream() )
{
    parseMaterial( materialSource->createStream() );
    parse( includeUVs );
	load();	
}

void ObjLoader::loadInto( geom::Target *target ) const
{
	// copy attributes
	if( getAttribDims( geom::Attrib::POSITION ) )
		target->copyAttrib( geom::Attrib::POSITION, getAttribDims( geom::Attrib::POSITION ), 0, (const float*)mOutputVertices.data(), getNumVertices() );
	if( getAttribDims( geom::Attrib::COLOR ) )
		target->copyAttrib( geom::Attrib::COLOR, getAttribDims( geom::Attrib::COLOR ), 0, (const float*)mOutputColors.data(), std::min( mOutputColors.size(), mOutputVertices.size() ) );
	if( getAttribDims( geom::Attrib::TEX_COORD_0 ) )
		target->copyAttrib( geom::Attrib::TEX_COORD_0, getAttribDims( geom::Attrib::TEX_COORD_0 ), 0, (const float*)mOutputTexCoords.data(), std::min( mOutputTexCoords.size(), mOutputVertices.size() ) );
	if( getAttribDims( geom::Attrib::NORMAL ) )
		target->copyAttrib( geom::Attrib::NORMAL, getAttribDims( geom::Attrib::NORMAL ), 0, (const float*)mOutputNormals.data(), std::min( mOutputNormals.size(), mOutputVertices.size() ) );
	
	// copy indices
	if( getNumIndices() )
		target->copyIndices( geom::Primitive::TRIANGLES, mIndices.data(), getNumIndices(), 4 /* bytes per index */ );
}

uint8_t	ObjLoader::getAttribDims( geom::Attrib attr ) const
{
	switch( attr ) {
		case geom::Attrib::POSITION: return mOutputVertices.empty() ? 0 : 3;
		case geom::Attrib::COLOR: return mOutputColors.empty() ? 0 : 3;
		case geom::Attrib::TEX_COORD_0: return mOutputTexCoords.empty() ? 0 : 2;
		case geom::Attrib::NORMAL: return mOutputNormals.empty() ? 0 : 3;
		default:
			return 0;
	}
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
			mInternalVertices.push_back( v );
		}
		else if( tag == "vt" ) { // vertex texture coordinates
			if( includeUVs ) {
				Vec2f tex;
				ss >> tex.x >> tex.y;
				mInternalTexCoords.push_back( tex );
			}
		}
		else if( tag == "vn" ) { // vertex normals
			Vec3f v;
			ss >> v.x >> v.y >> v.z;
			mInternalNormals.push_back( v.normalized() );
		}
		else if( tag == "f" ) { // face
			parseFace( currentGroup, currentMaterial, line, includeUVs );
		}
		else if( tag == "g" ) { // group
			if( ! currentGroup->mFaces.empty() )
				mGroups.push_back( Group() );
			currentGroup = &mGroups[mGroups.size()-1];
			currentGroup->mBaseVertexOffset = mInternalVertices.size();
			currentGroup->mBaseTexCoordOffset = mInternalTexCoords.size();
			currentGroup->mBaseNormalOffset = mInternalNormals.size();
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

void ObjLoader::load( size_t groupIndex, boost::tribool loadNormals, boost::tribool loadTexCoords )
{
	bool texCoords;
	if( loadTexCoords ) texCoords = true;
	else if( ! loadTexCoords ) texCoords = false;
	else texCoords = mGroups[groupIndex].mHasTexCoords;

	bool normals;
	if( loadNormals ) normals = true;
	else if( ! loadNormals ) normals = false;
	else normals = mGroups[groupIndex].mHasNormals;

	if( normals && texCoords ) {
		map<VertexTriple,int> uniqueVerts;
		loadGroupNormalsTextures( mGroups[groupIndex], uniqueVerts );
	}
	else if( normals ) {
		map<VertexPair,int> uniqueVerts;
		loadGroupNormals( mGroups[groupIndex], uniqueVerts );
	}
	else if( texCoords ) {
		map<VertexPair,int> uniqueVerts;
		loadGroupTextures( mGroups[groupIndex], uniqueVerts );
	}
	else {
		map<int,int> uniqueVerts;
		loadGroup( mGroups[groupIndex], uniqueVerts );
	}

}

void ObjLoader::load( boost::tribool loadNormals, boost::tribool loadTexCoords )
{
	// sort out if we're loading texCoords
	bool texCoords, normals;
	if( loadTexCoords ) texCoords = true;
	else if( ! loadTexCoords ) texCoords = false;
	else { // determine if any groups have texCoords
		texCoords = false;
		for( vector<Group>::const_iterator groupIt = mGroups.begin(); groupIt != mGroups.end(); ++groupIt ) {
			if( groupIt->mHasTexCoords ) {
				texCoords = true;
				break;
			}
		}
	}

	// sort out if we're loading normals
	if( loadNormals ) normals = true;
	else if( ! loadNormals ) normals = false;
	else { // determine if any groups have normals
		normals = false;
		for( vector<Group>::const_iterator groupIt = mGroups.begin(); groupIt != mGroups.end(); ++groupIt ) {
			if( groupIt->mHasNormals ) {
				normals = true;
				break;
			}
		}
	}

	if( normals && texCoords ) {
		map<VertexTriple,int> uniqueVerts;
		for( vector<Group>::const_iterator groupIt = mGroups.begin(); groupIt != mGroups.end(); ++groupIt )
			loadGroupNormalsTextures( *groupIt, uniqueVerts );
	}
	else if( normals ) {
		map<VertexPair,int> uniqueVerts;
		for( vector<Group>::const_iterator groupIt = mGroups.begin(); groupIt != mGroups.end(); ++groupIt )
			loadGroupNormals( *groupIt, uniqueVerts );
	}
	else if( texCoords ) {
		map<VertexPair,int> uniqueVerts;
		for( vector<Group>::const_iterator groupIt = mGroups.begin(); groupIt != mGroups.end(); ++groupIt )
			loadGroupTextures( *groupIt, uniqueVerts );
	}
	else {
		map<int,int> uniqueVerts;
		for( vector<Group>::const_iterator groupIt = mGroups.begin(); groupIt != mGroups.end(); ++groupIt )
			loadGroup( *groupIt, uniqueVerts );
	}
}

void ObjLoader::loadGroupNormalsTextures( const Group &group, map<VertexTriple,int> &uniqueVerts )
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
			Vec3f edge1 = mInternalVertices[group.mFaces[f].mVertexIndices[1]] - mInternalVertices[group.mFaces[f].mVertexIndices[0]];
			Vec3f edge2 = mInternalVertices[group.mFaces[f].mVertexIndices[2]] - mInternalVertices[group.mFaces[f].mVertexIndices[0]];
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
				pair<map<VertexTriple,int>::iterator,bool> result = uniqueVerts.insert( make_pair( triple, mOutputVertices.size() ) );
				if( result.second ) { // we've got a new, unique vertex here, so let's append it
					mOutputVertices.push_back( mInternalVertices[group.mFaces[f].mVertexIndices[v]] );
					mOutputNormals.push_back( mInternalNormals[group.mFaces[f].mNormalIndices[v]] );
					mOutputTexCoords.push_back( mInternalTexCoords[group.mFaces[f].mTexCoordIndices[v]] );
					if( hasColors )
						mOutputColors.push_back( rgb );
				}
				// the unique ID of the vertex is appended for this vert
				faceIndices.push_back( result.first->second );
			}
			else { // have to force unique because this group lacks either normals or texCoords
				faceIndices.push_back( mOutputVertices.size() );

				mOutputVertices.push_back( mInternalVertices[group.mFaces[f].mVertexIndices[v]] );
				if( ! group.mHasNormals )
					mOutputNormals.push_back( inferredNormal );
				else
					mOutputNormals.push_back( mInternalNormals[group.mFaces[f].mNormalIndices[v]] );
				if( ! group.mHasTexCoords )
					mOutputTexCoords.push_back( Vec2f::zero() );
				else
					mOutputTexCoords.push_back( mInternalTexCoords[group.mFaces[f].mTexCoordIndices[v]] );
                if( hasColors )
                    mOutputColors.push_back( rgb );
			}
		}

		int triangles = faceIndices.size() - 2;
		for( int t = 0; t < triangles; ++t ) {
			mIndices.push_back( faceIndices[0] ); mIndices.push_back( faceIndices[t + 1] ); mIndices.push_back( faceIndices[t + 2] );
		}
	}	
}

void ObjLoader::loadGroupNormals( const Group &group, map<VertexPair,int> &uniqueVerts )
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
			Vec3f edge1 = mInternalVertices[group.mFaces[f].mVertexIndices[1]] - mInternalVertices[group.mFaces[f].mVertexIndices[0]];
			Vec3f edge2 = mInternalVertices[group.mFaces[f].mVertexIndices[2]] - mInternalVertices[group.mFaces[f].mVertexIndices[0]];
			inferredNormal = edge1.cross( edge2 ).normalized();
			forceUnique = true;
		}
		
		vector<int> faceIndices;
		faceIndices.reserve( group.mFaces[f].mNumVertices );
		for( int v = 0; v < group.mFaces[f].mNumVertices; ++v ) {
			if( ! forceUnique ) {
				VertexPair triple = make_tuple( group.mFaces[f].mVertexIndices[v], group.mFaces[f].mNormalIndices[v] );
				pair<map<VertexPair,int>::iterator,bool> result = uniqueVerts.insert( make_pair( triple, mOutputVertices.size() ) );
				if( result.second ) { // we've got a new, unique vertex here, so let's append it
					mOutputVertices.push_back( mInternalVertices[group.mFaces[f].mVertexIndices[v]] );
					mOutputNormals.push_back( mInternalNormals[group.mFaces[f].mNormalIndices[v]] );
                    if( hasColors )
                        mOutputColors.push_back( rgb );
				}
				// the unique ID of the vertex is appended for this vert
				faceIndices.push_back( result.first->second );
			}
			else { // have to force unique because this group lacks normals
				faceIndices.push_back( mOutputVertices.size() );

				mOutputVertices.push_back( mInternalVertices[group.mFaces[f].mVertexIndices[v]] );
				if( ! group.mHasNormals )
					mOutputNormals.push_back( inferredNormal );
				else
					mOutputNormals.push_back( mInternalNormals[group.mFaces[f].mNormalIndices[v]] );
                if( hasColors )
                    mOutputColors.push_back( rgb );
			}
		}

		int triangles = faceIndices.size() - 2;
		for( int t = 0; t < triangles; ++t ) {
			mIndices.push_back( faceIndices[0] ); mIndices.push_back( faceIndices[t + 1] ); mIndices.push_back( faceIndices[t + 2] );
		}
	}	
}

void ObjLoader::loadGroupTextures( const Group &group, map<VertexPair,int> &uniqueVerts )
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
				pair<map<VertexPair,int>::iterator,bool> result = uniqueVerts.insert( make_pair( triple, mOutputVertices.size() ) );
				if( result.second ) { // we've got a new, unique vertex here, so let's append it
					mOutputVertices.push_back( mInternalVertices[group.mFaces[f].mVertexIndices[v]] );
					mOutputTexCoords.push_back( mInternalTexCoords[group.mFaces[f].mTexCoordIndices[v]] );
                    if( hasColors )
                        mOutputColors.push_back( rgb );
				}
				// the unique ID of the vertex is appended for this vert
				faceIndices.push_back( result.first->second );
			}
			else { // have to force unique because this group lacks texCoords
				faceIndices.push_back( mOutputVertices.size() );

				mOutputVertices.push_back( mInternalVertices[group.mFaces[f].mVertexIndices[v]] );
				if( ! group.mHasTexCoords )
					mOutputTexCoords.push_back( Vec2f::zero() );
				else
					mOutputTexCoords.push_back( mInternalTexCoords[group.mFaces[f].mTexCoordIndices[v]] );
                if( hasColors )
                    mOutputColors.push_back( rgb );
			}
		}

		int triangles = faceIndices.size() - 2;
		for( int t = 0; t < triangles; ++t ) {
			mIndices.push_back( faceIndices[0] ); mIndices.push_back( faceIndices[t + 1] ); mIndices.push_back( faceIndices[t + 2] );
		}
	}	
}

void ObjLoader::loadGroup( const Group &group, map<int,int> &uniqueVerts )
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
			pair<map<int,int>::iterator,bool> result = uniqueVerts.insert( make_pair( group.mFaces[f].mVertexIndices[v], mOutputVertices.size() ) );
			if( result.second ) { // we've got a new, unique vertex here, so let's append it
				mOutputVertices.push_back( mInternalVertices[group.mFaces[f].mVertexIndices[v]] );
                if( hasColors )
                    mOutputColors.push_back( rgb );
			}
			// the unique ID of the vertex is appended for this vert
			faceIndices.push_back( result.first->second );
		}

		int triangles = faceIndices.size() - 2;
		for( int t = 0; t < triangles; ++t ) {
			mIndices.push_back( faceIndices[0] ); mIndices.push_back( faceIndices[t + 1] ); mIndices.push_back( faceIndices[t + 2] );
		}
	}	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OBJ Writing
namespace {
class ObjWriteTarget : public geom::Target {
  public:
	ObjWriteTarget( OStreamRef stream, bool includeNormals, bool includeTexCoords )
		: mStream( stream ), mIncludeNormals( includeNormals ), mIncludeTexCoords( includeTexCoords )
	{
		mHasNormals = mHasTexCoords = false;
	}
	
	virtual geom::Primitive	getPrimitive() const override;
	virtual uint8_t	getAttribDims( geom::Attrib attr ) const override;
	virtual void copyAttrib( geom::Attrib attr, uint8_t dims, size_t strideBytes, const float *srcData, size_t count ) override;
	virtual void copyIndices( geom::Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytesPerIndex ) override;
	
  protected:
	void writeData( const std::string &typeSpecifier, uint8_t dims, size_t strideBytes, const float *srcData, size_t count );

	OStreamRef		mStream;
	bool			mIncludeTexCoords, mIncludeNormals;
	bool			mHasTexCoords, mHasNormals;
};

geom::Primitive	ObjWriteTarget::getPrimitive() const
{
	return geom::Primitive::TRIANGLES;
}

uint8_t	ObjWriteTarget::getAttribDims( geom::Attrib attr ) const
{
	switch( attr ) {
		case geom::Attrib::POSITION: return 3;
		case geom::Attrib::COLOR: return 3;
		case geom::Attrib::TEX_COORD_0: return 2;
		case geom::Attrib::NORMAL: return 3;
		default:
			return 0;
	}
}

void ObjWriteTarget::writeData( const std::string &typeSpecifier, uint8_t dims, size_t strideBytes, const float *srcData, size_t count )
{
	if( strideBytes == 0 )
		strideBytes = sizeof(float) * dims;
	for( size_t v = 0; v < count; ++v ) {
		const float *data = (const float*)(((const uint8_t*)srcData) + v * strideBytes);
		ostringstream os;
		os << typeSpecifier << " ";
		for( uint8_t d = 0; d < dims; ++d ) {
			os << data[d];
			if( d == dims - 1 ) os << std::endl;
			else os << ' ';
		}
		mStream->writeData( os.str().c_str(), os.str().length() );
	}
}

void ObjWriteTarget::copyAttrib( geom::Attrib attr, uint8_t dims, size_t strideBytes, const float *srcData, size_t count )
{
	switch( attr ) {
		case geom::Attrib::POSITION: {
			if( dims == 2 ) { // we need to convert to 3d
				if( strideBytes == 0 )
					strideBytes = sizeof(float) * 2;
				unique_ptr<float[]> tempData( new float[count * 3] );
				for( int i = 0; i < count; ++i ) {
					tempData.get()[i*3+0] = ((const float*)(((const uint8_t*)srcData) + i*strideBytes))[0];
					tempData.get()[i*3+1] = ((const float*)(((const uint8_t*)srcData) + i*strideBytes))[1];
					tempData.get()[i*3+2] = 0;
				}
				writeData( "v", 3, 0, tempData.get(), count );
			}
			else
				writeData( "v", dims, strideBytes, srcData, count );
		}
		break;
		case geom::Attrib::TEX_COORD_0:
			if( mIncludeTexCoords ) {
				writeData( "vt", dims, strideBytes, srcData, count );
				mHasTexCoords = true;
			}
		break;
		case geom::Attrib::NORMAL:
			if( mIncludeNormals ) {
				writeData( "vn", dims, strideBytes, srcData, count );
				mHasNormals = true;
			}
		break;
		default:
		break;
	}
}

void ObjWriteTarget::copyIndices( geom::Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytesPerIndex )
{
	for( size_t i = 0; i < numIndices; i += 3 ) {
		ostringstream os;
		os << "f ";
		if( mHasNormals && mHasTexCoords ) {
			os << source[i+0]+1 << "/" << source[i+0]+1 << "/" << source[i+0]+1 << " ";
			os << source[i+1]+1 << "/" << source[i+1]+1 << "/" << source[i+1]+1 << " ";
			os << source[i+2]+1 << "/" << source[i+2]+1 << "/" << source[i+2]+1 << " ";
		}
		else if( mHasNormals ) {
			os << source[i+0]+1 << "//" << source[i+0]+1 << " ";
			os << source[i+1]+1 << "//" << source[i+1]+1 << " ";
			os << source[i+2]+1 << "//" << source[i+2]+1 << " ";
		}
		else if( mHasTexCoords ) {
			os << source[i+0]+1 << "/" << source[i+0]+1 << " ";
			os << source[i+1]+1 << "/" << source[i+1]+1 << " ";
			os << source[i+2]+1 << "/" << source[i+2]+1 << " ";
		}
		else { // just verts
			os << source[i+0]+1 << " ";
			os << source[i+1]+1 << " ";
			os << source[i+2]+1 << " ";			
		}
		os << std::endl;
		mStream->writeData( os.str().c_str(), os.str().length() );
	}
}
} // anonymous namespace

void objWrite( DataTargetRef dataTarget, const geom::Source &source, bool includeNormals, bool includeTexCoords )
{
	OStreamRef stream = dataTarget->getStream();
	
	unique_ptr<ObjWriteTarget> target( new ObjWriteTarget( stream, includeNormals, includeTexCoords ) );
	source.loadInto( target.get() );	
	
	if( source.getNumIndices() == 0 )
		target->generateIndices( geom::Primitive::TRIANGLES, source.getNumVertices() );
}

} // namespace cinder
