#ifndef WINDOWSGL_GEN_EXTENSIONS_H
#define WINDOWSGL_GEN_EXTENSIONS_H


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
extern int wglext_3DFX_multisample;
extern int wglext_3DL_stereo_control;
extern int wglext_AMD_gpu_association;
extern int wglext_ARB_buffer_region;
extern int wglext_ARB_create_context;
extern int wglext_ARB_create_context_profile;
extern int wglext_ARB_create_context_robustness;
extern int wglext_ARB_extensions_string;
extern int wglext_ARB_framebuffer_sRGB;
extern int wglext_ARB_make_current_read;
extern int wglext_ARB_multisample;
extern int wglext_ARB_pbuffer;
extern int wglext_ARB_pixel_format;
extern int wglext_ARB_pixel_format_float;
extern int wglext_ARB_render_texture;
extern int wglext_ARB_robustness_application_isolation;
extern int wglext_ARB_robustness_share_group_isolation;
extern int wglext_ATI_pixel_format_float;
extern int wglext_EXT_create_context_es2_profile;
extern int wglext_EXT_create_context_es_profile;
extern int wglext_EXT_depth_float;
extern int wglext_EXT_display_color_table;
extern int wglext_EXT_extensions_string;
extern int wglext_EXT_framebuffer_sRGB;
extern int wglext_EXT_make_current_read;
extern int wglext_EXT_multisample;
extern int wglext_EXT_pbuffer;
extern int wglext_EXT_pixel_format;
extern int wglext_EXT_pixel_format_packed_float;
extern int wglext_EXT_swap_control;
extern int wglext_EXT_swap_control_tear;
extern int wglext_I3D_digital_video_control;
extern int wglext_I3D_gamma;
extern int wglext_I3D_genlock;
extern int wglext_I3D_image_buffer;
extern int wglext_I3D_swap_frame_lock;
extern int wglext_I3D_swap_frame_usage;
extern int wglext_NV_DX_interop;
extern int wglext_NV_DX_interop2;
extern int wglext_NV_copy_image;
extern int wglext_NV_float_buffer;
extern int wglext_NV_gpu_affinity;
extern int wglext_NV_multisample_coverage;
extern int wglext_NV_present_video;
extern int wglext_NV_render_depth_texture;
extern int wglext_NV_render_texture_rectangle;
extern int wglext_NV_swap_group;
extern int wglext_NV_vertex_array_range;
extern int wglext_NV_video_capture;
extern int wglext_NV_video_output;
extern int wglext_OML_sync_control;

#define WGL_SAMPLES_3DFX 0x2061
#define WGL_SAMPLE_BUFFERS_3DFX 0x2060

#define WGL_STEREO_EMITTER_DISABLE_3DL 0x2056
#define WGL_STEREO_EMITTER_ENABLE_3DL 0x2055
#define WGL_STEREO_POLARITY_INVERT_3DL 0x2058
#define WGL_STEREO_POLARITY_NORMAL_3DL 0x2057

typedef BOOL (CODEGEN_FUNCPTR * PFNWGLSETSTEREOEMITTERSTATE3DLPROC)(HDC hDC, UINT uState);

extern PFNWGLSETSTEREOEMITTERSTATE3DLPROC _funcptr_wglSetStereoEmitterState3DL;
#define wglSetStereoEmitterState3DL _funcptr_wglSetStereoEmitterState3DL

#define WGL_GPU_CLOCK_AMD 0x21A4
#define WGL_GPU_FASTEST_TARGET_GPUS_AMD 0x21A2
#define WGL_GPU_NUM_PIPES_AMD 0x21A5
#define WGL_GPU_NUM_RB_AMD 0x21A7
#define WGL_GPU_NUM_SIMD_AMD 0x21A6
#define WGL_GPU_NUM_SPI_AMD 0x21A8
#define WGL_GPU_OPENGL_VERSION_STRING_AMD 0x1F02
#define WGL_GPU_RAM_AMD 0x21A3
#define WGL_GPU_RENDERER_STRING_AMD 0x1F01
#define WGL_GPU_VENDOR_AMD 0x1F00

typedef VOID (CODEGEN_FUNCPTR * PFNWGLBLITCONTEXTFRAMEBUFFERAMDPROC)(HGLRC dstCtx, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
typedef HGLRC (CODEGEN_FUNCPTR * PFNWGLCREATEASSOCIATEDCONTEXTAMDPROC)(UINT id);
typedef HGLRC (CODEGEN_FUNCPTR * PFNWGLCREATEASSOCIATEDCONTEXTATTRIBSAMDPROC)(UINT id, HGLRC hShareContext, const int * attribList);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLDELETEASSOCIATEDCONTEXTAMDPROC)(HGLRC hglrc);
typedef UINT (CODEGEN_FUNCPTR * PFNWGLGETCONTEXTGPUIDAMDPROC)(HGLRC hglrc);
typedef HGLRC (CODEGEN_FUNCPTR * PFNWGLGETCURRENTASSOCIATEDCONTEXTAMDPROC)();
typedef UINT (CODEGEN_FUNCPTR * PFNWGLGETGPUIDSAMDPROC)(UINT maxCount, UINT * ids);
typedef INT (CODEGEN_FUNCPTR * PFNWGLGETGPUINFOAMDPROC)(UINT id, int property, GLenum dataType, UINT size, void * data);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLMAKEASSOCIATEDCONTEXTCURRENTAMDPROC)(HGLRC hglrc);

extern PFNWGLBLITCONTEXTFRAMEBUFFERAMDPROC _funcptr_wglBlitContextFramebufferAMD;
#define wglBlitContextFramebufferAMD _funcptr_wglBlitContextFramebufferAMD
extern PFNWGLCREATEASSOCIATEDCONTEXTAMDPROC _funcptr_wglCreateAssociatedContextAMD;
#define wglCreateAssociatedContextAMD _funcptr_wglCreateAssociatedContextAMD
extern PFNWGLCREATEASSOCIATEDCONTEXTATTRIBSAMDPROC _funcptr_wglCreateAssociatedContextAttribsAMD;
#define wglCreateAssociatedContextAttribsAMD _funcptr_wglCreateAssociatedContextAttribsAMD
extern PFNWGLDELETEASSOCIATEDCONTEXTAMDPROC _funcptr_wglDeleteAssociatedContextAMD;
#define wglDeleteAssociatedContextAMD _funcptr_wglDeleteAssociatedContextAMD
extern PFNWGLGETCONTEXTGPUIDAMDPROC _funcptr_wglGetContextGPUIDAMD;
#define wglGetContextGPUIDAMD _funcptr_wglGetContextGPUIDAMD
extern PFNWGLGETCURRENTASSOCIATEDCONTEXTAMDPROC _funcptr_wglGetCurrentAssociatedContextAMD;
#define wglGetCurrentAssociatedContextAMD _funcptr_wglGetCurrentAssociatedContextAMD
extern PFNWGLGETGPUIDSAMDPROC _funcptr_wglGetGPUIDsAMD;
#define wglGetGPUIDsAMD _funcptr_wglGetGPUIDsAMD
extern PFNWGLGETGPUINFOAMDPROC _funcptr_wglGetGPUInfoAMD;
#define wglGetGPUInfoAMD _funcptr_wglGetGPUInfoAMD
extern PFNWGLMAKEASSOCIATEDCONTEXTCURRENTAMDPROC _funcptr_wglMakeAssociatedContextCurrentAMD;
#define wglMakeAssociatedContextCurrentAMD _funcptr_wglMakeAssociatedContextCurrentAMD

