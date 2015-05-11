#ifndef WINDOWSGL_GEN_LOAD_FUNCTIONS_H
#define WINDOWSGL_GEN_LOAD_FUNCTIONS_H
/**
\file
\brief C header to include if you want to initialize WGL.

**/

/**\addtogroup module_glload_cinter**/
/**@{**/

/**
\brief The loading status returned by the loading functions.

**/
enum
{
	wgl_LOAD_FAILED = 0, ///<Failed to load entirely.
	wgl_LOAD_SUCCEEDED, ///<At least partially succeded.
};

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/**
\brief Loads all of the function pointers available.

Loads function pointers for WGL extensions.

\return Will return wgl_LOAD_FAILED if the loading failed entirely and nothing was loaded. Returns wgl_LOAD_SUCCEEDED if the loading process worked as planned. If it is neither, then the (return value - wgl_LOAD_SUCCEEDED) is the number of core functions that fialed to load.
**/
int wgl_LoadFunctions(HDC hdc);


/**@}**/

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*WINDOWSGL_GEN_LOAD_FUNCTIONS_H*/
