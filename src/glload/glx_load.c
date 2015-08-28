#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "glload/glx_all.h"
#include "glload/glx_load.h"

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

int glXext_3DFX_multisample = 0;
int glXext_AMD_gpu_association = 0;
int glXext_ARB_create_context = 0;
int glXext_ARB_create_context_profile = 0;
int glXext_ARB_create_context_robustness = 0;
int glXext_ARB_fbconfig_float = 0;
int glXext_ARB_framebuffer_sRGB = 0;
int glXext_ARB_get_proc_address = 0;
int glXext_ARB_multisample = 0;
int glXext_ARB_robustness_application_isolation = 0;
int glXext_ARB_robustness_share_group_isolation = 0;
int glXext_ARB_vertex_buffer_object = 0;
int glXext_EXT_buffer_age = 0;
int glXext_EXT_create_context_es2_profile = 0;
int glXext_EXT_create_context_es_profile = 0;
int glXext_EXT_fbconfig_packed_float = 0;
int glXext_EXT_framebuffer_sRGB = 0;
int glXext_EXT_import_context = 0;
int glXext_EXT_swap_control = 0;
int glXext_EXT_swap_control_tear = 0;
int glXext_EXT_texture_from_pixmap = 0;
int glXext_EXT_visual_info = 0;
int glXext_EXT_visual_rating = 0;
int glXext_INTEL_swap_event = 0;
int glXext_MESA_agp_offset = 0;
int glXext_MESA_copy_sub_buffer = 0;
int glXext_MESA_pixmap_colormap = 0;
int glXext_MESA_release_buffers = 0;
int glXext_MESA_set_3dfx_mode = 0;
int glXext_NV_copy_image = 0;
int glXext_NV_float_buffer = 0;
int glXext_NV_multisample_coverage = 0;
int glXext_NV_present_video = 0;
int glXext_NV_swap_group = 0;
int glXext_NV_video_capture = 0;
int glXext_NV_video_output = 0;
int glXext_OML_swap_method = 0;
int glXext_OML_sync_control = 0;
int glXext_SGIS_blended_overlay = 0;
int glXext_SGIS_multisample = 0;
int glXext_SGIS_shared_multisample = 0;
int glXext_SGIX_fbconfig = 0;
int glXext_SGIX_pbuffer = 0;
int glXext_SGIX_swap_barrier = 0;
int glXext_SGIX_swap_group = 0;
int glXext_SGIX_video_resize = 0;
int glXext_SGIX_visual_select_group = 0;
int glXext_SGI_cushion = 0;
int glXext_SGI_make_current_read = 0;
int glXext_SGI_swap_control = 0;
int glXext_SGI_video_sync = 0;
int glXext_SUN_get_transparent_index = 0;

PFNGLXCREATECONTEXTATTRIBSARBPROC _funcptr_glXCreateContextAttribsARB = NULL;

static int LoadExt_ARB_create_context()
{
	int numFailed = 0;
	_funcptr_glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)IntGetProcAddress("glXCreateContextAttribsARB");
	if(!_funcptr_glXCreateContextAttribsARB) ++numFailed;
	return numFailed;
}

PFNGLXGETPROCADDRESSARBPROC _funcptr_glXGetProcAddressARB = NULL;

static int LoadExt_ARB_get_proc_address()
{
	int numFailed = 0;
	_funcptr_glXGetProcAddressARB = (PFNGLXGETPROCADDRESSARBPROC)IntGetProcAddress("glXGetProcAddressARB");
	if(!_funcptr_glXGetProcAddressARB) ++numFailed;
	return numFailed;
}

PFNGLXFREECONTEXTEXTPROC _funcptr_glXFreeContextEXT = NULL;
PFNGLXGETCONTEXTIDEXTPROC _funcptr_glXGetContextIDEXT = NULL;
PFNGLXGETCURRENTDISPLAYEXTPROC _funcptr_glXGetCurrentDisplayEXT = NULL;
PFNGLXIMPORTCONTEXTEXTPROC _funcptr_glXImportContextEXT = NULL;
PFNGLXQUERYCONTEXTINFOEXTPROC _funcptr_glXQueryContextInfoEXT = NULL;

static int LoadExt_EXT_import_context()
{
	int numFailed = 0;
	_funcptr_glXFreeContextEXT = (PFNGLXFREECONTEXTEXTPROC)IntGetProcAddress("glXFreeContextEXT");
	if(!_funcptr_glXFreeContextEXT) ++numFailed;
	_funcptr_glXGetContextIDEXT = (PFNGLXGETCONTEXTIDEXTPROC)IntGetProcAddress("glXGetContextIDEXT");
	if(!_funcptr_glXGetContextIDEXT) ++numFailed;
	_funcptr_glXGetCurrentDisplayEXT = (PFNGLXGETCURRENTDISPLAYEXTPROC)IntGetProcAddress("glXGetCurrentDisplayEXT");
	if(!_funcptr_glXGetCurrentDisplayEXT) ++numFailed;
	_funcptr_glXImportContextEXT = (PFNGLXIMPORTCONTEXTEXTPROC)IntGetProcAddress("glXImportContextEXT");
	if(!_funcptr_glXImportContextEXT) ++numFailed;
	_funcptr_glXQueryContextInfoEXT = (PFNGLXQUERYCONTEXTINFOEXTPROC)IntGetProcAddress("glXQueryContextInfoEXT");
	if(!_funcptr_glXQueryContextInfoEXT) ++numFailed;
	return numFailed;
}

