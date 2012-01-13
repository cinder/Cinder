/*
	GLSL program effect files
	Load your glsl shaders into one single file.

	GLSW is created by Philip Rideout http://prideout.net/blog/?p=11
	All credit goes to himself.


	Victor Martins http://www.pixelnerve.com
	2011 January
*/

#include "cinder/gl/gl.h"
#include "cinder/gl/glsw.h"


namespace cinder 
{ 
	namespace glsw 
	{
		int shutDownGLSW()
		{
			return glswShutdown();
		}

		int initGLSW()
		{
			return glswInit();
		}

		int addDirective( const char* token, const char* directive )
		{
			return glswAddDirective( token, directive );
		}

		cinder::gl::GlslProg create( const char* vs, const char* ps/*=0*/, const char* gs/*=0 */ )
		{
			std::string prefix( mPath );
			if (prefix[prefix.size() - 1] != '/')
				prefix = prefix + "/";

			static bool first = true;
			if (first) 
			{
				std::string prefix( mPath );
				if (prefix[prefix.size() - 1] != '/')
					prefix = prefix + "/";

				//glswInit();
				if( !glswInit() )
					throw gl::GlslProgCompileExc( "[glsw]  Failed to init", 0 );
				if( !glswAddPath( prefix.c_str(), ".glsl" ) )
					throw gl::GlslProgCompileExc( "[glsw]  Failed on path add", 0 );
				std::stringstream ss;
				ss << "#version " << mVersion << std::endl;
				if( !glswAddDirective( "GLVERSION", ss.str().c_str() ) )
					throw gl::GlslProgCompileExc( "[glsw]  Failed on directive add", 0 );

				first = false;
			}

			/*glswInit();
			//if( !glswInit() )
				//throw gl::GlslProgCompileExc( "[glsw]  Failed to init", 0 );
			if( !glswAddPath( prefix .c_str(), ".glsl" ) )
				throw gl::GlslProgCompileExc( "[glsw]  Failed on path add", 0 );

			std::stringstream ss;
			ss << "#version " << mVersion << std::endl;
			if( !glswAddDirective( "*", ss.str().c_str() ) )
				throw gl::GlslProgCompileExc( "[glsw]  Failed on directive add", 0 );*/

			const char* vsProgram = NULL;
			const char* psProgram = NULL;
			const char* gsProgram = NULL;
			if ( vs )
			{  
				vsProgram = glswGetShader( vs );
				//loadShader( src, GL_VERTEX_SHADER_ARB );
			}
			if( ps )
			{
				psProgram = glswGetShader( ps );
				//loadShader( src, GL_FRAGMENT_SHADER_ARB );
			}
			if( gs )
			{
				gsProgram = glswGetShader( gs );
				//loadShader( src, GL_GEOMETRY_SHADER_EXT );
			}

			cinder::gl::GlslProg program( vsProgram, psProgram, gsProgram );

			return program;
		}

	} 
} // namespace glsw::cinder
