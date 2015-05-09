#ifndef GLXWIN_GEN_EXTENSIONS_H
#define GLXWIN_GEN_EXTENSIONS_H


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
extern int glXext_3DFX_multisample;
extern int glXext_AMD_gpu_association;
extern int glXext_ARB_create_context;
extern int glXext_ARB_create_context_profile;
extern int glXext_ARB_create_context_robustness;
extern int glXext_ARB_fbconfig_float;
extern int glXext_ARB_framebuffer_sRGB;
extern int glXext_ARB_get_proc_address;
extern int glXext_ARB_multisample;
extern int glXext_ARB_robustness_application_isolation;
extern int glXext_ARB_robustness_share_group_isolation;
extern int glXext_ARB_vertex_buffer_object;
extern int glXext_EXT_buffer_age;
extern int glXext_EXT_create_context_es2_profile;
extern int glXext_EXT_create_context_es_profile;
extern int glXext_EXT_fbconfig_packed_float;
extern int glXext_EXT_framebuffer_sRGB;
extern int glXext_EXT_import_context;
extern int glXext_EXT_swap_control;
extern int glXext_EXT_swap_control_tear;
extern int glXext_EXT_texture_from_pixmap;
extern int glXext_EXT_visual_info;
extern int glXext_EXT_visual_rating;
extern int glXext_INTEL_swap_event;
extern int glXext_MESA_agp_offset;
extern int glXext_MESA_copy_sub_buffer;
extern int glXext_MESA_pixmap_colormap;
extern int glXext_MESA_release_buffers;
extern int glXext_MESA_set_3dfx_mode;
extern int glXext_NV_copy_image;
extern int glXext_NV_float_buffer;
extern int glXext_NV_multisample_coverage;
extern int glXext_NV_present_video;
extern int glXext_NV_swap_group;
extern int glXext_NV_video_capture;
extern int glXext_NV_video_output;
extern int glXext_OML_swap_method;
extern int glXext_OML_sync_control;
extern int glXext_SGIS_blended_overlay;
extern int glXext_SGIS_multisample;
extern int glXext_SGIS_shared_multisample;
extern int glXext_SGIX_fbconfig;
extern int glXext_SGIX_pbuffer;
extern int glXext_SGIX_swap_barrier;
extern int glXext_SGIX_swap_group;
extern int glXext_SGIX_video_resize;
extern int glXext_SGIX_visual_select_group;
extern int glXext_SGI_cushion;
extern int glXext_SGI_make_current_read;
extern int glXext_SGI_swap_control;
extern int glXext_SGI_video_sync;
extern int glXext_SUN_get_transparent_index;

#define GLX_SAMPLES_3DFX 0x8051
#define GLX_SAMPLE_BUFFERS_3DFX 0x8050

#define GLX_GPU_CLOCK_AMD 0x21A4
#define GLX_GPU_FASTEST_TARGET_GPUS_AMD 0x21A2
#define GLX_GPU_NUM_PIPES_AMD 0x21A5
#define GLX_GPU_NUM_RB_AMD 0x21A7
#define GLX_GPU_NUM_SIMD_AMD 0x21A6
#define GLX_GPU_NUM_SPI_AMD 0x21A8
#define GLX_GPU_OPENGL_VERSION_STRING_AMD 0x1F02
#define GLX_GPU_RAM_AMD 0x21A3
#define GLX_GPU_RENDERER_STRING_AMD 0x1F01
#define GLX_GPU_VENDOR_AMD 0x1F00

#define GLX_CONTEXT_DEBUG_BIT_ARB 0x00000001
#define GLX_CONTEXT_FLAGS_ARB 0x2094
#define GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092

typedef GLXContext (CODEGEN_FUNCPTR * PFNGLXCREATECONTEXTATTRIBSARBPROC)(Display * dpy, GLXFBConfig config, GLXContext share_context, Bool direct, const int * attrib_list);

extern PFNGLXCREATECONTEXTATTRIBSARBPROC _funcptr_glXCreateContextAttribsARB;
#define glXCreateContextAttribsARB _funcptr_glXCreateContextAttribsARB

#define GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define GLX_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define GLX_CONTEXT_PROFILE_MASK_ARB 0x9126

#define GLX_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
#define GLX_CONTEXT_ROBUST_ACCESS_BIT_ARB 0x00000004
#define GLX_LOSE_CONTEXT_ON_RESET_ARB 0x8252
#define GLX_NO_RESET_NOTIFICATION_ARB 0x8261

#define GLX_RGBA_FLOAT_BIT_ARB 0x00000004
#define GLX_RGBA_FLOAT_TYPE_ARB 0x20B9

#define GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB 0x20B2

typedef __GLXextFuncPtr (CODEGEN_FUNCPTR * PFNGLXGETPROCADDRESSARBPROC)(const GLubyte * procName);

extern PFNGLXGETPROCADDRESSARBPROC _funcptr_glXGetProcAddressARB;
#define glXGetProcAddressARB _funcptr_glXGetProcAddressARB

#define GLX_SAMPLES_ARB 100001
#define GLX_SAMPLE_BUFFERS_ARB 100000

#define GLX_CONTEXT_RESET_ISOLATION_BIT_ARB 0x00000008