PFNGLXSWAPINTERVALEXTPROC _funcptr_glXSwapIntervalEXT = NULL;

static int LoadExt_EXT_swap_control()
{
	int numFailed = 0;
	_funcptr_glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)IntGetProcAddress("glXSwapIntervalEXT");
	if(!_funcptr_glXSwapIntervalEXT) ++numFailed;
	return numFailed;
}

PFNGLXBINDTEXIMAGEEXTPROC _funcptr_glXBindTexImageEXT = NULL;
PFNGLXRELEASETEXIMAGEEXTPROC _funcptr_glXReleaseTexImageEXT = NULL;

static int LoadExt_EXT_texture_from_pixmap()
{
	int numFailed = 0;
	_funcptr_glXBindTexImageEXT = (PFNGLXBINDTEXIMAGEEXTPROC)IntGetProcAddress("glXBindTexImageEXT");
	if(!_funcptr_glXBindTexImageEXT) ++numFailed;
	_funcptr_glXReleaseTexImageEXT = (PFNGLXRELEASETEXIMAGEEXTPROC)IntGetProcAddress("glXReleaseTexImageEXT");
	if(!_funcptr_glXReleaseTexImageEXT) ++numFailed;
	return numFailed;
}

PFNGLXGETAGPOFFSETMESAPROC _funcptr_glXGetAGPOffsetMESA = NULL;

static int LoadExt_MESA_agp_offset()
{
	int numFailed = 0;
	_funcptr_glXGetAGPOffsetMESA = (PFNGLXGETAGPOFFSETMESAPROC)IntGetProcAddress("glXGetAGPOffsetMESA");
	if(!_funcptr_glXGetAGPOffsetMESA) ++numFailed;
	return numFailed;
}

PFNGLXCOPYSUBBUFFERMESAPROC _funcptr_glXCopySubBufferMESA = NULL;

static int LoadExt_MESA_copy_sub_buffer()
{
	int numFailed = 0;
	_funcptr_glXCopySubBufferMESA = (PFNGLXCOPYSUBBUFFERMESAPROC)IntGetProcAddress("glXCopySubBufferMESA");
	if(!_funcptr_glXCopySubBufferMESA) ++numFailed;
	return numFailed;
}

PFNGLXCREATEGLXPIXMAPMESAPROC _funcptr_glXCreateGLXPixmapMESA = NULL;

static int LoadExt_MESA_pixmap_colormap()
{
	int numFailed = 0;
	_funcptr_glXCreateGLXPixmapMESA = (PFNGLXCREATEGLXPIXMAPMESAPROC)IntGetProcAddress("glXCreateGLXPixmapMESA");
	if(!_funcptr_glXCreateGLXPixmapMESA) ++numFailed;
	return numFailed;
}

PFNGLXRELEASEBUFFERSMESAPROC _funcptr_glXReleaseBuffersMESA = NULL;

static int LoadExt_MESA_release_buffers()
{
	int numFailed = 0;
	_funcptr_glXReleaseBuffersMESA = (PFNGLXRELEASEBUFFERSMESAPROC)IntGetProcAddress("glXReleaseBuffersMESA");
	if(!_funcptr_glXReleaseBuffersMESA) ++numFailed;
	return numFailed;
}

PFNGLXSET3DFXMODEMESAPROC _funcptr_glXSet3DfxModeMESA = NULL;

static int LoadExt_MESA_set_3dfx_mode()
{
	int numFailed = 0;
	_funcptr_glXSet3DfxModeMESA = (PFNGLXSET3DFXMODEMESAPROC)IntGetProcAddress("glXSet3DfxModeMESA");
	if(!_funcptr_glXSet3DfxModeMESA) ++numFailed;
	return numFailed;
}

PFNGLXCOPYIMAGESUBDATANVPROC _funcptr_glXCopyImageSubDataNV = NULL;

static int LoadExt_NV_copy_image()
{
	int numFailed = 0;
	_funcptr_glXCopyImageSubDataNV = (PFNGLXCOPYIMAGESUBDATANVPROC)IntGetProcAddress("glXCopyImageSubDataNV");
	if(!_funcptr_glXCopyImageSubDataNV) ++numFailed;
	return numFailed;
}

PFNGLXBINDVIDEODEVICENVPROC _funcptr_glXBindVideoDeviceNV = NULL;
PFNGLXENUMERATEVIDEODEVICESNVPROC _funcptr_glXEnumerateVideoDevicesNV = NULL;

static int LoadExt_NV_present_video()
{
	int numFailed = 0;
	_funcptr_glXBindVideoDeviceNV = (PFNGLXBINDVIDEODEVICENVPROC)IntGetProcAddress("glXBindVideoDeviceNV");
	if(!_funcptr_glXBindVideoDeviceNV) ++numFailed;
	_funcptr_glXEnumerateVideoDevicesNV = (PFNGLXENUMERATEVIDEODEVICESNVPROC)IntGetProcAddress("glXEnumerateVideoDevicesNV");
	if(!_funcptr_glXEnumerateVideoDevicesNV) ++numFailed;
	return numFailed;
}

