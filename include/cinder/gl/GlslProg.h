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
#include <map>
#include <set>

#include "cinder/gl/wrapper.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Matrix.h"
#include "cinder/DataSource.h"
#include "cinder/GeomIo.h"
#include "cinder/Exception.h"

//! Convenience macro that allows one to embed raw glsl code in-line. The \a VERSION parameter will be used for the glsl's '#version' define.
//! \note Some strings will confuse different compilers, most commonly being preprocessor derictives (hence the need for \a VERSION to be a pamaeter).
//! If available on all target platforms, users should use C+11 raw string literals, which do not suffer from the same limitations.
#define CI_GLSL(VERSION,CODE) "#version " #VERSION "\n" #CODE

namespace cinder { namespace gl {
	
typedef std::shared_ptr<class GlslProg> GlslProgRef;

class ShaderPreprocessor;

class GlslProg : public std::enable_shared_from_this<GlslProg> {
  public:
	
	//! Resembles all the information Queryable of an attribute
	struct Attribute {
		std::string		mName;
		GLint			mCount = 0, mLoc = -1;
		GLenum			mType;
		geom::Attrib	mSemantic = geom::Attrib::NUM_ATTRIBS;
	};
	
	struct Uniform {
		std::string		mName;
		GLint			mCount = 0, mLoc = -1, mIndex = -1;
		GLint			mDataSize = 0;
		GLenum			mType;
		UniformSemantic mSemantic = UniformSemantic::USER_DEFINED_UNIFORM;
	};
	
#if ! defined( CINDER_GL_ES_2 )
	struct UniformBlock {
		std::string mName;
		GLint mSize = 0, mLoc = -1;
		GLint mBlockBinding;
		//! Contains info on each of the active uniforms contained by this Uniform Block
		std::vector<Uniform> mActiveUniforms;
		//! Includes information attached to GL_UNIFORM_OFFSET, GL_UNIFORM_ARRAY_STRIDE, GL_UNIFORM_MATRIX_STRIDE
		std::map<GLenum, std::vector<GLint>> mActiveUniformInfo;
	};
	
	struct TransformFeedbackVaryings {
		std::string		mName;
		GLint			mCount = 0;
		GLenum			mType;
	};
#endif

	struct Format {
		//! Defaults to specifying location 0 for the \c geom::Attrib::POSITION semantic
		Format();
		
		//! Supplies the GLSL source for the vertex shader
		Format&		vertex( const DataSourceRef &dataSource );
		//! Supplies the GLSL source for the vertex shader
		Format&		vertex( const std::string &vertexShader );
		//! Supplies the GLSL source for the fragment shader
		Format&		fragment( const DataSourceRef &dataSource );
		//! Supplies the GLSL source for the fragment shader
		Format&		fragment( const std::string &vertexShader );
#if ! defined( CINDER_GL_ES )
		//! Supplies the GLSL source for the geometry shader
		Format&		geometry( const DataSourceRef &dataSource );
		//! Supplies the GLSL source for the geometry shader
		Format&		geometry( const std::string &geometryShader );
		//! Supplies the GLSL source for the tessellation control shader
		Format&		tessellationCtrl( const DataSourceRef &dataSource );
		//! Supplies the GLSL source for the tessellation control shader
		Format&		tessellationCtrl( const std::string &tessellationCtrlShader );
		//! Supplies the GLSL source for the tessellation control shader
		Format&		tessellationEval( const DataSourceRef &dataSource );
		//! Supplies the GLSL source for the tessellation control shader
		Format&		tessellationEval( const std::string &tessellationEvalShader );
#endif
#if ! defined( CINDER_GL_ES_2 )		
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

#if ! defined( CINDER_GL_ES )
		//! Specifies a binding for a user-defined varying out variable to a fragment shader color number. Analogous to glBindFragDataLocation.
		Format&		fragDataLocation( GLuint colorNumber, const std::string &name );
#endif