#define WGL_BACK_COLOR_BUFFER_BIT_ARB 0x00000002
#define WGL_DEPTH_BUFFER_BIT_ARB 0x00000004
#define WGL_FRONT_COLOR_BUFFER_BIT_ARB 0x00000001
#define WGL_STENCIL_BUFFER_BIT_ARB 0x00000008

typedef HANDLE (CODEGEN_FUNCPTR * PFNWGLCREATEBUFFERREGIONARBPROC)(HDC hDC, int iLayerPlane, UINT uType);
typedef VOID (CODEGEN_FUNCPTR * PFNWGLDELETEBUFFERREGIONARBPROC)(HANDLE hRegion);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLRESTOREBUFFERREGIONARBPROC)(HANDLE hRegion, int x, int y, int width, int height, int xSrc, int ySrc);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLSAVEBUFFERREGIONARBPROC)(HANDLE hRegion, int x, int y, int width, int height);

extern PFNWGLCREATEBUFFERREGIONARBPROC _funcptr_wglCreateBufferRegionARB;
#define wglCreateBufferRegionARB _funcptr_wglCreateBufferRegionARB
extern PFNWGLDELETEBUFFERREGIONARBPROC _funcptr_wglDeleteBufferRegionARB;
#define wglDeleteBufferRegionARB _funcptr_wglDeleteBufferRegionARB
extern PFNWGLRESTOREBUFFERREGIONARBPROC _funcptr_wglRestoreBufferRegionARB;
#define wglRestoreBufferRegionARB _funcptr_wglRestoreBufferRegionARB
extern PFNWGLSAVEBUFFERREGIONARBPROC _funcptr_wglSaveBufferRegionARB;
#define wglSaveBufferRegionARB _funcptr_wglSaveBufferRegionARB

#define WGL_CONTEXT_DEBUG_BIT_ARB 0x00000001
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define WGL_CONTEXT_LAYER_PLANE_ARB 0x2093
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_ERROR_INVALID_VERSION_ARB 0x2095

typedef HGLRC (CODEGEN_FUNCPTR * PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hShareContext, const int * attribList);

extern PFNWGLCREATECONTEXTATTRIBSARBPROC _funcptr_wglCreateContextAttribsARB;
#define wglCreateContextAttribsARB _funcptr_wglCreateContextAttribsARB

#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_ERROR_INVALID_PROFILE_ARB 0x2096

#define WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
#define WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB 0x00000004
#define WGL_LOSE_CONTEXT_ON_RESET_ARB 0x8252
#define WGL_NO_RESET_NOTIFICATION_ARB 0x8261

typedef const char * (CODEGEN_FUNCPTR * PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC hdc);

extern PFNWGLGETEXTENSIONSSTRINGARBPROC _funcptr_wglGetExtensionsStringARB;
#define wglGetExtensionsStringARB _funcptr_wglGetExtensionsStringARB

#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB 0x20A9

#define WGL_ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB 0x2054
#define WGL_ERROR_INVALID_PIXEL_TYPE_ARB 0x2043

typedef HDC (CODEGEN_FUNCPTR * PFNWGLGETCURRENTREADDCARBPROC)();
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLMAKECONTEXTCURRENTARBPROC)(HDC hDrawDC, HDC hReadDC, HGLRC hglrc);

extern PFNWGLGETCURRENTREADDCARBPROC _funcptr_wglGetCurrentReadDCARB;
#define wglGetCurrentReadDCARB _funcptr_wglGetCurrentReadDCARB
extern PFNWGLMAKECONTEXTCURRENTARBPROC _funcptr_wglMakeContextCurrentARB;
#define wglMakeContextCurrentARB _funcptr_wglMakeContextCurrentARB

#define WGL_SAMPLES_ARB 0x2042
#define WGL_SAMPLE_BUFFERS_ARB 0x2041

#define WGL_DRAW_TO_PBUFFER_ARB 0x202D
#define WGL_MAX_PBUFFER_HEIGHT_ARB 0x2030
#define WGL_MAX_PBUFFER_PIXELS_ARB 0x202E
#define WGL_MAX_PBUFFER_WIDTH_ARB 0x202F
#define WGL_PBUFFER_HEIGHT_ARB 0x2035
#define WGL_PBUFFER_LARGEST_ARB 0x2033
#define WGL_PBUFFER_LOST_ARB 0x2036
#define WGL_PBUFFER_WIDTH_ARB 0x2034

typedef HPBUFFERARB (CODEGEN_FUNCPTR * PFNWGLCREATEPBUFFERARBPROC)(HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int * piAttribList);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLDESTROYPBUFFERARBPROC)(HPBUFFERARB hPbuffer);
typedef HDC (CODEGEN_FUNCPTR * PFNWGLGETPBUFFERDCARBPROC)(HPBUFFERARB hPbuffer);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLQUERYPBUFFERARBPROC)(HPBUFFERARB hPbuffer, int iAttribute, int * piValue);
typedef int (CODEGEN_FUNCPTR * PFNWGLRELEASEPBUFFERDCARBPROC)(HPBUFFERARB hPbuffer, HDC hDC);

extern PFNWGLCREATEPBUFFERARBPROC _funcptr_wglCreatePbufferARB;
#define wglCreatePbufferARB _funcptr_wglCreatePbufferARB
extern PFNWGLDESTROYPBUFFERARBPROC _funcptr_wglDestroyPbufferARB;
#define wglDestroyPbufferARB _funcptr_wglDestroyPbufferARB
extern PFNWGLGETPBUFFERDCARBPROC _funcptr_wglGetPbufferDCARB;
#define wglGetPbufferDCARB _funcptr_wglGetPbufferDCARB
extern PFNWGLQUERYPBUFFERARBPROC _funcptr_wglQueryPbufferARB;
#define wglQueryPbufferARB _funcptr_wglQueryPbufferARB
extern PFNWGLRELEASEPBUFFERDCARBPROC _funcptr_wglReleasePbufferDCARB;
#define wglReleasePbufferDCARB _funcptr_wglReleasePbufferDCARB

#define WGL_ACCELERATION_ARB 0x2003
#define WGL_ACCUM_ALPHA_BITS_ARB 0x2021
#define WGL_ACCUM_BITS_ARB 0x201D
#define WGL_ACCUM_BLUE_BITS_ARB 0x2020
#define WGL_ACCUM_GREEN_BITS_ARB 0x201F
#define WGL_ACCUM_RED_BITS_ARB 0x201E
#define WGL_ALPHA_BITS_ARB 0x201B
#define WGL_ALPHA_SHIFT_ARB 0x201C
#define WGL_AUX_BUFFERS_ARB 0x2024
#define WGL_BLUE_BITS_ARB 0x2019
#define WGL_BLUE_SHIFT_ARB 0x201A
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_DRAW_TO_BITMAP_ARB 0x2002
#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_FULL_ACCELERATION_ARB 0x2027
#define WGL_GENERIC_ACCELERATION_ARB 0x2026
#define WGL_GREEN_BITS_ARB 0x2017
#define WGL_GREEN_SHIFT_ARB 0x2018
#define WGL_NEED_PALETTE_ARB 0x2004
#define WGL_NEED_SYSTEM_PALETTE_ARB 0x2005
#define WGL_NO_ACCELERATION_ARB 0x2025
#define WGL_NUMBER_OVERLAYS_ARB 0x2008
#define WGL_NUMBER_PIXEL_FORMATS_ARB 0x2000
#define WGL_NUMBER_UNDERLAYS_ARB 0x2009
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_RED_BITS_ARB 0x2015
#define WGL_RED_SHIFT_ARB 0x2016
#define WGL_SHARE_ACCUM_ARB 0x200E
#define WGL_SHARE_DEPTH_ARB 0x200C
#define WGL_SHARE_STENCIL_ARB 0x200D
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_STEREO_ARB 0x2012
#define WGL_SUPPORT_GDI_ARB 0x200F
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_SWAP_COPY_ARB 0x2029
#define WGL_SWAP_EXCHANGE_ARB 0x2028
#define WGL_SWAP_LAYER_BUFFERS_ARB 0x2006
#define WGL_SWAP_METHOD_ARB 0x2007
#define WGL_SWAP_UNDEFINED_ARB 0x202A
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB 0x203A
#define WGL_TRANSPARENT_ARB 0x200A
#define WGL_TRANSPARENT_BLUE_VALUE_ARB 0x2039
#define WGL_TRANSPARENT_GREEN_VALUE_ARB 0x2038
#define WGL_TRANSPARENT_INDEX_VALUE_ARB 0x203B
#define WGL_TRANSPARENT_RED_VALUE_ARB 0x2037
#define WGL_TYPE_COLORINDEX_ARB 0x202C
#define WGL_TYPE_RGBA_ARB 0x202B