PFNGLXBINDSWAPBARRIERNVPROC _funcptr_glXBindSwapBarrierNV = NULL;
PFNGLXJOINSWAPGROUPNVPROC _funcptr_glXJoinSwapGroupNV = NULL;
PFNGLXQUERYFRAMECOUNTNVPROC _funcptr_glXQueryFrameCountNV = NULL;
PFNGLXQUERYMAXSWAPGROUPSNVPROC _funcptr_glXQueryMaxSwapGroupsNV = NULL;
PFNGLXQUERYSWAPGROUPNVPROC _funcptr_glXQuerySwapGroupNV = NULL;
PFNGLXRESETFRAMECOUNTNVPROC _funcptr_glXResetFrameCountNV = NULL;

static int LoadExt_NV_swap_group()
{
	int numFailed = 0;
	_funcptr_glXBindSwapBarrierNV = (PFNGLXBINDSWAPBARRIERNVPROC)IntGetProcAddress("glXBindSwapBarrierNV");
	if(!_funcptr_glXBindSwapBarrierNV) ++numFailed;
	_funcptr_glXJoinSwapGroupNV = (PFNGLXJOINSWAPGROUPNVPROC)IntGetProcAddress("glXJoinSwapGroupNV");
	if(!_funcptr_glXJoinSwapGroupNV) ++numFailed;
	_funcptr_glXQueryFrameCountNV = (PFNGLXQUERYFRAMECOUNTNVPROC)IntGetProcAddress("glXQueryFrameCountNV");
	if(!_funcptr_glXQueryFrameCountNV) ++numFailed;
	_funcptr_glXQueryMaxSwapGroupsNV = (PFNGLXQUERYMAXSWAPGROUPSNVPROC)IntGetProcAddress("glXQueryMaxSwapGroupsNV");
	if(!_funcptr_glXQueryMaxSwapGroupsNV) ++numFailed;
	_funcptr_glXQuerySwapGroupNV = (PFNGLXQUERYSWAPGROUPNVPROC)IntGetProcAddress("glXQuerySwapGroupNV");
	if(!_funcptr_glXQuerySwapGroupNV) ++numFailed;
	_funcptr_glXResetFrameCountNV = (PFNGLXRESETFRAMECOUNTNVPROC)IntGetProcAddress("glXResetFrameCountNV");
	if(!_funcptr_glXResetFrameCountNV) ++numFailed;
	return numFailed;
}

PFNGLXBINDVIDEOCAPTUREDEVICENVPROC _funcptr_glXBindVideoCaptureDeviceNV = NULL;
PFNGLXENUMERATEVIDEOCAPTUREDEVICESNVPROC _funcptr_glXEnumerateVideoCaptureDevicesNV = NULL;
PFNGLXLOCKVIDEOCAPTUREDEVICENVPROC _funcptr_glXLockVideoCaptureDeviceNV = NULL;
PFNGLXQUERYVIDEOCAPTUREDEVICENVPROC _funcptr_glXQueryVideoCaptureDeviceNV = NULL;
PFNGLXRELEASEVIDEOCAPTUREDEVICENVPROC _funcptr_glXReleaseVideoCaptureDeviceNV = NULL;

static int LoadExt_NV_video_capture()
{
	int numFailed = 0;
	_funcptr_glXBindVideoCaptureDeviceNV = (PFNGLXBINDVIDEOCAPTUREDEVICENVPROC)IntGetProcAddress("glXBindVideoCaptureDeviceNV");
	if(!_funcptr_glXBindVideoCaptureDeviceNV) ++numFailed;
	_funcptr_glXEnumerateVideoCaptureDevicesNV = (PFNGLXENUMERATEVIDEOCAPTUREDEVICESNVPROC)IntGetProcAddress("glXEnumerateVideoCaptureDevicesNV");
	if(!_funcptr_glXEnumerateVideoCaptureDevicesNV) ++numFailed;
	_funcptr_glXLockVideoCaptureDeviceNV = (PFNGLXLOCKVIDEOCAPTUREDEVICENVPROC)IntGetProcAddress("glXLockVideoCaptureDeviceNV");
	if(!_funcptr_glXLockVideoCaptureDeviceNV) ++numFailed;
	_funcptr_glXQueryVideoCaptureDeviceNV = (PFNGLXQUERYVIDEOCAPTUREDEVICENVPROC)IntGetProcAddress("glXQueryVideoCaptureDeviceNV");
	if(!_funcptr_glXQueryVideoCaptureDeviceNV) ++numFailed;
	_funcptr_glXReleaseVideoCaptureDeviceNV = (PFNGLXRELEASEVIDEOCAPTUREDEVICENVPROC)IntGetProcAddress("glXReleaseVideoCaptureDeviceNV");
	if(!_funcptr_glXReleaseVideoCaptureDeviceNV) ++numFailed;
	return numFailed;
}