		//! Returns the GLSL source for the vertex shader. Returns an empty string if it isn't present.
		const std::string&	getVertex() const { return mVertexShader; }
		//! Returns the GLSL source for the fragment shader. Returns an empty string if it isn't present.
		const std::string&	getFragment() const { return mFragmentShader; }
#if ! defined( CINDER_GL_ES )
		//! Returns the GLSL source for the geometry shader
		const std::string&	getGeometry() const { return mGeometryShader; }
		const std::string&	getTessellationCtrl() const { return mTessellationCtrlShader; }
		const std::string&	getTessellationEval() const { return mTessellationEvalShader; }
#endif
#if ! defined( CINDER_GL_ES_2 )
		const std::vector<std::string>&  getVaryings() const { return mTransformVaryings; }
		//! Returns the TransFormFeedback format
		GLenum			getTransformFormat() const { return mTransformFormat; }
		//! Returns the map between output variable names and their bound color numbers
		const std::map<std::string,GLuint>&	getFragDataLocations() const { return mFragDataLocations; }
#endif
		
		//! Returns the debugging label associated with the Program.
		const std::string&	getLabel() const { return mLabel; }
		//! Sets the debugging label associated with the Program. Calls glObjectLabel() when available.
		void				setLabel( const std::string &label ) { mLabel = label; }
		//! Sets the debugging label associated with the Program. Calls glObjectLabel() when available.
		Format&				label( const std::string &label ) { setLabel( label ); return *this; }
        
        	//! Returns the fs::path for the vertex shader. Returns an empty fs::path if it isn't present.
        	const fs::path&	getVertexPath() const { return mVertexShaderPath; }
        	//! Returns the fs::path for the fragment shader. Returns an empty fs::path if it isn't present.
        	const fs::path&	getFragmentPath() const { return mFragmentShaderPath; }
#if ! defined( CINDER_GL_ES )
        	//! Returns the fs::path for the geometry shader. Returns an empty fs::path if it isn't present.
        	const fs::path&	getGeometryPath() const { return mGeometryShaderPath; }
        	//! Returns the fs::path for the tessellation control shader. Returns an empty fs::path if it isn't present.
        	const fs::path&	getTessellationCtrlPath() const { return mTessellationCtrlShaderPath; }
        	//! Returns the fs::path for the tessellation eval shader. Returns an empty fs::path if it isn't present.
        	const fs::path&	getTessellationEvalPath() const { return mTessellationEvalShaderPath; }
#endif
		const std::vector<Uniform>&		getUniforms() const { return mUniforms; }
		const std::vector<Attribute>&	getAttributes() const { return mAttributes; }
		std::vector<Uniform>&			getUniforms() { return mUniforms; }
		std::vector<Attribute>&			getAttributes() { return mAttributes; }
		
	  protected:
		void			setShaderSource( const DataSourceRef &dataSource, std::string *shaderSourceDest, fs::path *shaderPathDest );
		void			setShaderSource( const std::string &source, std::string *shaderSourceDest, fs::path *shaderPathDest );

		std::string		mVertexShader;
		std::string		mFragmentShader;

		fs::path		mVertexShaderPath;
		fs::path		mFragmentShaderPath;

#if ! defined( CINDER_GL_ES )
		std::string		mGeometryShader;
		std::string		mTessellationCtrlShader;
		std::string		mTessellationEvalShader;
		fs::path		mGeometryShaderPath;
		fs::path		mTessellationCtrlShaderPath;
		fs::path		mTessellationEvalShaderPath;
#endif
#if ! defined( CINDER_GL_ES_2 )
		GLenum									mTransformFormat;
		std::vector<std::string>				mTransformVaryings;
		std::map<std::string,GLuint>			mFragDataLocations;
#endif
		std::vector<Attribute>					mAttributes;
		std::vector<Uniform>					mUniforms;
		
		
		bool									mPreprocessingEnabled;
		std::string								mLabel;

		friend class		GlslProg;
	};
  
	typedef std::map<std::string,UniformSemantic>	UniformSemanticMap;
	typedef std::map<std::string,geom::Attrib>		AttribSemanticMap;