#define GLX_CONTEXT_RESET_ISOLATION_BIT_ARB 0x00000008

#define GLX_CONTEXT_ALLOW_BUFFER_BYTE_ORDER_MISMATCH_ARB 0x2095

#define GLX_BACK_BUFFER_AGE_EXT 0x20F4

#define GLX_CONTEXT_ES2_PROFILE_BIT_EXT 0x00000004

#define GLX_CONTEXT_ES_PROFILE_BIT_EXT 0x00000004

#define GLX_RGBA_UNSIGNED_FLOAT_BIT_EXT 0x00000008
#define GLX_RGBA_UNSIGNED_FLOAT_TYPE_EXT 0x20B1

#define GLX_FRAMEBUFFER_SRGB_CAPABLE_EXT 0x20B2

#define GLX_SCREEN_EXT 0x800C
#define GLX_SHARE_CONTEXT_EXT 0x800A
#define GLX_VISUAL_ID_EXT 0x800B

typedef void (CODEGEN_FUNCPTR * PFNGLXFREECONTEXTEXTPROC)(Display * dpy, GLXContext context);
typedef GLXContextID (CODEGEN_FUNCPTR * PFNGLXGETCONTEXTIDEXTPROC)(const GLXContext context);
typedef Display * (CODEGEN_FUNCPTR * PFNGLXGETCURRENTDISPLAYEXTPROC)();
typedef GLXContext (CODEGEN_FUNCPTR * PFNGLXIMPORTCONTEXTEXTPROC)(Display * dpy, GLXContextID contextID);
typedef int (CODEGEN_FUNCPTR * PFNGLXQUERYCONTEXTINFOEXTPROC)(Display * dpy, GLXContext context, int attribute, int * value);

extern PFNGLXFREECONTEXTEXTPROC _funcptr_glXFreeContextEXT;
#define glXFreeContextEXT _funcptr_glXFreeContextEXT
extern PFNGLXGETCONTEXTIDEXTPROC _funcptr_glXGetContextIDEXT;
#define glXGetContextIDEXT _funcptr_glXGetContextIDEXT
extern PFNGLXGETCURRENTDISPLAYEXTPROC _funcptr_glXGetCurrentDisplayEXT;
#define glXGetCurrentDisplayEXT _funcptr_glXGetCurrentDisplayEXT
extern PFNGLXIMPORTCONTEXTEXTPROC _funcptr_glXImportContextEXT;
#define glXImportContextEXT _funcptr_glXImportContextEXT
extern PFNGLXQUERYCONTEXTINFOEXTPROC _funcptr_glXQueryContextInfoEXT;
#define glXQueryContextInfoEXT _funcptr_glXQueryContextInfoEXT

#define GLX_MAX_SWAP_INTERVAL_EXT 0x20F2
#define GLX_SWAP_INTERVAL_EXT 0x20F1

typedef void (CODEGEN_FUNCPTR * PFNGLXSWAPINTERVALEXTPROC)(Display * dpy, GLXDrawable drawable, int interval);

extern PFNGLXSWAPINTERVALEXTPROC _funcptr_glXSwapIntervalEXT;
#define glXSwapIntervalEXT _funcptr_glXSwapIntervalEXT

#define GLX_LATE_SWAPS_TEAR_EXT 0x20F3

#define GLX_AUX0_EXT 0x20E2
#define GLX_AUX1_EXT 0x20E3
#define GLX_AUX2_EXT 0x20E4
#define GLX_AUX3_EXT 0x20E5
#define GLX_AUX4_EXT 0x20E6
#define GLX_AUX5_EXT 0x20E7
#define GLX_AUX6_EXT 0x20E8
#define GLX_AUX7_EXT 0x20E9
#define GLX_AUX8_EXT 0x20EA
#define GLX_AUX9_EXT 0x20EB
#define GLX_BACK_EXT 0x20E0
#define GLX_BACK_LEFT_EXT 0x20E0
#define GLX_BACK_RIGHT_EXT 0x20E1
#define GLX_BIND_TO_MIPMAP_TEXTURE_EXT 0x20D2
#define GLX_BIND_TO_TEXTURE_RGBA_EXT 0x20D1
#define GLX_BIND_TO_TEXTURE_RGB_EXT 0x20D0
#define GLX_BIND_TO_TEXTURE_TARGETS_EXT 0x20D3
#define GLX_FRONT_EXT 0x20DE
#define GLX_FRONT_LEFT_EXT 0x20DE
#define GLX_FRONT_RIGHT_EXT 0x20DF
#define GLX_MIPMAP_TEXTURE_EXT 0x20D7
#define GLX_TEXTURE_1D_BIT_EXT 0x00000001
#define GLX_TEXTURE_1D_EXT 0x20DB
#define GLX_TEXTURE_2D_BIT_EXT 0x00000002
#define GLX_TEXTURE_2D_EXT 0x20DC
#define GLX_TEXTURE_FORMAT_EXT 0x20D5
#define GLX_TEXTURE_FORMAT_NONE_EXT 0x20D8
#define GLX_TEXTURE_FORMAT_RGBA_EXT 0x20DA
#define GLX_TEXTURE_FORMAT_RGB_EXT 0x20D9
#define GLX_TEXTURE_RECTANGLE_BIT_EXT 0x00000004
#define GLX_TEXTURE_RECTANGLE_EXT 0x20DD
#define GLX_TEXTURE_TARGET_EXT 0x20D6
#define GLX_Y_INVERTED_EXT 0x20D4

