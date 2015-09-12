#include <algorithm>
#include <vector>
#include <string.h>
#include "glload/glx_all.hpp"
#include "glload/glx_load.hpp"

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

extern glX::_detail::Proc_glXCreateContextAttribsARB _funcptr_glXCreateContextAttribsARB;
extern glX::_detail::Proc_glXGetProcAddressARB _funcptr_glXGetProcAddressARB;
extern glX::_detail::Proc_glXFreeContextEXT _funcptr_glXFreeContextEXT;
extern glX::_detail::Proc_glXGetContextIDEXT _funcptr_glXGetContextIDEXT;
extern glX::_detail::Proc_glXGetCurrentDisplayEXT _funcptr_glXGetCurrentDisplayEXT;
extern glX::_detail::Proc_glXImportContextEXT _funcptr_glXImportContextEXT;
extern glX::_detail::Proc_glXQueryContextInfoEXT _funcptr_glXQueryContextInfoEXT;
extern glX::_detail::Proc_glXSwapIntervalEXT _funcptr_glXSwapIntervalEXT;
extern glX::_detail::Proc_glXBindTexImageEXT _funcptr_glXBindTexImageEXT;
extern glX::_detail::Proc_glXReleaseTexImageEXT _funcptr_glXReleaseTexImageEXT;
extern glX::_detail::Proc_glXGetAGPOffsetMESA _funcptr_glXGetAGPOffsetMESA;
extern glX::_detail::Proc_glXCopySubBufferMESA _funcptr_glXCopySubBufferMESA;
extern glX::_detail::Proc_glXCreateGLXPixmapMESA _funcptr_glXCreateGLXPixmapMESA;
extern glX::_detail::Proc_glXReleaseBuffersMESA _funcptr_glXReleaseBuffersMESA;
extern glX::_detail::Proc_glXSet3DfxModeMESA _funcptr_glXSet3DfxModeMESA;
extern glX::_detail::Proc_glXCopyImageSubDataNV _funcptr_glXCopyImageSubDataNV;
extern glX::_detail::Proc_glXBindVideoDeviceNV _funcptr_glXBindVideoDeviceNV;
extern glX::_detail::Proc_glXEnumerateVideoDevicesNV _funcptr_glXEnumerateVideoDevicesNV;
extern glX::_detail::Proc_glXBindSwapBarrierNV _funcptr_glXBindSwapBarrierNV;
extern glX::_detail::Proc_glXJoinSwapGroupNV _funcptr_glXJoinSwapGroupNV;
extern glX::_detail::Proc_glXQueryFrameCountNV _funcptr_glXQueryFrameCountNV;
extern glX::_detail::Proc_glXQueryMaxSwapGroupsNV _funcptr_glXQueryMaxSwapGroupsNV;
extern glX::_detail::Proc_glXQuerySwapGroupNV _funcptr_glXQuerySwapGroupNV;
extern glX::_detail::Proc_glXResetFrameCountNV _funcptr_glXResetFrameCountNV;
extern glX::_detail::Proc_glXBindVideoCaptureDeviceNV _funcptr_glXBindVideoCaptureDeviceNV;
extern glX::_detail::Proc_glXEnumerateVideoCaptureDevicesNV _funcptr_glXEnumerateVideoCaptureDevicesNV;
extern glX::_detail::Proc_glXLockVideoCaptureDeviceNV _funcptr_glXLockVideoCaptureDeviceNV;
extern glX::_detail::Proc_glXQueryVideoCaptureDeviceNV _funcptr_glXQueryVideoCaptureDeviceNV;
extern glX::_detail::Proc_glXReleaseVideoCaptureDeviceNV _funcptr_glXReleaseVideoCaptureDeviceNV;
extern glX::_detail::Proc_glXBindVideoImageNV _funcptr_glXBindVideoImageNV;
extern glX::_detail::Proc_glXGetVideoDeviceNV _funcptr_glXGetVideoDeviceNV;
extern glX::_detail::Proc_glXGetVideoInfoNV _funcptr_glXGetVideoInfoNV;
extern glX::_detail::Proc_glXReleaseVideoDeviceNV _funcptr_glXReleaseVideoDeviceNV;
extern glX::_detail::Proc_glXReleaseVideoImageNV _funcptr_glXReleaseVideoImageNV;
extern glX::_detail::Proc_glXSendPbufferToVideoNV _funcptr_glXSendPbufferToVideoNV;
extern glX::_detail::Proc_glXGetMscRateOML _funcptr_glXGetMscRateOML;
extern glX::_detail::Proc_glXGetSyncValuesOML _funcptr_glXGetSyncValuesOML;
extern glX::_detail::Proc_glXSwapBuffersMscOML _funcptr_glXSwapBuffersMscOML;
extern glX::_detail::Proc_glXWaitForMscOML _funcptr_glXWaitForMscOML;
extern glX::_detail::Proc_glXWaitForSbcOML _funcptr_glXWaitForSbcOML;
extern glX::_detail::Proc_glXChooseFBConfigSGIX _funcptr_glXChooseFBConfigSGIX;
extern glX::_detail::Proc_glXCreateContextWithConfigSGIX _funcptr_glXCreateContextWithConfigSGIX;
extern glX::_detail::Proc_glXCreateGLXPixmapWithConfigSGIX _funcptr_glXCreateGLXPixmapWithConfigSGIX;
extern glX::_detail::Proc_glXGetFBConfigAttribSGIX _funcptr_glXGetFBConfigAttribSGIX;
extern glX::_detail::Proc_glXGetFBConfigFromVisualSGIX _funcptr_glXGetFBConfigFromVisualSGIX;
extern glX::_detail::Proc_glXGetVisualFromFBConfigSGIX _funcptr_glXGetVisualFromFBConfigSGIX;
extern glX::_detail::Proc_glXCreateGLXPbufferSGIX _funcptr_glXCreateGLXPbufferSGIX;
extern glX::_detail::Proc_glXDestroyGLXPbufferSGIX _funcptr_glXDestroyGLXPbufferSGIX;
extern glX::_detail::Proc_glXGetSelectedEventSGIX _funcptr_glXGetSelectedEventSGIX;
extern glX::_detail::Proc_glXQueryGLXPbufferSGIX _funcptr_glXQueryGLXPbufferSGIX;
extern glX::_detail::Proc_glXSelectEventSGIX _funcptr_glXSelectEventSGIX;
extern glX::_detail::Proc_glXBindSwapBarrierSGIX _funcptr_glXBindSwapBarrierSGIX;
extern glX::_detail::Proc_glXQueryMaxSwapBarriersSGIX _funcptr_glXQueryMaxSwapBarriersSGIX;
extern glX::_detail::Proc_glXJoinSwapGroupSGIX _funcptr_glXJoinSwapGroupSGIX;
extern glX::_detail::Proc_glXBindChannelToWindowSGIX _funcptr_glXBindChannelToWindowSGIX;
extern glX::_detail::Proc_glXChannelRectSGIX _funcptr_glXChannelRectSGIX;
extern glX::_detail::Proc_glXChannelRectSyncSGIX _funcptr_glXChannelRectSyncSGIX;
extern glX::_detail::Proc_glXQueryChannelDeltasSGIX _funcptr_glXQueryChannelDeltasSGIX;
extern glX::_detail::Proc_glXQueryChannelRectSGIX _funcptr_glXQueryChannelRectSGIX;
extern glX::_detail::Proc_glXCushionSGI _funcptr_glXCushionSGI;
extern glX::_detail::Proc_glXGetCurrentReadDrawableSGI _funcptr_glXGetCurrentReadDrawableSGI;
extern glX::_detail::Proc_glXMakeCurrentReadSGI _funcptr_glXMakeCurrentReadSGI;
extern glX::_detail::Proc_glXSwapIntervalSGI _funcptr_glXSwapIntervalSGI;
extern glX::_detail::Proc_glXGetVideoSyncSGI _funcptr_glXGetVideoSyncSGI;
extern glX::_detail::Proc_glXWaitVideoSyncSGI _funcptr_glXWaitVideoSyncSGI;
extern glX::_detail::Proc_glXGetTransparentIndexSUN _funcptr_glXGetTransparentIndexSUN;

