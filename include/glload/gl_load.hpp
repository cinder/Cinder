#ifndef OPENGL_GEN_LOAD_FUNCTIONS_HPP
#define OPENGL_GEN_LOAD_FUNCTIONS_HPP
/**
\file
\brief C++ header to include if you want to initialize OpenGL.

**/

#include "_int_load_test.hpp"

///\addtogroup module_glload_cppinter
///@{

///The core namespace for the C++ interface for the OpenGL initialization functions.
namespace glload
{
	/**
	\brief Loads all of the function pointers available.

Loads function pointers for OpenGL. This function will also load the core OpenGL functions (ie: not in extensions). It will only load the version and profile specified by the current OpenGL context. So if you get a 3.2 compatibility context, then it will load only try to load 3.2 compatibility in addition to any available extensions.

	\return A sys::LoadTest object that defines whether the loading was successful.
	**/
	glload::LoadTest LoadFunctions();

		/**
		This function retrieves the major GL version number. Only works after LoadFunctions has been called.
		**/
		int GetMajorVersion();

		/**
		This function retrieves the minor GL version number. Only works after LoadFunctions has been called.
		**/
		int GetMinorVersion();

		/**Returns non-zero if the current GL version is greater than or equal to the given version.**/
		int IsVersionGEQ(int testMajorVersion, int testMinorVersion);

}
///@}
#endif /*OPENGL_GEN_LOAD_FUNCTIONS_HPP*/
