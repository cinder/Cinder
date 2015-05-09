#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "glload/wgl_all.h"
#include "glload/wgl_load.h"

#if ! defined( CINDER_GL_ANGLE )

#if defined(__APPLE__)
#include <mach-o/dyld.h>

static void* AppleGLGetProcAddress (const GLubyte *name)
{
  static const struct mach_header* image = NULL;
  NSSymbol symbol;
  char* symbolName;
  if (NULL == image)
  {
    image = NSAddImage("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", NSADDIMAGE_OPTION_RETURN_ON_ERROR);
  }
  /* prepend a '_' for the Unix C symbol mangling convention */
  symbolName = malloc(strlen((const char*)name) + 2);
  strcpy(symbolName+1, (const char*)name);
  symbolName[0] = '_';
  symbol = NULL;
  /* if (NSIsSymbolNameDefined(symbolName))
	 symbol = NSLookupAndBindSymbol(symbolName); */
  symbol = image ? NSLookupSymbolInImage(image, symbolName, NSLOOKUPSYMBOLINIMAGE_OPTION_BIND | NSLOOKUPSYMBOLINIMAGE_OPTION_RETURN_ON_ERROR) : NULL;
  free(symbolName);
  return symbol ? NSAddressOfSymbol(symbol) : NULL;
}
#endif /* __APPLE__ */

#if defined(__sgi) || defined (__sun)
#include <dlfcn.h>
#include <stdio.h>

static void* SunGetProcAddress (const GLubyte* name)
{
  static void* h = NULL;
  static void* gpa;

  if (h == NULL)
  {
    if ((h = dlopen(NULL, RTLD_LAZY | RTLD_LOCAL)) == NULL) return NULL;
    gpa = dlsym(h, "glXGetProcAddress");
  }

  if (gpa != NULL)
    return ((void*(*)(const GLubyte*))gpa)(name);
  else
    return dlsym(h, (const char*)name);
}
#endif /* __sgi || __sun */

#if defined(_WIN32)

#ifdef _MSC_VER
#pragma warning(disable: 4055)
#pragma warning(disable: 4054)
#endif

static int TestPointer(const PROC pTest)
{
	ptrdiff_t iTest;
	if(!pTest) return 0;
	iTest = (ptrdiff_t)pTest;
	
	if(iTest == 1 || iTest == 2 || iTest == 3 || iTest == -1) return 0;
	
	return 1;
}

static PROC WinGetProcAddress(const char *name)
{
	HMODULE glMod = NULL;
	PROC pFunc = wglGetProcAddress((LPCSTR)name);
	if(TestPointer(pFunc))
	{
		return pFunc;
	}
	glMod = GetModuleHandleA("OpenGL32.dll");
	return (PROC)GetProcAddress(glMod, (LPCSTR)name);
}
	
#define IntGetProcAddress(name) WinGetProcAddress(name)
#else
	#if defined(__APPLE__)
		#define IntGetProcAddress(name) AppleGLGetProcAddress(name)
	#else
		#if defined(__sgi) || defined(__sun)
			#define IntGetProcAddress(name) SunGetProcAddress(name)
		#else /* GLX */
		    #include <GL/glx.h>

			#define IntGetProcAddress(name) (*glXGetProcAddressARB)((const GLubyte*)name)
		#endif
	#endif
#endif

int wglext_3DFX_multisample = 0;
int wglext_3DL_stereo_control = 0;
int wglext_AMD_gpu_association = 0;
int wglext_ARB_buffer_region = 0;
int wglext_ARB_create_context = 0;
int wglext_ARB_create_context_profile = 0;
int wglext_ARB_create_context_robustness = 0;
int wglext_ARB_extensions_string = 0;
int wglext_ARB_framebuffer_sRGB = 0;
int wglext_ARB_make_current_read = 0;
int wglext_ARB_multisample = 0;
int wglext_ARB_pbuffer = 0;
int wglext_ARB_pixel_format = 0;
int wglext_ARB_pixel_format_float = 0;
int wglext_ARB_render_texture = 0;
int wglext_ATI_pixel_format_float = 0;
int wglext_EXT_create_context_es2_profile = 0;
int wglext_EXT_depth_float = 0;
int wglext_EXT_framebuffer_sRGB = 0;
int wglext_EXT_make_current_read = 0;
int wglext_EXT_multisample = 0;
int wglext_EXT_pbuffer = 0;
int wglext_EXT_pixel_format = 0;
int wglext_EXT_pixel_format_packed_float = 0;
int wglext_EXT_swap_control = 0;
int wglext_EXT_swap_control_tear = 0;
int wglext_I3D_digital_video_control = 0;
int wglext_I3D_gamma = 0;
int wglext_I3D_genlock = 0;
int wglext_I3D_image_buffer = 0;
int wglext_I3D_swap_frame_lock = 0;
int wglext_NV_DX_interop = 0;
int wglext_NV_DX_interop2 = 0;
int wglext_NV_copy_image = 0;
int wglext_NV_float_buffer = 0;
int wglext_NV_gpu_affinity = 0;
int wglext_NV_multisample_coverage = 0;
int wglext_NV_present_video = 0;
int wglext_NV_render_depth_texture = 0;
int wglext_NV_render_texture_rectangle = 0;
int wglext_NV_swap_group = 0;
int wglext_NV_video_capture = 0;
int wglext_NV_video_output = 0;

PFNWGLSETSTEREOEMITTERSTATE3DLPROC _funcptr_wglSetStereoEmitterState3DL = NULL;

static int LoadExt_3DL_stereo_control()
{
	int numFailed = 0;
	_funcptr_wglSetStereoEmitterState3DL = (PFNWGLSETSTEREOEMITTERSTATE3DLPROC)IntGetProcAddress("wglSetStereoEmitterState3DL");
	if(!_funcptr_wglSetStereoEmitterState3DL) ++numFailed;
	return numFailed;
}

PFNWGLGETGPUIDSAMDPROC _funcptr_wglGetGPUIDsAMD = NULL;
PFNWGLGETGPUINFOAMDPROC _funcptr_wglGetGPUInfoAMD = NULL;
PFNWGLGETCONTEXTGPUIDAMDPROC _funcptr_wglGetContextGPUIDAMD = NULL;
PFNWGLCREATEASSOCIATEDCONTEXTAMDPROC _funcptr_wglCreateAssociatedContextAMD = NULL;
PFNWGLCREATEASSOCIATEDCONTEXTATTRIBSAMDPROC _funcptr_wglCreateAssociatedContextAttribsAMD = NULL;
PFNWGLDELETEASSOCIATEDCONTEXTAMDPROC _funcptr_wglDeleteAssociatedContextAMD = NULL;
PFNWGLMAKEASSOCIATEDCONTEXTCURRENTAMDPROC _funcptr_wglMakeAssociatedContextCurrentAMD = NULL;
PFNWGLGETCURRENTASSOCIATEDCONTEXTAMDPROC _funcptr_wglGetCurrentAssociatedContextAMD = NULL;
PFNWGLBLITCONTEXTFRAMEBUFFERAMDPROC _funcptr_wglBlitContextFramebufferAMD = NULL;