int glx_LoadFunctions(Display *display, int screen);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

namespace glX
{
	namespace exts
	{
		glload::LoadTest var_3DFX_multisample;
		glload::LoadTest var_AMD_gpu_association;
		glload::LoadTest var_ARB_create_context;
		glload::LoadTest var_ARB_create_context_profile;
		glload::LoadTest var_ARB_create_context_robustness;
		glload::LoadTest var_ARB_fbconfig_float;
		glload::LoadTest var_ARB_framebuffer_sRGB;
		glload::LoadTest var_ARB_get_proc_address;
		glload::LoadTest var_ARB_multisample;
		glload::LoadTest var_ARB_robustness_application_isolation;
		glload::LoadTest var_ARB_robustness_share_group_isolation;
		glload::LoadTest var_ARB_vertex_buffer_object;
		glload::LoadTest var_EXT_buffer_age;
		glload::LoadTest var_EXT_create_context_es2_profile;
		glload::LoadTest var_EXT_create_context_es_profile;
		glload::LoadTest var_EXT_fbconfig_packed_float;
		glload::LoadTest var_EXT_framebuffer_sRGB;
		glload::LoadTest var_EXT_import_context;
		glload::LoadTest var_EXT_swap_control;
		glload::LoadTest var_EXT_swap_control_tear;
		glload::LoadTest var_EXT_texture_from_pixmap;
		glload::LoadTest var_EXT_visual_info;
		glload::LoadTest var_EXT_visual_rating;
		glload::LoadTest var_INTEL_swap_event;
		glload::LoadTest var_MESA_agp_offset;
		glload::LoadTest var_MESA_copy_sub_buffer;
		glload::LoadTest var_MESA_pixmap_colormap;
		glload::LoadTest var_MESA_release_buffers;
		glload::LoadTest var_MESA_set_3dfx_mode;
		glload::LoadTest var_NV_copy_image;
		glload::LoadTest var_NV_float_buffer;
		glload::LoadTest var_NV_multisample_coverage;
		glload::LoadTest var_NV_present_video;
		glload::LoadTest var_NV_swap_group;
		glload::LoadTest var_NV_video_capture;
		glload::LoadTest var_NV_video_output;
		glload::LoadTest var_OML_swap_method;
		glload::LoadTest var_OML_sync_control;
		glload::LoadTest var_SGIS_blended_overlay;
		glload::LoadTest var_SGIS_multisample;
		glload::LoadTest var_SGIS_shared_multisample;
		glload::LoadTest var_SGIX_fbconfig;
		glload::LoadTest var_SGIX_pbuffer;
		glload::LoadTest var_SGIX_swap_barrier;
		glload::LoadTest var_SGIX_swap_group;
		glload::LoadTest var_SGIX_video_resize;
		glload::LoadTest var_SGIX_visual_select_group;
		glload::LoadTest var_SGI_cushion;
		glload::LoadTest var_SGI_make_current_read;
		glload::LoadTest var_SGI_swap_control;
		glload::LoadTest var_SGI_video_sync;
		glload::LoadTest var_SUN_get_transparent_index;
	}
	
