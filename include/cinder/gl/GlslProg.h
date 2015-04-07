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

		//! Returns the map between uniform semantics and uniform names
		const std::map<std::string,UniformSemantic>&	getUniformSemantics() const { return mUniformSemanticMap; }
		//! Returns the map between attribute semantics and attribute names
		const std::map<std::string,geom::Attrib>&		getAttribSemantics() const { return mAttribSemanticMap; }
		
		//! Returns the map between attribute names and specified locations
		const std::map<std::string,GLint>&	getAttribNameLocations() const { return mAttribNameLocMap; }
		//! Returns the map between attribute semantics and specified locations
		const std::map<geom::Attrib,GLint>&	getAttribSemanticLocations() const { return mAttribSemanticLocMap; }

		//! Returns whether preprocessing is enabled or not, e.g. `#include` statements. \default true.
		bool		isPreprocessingEnabled() const				{ return mPreprocessingEnabled; }
		//! Sets whether preprocessing is enabled or not, e.g. `#include` statements.
		void		setPreprocessingEnabled( bool enable )		{ mPreprocessingEnabled = enable; }
		//! Sets whether preprocessing is enabled or not, e.g. `#include` statements.
		Format&		preprocess( bool enable )					{ mPreprocessingEnabled = enable; return *this; }
		//! Specifies a define directive to add to the shader sources
		Format&		define( const std::string &define );
		//! Specifies a define directive to add to the shader sources
		Format&		define( const std::string &define, const std::string &value );
		//! Specifies a series of define directives to add to the shader sources
		Format&		defineDirectives( const std::vector<std::string> &defines );
		//! Specifies the #version directive to add to the shader sources
		Format&		version( int version );
		//! Returns the version number associated with this GlslProg, or 0 if none was speciefied.
		int	getVersion() const										{ return mVersion; }
		//! Returns the list of `#define` directives.
		const std::vector<std::string>& getDefineDirectives() const { return mDefineDirectives; }

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
		std::map<std::string,GLint>				mAttribNameLocMap;
		std::map<geom::Attrib,GLint>			mAttribSemanticLocMap;
		std::map<std::string,UniformSemantic>	mUniformSemanticMap;
		std::map<std::string,geom::Attrib>		mAttribSemanticMap;
		std::vector<std::string>				mDefineDirectives;
		int										mVersion;
		
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
	
	void	uniform( const std::string &name, int data ) const;
	void	uniform( int location, int data ) const;	
	void	uniform( const std::string &name, const ivec2 &data ) const;
	void	uniform( int location, const ivec2 &data ) const;	
	void	uniform( const std::string &name, const int *data, int count ) const;
	void	uniform( int location, const int *data, int count ) const;	
	void	uniform( const std::string &name, const ivec2 *data, int count ) const;
	void	uniform( int location, const ivec2 *data, int count ) const;	
	void	uniform( const std::string &name, float data ) const;
	void	uniform( int location, float data ) const;	
	void	uniform( const std::string &name, const vec2 &data ) const;
	void	uniform( int location, const vec2 &data ) const;	
	void	uniform( const std::string &name, const vec3 &data ) const;
	void	uniform( int location, const vec3 &data ) const;
	void	uniform( const std::string &name, const vec4 &data ) const;
	void	uniform( int location, const vec4 &data ) const;
	void	uniform( const std::string &name, const Color &data ) const;
	void	uniform( int location, const Color &data ) const;
	void	uniform( const std::string &name, const ColorA &data ) const;
	void	uniform( int location, const ColorA &data ) const;
	void	uniform( const std::string &name, const mat3 &data, bool transpose = false ) const;
	void	uniform( int location, const mat3 &data, bool transpose = false ) const;
	void	uniform( const std::string &name, const mat4 &data, bool transpose = false ) const;
	void	uniform( int location, const mat4 &data, bool transpose = false ) const;
	void	uniform( const std::string &name, const float *data, int count ) const;
	void	uniform( int location, const float *data, int count ) const;
	void	uniform( const std::string &name, const vec2 *data, int count ) const;
	void	uniform( int location, const vec2 *data, int count ) const;
	void	uniform( const std::string &name, const vec3 *data, int count ) const;
	void	uniform( int location, const vec3 *data, int count ) const;
	void	uniform( const std::string &name, const vec4 *data, int count ) const;
	void	uniform( int location, const vec4 *data, int count ) const;
	void    uniform( int location, const mat3 *data, int count, bool transpose = false ) const;
	void    uniform( const std::string &name, const mat3 *data, int count, bool transpose = false ) const;
	void    uniform( int location, const mat4 *data, int count, bool transpose = false ) const;
	void    uniform( const std::string &name, const mat4 *data, int count, bool transpose = false ) const;

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

#if ! defined( CINDER_GL_ES_2 )
	// Uniform blocks
	//! Analogous to glUniformBlockBinding()
	void	uniformBlock( const std::string &name, GLint binding );
	//! Analogous to glUniformBlockBinding()
	void	uniformBlock( GLint loc, GLint binding );
	GLint	getUniformBlockLocation( const std::string &name ) const;
	GLint	getUniformBlockSize( GLint blockIndex ) const;
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
	std::unique_ptr<ShaderPreprocessor>		mShaderPreprocessor;

	// storage as a work-around for NVidia on MSW driver bug expecting persistent memory in calls to glTransformFeedbackVaryings
#if ! defined( CINDER_GL_ES_2 )
	std::unique_ptr<std::vector<GLchar>>	mTransformFeedbackVaryingsChars;
	std::unique_ptr<std::vector<GLchar*>>	mTransformFeedbackVaryingsCharStarts;
	mutable std::map<std::string, GLint>	mUniformBlockLocs; // map between name and location
	mutable std::map<GLint, GLint>			mUniformBlockSizes;
#endif

	friend class Context;
	friend std::ostream& operator<<( std::ostream &os, const GlslProg &rhs );
};

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