static int LoadExt_AMD_gpu_association()
{
	int numFailed = 0;
	_funcptr_wglGetGPUIDsAMD = (PFNWGLGETGPUIDSAMDPROC)IntGetProcAddress("wglGetGPUIDsAMD");
	if(!_funcptr_wglGetGPUIDsAMD) ++numFailed;
	_funcptr_wglGetGPUInfoAMD = (PFNWGLGETGPUINFOAMDPROC)IntGetProcAddress("wglGetGPUInfoAMD");
	if(!_funcptr_wglGetGPUInfoAMD) ++numFailed;
	_funcptr_wglGetContextGPUIDAMD = (PFNWGLGETCONTEXTGPUIDAMDPROC)IntGetProcAddress("wglGetContextGPUIDAMD");
	if(!_funcptr_wglGetContextGPUIDAMD) ++numFailed;
	_funcptr_wglCreateAssociatedContextAMD = (PFNWGLCREATEASSOCIATEDCONTEXTAMDPROC)IntGetProcAddress("wglCreateAssociatedContextAMD");
	if(!_funcptr_wglCreateAssociatedContextAMD) ++numFailed;
	_funcptr_wglCreateAssociatedContextAttribsAMD = (PFNWGLCREATEASSOCIATEDCONTEXTATTRIBSAMDPROC)IntGetProcAddress("wglCreateAssociatedContextAttribsAMD");
	if(!_funcptr_wglCreateAssociatedContextAttribsAMD) ++numFailed;
	_funcptr_wglDeleteAssociatedContextAMD = (PFNWGLDELETEASSOCIATEDCONTEXTAMDPROC)IntGetProcAddress("wglDeleteAssociatedContextAMD");
	if(!_funcptr_wglDeleteAssociatedContextAMD) ++numFailed;
	_funcptr_wglMakeAssociatedContextCurrentAMD = (PFNWGLMAKEASSOCIATEDCONTEXTCURRENTAMDPROC)IntGetProcAddress("wglMakeAssociatedContextCurrentAMD");
	if(!_funcptr_wglMakeAssociatedContextCurrentAMD) ++numFailed;
	_funcptr_wglGetCurrentAssociatedContextAMD = (PFNWGLGETCURRENTASSOCIATEDCONTEXTAMDPROC)IntGetProcAddress("wglGetCurrentAssociatedContextAMD");
	if(!_funcptr_wglGetCurrentAssociatedContextAMD) ++numFailed;
	_funcptr_wglBlitContextFramebufferAMD = (PFNWGLBLITCONTEXTFRAMEBUFFERAMDPROC)IntGetProcAddress("wglBlitContextFramebufferAMD");
	if(!_funcptr_wglBlitContextFramebufferAMD) ++numFailed;
	return numFailed;
}

PFNWGLCREATEBUFFERREGIONARBPROC _funcptr_wglCreateBufferRegionARB = NULL;
PFNWGLDELETEBUFFERREGIONARBPROC _funcptr_wglDeleteBufferRegionARB = NULL;
PFNWGLSAVEBUFFERREGIONARBPROC _funcptr_wglSaveBufferRegionARB = NULL;
PFNWGLRESTOREBUFFERREGIONARBPROC _funcptr_wglRestoreBufferRegionARB = NULL;

static int LoadExt_ARB_buffer_region()
{
	int numFailed = 0;
	_funcptr_wglCreateBufferRegionARB = (PFNWGLCREATEBUFFERREGIONARBPROC)IntGetProcAddress("wglCreateBufferRegionARB");
	if(!_funcptr_wglCreateBufferRegionARB) ++numFailed;
	_funcptr_wglDeleteBufferRegionARB = (PFNWGLDELETEBUFFERREGIONARBPROC)IntGetProcAddress("wglDeleteBufferRegionARB");
	if(!_funcptr_wglDeleteBufferRegionARB) ++numFailed;
	_funcptr_wglSaveBufferRegionARB = (PFNWGLSAVEBUFFERREGIONARBPROC)IntGetProcAddress("wglSaveBufferRegionARB");
	if(!_funcptr_wglSaveBufferRegionARB) ++numFailed;
	_funcptr_wglRestoreBufferRegionARB = (PFNWGLRESTOREBUFFERREGIONARBPROC)IntGetProcAddress("wglRestoreBufferRegionARB");
	if(!_funcptr_wglRestoreBufferRegionARB) ++numFailed;
	return numFailed;
}

PFNWGLCREATECONTEXTATTRIBSARBPROC _funcptr_wglCreateContextAttribsARB = NULL;

static int LoadExt_ARB_create_context()
{
	int numFailed = 0;
	_funcptr_wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)IntGetProcAddress("wglCreateContextAttribsARB");
	if(!_funcptr_wglCreateContextAttribsARB) ++numFailed;
	return numFailed;
}

PFNWGLGETEXTENSIONSSTRINGARBPROC _funcptr_wglGetExtensionsStringARB = NULL;

static int LoadExt_ARB_extensions_string()
{
	int numFailed = 0;
	_funcptr_wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)IntGetProcAddress("wglGetExtensionsStringARB");
	if(!_funcptr_wglGetExtensionsStringARB) ++numFailed;
	return numFailed;
}

PFNWGLMAKECONTEXTCURRENTARBPROC _funcptr_wglMakeContextCurrentARB = NULL;
PFNWGLGETCURRENTREADDCARBPROC _funcptr_wglGetCurrentReadDCARB = NULL;

static int LoadExt_ARB_make_current_read()
{
	int numFailed = 0;
	_funcptr_wglMakeContextCurrentARB = (PFNWGLMAKECONTEXTCURRENTARBPROC)IntGetProcAddress("wglMakeContextCurrentARB");
	if(!_funcptr_wglMakeContextCurrentARB) ++numFailed;
	_funcptr_wglGetCurrentReadDCARB = (PFNWGLGETCURRENTREADDCARBPROC)IntGetProcAddress("wglGetCurrentReadDCARB");
	if(!_funcptr_wglGetCurrentReadDCARB) ++numFailed;
	return numFailed;
}

PFNWGLCREATEPBUFFERARBPROC _funcptr_wglCreatePbufferARB = NULL;
PFNWGLGETPBUFFERDCARBPROC _funcptr_wglGetPbufferDCARB = NULL;
PFNWGLRELEASEPBUFFERDCARBPROC _funcptr_wglReleasePbufferDCARB = NULL;
PFNWGLDESTROYPBUFFERARBPROC _funcptr_wglDestroyPbufferARB = NULL;
PFNWGLQUERYPBUFFERARBPROC _funcptr_wglQueryPbufferARB = NULL;