PFNGLXBINDVIDEOIMAGENVPROC _funcptr_glXBindVideoImageNV = NULL;
PFNGLXGETVIDEODEVICENVPROC _funcptr_glXGetVideoDeviceNV = NULL;
PFNGLXGETVIDEOINFONVPROC _funcptr_glXGetVideoInfoNV = NULL;
PFNGLXRELEASEVIDEODEVICENVPROC _funcptr_glXReleaseVideoDeviceNV = NULL;
PFNGLXRELEASEVIDEOIMAGENVPROC _funcptr_glXReleaseVideoImageNV = NULL;
PFNGLXSENDPBUFFERTOVIDEONVPROC _funcptr_glXSendPbufferToVideoNV = NULL;

static int LoadExt_NV_video_output()
{
	int numFailed = 0;
	_funcptr_glXBindVideoImageNV = (PFNGLXBINDVIDEOIMAGENVPROC)IntGetProcAddress("glXBindVideoImageNV");
	if(!_funcptr_glXBindVideoImageNV) ++numFailed;
	_funcptr_glXGetVideoDeviceNV = (PFNGLXGETVIDEODEVICENVPROC)IntGetProcAddress("glXGetVideoDeviceNV");
	if(!_funcptr_glXGetVideoDeviceNV) ++numFailed;
	_funcptr_glXGetVideoInfoNV = (PFNGLXGETVIDEOINFONVPROC)IntGetProcAddress("glXGetVideoInfoNV");
	if(!_funcptr_glXGetVideoInfoNV) ++numFailed;
	_funcptr_glXReleaseVideoDeviceNV = (PFNGLXRELEASEVIDEODEVICENVPROC)IntGetProcAddress("glXReleaseVideoDeviceNV");
	if(!_funcptr_glXReleaseVideoDeviceNV) ++numFailed;
	_funcptr_glXReleaseVideoImageNV = (PFNGLXRELEASEVIDEOIMAGENVPROC)IntGetProcAddress("glXReleaseVideoImageNV");
	if(!_funcptr_glXReleaseVideoImageNV) ++numFailed;
	_funcptr_glXSendPbufferToVideoNV = (PFNGLXSENDPBUFFERTOVIDEONVPROC)IntGetProcAddress("glXSendPbufferToVideoNV");
	if(!_funcptr_glXSendPbufferToVideoNV) ++numFailed;
	return numFailed;
}

PFNGLXGETMSCRATEOMLPROC _funcptr_glXGetMscRateOML = NULL;
PFNGLXGETSYNCVALUESOMLPROC _funcptr_glXGetSyncValuesOML = NULL;
PFNGLXSWAPBUFFERSMSCOMLPROC _funcptr_glXSwapBuffersMscOML = NULL;
PFNGLXWAITFORMSCOMLPROC _funcptr_glXWaitForMscOML = NULL;
PFNGLXWAITFORSBCOMLPROC _funcptr_glXWaitForSbcOML = NULL;

static int LoadExt_OML_sync_control()
{
	int numFailed = 0;
	_funcptr_glXGetMscRateOML = (PFNGLXGETMSCRATEOMLPROC)IntGetProcAddress("glXGetMscRateOML");
	if(!_funcptr_glXGetMscRateOML) ++numFailed;
	_funcptr_glXGetSyncValuesOML = (PFNGLXGETSYNCVALUESOMLPROC)IntGetProcAddress("glXGetSyncValuesOML");
	if(!_funcptr_glXGetSyncValuesOML) ++numFailed;
	_funcptr_glXSwapBuffersMscOML = (PFNGLXSWAPBUFFERSMSCOMLPROC)IntGetProcAddress("glXSwapBuffersMscOML");
	if(!_funcptr_glXSwapBuffersMscOML) ++numFailed;
	_funcptr_glXWaitForMscOML = (PFNGLXWAITFORMSCOMLPROC)IntGetProcAddress("glXWaitForMscOML");
	if(!_funcptr_glXWaitForMscOML) ++numFailed;
	_funcptr_glXWaitForSbcOML = (PFNGLXWAITFORSBCOMLPROC)IntGetProcAddress("glXWaitForSbcOML");
	if(!_funcptr_glXWaitForSbcOML) ++numFailed;
	return numFailed;
}

PFNGLXCHOOSEFBCONFIGSGIXPROC _funcptr_glXChooseFBConfigSGIX = NULL;
PFNGLXCREATECONTEXTWITHCONFIGSGIXPROC _funcptr_glXCreateContextWithConfigSGIX = NULL;
PFNGLXCREATEGLXPIXMAPWITHCONFIGSGIXPROC _funcptr_glXCreateGLXPixmapWithConfigSGIX = NULL;
PFNGLXGETFBCONFIGATTRIBSGIXPROC _funcptr_glXGetFBConfigAttribSGIX = NULL;
PFNGLXGETFBCONFIGFROMVISUALSGIXPROC _funcptr_glXGetFBConfigFromVisualSGIX = NULL;
PFNGLXGETVISUALFROMFBCONFIGSGIXPROC _funcptr_glXGetVisualFromFBConfigSGIX = NULL;