	_detail::Proc_glXCreateContextAttribsARB CreateContextAttribsARB = 0;
	
	_detail::Proc_glXGetProcAddressARB GetProcAddressARB = 0;
	
	_detail::Proc_glXFreeContextEXT FreeContextEXT = 0;
	_detail::Proc_glXGetContextIDEXT GetContextIDEXT = 0;
	_detail::Proc_glXGetCurrentDisplayEXT GetCurrentDisplayEXT = 0;
	_detail::Proc_glXImportContextEXT ImportContextEXT = 0;
	_detail::Proc_glXQueryContextInfoEXT QueryContextInfoEXT = 0;
	
	_detail::Proc_glXSwapIntervalEXT SwapIntervalEXT = 0;
	
	_detail::Proc_glXBindTexImageEXT BindTexImageEXT = 0;
	_detail::Proc_glXReleaseTexImageEXT ReleaseTexImageEXT = 0;
	
	_detail::Proc_glXGetAGPOffsetMESA GetAGPOffsetMESA = 0;
	
	_detail::Proc_glXCopySubBufferMESA CopySubBufferMESA = 0;
	
	_detail::Proc_glXCreateGLXPixmapMESA CreateGLXPixmapMESA = 0;
	
	_detail::Proc_glXReleaseBuffersMESA ReleaseBuffersMESA = 0;
	