typedef BOOL (CODEGEN_FUNCPTR * PFNWGLCHOOSEPIXELFORMATARBPROC)(HDC hdc, const int * piAttribIList, const FLOAT * pfAttribFList, UINT nMaxFormats, int * piFormats, UINT * nNumFormats);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLGETPIXELFORMATATTRIBFVARBPROC)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int * piAttributes, FLOAT * pfValues);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLGETPIXELFORMATATTRIBIVARBPROC)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int * piAttributes, int * piValues);

extern PFNWGLCHOOSEPIXELFORMATARBPROC _funcptr_wglChoosePixelFormatARB;
#define wglChoosePixelFormatARB _funcptr_wglChoosePixelFormatARB
extern PFNWGLGETPIXELFORMATATTRIBFVARBPROC _funcptr_wglGetPixelFormatAttribfvARB;
#define wglGetPixelFormatAttribfvARB _funcptr_wglGetPixelFormatAttribfvARB
extern PFNWGLGETPIXELFORMATATTRIBIVARBPROC _funcptr_wglGetPixelFormatAttribivARB;
#define wglGetPixelFormatAttribivARB _funcptr_wglGetPixelFormatAttribivARB

#define WGL_TYPE_RGBA_FLOAT_ARB 0x21A0

#define WGL_AUX0_ARB 0x2087
#define WGL_AUX1_ARB 0x2088
#define WGL_AUX2_ARB 0x2089
#define WGL_AUX3_ARB 0x208A
#define WGL_AUX4_ARB 0x208B
#define WGL_AUX5_ARB 0x208C
#define WGL_AUX6_ARB 0x208D
#define WGL_AUX7_ARB 0x208E
#define WGL_AUX8_ARB 0x208F
#define WGL_AUX9_ARB 0x2090
#define WGL_BACK_LEFT_ARB 0x2085
#define WGL_BACK_RIGHT_ARB 0x2086
#define WGL_BIND_TO_TEXTURE_RGBA_ARB 0x2071
#define WGL_BIND_TO_TEXTURE_RGB_ARB 0x2070
#define WGL_CUBE_MAP_FACE_ARB 0x207C
#define WGL_FRONT_LEFT_ARB 0x2083
#define WGL_FRONT_RIGHT_ARB 0x2084
#define WGL_MIPMAP_LEVEL_ARB 0x207B
#define WGL_MIPMAP_TEXTURE_ARB 0x2074
#define WGL_NO_TEXTURE_ARB 0x2077
#define WGL_TEXTURE_1D_ARB 0x2079
#define WGL_TEXTURE_2D_ARB 0x207A
#define WGL_TEXTURE_CUBE_MAP_ARB 0x2078
#define WGL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB 0x207E
#define WGL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB 0x2080
#define WGL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB 0x2082
#define WGL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB 0x207D
#define WGL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB 0x207F
#define WGL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB 0x2081
#define WGL_TEXTURE_FORMAT_ARB 0x2072
#define WGL_TEXTURE_RGBA_ARB 0x2076
#define WGL_TEXTURE_RGB_ARB 0x2075
#define WGL_TEXTURE_TARGET_ARB 0x2073

typedef BOOL (CODEGEN_FUNCPTR * PFNWGLBINDTEXIMAGEARBPROC)(HPBUFFERARB hPbuffer, int iBuffer);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLRELEASETEXIMAGEARBPROC)(HPBUFFERARB hPbuffer, int iBuffer);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLSETPBUFFERATTRIBARBPROC)(HPBUFFERARB hPbuffer, const int * piAttribList);

extern PFNWGLBINDTEXIMAGEARBPROC _funcptr_wglBindTexImageARB;
#define wglBindTexImageARB _funcptr_wglBindTexImageARB
extern PFNWGLRELEASETEXIMAGEARBPROC _funcptr_wglReleaseTexImageARB;
#define wglReleaseTexImageARB _funcptr_wglReleaseTexImageARB
extern PFNWGLSETPBUFFERATTRIBARBPROC _funcptr_wglSetPbufferAttribARB;
#define wglSetPbufferAttribARB _funcptr_wglSetPbufferAttribARB

#define WGL_CONTEXT_RESET_ISOLATION_BIT_ARB 0x00000008

#define WGL_CONTEXT_RESET_ISOLATION_BIT_ARB 0x00000008

#define WGL_TYPE_RGBA_FLOAT_ATI 0x21A0

#define WGL_CONTEXT_ES2_PROFILE_BIT_EXT 0x00000004

#define WGL_CONTEXT_ES_PROFILE_BIT_EXT 0x00000004

#define WGL_DEPTH_FLOAT_EXT 0x2040

typedef GLboolean (CODEGEN_FUNCPTR * PFNWGLBINDDISPLAYCOLORTABLEEXTPROC)(GLushort id);
typedef GLboolean (CODEGEN_FUNCPTR * PFNWGLCREATEDISPLAYCOLORTABLEEXTPROC)(GLushort id);
typedef VOID (CODEGEN_FUNCPTR * PFNWGLDESTROYDISPLAYCOLORTABLEEXTPROC)(GLushort id);
typedef GLboolean (CODEGEN_FUNCPTR * PFNWGLLOADDISPLAYCOLORTABLEEXTPROC)(const GLushort * table, GLuint length);

extern PFNWGLBINDDISPLAYCOLORTABLEEXTPROC _funcptr_wglBindDisplayColorTableEXT;
#define wglBindDisplayColorTableEXT _funcptr_wglBindDisplayColorTableEXT
extern PFNWGLCREATEDISPLAYCOLORTABLEEXTPROC _funcptr_wglCreateDisplayColorTableEXT;
#define wglCreateDisplayColorTableEXT _funcptr_wglCreateDisplayColorTableEXT
extern PFNWGLDESTROYDISPLAYCOLORTABLEEXTPROC _funcptr_wglDestroyDisplayColorTableEXT;
#define wglDestroyDisplayColorTableEXT _funcptr_wglDestroyDisplayColorTableEXT
extern PFNWGLLOADDISPLAYCOLORTABLEEXTPROC _funcptr_wglLoadDisplayColorTableEXT;
#define wglLoadDisplayColorTableEXT _funcptr_wglLoadDisplayColorTableEXT

typedef const char * (CODEGEN_FUNCPTR * PFNWGLGETEXTENSIONSSTRINGEXTPROC)();

extern PFNWGLGETEXTENSIONSSTRINGEXTPROC _funcptr_wglGetExtensionsStringEXT;
#define wglGetExtensionsStringEXT _funcptr_wglGetExtensionsStringEXT

#define WGL_FRAMEBUFFER_SRGB_CAPABLE_EXT 0x20A9

#define WGL_ERROR_INVALID_PIXEL_TYPE_EXT 0x2043