static int LoadExt_ARB_pbuffer()
{
	int numFailed = 0;
	_funcptr_wglCreatePbufferARB = (PFNWGLCREATEPBUFFERARBPROC)IntGetProcAddress("wglCreatePbufferARB");
	if(!_funcptr_wglCreatePbufferARB) ++numFailed;
	_funcptr_wglGetPbufferDCARB = (PFNWGLGETPBUFFERDCARBPROC)IntGetProcAddress("wglGetPbufferDCARB");
	if(!_funcptr_wglGetPbufferDCARB) ++numFailed;
	_funcptr_wglReleasePbufferDCARB = (PFNWGLRELEASEPBUFFERDCARBPROC)IntGetProcAddress("wglReleasePbufferDCARB");
	if(!_funcptr_wglReleasePbufferDCARB) ++numFailed;
	_funcptr_wglDestroyPbufferARB = (PFNWGLDESTROYPBUFFERARBPROC)IntGetProcAddress("wglDestroyPbufferARB");
	if(!_funcptr_wglDestroyPbufferARB) ++numFailed;
	_funcptr_wglQueryPbufferARB = (PFNWGLQUERYPBUFFERARBPROC)IntGetProcAddress("wglQueryPbufferARB");
	if(!_funcptr_wglQueryPbufferARB) ++numFailed;
	return numFailed;
}

PFNWGLGETPIXELFORMATATTRIBIVARBPROC _funcptr_wglGetPixelFormatAttribivARB = NULL;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC _funcptr_wglGetPixelFormatAttribfvARB = NULL;
PFNWGLCHOOSEPIXELFORMATARBPROC _funcptr_wglChoosePixelFormatARB = NULL;

static int LoadExt_ARB_pixel_format()
{
	int numFailed = 0;
	_funcptr_wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)IntGetProcAddress("wglGetPixelFormatAttribivARB");
	if(!_funcptr_wglGetPixelFormatAttribivARB) ++numFailed;
	_funcptr_wglGetPixelFormatAttribfvARB = (PFNWGLGETPIXELFORMATATTRIBFVARBPROC)IntGetProcAddress("wglGetPixelFormatAttribfvARB");
	if(!_funcptr_wglGetPixelFormatAttribfvARB) ++numFailed;
	_funcptr_wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)IntGetProcAddress("wglChoosePixelFormatARB");
	if(!_funcptr_wglChoosePixelFormatARB) ++numFailed;
	return numFailed;
}

PFNWGLBINDTEXIMAGEARBPROC _funcptr_wglBindTexImageARB = NULL;
PFNWGLRELEASETEXIMAGEARBPROC _funcptr_wglReleaseTexImageARB = NULL;
PFNWGLSETPBUFFERATTRIBARBPROC _funcptr_wglSetPbufferAttribARB = NULL;

static int LoadExt_ARB_render_texture()
{
	int numFailed = 0;
	_funcptr_wglBindTexImageARB = (PFNWGLBINDTEXIMAGEARBPROC)IntGetProcAddress("wglBindTexImageARB");
	if(!_funcptr_wglBindTexImageARB) ++numFailed;
	_funcptr_wglReleaseTexImageARB = (PFNWGLRELEASETEXIMAGEARBPROC)IntGetProcAddress("wglReleaseTexImageARB");
	if(!_funcptr_wglReleaseTexImageARB) ++numFailed;
	_funcptr_wglSetPbufferAttribARB = (PFNWGLSETPBUFFERATTRIBARBPROC)IntGetProcAddress("wglSetPbufferAttribARB");
	if(!_funcptr_wglSetPbufferAttribARB) ++numFailed;
	return numFailed;
}

PFNWGLMAKECONTEXTCURRENTEXTPROC _funcptr_wglMakeContextCurrentEXT = NULL;
PFNWGLGETCURRENTREADDCEXTPROC _funcptr_wglGetCurrentReadDCEXT = NULL;

static int LoadExt_EXT_make_current_read()
{
	int numFailed = 0;
	_funcptr_wglMakeContextCurrentEXT = (PFNWGLMAKECONTEXTCURRENTEXTPROC)IntGetProcAddress("wglMakeContextCurrentEXT");
	if(!_funcptr_wglMakeContextCurrentEXT) ++numFailed;
	_funcptr_wglGetCurrentReadDCEXT = (PFNWGLGETCURRENTREADDCEXTPROC)IntGetProcAddress("wglGetCurrentReadDCEXT");
	if(!_funcptr_wglGetCurrentReadDCEXT) ++numFailed;
	return numFailed;
}

PFNWGLCREATEPBUFFEREXTPROC _funcptr_wglCreatePbufferEXT = NULL;
PFNWGLGETPBUFFERDCEXTPROC _funcptr_wglGetPbufferDCEXT = NULL;
PFNWGLRELEASEPBUFFERDCEXTPROC _funcptr_wglReleasePbufferDCEXT = NULL;
PFNWGLDESTROYPBUFFEREXTPROC _funcptr_wglDestroyPbufferEXT = NULL;
PFNWGLQUERYPBUFFEREXTPROC _funcptr_wglQueryPbufferEXT = NULL;

static int LoadExt_EXT_pbuffer()
{
	int numFailed = 0;
	_funcptr_wglCreatePbufferEXT = (PFNWGLCREATEPBUFFEREXTPROC)IntGetProcAddress("wglCreatePbufferEXT");
	if(!_funcptr_wglCreatePbufferEXT) ++numFailed;
	_funcptr_wglGetPbufferDCEXT = (PFNWGLGETPBUFFERDCEXTPROC)IntGetProcAddress("wglGetPbufferDCEXT");
	if(!_funcptr_wglGetPbufferDCEXT) ++numFailed;
	_funcptr_wglReleasePbufferDCEXT = (PFNWGLRELEASEPBUFFERDCEXTPROC)IntGetProcAddress("wglReleasePbufferDCEXT");
	if(!_funcptr_wglReleasePbufferDCEXT) ++numFailed;
	_funcptr_wglDestroyPbufferEXT = (PFNWGLDESTROYPBUFFEREXTPROC)IntGetProcAddress("wglDestroyPbufferEXT");
	if(!_funcptr_wglDestroyPbufferEXT) ++numFailed;
	_funcptr_wglQueryPbufferEXT = (PFNWGLQUERYPBUFFEREXTPROC)IntGetProcAddress("wglQueryPbufferEXT");
	if(!_funcptr_wglQueryPbufferEXT) ++numFailed;
	return numFailed;
}

PFNWGLGETPIXELFORMATATTRIBIVEXTPROC _funcptr_wglGetPixelFormatAttribivEXT = NULL;
PFNWGLGETPIXELFORMATATTRIBFVEXTPROC _funcptr_wglGetPixelFormatAttribfvEXT = NULL;
PFNWGLCHOOSEPIXELFORMATEXTPROC _funcptr_wglChoosePixelFormatEXT = NULL;

static int LoadExt_EXT_pixel_format()
{
	int numFailed = 0;
	_funcptr_wglGetPixelFormatAttribivEXT = (PFNWGLGETPIXELFORMATATTRIBIVEXTPROC)IntGetProcAddress("wglGetPixelFormatAttribivEXT");
	if(!_funcptr_wglGetPixelFormatAttribivEXT) ++numFailed;
	_funcptr_wglGetPixelFormatAttribfvEXT = (PFNWGLGETPIXELFORMATATTRIBFVEXTPROC)IntGetProcAddress("wglGetPixelFormatAttribfvEXT");
	if(!_funcptr_wglGetPixelFormatAttribfvEXT) ++numFailed;
	_funcptr_wglChoosePixelFormatEXT = (PFNWGLCHOOSEPIXELFORMATEXTPROC)IntGetProcAddress("wglChoosePixelFormatEXT");
	if(!_funcptr_wglChoosePixelFormatEXT) ++numFailed;
	return numFailed;
}