typedef void (CODEGEN_FUNCPTR * PFNGLXBINDTEXIMAGEEXTPROC)(Display * dpy, GLXDrawable drawable, int buffer, const int * attrib_list);
typedef void (CODEGEN_FUNCPTR * PFNGLXRELEASETEXIMAGEEXTPROC)(Display * dpy, GLXDrawable drawable, int buffer);

extern PFNGLXBINDTEXIMAGEEXTPROC _funcptr_glXBindTexImageEXT;
#define glXBindTexImageEXT _funcptr_glXBindTexImageEXT
extern PFNGLXRELEASETEXIMAGEEXTPROC _funcptr_glXReleaseTexImageEXT;
#define glXReleaseTexImageEXT _funcptr_glXReleaseTexImageEXT

#define GLX_DIRECT_COLOR_EXT 0x8003
#define GLX_GRAY_SCALE_EXT 0x8006
#define GLX_NONE_EXT 0x8000
#define GLX_PSEUDO_COLOR_EXT 0x8004
#define GLX_STATIC_COLOR_EXT 0x8005
#define GLX_STATIC_GRAY_EXT 0x8007
#define GLX_TRANSPARENT_ALPHA_VALUE_EXT 0x28
#define GLX_TRANSPARENT_BLUE_VALUE_EXT 0x27
#define GLX_TRANSPARENT_GREEN_VALUE_EXT 0x26
#define GLX_TRANSPARENT_INDEX_EXT 0x8009
#define GLX_TRANSPARENT_INDEX_VALUE_EXT 0x24
#define GLX_TRANSPARENT_RED_VALUE_EXT 0x25
#define GLX_TRANSPARENT_RGB_EXT 0x8008
#define GLX_TRANSPARENT_TYPE_EXT 0x23
#define GLX_TRUE_COLOR_EXT 0x8002
#define GLX_X_VISUAL_TYPE_EXT 0x22

#define GLX_NONE_EXT 0x8000
#define GLX_NON_CONFORMANT_VISUAL_EXT 0x800D
#define GLX_SLOW_VISUAL_EXT 0x8001
#define GLX_VISUAL_CAVEAT_EXT 0x20

#define GLX_BUFFER_SWAP_COMPLETE_INTEL_MASK 0x04000000
#define GLX_COPY_COMPLETE_INTEL 0x8181
#define GLX_EXCHANGE_COMPLETE_INTEL 0x8180
#define GLX_FLIP_COMPLETE_INTEL 0x8182

typedef unsigned int (CODEGEN_FUNCPTR * PFNGLXGETAGPOFFSETMESAPROC)(const void * pointer);

extern PFNGLXGETAGPOFFSETMESAPROC _funcptr_glXGetAGPOffsetMESA;
#define glXGetAGPOffsetMESA _funcptr_glXGetAGPOffsetMESA

typedef void (CODEGEN_FUNCPTR * PFNGLXCOPYSUBBUFFERMESAPROC)(Display * dpy, GLXDrawable drawable, int x, int y, int width, int height);

extern PFNGLXCOPYSUBBUFFERMESAPROC _funcptr_glXCopySubBufferMESA;
#define glXCopySubBufferMESA _funcptr_glXCopySubBufferMESA

typedef GLXPixmap (CODEGEN_FUNCPTR * PFNGLXCREATEGLXPIXMAPMESAPROC)(Display * dpy, XVisualInfo * visual, Pixmap pixmap, Colormap cmap);

extern PFNGLXCREATEGLXPIXMAPMESAPROC _funcptr_glXCreateGLXPixmapMESA;
#define glXCreateGLXPixmapMESA _funcptr_glXCreateGLXPixmapMESA

typedef Bool (CODEGEN_FUNCPTR * PFNGLXRELEASEBUFFERSMESAPROC)(Display * dpy, GLXDrawable drawable);

extern PFNGLXRELEASEBUFFERSMESAPROC _funcptr_glXReleaseBuffersMESA;
#define glXReleaseBuffersMESA _funcptr_glXReleaseBuffersMESA

#define GLX_3DFX_FULLSCREEN_MODE_MESA 0x2
#define GLX_3DFX_WINDOW_MODE_MESA 0x1

typedef Bool (CODEGEN_FUNCPTR * PFNGLXSET3DFXMODEMESAPROC)(int mode);

extern PFNGLXSET3DFXMODEMESAPROC _funcptr_glXSet3DfxModeMESA;
#define glXSet3DfxModeMESA _funcptr_glXSet3DfxModeMESA

typedef void (CODEGEN_FUNCPTR * PFNGLXCOPYIMAGESUBDATANVPROC)(Display * dpy, GLXContext srcCtx, GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLXContext dstCtx, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei width, GLsizei height, GLsizei depth);

extern PFNGLXCOPYIMAGESUBDATANVPROC _funcptr_glXCopyImageSubDataNV;
#define glXCopyImageSubDataNV _funcptr_glXCopyImageSubDataNV