	static GlslProgRef create( const Format &format );

#if ! defined( CINDER_GL_ES )
	static GlslProgRef create( DataSourceRef vertexShader,
								   DataSourceRef fragmentShader = DataSourceRef(),
								   DataSourceRef geometryShader = DataSourceRef(),
								   DataSourceRef tessEvalShader = DataSourceRef(),
								   DataSourceRef tessCtrlShader = DataSourceRef() );
	static GlslProgRef create( const std::string &vertexShader,
								   const std::string &fragmentShader = std::string(),
								   const std::string &geometryShader = std::string(),
								   const std::string &tessEvalShader = std::string(),
								   const std::string &tessCtrlShader = std::string() );
#else
	static GlslProgRef create( DataSourceRef vertexShader, DataSourceRef fragmentShader = DataSourceRef() );
	static GlslProgRef create( const std::string &vertexShader, const std::string &fragmentShader = std::string() );
#endif
	~GlslProg();
	
	void			bind() const;
	
	GLuint			getHandle() const { return mHandle; }
	
	inline void		uniform( const std::string &name, bool data ) const { uniformImpl( name, data ); }
	inline void		uniform( const std::string &name, int data ) const { uniformImpl( name, data ); }
	inline void		uniform( const std::string &name, uint32_t data ) const { uniformImpl( name, data ); }
	inline void		uniform( const std::string &name, float data ) const { uniformImpl( name, data ); }
	void			uniform( int location, bool data ) const;
	void			uniform( int location, int data ) const;
	void			uniform( int location, uint32_t data ) const;
	void			uniform( int location, float data ) const;
	inline void		uniform( const std::string &name, const vec2 &data ) const { uniformImpl( name, data ); }
	inline void		uniform( const std::string &name, const vec3 &data ) const { uniformImpl( name, data ); }
	inline void		uniform( const std::string &name, const vec4 &data ) const { uniformImpl( name, data ); }
	void			uniform( int location, const vec2 &data ) const;
	void			uniform( int location, const vec3 &data ) const;
	void			uniform( int location, const vec4 &data ) const;
	inline void		uniform( const std::string &name, const ivec2 &data ) const { uniformImpl( name, data ); }
	inline void		uniform( const std::string &name, const ivec3 &data ) const { uniformImpl( name, data ); }
	inline void		uniform( const std::string &name, const ivec4 &data ) const { uniformImpl( name, data ); }
	void			uniform( int location, const ivec2 &data ) const;
	void			uniform( int location, const ivec3 &data ) const;
	void			uniform( int location, const ivec4 &data ) const;
	inline void		uniform( const std::string &name, const uvec2 &data ) const { uniformImpl( name, data ); }
	inline void		uniform( const std::string &name, const uvec3 &data ) const { uniformImpl( name, data ); }
	inline void		uniform( const std::string &name, const uvec4 &data ) const { uniformImpl( name, data ); }
	void			uniform( int location, const uvec2 &data ) const;
	void			uniform( int location, const uvec3 &data ) const;
	void			uniform( int location, const uvec4 &data ) const;
	inline void		uniform( const std::string &name, const Color &data ) const { uniformImpl( name, data ); }
	inline void		uniform( const std::string &name, const ColorA &data ) const { uniformImpl( name, data ); }
	void			uniform( int location, const Color &data ) const;
	void			uniform( int location, const ColorA &data ) const;
	inline void		uniform( const std::string &name, const mat2 &data, bool transpose = false ) const { uniformMatImpl( name, data, transpose ); }
	inline void		uniform( const std::string &name, const mat3 &data, bool transpose = false ) const { uniformMatImpl( name, data, transpose ); }
	inline void		uniform( const std::string &name, const mat4 &data, bool transpose = false ) const { uniformMatImpl( name, data, transpose ); }
	void			uniform( int location, const mat2 &data, bool transpose = false ) const;
	void			uniform( int location, const mat3 &data, bool transpose = false ) const;
	void			uniform( int location, const mat4 &data, bool transpose = false ) const;
	