	_detail::Proc_glXSet3DfxModeMESA Set3DfxModeMESA = 0;
	
	_detail::Proc_glXCopyImageSubDataNV CopyImageSubDataNV = 0;
	
	_detail::Proc_glXBindVideoDeviceNV BindVideoDeviceNV = 0;
	_detail::Proc_glXEnumerateVideoDevicesNV EnumerateVideoDevicesNV = 0;
	
	_detail::Proc_glXBindSwapBarrierNV BindSwapBarrierNV = 0;
	_detail::Proc_glXJoinSwapGroupNV JoinSwapGroupNV = 0;
	_detail::Proc_glXQueryFrameCountNV QueryFrameCountNV = 0;
	_detail::Proc_glXQueryMaxSwapGroupsNV QueryMaxSwapGroupsNV = 0;
	_detail::Proc_glXQuerySwapGroupNV QuerySwapGroupNV = 0;
	_detail::Proc_glXResetFrameCountNV ResetFrameCountNV = 0;
	
	_detail::Proc_glXBindVideoCaptureDeviceNV BindVideoCaptureDeviceNV = 0;
	_detail::Proc_glXEnumerateVideoCaptureDevicesNV EnumerateVideoCaptureDevicesNV = 0;
	_detail::Proc_glXLockVideoCaptureDeviceNV LockVideoCaptureDeviceNV = 0;
	_detail::Proc_glXQueryVideoCaptureDeviceNV QueryVideoCaptureDeviceNV = 0;
	_detail::Proc_glXReleaseVideoCaptureDeviceNV ReleaseVideoCaptureDeviceNV = 0;
	
	_detail::Proc_glXBindVideoImageNV BindVideoImageNV = 0;
	_detail::Proc_glXGetVideoDeviceNV GetVideoDeviceNV = 0;
	_detail::Proc_glXGetVideoInfoNV GetVideoInfoNV = 0;
	_detail::Proc_glXReleaseVideoDeviceNV ReleaseVideoDeviceNV = 0;
	_detail::Proc_glXReleaseVideoImageNV ReleaseVideoImageNV = 0;
	_detail::Proc_glXSendPbufferToVideoNV SendPbufferToVideoNV = 0;
	
	_detail::Proc_glXGetMscRateOML GetMscRateOML = 0;
	_detail::Proc_glXGetSyncValuesOML GetSyncValuesOML = 0;
	_detail::Proc_glXSwapBuffersMscOML SwapBuffersMscOML = 0;
	_detail::Proc_glXWaitForMscOML WaitForMscOML = 0;
	_detail::Proc_glXWaitForSbcOML WaitForSbcOML = 0;
	
	_detail::Proc_glXChooseFBConfigSGIX ChooseFBConfigSGIX = 0;
	_detail::Proc_glXCreateContextWithConfigSGIX CreateContextWithConfigSGIX = 0;
	_detail::Proc_glXCreateGLXPixmapWithConfigSGIX CreateGLXPixmapWithConfigSGIX = 0;
	_detail::Proc_glXGetFBConfigAttribSGIX GetFBConfigAttribSGIX = 0;
	_detail::Proc_glXGetFBConfigFromVisualSGIX GetFBConfigFromVisualSGIX = 0;
	_detail::Proc_glXGetVisualFromFBConfigSGIX GetVisualFromFBConfigSGIX = 0;
	
	_detail::Proc_glXCreateGLXPbufferSGIX CreateGLXPbufferSGIX = 0;
	_detail::Proc_glXDestroyGLXPbufferSGIX DestroyGLXPbufferSGIX = 0;
	_detail::Proc_glXGetSelectedEventSGIX GetSelectedEventSGIX = 0;
	_detail::Proc_glXQueryGLXPbufferSGIX QueryGLXPbufferSGIX = 0;
	_detail::Proc_glXSelectEventSGIX SelectEventSGIX = 0;
	