static int LoadExt_SGIX_fbconfig()
{
	int numFailed = 0;
	_funcptr_glXChooseFBConfigSGIX = (PFNGLXCHOOSEFBCONFIGSGIXPROC)IntGetProcAddress("glXChooseFBConfigSGIX");
	if(!_funcptr_glXChooseFBConfigSGIX) ++numFailed;
	_funcptr_glXCreateContextWithConfigSGIX = (PFNGLXCREATECONTEXTWITHCONFIGSGIXPROC)IntGetProcAddress("glXCreateContextWithConfigSGIX");
	if(!_funcptr_glXCreateContextWithConfigSGIX) ++numFailed;
	_funcptr_glXCreateGLXPixmapWithConfigSGIX = (PFNGLXCREATEGLXPIXMAPWITHCONFIGSGIXPROC)IntGetProcAddress("glXCreateGLXPixmapWithConfigSGIX");
	if(!_funcptr_glXCreateGLXPixmapWithConfigSGIX) ++numFailed;
	_funcptr_glXGetFBConfigAttribSGIX = (PFNGLXGETFBCONFIGATTRIBSGIXPROC)IntGetProcAddress("glXGetFBConfigAttribSGIX");
	if(!_funcptr_glXGetFBConfigAttribSGIX) ++numFailed;
	_funcptr_glXGetFBConfigFromVisualSGIX = (PFNGLXGETFBCONFIGFROMVISUALSGIXPROC)IntGetProcAddress("glXGetFBConfigFromVisualSGIX");
	if(!_funcptr_glXGetFBConfigFromVisualSGIX) ++numFailed;
	_funcptr_glXGetVisualFromFBConfigSGIX = (PFNGLXGETVISUALFROMFBCONFIGSGIXPROC)IntGetProcAddress("glXGetVisualFromFBConfigSGIX");
	if(!_funcptr_glXGetVisualFromFBConfigSGIX) ++numFailed;
	return numFailed;
}

PFNGLXCREATEGLXPBUFFERSGIXPROC _funcptr_glXCreateGLXPbufferSGIX = NULL;
PFNGLXDESTROYGLXPBUFFERSGIXPROC _funcptr_glXDestroyGLXPbufferSGIX = NULL;
PFNGLXGETSELECTEDEVENTSGIXPROC _funcptr_glXGetSelectedEventSGIX = NULL;
PFNGLXQUERYGLXPBUFFERSGIXPROC _funcptr_glXQueryGLXPbufferSGIX = NULL;
PFNGLXSELECTEVENTSGIXPROC _funcptr_glXSelectEventSGIX = NULL;

static int LoadExt_SGIX_pbuffer()
{
	int numFailed = 0;
	_funcptr_glXCreateGLXPbufferSGIX = (PFNGLXCREATEGLXPBUFFERSGIXPROC)IntGetProcAddress("glXCreateGLXPbufferSGIX");
	if(!_funcptr_glXCreateGLXPbufferSGIX) ++numFailed;
	_funcptr_glXDestroyGLXPbufferSGIX = (PFNGLXDESTROYGLXPBUFFERSGIXPROC)IntGetProcAddress("glXDestroyGLXPbufferSGIX");
	if(!_funcptr_glXDestroyGLXPbufferSGIX) ++numFailed;
	_funcptr_glXGetSelectedEventSGIX = (PFNGLXGETSELECTEDEVENTSGIXPROC)IntGetProcAddress("glXGetSelectedEventSGIX");
	if(!_funcptr_glXGetSelectedEventSGIX) ++numFailed;
	_funcptr_glXQueryGLXPbufferSGIX = (PFNGLXQUERYGLXPBUFFERSGIXPROC)IntGetProcAddress("glXQueryGLXPbufferSGIX");
	if(!_funcptr_glXQueryGLXPbufferSGIX) ++numFailed;
	_funcptr_glXSelectEventSGIX = (PFNGLXSELECTEVENTSGIXPROC)IntGetProcAddress("glXSelectEventSGIX");
	if(!_funcptr_glXSelectEventSGIX) ++numFailed;
	return numFailed;
}

PFNGLXBINDSWAPBARRIERSGIXPROC _funcptr_glXBindSwapBarrierSGIX = NULL;
PFNGLXQUERYMAXSWAPBARRIERSSGIXPROC _funcptr_glXQueryMaxSwapBarriersSGIX = NULL;

static int LoadExt_SGIX_swap_barrier()
{
	int numFailed = 0;
	_funcptr_glXBindSwapBarrierSGIX = (PFNGLXBINDSWAPBARRIERSGIXPROC)IntGetProcAddress("glXBindSwapBarrierSGIX");
	if(!_funcptr_glXBindSwapBarrierSGIX) ++numFailed;
	_funcptr_glXQueryMaxSwapBarriersSGIX = (PFNGLXQUERYMAXSWAPBARRIERSSGIXPROC)IntGetProcAddress("glXQueryMaxSwapBarriersSGIX");
	if(!_funcptr_glXQueryMaxSwapBarriersSGIX) ++numFailed;
	return numFailed;
}

PFNGLXJOINSWAPGROUPSGIXPROC _funcptr_glXJoinSwapGroupSGIX = NULL;

static int LoadExt_SGIX_swap_group()
{
	int numFailed = 0;
	_funcptr_glXJoinSwapGroupSGIX = (PFNGLXJOINSWAPGROUPSGIXPROC)IntGetProcAddress("glXJoinSwapGroupSGIX");
	if(!_funcptr_glXJoinSwapGroupSGIX) ++numFailed;
	return numFailed;
}

