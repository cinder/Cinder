#pragma once

#include "cinder/gl/GlslProg.h"


#ifdef __cplusplus
extern "C" {
#endif

int glswInit();
int glswShutdown();
int glswAddPath(const char* pathPrefix, const char* pathSuffix);
const char* glswGetShader(const char* effectKey);
const char* glswGetError();
int glswAddDirective(const char* token, const char* directive);

#ifdef __cplusplus
}
#endif




namespace cinder
{
	namespace glsw
	{
		// Resources path
		static std::string mPath = "data";
		// Glsl shader version
		static std::string mVersion = "120";

		// Sets path
		static void setResourcesFolderName( const std::string& path ) { mPath = path; }
		// Set shader version
		static void setGlslVersion( const std::string& version ) { mVersion = version; }

		int shutDown();
		int addDirective( const char* token, const char* directive );
		cinder::gl::GlslProg create( const char* vs, const char* ps=0, const char* gs=0 );
	}
}