#define GLX_FLOAT_COMPONENTS_NV 0x20B0

#define GLX_COLOR_SAMPLES_NV 0x20B3
#define GLX_COVERAGE_SAMPLES_NV 100001

#define GLX_NUM_VIDEO_SLOTS_NV 0x20F0

typedef int (CODEGEN_FUNCPTR * PFNGLXBINDVIDEODEVICENVPROC)(Display * dpy, unsigned int video_slot, unsigned int video_device, const int * attrib_list);
typedef unsigned int * (CODEGEN_FUNCPTR * PFNGLXENUMERATEVIDEODEVICESNVPROC)(Display * dpy, int screen, int * nelements);

extern PFNGLXBINDVIDEODEVICENVPROC _funcptr_glXBindVideoDeviceNV;
#define glXBindVideoDeviceNV _funcptr_glXBindVideoDeviceNV
extern PFNGLXENUMERATEVIDEODEVICESNVPROC _funcptr_glXEnumerateVideoDevicesNV;
#define glXEnumerateVideoDevicesNV _funcptr_glXEnumerateVideoDevicesNV

typedef Bool (CODEGEN_FUNCPTR * PFNGLXBINDSWAPBARRIERNVPROC)(Display * dpy, GLuint group, GLuint barrier);
typedef Bool (CODEGEN_FUNCPTR * PFNGLXJOINSWAPGROUPNVPROC)(Display * dpy, GLXDrawable drawable, GLuint group);
typedef Bool (CODEGEN_FUNCPTR * PFNGLXQUERYFRAMECOUNTNVPROC)(Display * dpy, int screen, GLuint * count);
typedef Bool (CODEGEN_FUNCPTR * PFNGLXQUERYMAXSWAPGROUPSNVPROC)(Display * dpy, int screen, GLuint * maxGroups, GLuint * maxBarriers);
typedef Bool (CODEGEN_FUNCPTR * PFNGLXQUERYSWAPGROUPNVPROC)(Display * dpy, GLXDrawable drawable, GLuint * group, GLuint * barrier);
typedef Bool (CODEGEN_FUNCPTR * PFNGLXRESETFRAMECOUNTNVPROC)(Display * dpy, int screen);

extern PFNGLXBINDSWAPBARRIERNVPROC _funcptr_glXBindSwapBarrierNV;
#define glXBindSwapBarrierNV _funcptr_glXBindSwapBarrierNV
extern PFNGLXJOINSWAPGROUPNVPROC _funcptr_glXJoinSwapGroupNV;
#define glXJoinSwapGroupNV _funcptr_glXJoinSwapGroupNV
extern PFNGLXQUERYFRAMECOUNTNVPROC _funcptr_glXQueryFrameCountNV;
#define glXQueryFrameCountNV _funcptr_glXQueryFrameCountNV
extern PFNGLXQUERYMAXSWAPGROUPSNVPROC _funcptr_glXQueryMaxSwapGroupsNV;
#define glXQueryMaxSwapGroupsNV _funcptr_glXQueryMaxSwapGroupsNV
extern PFNGLXQUERYSWAPGROUPNVPROC _funcptr_glXQuerySwapGroupNV;
#define glXQuerySwapGroupNV _funcptr_glXQuerySwapGroupNV
extern PFNGLXRESETFRAMECOUNTNVPROC _funcptr_glXResetFrameCountNV;
#define glXResetFrameCountNV _funcptr_glXResetFrameCountNV

#define GLX_DEVICE_ID_NV 0x20CD
#define GLX_NUM_VIDEO_CAPTURE_SLOTS_NV 0x20CF
#define GLX_UNIQUE_ID_NV 0x20CE

typedef int (CODEGEN_FUNCPTR * PFNGLXBINDVIDEOCAPTUREDEVICENVPROC)(Display * dpy, unsigned int video_capture_slot, GLXVideoCaptureDeviceNV device);
typedef GLXVideoCaptureDeviceNV * (CODEGEN_FUNCPTR * PFNGLXENUMERATEVIDEOCAPTUREDEVICESNVPROC)(Display * dpy, int screen, int * nelements);
typedef void (CODEGEN_FUNCPTR * PFNGLXLOCKVIDEOCAPTUREDEVICENVPROC)(Display * dpy, GLXVideoCaptureDeviceNV device);
typedef int (CODEGEN_FUNCPTR * PFNGLXQUERYVIDEOCAPTUREDEVICENVPROC)(Display * dpy, GLXVideoCaptureDeviceNV device, int attribute, int * value);
typedef void (CODEGEN_FUNCPTR * PFNGLXRELEASEVIDEOCAPTUREDEVICENVPROC)(Display * dpy, GLXVideoCaptureDeviceNV device);