PFNWGLSWAPINTERVALEXTPROC _funcptr_wglSwapIntervalEXT = NULL;
PFNWGLGETSWAPINTERVALEXTPROC _funcptr_wglGetSwapIntervalEXT = NULL;

static int LoadExt_EXT_swap_control()
{
	int numFailed = 0;
	_funcptr_wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)IntGetProcAddress("wglSwapIntervalEXT");
	if(!_funcptr_wglSwapIntervalEXT) ++numFailed;
	_funcptr_wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)IntGetProcAddress("wglGetSwapIntervalEXT");
	if(!_funcptr_wglGetSwapIntervalEXT) ++numFailed;
	return numFailed;
}

PFNWGLGETDIGITALVIDEOPARAMETERSI3DPROC _funcptr_wglGetDigitalVideoParametersI3D = NULL;
PFNWGLSETDIGITALVIDEOPARAMETERSI3DPROC _funcptr_wglSetDigitalVideoParametersI3D = NULL;

static int LoadExt_I3D_digital_video_control()
{
	int numFailed = 0;
	_funcptr_wglGetDigitalVideoParametersI3D = (PFNWGLGETDIGITALVIDEOPARAMETERSI3DPROC)IntGetProcAddress("wglGetDigitalVideoParametersI3D");
	if(!_funcptr_wglGetDigitalVideoParametersI3D) ++numFailed;
	_funcptr_wglSetDigitalVideoParametersI3D = (PFNWGLSETDIGITALVIDEOPARAMETERSI3DPROC)IntGetProcAddress("wglSetDigitalVideoParametersI3D");
	if(!_funcptr_wglSetDigitalVideoParametersI3D) ++numFailed;
	return numFailed;
}

PFNWGLGETGAMMATABLEPARAMETERSI3DPROC _funcptr_wglGetGammaTableParametersI3D = NULL;
PFNWGLSETGAMMATABLEPARAMETERSI3DPROC _funcptr_wglSetGammaTableParametersI3D = NULL;
PFNWGLGETGAMMATABLEI3DPROC _funcptr_wglGetGammaTableI3D = NULL;
PFNWGLSETGAMMATABLEI3DPROC _funcptr_wglSetGammaTableI3D = NULL;

static int LoadExt_I3D_gamma()
{
	int numFailed = 0;
	_funcptr_wglGetGammaTableParametersI3D = (PFNWGLGETGAMMATABLEPARAMETERSI3DPROC)IntGetProcAddress("wglGetGammaTableParametersI3D");
	if(!_funcptr_wglGetGammaTableParametersI3D) ++numFailed;
	_funcptr_wglSetGammaTableParametersI3D = (PFNWGLSETGAMMATABLEPARAMETERSI3DPROC)IntGetProcAddress("wglSetGammaTableParametersI3D");
	if(!_funcptr_wglSetGammaTableParametersI3D) ++numFailed;
	_funcptr_wglGetGammaTableI3D = (PFNWGLGETGAMMATABLEI3DPROC)IntGetProcAddress("wglGetGammaTableI3D");
	if(!_funcptr_wglGetGammaTableI3D) ++numFailed;
	_funcptr_wglSetGammaTableI3D = (PFNWGLSETGAMMATABLEI3DPROC)IntGetProcAddress("wglSetGammaTableI3D");
	if(!_funcptr_wglSetGammaTableI3D) ++numFailed;
	return numFailed;
}

PFNWGLENABLEGENLOCKI3DPROC _funcptr_wglEnableGenlockI3D = NULL;
PFNWGLDISABLEGENLOCKI3DPROC _funcptr_wglDisableGenlockI3D = NULL;
PFNWGLISENABLEDGENLOCKI3DPROC _funcptr_wglIsEnabledGenlockI3D = NULL;
PFNWGLGENLOCKSOURCEI3DPROC _funcptr_wglGenlockSourceI3D = NULL;
PFNWGLGETGENLOCKSOURCEI3DPROC _funcptr_wglGetGenlockSourceI3D = NULL;
PFNWGLGENLOCKSOURCEEDGEI3DPROC _funcptr_wglGenlockSourceEdgeI3D = NULL;
PFNWGLGETGENLOCKSOURCEEDGEI3DPROC _funcptr_wglGetGenlockSourceEdgeI3D = NULL;
PFNWGLGENLOCKSAMPLERATEI3DPROC _funcptr_wglGenlockSampleRateI3D = NULL;
PFNWGLGETGENLOCKSAMPLERATEI3DPROC _funcptr_wglGetGenlockSampleRateI3D = NULL;
PFNWGLGENLOCKSOURCEDELAYI3DPROC _funcptr_wglGenlockSourceDelayI3D = NULL;
PFNWGLGETGENLOCKSOURCEDELAYI3DPROC _funcptr_wglGetGenlockSourceDelayI3D = NULL;
PFNWGLQUERYGENLOCKMAXSOURCEDELAYI3DPROC _funcptr_wglQueryGenlockMaxSourceDelayI3D = NULL;

static int LoadExt_I3D_genlock()
{
	int numFailed = 0;
	_funcptr_wglEnableGenlockI3D = (PFNWGLENABLEGENLOCKI3DPROC)IntGetProcAddress("wglEnableGenlockI3D");
	if(!_funcptr_wglEnableGenlockI3D) ++numFailed;
	_funcptr_wglDisableGenlockI3D = (PFNWGLDISABLEGENLOCKI3DPROC)IntGetProcAddress("wglDisableGenlockI3D");
	if(!_funcptr_wglDisableGenlockI3D) ++numFailed;
	_funcptr_wglIsEnabledGenlockI3D = (PFNWGLISENABLEDGENLOCKI3DPROC)IntGetProcAddress("wglIsEnabledGenlockI3D");
	if(!_funcptr_wglIsEnabledGenlockI3D) ++numFailed;
	_funcptr_wglGenlockSourceI3D = (PFNWGLGENLOCKSOURCEI3DPROC)IntGetProcAddress("wglGenlockSourceI3D");
	if(!_funcptr_wglGenlockSourceI3D) ++numFailed;
	_funcptr_wglGetGenlockSourceI3D = (PFNWGLGETGENLOCKSOURCEI3DPROC)IntGetProcAddress("wglGetGenlockSourceI3D");
	if(!_funcptr_wglGetGenlockSourceI3D) ++numFailed;
	_funcptr_wglGenlockSourceEdgeI3D = (PFNWGLGENLOCKSOURCEEDGEI3DPROC)IntGetProcAddress("wglGenlockSourceEdgeI3D");
	if(!_funcptr_wglGenlockSourceEdgeI3D) ++numFailed;
	_funcptr_wglGetGenlockSourceEdgeI3D = (PFNWGLGETGENLOCKSOURCEEDGEI3DPROC)IntGetProcAddress("wglGetGenlockSourceEdgeI3D");
	if(!_funcptr_wglGetGenlockSourceEdgeI3D) ++numFailed;
	_funcptr_wglGenlockSampleRateI3D = (PFNWGLGENLOCKSAMPLERATEI3DPROC)IntGetProcAddress("wglGenlockSampleRateI3D");
	if(!_funcptr_wglGenlockSampleRateI3D) ++numFailed;
	_funcptr_wglGetGenlockSampleRateI3D = (PFNWGLGETGENLOCKSAMPLERATEI3DPROC)IntGetProcAddress("wglGetGenlockSampleRateI3D");
	if(!_funcptr_wglGetGenlockSampleRateI3D) ++numFailed;
	_funcptr_wglGenlockSourceDelayI3D = (PFNWGLGENLOCKSOURCEDELAYI3DPROC)IntGetProcAddress("wglGenlockSourceDelayI3D");
	if(!_funcptr_wglGenlockSourceDelayI3D) ++numFailed;
	_funcptr_wglGetGenlockSourceDelayI3D = (PFNWGLGETGENLOCKSOURCEDELAYI3DPROC)IntGetProcAddress("wglGetGenlockSourceDelayI3D");
	if(!_funcptr_wglGetGenlockSourceDelayI3D) ++numFailed;
	_funcptr_wglQueryGenlockMaxSourceDelayI3D = (PFNWGLQUERYGENLOCKMAXSOURCEDELAYI3DPROC)IntGetProcAddress("wglQueryGenlockMaxSourceDelayI3D");
	if(!_funcptr_wglQueryGenlockMaxSourceDelayI3D) ++numFailed;
	return numFailed;
}