	_detail::Proc_glXBindSwapBarrierSGIX BindSwapBarrierSGIX = 0;
	_detail::Proc_glXQueryMaxSwapBarriersSGIX QueryMaxSwapBarriersSGIX = 0;
	
	_detail::Proc_glXJoinSwapGroupSGIX JoinSwapGroupSGIX = 0;
	
	_detail::Proc_glXBindChannelToWindowSGIX BindChannelToWindowSGIX = 0;
	_detail::Proc_glXChannelRectSGIX ChannelRectSGIX = 0;
	_detail::Proc_glXChannelRectSyncSGIX ChannelRectSyncSGIX = 0;
	_detail::Proc_glXQueryChannelDeltasSGIX QueryChannelDeltasSGIX = 0;
	_detail::Proc_glXQueryChannelRectSGIX QueryChannelRectSGIX = 0;
	
	_detail::Proc_glXCushionSGI CushionSGI = 0;
	
	_detail::Proc_glXGetCurrentReadDrawableSGI GetCurrentReadDrawableSGI = 0;
	_detail::Proc_glXMakeCurrentReadSGI MakeCurrentReadSGI = 0;
	
	_detail::Proc_glXSwapIntervalSGI SwapIntervalSGI = 0;
	
	_detail::Proc_glXGetVideoSyncSGI GetVideoSyncSGI = 0;
	_detail::Proc_glXWaitVideoSyncSGI WaitVideoSyncSGI = 0;
	
	_detail::Proc_glXGetTransparentIndexSUN GetTransparentIndexSUN = 0;
	