extern PFNGLXBINDVIDEOCAPTUREDEVICENVPROC _funcptr_glXBindVideoCaptureDeviceNV;
#define glXBindVideoCaptureDeviceNV _funcptr_glXBindVideoCaptureDeviceNV
extern PFNGLXENUMERATEVIDEOCAPTUREDEVICESNVPROC _funcptr_glXEnumerateVideoCaptureDevicesNV;
#define glXEnumerateVideoCaptureDevicesNV _funcptr_glXEnumerateVideoCaptureDevicesNV
extern PFNGLXLOCKVIDEOCAPTUREDEVICENVPROC _funcptr_glXLockVideoCaptureDeviceNV;
#define glXLockVideoCaptureDeviceNV _funcptr_glXLockVideoCaptureDeviceNV
extern PFNGLXQUERYVIDEOCAPTUREDEVICENVPROC _funcptr_glXQueryVideoCaptureDeviceNV;
#define glXQueryVideoCaptureDeviceNV _funcptr_glXQueryVideoCaptureDeviceNV
extern PFNGLXRELEASEVIDEOCAPTUREDEVICENVPROC _funcptr_glXReleaseVideoCaptureDeviceNV;
#define glXReleaseVideoCaptureDeviceNV _funcptr_glXReleaseVideoCaptureDeviceNV

#define GLX_VIDEO_OUT_ALPHA_NV 0x20C4
#define GLX_VIDEO_OUT_COLOR_AND_ALPHA_NV 0x20C6
#define GLX_VIDEO_OUT_COLOR_AND_DEPTH_NV 0x20C7
#define GLX_VIDEO_OUT_COLOR_NV 0x20C3
#define GLX_VIDEO_OUT_DEPTH_NV 0x20C5
#define GLX_VIDEO_OUT_FIELD_1_NV 0x20C9
#define GLX_VIDEO_OUT_FIELD_2_NV 0x20CA
#define GLX_VIDEO_OUT_FRAME_NV 0x20C8
#define GLX_VIDEO_OUT_STACKED_FIELDS_1_2_NV 0x20CB
#define GLX_VIDEO_OUT_STACKED_FIELDS_2_1_NV 0x20CC

typedef int (CODEGEN_FUNCPTR * PFNGLXBINDVIDEOIMAGENVPROC)(Display * dpy, GLXVideoDeviceNV VideoDevice, GLXPbuffer pbuf, int iVideoBuffer);
typedef int (CODEGEN_FUNCPTR * PFNGLXGETVIDEODEVICENVPROC)(Display * dpy, int screen, int numVideoDevices, GLXVideoDeviceNV * pVideoDevice);
typedef int (CODEGEN_FUNCPTR * PFNGLXGETVIDEOINFONVPROC)(Display * dpy, int screen, GLXVideoDeviceNV VideoDevice, unsigned long * pulCounterOutputPbuffer, unsigned long * pulCounterOutputVideo);
typedef int (CODEGEN_FUNCPTR * PFNGLXRELEASEVIDEODEVICENVPROC)(Display * dpy, int screen, GLXVideoDeviceNV VideoDevice);
typedef int (CODEGEN_FUNCPTR * PFNGLXRELEASEVIDEOIMAGENVPROC)(Display * dpy, GLXPbuffer pbuf);
typedef int (CODEGEN_FUNCPTR * PFNGLXSENDPBUFFERTOVIDEONVPROC)(Display * dpy, GLXPbuffer pbuf, int iBufferType, unsigned long * pulCounterPbuffer, GLboolean bBlock);

extern PFNGLXBINDVIDEOIMAGENVPROC _funcptr_glXBindVideoImageNV;
#define glXBindVideoImageNV _funcptr_glXBindVideoImageNV
extern PFNGLXGETVIDEODEVICENVPROC _funcptr_glXGetVideoDeviceNV;
#define glXGetVideoDeviceNV _funcptr_glXGetVideoDeviceNV
extern PFNGLXGETVIDEOINFONVPROC _funcptr_glXGetVideoInfoNV;
#define glXGetVideoInfoNV _funcptr_glXGetVideoInfoNV
extern PFNGLXRELEASEVIDEODEVICENVPROC _funcptr_glXReleaseVideoDeviceNV;
#define glXReleaseVideoDeviceNV _funcptr_glXReleaseVideoDeviceNV
extern PFNGLXRELEASEVIDEOIMAGENVPROC _funcptr_glXReleaseVideoImageNV;
#define glXReleaseVideoImageNV _funcptr_glXReleaseVideoImageNV
extern PFNGLXSENDPBUFFERTOVIDEONVPROC _funcptr_glXSendPbufferToVideoNV;
#define glXSendPbufferToVideoNV _funcptr_glXSendPbufferToVideoNV

#define GLX_SWAP_COPY_OML 0x8062
#define GLX_SWAP_EXCHANGE_OML 0x8061
#define GLX_SWAP_METHOD_OML 0x8060
#define GLX_SWAP_UNDEFINED_OML 0x8063

typedef Bool (CODEGEN_FUNCPTR * PFNGLXGETMSCRATEOMLPROC)(Display * dpy, GLXDrawable drawable, int32_t * numerator, int32_t * denominator);
typedef Bool (CODEGEN_FUNCPTR * PFNGLXGETSYNCVALUESOMLPROC)(Display * dpy, GLXDrawable drawable, int64_t * ust, int64_t * msc, int64_t * sbc);
typedef int64_t (CODEGEN_FUNCPTR * PFNGLXSWAPBUFFERSMSCOMLPROC)(Display * dpy, GLXDrawable drawable, int64_t target_msc, int64_t divisor, int64_t remainder);
typedef Bool (CODEGEN_FUNCPTR * PFNGLXWAITFORMSCOMLPROC)(Display * dpy, GLXDrawable drawable, int64_t target_msc, int64_t divisor, int64_t remainder, int64_t * ust, int64_t * msc, int64_t * sbc);
typedef Bool (CODEGEN_FUNCPTR * PFNGLXWAITFORSBCOMLPROC)(Display * dpy, GLXDrawable drawable, int64_t target_sbc, int64_t * ust, int64_t * msc, int64_t * sbc);

