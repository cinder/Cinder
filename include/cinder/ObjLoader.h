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

#pragma once

#include "cinder/DataSource.h"
#include "cinder/DataTarget.h"
#include "cinder/GeomIo.h"

#include <tuple>
#include <map>

namespace cinder {

/** \brief Loads Alias|Wavefront .OBJ file format
 *
 * Example usage:
 * \code
 * cinder::gl::BatchRef myCubeRef;
 * ObjLoader loader( loadFile( "myPath/cube.obj" ) );
 * myCubeRef = gl::Batch::create( loader, gl::getStockShader( gl::ShaderDef().color() ) );
 * myCubeRef->draw();
 * \endcode
**/

class CI_API ObjLoader : public geom::Source {
  public:
	/**Constructs and does the parsing of the file
	 * \param includeNormals if false texture coordinates will be skipped, which can provide a faster load time
	 * \param includeTexCoords if false normals will be skipped, which can provide a faster load time
	**/
	ObjLoader( std::shared_ptr<IStreamCinder> stream, bool includeNormals = true, bool includeTexCoords = true, bool optimize = true );
	/**Constructs and does the parsing of the file
	 * \param includeNormals if false texture coordinates will be skipped, which can provide a faster load time
	 * \param includeTexCoords if false normals will be skipped, which can provide a faster load time
	**/
	ObjLoader( DataSourceRef dataSource, bool includeNormals = true, bool includeTexCoords = true, bool optimize = true );
	/**Constructs and does the parsing of the file
	 * \param includeNormals if false texture coordinates will be skipped, which can provide a faster load time
	 * \param includeTexCoords if false normals will be skipped, which can provide a faster load time
	**/
	ObjLoader( DataSourceRef dataSource, DataSourceRef materialSource, bool includeNormals = true, bool includeTexCoords = true,  bool optimize = true );

	/**Loads a specific group index from the file**/
	ObjLoader&	groupIndex( size_t groupIndex );
	/**Loads a specific group name from the file**/
	ObjLoader&	groupName( const std::string &groupName );
	/**Returns whether the file contains a group labeled with \a groupName**/
	bool		hasGroup( const std::string &groupName ) const;

	struct Material {
        Material() {
            Ka[0] = Ka[1] = Ka[2] = 0;
            Kd[0] = Kd[1] = Kd[2] = 1;
        }

        Material( const Material& rhs ) {
            mName = rhs.mName;
            Ka[0] = rhs.Ka[0];
            Ka[1] = rhs.Ka[1];
            Ka[2] = rhs.Ka[2];
            Kd[0] = rhs.Kd[0];
            Kd[1] = rhs.Kd[1];
            Kd[2] = rhs.Kd[2];
        }

        std::string mName;
        float		Ka[3];
        float		Kd[3];
    };
    
	struct Face {
		int						mNumVertices;
		std::vector<int32_t>	mVertexIndices;
		std::vector<int32_t>	mTexCoordIndices;
		std::vector<int32_t>	mNormalIndices;
		const Material*			mMaterial;
	};

	struct Group {
		std::string				mName;
		int32_t					mBaseVertexOffset, mBaseTexCoordOffset, mBaseNormalOffset;
		std::vector<Face>		mFaces;
		bool					mHasTexCoords;
		bool					mHasNormals;
	};

	//! Returns the total number of groups.
	size_t		getNumGroups() const { return mGroups.size(); }
	
	//! Returns a vector<> of the Groups in the OBJ.
	const std::vector<Group>&		getGroups() const { return mGroups; }

	size_t			getNumVertices() const override { load(); return mOutputVertices.size(); }
	size_t			getNumIndices() const override { load(); return mOutputIndices.size(); }
	geom::Primitive	getPrimitive() const override { return geom::Primitive::TRIANGLES; }
	uint8_t			getAttribDims( geom::Attrib attr ) const override;
	geom::AttribSet	getAvailableAttribs() const override;
	void			loadInto( geom::Target *target, const geom::AttribSet &requestedAttribs ) const override;
	Source*			clone() const override { return new ObjLoader( *this ); }

  private:
	typedef std::tuple<int,int> VertexPair;
	typedef std::tuple<int,int,int> VertexTriple;

	void	parse( bool includeNormals, bool includeTexCoords );
 	void	parseFace( Group *group, const Material *material, const std::string &s, bool includeNormals, bool includeTexCoords );
    void    parseMaterial( std::shared_ptr<IStreamCinder> material );

	void	load() const;

	void	loadGroupNormalsTextures( const Group &group, std::map<VertexTriple,int> &uniqueVerts ) const;
	void	loadGroupNormals( const Group &group, std::map<VertexPair,int> &uniqueVerts ) const;
	void	loadGroupTextures( const Group &group, std::map<VertexPair,int> &uniqueVerts ) const;
	void	loadGroup( const Group &group, std::map<int,int> &uniqueVerts ) const;

	std::shared_ptr<IStreamCinder>	mStream;

	std::vector<vec3>			    mInternalVertices, mInternalNormals;
	std::vector<vec2>			    mInternalTexCoords;
	std::vector<Colorf>				mInternalColors;

    mutable bool					mOptimizeVertices;
	mutable bool					mOutputCached;
	mutable std::vector<vec3>		mOutputVertices, mOutputNormals;
	mutable std::vector<vec2>		mOutputTexCoords;
	mutable std::vector<Colorf>		mOutputColors;
	mutable std::vector<uint32_t>	mOutputIndices;

	size_t							mGroupIndex;

	std::vector<Group>				mGroups;
	std::map<std::string, Material>	mMaterials;

};

//! Writes \a source to a new OBJ file to \a dataTarget.
CI_API void			writeObj( const DataTargetRef &dataTarget, const geom::Source &source, bool includeNormals = true, bool includeTexCoords = true );
//! Writes \a source to a new OBJ file to \a dataTarget.
CI_API inline void	writeObj( const DataTargetRef &dataTarget, const geom::SourceRef &source, bool includeNormals = true, bool includeTexCoords = true )
{
	writeObj( dataTarget, *source, includeNormals, includeTexCoords );
}

} // namespace cinder