	static void CopyExtensionVariables()
	{
		exts::var_3DFX_multisample = glload::LoadTest((::glXext_3DFX_multisample != 0), ::glXext_3DFX_multisample - 1);
		exts::var_AMD_gpu_association = glload::LoadTest((::glXext_AMD_gpu_association != 0), ::glXext_AMD_gpu_association - 1);
		exts::var_ARB_create_context = glload::LoadTest((::glXext_ARB_create_context != 0), ::glXext_ARB_create_context - 1);
		exts::var_ARB_create_context_profile = glload::LoadTest((::glXext_ARB_create_context_profile != 0), ::glXext_ARB_create_context_profile - 1);
		exts::var_ARB_create_context_robustness = glload::LoadTest((::glXext_ARB_create_context_robustness != 0), ::glXext_ARB_create_context_robustness - 1);
		exts::var_ARB_fbconfig_float = glload::LoadTest((::glXext_ARB_fbconfig_float != 0), ::glXext_ARB_fbconfig_float - 1);
		exts::var_ARB_framebuffer_sRGB = glload::LoadTest((::glXext_ARB_framebuffer_sRGB != 0), ::glXext_ARB_framebuffer_sRGB - 1);
		exts::var_ARB_get_proc_address = glload::LoadTest((::glXext_ARB_get_proc_address != 0), ::glXext_ARB_get_proc_address - 1);
		exts::var_ARB_multisample = glload::LoadTest((::glXext_ARB_multisample != 0), ::glXext_ARB_multisample - 1);
		exts::var_ARB_robustness_application_isolation = glload::LoadTest((::glXext_ARB_robustness_application_isolation != 0), ::glXext_ARB_robustness_application_isolation - 1);
		exts::var_ARB_robustness_share_group_isolation = glload::LoadTest((::glXext_ARB_robustness_share_group_isolation != 0), ::glXext_ARB_robustness_share_group_isolation - 1);
		exts::var_ARB_vertex_buffer_object = glload::LoadTest((::glXext_ARB_vertex_buffer_object != 0), ::glXext_ARB_vertex_buffer_object - 1);
		exts::var_EXT_buffer_age = glload::LoadTest((::glXext_EXT_buffer_age != 0), ::glXext_EXT_buffer_age - 1);
		exts::var_EXT_create_context_es2_profile = glload::LoadTest((::glXext_EXT_create_context_es2_profile != 0), ::glXext_EXT_create_context_es2_profile - 1);
		exts::var_EXT_create_context_es_profile = glload::LoadTest((::glXext_EXT_create_context_es_profile != 0), ::glXext_EXT_create_context_es_profile - 1);
		exts::var_EXT_fbconfig_packed_float = glload::LoadTest((::glXext_EXT_fbconfig_packed_float != 0), ::glXext_EXT_fbconfig_packed_float - 1);
		exts::var_EXT_framebuffer_sRGB = glload::LoadTest((::glXext_EXT_framebuffer_sRGB != 0), ::glXext_EXT_framebuffer_sRGB - 1);
		exts::var_EXT_import_context = glload::LoadTest((::glXext_EXT_import_context != 0), ::glXext_EXT_import_context - 1);
		exts::var_EXT_swap_control = glload::LoadTest((::glXext_EXT_swap_control != 0), ::glXext_EXT_swap_control - 1);
		exts::var_EXT_swap_control_tear = glload::LoadTest((::glXext_EXT_swap_control_tear != 0), ::glXext_EXT_swap_control_tear - 1);
		exts::var_EXT_texture_from_pixmap = glload::LoadTest((::glXext_EXT_texture_from_pixmap != 0), ::glXext_EXT_texture_from_pixmap - 1);
		exts::var_EXT_visual_info = glload::LoadTest((::glXext_EXT_visual_info != 0), ::glXext_EXT_visual_info - 1);
		exts::var_EXT_visual_rating = glload::LoadTest((::glXext_EXT_visual_rating != 0), ::glXext_EXT_visual_rating - 1);
		exts::var_INTEL_swap_event = glload::LoadTest((::glXext_INTEL_swap_event != 0), ::glXext_INTEL_swap_event - 1);
		exts::var_MESA_agp_offset = glload::LoadTest((::glXext_MESA_agp_offset != 0), ::glXext_MESA_agp_offset - 1);
		exts::var_MESA_copy_sub_buffer = glload::LoadTest((::glXext_MESA_copy_sub_buffer != 0), ::glXext_MESA_copy_sub_buffer - 1);
		exts::var_MESA_pixmap_colormap = glload::LoadTest((::glXext_MESA_pixmap_colormap != 0), ::glXext_MESA_pixmap_colormap - 1);
		exts::var_MESA_release_buffers = glload::LoadTest((::glXext_MESA_release_buffers != 0), ::glXext_MESA_release_buffers - 1);
		exts::var_MESA_set_3dfx_mode = glload::LoadTest((::glXext_MESA_set_3dfx_mode != 0), ::glXext_MESA_set_3dfx_mode - 1);
		exts::var_NV_copy_image = glload::LoadTest((::glXext_NV_copy_image != 0), ::glXext_NV_copy_image - 1);
		exts::var_NV_float_buffer = glload::LoadTest((::glXext_NV_float_buffer != 0), ::glXext_NV_float_buffer - 1);
		exts::var_NV_multisample_coverage = glload::LoadTest((::glXext_NV_multisample_coverage != 0), ::glXext_NV_multisample_coverage - 1);
		exts::var_NV_present_video = glload::LoadTest((::glXext_NV_present_video != 0), ::glXext_NV_present_video - 1);
		exts::var_NV_swap_group = glload::LoadTest((::glXext_NV_swap_group != 0), ::glXext_NV_swap_group - 1);
		exts::var_NV_video_capture = glload::LoadTest((::glXext_NV_video_capture != 0), ::glXext_NV_video_capture - 1);
		exts::var_NV_video_output = glload::LoadTest((::glXext_NV_video_output != 0), ::glXext_NV_video_output - 1);
		exts::var_OML_swap_method = glload::LoadTest((::glXext_OML_swap_method != 0), ::glXext_OML_swap_method - 1);
		exts::var_OML_sync_control = glload::LoadTest((::glXext_OML_sync_control != 0), ::glXext_OML_sync_control - 1);
		exts::var_SGIS_blended_overlay = glload::LoadTest((::glXext_SGIS_blended_overlay != 0), ::glXext_SGIS_blended_overlay - 1);
		exts::var_SGIS_multisample = glload::LoadTest((::glXext_SGIS_multisample != 0), ::glXext_SGIS_multisample - 1);
		exts::var_SGIS_shared_multisample = glload::LoadTest((::glXext_SGIS_shared_multisample != 0), ::glXext_SGIS_shared_multisample - 1);
		exts::var_SGIX_fbconfig = glload::LoadTest((::glXext_SGIX_fbconfig != 0), ::glXext_SGIX_fbconfig - 1);
		exts::var_SGIX_pbuffer = glload::LoadTest((::glXext_SGIX_pbuffer != 0), ::glXext_SGIX_pbuffer - 1);
		exts::var_SGIX_swap_barrier = glload::LoadTest((::glXext_SGIX_swap_barrier != 0), ::glXext_SGIX_swap_barrier - 1);
		exts::var_SGIX_swap_group = glload::LoadTest((::glXext_SGIX_swap_group != 0), ::glXext_SGIX_swap_group - 1);
		exts::var_SGIX_video_resize = glload::LoadTest((::glXext_SGIX_video_resize != 0), ::glXext_SGIX_video_resize - 1);
		exts::var_SGIX_visual_select_group = glload::LoadTest((::glXext_SGIX_visual_select_group != 0), ::glXext_SGIX_visual_select_group - 1);
		exts::var_SGI_cushion = glload::LoadTest((::glXext_SGI_cushion != 0), ::glXext_SGI_cushion - 1);
		exts::var_SGI_make_current_read = glload::LoadTest((::glXext_SGI_make_current_read != 0), ::glXext_SGI_make_current_read - 1);
		exts::var_SGI_swap_control = glload::LoadTest((::glXext_SGI_swap_control != 0), ::glXext_SGI_swap_control - 1);
		exts::var_SGI_video_sync = glload::LoadTest((::glXext_SGI_video_sync != 0), ::glXext_SGI_video_sync - 1);
		exts::var_SUN_get_transparent_index = glload::LoadTest((::glXext_SUN_get_transparent_index != 0), ::glXext_SUN_get_transparent_index - 1);
	}
	