	inline void		uniform( const std::string &name, const int *data, int count ) const { uniformImpl( name, data, count ); }
	inline void		uniform( const std::string &name, const uint32_t *data, int count ) const { uniformImpl( name, data, count ); }
	inline void		uniform( const std::string &name, const float *data, int count ) const { uniformImpl( name, data, count ); }
	void			uniform( int location, const int *data, int count ) const;
	void			uniform( int location, const uint32_t *data, int count ) const;
	void			uniform( int location, const float *data, int count ) const;
	inline void		uniform( const std::string &name, const ivec2 *data, int count ) const { uniformImpl( name, data, count ); }
	inline void		uniform( const std::string &name, const vec2 *data, int count ) const { uniformImpl( name, data, count ); }
	inline void		uniform( const std::string &name, const vec3 *data, int count ) const { uniformImpl( name, data, count ); }
	inline void		uniform( const std::string &name, const vec4 *data, int count ) const { uniformImpl( name, data, count ); }
	void			uniform( int location, const ivec2 *data, int count ) const;
	void			uniform( int location, const vec2 *data, int count ) const;
	void			uniform( int location, const vec3 *data, int count ) const;
	void			uniform( int location, const vec4 *data, int count ) const;
	inline void		uniform( const std::string &name, const mat3 *data, int count, bool transpose = false ) const { uniformMatImpl( name, data, count, transpose ); }
	inline void		uniform( const std::string &name, const mat4 *data, int count, bool transpose = false ) const { uniformMatImpl( name, data, count, transpose ); }
	void			uniform( int location, const mat3 *data, int count, bool transpose = false ) const;
	void			uniform( int location, const mat4 *data, int count, bool transpose = false ) const;
	
	bool	hasAttribSemantic( geom::Attrib semantic ) const;
	GLint	getAttribSemanticLocation( geom::Attrib semantic ) const;
	GLint	operator[]( geom::Attrib sem ) const { return getAttribSemanticLocation( sem ); }
	
	//! Default mapping from uniform name to semantic. Can be modified via the reference. Not thread-safe.
	static UniformSemanticMap&		getDefaultUniformNameToSemanticMap();
	//! Default mapping from attribute name to semantic. Can be modified via the reference. Not thread-safe.
	static AttribSemanticMap&		getDefaultAttribNameToSemanticMap();
	
	//! Returns the attrib location of the Attribute that matches \a name.
	GLint							getAttribLocation( const std::string &name ) const;
	//! Returns a const reference to the Active Attribute cache.
	const std::vector<Attribute>&	getActiveAttributes() const { return mAttributes; }
	//! Returns a const pointer to the Attribute that matches \a name. Returns nullptr if the attrib doesn't exist.
	const Attribute*	findAttrib( const std::string &name ) const;
	//! Returns the uniform location of the Uniform that matches \a name.
	GLint							getUniformLocation( const std::string &name ) const;
	//! Returns a const reference to the Active Uniform cache.
	const std::vector<Uniform>&		getActiveUniforms() const { return mUniforms; }
	//! Returns a const pointer to the Uniform that matches \a name. Returns nullptr if the uniform doesn't exist.
	const Uniform*					findUniform( const std::string &name ) const;

#if ! defined( CINDER_GL_ES_2 )
	// Uniform blocks
	//! Analogous to glUniformBlockBinding()
	void	uniformBlock( const std::string &name, GLint binding );
	//! Analogous to glUniformBlockBinding()
	void	uniformBlock( GLint loc, GLint binding );
	//!	Returns the uniform block location of the Uniform Block that matches \a name.
	GLint	getUniformBlockLocation( const std::string &name ) const;
	//! Returns the size of the Uniform block matching \a blockIndex.
	GLint	getUniformBlockSize( GLint blockIndex ) const;
	//! Returns a const pointer to the UniformBlock that matches \a name. Returns nullptr if the uniform block doesn't exist.
	const UniformBlock* findUniformBlock( const std::string &name ) const;
	//! Returns a const reference to the UniformBlock cache.
	const std::vector<UniformBlock>& getActiveUniformBlocks() const { return mUniformBlocks; }
	//! Returns a const pointer to the TransformFeedbackVarying that matches \a name. Returns nullptr if the transform feedback varying doesn't exist.
	const TransformFeedbackVaryings* findTransformFeedbackVaryings( const std::string &name ) const;
	//! Returns a const reference to the TransformFeedbackVaryings cache.
	const std::vector<TransformFeedbackVaryings>& getActiveTransformFeedbackVaryings() const { return mTransformFeedbackVaryings; }
#endif
	