extern PFNGLXGETMSCRATEOMLPROC _funcptr_glXGetMscRateOML;
#define glXGetMscRateOML _funcptr_glXGetMscRateOML
extern PFNGLXGETSYNCVALUESOMLPROC _funcptr_glXGetSyncValuesOML;
#define glXGetSyncValuesOML _funcptr_glXGetSyncValuesOML
extern PFNGLXSWAPBUFFERSMSCOMLPROC _funcptr_glXSwapBuffersMscOML;
#define glXSwapBuffersMscOML _funcptr_glXSwapBuffersMscOML
extern PFNGLXWAITFORMSCOMLPROC _funcptr_glXWaitForMscOML;
#define glXWaitForMscOML _funcptr_glXWaitForMscOML
extern PFNGLXWAITFORSBCOMLPROC _funcptr_glXWaitForSbcOML;
#define glXWaitForSbcOML _funcptr_glXWaitForSbcOML

#define GLX_BLENDED_RGBA_SGIS 0x8025

#define GLX_SAMPLES_SGIS 100001
#define GLX_SAMPLE_BUFFERS_SGIS 100000

#define GLX_MULTISAMPLE_SUB_RECT_HEIGHT_SGIS 0x8027
#define GLX_MULTISAMPLE_SUB_RECT_WIDTH_SGIS 0x8026

#define GLX_COLOR_INDEX_BIT_SGIX 0x00000002
#define GLX_COLOR_INDEX_TYPE_SGIX 0x8015
#define GLX_DRAWABLE_TYPE_SGIX 0x8010
#define GLX_FBCONFIG_ID_SGIX 0x8013
#define GLX_PIXMAP_BIT_SGIX 0x00000002
#define GLX_RENDER_TYPE_SGIX 0x8011
#define GLX_RGBA_BIT_SGIX 0x00000001
#define GLX_RGBA_TYPE_SGIX 0x8014
#define GLX_SCREEN_EXT 0x800C
#define GLX_WINDOW_BIT_SGIX 0x00000001
#define GLX_X_RENDERABLE_SGIX 0x8012

typedef GLXFBConfigSGIX * (CODEGEN_FUNCPTR * PFNGLXCHOOSEFBCONFIGSGIXPROC)(Display * dpy, int screen, int * attrib_list, int * nelements);
typedef GLXContext (CODEGEN_FUNCPTR * PFNGLXCREATECONTEXTWITHCONFIGSGIXPROC)(Display * dpy, GLXFBConfigSGIX config, int render_type, GLXContext share_list, Bool direct);
typedef GLXPixmap (CODEGEN_FUNCPTR * PFNGLXCREATEGLXPIXMAPWITHCONFIGSGIXPROC)(Display * dpy, GLXFBConfigSGIX config, Pixmap pixmap);
typedef int (CODEGEN_FUNCPTR * PFNGLXGETFBCONFIGATTRIBSGIXPROC)(Display * dpy, GLXFBConfigSGIX config, int attribute, int * value);
typedef GLXFBConfigSGIX (CODEGEN_FUNCPTR * PFNGLXGETFBCONFIGFROMVISUALSGIXPROC)(Display * dpy, XVisualInfo * vis);
typedef XVisualInfo * (CODEGEN_FUNCPTR * PFNGLXGETVISUALFROMFBCONFIGSGIXPROC)(Display * dpy, GLXFBConfigSGIX config);

extern PFNGLXCHOOSEFBCONFIGSGIXPROC _funcptr_glXChooseFBConfigSGIX;
#define glXChooseFBConfigSGIX _funcptr_glXChooseFBConfigSGIX
extern PFNGLXCREATECONTEXTWITHCONFIGSGIXPROC _funcptr_glXCreateContextWithConfigSGIX;
#define glXCreateContextWithConfigSGIX _funcptr_glXCreateContextWithConfigSGIX
extern PFNGLXCREATEGLXPIXMAPWITHCONFIGSGIXPROC _funcptr_glXCreateGLXPixmapWithConfigSGIX;
#define glXCreateGLXPixmapWithConfigSGIX _funcptr_glXCreateGLXPixmapWithConfigSGIX
extern PFNGLXGETFBCONFIGATTRIBSGIXPROC _funcptr_glXGetFBConfigAttribSGIX;
#define glXGetFBConfigAttribSGIX _funcptr_glXGetFBConfigAttribSGIX
extern PFNGLXGETFBCONFIGFROMVISUALSGIXPROC _funcptr_glXGetFBConfigFromVisualSGIX;
#define glXGetFBConfigFromVisualSGIX _funcptr_glXGetFBConfigFromVisualSGIX
extern PFNGLXGETVISUALFROMFBCONFIGSGIXPROC _funcptr_glXGetVisualFromFBConfigSGIX;
#define glXGetVisualFromFBConfigSGIX _funcptr_glXGetVisualFromFBConfigSGIX