	static void CopyFunctionPointers()
	{
		CreateContextAttribsARB = _funcptr_glXCreateContextAttribsARB;
		GetProcAddressARB = _funcptr_glXGetProcAddressARB;
		FreeContextEXT = _funcptr_glXFreeContextEXT;
		GetContextIDEXT = _funcptr_glXGetContextIDEXT;
		GetCurrentDisplayEXT = _funcptr_glXGetCurrentDisplayEXT;
		ImportContextEXT = _funcptr_glXImportContextEXT;
		QueryContextInfoEXT = _funcptr_glXQueryContextInfoEXT;
		SwapIntervalEXT = _funcptr_glXSwapIntervalEXT;
		BindTexImageEXT = _funcptr_glXBindTexImageEXT;
		ReleaseTexImageEXT = _funcptr_glXReleaseTexImageEXT;
		GetAGPOffsetMESA = _funcptr_glXGetAGPOffsetMESA;
		CopySubBufferMESA = _funcptr_glXCopySubBufferMESA;
		CreateGLXPixmapMESA = _funcptr_glXCreateGLXPixmapMESA;
		ReleaseBuffersMESA = _funcptr_glXReleaseBuffersMESA;
		Set3DfxModeMESA = _funcptr_glXSet3DfxModeMESA;
		CopyImageSubDataNV = _funcptr_glXCopyImageSubDataNV;
		BindVideoDeviceNV = _funcptr_glXBindVideoDeviceNV;
		EnumerateVideoDevicesNV = _funcptr_glXEnumerateVideoDevicesNV;
		BindSwapBarrierNV = _funcptr_glXBindSwapBarrierNV;
		JoinSwapGroupNV = _funcptr_glXJoinSwapGroupNV;
		QueryFrameCountNV = _funcptr_glXQueryFrameCountNV;
		QueryMaxSwapGroupsNV = _funcptr_glXQueryMaxSwapGroupsNV;
		QuerySwapGroupNV = _funcptr_glXQuerySwapGroupNV;
		ResetFrameCountNV = _funcptr_glXResetFrameCountNV;
		BindVideoCaptureDeviceNV = _funcptr_glXBindVideoCaptureDeviceNV;
		EnumerateVideoCaptureDevicesNV = _funcptr_glXEnumerateVideoCaptureDevicesNV;
		LockVideoCaptureDeviceNV = _funcptr_glXLockVideoCaptureDeviceNV;
		QueryVideoCaptureDeviceNV = _funcptr_glXQueryVideoCaptureDeviceNV;
		ReleaseVideoCaptureDeviceNV = _funcptr_glXReleaseVideoCaptureDeviceNV;
		BindVideoImageNV = _funcptr_glXBindVideoImageNV;
		GetVideoDeviceNV = _funcptr_glXGetVideoDeviceNV;
		GetVideoInfoNV = _funcptr_glXGetVideoInfoNV;
		ReleaseVideoDeviceNV = _funcptr_glXReleaseVideoDeviceNV;
		ReleaseVideoImageNV = _funcptr_glXReleaseVideoImageNV;
		SendPbufferToVideoNV = _funcptr_glXSendPbufferToVideoNV;
		GetMscRateOML = _funcptr_glXGetMscRateOML;
		GetSyncValuesOML = _funcptr_glXGetSyncValuesOML;
		SwapBuffersMscOML = _funcptr_glXSwapBuffersMscOML;
		WaitForMscOML = _funcptr_glXWaitForMscOML;
		WaitForSbcOML = _funcptr_glXWaitForSbcOML;
		ChooseFBConfigSGIX = _funcptr_glXChooseFBConfigSGIX;
		CreateContextWithConfigSGIX = _funcptr_glXCreateContextWithConfigSGIX;
		CreateGLXPixmapWithConfigSGIX = _funcptr_glXCreateGLXPixmapWithConfigSGIX;
		GetFBConfigAttribSGIX = _funcptr_glXGetFBConfigAttribSGIX;
		GetFBConfigFromVisualSGIX = _funcptr_glXGetFBConfigFromVisualSGIX;
		GetVisualFromFBConfigSGIX = _funcptr_glXGetVisualFromFBConfigSGIX;
		CreateGLXPbufferSGIX = _funcptr_glXCreateGLXPbufferSGIX;
		DestroyGLXPbufferSGIX = _funcptr_glXDestroyGLXPbufferSGIX;
		GetSelectedEventSGIX = _funcptr_glXGetSelectedEventSGIX;
		QueryGLXPbufferSGIX = _funcptr_glXQueryGLXPbufferSGIX;
		SelectEventSGIX = _funcptr_glXSelectEventSGIX;
		BindSwapBarrierSGIX = _funcptr_glXBindSwapBarrierSGIX;
		QueryMaxSwapBarriersSGIX = _funcptr_glXQueryMaxSwapBarriersSGIX;
		JoinSwapGroupSGIX = _funcptr_glXJoinSwapGroupSGIX;
		BindChannelToWindowSGIX = _funcptr_glXBindChannelToWindowSGIX;
		ChannelRectSGIX = _funcptr_glXChannelRectSGIX;
		ChannelRectSyncSGIX = _funcptr_glXChannelRectSyncSGIX;
		QueryChannelDeltasSGIX = _funcptr_glXQueryChannelDeltasSGIX;
		QueryChannelRectSGIX = _funcptr_glXQueryChannelRectSGIX;
		CushionSGI = _funcptr_glXCushionSGI;
		GetCurrentReadDrawableSGI = _funcptr_glXGetCurrentReadDrawableSGI;
		MakeCurrentReadSGI = _funcptr_glXMakeCurrentReadSGI;
		SwapIntervalSGI = _funcptr_glXSwapIntervalSGI;
		GetVideoSyncSGI = _funcptr_glXGetVideoSyncSGI;
		WaitVideoSyncSGI = _funcptr_glXWaitVideoSyncSGI;
		GetTransparentIndexSUN = _funcptr_glXGetTransparentIndexSUN;
	}
	
}
extern "C" void glx_CopyFromC()
{
  glX::CopyExtensionVariables();
  glX::CopyFunctionPointers();
}

namespace glload
{
	glload::LoadTest LoadFunctions(Display *display, int screen)
	{
		int test = ::glx_LoadFunctions(display, screen);
		//The C loader will call the above function to copy the variables and such.
		
		if(test == 0)
			return glload::LoadTest(false, 0);
		return glload::LoadTest(true, test - 1);
	}
	
}