	std::string		getShaderLog( GLuint handle ) const;

	//! Returns the debugging label associated with the Program.
	const std::string&	getLabel() const { return mLabel; }
	//! Sets the debugging label associated with the Program. Calls glObjectLabel() when available.
	void				setLabel( const std::string &label );

  protected:
	GlslProg( const Format &format );

	void			bindImpl() const;
	void			loadShader( const std::string &shaderSource, const fs::path &shaderPath, GLint shaderType );
	void			attachShaders();
	void			link();
	
	//! Caches all active Attributes after linking.
	void				cacheActiveAttribs();
	//! Returns a pointer to the Attribute that matches \a name. Returns nullptr if the attrib doesn't exist.
	Attribute*			findAttrib( const std::string &name );
	//! Caches all active Uniforms after linrking.
	void				cacheActiveUniforms();
	//! Returns a pointer to the Uniform that matches \a name. Returns nullptr if the attrib doesn't exist.
	Uniform*			findUniform( const std::string &name );
	//! Performs the finding, validation, and implementation of single uniform variables. Ends by calling the location
	//! variant uniform function.
	template<typename T>
	inline void	uniformImpl( const std::string &name, const T &data ) const;
	//! Performs the finding, validation, and implementation of single uniform Matrix variables. Ends by calling the location
	//! variant uniform function.
	template<typename T>
	inline void	uniformMatImpl( const std::string &name, const T &data, bool transpose ) const;
	//! Performs the finding, validation, and implementation of multiple uniform variables. Ends by calling the location
	//! variant uniform function.
	template<typename T>
	inline void	uniformImpl( const std::string &name, const T *data, int count ) const;
	//! Performs the finding, validation, and implementation of multiple uniform Matrix variables. Ends by calling the location
	//! variant uniform function.
	template<typename T>
	inline void	uniformMatImpl( const std::string &name, const T *data, int count, bool transpose ) const;
	//! Logs an error and caches the name.
	void				logMissingUniform( const std::string &name ) const;
	//! Logs a warning and caches the name.
	void				logUniformWrongType( const std::string &name, GLenum uniformType, const std::string &userType ) const;
	//! Checks the validity of the settings on this uniform, specifically type and value
	template<typename T>
	inline bool			validateUniform( const Uniform &uniform, const T &val ) const;
	//! Checks the validity of the settings on this uniform, specifically type and value
	template<typename T>
	inline bool			validateUniform( const Uniform &uniform, const T *val, int count ) const;
	//! Implementing later for CPU Uniform Buffer Cache.
	template<typename T>
	inline bool			checkUniformValue( const Uniform &uniform, const T &val ) const;
	//! Checks the type of the uniform against the provided type of data in validateUniform. If the provided
	//! type, \a uniformType, and the type T match this function returns true, otherwise it returns false.
	template<typename T>
	inline bool			checkUniformType( GLenum uniformType, std::string &typeString ) const;
#if ! defined( CINDER_GL_ES_2 )
	//! Caches all active Uniform Blocks after linking.
	void				cacheActiveUniformBlocks();
	//! Returns a pointer to the Uniform Block that matches \a name. Returns nullptr if the attrib doesn't exist.
	UniformBlock*		findUniformBlock( const std::string &name );
	//! Caches all active Transform Feedback Varyings after linking.
	void				cacheActiveTransformFeedbackVaryings();
	//! Returns a pointer to the Transform Feedback Varyings that matches \a name. Returns nullptr if the attrib doesn't exist.
	TransformFeedbackVaryings* findTransformFeedbackVaryings( const std::string &name );
#endif
	
	GLuint									mHandle;
	