typedef HDC (CODEGEN_FUNCPTR * PFNWGLGETCURRENTREADDCEXTPROC)();
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLMAKECONTEXTCURRENTEXTPROC)(HDC hDrawDC, HDC hReadDC, HGLRC hglrc);

extern PFNWGLGETCURRENTREADDCEXTPROC _funcptr_wglGetCurrentReadDCEXT;
#define wglGetCurrentReadDCEXT _funcptr_wglGetCurrentReadDCEXT
extern PFNWGLMAKECONTEXTCURRENTEXTPROC _funcptr_wglMakeContextCurrentEXT;
#define wglMakeContextCurrentEXT _funcptr_wglMakeContextCurrentEXT

#define WGL_SAMPLES_EXT 0x2042
#define WGL_SAMPLE_BUFFERS_EXT 0x2041

#define WGL_DRAW_TO_PBUFFER_EXT 0x202D
#define WGL_MAX_PBUFFER_HEIGHT_EXT 0x2030
#define WGL_MAX_PBUFFER_PIXELS_EXT 0x202E
#define WGL_MAX_PBUFFER_WIDTH_EXT 0x202F
#define WGL_OPTIMAL_PBUFFER_HEIGHT_EXT 0x2032
#define WGL_OPTIMAL_PBUFFER_WIDTH_EXT 0x2031
#define WGL_PBUFFER_HEIGHT_EXT 0x2035
#define WGL_PBUFFER_LARGEST_EXT 0x2033
#define WGL_PBUFFER_WIDTH_EXT 0x2034

typedef HPBUFFEREXT (CODEGEN_FUNCPTR * PFNWGLCREATEPBUFFEREXTPROC)(HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int * piAttribList);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLDESTROYPBUFFEREXTPROC)(HPBUFFEREXT hPbuffer);
typedef HDC (CODEGEN_FUNCPTR * PFNWGLGETPBUFFERDCEXTPROC)(HPBUFFEREXT hPbuffer);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLQUERYPBUFFEREXTPROC)(HPBUFFEREXT hPbuffer, int iAttribute, int * piValue);
typedef int (CODEGEN_FUNCPTR * PFNWGLRELEASEPBUFFERDCEXTPROC)(HPBUFFEREXT hPbuffer, HDC hDC);

extern PFNWGLCREATEPBUFFEREXTPROC _funcptr_wglCreatePbufferEXT;
#define wglCreatePbufferEXT _funcptr_wglCreatePbufferEXT
extern PFNWGLDESTROYPBUFFEREXTPROC _funcptr_wglDestroyPbufferEXT;
#define wglDestroyPbufferEXT _funcptr_wglDestroyPbufferEXT
extern PFNWGLGETPBUFFERDCEXTPROC _funcptr_wglGetPbufferDCEXT;
#define wglGetPbufferDCEXT _funcptr_wglGetPbufferDCEXT
extern PFNWGLQUERYPBUFFEREXTPROC _funcptr_wglQueryPbufferEXT;
#define wglQueryPbufferEXT _funcptr_wglQueryPbufferEXT
extern PFNWGLRELEASEPBUFFERDCEXTPROC _funcptr_wglReleasePbufferDCEXT;
#define wglReleasePbufferDCEXT _funcptr_wglReleasePbufferDCEXT

#define WGL_ACCELERATION_EXT 0x2003
#define WGL_ACCUM_ALPHA_BITS_EXT 0x2021
#define WGL_ACCUM_BITS_EXT 0x201D
#define WGL_ACCUM_BLUE_BITS_EXT 0x2020
#define WGL_ACCUM_GREEN_BITS_EXT 0x201F
#define WGL_ACCUM_RED_BITS_EXT 0x201E
#define WGL_ALPHA_BITS_EXT 0x201B
#define WGL_ALPHA_SHIFT_EXT 0x201C
#define WGL_AUX_BUFFERS_EXT 0x2024
#define WGL_BLUE_BITS_EXT 0x2019
#define WGL_BLUE_SHIFT_EXT 0x201A
#define WGL_COLOR_BITS_EXT 0x2014
#define WGL_DEPTH_BITS_EXT 0x2022
#define WGL_DOUBLE_BUFFER_EXT 0x2011
#define WGL_DRAW_TO_BITMAP_EXT 0x2002
#define WGL_DRAW_TO_WINDOW_EXT 0x2001
#define WGL_FULL_ACCELERATION_EXT 0x2027
#define WGL_GENERIC_ACCELERATION_EXT 0x2026
#define WGL_GREEN_BITS_EXT 0x2017
#define WGL_GREEN_SHIFT_EXT 0x2018
#define WGL_NEED_PALETTE_EXT 0x2004
#define WGL_NEED_SYSTEM_PALETTE_EXT 0x2005
#define WGL_NO_ACCELERATION_EXT 0x2025
#define WGL_NUMBER_OVERLAYS_EXT 0x2008
#define WGL_NUMBER_PIXEL_FORMATS_EXT 0x2000
#define WGL_NUMBER_UNDERLAYS_EXT 0x2009
#define WGL_PIXEL_TYPE_EXT 0x2013
#define WGL_RED_BITS_EXT 0x2015
#define WGL_RED_SHIFT_EXT 0x2016
#define WGL_SHARE_ACCUM_EXT 0x200E
#define WGL_SHARE_DEPTH_EXT 0x200C
#define WGL_SHARE_STENCIL_EXT 0x200D
#define WGL_STENCIL_BITS_EXT 0x2023
#define WGL_STEREO_EXT 0x2012
#define WGL_SUPPORT_GDI_EXT 0x200F
#define WGL_SUPPORT_OPENGL_EXT 0x2010
#define WGL_SWAP_COPY_EXT 0x2029
#define WGL_SWAP_EXCHANGE_EXT 0x2028
#define WGL_SWAP_LAYER_BUFFERS_EXT 0x2006
#define WGL_SWAP_METHOD_EXT 0x2007
#define WGL_SWAP_UNDEFINED_EXT 0x202A
#define WGL_TRANSPARENT_EXT 0x200A
#define WGL_TRANSPARENT_VALUE_EXT 0x200B
#define WGL_TYPE_COLORINDEX_EXT 0x202C
#define WGL_TYPE_RGBA_EXT 0x202B

typedef BOOL (CODEGEN_FUNCPTR * PFNWGLCHOOSEPIXELFORMATEXTPROC)(HDC hdc, const int * piAttribIList, const FLOAT * pfAttribFList, UINT nMaxFormats, int * piFormats, UINT * nNumFormats);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLGETPIXELFORMATATTRIBFVEXTPROC)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, FLOAT * pfValues);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLGETPIXELFORMATATTRIBIVEXTPROC)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int * piAttributes, int * piValues);

extern PFNWGLCHOOSEPIXELFORMATEXTPROC _funcptr_wglChoosePixelFormatEXT;
#define wglChoosePixelFormatEXT _funcptr_wglChoosePixelFormatEXT
extern PFNWGLGETPIXELFORMATATTRIBFVEXTPROC _funcptr_wglGetPixelFormatAttribfvEXT;
#define wglGetPixelFormatAttribfvEXT _funcptr_wglGetPixelFormatAttribfvEXT
extern PFNWGLGETPIXELFORMATATTRIBIVEXTPROC _funcptr_wglGetPixelFormatAttribivEXT;
#define wglGetPixelFormatAttribivEXT _funcptr_wglGetPixelFormatAttribivEXT

#define WGL_TYPE_RGBA_UNSIGNED_FLOAT_EXT 0x20A8

typedef int (CODEGEN_FUNCPTR * PFNWGLGETSWAPINTERVALEXTPROC)();
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLSWAPINTERVALEXTPROC)(int interval);

