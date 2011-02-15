/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Changes by Victor Martins to use Philip Rideout's GLSW
 February 2011
*/

#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <exception>
#include <map>

#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Vector.h"
#include "cinder/Matrix.h"
#include "cinder/DataSource.h"
#include "cinder/gl/glsw.h"


namespace cinder 
{ 
	namespace gl 
	{
		//! Represents an OpenGL GLSL program. \ImplShared
		class GlslEffect 
		{
		  public: 
			GlslEffect() {};
			GlslEffect( const char* v, const char* f=NULL, const char* g=NULL );
			~GlslEffect();

			static int		shutDown();
			static int		addDirective( const char* token, const char* directive );

			void			bind() const;
			static void		unbind();

			GLuint			getHandle() const { return mObj->mHandle; }

			void	uniform( const std::string &name, int data );
			void	uniform( const std::string &name, const int32_t x, const int32_t y );
			void	uniform( const std::string &name, const int *data, int count );		
			void	uniform( const std::string &name, const Vec2i *data, int count );	
			void	uniform( const std::string &name, float data );
			void	uniform( const std::string &name, const float x, const float y );
			void	uniform( const std::string &name, const float x, const float y, const float z );
			void	uniform( const std::string &name, const float x, const float y, const float z, const float w );
			void	uniform( const std::string &name, const Matrix44f &data, bool transpose = false );	
			void	uniform( const std::string &name, const float *data, int count );
			void	uniform( const std::string &name, const Vec2f *data, int count );
			void	uniform( const std::string &name, const Vec3f *data, int count );
			void	uniform( const std::string &name, const Vec4f *data, int count );

			GLint	getUniformLocation( const std::string &name );
			GLint	getAttribLocation( const std::string &name );

			std::string		getShaderLog( GLuint handle ) const;

		  protected:
			void			loadShader( const char *shaderSource, GLint shaderType );
			void			attachShaders();
			void			link();

			struct Obj {
				Obj() : mHandle( 0 ) {}
				~Obj();
		
				GLuint						mHandle;
				std::map<std::string,int>	mUniformLocs;
			};
 
			std::shared_ptr<Obj>	mObj;

		  public:
			//@{
			//! Emulates shared_ptr-like behavior
			typedef std::shared_ptr<Obj> GlslEffect::*unspecified_bool_type;
			operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &GlslEffect::mObj; }
			void reset() { mObj.reset(); }
			//@}  


			// Sets path
			static void setResourcesFolderName( const std::string& path ) { mPath = path; }
			// Set shader version
			static void setGlslVersion( const std::string& version ) { mVersion = version; }
			// Resources path
			static std::string mPath;
			// Glsl shader version
			static std::string mVersion;
		};

		class GlslEffectCompileExc : public std::exception {
		 public:	
			GlslEffectCompileExc( const std::string &log, GLint aShaderType ) throw();
			virtual const char* what() const throw()
			{
				return mMessage;
			}

		 private:
			char	mMessage[16001];
			GLint	mShaderType;
		};

		/*class GlslNullProgramExc : public std::exception {
		 public:	
			virtual const char* what() const throw()
			{
				return "Glsl: Attempt to use null shader";
			}

		};*/
	}  // namespace GL
} // namespace V
