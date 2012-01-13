/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Changes by Victor Martins to use Philip Rideout's GLSW
 February 2011
*/

#include "cinder/gl/GlslEffect.h"

using namespace std;

namespace cinder
{ 
	namespace gl
	{
		std::string GlslEffect::mPath = "data";
		std::string GlslEffect::mVersion = "120";

		unsigned long getFileLength( std::ifstream& file )
		{
			if( !file.good() ) return 0;

			std::streamoff pos = file.tellg();
			//unsigned long pos = file.tellg();
			file.seekg( 0, std::ios::end );
			std::streamoff len = file.tellg();
			//unsigned long len = file.tellg();
			file.seekg( std::ios::beg );

			return static_cast<unsigned long>(len);
		}

		char* loadSourceFile( const std::string& filename )
		{
			std::ifstream file;
			file.open( filename.c_str(), std::ios::in ); // opens as ASCII!
			if( !file )
			{
				std::stringstream str;
				str << file << " loading failed. Can't open file.";
				throw GlslEffectCompileExc( str.str(), 0 );
			}

			unsigned long len = getFileLength( file );

			if( len == 0 )
			{
				std::stringstream str;
				str << file << " loading failed. Empty file.";
				throw GlslEffectCompileExc( str.str(), 0 );
			}

			char* ShaderSource = new char[len+1];
			if( *ShaderSource == 0 )
			{
				delete [] ShaderSource;  ShaderSource = NULL;
				std::stringstream str;
				str << file << " loading failed. Can't reserve memory.";
				throw GlslEffectCompileExc( str.str(), 0 );
			}

			// len isn't always strlen cause some characters are stripped in ascii read...
			// it is important to 0-terminate the real length later, len is just max possible value... 
			ShaderSource[len] = 0; 

			unsigned int i=0;
			while( file.good() )
			{
				ShaderSource[i] = file.get();       // get character from file.
				if (!file.eof())
					i++;
			}

			ShaderSource[i] = 0;  // 0-terminate it at the correct position

			file.close();

			return ShaderSource;
		}


		GlslEffect::Obj::~Obj()
		{
			glswShutdown();
			if( mHandle )
				glDeleteProgram( (GLuint)mHandle );
		}

		//////////////////////////////////////////////////////////////////////////
		// GlslEffect
		GlslEffect::GlslEffect( const char* v, const char* f, const char* g )
			: mObj( shared_ptr<Obj>( new Obj ) )
		{
			mObj->mHandle = glCreateProgram();

			//static bool first = true;
			//if (first) 
			{
				string prefix( mPath );
				if (prefix[prefix.size() - 1] != '/')
					prefix = prefix + "/";

				if( !glswInit() )
					throw GlslEffectCompileExc( "[glsw]  Failed to init", 0 );
				if( !glswAddPath( prefix .c_str(), ".glsl" ) )
					throw GlslEffectCompileExc( "[glsw]  Failed on path add", 0 );

				std::stringstream ss;
				ss << "#version " << mVersion << std::endl;
				if( !glswAddDirective( "*", ss.str().c_str() ) )
					throw GlslEffectCompileExc( "[glsw]  Failed on directive add", 0 );

				//first = false;
			}

			if ( v )
			{  
				const char* src = glswGetShader( v );
				loadShader( src, GL_VERTEX_SHADER_ARB );
			}
			if( f )
			{
				const char* src = glswGetShader( f );
				loadShader( src, GL_FRAGMENT_SHADER_ARB );
			}
			if( g )
			{
				const char* src = glswGetShader( g );
				loadShader( src, GL_GEOMETRY_SHADER_EXT );
			}

			link();
		}

		GlslEffect::~GlslEffect()
		{ 
		}

		int GlslEffect::shutDown()
		{
			return glswShutdown();
		}


		int GlslEffect::addDirective( const char* token, const char* directive )
		{
			return glswAddDirective( token, directive );
		}

		void GlslEffect::loadShader( const char *shaderSource, GLint shaderType )
		{
			GLuint handle = glCreateShader( shaderType );
			glShaderSource( handle, 1, reinterpret_cast<const GLchar**>( &shaderSource ), NULL );
			glCompileShader( handle );
	
			GLint status;
			glGetShaderiv( (GLuint) handle, GL_COMPILE_STATUS, &status );
			if( status != GL_TRUE ) {
				std::string log = getShaderLog( (GLuint)handle );
				throw GlslEffectCompileExc( log, shaderType );
			}
			glAttachShader( mObj->mHandle, handle );
		}

		void GlslEffect::link()
		{
			glLinkProgram( mObj->mHandle );	
		}

