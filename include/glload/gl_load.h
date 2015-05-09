#ifndef OPENGL_GEN_LOAD_FUNCTIONS_H
#define OPENGL_GEN_LOAD_FUNCTIONS_H
/**
\file
\brief C header to include if you want to initialize OpenGL.

**/

/**\addtogroup module_glload_cinter**/
/**@{**/

/**
\brief The loading status returned by the loading functions.

**/
enum
{
	ogl_LOAD_FAILED = 0, ///<Failed to load entirely.
	ogl_LOAD_SUCCEEDED, ///<At least partially succeded.
};

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/**
\brief Loads all of the function pointers available.

Loads function pointers for OpenGL. This function will also load the core OpenGL functions (ie: not in extensions). It will only load the version and profile specified by the current OpenGL context. So if you get a 3.2 compatibility context, then it will load only try to load 3.2 compatibility in addition to any available extensions.

\return Will return ogl_LOAD_FAILED if the loading failed entirely and nothing was loaded. Returns ogl_LOAD_SUCCEEDED if the loading process worked as planned. If it is neither, then the (return value - ogl_LOAD_SUCCEEDED) is the number of core functions that fialed to load.
**/
int ogl_LoadFunctions();

/**
This function retrieves the major GL version number. Only works after LoadFunctions has been called.
**/
int ogl_GetMajorVersion();

/**
This function retrieves the minor GL version number. Only works after LoadFunctions has been called.
**/
int ogl_GetMinorVersion();

/**Returns non-zero if the current GL version is greater than or equal to the given version.**/
int ogl_IsVersionGEQ(int testMajorVersion, int testMinorVersion);

/**@}**/

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*OPENGL_GEN_LOAD_FUNCTIONS_H*/