extern PFNWGLGETSWAPINTERVALEXTPROC _funcptr_wglGetSwapIntervalEXT;
#define wglGetSwapIntervalEXT _funcptr_wglGetSwapIntervalEXT
extern PFNWGLSWAPINTERVALEXTPROC _funcptr_wglSwapIntervalEXT;
#define wglSwapIntervalEXT _funcptr_wglSwapIntervalEXT

#define WGL_DIGITAL_VIDEO_CURSOR_ALPHA_FRAMEBUFFER_I3D 0x2050
#define WGL_DIGITAL_VIDEO_CURSOR_ALPHA_VALUE_I3D 0x2051
#define WGL_DIGITAL_VIDEO_CURSOR_INCLUDED_I3D 0x2052
#define WGL_DIGITAL_VIDEO_GAMMA_CORRECTED_I3D 0x2053

typedef BOOL (CODEGEN_FUNCPTR * PFNWGLGETDIGITALVIDEOPARAMETERSI3DPROC)(HDC hDC, int iAttribute, int * piValue);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLSETDIGITALVIDEOPARAMETERSI3DPROC)(HDC hDC, int iAttribute, const int * piValue);

extern PFNWGLGETDIGITALVIDEOPARAMETERSI3DPROC _funcptr_wglGetDigitalVideoParametersI3D;
#define wglGetDigitalVideoParametersI3D _funcptr_wglGetDigitalVideoParametersI3D
extern PFNWGLSETDIGITALVIDEOPARAMETERSI3DPROC _funcptr_wglSetDigitalVideoParametersI3D;
#define wglSetDigitalVideoParametersI3D _funcptr_wglSetDigitalVideoParametersI3D

#define WGL_GAMMA_EXCLUDE_DESKTOP_I3D 0x204F
#define WGL_GAMMA_TABLE_SIZE_I3D 0x204E

typedef BOOL (CODEGEN_FUNCPTR * PFNWGLGETGAMMATABLEI3DPROC)(HDC hDC, int iEntries, USHORT * puRed, USHORT * puGreen, USHORT * puBlue);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLGETGAMMATABLEPARAMETERSI3DPROC)(HDC hDC, int iAttribute, int * piValue);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLSETGAMMATABLEI3DPROC)(HDC hDC, int iEntries, const USHORT * puRed, const USHORT * puGreen, const USHORT * puBlue);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLSETGAMMATABLEPARAMETERSI3DPROC)(HDC hDC, int iAttribute, const int * piValue);

extern PFNWGLGETGAMMATABLEI3DPROC _funcptr_wglGetGammaTableI3D;
#define wglGetGammaTableI3D _funcptr_wglGetGammaTableI3D
extern PFNWGLGETGAMMATABLEPARAMETERSI3DPROC _funcptr_wglGetGammaTableParametersI3D;
#define wglGetGammaTableParametersI3D _funcptr_wglGetGammaTableParametersI3D
extern PFNWGLSETGAMMATABLEI3DPROC _funcptr_wglSetGammaTableI3D;
#define wglSetGammaTableI3D _funcptr_wglSetGammaTableI3D
extern PFNWGLSETGAMMATABLEPARAMETERSI3DPROC _funcptr_wglSetGammaTableParametersI3D;
#define wglSetGammaTableParametersI3D _funcptr_wglSetGammaTableParametersI3D

#define WGL_GENLOCK_SOURCE_DIGITAL_FIELD_I3D 0x2049
#define WGL_GENLOCK_SOURCE_DIGITAL_SYNC_I3D 0x2048
#define WGL_GENLOCK_SOURCE_EDGE_BOTH_I3D 0x204C
#define WGL_GENLOCK_SOURCE_EDGE_FALLING_I3D 0x204A
#define WGL_GENLOCK_SOURCE_EDGE_RISING_I3D 0x204B
#define WGL_GENLOCK_SOURCE_EXTERNAL_FIELD_I3D 0x2046
#define WGL_GENLOCK_SOURCE_EXTERNAL_SYNC_I3D 0x2045
#define WGL_GENLOCK_SOURCE_EXTERNAL_TTL_I3D 0x2047
#define WGL_GENLOCK_SOURCE_MULTIVIEW_I3D 0x2044

typedef BOOL (CODEGEN_FUNCPTR * PFNWGLDISABLEGENLOCKI3DPROC)(HDC hDC);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLENABLEGENLOCKI3DPROC)(HDC hDC);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLGENLOCKSAMPLERATEI3DPROC)(HDC hDC, UINT uRate);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLGENLOCKSOURCEDELAYI3DPROC)(HDC hDC, UINT uDelay);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLGENLOCKSOURCEEDGEI3DPROC)(HDC hDC, UINT uEdge);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLGENLOCKSOURCEI3DPROC)(HDC hDC, UINT uSource);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLGETGENLOCKSAMPLERATEI3DPROC)(HDC hDC, UINT * uRate);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLGETGENLOCKSOURCEDELAYI3DPROC)(HDC hDC, UINT * uDelay);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLGETGENLOCKSOURCEEDGEI3DPROC)(HDC hDC, UINT * uEdge);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLGETGENLOCKSOURCEI3DPROC)(HDC hDC, UINT * uSource);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLISENABLEDGENLOCKI3DPROC)(HDC hDC, BOOL * pFlag);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLQUERYGENLOCKMAXSOURCEDELAYI3DPROC)(HDC hDC, UINT * uMaxLineDelay, UINT * uMaxPixelDelay);

extern PFNWGLDISABLEGENLOCKI3DPROC _funcptr_wglDisableGenlockI3D;
#define wglDisableGenlockI3D _funcptr_wglDisableGenlockI3D
extern PFNWGLENABLEGENLOCKI3DPROC _funcptr_wglEnableGenlockI3D;
#define wglEnableGenlockI3D _funcptr_wglEnableGenlockI3D
extern PFNWGLGENLOCKSAMPLERATEI3DPROC _funcptr_wglGenlockSampleRateI3D;
#define wglGenlockSampleRateI3D _funcptr_wglGenlockSampleRateI3D
extern PFNWGLGENLOCKSOURCEDELAYI3DPROC _funcptr_wglGenlockSourceDelayI3D;
#define wglGenlockSourceDelayI3D _funcptr_wglGenlockSourceDelayI3D
extern PFNWGLGENLOCKSOURCEEDGEI3DPROC _funcptr_wglGenlockSourceEdgeI3D;
#define wglGenlockSourceEdgeI3D _funcptr_wglGenlockSourceEdgeI3D
extern PFNWGLGENLOCKSOURCEI3DPROC _funcptr_wglGenlockSourceI3D;
#define wglGenlockSourceI3D _funcptr_wglGenlockSourceI3D
extern PFNWGLGETGENLOCKSAMPLERATEI3DPROC _funcptr_wglGetGenlockSampleRateI3D;
#define wglGetGenlockSampleRateI3D _funcptr_wglGetGenlockSampleRateI3D
extern PFNWGLGETGENLOCKSOURCEDELAYI3DPROC _funcptr_wglGetGenlockSourceDelayI3D;
#define wglGetGenlockSourceDelayI3D _funcptr_wglGetGenlockSourceDelayI3D
extern PFNWGLGETGENLOCKSOURCEEDGEI3DPROC _funcptr_wglGetGenlockSourceEdgeI3D;
#define wglGetGenlockSourceEdgeI3D _funcptr_wglGetGenlockSourceEdgeI3D
extern PFNWGLGETGENLOCKSOURCEI3DPROC _funcptr_wglGetGenlockSourceI3D;
#define wglGetGenlockSourceI3D _funcptr_wglGetGenlockSourceI3D
extern PFNWGLISENABLEDGENLOCKI3DPROC _funcptr_wglIsEnabledGenlockI3D;
#define wglIsEnabledGenlockI3D _funcptr_wglIsEnabledGenlockI3D
extern PFNWGLQUERYGENLOCKMAXSOURCEDELAYI3DPROC _funcptr_wglQueryGenlockMaxSourceDelayI3D;
#define wglQueryGenlockMaxSourceDelayI3D _funcptr_wglQueryGenlockMaxSourceDelayI3D

