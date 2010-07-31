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

#include "cinder/gl/gl.h"
#include "cinder/Vector.h"
#include "cinder/Matrix.h"
#include "cinder/DataSource.h"

namespace cinder { namespace gl {

class GlslProg {
  public: 
	GlslProg() {}
	GlslProg( DataSourceRef vertexShader, DataSourceRef fragmentShader = DataSourceRef(), DataSourceRef geometryShader = DataSourceRef() );
	GlslProg( const char *vertexShader, const char *fragmentShader = 0, const char *geometryShader = 0 );

	void			bind() const;
	static void		unbind();

	GLuint			getHandle() const { return mObj->mHandle; }

	void	uniform( const std::string &name, int data );
	void	uniform( const std::string &name, const Vec2i &data );
	void	uniform( const std::string &name, const int *data, int count );		
	void	uniform( const std::string &name, const Vec2i *data, int count );	
	void	uniform( const std::string &name, float data );
	void	uniform( const std::string &name, const Vec2f &data );
	void	uniform( const std::string &name, const Vec3f &data );
	void	uniform( const std::string &name, const Vec4f &data );
	void	uniform( const std::string &name, const Matrix44f &data, bool transpose = false );	
	void	uniform( const std::string &name, const float *data, int count );
	void	uniform( const std::string &name, const Vec2f *data, int count );
	void	uniform( const std::string &name, const Vec3f *data, int count );
	void	uniform( const std::string &name, const Vec4f *data, int count );

	GLint	getUniformLocation( const std::string &name );
	GLint	getAttribLocation( const std::string &name );

	std::string		getShaderLog( GLuint handle ) const;

  protected:
	void			loadShader( Buffer shaderSourceBuffer, GLint shaderType );
	void			loadShader( const char *shaderSource, GLint shaderType );
	void			attachShaders();
	void			link();

	struct Obj {
		Obj() : mHandle( 0 ) {}
		~Obj();
		
		GLuint						mHandle;
		std::map<std::string,int>	mUniformLocs;
	};
 
	shared_ptr<Obj>	mObj;

  public:
	//@{
	//! Emulates shared_ptr-like behavior
	typedef shared_ptr<Obj> GlslProg::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &GlslProg::mObj; }
	void reset() { mObj.reset(); }
	//@}  
};

class GlslProgCompileExc : public std::exception {
 public:	
	GlslProgCompileExc( const std::string &log, GLint aShaderType ) throw();
	virtual const char* what() const throw()
	{
		return mMessage;
	}

 private:
	char	mMessage[16001];
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