PFNGLXBINDCHANNELTOWINDOWSGIXPROC _funcptr_glXBindChannelToWindowSGIX = NULL;
PFNGLXCHANNELRECTSGIXPROC _funcptr_glXChannelRectSGIX = NULL;
PFNGLXCHANNELRECTSYNCSGIXPROC _funcptr_glXChannelRectSyncSGIX = NULL;
PFNGLXQUERYCHANNELDELTASSGIXPROC _funcptr_glXQueryChannelDeltasSGIX = NULL;
PFNGLXQUERYCHANNELRECTSGIXPROC _funcptr_glXQueryChannelRectSGIX = NULL;

static int LoadExt_SGIX_video_resize()
{
	int numFailed = 0;
	_funcptr_glXBindChannelToWindowSGIX = (PFNGLXBINDCHANNELTOWINDOWSGIXPROC)IntGetProcAddress("glXBindChannelToWindowSGIX");
	if(!_funcptr_glXBindChannelToWindowSGIX) ++numFailed;
	_funcptr_glXChannelRectSGIX = (PFNGLXCHANNELRECTSGIXPROC)IntGetProcAddress("glXChannelRectSGIX");
	if(!_funcptr_glXChannelRectSGIX) ++numFailed;
	_funcptr_glXChannelRectSyncSGIX = (PFNGLXCHANNELRECTSYNCSGIXPROC)IntGetProcAddress("glXChannelRectSyncSGIX");
	if(!_funcptr_glXChannelRectSyncSGIX) ++numFailed;
	_funcptr_glXQueryChannelDeltasSGIX = (PFNGLXQUERYCHANNELDELTASSGIXPROC)IntGetProcAddress("glXQueryChannelDeltasSGIX");
	if(!_funcptr_glXQueryChannelDeltasSGIX) ++numFailed;
	_funcptr_glXQueryChannelRectSGIX = (PFNGLXQUERYCHANNELRECTSGIXPROC)IntGetProcAddress("glXQueryChannelRectSGIX");
	if(!_funcptr_glXQueryChannelRectSGIX) ++numFailed;
	return numFailed;
}

PFNGLXCUSHIONSGIPROC _funcptr_glXCushionSGI = NULL;

static int LoadExt_SGI_cushion()
{
	int numFailed = 0;
	_funcptr_glXCushionSGI = (PFNGLXCUSHIONSGIPROC)IntGetProcAddress("glXCushionSGI");
	if(!_funcptr_glXCushionSGI) ++numFailed;
	return numFailed;
}

PFNGLXGETCURRENTREADDRAWABLESGIPROC _funcptr_glXGetCurrentReadDrawableSGI = NULL;
PFNGLXMAKECURRENTREADSGIPROC _funcptr_glXMakeCurrentReadSGI = NULL;

static int LoadExt_SGI_make_current_read()
{
	int numFailed = 0;
	_funcptr_glXGetCurrentReadDrawableSGI = (PFNGLXGETCURRENTREADDRAWABLESGIPROC)IntGetProcAddress("glXGetCurrentReadDrawableSGI");
	if(!_funcptr_glXGetCurrentReadDrawableSGI) ++numFailed;
	_funcptr_glXMakeCurrentReadSGI = (PFNGLXMAKECURRENTREADSGIPROC)IntGetProcAddress("glXMakeCurrentReadSGI");
	if(!_funcptr_glXMakeCurrentReadSGI) ++numFailed;
	return numFailed;
}

PFNGLXSWAPINTERVALSGIPROC _funcptr_glXSwapIntervalSGI = NULL;

static int LoadExt_SGI_swap_control()
{
	int numFailed = 0;
	_funcptr_glXSwapIntervalSGI = (PFNGLXSWAPINTERVALSGIPROC)IntGetProcAddress("glXSwapIntervalSGI");
	if(!_funcptr_glXSwapIntervalSGI) ++numFailed;
	return numFailed;
}

PFNGLXGETVIDEOSYNCSGIPROC _funcptr_glXGetVideoSyncSGI = NULL;
PFNGLXWAITVIDEOSYNCSGIPROC _funcptr_glXWaitVideoSyncSGI = NULL;

static int LoadExt_SGI_video_sync()
{
	int numFailed = 0;
	_funcptr_glXGetVideoSyncSGI = (PFNGLXGETVIDEOSYNCSGIPROC)IntGetProcAddress("glXGetVideoSyncSGI");
	if(!_funcptr_glXGetVideoSyncSGI) ++numFailed;
	_funcptr_glXWaitVideoSyncSGI = (PFNGLXWAITVIDEOSYNCSGIPROC)IntGetProcAddress("glXWaitVideoSyncSGI");
	if(!_funcptr_glXWaitVideoSyncSGI) ++numFailed;
	return numFailed;
}

PFNGLXGETTRANSPARENTINDEXSUNPROC _funcptr_glXGetTransparentIndexSUN = NULL;

static int LoadExt_SUN_get_transparent_index()
{
	int numFailed = 0;
	_funcptr_glXGetTransparentIndexSUN = (PFNGLXGETTRANSPARENTINDEXSUNPROC)IntGetProcAddress("glXGetTransparentIndexSUN");
	if(!_funcptr_glXGetTransparentIndexSUN) ++numFailed;
	return numFailed;
}