#define GLX_ACCUM_BUFFER_BIT_SGIX 0x00000080
#define GLX_AUX_BUFFERS_BIT_SGIX 0x00000010
#define GLX_BACK_LEFT_BUFFER_BIT_SGIX 0x00000004
#define GLX_BACK_RIGHT_BUFFER_BIT_SGIX 0x00000008
#define GLX_BUFFER_CLOBBER_MASK_SGIX 0x08000000
#define GLX_DAMAGED_SGIX 0x8020
#define GLX_DEPTH_BUFFER_BIT_SGIX 0x00000020
#define GLX_EVENT_MASK_SGIX 0x801F
#define GLX_FRONT_LEFT_BUFFER_BIT_SGIX 0x00000001
#define GLX_FRONT_RIGHT_BUFFER_BIT_SGIX 0x00000002
#define GLX_HEIGHT_SGIX 0x801E
#define GLX_LARGEST_PBUFFER_SGIX 0x801C
#define GLX_MAX_PBUFFER_HEIGHT_SGIX 0x8017
#define GLX_MAX_PBUFFER_PIXELS_SGIX 0x8018
#define GLX_MAX_PBUFFER_WIDTH_SGIX 0x8016
#define GLX_OPTIMAL_PBUFFER_HEIGHT_SGIX 0x801A
#define GLX_OPTIMAL_PBUFFER_WIDTH_SGIX 0x8019
#define GLX_PBUFFER_BIT_SGIX 0x00000004
#define GLX_PBUFFER_SGIX 0x8023
#define GLX_PRESERVED_CONTENTS_SGIX 0x801B
#define GLX_SAMPLE_BUFFERS_BIT_SGIX 0x00000100
#define GLX_SAVED_SGIX 0x8021
#define GLX_STENCIL_BUFFER_BIT_SGIX 0x00000040
#define GLX_WIDTH_SGIX 0x801D
#define GLX_WINDOW_SGIX 0x8022

typedef GLXPbufferSGIX (CODEGEN_FUNCPTR * PFNGLXCREATEGLXPBUFFERSGIXPROC)(Display * dpy, GLXFBConfigSGIX config, unsigned int width, unsigned int height, int * attrib_list);
typedef void (CODEGEN_FUNCPTR * PFNGLXDESTROYGLXPBUFFERSGIXPROC)(Display * dpy, GLXPbufferSGIX pbuf);
typedef void (CODEGEN_FUNCPTR * PFNGLXGETSELECTEDEVENTSGIXPROC)(Display * dpy, GLXDrawable drawable, unsigned long * mask);
typedef int (CODEGEN_FUNCPTR * PFNGLXQUERYGLXPBUFFERSGIXPROC)(Display * dpy, GLXPbufferSGIX pbuf, int attribute, unsigned int * value);
typedef void (CODEGEN_FUNCPTR * PFNGLXSELECTEVENTSGIXPROC)(Display * dpy, GLXDrawable drawable, unsigned long mask);

extern PFNGLXCREATEGLXPBUFFERSGIXPROC _funcptr_glXCreateGLXPbufferSGIX;
#define glXCreateGLXPbufferSGIX _funcptr_glXCreateGLXPbufferSGIX
extern PFNGLXDESTROYGLXPBUFFERSGIXPROC _funcptr_glXDestroyGLXPbufferSGIX;
#define glXDestroyGLXPbufferSGIX _funcptr_glXDestroyGLXPbufferSGIX
extern PFNGLXGETSELECTEDEVENTSGIXPROC _funcptr_glXGetSelectedEventSGIX;
#define glXGetSelectedEventSGIX _funcptr_glXGetSelectedEventSGIX
extern PFNGLXQUERYGLXPBUFFERSGIXPROC _funcptr_glXQueryGLXPbufferSGIX;
#define glXQueryGLXPbufferSGIX _funcptr_glXQueryGLXPbufferSGIX
extern PFNGLXSELECTEVENTSGIXPROC _funcptr_glXSelectEventSGIX;
#define glXSelectEventSGIX _funcptr_glXSelectEventSGIX

typedef void (CODEGEN_FUNCPTR * PFNGLXBINDSWAPBARRIERSGIXPROC)(Display * dpy, GLXDrawable drawable, int barrier);
typedef Bool (CODEGEN_FUNCPTR * PFNGLXQUERYMAXSWAPBARRIERSSGIXPROC)(Display * dpy, int screen, int * max);

extern PFNGLXBINDSWAPBARRIERSGIXPROC _funcptr_glXBindSwapBarrierSGIX;
#define glXBindSwapBarrierSGIX _funcptr_glXBindSwapBarrierSGIX
extern PFNGLXQUERYMAXSWAPBARRIERSSGIXPROC _funcptr_glXQueryMaxSwapBarriersSGIX;
#define glXQueryMaxSwapBarriersSGIX _funcptr_glXQueryMaxSwapBarriersSGIX

