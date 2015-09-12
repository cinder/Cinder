#ifndef GLXWIN_GEN_LOAD_FUNCTIONS_H
#define GLXWIN_GEN_LOAD_FUNCTIONS_H
/**
\file
\brief C header to include if you want to initialize GLX.

**/

/**\addtogroup module_glload_cinter**/
/**@{**/

/**
\brief The loading status returned by the loading functions.

**/
enum
{
	glx_LOAD_FAILED = 0, ///<Failed to load entirely.
	glx_LOAD_SUCCEEDED, ///<At least partially succeded.
};

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/**
\brief Loads all of the function pointers available.

Loads function pointers for GLX extensions.

\return Will return glx_LOAD_FAILED if the loading failed entirely and nothing was loaded. Returns glx_LOAD_SUCCEEDED if the loading process worked as planned. If it is neither, then the (return value - glx_LOAD_SUCCEEDED) is the number of core functions that fialed to load.
**/
int glx_LoadFunctions(Display *display, int screen);


/**@}**/

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*GLXWIN_GEN_LOAD_FUNCTIONS_H*/