typedef int (*PFN_LOADFUNCPOINTERS)();
typedef struct glx_StrToExtMap_s
{
	char *extensionName;
	int *extensionVariable;
	PFN_LOADFUNCPOINTERS LoadExtension;
} glx_StrToExtMap;

static glx_StrToExtMap ExtensionTable[52] = {
	{"GLX_3DFX_multisample", &glXext_3DFX_multisample, NULL},
	{"GLX_AMD_gpu_association", &glXext_AMD_gpu_association, NULL},
	{"GLX_ARB_create_context", &glXext_ARB_create_context, LoadExt_ARB_create_context},
	{"GLX_ARB_create_context_profile", &glXext_ARB_create_context_profile, NULL},
	{"GLX_ARB_create_context_robustness", &glXext_ARB_create_context_robustness, NULL},
	{"GLX_ARB_fbconfig_float", &glXext_ARB_fbconfig_float, NULL},
	{"GLX_ARB_framebuffer_sRGB", &glXext_ARB_framebuffer_sRGB, NULL},
	{"GLX_ARB_get_proc_address", &glXext_ARB_get_proc_address, LoadExt_ARB_get_proc_address},
	{"GLX_ARB_multisample", &glXext_ARB_multisample, NULL},
	{"GLX_ARB_robustness_application_isolation", &glXext_ARB_robustness_application_isolation, NULL},
	{"GLX_ARB_robustness_share_group_isolation", &glXext_ARB_robustness_share_group_isolation, NULL},
	{"GLX_ARB_vertex_buffer_object", &glXext_ARB_vertex_buffer_object, NULL},
	{"GLX_EXT_buffer_age", &glXext_EXT_buffer_age, NULL},
	{"GLX_EXT_create_context_es2_profile", &glXext_EXT_create_context_es2_profile, NULL},
	{"GLX_EXT_create_context_es_profile", &glXext_EXT_create_context_es_profile, NULL},
	{"GLX_EXT_fbconfig_packed_float", &glXext_EXT_fbconfig_packed_float, NULL},
	{"GLX_EXT_framebuffer_sRGB", &glXext_EXT_framebuffer_sRGB, NULL},
	{"GLX_EXT_import_context", &glXext_EXT_import_context, LoadExt_EXT_import_context},
	{"GLX_EXT_swap_control", &glXext_EXT_swap_control, LoadExt_EXT_swap_control},
	{"GLX_EXT_swap_control_tear", &glXext_EXT_swap_control_tear, NULL},
	{"GLX_EXT_texture_from_pixmap", &glXext_EXT_texture_from_pixmap, LoadExt_EXT_texture_from_pixmap},
	{"GLX_EXT_visual_info", &glXext_EXT_visual_info, NULL},
	{"GLX_EXT_visual_rating", &glXext_EXT_visual_rating, NULL},
	{"GLX_INTEL_swap_event", &glXext_INTEL_swap_event, NULL},
	{"GLX_MESA_agp_offset", &glXext_MESA_agp_offset, LoadExt_MESA_agp_offset},
	{"GLX_MESA_copy_sub_buffer", &glXext_MESA_copy_sub_buffer, LoadExt_MESA_copy_sub_buffer},
	{"GLX_MESA_pixmap_colormap", &glXext_MESA_pixmap_colormap, LoadExt_MESA_pixmap_colormap},
	{"GLX_MESA_release_buffers", &glXext_MESA_release_buffers, LoadExt_MESA_release_buffers},
	{"GLX_MESA_set_3dfx_mode", &glXext_MESA_set_3dfx_mode, LoadExt_MESA_set_3dfx_mode},
	{"GLX_NV_copy_image", &glXext_NV_copy_image, LoadExt_NV_copy_image},
	{"GLX_NV_float_buffer", &glXext_NV_float_buffer, NULL},
	{"GLX_NV_multisample_coverage", &glXext_NV_multisample_coverage, NULL},
	{"GLX_NV_present_video", &glXext_NV_present_video, LoadExt_NV_present_video},
	{"GLX_NV_swap_group", &glXext_NV_swap_group, LoadExt_NV_swap_group},
	{"GLX_NV_video_capture", &glXext_NV_video_capture, LoadExt_NV_video_capture},
	{"GLX_NV_video_output", &glXext_NV_video_output, LoadExt_NV_video_output},
	{"GLX_OML_swap_method", &glXext_OML_swap_method, NULL},
	{"GLX_OML_sync_control", &glXext_OML_sync_control, LoadExt_OML_sync_control},
	{"GLX_SGIS_blended_overlay", &glXext_SGIS_blended_overlay, NULL},
	{"GLX_SGIS_multisample", &glXext_SGIS_multisample, NULL},
	{"GLX_SGIS_shared_multisample", &glXext_SGIS_shared_multisample, NULL},
	{"GLX_SGIX_fbconfig", &glXext_SGIX_fbconfig, LoadExt_SGIX_fbconfig},
	{"GLX_SGIX_pbuffer", &glXext_SGIX_pbuffer, LoadExt_SGIX_pbuffer},
	{"GLX_SGIX_swap_barrier", &glXext_SGIX_swap_barrier, LoadExt_SGIX_swap_barrier},
	{"GLX_SGIX_swap_group", &glXext_SGIX_swap_group, LoadExt_SGIX_swap_group},
	{"GLX_SGIX_video_resize", &glXext_SGIX_video_resize, LoadExt_SGIX_video_resize},
	{"GLX_SGIX_visual_select_group", &glXext_SGIX_visual_select_group, NULL},
	{"GLX_SGI_cushion", &glXext_SGI_cushion, LoadExt_SGI_cushion},
	{"GLX_SGI_make_current_read", &glXext_SGI_make_current_read, LoadExt_SGI_make_current_read},
	{"GLX_SGI_swap_control", &glXext_SGI_swap_control, LoadExt_SGI_swap_control},
	{"GLX_SGI_video_sync", &glXext_SGI_video_sync, LoadExt_SGI_video_sync},
	{"GLX_SUN_get_transparent_index", &glXext_SUN_get_transparent_index, LoadExt_SUN_get_transparent_index},
};