PFNWGLCREATEIMAGEBUFFERI3DPROC _funcptr_wglCreateImageBufferI3D = NULL;
PFNWGLDESTROYIMAGEBUFFERI3DPROC _funcptr_wglDestroyImageBufferI3D = NULL;
PFNWGLASSOCIATEIMAGEBUFFEREVENTSI3DPROC _funcptr_wglAssociateImageBufferEventsI3D = NULL;
PFNWGLRELEASEIMAGEBUFFEREVENTSI3DPROC _funcptr_wglReleaseImageBufferEventsI3D = NULL;

static int LoadExt_I3D_image_buffer()
{
	int numFailed = 0;
	_funcptr_wglCreateImageBufferI3D = (PFNWGLCREATEIMAGEBUFFERI3DPROC)IntGetProcAddress("wglCreateImageBufferI3D");
	if(!_funcptr_wglCreateImageBufferI3D) ++numFailed;
	_funcptr_wglDestroyImageBufferI3D = (PFNWGLDESTROYIMAGEBUFFERI3DPROC)IntGetProcAddress("wglDestroyImageBufferI3D");
	if(!_funcptr_wglDestroyImageBufferI3D) ++numFailed;
	_funcptr_wglAssociateImageBufferEventsI3D = (PFNWGLASSOCIATEIMAGEBUFFEREVENTSI3DPROC)IntGetProcAddress("wglAssociateImageBufferEventsI3D");
	if(!_funcptr_wglAssociateImageBufferEventsI3D) ++numFailed;
	_funcptr_wglReleaseImageBufferEventsI3D = (PFNWGLRELEASEIMAGEBUFFEREVENTSI3DPROC)IntGetProcAddress("wglReleaseImageBufferEventsI3D");
	if(!_funcptr_wglReleaseImageBufferEventsI3D) ++numFailed;
	return numFailed;
}

PFNWGLENABLEFRAMELOCKI3DPROC _funcptr_wglEnableFrameLockI3D = NULL;
PFNWGLDISABLEFRAMELOCKI3DPROC _funcptr_wglDisableFrameLockI3D = NULL;
PFNWGLISENABLEDFRAMELOCKI3DPROC _funcptr_wglIsEnabledFrameLockI3D = NULL;
PFNWGLQUERYFRAMELOCKMASTERI3DPROC _funcptr_wglQueryFrameLockMasterI3D = NULL;

static int LoadExt_I3D_swap_frame_lock()
{
	int numFailed = 0;
	_funcptr_wglEnableFrameLockI3D = (PFNWGLENABLEFRAMELOCKI3DPROC)IntGetProcAddress("wglEnableFrameLockI3D");
	if(!_funcptr_wglEnableFrameLockI3D) ++numFailed;
	_funcptr_wglDisableFrameLockI3D = (PFNWGLDISABLEFRAMELOCKI3DPROC)IntGetProcAddress("wglDisableFrameLockI3D");
	if(!_funcptr_wglDisableFrameLockI3D) ++numFailed;
	_funcptr_wglIsEnabledFrameLockI3D = (PFNWGLISENABLEDFRAMELOCKI3DPROC)IntGetProcAddress("wglIsEnabledFrameLockI3D");
	if(!_funcptr_wglIsEnabledFrameLockI3D) ++numFailed;
	_funcptr_wglQueryFrameLockMasterI3D = (PFNWGLQUERYFRAMELOCKMASTERI3DPROC)IntGetProcAddress("wglQueryFrameLockMasterI3D");
	if(!_funcptr_wglQueryFrameLockMasterI3D) ++numFailed;
	return numFailed;
}

PFNWGLDXSETRESOURCESHAREHANDLENVPROC _funcptr_wglDXSetResourceShareHandleNV = NULL;
PFNWGLDXOPENDEVICENVPROC _funcptr_wglDXOpenDeviceNV = NULL;
PFNWGLDXCLOSEDEVICENVPROC _funcptr_wglDXCloseDeviceNV = NULL;
PFNWGLDXREGISTEROBJECTNVPROC _funcptr_wglDXRegisterObjectNV = NULL;
PFNWGLDXUNREGISTEROBJECTNVPROC _funcptr_wglDXUnregisterObjectNV = NULL;
PFNWGLDXOBJECTACCESSNVPROC _funcptr_wglDXObjectAccessNV = NULL;
PFNWGLDXLOCKOBJECTSNVPROC _funcptr_wglDXLockObjectsNV = NULL;
PFNWGLDXUNLOCKOBJECTSNVPROC _funcptr_wglDXUnlockObjectsNV = NULL;

static int LoadExt_NV_DX_interop()
{
	int numFailed = 0;
	_funcptr_wglDXSetResourceShareHandleNV = (PFNWGLDXSETRESOURCESHAREHANDLENVPROC)IntGetProcAddress("wglDXSetResourceShareHandleNV");
	if(!_funcptr_wglDXSetResourceShareHandleNV) ++numFailed;
	_funcptr_wglDXOpenDeviceNV = (PFNWGLDXOPENDEVICENVPROC)IntGetProcAddress("wglDXOpenDeviceNV");
	if(!_funcptr_wglDXOpenDeviceNV) ++numFailed;
	_funcptr_wglDXCloseDeviceNV = (PFNWGLDXCLOSEDEVICENVPROC)IntGetProcAddress("wglDXCloseDeviceNV");
	if(!_funcptr_wglDXCloseDeviceNV) ++numFailed;
	_funcptr_wglDXRegisterObjectNV = (PFNWGLDXREGISTEROBJECTNVPROC)IntGetProcAddress("wglDXRegisterObjectNV");
	if(!_funcptr_wglDXRegisterObjectNV) ++numFailed;
	_funcptr_wglDXUnregisterObjectNV = (PFNWGLDXUNREGISTEROBJECTNVPROC)IntGetProcAddress("wglDXUnregisterObjectNV");
	if(!_funcptr_wglDXUnregisterObjectNV) ++numFailed;
	_funcptr_wglDXObjectAccessNV = (PFNWGLDXOBJECTACCESSNVPROC)IntGetProcAddress("wglDXObjectAccessNV");
	if(!_funcptr_wglDXObjectAccessNV) ++numFailed;
	_funcptr_wglDXLockObjectsNV = (PFNWGLDXLOCKOBJECTSNVPROC)IntGetProcAddress("wglDXLockObjectsNV");
	if(!_funcptr_wglDXLockObjectsNV) ++numFailed;
	_funcptr_wglDXUnlockObjectsNV = (PFNWGLDXUNLOCKOBJECTSNVPROC)IntGetProcAddress("wglDXUnlockObjectsNV");
	if(!_funcptr_wglDXUnlockObjectsNV) ++numFailed;
	return numFailed;
}

