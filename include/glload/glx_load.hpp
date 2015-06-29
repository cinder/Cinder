#ifndef GLXWIN_GEN_LOAD_FUNCTIONS_HPP
#define GLXWIN_GEN_LOAD_FUNCTIONS_HPP
/**
\file
\brief C++ header to include if you want to initialize GLX.

**/

#include "_int_load_test.hpp"

///\addtogroup module_glload_cppinter
///@{

///The core namespace for the C++ interface for the OpenGL initialization functions.
namespace glload
{
	/**
	\brief Loads all of the function pointers available.

Loads function pointers for GLX extensions.

	\return A sys::LoadTest object that defines whether the loading was successful.
	**/
	glload::LoadTest LoadFunctions(Display *display, int screen);


}
///@}
#endif /*GLXWIN_GEN_LOAD_FUNCTIONS_HPP*/