#define WGL_IMAGE_BUFFER_LOCK_I3D 0x00000002
#define WGL_IMAGE_BUFFER_MIN_ACCESS_I3D 0x00000001

typedef BOOL (CODEGEN_FUNCPTR * PFNWGLASSOCIATEIMAGEBUFFEREVENTSI3DPROC)(HDC hDC, const HANDLE * pEvent, const LPVOID * pAddress, const DWORD * pSize, UINT count);
typedef LPVOID (CODEGEN_FUNCPTR * PFNWGLCREATEIMAGEBUFFERI3DPROC)(HDC hDC, DWORD dwSize, UINT uFlags);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLDESTROYIMAGEBUFFERI3DPROC)(HDC hDC, LPVOID pAddress);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLRELEASEIMAGEBUFFEREVENTSI3DPROC)(HDC hDC, const LPVOID * pAddress, UINT count);

extern PFNWGLASSOCIATEIMAGEBUFFEREVENTSI3DPROC _funcptr_wglAssociateImageBufferEventsI3D;
#define wglAssociateImageBufferEventsI3D _funcptr_wglAssociateImageBufferEventsI3D
extern PFNWGLCREATEIMAGEBUFFERI3DPROC _funcptr_wglCreateImageBufferI3D;
#define wglCreateImageBufferI3D _funcptr_wglCreateImageBufferI3D
extern PFNWGLDESTROYIMAGEBUFFERI3DPROC _funcptr_wglDestroyImageBufferI3D;
#define wglDestroyImageBufferI3D _funcptr_wglDestroyImageBufferI3D
extern PFNWGLRELEASEIMAGEBUFFEREVENTSI3DPROC _funcptr_wglReleaseImageBufferEventsI3D;
#define wglReleaseImageBufferEventsI3D _funcptr_wglReleaseImageBufferEventsI3D

typedef BOOL (CODEGEN_FUNCPTR * PFNWGLDISABLEFRAMELOCKI3DPROC)();
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLENABLEFRAMELOCKI3DPROC)();
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLISENABLEDFRAMELOCKI3DPROC)(BOOL * pFlag);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLQUERYFRAMELOCKMASTERI3DPROC)(BOOL * pFlag);

extern PFNWGLDISABLEFRAMELOCKI3DPROC _funcptr_wglDisableFrameLockI3D;
#define wglDisableFrameLockI3D _funcptr_wglDisableFrameLockI3D
extern PFNWGLENABLEFRAMELOCKI3DPROC _funcptr_wglEnableFrameLockI3D;
#define wglEnableFrameLockI3D _funcptr_wglEnableFrameLockI3D
extern PFNWGLISENABLEDFRAMELOCKI3DPROC _funcptr_wglIsEnabledFrameLockI3D;
#define wglIsEnabledFrameLockI3D _funcptr_wglIsEnabledFrameLockI3D
extern PFNWGLQUERYFRAMELOCKMASTERI3DPROC _funcptr_wglQueryFrameLockMasterI3D;
#define wglQueryFrameLockMasterI3D _funcptr_wglQueryFrameLockMasterI3D

typedef BOOL (CODEGEN_FUNCPTR * PFNWGLBEGINFRAMETRACKINGI3DPROC)();
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLENDFRAMETRACKINGI3DPROC)();
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLGETFRAMEUSAGEI3DPROC)(float * pUsage);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLQUERYFRAMETRACKINGI3DPROC)(DWORD * pFrameCount, DWORD * pMissedFrames, float * pLastMissedUsage);

extern PFNWGLBEGINFRAMETRACKINGI3DPROC _funcptr_wglBeginFrameTrackingI3D;
#define wglBeginFrameTrackingI3D _funcptr_wglBeginFrameTrackingI3D
extern PFNWGLENDFRAMETRACKINGI3DPROC _funcptr_wglEndFrameTrackingI3D;
#define wglEndFrameTrackingI3D _funcptr_wglEndFrameTrackingI3D
extern PFNWGLGETFRAMEUSAGEI3DPROC _funcptr_wglGetFrameUsageI3D;
#define wglGetFrameUsageI3D _funcptr_wglGetFrameUsageI3D
extern PFNWGLQUERYFRAMETRACKINGI3DPROC _funcptr_wglQueryFrameTrackingI3D;
#define wglQueryFrameTrackingI3D _funcptr_wglQueryFrameTrackingI3D

#define WGL_ACCESS_READ_ONLY_NV 0x00000000
#define WGL_ACCESS_READ_WRITE_NV 0x00000001
#define WGL_ACCESS_WRITE_DISCARD_NV 0x00000002

typedef BOOL (CODEGEN_FUNCPTR * PFNWGLDXCLOSEDEVICENVPROC)(HANDLE hDevice);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLDXLOCKOBJECTSNVPROC)(HANDLE hDevice, GLint count, HANDLE * hObjects);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLDXOBJECTACCESSNVPROC)(HANDLE hObject, GLenum access);
typedef HANDLE (CODEGEN_FUNCPTR * PFNWGLDXOPENDEVICENVPROC)(void * dxDevice);
typedef HANDLE (CODEGEN_FUNCPTR * PFNWGLDXREGISTEROBJECTNVPROC)(HANDLE hDevice, void * dxObject, GLuint name, GLenum type, GLenum access);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLDXSETRESOURCESHAREHANDLENVPROC)(void * dxObject, HANDLE shareHandle);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLDXUNLOCKOBJECTSNVPROC)(HANDLE hDevice, GLint count, HANDLE * hObjects);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLDXUNREGISTEROBJECTNVPROC)(HANDLE hDevice, HANDLE hObject);

extern PFNWGLDXCLOSEDEVICENVPROC _funcptr_wglDXCloseDeviceNV;
#define wglDXCloseDeviceNV _funcptr_wglDXCloseDeviceNV
extern PFNWGLDXLOCKOBJECTSNVPROC _funcptr_wglDXLockObjectsNV;
#define wglDXLockObjectsNV _funcptr_wglDXLockObjectsNV
extern PFNWGLDXOBJECTACCESSNVPROC _funcptr_wglDXObjectAccessNV;
#define wglDXObjectAccessNV _funcptr_wglDXObjectAccessNV
extern PFNWGLDXOPENDEVICENVPROC _funcptr_wglDXOpenDeviceNV;
#define wglDXOpenDeviceNV _funcptr_wglDXOpenDeviceNV
extern PFNWGLDXREGISTEROBJECTNVPROC _funcptr_wglDXRegisterObjectNV;
#define wglDXRegisterObjectNV _funcptr_wglDXRegisterObjectNV
extern PFNWGLDXSETRESOURCESHAREHANDLENVPROC _funcptr_wglDXSetResourceShareHandleNV;
#define wglDXSetResourceShareHandleNV _funcptr_wglDXSetResourceShareHandleNV
extern PFNWGLDXUNLOCKOBJECTSNVPROC _funcptr_wglDXUnlockObjectsNV;
#define wglDXUnlockObjectsNV _funcptr_wglDXUnlockObjectsNV
extern PFNWGLDXUNREGISTEROBJECTNVPROC _funcptr_wglDXUnregisterObjectNV;
#define wglDXUnregisterObjectNV _funcptr_wglDXUnregisterObjectNV