PFNWGLCOPYIMAGESUBDATANVPROC _funcptr_wglCopyImageSubDataNV = NULL;

static int LoadExt_NV_copy_image()
{
	int numFailed = 0;
	_funcptr_wglCopyImageSubDataNV = (PFNWGLCOPYIMAGESUBDATANVPROC)IntGetProcAddress("wglCopyImageSubDataNV");
	if(!_funcptr_wglCopyImageSubDataNV) ++numFailed;
	return numFailed;
}

PFNWGLENUMGPUSNVPROC _funcptr_wglEnumGpusNV = NULL;
PFNWGLENUMGPUDEVICESNVPROC _funcptr_wglEnumGpuDevicesNV = NULL;
PFNWGLCREATEAFFINITYDCNVPROC _funcptr_wglCreateAffinityDCNV = NULL;
PFNWGLENUMGPUSFROMAFFINITYDCNVPROC _funcptr_wglEnumGpusFromAffinityDCNV = NULL;
PFNWGLDELETEDCNVPROC _funcptr_wglDeleteDCNV = NULL;

static int LoadExt_NV_gpu_affinity()
{
	int numFailed = 0;
	_funcptr_wglEnumGpusNV = (PFNWGLENUMGPUSNVPROC)IntGetProcAddress("wglEnumGpusNV");
	if(!_funcptr_wglEnumGpusNV) ++numFailed;
	_funcptr_wglEnumGpuDevicesNV = (PFNWGLENUMGPUDEVICESNVPROC)IntGetProcAddress("wglEnumGpuDevicesNV");
	if(!_funcptr_wglEnumGpuDevicesNV) ++numFailed;
	_funcptr_wglCreateAffinityDCNV = (PFNWGLCREATEAFFINITYDCNVPROC)IntGetProcAddress("wglCreateAffinityDCNV");
	if(!_funcptr_wglCreateAffinityDCNV) ++numFailed;
	_funcptr_wglEnumGpusFromAffinityDCNV = (PFNWGLENUMGPUSFROMAFFINITYDCNVPROC)IntGetProcAddress("wglEnumGpusFromAffinityDCNV");
	if(!_funcptr_wglEnumGpusFromAffinityDCNV) ++numFailed;
	_funcptr_wglDeleteDCNV = (PFNWGLDELETEDCNVPROC)IntGetProcAddress("wglDeleteDCNV");
	if(!_funcptr_wglDeleteDCNV) ++numFailed;
	return numFailed;
}

PFNWGLENUMERATEVIDEODEVICESNVPROC _funcptr_wglEnumerateVideoDevicesNV = NULL;
PFNWGLBINDVIDEODEVICENVPROC _funcptr_wglBindVideoDeviceNV = NULL;
PFNWGLQUERYCURRENTCONTEXTNVPROC _funcptr_wglQueryCurrentContextNV = NULL;

static int LoadExt_NV_present_video()
{
	int numFailed = 0;
	_funcptr_wglEnumerateVideoDevicesNV = (PFNWGLENUMERATEVIDEODEVICESNVPROC)IntGetProcAddress("wglEnumerateVideoDevicesNV");
	if(!_funcptr_wglEnumerateVideoDevicesNV) ++numFailed;
	_funcptr_wglBindVideoDeviceNV = (PFNWGLBINDVIDEODEVICENVPROC)IntGetProcAddress("wglBindVideoDeviceNV");
	if(!_funcptr_wglBindVideoDeviceNV) ++numFailed;
	_funcptr_wglQueryCurrentContextNV = (PFNWGLQUERYCURRENTCONTEXTNVPROC)IntGetProcAddress("wglQueryCurrentContextNV");
	if(!_funcptr_wglQueryCurrentContextNV) ++numFailed;
	return numFailed;
}

PFNWGLJOINSWAPGROUPNVPROC _funcptr_wglJoinSwapGroupNV = NULL;
PFNWGLBINDSWAPBARRIERNVPROC _funcptr_wglBindSwapBarrierNV = NULL;
PFNWGLQUERYSWAPGROUPNVPROC _funcptr_wglQuerySwapGroupNV = NULL;
PFNWGLQUERYMAXSWAPGROUPSNVPROC _funcptr_wglQueryMaxSwapGroupsNV = NULL;
PFNWGLQUERYFRAMECOUNTNVPROC _funcptr_wglQueryFrameCountNV = NULL;
PFNWGLRESETFRAMECOUNTNVPROC _funcptr_wglResetFrameCountNV = NULL;

static int LoadExt_NV_swap_group()
{
	int numFailed = 0;
	_funcptr_wglJoinSwapGroupNV = (PFNWGLJOINSWAPGROUPNVPROC)IntGetProcAddress("wglJoinSwapGroupNV");
	if(!_funcptr_wglJoinSwapGroupNV) ++numFailed;
	_funcptr_wglBindSwapBarrierNV = (PFNWGLBINDSWAPBARRIERNVPROC)IntGetProcAddress("wglBindSwapBarrierNV");
	if(!_funcptr_wglBindSwapBarrierNV) ++numFailed;
	_funcptr_wglQuerySwapGroupNV = (PFNWGLQUERYSWAPGROUPNVPROC)IntGetProcAddress("wglQuerySwapGroupNV");
	if(!_funcptr_wglQuerySwapGroupNV) ++numFailed;
	_funcptr_wglQueryMaxSwapGroupsNV = (PFNWGLQUERYMAXSWAPGROUPSNVPROC)IntGetProcAddress("wglQueryMaxSwapGroupsNV");
	if(!_funcptr_wglQueryMaxSwapGroupsNV) ++numFailed;
	_funcptr_wglQueryFrameCountNV = (PFNWGLQUERYFRAMECOUNTNVPROC)IntGetProcAddress("wglQueryFrameCountNV");
	if(!_funcptr_wglQueryFrameCountNV) ++numFailed;
	_funcptr_wglResetFrameCountNV = (PFNWGLRESETFRAMECOUNTNVPROC)IntGetProcAddress("wglResetFrameCountNV");
	if(!_funcptr_wglResetFrameCountNV) ++numFailed;
	return numFailed;
}

