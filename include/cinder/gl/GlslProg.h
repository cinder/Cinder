/*
 Copyright (c) 2010, The Barbarian Group
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

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <exception>
#include <map>
#include <set>

#include "cinder/gl/gl.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Matrix.h"
#include "cinder/DataSource.h"
#include "cinder/GeomIo.h"

//! Convenience macro that allows one to embed raw glsl code in-line. The \a VERSION parameter will be used for the glsl's '#version' define.
//! \note Some strings will confuse different compilers, most commonly being preprocessor derictives (hence the need for \a VERSION to be a pamaeter).
//! If available on all target platforms, users should use C+11 raw string literals, which do not suffer from the same limitations.
#define CI_GLSL(VERSION,CODE) "#version " #VERSION "\n" #CODE

namespace cinder { namespace gl {
	
typedef std::shared_ptr<class GlslProg> GlslProgRef;

class GlslProg : public std::enable_shared_from_this<GlslProg> {
  public:
	struct Format {
		//! Defaults to specifying location 0 for the \c geom::Attrib::POSITION semantic
		Format();
		
		//! Supplies the GLSL source for the vertex shader
		Format&		vertex( const DataSourceRef &dataSource );
		//! Supplies the GLSL source for the vertex shader
		Format&		vertex( const char *vertexShader );
		//! Supplies the GLSL source for the fragment shader
		Format&		fragment( const DataSourceRef &dataSource );
		//! Supplies the GLSL source for the fragment shader
		Format&		fragment( const char *vertexShader );
#if ! defined( CINDER_GL_ES )
		//! Supplies the GLSL source for the geometry shader
		Format&		geometry( const DataSourceRef &dataSource );
		//! Supplies the GLSL source for the geometry shader
		Format&		geometry( const char *geometryShader );
		//! Sets the TransformFeedback varyings
		Format&		feedbackVaryings( const std::vector<std::string>& varyings ) { mTransformVaryings = varyings; return *this; }
		//! Sets the TransformFeedback format
		Format&		feedbackFormat( GLenum format ) { mTransformFormat = format; return *this; }
#endif
		
		//! Specifies an attribute name to map to a specific semantic
		Format&		attrib( geom::Attrib semantic, const std::string &attribName );
		//! Specifies a uniform name to map to a specific semantic
		Format&		uniform( UniformSemantic semantic, const std::string &attribName );
		
		//! Specifies a location for a specific named attribute
		Format&		attribLocation( const std::string &attribName, GLint location );
		//! Specifies a location for a semantic
		Format&		attribLocation( geom::Attrib attr, GLint location );

		//! Returns the GLSL source for the vertex shader. Returns an empty string if it isn't present.
		const std::string&	getVertex() const { return mVertexShader; }
		//! Returns the GLSL source for the fragment shader. Returns an empty string if it isn't present.
		const std::string&	getFragment() const { return mFragmentShader; }
#if ! defined( CINDER_GL_ES )
		//! Returns the GLSL source for the geometry shader
		const std::string&	getGeometry() const { return mGeometryShader; }
		const std::vector<std::string>&  getVaryings() const { return mTransformVaryings; }
		//! Returns the TransFormFeedback format
		GLenum			getTransformFormat() const { return mTransformFormat; }
#endif

		//! Returns the map between uniform semantics and uniform names
		const std::map<std::string,UniformSemantic>&	getUniformSemantics() const { return mUniformSemanticMap; }
		//! Returns the map between attribute semantics and attribute names
		const std::map<std::string,geom::Attrib>&		getAttribSemantics() const { return mAttribSemanticMap; }
		
		//! Returns the map between attribute names and specified locations
		const std::map<std::string,GLint>&	getAttribNameLocations() const { return mAttribNameLocMap; }
		//! Returns the map between attribute semantics and specified locations
		const std::map<geom::Attrib,GLint>&	getAttribSemanticLocations() const { return mAttribSemanticLocMap; }
		
		//! Returns the debugging label associated with the Program.
		const std::string&	getLabel() const { return mLabel; }
		//! Sets the debugging label associated with the Program. Calls glObjectLabel() when available.
		void				setLabel( const std::string &label ) { mLabel = label; }
		//! Sets the debugging label associated with the Program. Calls glObjectLabel() when available.
		Format&				label( const std::string &label ) { setLabel( label ); return *this; }
		
	  protected:
		std::string					mVertexShader;
		std::string					mFragmentShader;
#if ! defined( CINDER_GL_ES )
		std::string								mGeometryShader;
		GLenum									mTransformFormat;
		std::vector<std::string>				mTransformVaryings;
#endif
		std::map<std::string,GLint>				mAttribNameLocMap;
		std::map<geom::Attrib,GLint>			mAttribSemanticLocMap;
		std::map<std::string,UniformSemantic>	mUniformSemanticMap;
		std::map<std::string,geom::Attrib>		mAttribSemanticMap;
		
		std::string								mLabel;
	};
  
	typedef std::map<std::string,UniformSemantic>	UniformSemanticMap;
	typedef std::map<std::string,geom::Attrib>		AttribSemanticMap;

	static GlslProgRef create( const Format &format );  
	static GlslProgRef create( DataSourceRef vertexShader, DataSourceRef fragmentShader = DataSourceRef() );
	static GlslProgRef create( const char *vertexShader, const char *fragmentShader = 0 );
	
	~GlslProg();
	
	void			bind() const;
	
	GLuint			getHandle() const { return mHandle; }
	
	void	uniform( const std::string &name, int data ) const;
	void	uniform( int location, int data ) const;	
	void	uniform( const std::string &name, const Vec2i &data ) const;
	void	uniform( int location, const Vec2i &data ) const;	
	void	uniform( const std::string &name, const int *data, int count ) const;
	void	uniform( int location, const int *data, int count ) const;	
	void	uniform( const std::string &name, const Vec2i *data, int count ) const;
	void	uniform( int location, const Vec2i *data, int count ) const;	
	void	uniform( const std::string &name, float data ) const;
	void	uniform( int location, float data ) const;	
	void	uniform( const std::string &name, const Vec2f &data ) const;
	void	uniform( int location, const Vec2f &data ) const;	
	void	uniform( const std::string &name, const Vec3f &data ) const;
	void	uniform( int location, const Vec3f &data ) const;
	void	uniform( const std::string &name, const Vec4f &data ) const;
	void	uniform( int location, const Vec4f &data ) const;
	void	uniform( const std::string &name, const Color &data ) const;
	void	uniform( int location, const Color &data ) const;
	void	uniform( const std::string &name, const ColorA &data ) const;
	void	uniform( int location, const ColorA &data ) const;
	void	uniform( const std::string &name, const Matrix33f &data, bool transpose = false ) const;
	void	uniform( int location, const Matrix33f &data, bool transpose = false ) const;
	void	uniform( const std::string &name, const Matrix44f &data, bool transpose = false ) const;
	void	uniform( int location, const Matrix44f &data, bool transpose = false ) const;
	void	uniform( const std::string &name, const float *data, int count ) const;
	void	uniform( int location, const float *data, int count ) const;
	void	uniform( const std::string &name, const Vec2f *data, int count ) const;
	void	uniform( int location, const Vec2f *data, int count ) const;
	void	uniform( const std::string &name, const Vec3f *data, int count ) const;
	void	uniform( int location, const Vec3f *data, int count ) const;
	void	uniform( const std::string &name, const Vec4f *data, int count ) const;
	void	uniform( int location, const Vec4f *data, int count ) const;
	void    uniform( int location, const Matrix33f *data, int count, bool transpose = false ) const;
	void    uniform( const std::string &name, const Matrix33f *data, int count, bool transpose = false ) const;
	void    uniform( int location, const Matrix44f *data, int count, bool transpose = false ) const;
	void    uniform( const std::string &name, const Matrix44f *data, int count, bool transpose = false ) const;

	//! Returns a std::map from the uniform name to its OpenGL type (GL_BOOL, GL_FLOAT_VEC3, etc)
	const std::map<std::string,GLenum>&		getActiveUniformTypes() const;
	//! Returns a std::map from the attribute name to its OpenGL type (GL_BOOL, GL_FLOAT_VEC3, etc)
	const std::map<std::string,GLenum>&		getActiveAttribTypes() const;

	//! Returns the map between uniform semantics and active uniforms' names
	const UniformSemanticMap&		getUniformSemantics() const;
	//! Returns the map between attribute semantics and active attributes' names
	const AttribSemanticMap&		getAttribSemantics() const;
	
	bool	hasAttribSemantic( geom::Attrib semantic ) const;
	GLint	getAttribSemanticLocation( geom::Attrib semantic ) const;
	GLint	operator[]( geom::Attrib sem ) const { return getAttribSemanticLocation( sem ); }
	
	//! Default mapping from uniform name to semantic. Can be modified via the reference. Not thread-safe.
	static UniformSemanticMap&		getDefaultUniformNameToSemanticMap();
	//! Default mapping from attribute name to semantic. Can be modified via the reference. Not thread-safe.
	static AttribSemanticMap&		getDefaultAttribNameToSemanticMap();
	
	GLint	getAttribLocation( const std::string &name ) const;
	GLint	getUniformLocation( const std::string &name ) const;
	
	std::string		getShaderLog( GLuint handle ) const;

	//! Returns the debugging label associated with the Program.
	const std::string&	getLabel() const { return mLabel; }
	//! Sets the debugging label associated with the Program. Calls glObjectLabel() when available.
	void				setLabel( const std::string &label );

  protected:
	GlslProg( const Format &format );

	void			bindImpl();
	void			loadShader( const std::string &shaderSource, GLint shaderType );
	void			attachShaders();
	void			link();
	
	GLuint									mHandle;

	mutable std::map<std::string, int>		mUniformLocs;
	mutable bool							mActiveUniformTypesCached;
	mutable std::map<std::string, GLenum>	mActiveUniformTypes;

	mutable std::map<std::string, int>		mAttribLocs; // map between name and location
	mutable bool							mActiveAttribTypesCached;
	mutable std::map<std::string, GLenum>	mActiveAttribTypes; // map between name and type, ie GL_FLOAT_VEC2, GL_FLOAT_MAT2x4, etc
	
	static UniformSemanticMap				sDefaultUniformNameToSemanticMap;
	UniformSemanticMap						mUniformNameToSemanticMap;
	mutable bool							mUniformSemanticsCached;
	mutable UniformSemanticMap				mUniformSemantics;
	
	static AttribSemanticMap				sDefaultAttribNameToSemanticMap;
	AttribSemanticMap						mAttribNameToSemanticMap;
	mutable bool							mAttribSemanticsCached;
	mutable AttribSemanticMap				mAttribSemantics;
	// enumerates the uniforms we've already logged as missing so that we don't flood the log with the same message
	mutable std::set<std::string>			mLoggedMissingUniforms;

	std::string								mLabel; // debug label

	// storage as a work-around for NVidia on MSW driver bug expecting persistent memory in calls to glTransformFeedbackVaryings
#if ! defined( CINDER_GL_ES )
	std::unique_ptr<std::vector<GLchar>>	mTransformFeedbackVaryingsChars;
	std::unique_ptr<std::vector<GLchar*>>	mTransformFeedbackVaryingsCharStarts;
#endif

	friend class Context;
	friend std::ostream& operator<<( std::ostream &os, const GlslProg &rhs );
};

class GlslProgCompileExc : public std::exception {
  public:
	GlslProgCompileExc( const std::string &log, GLint aShaderType ) throw();
	virtual const char* what() const throw()
	{
		return mMessage;
	}
	
  private:
	char	mMessage[ 16001 ];
	GLint	mShaderType;
};

class GlslNullProgramExc : public std::exception {
  public:
	virtual const char* what() const throw()
	{
		return "Glsl: Attempt to use null shader";
	}
	
};

} } // namespace cinder::gl