typedef BOOL (CODEGEN_FUNCPTR * PFNWGLCOPYIMAGESUBDATANVPROC)(HGLRC hSrcRC, GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, HGLRC hDstRC, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei width, GLsizei height, GLsizei depth);

extern PFNWGLCOPYIMAGESUBDATANVPROC _funcptr_wglCopyImageSubDataNV;
#define wglCopyImageSubDataNV _funcptr_wglCopyImageSubDataNV

#define WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_RGBA_NV 0x20B4
#define WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_RGB_NV 0x20B3
#define WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_RG_NV 0x20B2
#define WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_R_NV 0x20B1
#define WGL_FLOAT_COMPONENTS_NV 0x20B0
#define WGL_TEXTURE_FLOAT_RGBA_NV 0x20B8
#define WGL_TEXTURE_FLOAT_RGB_NV 0x20B7
#define WGL_TEXTURE_FLOAT_RG_NV 0x20B6
#define WGL_TEXTURE_FLOAT_R_NV 0x20B5

#define WGL_ERROR_INCOMPATIBLE_AFFINITY_MASKS_NV 0x20D0
#define WGL_ERROR_MISSING_AFFINITY_MASK_NV 0x20D1

typedef HDC (CODEGEN_FUNCPTR * PFNWGLCREATEAFFINITYDCNVPROC)(const HGPUNV * phGpuList);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLDELETEDCNVPROC)(HDC hdc);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLENUMGPUDEVICESNVPROC)(HGPUNV hGpu, UINT iDeviceIndex, PGPU_DEVICE lpGpuDevice);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLENUMGPUSFROMAFFINITYDCNVPROC)(HDC hAffinityDC, UINT iGpuIndex, HGPUNV * hGpu);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLENUMGPUSNVPROC)(UINT iGpuIndex, HGPUNV * phGpu);

extern PFNWGLCREATEAFFINITYDCNVPROC _funcptr_wglCreateAffinityDCNV;
#define wglCreateAffinityDCNV _funcptr_wglCreateAffinityDCNV
extern PFNWGLDELETEDCNVPROC _funcptr_wglDeleteDCNV;
#define wglDeleteDCNV _funcptr_wglDeleteDCNV
extern PFNWGLENUMGPUDEVICESNVPROC _funcptr_wglEnumGpuDevicesNV;
#define wglEnumGpuDevicesNV _funcptr_wglEnumGpuDevicesNV
extern PFNWGLENUMGPUSFROMAFFINITYDCNVPROC _funcptr_wglEnumGpusFromAffinityDCNV;
#define wglEnumGpusFromAffinityDCNV _funcptr_wglEnumGpusFromAffinityDCNV
extern PFNWGLENUMGPUSNVPROC _funcptr_wglEnumGpusNV;
#define wglEnumGpusNV _funcptr_wglEnumGpusNV

#define WGL_COLOR_SAMPLES_NV 0x20B9
#define WGL_COVERAGE_SAMPLES_NV 0x2042

#define WGL_NUM_VIDEO_SLOTS_NV 0x20F0

typedef BOOL (CODEGEN_FUNCPTR * PFNWGLBINDVIDEODEVICENVPROC)(HDC hDC, unsigned int uVideoSlot, HVIDEOOUTPUTDEVICENV hVideoDevice, const int * piAttribList);
typedef int (CODEGEN_FUNCPTR * PFNWGLENUMERATEVIDEODEVICESNVPROC)(HDC hDC, HVIDEOOUTPUTDEVICENV * phDeviceList);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLQUERYCURRENTCONTEXTNVPROC)(int iAttribute, int * piValue);

extern PFNWGLBINDVIDEODEVICENVPROC _funcptr_wglBindVideoDeviceNV;
#define wglBindVideoDeviceNV _funcptr_wglBindVideoDeviceNV
extern PFNWGLENUMERATEVIDEODEVICESNVPROC _funcptr_wglEnumerateVideoDevicesNV;
#define wglEnumerateVideoDevicesNV _funcptr_wglEnumerateVideoDevicesNV
extern PFNWGLQUERYCURRENTCONTEXTNVPROC _funcptr_wglQueryCurrentContextNV;
#define wglQueryCurrentContextNV _funcptr_wglQueryCurrentContextNV

#define WGL_BIND_TO_TEXTURE_DEPTH_NV 0x20A3
#define WGL_BIND_TO_TEXTURE_RECTANGLE_DEPTH_NV 0x20A4
#define WGL_DEPTH_COMPONENT_NV 0x20A7
#define WGL_DEPTH_TEXTURE_FORMAT_NV 0x20A5
#define WGL_TEXTURE_DEPTH_COMPONENT_NV 0x20A6

#define WGL_BIND_TO_TEXTURE_RECTANGLE_RGBA_NV 0x20A1
#define WGL_BIND_TO_TEXTURE_RECTANGLE_RGB_NV 0x20A0
#define WGL_TEXTURE_RECTANGLE_NV 0x20A2

typedef BOOL (CODEGEN_FUNCPTR * PFNWGLBINDSWAPBARRIERNVPROC)(GLuint group, GLuint barrier);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLJOINSWAPGROUPNVPROC)(HDC hDC, GLuint group);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLQUERYFRAMECOUNTNVPROC)(HDC hDC, GLuint * count);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLQUERYMAXSWAPGROUPSNVPROC)(HDC hDC, GLuint * maxGroups, GLuint * maxBarriers);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLQUERYSWAPGROUPNVPROC)(HDC hDC, GLuint * group, GLuint * barrier);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLRESETFRAMECOUNTNVPROC)(HDC hDC);

extern PFNWGLBINDSWAPBARRIERNVPROC _funcptr_wglBindSwapBarrierNV;
#define wglBindSwapBarrierNV _funcptr_wglBindSwapBarrierNV
extern PFNWGLJOINSWAPGROUPNVPROC _funcptr_wglJoinSwapGroupNV;
#define wglJoinSwapGroupNV _funcptr_wglJoinSwapGroupNV
extern PFNWGLQUERYFRAMECOUNTNVPROC _funcptr_wglQueryFrameCountNV;
#define wglQueryFrameCountNV _funcptr_wglQueryFrameCountNV
extern PFNWGLQUERYMAXSWAPGROUPSNVPROC _funcptr_wglQueryMaxSwapGroupsNV;
#define wglQueryMaxSwapGroupsNV _funcptr_wglQueryMaxSwapGroupsNV
extern PFNWGLQUERYSWAPGROUPNVPROC _funcptr_wglQuerySwapGroupNV;
#define wglQuerySwapGroupNV _funcptr_wglQuerySwapGroupNV
extern PFNWGLRESETFRAMECOUNTNVPROC _funcptr_wglResetFrameCountNV;
#define wglResetFrameCountNV _funcptr_wglResetFrameCountNV

typedef void * (CODEGEN_FUNCPTR * PFNWGLALLOCATEMEMORYNVPROC)(GLsizei size, GLfloat readfreq, GLfloat writefreq, GLfloat priority);
typedef void (CODEGEN_FUNCPTR * PFNWGLFREEMEMORYNVPROC)(void * pointer);

extern PFNWGLALLOCATEMEMORYNVPROC _funcptr_wglAllocateMemoryNV;
#define wglAllocateMemoryNV _funcptr_wglAllocateMemoryNV
extern PFNWGLFREEMEMORYNVPROC _funcptr_wglFreeMemoryNV;
#define wglFreeMemoryNV _funcptr_wglFreeMemoryNV

#define WGL_NUM_VIDEO_CAPTURE_SLOTS_NV 0x20CF
#define WGL_UNIQUE_ID_NV 0x20CE