PFNWGLBINDVIDEOCAPTUREDEVICENVPROC _funcptr_wglBindVideoCaptureDeviceNV = NULL;
PFNWGLENUMERATEVIDEOCAPTUREDEVICESNVPROC _funcptr_wglEnumerateVideoCaptureDevicesNV = NULL;
PFNWGLLOCKVIDEOCAPTUREDEVICENVPROC _funcptr_wglLockVideoCaptureDeviceNV = NULL;
PFNWGLQUERYVIDEOCAPTUREDEVICENVPROC _funcptr_wglQueryVideoCaptureDeviceNV = NULL;
PFNWGLRELEASEVIDEOCAPTUREDEVICENVPROC _funcptr_wglReleaseVideoCaptureDeviceNV = NULL;

static int LoadExt_NV_video_capture()
{
	int numFailed = 0;
	_funcptr_wglBindVideoCaptureDeviceNV = (PFNWGLBINDVIDEOCAPTUREDEVICENVPROC)IntGetProcAddress("wglBindVideoCaptureDeviceNV");
	if(!_funcptr_wglBindVideoCaptureDeviceNV) ++numFailed;
	_funcptr_wglEnumerateVideoCaptureDevicesNV = (PFNWGLENUMERATEVIDEOCAPTUREDEVICESNVPROC)IntGetProcAddress("wglEnumerateVideoCaptureDevicesNV");
	if(!_funcptr_wglEnumerateVideoCaptureDevicesNV) ++numFailed;
	_funcptr_wglLockVideoCaptureDeviceNV = (PFNWGLLOCKVIDEOCAPTUREDEVICENVPROC)IntGetProcAddress("wglLockVideoCaptureDeviceNV");
	if(!_funcptr_wglLockVideoCaptureDeviceNV) ++numFailed;
	_funcptr_wglQueryVideoCaptureDeviceNV = (PFNWGLQUERYVIDEOCAPTUREDEVICENVPROC)IntGetProcAddress("wglQueryVideoCaptureDeviceNV");
	if(!_funcptr_wglQueryVideoCaptureDeviceNV) ++numFailed;
	_funcptr_wglReleaseVideoCaptureDeviceNV = (PFNWGLRELEASEVIDEOCAPTUREDEVICENVPROC)IntGetProcAddress("wglReleaseVideoCaptureDeviceNV");
	if(!_funcptr_wglReleaseVideoCaptureDeviceNV) ++numFailed;
	return numFailed;
}

PFNWGLGETVIDEODEVICENVPROC _funcptr_wglGetVideoDeviceNV = NULL;
PFNWGLRELEASEVIDEODEVICENVPROC _funcptr_wglReleaseVideoDeviceNV = NULL;
PFNWGLBINDVIDEOIMAGENVPROC _funcptr_wglBindVideoImageNV = NULL;
PFNWGLRELEASEVIDEOIMAGENVPROC _funcptr_wglReleaseVideoImageNV = NULL;
PFNWGLSENDPBUFFERTOVIDEONVPROC _funcptr_wglSendPbufferToVideoNV = NULL;
PFNWGLGETVIDEOINFONVPROC _funcptr_wglGetVideoInfoNV = NULL;

static int LoadExt_NV_video_output()
{
	int numFailed = 0;
	_funcptr_wglGetVideoDeviceNV = (PFNWGLGETVIDEODEVICENVPROC)IntGetProcAddress("wglGetVideoDeviceNV");
	if(!_funcptr_wglGetVideoDeviceNV) ++numFailed;
	_funcptr_wglReleaseVideoDeviceNV = (PFNWGLRELEASEVIDEODEVICENVPROC)IntGetProcAddress("wglReleaseVideoDeviceNV");
	if(!_funcptr_wglReleaseVideoDeviceNV) ++numFailed;
	_funcptr_wglBindVideoImageNV = (PFNWGLBINDVIDEOIMAGENVPROC)IntGetProcAddress("wglBindVideoImageNV");
	if(!_funcptr_wglBindVideoImageNV) ++numFailed;
	_funcptr_wglReleaseVideoImageNV = (PFNWGLRELEASEVIDEOIMAGENVPROC)IntGetProcAddress("wglReleaseVideoImageNV");
	if(!_funcptr_wglReleaseVideoImageNV) ++numFailed;
	_funcptr_wglSendPbufferToVideoNV = (PFNWGLSENDPBUFFERTOVIDEONVPROC)IntGetProcAddress("wglSendPbufferToVideoNV");
	if(!_funcptr_wglSendPbufferToVideoNV) ++numFailed;
	_funcptr_wglGetVideoInfoNV = (PFNWGLGETVIDEOINFONVPROC)IntGetProcAddress("wglGetVideoInfoNV");
	if(!_funcptr_wglGetVideoInfoNV) ++numFailed;
	return numFailed;
}


typedef int (*PFN_LOADFUNCPOINTERS)();
typedef struct wgl_StrToExtMap_s
{
	char *extensionName;
	int *extensionVariable;
	PFN_LOADFUNCPOINTERS LoadExtension;
} wgl_StrToExtMap;