static int g_extensionMapSize = 52;

static glx_StrToExtMap *FindExtEntry(const char *extensionName)
{
	int loop;
	glx_StrToExtMap *currLoc = ExtensionTable;
	for(loop = 0; loop < g_extensionMapSize; ++loop, ++currLoc)
	{
		if(strcmp(extensionName, currLoc->extensionName) == 0)
			return currLoc;
	}
	
	return NULL;
}

static void ClearExtensionVars()
{
	glXext_3DFX_multisample = 0;
	glXext_AMD_gpu_association = 0;
	glXext_ARB_create_context = 0;
	glXext_ARB_create_context_profile = 0;
	glXext_ARB_create_context_robustness = 0;
	glXext_ARB_fbconfig_float = 0;
	glXext_ARB_framebuffer_sRGB = 0;
	glXext_ARB_get_proc_address = 0;
	glXext_ARB_multisample = 0;
	glXext_ARB_robustness_application_isolation = 0;
	glXext_ARB_robustness_share_group_isolation = 0;
	glXext_ARB_vertex_buffer_object = 0;
	glXext_EXT_buffer_age = 0;
	glXext_EXT_create_context_es2_profile = 0;
	glXext_EXT_create_context_es_profile = 0;
	glXext_EXT_fbconfig_packed_float = 0;
	glXext_EXT_framebuffer_sRGB = 0;
	glXext_EXT_import_context = 0;
	glXext_EXT_swap_control = 0;
	glXext_EXT_swap_control_tear = 0;
	glXext_EXT_texture_from_pixmap = 0;
	glXext_EXT_visual_info = 0;
	glXext_EXT_visual_rating = 0;
	glXext_INTEL_swap_event = 0;
	glXext_MESA_agp_offset = 0;
	glXext_MESA_copy_sub_buffer = 0;
	glXext_MESA_pixmap_colormap = 0;
	glXext_MESA_release_buffers = 0;
	glXext_MESA_set_3dfx_mode = 0;
	glXext_NV_copy_image = 0;
	glXext_NV_float_buffer = 0;
	glXext_NV_multisample_coverage = 0;
	glXext_NV_present_video = 0;
	glXext_NV_swap_group = 0;
	glXext_NV_video_capture = 0;
	glXext_NV_video_output = 0;
	glXext_OML_swap_method = 0;
	glXext_OML_sync_control = 0;
	glXext_SGIS_blended_overlay = 0;
	glXext_SGIS_multisample = 0;
	glXext_SGIS_shared_multisample = 0;
	glXext_SGIX_fbconfig = 0;
	glXext_SGIX_pbuffer = 0;
	glXext_SGIX_swap_barrier = 0;
	glXext_SGIX_swap_group = 0;
	glXext_SGIX_video_resize = 0;
	glXext_SGIX_visual_select_group = 0;
	glXext_SGI_cushion = 0;
	glXext_SGI_make_current_read = 0;
	glXext_SGI_swap_control = 0;
	glXext_SGI_video_sync = 0;
	glXext_SUN_get_transparent_index = 0;
}


static void LoadExtByName(const char *extensionName)
{
	glx_StrToExtMap *entry = NULL;
	entry = FindExtEntry(extensionName);
	if(entry)
	{
		if(entry->LoadExtension)
		{
			int numFailed = entry->LoadExtension();
			if(numFailed == 0)
			{
				*(entry->extensionVariable) = glx_LOAD_SUCCEEDED;
			}
			else
			{
				*(entry->extensionVariable) = glx_LOAD_SUCCEEDED + numFailed;
			}
		}
		else
		{
			*(entry->extensionVariable) = glx_LOAD_SUCCEEDED;
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

void glx_CopyFromC();

static int InternalLoad(Display *display, int screen)
{
	ClearExtensionVars();
	
	
	ProcExtsFromExtString((const char *)glXQueryExtensionsString(display, screen));
	
	return glx_LOAD_SUCCEEDED;
}

int glx_LoadFunctions(Display *display, int screen)
{
	int numFailed = 0;
	numFailed = InternalLoad(display, screen);
	glx_CopyFromC();
	return numFailed;
}

