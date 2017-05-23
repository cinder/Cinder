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

#include <sstream>
using namespace std;

// For stoi
#if defined( CINDER_ANDROID )
	#include "cinder/android/CinderAndroid.h"
#endif

namespace cinder {

ObjLoader::ObjLoader( shared_ptr<IStreamCinder> stream, bool includeNormals, bool includeTexCoords, bool optimize )
	: mStream( stream ), mOutputCached( false ), mOptimizeVertices( optimize ), mGroupIndex( numeric_limits<size_t>::max() )
{
	parse( includeNormals, includeTexCoords );
}

ObjLoader::ObjLoader( DataSourceRef dataSource, bool includeNormals, bool includeTexCoords, bool optimize )
	: mStream( dataSource->createStream() ), mOutputCached( false ), mOptimizeVertices( optimize ), mGroupIndex( numeric_limits<size_t>::max() )
{
	parse( includeNormals, includeTexCoords );
}

ObjLoader::ObjLoader( DataSourceRef dataSource, DataSourceRef materialSource, bool includeNormals, bool includeTexCoords, bool optimize )
	: mStream( dataSource->createStream() ), mOutputCached( false ), mOptimizeVertices( optimize ), mGroupIndex( numeric_limits<size_t>::max() )
{
	parseMaterial( materialSource->createStream() );
	parse( includeNormals, includeTexCoords );
}

ObjLoader& ObjLoader::groupIndex( size_t groupIndex )
{
	if ( groupIndex < mGroups.size() ) {
		if ( groupIndex != mGroupIndex ) {
			mGroupIndex = groupIndex;
			mOutputCached = false;
		}
	}

	return *this;
}

ObjLoader& ObjLoader::groupName( const std::string &groupName )
{
	auto it = std::find_if( mGroups.begin(), mGroups.end(), [&] ( const Group &group ) {
		return group.mName == groupName;
	} );

	if ( it != mGroups.end() ) {
		size_t groupIndex = std::distance( mGroups.begin(), it );
		if ( groupIndex != mGroupIndex ) {
			mGroupIndex = groupIndex;
			mOutputCached = false;
		}
	}

	return *this;
}

bool ObjLoader::hasGroup( const std::string &groupName ) const
{
	auto it = std::find_if( mGroups.begin(), mGroups.end(), [&] ( const Group &group ) {
		return group.mName == groupName;
	} );

	return it != mGroups.end();
}

void ObjLoader::loadInto( geom::Target *target, const geom::AttribSet & /*requestedAttribs*/ ) const
{
	load();

	// copy attributes
	if( getAttribDims( geom::Attrib::POSITION ) )
		target->copyAttrib( geom::Attrib::POSITION, getAttribDims( geom::Attrib::POSITION ), 0, (const float*)mOutputVertices.data(), getNumVertices() );
	if( getAttribDims( geom::Attrib::NORMAL ) )
		target->copyAttrib( geom::Attrib::NORMAL, getAttribDims( geom::Attrib::NORMAL ), 0, (const float*)mOutputNormals.data(), std::min( mOutputNormals.size(), mOutputVertices.size() ) );
	if( getAttribDims( geom::Attrib::TEX_COORD_0 ) )
		target->copyAttrib( geom::Attrib::TEX_COORD_0, getAttribDims( geom::Attrib::TEX_COORD_0 ), 0, (const float*)mOutputTexCoords.data(), std::min( mOutputTexCoords.size(), mOutputVertices.size() ) );
	if( getAttribDims( geom::Attrib::COLOR ) )
		target->copyAttrib( geom::Attrib::COLOR, getAttribDims( geom::Attrib::COLOR ), 0, (const float*)mOutputColors.data(), std::min( mOutputColors.size(), mOutputVertices.size() ) );

	// copy indices
	if( getNumIndices() )
		target->copyIndices( geom::Primitive::TRIANGLES, mOutputIndices.data(), getNumIndices(), 4 /* bytes per index */ );
}

uint8_t	ObjLoader::getAttribDims( geom::Attrib attr ) const
{
	load();

	switch( attr ) {
		case geom::Attrib::POSITION: return mOutputVertices.empty() ? 0 : 3;
		case geom::Attrib::NORMAL: return mOutputNormals.empty() ? 0 : 3;
		case geom::Attrib::TEX_COORD_0: return mOutputTexCoords.empty() ? 0 : 2;
		case geom::Attrib::COLOR: return mOutputColors.empty() ? 0 : 3;
		default:
			return 0;
	}
}

geom::AttribSet	ObjLoader::getAvailableAttribs() const
{
	load();

	geom::AttribSet result;

	if( ! mOutputVertices.empty() )
		result.insert( geom::Attrib::POSITION );
	if( ! mOutputNormals.empty() )
		result.insert( geom::Attrib::NORMAL );
	if( ! mOutputTexCoords.empty() )
		result.insert( geom::Attrib::TEX_COORD_0 );
	if( ! mOutputColors.empty() )
		result.insert( geom::Attrib::COLOR );

	return result;
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

		while( line.back() == '\\' && ! material->isEof() ) {
			auto next = material->readLine();
			line = line.substr( 0, line.size() - 1 ) + next;
		}

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

void ObjLoader::parse( bool includeNormals, bool includeTexCoords )
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

		while( line.back() == '\\' && ! mStream->isEof() ) {
			auto next = mStream->readLine();
			line = line.substr( 0, line.size() - 1 ) + next;
		}

		stringstream ss( line );
		ss >> tag;
		if( tag == "v" ) { // vertex
			vec3 v;
			ss >> v.x >> v.y >> v.z;
			mInternalVertices.push_back( v );
		}
		else if( tag == "vt" ) { // vertex texture coordinates
			if( includeTexCoords ) {
				vec2 tex;
				ss >> tex.x >> tex.y;
				mInternalTexCoords.push_back( tex );
			}
		}
		else if( tag == "vn" ) { // vertex normals
			if ( includeNormals ) {
				vec3 v;
				ss >> v.x >> v.y >> v.z;
				mInternalNormals.push_back( normalize( v ) );
			}
		}
		else if( tag == "f" ) { // face
			parseFace( currentGroup, currentMaterial, line, includeNormals, includeTexCoords );
		}
		else if( tag == "g" ) { // group
			if( ! currentGroup->mFaces.empty() )
				mGroups.push_back( Group() );
			currentGroup = &mGroups[mGroups.size()-1];
			currentGroup->mBaseVertexOffset = (int32_t)mInternalVertices.size();
			currentGroup->mBaseTexCoordOffset = (int32_t)mInternalTexCoords.size();
			currentGroup->mBaseNormalOffset = (int32_t)mInternalNormals.size();
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

void ObjLoader::parseFace( Group *group, const Material *material, const std::string &s, bool includeNormals, bool includeTexCoords )
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
            stoi( s.substr( offset, firstSlashOffset - offset ) ) :
            stoi( s.substr( offset, endOfTriple - offset));

		if( vertexIndex < 0 )
			result.mVertexIndices.push_back( group->mBaseVertexOffset + vertexIndex );
		else
			result.mVertexIndices.push_back( vertexIndex - 1 );

		// process the tex coord index
		if( includeTexCoords && ( firstSlashOffset != string::npos ) ) {
			size_t numSize = ( secondSlashOffset == string::npos ) ? ( endOfTriple - firstSlashOffset - 1 ) : secondSlashOffset - firstSlashOffset - 1;
			if( numSize > 0 ) {
				int texCoordIndex = stoi( s.substr( firstSlashOffset + 1, numSize ) );
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
		if( includeNormals && ( secondSlashOffset != string::npos ) ) {
			int normalIndex = stoi( s.substr( secondSlashOffset + 1, endOfTriple - secondSlashOffset - 1 ) );
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

void ObjLoader::load() const
{
	if( mOutputCached )
		return;

	mOutputVertices.clear();
	mOutputNormals.clear();
	mOutputTexCoords.clear();
	mOutputColors.clear();
	mOutputIndices.clear();

	bool hasGroupIndex = ( mGroupIndex != numeric_limits<size_t>::max() );

	bool texCoords;
	if( hasGroupIndex ) {
		texCoords = mGroups[mGroupIndex].mHasTexCoords;
	}
	else {
		texCoords = false;
		for( vector<Group>::const_iterator groupIt = mGroups.begin(); groupIt != mGroups.end(); ++groupIt ) {
			if( groupIt->mHasTexCoords ) {
				texCoords = true;
				break;
			}
		}
	}

	bool normals;
	if( hasGroupIndex ) {
		normals = mGroups[mGroupIndex].mHasNormals;
	}
	else {
		normals = false;
		for( vector<Group>::const_iterator groupIt = mGroups.begin(); groupIt != mGroups.end(); ++groupIt ) {
			if( groupIt->mHasNormals ) {
				normals = true;
				break;
			}
		}
	}

	if( normals && texCoords ) {
		if( hasGroupIndex ) {
			map<VertexTriple,int> uniqueVerts;
			loadGroupNormalsTextures( mGroups[mGroupIndex], uniqueVerts );
		}
		else {
			map<VertexTriple,int> uniqueVerts;
			for( vector<Group>::const_iterator groupIt = mGroups.begin(); groupIt != mGroups.end(); ++groupIt )
				loadGroupNormalsTextures( *groupIt, uniqueVerts );
		}
	}
	else if( normals ) {
		if( hasGroupIndex ) {
			map<VertexPair,int> uniqueVerts;
			loadGroupNormals( mGroups[mGroupIndex], uniqueVerts );
		}
		else {
			map<VertexPair,int> uniqueVerts;
			for( vector<Group>::const_iterator groupIt = mGroups.begin(); groupIt != mGroups.end(); ++groupIt )
				loadGroupNormals( *groupIt, uniqueVerts );
		}
	}
	else if( texCoords ) {
		if( hasGroupIndex ) {
			map<VertexPair,int> uniqueVerts;
			loadGroupTextures( mGroups[mGroupIndex], uniqueVerts );
		}
		else {
			map<VertexPair,int> uniqueVerts;
			for( vector<Group>::const_iterator groupIt = mGroups.begin(); groupIt != mGroups.end(); ++groupIt )
				loadGroupTextures( *groupIt, uniqueVerts );
		}
	}
	else {
		if( hasGroupIndex ) {
			map<int,int> uniqueVerts;
			loadGroup( mGroups[mGroupIndex], uniqueVerts );
		}
		else {
			map<int,int> uniqueVerts;
			for( vector<Group>::const_iterator groupIt = mGroups.begin(); groupIt != mGroups.end(); ++groupIt )
				loadGroup( *groupIt, uniqueVerts );
		}
	}

	mOutputCached = true;
}

void ObjLoader::loadGroupNormalsTextures( const Group &group, map<VertexTriple,int> &uniqueVerts ) const
{
    bool hasColors = mMaterials.size() > 0;
	for( size_t f = 0; f < group.mFaces.size(); ++f ) {
		vec3 inferredNormal;
		bool forceUnique = ! mOptimizeVertices;
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
			vec3 edge1 = mInternalVertices[group.mFaces[f].mVertexIndices[1]] - mInternalVertices[group.mFaces[f].mVertexIndices[0]];
			vec3 edge2 = mInternalVertices[group.mFaces[f].mVertexIndices[2]] - mInternalVertices[group.mFaces[f].mVertexIndices[0]];
			inferredNormal = normalize( cross( edge1, edge2 ) );
			forceUnique = true;
		}

		if( group.mFaces[f].mTexCoordIndices.empty() )
			forceUnique = true;

		vector<int> faceIndices;
		faceIndices.reserve( group.mFaces[f].mNumVertices );
		for( int v = 0; v < group.mFaces[f].mNumVertices; ++v ) {
			if( ! forceUnique ) {
				VertexTriple vTriple = make_tuple( group.mFaces[f].mVertexIndices[v], group.mFaces[f].mTexCoordIndices[v], group.mFaces[f].mNormalIndices[v] );
				pair<map<VertexTriple,int>::iterator,bool> result = uniqueVerts.insert( make_pair( vTriple, mOutputVertices.size() ) );
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
				faceIndices.push_back( (int32_t)mOutputVertices.size() );

				mOutputVertices.push_back( mInternalVertices[group.mFaces[f].mVertexIndices[v]] );
				if( ! group.mHasNormals )
					mOutputNormals.push_back( inferredNormal );
				else
					mOutputNormals.push_back( mInternalNormals[group.mFaces[f].mNormalIndices[v]] );
				if( ! group.mHasTexCoords )
					mOutputTexCoords.push_back( vec2() );
				else
					mOutputTexCoords.push_back( mInternalTexCoords[group.mFaces[f].mTexCoordIndices[v]] );
                if( hasColors )
                    mOutputColors.push_back( rgb );
			}
		}

		int32_t triangles = (int32_t)faceIndices.size() - 2;
		for( int t = 0; t < triangles; ++t ) {
			mOutputIndices.push_back( faceIndices[0] ); mOutputIndices.push_back( faceIndices[t + 1] ); mOutputIndices.push_back( faceIndices[t + 2] );
		}
	}
}

void ObjLoader::loadGroupNormals( const Group &group, map<VertexPair,int> &uniqueVerts ) const
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
		vec3 inferredNormal;
		bool forceUnique = ! mOptimizeVertices;
		if( group.mFaces[f].mNormalIndices.empty() ) { // we'll have to derive it from two edges
			vec3 edge1 = mInternalVertices[group.mFaces[f].mVertexIndices[1]] - mInternalVertices[group.mFaces[f].mVertexIndices[0]];
			vec3 edge2 = mInternalVertices[group.mFaces[f].mVertexIndices[2]] - mInternalVertices[group.mFaces[f].mVertexIndices[0]];
			inferredNormal = normalize( cross( edge1, edge2 ) );
			forceUnique = true;
		}

		vector<int> faceIndices;
		faceIndices.reserve( group.mFaces[f].mNumVertices );
		for( int v = 0; v < group.mFaces[f].mNumVertices; ++v ) {
			if( ! forceUnique ) {
				VertexPair vPair = make_tuple( group.mFaces[f].mVertexIndices[v], group.mFaces[f].mNormalIndices[v] );
				pair<map<VertexPair,int>::iterator,bool> result = uniqueVerts.insert( make_pair( vPair, mOutputVertices.size() ) );
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
				faceIndices.push_back( (int32_t)mOutputVertices.size() );

				mOutputVertices.push_back( mInternalVertices[group.mFaces[f].mVertexIndices[v]] );
				if( ! group.mHasNormals )
					mOutputNormals.push_back( inferredNormal );
				else
					mOutputNormals.push_back( mInternalNormals[group.mFaces[f].mNormalIndices[v]] );
                if( hasColors )
                    mOutputColors.push_back( rgb );
			}
		}

		int32_t triangles = (int32_t)faceIndices.size() - 2;
		for( int t = 0; t < triangles; ++t ) {
			mOutputIndices.push_back( faceIndices[0] ); mOutputIndices.push_back( faceIndices[t + 1] ); mOutputIndices.push_back( faceIndices[t + 2] );
		}
	}
}

void ObjLoader::loadGroupTextures( const Group &group, map<VertexPair,int> &uniqueVerts ) const
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
		bool forceUnique = ! mOptimizeVertices;
		if( group.mFaces[f].mTexCoordIndices.empty() )
			forceUnique = true;

		vector<int32_t> faceIndices;
		faceIndices.reserve( group.mFaces[f].mNumVertices );
		for( int v = 0; v < group.mFaces[f].mNumVertices; ++v ) {
			if( ! forceUnique ) {
				VertexPair vPair = make_tuple( group.mFaces[f].mVertexIndices[v], group.mFaces[f].mTexCoordIndices[v] );
				pair<map<VertexPair,int>::iterator,bool> result = uniqueVerts.insert( make_pair( vPair, mOutputVertices.size() ) );
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
				faceIndices.push_back( (int32_t)mOutputVertices.size() );

				mOutputVertices.push_back( mInternalVertices[group.mFaces[f].mVertexIndices[v]] );
				if( ! group.mHasTexCoords )
					mOutputTexCoords.push_back( vec2() );
				else
					mOutputTexCoords.push_back( mInternalTexCoords[group.mFaces[f].mTexCoordIndices[v]] );
                if( hasColors )
                    mOutputColors.push_back( rgb );
			}
		}

		int32_t triangles = (int32_t)faceIndices.size() - 2;
		for( int t = 0; t < triangles; ++t ) {
			mOutputIndices.push_back( faceIndices[0] ); mOutputIndices.push_back( faceIndices[t + 1] ); mOutputIndices.push_back( faceIndices[t + 2] );
		}
	}
}

void ObjLoader::loadGroup( const Group &group, map<int,int> &uniqueVerts ) const
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