	std::vector<Attribute>					mAttributes;
	std::vector<Uniform>					mUniforms;
#if ! defined( CINDER_GL_ES_2 )
	std::vector<UniformBlock>				mUniformBlocks;
	std::vector<TransformFeedbackVaryings>  mTransformFeedbackVaryings;
	GLenum									mTransformFeedbackFormat;
#endif
	
	// enumerates the uniforms we've already logged as missing so that we don't flood the log with the same message
	mutable std::set<std::string>			mLoggedUniforms;

	std::string								mLabel; // debug label
	std::unique_ptr<ShaderPreprocessor>		mShaderPreprocessor;

	friend class Context;
	friend std::ostream& operator<<( std::ostream &os, const GlslProg &rhs );
};
	
template<typename T>
inline void GlslProg::uniformImpl( const std::string &name, const T &data ) const
{
	auto found = findUniform( name );
	if( ! found ) {
		logMissingUniform( name );
		return;
	}
	if( validateUniform( *found, data ) )
		uniform( found->mLoc, data );
}
	
template<typename T>
inline void	GlslProg::uniformMatImpl( const std::string &name, const T &data, bool transpose ) const
{
	auto found = findUniform( name );
	if( ! found ) {
		logMissingUniform( name );
		return;
	}
	if( validateUniform( *found, data ) )
		uniform( found->mLoc, data, transpose );
}

template<typename T>
inline void	GlslProg::uniformImpl( const std::string &name, const T *data, int count ) const
{
	auto found = findUniform( name );
	if( ! found ) {
		logMissingUniform( name );
		return;
	}
	if( validateUniform( *found, data, count ) )
		uniform( (int)found->mLoc, data, count );
}
	
template<typename T>
inline void	GlslProg::uniformMatImpl( const std::string &name, const T *data, int count, bool transpose ) const
{
	auto found = findUniform( name );
	if( ! found ) {
		logMissingUniform( name );
		return;
	}
	if( validateUniform( *found, data, count ) )
		uniform( found->mLoc, data, count, transpose );
}
	
template<typename T>
inline bool GlslProg::validateUniform( const Uniform &uniform, const T &val ) const
{
	std::string type;
	if( ! checkUniformType<T>( uniform.mType, type ) ) {
		logUniformWrongType( uniform.mName, uniform.mType, type );
		return false;
	}
	else
		return true;
}
	
template<typename T>
inline bool GlslProg::validateUniform( const Uniform &uniform, const T *val, int count ) const
{
	std::string type;
	if( ! checkUniformType<T>( uniform.mType, type ) ) {
		logUniformWrongType( uniform.mName, uniform.mType, type );
		return false;
	}
	else
		return true;
}
	
template<typename T>
inline bool GlslProg::checkUniformType( GLenum uniformType, std::string &type ) const
{
	type = "unknown type";
	return false;
}
template<>
inline bool GlslProg::checkUniformType<bool>( GLenum uniformType, std::string &type ) const
{
	type = "bool";
	return GL_BOOL == uniformType;
}
	
template<>
inline bool GlslProg::checkUniformType<int>( GLenum uniformType, std::string &type ) const
{
	// We could add more samplers here if needed.
	switch ( uniformType ) {
		case GL_BOOL: return true; break;
		case GL_INT: return true; break;
		case GL_SAMPLER_2D: return true; break;
		case GL_SAMPLER_2D_SHADOW: return true; break;
		case GL_SAMPLER_3D: return true; break;
		case GL_SAMPLER_CUBE: return true; break;
		default: type = "int"; return false; break;
	}
}
template<>
inline bool GlslProg::checkUniformType<float>( GLenum uniformType, std::string &type ) const
{
	type = "float";
	return GL_FLOAT == uniformType;
}
template<>
inline bool GlslProg::checkUniformType<uint32_t>( GLenum uniformType, std::string &type ) const
{
	type = "uint32_t";
	return GL_UNSIGNED_INT == uniformType;
}
template<>
inline bool GlslProg::checkUniformType<glm::bvec2>( GLenum uniformType, std::string &type ) const
{
	type = "bvec2";
	return GL_BOOL_VEC2 == uniformType;
}
template<>
inline bool GlslProg::checkUniformType<glm::bvec3>( GLenum uniformType, std::string &type ) const
{
	type = "bvec3";
	return GL_BOOL_VEC3 == uniformType;
}
template<>
inline bool GlslProg::checkUniformType<glm::bvec4>( GLenum uniformType, std::string &type ) const
{
	type = "bvec4";
	return GL_BOOL_VEC4 == uniformType;
}
template<>
inline bool GlslProg::checkUniformType<ivec2>( GLenum uniformType, std::string &type ) const
{
	type = "ivec2";
	return GL_INT_VEC2 == uniformType;
}
template<>
inline bool GlslProg::checkUniformType<ivec3>( GLenum uniformType, std::string &type ) const
{
	type = "ivec3";
	return GL_INT_VEC3 == uniformType;
}
template<>
inline bool GlslProg::checkUniformType<ivec4>( GLenum uniformType, std::string &type ) const
{
	type = "ivec4";
	return GL_INT_VEC4 == uniformType;
}
template<>
inline bool GlslProg::checkUniformType<vec2>( GLenum uniformType, std::string &type ) const
{
	type = "vec2";
	return GL_FLOAT_VEC2 == uniformType;
}
template<>
inline bool GlslProg::checkUniformType<vec3>( GLenum uniformType, std::string &type ) const
{
	type = "vec3";
	return GL_FLOAT_VEC3 == uniformType;
}
template<>
inline bool GlslProg::checkUniformType<vec4>( GLenum uniformType, std::string &type ) const
{
	type = "vec4";
	return GL_FLOAT_VEC4 == uniformType;
}
template<>
inline bool GlslProg::checkUniformType<Color>( GLenum uniformType, std::string &type ) const
{
	type = "vec3 as Color";
	return GL_FLOAT_VEC3 == uniformType;
}
template<>
inline bool GlslProg::checkUniformType<ColorA>( GLenum uniformType, std::string &type ) const
{
	type = "vec4 as ColorA";
	return GL_FLOAT_VEC4 == uniformType;
}
template<>
inline bool GlslProg::checkUniformType<uvec2>( GLenum uniformType, std::string &type ) const
{
	type = "uvec2";
	return GL_UNSIGNED_INT_VEC2 == uniformType;
}
template<>
inline bool GlslProg::checkUniformType<uvec3>( GLenum uniformType, std::string &type ) const
{
	type = "uvec3";
	return GL_UNSIGNED_INT_VEC3 == uniformType;
}
template<>
inline bool GlslProg::checkUniformType<uvec4>( GLenum uniformType, std::string &type ) const
{
	type = "uvec4";
	return GL_UNSIGNED_INT_VEC4 == uniformType;
}
template<>
inline bool GlslProg::checkUniformType<mat2>( GLenum uniformType, std::string &type ) const
{
	type = "mat2";
	return GL_FLOAT_MAT2 == uniformType;
}
template<>
inline bool GlslProg::checkUniformType<mat3>( GLenum uniformType, std::string &type ) const
{
	type = "mat3";
	return GL_FLOAT_MAT3 == uniformType;
}
template<>
inline bool GlslProg::checkUniformType<mat4>( GLenum uniformType, std::string &type ) const
{
	type = "mat4";
	return GL_FLOAT_MAT4 == uniformType;
}


class GlslProgExc : public cinder::gl::Exception {
  public:
	GlslProgExc()	{}
	GlslProgExc( const std::string &description ) : cinder::gl::Exception( description )	{}
};


class GlslProgCompileExc : public GlslProgExc {
  public:
	GlslProgCompileExc( const std::string &log, GLint shaderType );
};

class GlslProgLinkExc : public GlslProgExc {
  public:
	GlslProgLinkExc( const std::string &log ) : GlslProgExc( log ) {}
};

class GlslNullProgramExc : public GlslProgExc {
  public:
	virtual const char* what() const throw()
	{
		return "Glsl: Attempt to use null shader";
	}
	
};

} } // namespace cinder::gl