		void GlslEffect::bind() const
		{
			glUseProgram( mObj->mHandle );
		}

		void GlslEffect::unbind()
		{
			glUseProgram( 0 );
		}

		std::string GlslEffect::getShaderLog( GLuint handle ) const
		{
			std::string log;
	
			GLchar *debugLog;
			GLint debugLength = 0, charsWritten = 0;
			glGetShaderiv( handle, GL_INFO_LOG_LENGTH, &debugLength );

			if( debugLength > 0 ) {
				debugLog = new GLchar[debugLength];
				glGetShaderInfoLog( handle, debugLength, &charsWritten, debugLog );
				log.append( debugLog, 0, debugLength );
				delete [] debugLog;
			}
	
			return log;
		}

		void GlslEffect::uniform( const std::string &name, int data )
		{
			GLint loc = getUniformLocation( name );
			glUniform1i( loc, data );
		}

		void GlslEffect::uniform( const std::string &name, const int32_t x, const int32_t y )
		{
			GLint loc = getUniformLocation( name );
			glUniform2i( loc, x, y );
		}

		void GlslEffect::uniform( const std::string &name, const int *data, int count )
		{
			GLint loc = getUniformLocation( name );
			glUniform1iv( loc, count, data );
		}

		void GlslEffect::uniform( const std::string &name, const Vec2i *data, int count )
		{
			GLint loc = getUniformLocation( name );
			glUniform2iv( loc, count, &data[0].x );
		}

		void GlslEffect::uniform( const std::string &name, float data )
		{
			GLint loc = getUniformLocation( name );
			glUniform1f( loc, data );
		}

		void GlslEffect::uniform( const std::string &name, float x, const float y )
		{
			GLint loc = getUniformLocation( name );
			glUniform2f( loc, x, y );
		}

		void GlslEffect::uniform( const std::string &name, float x, const float y, const float z )
		{
			GLint loc = getUniformLocation( name );
			glUniform3f( loc, x, y, z );
		}

		void GlslEffect::uniform( const std::string &name, float x, const float y, const float z, const float w )
		{
			GLint loc = getUniformLocation( name );
			glUniform4f( loc, x, y, z, w );
		}

		void GlslEffect::uniform( const std::string &name, const float *data, int count )
		{
			GLint loc = getUniformLocation( name );
			glUniform1fv( loc, count, data );
		}

		void GlslEffect::uniform( const std::string &name, const Vec2f *data, int count )
		{
			GLint loc = getUniformLocation( name );
			glUniform2fv( loc, count, &data[0].x );
		}

		void GlslEffect::uniform( const std::string &name, const Vec3f *data, int count )
		{
			GLint loc = getUniformLocation( name );
			glUniform3fv( loc, count, &data[0].x );
		}

		void GlslEffect::uniform( const std::string &name, const Vec4f *data, int count )
		{
			GLint loc = getUniformLocation( name );
			glUniform4fv( loc, count, &data[0].x );
		}

		void GlslEffect::uniform( const std::string &name, const Matrix44f& data, bool transpose )
		{
			GLint loc = getUniformLocation( name );
			glUniformMatrix4fv( loc, 1, ( transpose ) ? GL_TRUE : GL_FALSE, data );
		}


		GLint GlslEffect::getUniformLocation( const std::string &name )
		{
			map<string,int>::const_iterator uniformIt = mObj->mUniformLocs.find( name );
			if( uniformIt == mObj->mUniformLocs.end() ) {
				GLint loc = glGetUniformLocation( mObj->mHandle, name.c_str() );
				mObj->mUniformLocs[name] = loc;
				return loc;
			}
			else
				return uniformIt->second;
		}

		GLint GlslEffect::getAttribLocation( const std::string &name )
		{
			return glGetAttribLocation( mObj->mHandle, name.c_str() );
		}

		//////////////////////////////////////////////////////////////////////////
		// GlslEffectCompileExc
		GlslEffectCompileExc::GlslEffectCompileExc( const std::string &log, GLint aShaderType ) throw()
			: mShaderType( aShaderType )
		{
			if( mShaderType == GL_VERTEX_SHADER_ARB )
				strncpy( mMessage, "VERTEX: ", 1000 );
			else if( mShaderType == GL_FRAGMENT_SHADER_ARB )
				strncpy( mMessage, "FRAGMENT: ", 1000 );
			else if( mShaderType == GL_GEOMETRY_SHADER_EXT )
				strncpy( mMessage, "GEOMETRY: ", 1000 );
			else
				strncpy( mMessage, "UNKNOWN: ", 1000 );
			strncat( mMessage, log.c_str(), 15000 );
		}

	} // namespace gl
} // namespace cinder