		int32_t triangles = (int32_t)faceIndices.size() - 2;
		for( int t = 0; t < triangles; ++t ) {
			mOutputIndices.push_back( faceIndices[0] ); mOutputIndices.push_back( faceIndices[t + 1] ); mOutputIndices.push_back( faceIndices[t + 2] );
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

	uint8_t	getAttribDims( geom::Attrib attr ) const override;
	void	copyAttrib( geom::Attrib attr, uint8_t dims, size_t strideBytes, const float *srcData, size_t count ) override;
	void	copyIndices( geom::Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytesPerIndex ) override;

  protected:
	void	writeData( const std::string &typeSpecifier, uint8_t dims, size_t strideBytes, const float *srcData, size_t count );

	OStreamRef		mStream;
	bool			mIncludeTexCoords, mIncludeNormals;
	bool			mHasTexCoords, mHasNormals;
};

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
				for( size_t i = 0; i < count; ++i ) {
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

void ObjWriteTarget::copyIndices( geom::Primitive /*primitive*/, const uint32_t *source, size_t numIndices, uint8_t /*requiredBytesPerIndex*/ )
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

void writeObj( const DataTargetRef &dataTarget, const geom::Source &source, bool includeNormals, bool includeTexCoords )
{
	OStreamRef stream = dataTarget->getStream();

	unique_ptr<ObjWriteTarget> target( new ObjWriteTarget( stream, includeNormals, includeTexCoords ) );

	geom::AttribSet requestedAttribs;
	requestedAttribs.insert( geom::POSITION );
	if( includeNormals )
		requestedAttribs.insert( geom::NORMAL );
	if( includeTexCoords )
		requestedAttribs.insert( geom::TEX_COORD_0 );

	source.loadInto( target.get(), requestedAttribs );

	if( source.getNumIndices() == 0 )
		target->generateIndices( geom::Primitive::TRIANGLES, source.getNumVertices() );
}

} // namespace cinder