typedef BOOL (CODEGEN_FUNCPTR * PFNWGLBINDVIDEOCAPTUREDEVICENVPROC)(UINT uVideoSlot, HVIDEOINPUTDEVICENV hDevice);
typedef UINT (CODEGEN_FUNCPTR * PFNWGLENUMERATEVIDEOCAPTUREDEVICESNVPROC)(HDC hDc, HVIDEOINPUTDEVICENV * phDeviceList);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLLOCKVIDEOCAPTUREDEVICENVPROC)(HDC hDc, HVIDEOINPUTDEVICENV hDevice);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLQUERYVIDEOCAPTUREDEVICENVPROC)(HDC hDc, HVIDEOINPUTDEVICENV hDevice, int iAttribute, int * piValue);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLRELEASEVIDEOCAPTUREDEVICENVPROC)(HDC hDc, HVIDEOINPUTDEVICENV hDevice);

extern PFNWGLBINDVIDEOCAPTUREDEVICENVPROC _funcptr_wglBindVideoCaptureDeviceNV;
#define wglBindVideoCaptureDeviceNV _funcptr_wglBindVideoCaptureDeviceNV
extern PFNWGLENUMERATEVIDEOCAPTUREDEVICESNVPROC _funcptr_wglEnumerateVideoCaptureDevicesNV;
#define wglEnumerateVideoCaptureDevicesNV _funcptr_wglEnumerateVideoCaptureDevicesNV
extern PFNWGLLOCKVIDEOCAPTUREDEVICENVPROC _funcptr_wglLockVideoCaptureDeviceNV;
#define wglLockVideoCaptureDeviceNV _funcptr_wglLockVideoCaptureDeviceNV
extern PFNWGLQUERYVIDEOCAPTUREDEVICENVPROC _funcptr_wglQueryVideoCaptureDeviceNV;
#define wglQueryVideoCaptureDeviceNV _funcptr_wglQueryVideoCaptureDeviceNV
extern PFNWGLRELEASEVIDEOCAPTUREDEVICENVPROC _funcptr_wglReleaseVideoCaptureDeviceNV;
#define wglReleaseVideoCaptureDeviceNV _funcptr_wglReleaseVideoCaptureDeviceNV

#define WGL_BIND_TO_VIDEO_RGBA_NV 0x20C1
#define WGL_BIND_TO_VIDEO_RGB_AND_DEPTH_NV 0x20C2
#define WGL_BIND_TO_VIDEO_RGB_NV 0x20C0
#define WGL_VIDEO_OUT_ALPHA_NV 0x20C4
#define WGL_VIDEO_OUT_COLOR_AND_ALPHA_NV 0x20C6
#define WGL_VIDEO_OUT_COLOR_AND_DEPTH_NV 0x20C7
#define WGL_VIDEO_OUT_COLOR_NV 0x20C3
#define WGL_VIDEO_OUT_DEPTH_NV 0x20C5
#define WGL_VIDEO_OUT_FIELD_1 0x20C9
#define WGL_VIDEO_OUT_FIELD_2 0x20CA
#define WGL_VIDEO_OUT_FRAME 0x20C8
#define WGL_VIDEO_OUT_STACKED_FIELDS_1_2 0x20CB
#define WGL_VIDEO_OUT_STACKED_FIELDS_2_1 0x20CC

typedef BOOL (CODEGEN_FUNCPTR * PFNWGLBINDVIDEOIMAGENVPROC)(HPVIDEODEV hVideoDevice, HPBUFFERARB hPbuffer, int iVideoBuffer);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLGETVIDEODEVICENVPROC)(HDC hDC, int numDevices, HPVIDEODEV * hVideoDevice);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLGETVIDEOINFONVPROC)(HPVIDEODEV hpVideoDevice, unsigned long * pulCounterOutputPbuffer, unsigned long * pulCounterOutputVideo);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLRELEASEVIDEODEVICENVPROC)(HPVIDEODEV hVideoDevice);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLRELEASEVIDEOIMAGENVPROC)(HPBUFFERARB hPbuffer, int iVideoBuffer);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLSENDPBUFFERTOVIDEONVPROC)(HPBUFFERARB hPbuffer, int iBufferType, unsigned long * pulCounterPbuffer, BOOL bBlock);

extern PFNWGLBINDVIDEOIMAGENVPROC _funcptr_wglBindVideoImageNV;
#define wglBindVideoImageNV _funcptr_wglBindVideoImageNV
extern PFNWGLGETVIDEODEVICENVPROC _funcptr_wglGetVideoDeviceNV;
#define wglGetVideoDeviceNV _funcptr_wglGetVideoDeviceNV
extern PFNWGLGETVIDEOINFONVPROC _funcptr_wglGetVideoInfoNV;
#define wglGetVideoInfoNV _funcptr_wglGetVideoInfoNV
extern PFNWGLRELEASEVIDEODEVICENVPROC _funcptr_wglReleaseVideoDeviceNV;
#define wglReleaseVideoDeviceNV _funcptr_wglReleaseVideoDeviceNV
extern PFNWGLRELEASEVIDEOIMAGENVPROC _funcptr_wglReleaseVideoImageNV;
#define wglReleaseVideoImageNV _funcptr_wglReleaseVideoImageNV
extern PFNWGLSENDPBUFFERTOVIDEONVPROC _funcptr_wglSendPbufferToVideoNV;
#define wglSendPbufferToVideoNV _funcptr_wglSendPbufferToVideoNV

typedef BOOL (CODEGEN_FUNCPTR * PFNWGLGETMSCRATEOMLPROC)(HDC hdc, INT32 * numerator, INT32 * denominator);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLGETSYNCVALUESOMLPROC)(HDC hdc, INT64 * ust, INT64 * msc, INT64 * sbc);
typedef INT64 (CODEGEN_FUNCPTR * PFNWGLSWAPBUFFERSMSCOMLPROC)(HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder);
typedef INT64 (CODEGEN_FUNCPTR * PFNWGLSWAPLAYERBUFFERSMSCOMLPROC)(HDC hdc, int fuPlanes, INT64 target_msc, INT64 divisor, INT64 remainder);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLWAITFORMSCOMLPROC)(HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder, INT64 * ust, INT64 * msc, INT64 * sbc);
typedef BOOL (CODEGEN_FUNCPTR * PFNWGLWAITFORSBCOMLPROC)(HDC hdc, INT64 target_sbc, INT64 * ust, INT64 * msc, INT64 * sbc);

extern PFNWGLGETMSCRATEOMLPROC _funcptr_wglGetMscRateOML;
#define wglGetMscRateOML _funcptr_wglGetMscRateOML
extern PFNWGLGETSYNCVALUESOMLPROC _funcptr_wglGetSyncValuesOML;
#define wglGetSyncValuesOML _funcptr_wglGetSyncValuesOML
extern PFNWGLSWAPBUFFERSMSCOMLPROC _funcptr_wglSwapBuffersMscOML;
#define wglSwapBuffersMscOML _funcptr_wglSwapBuffersMscOML
extern PFNWGLSWAPLAYERBUFFERSMSCOMLPROC _funcptr_wglSwapLayerBuffersMscOML;
#define wglSwapLayerBuffersMscOML _funcptr_wglSwapLayerBuffersMscOML
extern PFNWGLWAITFORMSCOMLPROC _funcptr_wglWaitForMscOML;
#define wglWaitForMscOML _funcptr_wglWaitForMscOML
extern PFNWGLWAITFORSBCOMLPROC _funcptr_wglWaitForSbcOML;
#define wglWaitForSbcOML _funcptr_wglWaitForSbcOML

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*WINDOWSGL_GEN_EXTENSIONS_H*/