typedef void (CODEGEN_FUNCPTR * PFNGLXJOINSWAPGROUPSGIXPROC)(Display * dpy, GLXDrawable drawable, GLXDrawable member);

extern PFNGLXJOINSWAPGROUPSGIXPROC _funcptr_glXJoinSwapGroupSGIX;
#define glXJoinSwapGroupSGIX _funcptr_glXJoinSwapGroupSGIX

#define GLX_SYNC_FRAME_SGIX 0x00000000
#define GLX_SYNC_SWAP_SGIX 0x00000001

typedef int (CODEGEN_FUNCPTR * PFNGLXBINDCHANNELTOWINDOWSGIXPROC)(Display * display, int screen, int channel, Window window);
typedef int (CODEGEN_FUNCPTR * PFNGLXCHANNELRECTSGIXPROC)(Display * display, int screen, int channel, int x, int y, int w, int h);
typedef int (CODEGEN_FUNCPTR * PFNGLXCHANNELRECTSYNCSGIXPROC)(Display * display, int screen, int channel, GLenum synctype);
typedef int (CODEGEN_FUNCPTR * PFNGLXQUERYCHANNELDELTASSGIXPROC)(Display * display, int screen, int channel, int * x, int * y, int * w, int * h);
typedef int (CODEGEN_FUNCPTR * PFNGLXQUERYCHANNELRECTSGIXPROC)(Display * display, int screen, int channel, int * dx, int * dy, int * dw, int * dh);

extern PFNGLXBINDCHANNELTOWINDOWSGIXPROC _funcptr_glXBindChannelToWindowSGIX;
#define glXBindChannelToWindowSGIX _funcptr_glXBindChannelToWindowSGIX
extern PFNGLXCHANNELRECTSGIXPROC _funcptr_glXChannelRectSGIX;
#define glXChannelRectSGIX _funcptr_glXChannelRectSGIX
extern PFNGLXCHANNELRECTSYNCSGIXPROC _funcptr_glXChannelRectSyncSGIX;
#define glXChannelRectSyncSGIX _funcptr_glXChannelRectSyncSGIX
extern PFNGLXQUERYCHANNELDELTASSGIXPROC _funcptr_glXQueryChannelDeltasSGIX;
#define glXQueryChannelDeltasSGIX _funcptr_glXQueryChannelDeltasSGIX
extern PFNGLXQUERYCHANNELRECTSGIXPROC _funcptr_glXQueryChannelRectSGIX;
#define glXQueryChannelRectSGIX _funcptr_glXQueryChannelRectSGIX

#define GLX_VISUAL_SELECT_GROUP_SGIX 0x8028

typedef void (CODEGEN_FUNCPTR * PFNGLXCUSHIONSGIPROC)(Display * dpy, Window window, float cushion);

extern PFNGLXCUSHIONSGIPROC _funcptr_glXCushionSGI;
#define glXCushionSGI _funcptr_glXCushionSGI

typedef GLXDrawable (CODEGEN_FUNCPTR * PFNGLXGETCURRENTREADDRAWABLESGIPROC)();
typedef Bool (CODEGEN_FUNCPTR * PFNGLXMAKECURRENTREADSGIPROC)(Display * dpy, GLXDrawable draw, GLXDrawable read, GLXContext ctx);

extern PFNGLXGETCURRENTREADDRAWABLESGIPROC _funcptr_glXGetCurrentReadDrawableSGI;
#define glXGetCurrentReadDrawableSGI _funcptr_glXGetCurrentReadDrawableSGI
extern PFNGLXMAKECURRENTREADSGIPROC _funcptr_glXMakeCurrentReadSGI;
#define glXMakeCurrentReadSGI _funcptr_glXMakeCurrentReadSGI

typedef int (CODEGEN_FUNCPTR * PFNGLXSWAPINTERVALSGIPROC)(int interval);

extern PFNGLXSWAPINTERVALSGIPROC _funcptr_glXSwapIntervalSGI;
#define glXSwapIntervalSGI _funcptr_glXSwapIntervalSGI

typedef int (CODEGEN_FUNCPTR * PFNGLXGETVIDEOSYNCSGIPROC)(unsigned int * count);
typedef int (CODEGEN_FUNCPTR * PFNGLXWAITVIDEOSYNCSGIPROC)(int divisor, int remainder, unsigned int * count);

extern PFNGLXGETVIDEOSYNCSGIPROC _funcptr_glXGetVideoSyncSGI;
#define glXGetVideoSyncSGI _funcptr_glXGetVideoSyncSGI
extern PFNGLXWAITVIDEOSYNCSGIPROC _funcptr_glXWaitVideoSyncSGI;
#define glXWaitVideoSyncSGI _funcptr_glXWaitVideoSyncSGI

typedef Status (CODEGEN_FUNCPTR * PFNGLXGETTRANSPARENTINDEXSUNPROC)(Display * dpy, Window overlay, Window underlay, long * pTransparentIndex);

extern PFNGLXGETTRANSPARENTINDEXSUNPROC _funcptr_glXGetTransparentIndexSUN;
#define glXGetTransparentIndexSUN _funcptr_glXGetTransparentIndexSUN

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*GLXWIN_GEN_EXTENSIONS_H*/
