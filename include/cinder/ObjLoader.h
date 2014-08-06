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

#include "cinder/TriMesh.h"
#include "cinder/Stream.h"
#include "cinder/GeomIo.h"

#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <map>

namespace cinder {

/** \brief Loads Alias|Wavefront .OBJ file format
 *
 * Currently does not support anything but polygonal data
 * \n Example usage:
 * \code
 * cinder::TriMesh myCube;
 * ObjLoader loader( loadFile( "myPath/cube.obj" ) );
 * loader.load( &myCube );
 * gl::draw( myCube );
 * \endcode
**/ 

class ObjLoader : public geom::Source {
  public:
	/**Constructs and does the parsing of the file
	 * \param includeUVs  if false UV coordinates will be skipped, which can provide a faster load time
	**/
	ObjLoader( std::shared_ptr<IStreamCinder> aStream, bool includeUVs = true );
	/**Constructs and does the parsing of the file
	 * \param includeUVs if false UV coordinates will be skipped, which can provide a faster load time
	**/
	ObjLoader( DataSourceRef dataSource, bool includeUVs = true );
	/**Constructs and does the parsing of the file
	 * \param includeUVs if false UV coordinates will be skipped, which can provide a faster load time
     **/
	ObjLoader( DataSourceRef dataSource, DataSourceRef materialSource, bool includeUVs = true );

	/**Loads all the groups present in the file
	 * \param loadNormals  should normals be loaded or generated if not present. Default determines from the contents of the file
	 * \param loadTexCoords  should 2D texture coordinates be loaded or set to zero if not present. Default determines from the contents of the file*/
	void	load( boost::tribool loadNormals = boost::logic::indeterminate, boost::tribool loadTexCoords = boost::logic::indeterminate );
	/**Loads a particular group into a TriMesh
	 * \param loadNormals  should normals be loaded or generated if not present. Default determines from the contents of the file
	 * \param loadTexCoords  should 2D texture coordinates be loaded or set to zero if not present. Default determines from the contents of the file
	 * \param optimizeVertices  should the loader minimize the vertices by identifying shared vertices between faces.*/
	void	load( size_t groupIndex, boost::tribool loadNormals = boost::logic::indeterminate, boost::tribool loadTexCoords = boost::logic::indeterminate );
	
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
		int					mNumVertices;
		std::vector<int>	mVertexIndices;
		std::vector<int>	mTexCoordIndices;
		std::vector<int>	mNormalIndices;
		const Material*     mMaterial;
	};

	struct Group {
		std::string				mName;
		int						mBaseVertexOffset, mBaseTexCoordOffset, mBaseNormalOffset;
		std::vector<Face>		mFaces;
		bool					mHasTexCoords;
		bool					mHasNormals;
	};

    //! Returns the total number of groups.
	size_t		getNumGroups() const { return mGroups.size(); }
	
	//! Returns a vector<> of the Groups in the OBJ.
	const std::vector<Group>&		getGroups() const { return mGroups; }

	virtual void			loadInto( geom::Target *target ) const override;
	virtual size_t			getNumVertices() const override { return mOutputVertices.size(); }
	virtual size_t			getNumIndices() const override { return mIndices.size(); }	
	virtual geom::Primitive	getPrimitive() const override { return geom::Primitive::TRIANGLES; }
	virtual uint8_t			getAttribDims( geom::Attrib attr ) const override;
	
  private:
	typedef boost::tuple<int,int> VertexPair;
	typedef boost::tuple<int,int,int> VertexTriple;

	void	parse( bool includeUVs );

 	void	parseFace( Group *group, const Material *material, const std::string &s, bool includeUVs );
    void    parseMaterial( std::shared_ptr<IStreamCinder> material );
	void	loadGroupNormalsTextures( const Group &group, std::map<boost::tuple<int,int,int>,int> &uniqueVerts );
	void	loadGroupNormals( const Group &group, std::map<boost::tuple<int,int>,int> &uniqueVerts );
	void	loadGroupTextures( const Group &group, std::map<boost::tuple<int,int>,int> &uniqueVerts );
	void	loadGroup( const Group &group, std::map<int,int> &uniqueVerts );	
 
	std::shared_ptr<IStreamCinder>	mStream;

	std::vector<Vec3f>			    mInternalVertices, mInternalNormals;
	std::vector<Vec2f>			    mInternalTexCoords;
	std::vector<Colorf>				mInternalColors;
	std::vector<Vec3f>			    mOutputVertices, mOutputNormals;
	std::vector<Vec2f>			    mOutputTexCoords;
	std::vector<Colorf>				mOutputColors;

	std::vector<Group>			    mGroups;
	std::vector<uint32_t>			mIndices;
	std::map<std::string, Material> mMaterials;
};

//! Writes a new OBJ file to \a dataTarget.
void	objWrite( DataTargetRef dataTarget, const geom::Source &source, bool includeNormals = true, bool includeTexCoords = true );	
inline void	objWrite( DataTargetRef dataTarget, const geom::SourceRef &source, bool includeNormals = true, bool includeTexCoords = true )
{
	objWrite( dataTarget, *source, includeNormals, includeTexCoords );
}

} // namespace cinder