static wgl_StrToExtMap ExtensionTable[43] = {
	{"WGL_3DFX_multisample", &wglext_3DFX_multisample, NULL},
	{"WGL_3DL_stereo_control", &wglext_3DL_stereo_control, LoadExt_3DL_stereo_control},
	{"WGL_AMD_gpu_association", &wglext_AMD_gpu_association, LoadExt_AMD_gpu_association},
	{"WGL_ARB_buffer_region", &wglext_ARB_buffer_region, LoadExt_ARB_buffer_region},
	{"WGL_ARB_create_context", &wglext_ARB_create_context, LoadExt_ARB_create_context},
	{"WGL_ARB_create_context_profile", &wglext_ARB_create_context_profile, NULL},
	{"WGL_ARB_create_context_robustness", &wglext_ARB_create_context_robustness, NULL},
	{"WGL_ARB_extensions_string", &wglext_ARB_extensions_string, LoadExt_ARB_extensions_string},
	{"WGL_ARB_framebuffer_sRGB", &wglext_ARB_framebuffer_sRGB, NULL},
	{"WGL_ARB_make_current_read", &wglext_ARB_make_current_read, LoadExt_ARB_make_current_read},
	{"WGL_ARB_multisample", &wglext_ARB_multisample, NULL},
	{"WGL_ARB_pbuffer", &wglext_ARB_pbuffer, LoadExt_ARB_pbuffer},
	{"WGL_ARB_pixel_format", &wglext_ARB_pixel_format, LoadExt_ARB_pixel_format},
	{"WGL_ARB_pixel_format_float", &wglext_ARB_pixel_format_float, NULL},
	{"WGL_ARB_render_texture", &wglext_ARB_render_texture, LoadExt_ARB_render_texture},
	{"WGL_ATI_pixel_format_float", &wglext_ATI_pixel_format_float, NULL},
	{"WGL_EXT_create_context_es2_profile", &wglext_EXT_create_context_es2_profile, NULL},
	{"WGL_EXT_depth_float", &wglext_EXT_depth_float, NULL},
	{"WGL_EXT_framebuffer_sRGB", &wglext_EXT_framebuffer_sRGB, NULL},
	{"WGL_EXT_make_current_read", &wglext_EXT_make_current_read, LoadExt_EXT_make_current_read},
	{"WGL_EXT_multisample", &wglext_EXT_multisample, NULL},
	{"WGL_EXT_pbuffer", &wglext_EXT_pbuffer, LoadExt_EXT_pbuffer},
	{"WGL_EXT_pixel_format", &wglext_EXT_pixel_format, LoadExt_EXT_pixel_format},
	{"WGL_EXT_pixel_format_packed_float", &wglext_EXT_pixel_format_packed_float, NULL},
	{"WGL_EXT_swap_control", &wglext_EXT_swap_control, LoadExt_EXT_swap_control},
	{"WGL_EXT_swap_control_tear", &wglext_EXT_swap_control_tear, NULL},
	{"WGL_I3D_digital_video_control", &wglext_I3D_digital_video_control, LoadExt_I3D_digital_video_control},
	{"WGL_I3D_gamma", &wglext_I3D_gamma, LoadExt_I3D_gamma},
	{"WGL_I3D_genlock", &wglext_I3D_genlock, LoadExt_I3D_genlock},
	{"WGL_I3D_image_buffer", &wglext_I3D_image_buffer, LoadExt_I3D_image_buffer},
	{"WGL_I3D_swap_frame_lock", &wglext_I3D_swap_frame_lock, LoadExt_I3D_swap_frame_lock},
	{"WGL_NV_DX_interop", &wglext_NV_DX_interop, LoadExt_NV_DX_interop},
	{"WGL_NV_DX_interop2", &wglext_NV_DX_interop2, NULL},
	{"WGL_NV_copy_image", &wglext_NV_copy_image, LoadExt_NV_copy_image},
	{"WGL_NV_float_buffer", &wglext_NV_float_buffer, NULL},
	{"WGL_NV_gpu_affinity", &wglext_NV_gpu_affinity, LoadExt_NV_gpu_affinity},
	{"WGL_NV_multisample_coverage", &wglext_NV_multisample_coverage, NULL},
	{"WGL_NV_present_video", &wglext_NV_present_video, LoadExt_NV_present_video},
	{"WGL_NV_render_depth_texture", &wglext_NV_render_depth_texture, NULL},
	{"WGL_NV_render_texture_rectangle", &wglext_NV_render_texture_rectangle, NULL},
	{"WGL_NV_swap_group", &wglext_NV_swap_group, LoadExt_NV_swap_group},
	{"WGL_NV_video_capture", &wglext_NV_video_capture, LoadExt_NV_video_capture},
	{"WGL_NV_video_output", &wglext_NV_video_output, LoadExt_NV_video_output},
};

static int g_extensionMapSize = 43;

static wgl_StrToExtMap *FindExtEntry(const char *extensionName)
{
	int loop;
	wgl_StrToExtMap *currLoc = ExtensionTable;
	for(loop = 0; loop < g_extensionMapSize; ++loop, ++currLoc)
	{
		if(strcmp(extensionName, currLoc->extensionName) == 0)
			return currLoc;
	}
	
	return NULL;
}

static void ClearExtensionVars()
{
	wglext_3DFX_multisample = 0;
	wglext_3DL_stereo_control = 0;
	wglext_AMD_gpu_association = 0;
	wglext_ARB_buffer_region = 0;
	wglext_ARB_create_context = 0;
	wglext_ARB_create_context_profile = 0;
	wglext_ARB_create_context_robustness = 0;
	wglext_ARB_extensions_string = 0;
	wglext_ARB_framebuffer_sRGB = 0;
	wglext_ARB_make_current_read = 0;
	wglext_ARB_multisample = 0;
	wglext_ARB_pbuffer = 0;
	wglext_ARB_pixel_format = 0;
	wglext_ARB_pixel_format_float = 0;
	wglext_ARB_render_texture = 0;
	wglext_ATI_pixel_format_float = 0;
	wglext_EXT_create_context_es2_profile = 0;
	wglext_EXT_depth_float = 0;
	wglext_EXT_framebuffer_sRGB = 0;
	wglext_EXT_make_current_read = 0;
	wglext_EXT_multisample = 0;
	wglext_EXT_pbuffer = 0;
	wglext_EXT_pixel_format = 0;
	wglext_EXT_pixel_format_packed_float = 0;
	wglext_EXT_swap_control = 0;
	wglext_EXT_swap_control_tear = 0;
	wglext_I3D_digital_video_control = 0;
	wglext_I3D_gamma = 0;
	wglext_I3D_genlock = 0;
	wglext_I3D_image_buffer = 0;
	wglext_I3D_swap_frame_lock = 0;
	wglext_NV_DX_interop = 0;
	wglext_NV_DX_interop2 = 0;
	wglext_NV_copy_image = 0;
	wglext_NV_float_buffer = 0;
	wglext_NV_gpu_affinity = 0;
	wglext_NV_multisample_coverage = 0;
	wglext_NV_present_video = 0;
	wglext_NV_render_depth_texture = 0;
	wglext_NV_render_texture_rectangle = 0;
	wglext_NV_swap_group = 0;
	wglext_NV_video_capture = 0;
	wglext_NV_video_output = 0;
}


static void LoadExtByName(const char *extensionName)
{
	wgl_StrToExtMap *entry = NULL;
	entry = FindExtEntry(extensionName);
	if(entry)
	{
		if(entry->LoadExtension)
		{
			int numFailed = entry->LoadExtension();
			if(numFailed == 0)
			{
				*(entry->extensionVariable) = wgl_LOAD_SUCCEEDED;
			}
			else
			{
				*(entry->extensionVariable) = wgl_LOAD_SUCCEEDED + numFailed;
			}
		}
		else
		{
			*(entry->extensionVariable) = wgl_LOAD_SUCCEEDED;
		}
	}
}

static void ProcExtsFromExtString(const char *strExtList)
{
	size_t iExtListLen = strlen(strExtList);
	const char *strExtListEnd = strExtList + iExtListLen;
	const char *strCurrPos = strExtList;
	char strWorkBuff[256];

	while(*strCurrPos)
	{
		/*Get the extension at our position.*/
		int iStrLen = 0;
		const char *strEndStr = strchr(strCurrPos, ' ');
		int iStop = 0;
		if(strEndStr == NULL)
		{
			strEndStr = strExtListEnd;
			iStop = 1;
		}

		iStrLen = (int)((ptrdiff_t)strEndStr - (ptrdiff_t)strCurrPos);

		if(iStrLen > 255)
			return;

		strncpy(strWorkBuff, strCurrPos, iStrLen);
		strWorkBuff[iStrLen] = '\0';

		LoadExtByName(strWorkBuff);

		strCurrPos = strEndStr + 1;
		if(iStop) break;
	}
}

void wgl_CopyFromC();

static int InternalLoad(HDC hdc)
{
	ClearExtensionVars();
	
	_funcptr_wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)IntGetProcAddress("wglGetExtensionsStringARB");
	if(!_funcptr_wglGetExtensionsStringARB) return 0;
	
	ProcExtsFromExtString((const char *)_funcptr_wglGetExtensionsStringARB(hdc));
	
	return wgl_LOAD_SUCCEEDED;
}

int wgl_LoadFunctions(HDC hdc)
{
	int numFailed = 0;
	numFailed = InternalLoad(hdc);
	wgl_CopyFromC();
	return numFailed;
}

#endif // defined( CINDER_GL_ANGLE )