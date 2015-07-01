#include <algorithm>
#include <vector>
#include <string.h>
#include "glload/wgl_all.hpp"
#include "glload/wgl_load.hpp"

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

extern wgl::_detail::Proc_wglSetStereoEmitterState3DL _funcptr_wglSetStereoEmitterState3DL;
extern wgl::_detail::Proc_wglBlitContextFramebufferAMD _funcptr_wglBlitContextFramebufferAMD;
extern wgl::_detail::Proc_wglCreateAssociatedContextAMD _funcptr_wglCreateAssociatedContextAMD;
extern wgl::_detail::Proc_wglCreateAssociatedContextAttribsAMD _funcptr_wglCreateAssociatedContextAttribsAMD;
extern wgl::_detail::Proc_wglDeleteAssociatedContextAMD _funcptr_wglDeleteAssociatedContextAMD;
extern wgl::_detail::Proc_wglGetContextGPUIDAMD _funcptr_wglGetContextGPUIDAMD;
extern wgl::_detail::Proc_wglGetCurrentAssociatedContextAMD _funcptr_wglGetCurrentAssociatedContextAMD;
extern wgl::_detail::Proc_wglGetGPUIDsAMD _funcptr_wglGetGPUIDsAMD;
extern wgl::_detail::Proc_wglGetGPUInfoAMD _funcptr_wglGetGPUInfoAMD;
extern wgl::_detail::Proc_wglMakeAssociatedContextCurrentAMD _funcptr_wglMakeAssociatedContextCurrentAMD;
extern wgl::_detail::Proc_wglCreateBufferRegionARB _funcptr_wglCreateBufferRegionARB;
extern wgl::_detail::Proc_wglDeleteBufferRegionARB _funcptr_wglDeleteBufferRegionARB;
extern wgl::_detail::Proc_wglRestoreBufferRegionARB _funcptr_wglRestoreBufferRegionARB;
extern wgl::_detail::Proc_wglSaveBufferRegionARB _funcptr_wglSaveBufferRegionARB;
extern wgl::_detail::Proc_wglCreateContextAttribsARB _funcptr_wglCreateContextAttribsARB;
extern wgl::_detail::Proc_wglGetExtensionsStringARB _funcptr_wglGetExtensionsStringARB;
extern wgl::_detail::Proc_wglGetCurrentReadDCARB _funcptr_wglGetCurrentReadDCARB;
extern wgl::_detail::Proc_wglMakeContextCurrentARB _funcptr_wglMakeContextCurrentARB;
extern wgl::_detail::Proc_wglCreatePbufferARB _funcptr_wglCreatePbufferARB;
extern wgl::_detail::Proc_wglDestroyPbufferARB _funcptr_wglDestroyPbufferARB;
extern wgl::_detail::Proc_wglGetPbufferDCARB _funcptr_wglGetPbufferDCARB;
extern wgl::_detail::Proc_wglQueryPbufferARB _funcptr_wglQueryPbufferARB;
extern wgl::_detail::Proc_wglReleasePbufferDCARB _funcptr_wglReleasePbufferDCARB;
extern wgl::_detail::Proc_wglChoosePixelFormatARB _funcptr_wglChoosePixelFormatARB;
extern wgl::_detail::Proc_wglGetPixelFormatAttribfvARB _funcptr_wglGetPixelFormatAttribfvARB;
extern wgl::_detail::Proc_wglGetPixelFormatAttribivARB _funcptr_wglGetPixelFormatAttribivARB;
extern wgl::_detail::Proc_wglBindTexImageARB _funcptr_wglBindTexImageARB;
extern wgl::_detail::Proc_wglReleaseTexImageARB _funcptr_wglReleaseTexImageARB;
extern wgl::_detail::Proc_wglSetPbufferAttribARB _funcptr_wglSetPbufferAttribARB;
extern wgl::_detail::Proc_wglBindDisplayColorTableEXT _funcptr_wglBindDisplayColorTableEXT;
extern wgl::_detail::Proc_wglCreateDisplayColorTableEXT _funcptr_wglCreateDisplayColorTableEXT;
extern wgl::_detail::Proc_wglDestroyDisplayColorTableEXT _funcptr_wglDestroyDisplayColorTableEXT;
extern wgl::_detail::Proc_wglLoadDisplayColorTableEXT _funcptr_wglLoadDisplayColorTableEXT;
extern wgl::_detail::Proc_wglGetExtensionsStringEXT _funcptr_wglGetExtensionsStringEXT;
extern wgl::_detail::Proc_wglGetCurrentReadDCEXT _funcptr_wglGetCurrentReadDCEXT;
extern wgl::_detail::Proc_wglMakeContextCurrentEXT _funcptr_wglMakeContextCurrentEXT;
extern wgl::_detail::Proc_wglCreatePbufferEXT _funcptr_wglCreatePbufferEXT;
extern wgl::_detail::Proc_wglDestroyPbufferEXT _funcptr_wglDestroyPbufferEXT;
extern wgl::_detail::Proc_wglGetPbufferDCEXT _funcptr_wglGetPbufferDCEXT;
extern wgl::_detail::Proc_wglQueryPbufferEXT _funcptr_wglQueryPbufferEXT;
extern wgl::_detail::Proc_wglReleasePbufferDCEXT _funcptr_wglReleasePbufferDCEXT;
extern wgl::_detail::Proc_wglChoosePixelFormatEXT _funcptr_wglChoosePixelFormatEXT;
extern wgl::_detail::Proc_wglGetPixelFormatAttribfvEXT _funcptr_wglGetPixelFormatAttribfvEXT;
extern wgl::_detail::Proc_wglGetPixelFormatAttribivEXT _funcptr_wglGetPixelFormatAttribivEXT;
extern wgl::_detail::Proc_wglGetSwapIntervalEXT _funcptr_wglGetSwapIntervalEXT;
extern wgl::_detail::Proc_wglSwapIntervalEXT _funcptr_wglSwapIntervalEXT;
extern wgl::_detail::Proc_wglGetDigitalVideoParametersI3D _funcptr_wglGetDigitalVideoParametersI3D;
extern wgl::_detail::Proc_wglSetDigitalVideoParametersI3D _funcptr_wglSetDigitalVideoParametersI3D;
extern wgl::_detail::Proc_wglGetGammaTableI3D _funcptr_wglGetGammaTableI3D;
extern wgl::_detail::Proc_wglGetGammaTableParametersI3D _funcptr_wglGetGammaTableParametersI3D;
extern wgl::_detail::Proc_wglSetGammaTableI3D _funcptr_wglSetGammaTableI3D;
extern wgl::_detail::Proc_wglSetGammaTableParametersI3D _funcptr_wglSetGammaTableParametersI3D;
extern wgl::_detail::Proc_wglDisableGenlockI3D _funcptr_wglDisableGenlockI3D;
extern wgl::_detail::Proc_wglEnableGenlockI3D _funcptr_wglEnableGenlockI3D;
extern wgl::_detail::Proc_wglGenlockSampleRateI3D _funcptr_wglGenlockSampleRateI3D;
extern wgl::_detail::Proc_wglGenlockSourceDelayI3D _funcptr_wglGenlockSourceDelayI3D;
extern wgl::_detail::Proc_wglGenlockSourceEdgeI3D _funcptr_wglGenlockSourceEdgeI3D;
extern wgl::_detail::Proc_wglGenlockSourceI3D _funcptr_wglGenlockSourceI3D;
extern wgl::_detail::Proc_wglGetGenlockSampleRateI3D _funcptr_wglGetGenlockSampleRateI3D;
extern wgl::_detail::Proc_wglGetGenlockSourceDelayI3D _funcptr_wglGetGenlockSourceDelayI3D;
extern wgl::_detail::Proc_wglGetGenlockSourceEdgeI3D _funcptr_wglGetGenlockSourceEdgeI3D;
extern wgl::_detail::Proc_wglGetGenlockSourceI3D _funcptr_wglGetGenlockSourceI3D;
extern wgl::_detail::Proc_wglIsEnabledGenlockI3D _funcptr_wglIsEnabledGenlockI3D;
extern wgl::_detail::Proc_wglQueryGenlockMaxSourceDelayI3D _funcptr_wglQueryGenlockMaxSourceDelayI3D;
extern wgl::_detail::Proc_wglAssociateImageBufferEventsI3D _funcptr_wglAssociateImageBufferEventsI3D;
extern wgl::_detail::Proc_wglCreateImageBufferI3D _funcptr_wglCreateImageBufferI3D;
extern wgl::_detail::Proc_wglDestroyImageBufferI3D _funcptr_wglDestroyImageBufferI3D;
extern wgl::_detail::Proc_wglReleaseImageBufferEventsI3D _funcptr_wglReleaseImageBufferEventsI3D;
extern wgl::_detail::Proc_wglDisableFrameLockI3D _funcptr_wglDisableFrameLockI3D;
extern wgl::_detail::Proc_wglEnableFrameLockI3D _funcptr_wglEnableFrameLockI3D;
extern wgl::_detail::Proc_wglIsEnabledFrameLockI3D _funcptr_wglIsEnabledFrameLockI3D;
extern wgl::_detail::Proc_wglQueryFrameLockMasterI3D _funcptr_wglQueryFrameLockMasterI3D;
extern wgl::_detail::Proc_wglBeginFrameTrackingI3D _funcptr_wglBeginFrameTrackingI3D;
extern wgl::_detail::Proc_wglEndFrameTrackingI3D _funcptr_wglEndFrameTrackingI3D;
extern wgl::_detail::Proc_wglGetFrameUsageI3D _funcptr_wglGetFrameUsageI3D;
extern wgl::_detail::Proc_wglQueryFrameTrackingI3D _funcptr_wglQueryFrameTrackingI3D;
extern wgl::_detail::Proc_wglDXCloseDeviceNV _funcptr_wglDXCloseDeviceNV;
extern wgl::_detail::Proc_wglDXLockObjectsNV _funcptr_wglDXLockObjectsNV;
extern wgl::_detail::Proc_wglDXObjectAccessNV _funcptr_wglDXObjectAccessNV;
extern wgl::_detail::Proc_wglDXOpenDeviceNV _funcptr_wglDXOpenDeviceNV;
extern wgl::_detail::Proc_wglDXRegisterObjectNV _funcptr_wglDXRegisterObjectNV;
extern wgl::_detail::Proc_wglDXSetResourceShareHandleNV _funcptr_wglDXSetResourceShareHandleNV;
extern wgl::_detail::Proc_wglDXUnlockObjectsNV _funcptr_wglDXUnlockObjectsNV;
extern wgl::_detail::Proc_wglDXUnregisterObjectNV _funcptr_wglDXUnregisterObjectNV;
extern wgl::_detail::Proc_wglCopyImageSubDataNV _funcptr_wglCopyImageSubDataNV;
extern wgl::_detail::Proc_wglCreateAffinityDCNV _funcptr_wglCreateAffinityDCNV;
extern wgl::_detail::Proc_wglDeleteDCNV _funcptr_wglDeleteDCNV;
extern wgl::_detail::Proc_wglEnumGpuDevicesNV _funcptr_wglEnumGpuDevicesNV;
extern wgl::_detail::Proc_wglEnumGpusFromAffinityDCNV _funcptr_wglEnumGpusFromAffinityDCNV;
extern wgl::_detail::Proc_wglEnumGpusNV _funcptr_wglEnumGpusNV;
extern wgl::_detail::Proc_wglBindVideoDeviceNV _funcptr_wglBindVideoDeviceNV;
extern wgl::_detail::Proc_wglEnumerateVideoDevicesNV _funcptr_wglEnumerateVideoDevicesNV;
extern wgl::_detail::Proc_wglQueryCurrentContextNV _funcptr_wglQueryCurrentContextNV;
extern wgl::_detail::Proc_wglBindSwapBarrierNV _funcptr_wglBindSwapBarrierNV;
extern wgl::_detail::Proc_wglJoinSwapGroupNV _funcptr_wglJoinSwapGroupNV;
extern wgl::_detail::Proc_wglQueryFrameCountNV _funcptr_wglQueryFrameCountNV;
extern wgl::_detail::Proc_wglQueryMaxSwapGroupsNV _funcptr_wglQueryMaxSwapGroupsNV;
extern wgl::_detail::Proc_wglQuerySwapGroupNV _funcptr_wglQuerySwapGroupNV;
extern wgl::_detail::Proc_wglResetFrameCountNV _funcptr_wglResetFrameCountNV;
extern wgl::_detail::Proc_wglAllocateMemoryNV _funcptr_wglAllocateMemoryNV;
extern wgl::_detail::Proc_wglFreeMemoryNV _funcptr_wglFreeMemoryNV;
extern wgl::_detail::Proc_wglBindVideoCaptureDeviceNV _funcptr_wglBindVideoCaptureDeviceNV;
extern wgl::_detail::Proc_wglEnumerateVideoCaptureDevicesNV _funcptr_wglEnumerateVideoCaptureDevicesNV;
extern wgl::_detail::Proc_wglLockVideoCaptureDeviceNV _funcptr_wglLockVideoCaptureDeviceNV;
extern wgl::_detail::Proc_wglQueryVideoCaptureDeviceNV _funcptr_wglQueryVideoCaptureDeviceNV;
extern wgl::_detail::Proc_wglReleaseVideoCaptureDeviceNV _funcptr_wglReleaseVideoCaptureDeviceNV;
extern wgl::_detail::Proc_wglBindVideoImageNV _funcptr_wglBindVideoImageNV;
extern wgl::_detail::Proc_wglGetVideoDeviceNV _funcptr_wglGetVideoDeviceNV;
extern wgl::_detail::Proc_wglGetVideoInfoNV _funcptr_wglGetVideoInfoNV;
extern wgl::_detail::Proc_wglReleaseVideoDeviceNV _funcptr_wglReleaseVideoDeviceNV;
extern wgl::_detail::Proc_wglReleaseVideoImageNV _funcptr_wglReleaseVideoImageNV;
extern wgl::_detail::Proc_wglSendPbufferToVideoNV _funcptr_wglSendPbufferToVideoNV;
extern wgl::_detail::Proc_wglGetMscRateOML _funcptr_wglGetMscRateOML;
extern wgl::_detail::Proc_wglGetSyncValuesOML _funcptr_wglGetSyncValuesOML;
extern wgl::_detail::Proc_wglSwapBuffersMscOML _funcptr_wglSwapBuffersMscOML;
extern wgl::_detail::Proc_wglSwapLayerBuffersMscOML _funcptr_wglSwapLayerBuffersMscOML;
extern wgl::_detail::Proc_wglWaitForMscOML _funcptr_wglWaitForMscOML;
extern wgl::_detail::Proc_wglWaitForSbcOML _funcptr_wglWaitForSbcOML;

int wgl_LoadFunctions(HDC hdc);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

namespace wgl
{
	namespace exts
	{
		glload::LoadTest var_3DFX_multisample;
		glload::LoadTest var_3DL_stereo_control;
		glload::LoadTest var_AMD_gpu_association;
		glload::LoadTest var_ARB_buffer_region;
		glload::LoadTest var_ARB_create_context;
		glload::LoadTest var_ARB_create_context_profile;
		glload::LoadTest var_ARB_create_context_robustness;
		glload::LoadTest var_ARB_extensions_string;
		glload::LoadTest var_ARB_framebuffer_sRGB;
		glload::LoadTest var_ARB_make_current_read;
		glload::LoadTest var_ARB_multisample;
		glload::LoadTest var_ARB_pbuffer;
		glload::LoadTest var_ARB_pixel_format;
		glload::LoadTest var_ARB_pixel_format_float;
		glload::LoadTest var_ARB_render_texture;
		glload::LoadTest var_ARB_robustness_application_isolation;
		glload::LoadTest var_ARB_robustness_share_group_isolation;
		glload::LoadTest var_ATI_pixel_format_float;
		glload::LoadTest var_EXT_create_context_es2_profile;
		glload::LoadTest var_EXT_create_context_es_profile;
		glload::LoadTest var_EXT_depth_float;
		glload::LoadTest var_EXT_display_color_table;
		glload::LoadTest var_EXT_extensions_string;
		glload::LoadTest var_EXT_framebuffer_sRGB;
		glload::LoadTest var_EXT_make_current_read;
		glload::LoadTest var_EXT_multisample;
		glload::LoadTest var_EXT_pbuffer;
		glload::LoadTest var_EXT_pixel_format;
		glload::LoadTest var_EXT_pixel_format_packed_float;
		glload::LoadTest var_EXT_swap_control;
		glload::LoadTest var_EXT_swap_control_tear;
		glload::LoadTest var_I3D_digital_video_control;
		glload::LoadTest var_I3D_gamma;
		glload::LoadTest var_I3D_genlock;
		glload::LoadTest var_I3D_image_buffer;
		glload::LoadTest var_I3D_swap_frame_lock;
		glload::LoadTest var_I3D_swap_frame_usage;
		glload::LoadTest var_NV_DX_interop;
		glload::LoadTest var_NV_DX_interop2;
		glload::LoadTest var_NV_copy_image;
		glload::LoadTest var_NV_float_buffer;
		glload::LoadTest var_NV_gpu_affinity;
		glload::LoadTest var_NV_multisample_coverage;
		glload::LoadTest var_NV_present_video;
		glload::LoadTest var_NV_render_depth_texture;
		glload::LoadTest var_NV_render_texture_rectangle;
		glload::LoadTest var_NV_swap_group;
		glload::LoadTest var_NV_vertex_array_range;
		glload::LoadTest var_NV_video_capture;
		glload::LoadTest var_NV_video_output;
		glload::LoadTest var_OML_sync_control;
	}
	
	_detail::Proc_wglSetStereoEmitterState3DL SetStereoEmitterState3DL = 0;
	
	_detail::Proc_wglBlitContextFramebufferAMD BlitContextFramebufferAMD = 0;
	_detail::Proc_wglCreateAssociatedContextAMD CreateAssociatedContextAMD = 0;
	_detail::Proc_wglCreateAssociatedContextAttribsAMD CreateAssociatedContextAttribsAMD = 0;
	_detail::Proc_wglDeleteAssociatedContextAMD DeleteAssociatedContextAMD = 0;
	_detail::Proc_wglGetContextGPUIDAMD GetContextGPUIDAMD = 0;
	_detail::Proc_wglGetCurrentAssociatedContextAMD GetCurrentAssociatedContextAMD = 0;
	_detail::Proc_wglGetGPUIDsAMD GetGPUIDsAMD = 0;
	_detail::Proc_wglGetGPUInfoAMD GetGPUInfoAMD = 0;
	_detail::Proc_wglMakeAssociatedContextCurrentAMD MakeAssociatedContextCurrentAMD = 0;
	
	_detail::Proc_wglCreateBufferRegionARB CreateBufferRegionARB = 0;
	_detail::Proc_wglDeleteBufferRegionARB DeleteBufferRegionARB = 0;
	_detail::Proc_wglRestoreBufferRegionARB RestoreBufferRegionARB = 0;
	_detail::Proc_wglSaveBufferRegionARB SaveBufferRegionARB = 0;
	
	_detail::Proc_wglCreateContextAttribsARB CreateContextAttribsARB = 0;
	
	_detail::Proc_wglGetExtensionsStringARB GetExtensionsStringARB = 0;
	
	_detail::Proc_wglGetCurrentReadDCARB GetCurrentReadDCARB = 0;
	_detail::Proc_wglMakeContextCurrentARB MakeContextCurrentARB = 0;
	
	_detail::Proc_wglCreatePbufferARB CreatePbufferARB = 0;
	_detail::Proc_wglDestroyPbufferARB DestroyPbufferARB = 0;
	_detail::Proc_wglGetPbufferDCARB GetPbufferDCARB = 0;
	_detail::Proc_wglQueryPbufferARB QueryPbufferARB = 0;
	_detail::Proc_wglReleasePbufferDCARB ReleasePbufferDCARB = 0;
	
	_detail::Proc_wglChoosePixelFormatARB ChoosePixelFormatARB = 0;
	_detail::Proc_wglGetPixelFormatAttribfvARB GetPixelFormatAttribfvARB = 0;
	_detail::Proc_wglGetPixelFormatAttribivARB GetPixelFormatAttribivARB = 0;
	
	_detail::Proc_wglBindTexImageARB BindTexImageARB = 0;
	_detail::Proc_wglReleaseTexImageARB ReleaseTexImageARB = 0;
	_detail::Proc_wglSetPbufferAttribARB SetPbufferAttribARB = 0;
	
	_detail::Proc_wglBindDisplayColorTableEXT BindDisplayColorTableEXT = 0;
	_detail::Proc_wglCreateDisplayColorTableEXT CreateDisplayColorTableEXT = 0;
	_detail::Proc_wglDestroyDisplayColorTableEXT DestroyDisplayColorTableEXT = 0;
	_detail::Proc_wglLoadDisplayColorTableEXT LoadDisplayColorTableEXT = 0;
	
	_detail::Proc_wglGetExtensionsStringEXT GetExtensionsStringEXT = 0;
	
	_detail::Proc_wglGetCurrentReadDCEXT GetCurrentReadDCEXT = 0;
	_detail::Proc_wglMakeContextCurrentEXT MakeContextCurrentEXT = 0;
	
	_detail::Proc_wglCreatePbufferEXT CreatePbufferEXT = 0;
	_detail::Proc_wglDestroyPbufferEXT DestroyPbufferEXT = 0;
	_detail::Proc_wglGetPbufferDCEXT GetPbufferDCEXT = 0;
	_detail::Proc_wglQueryPbufferEXT QueryPbufferEXT = 0;
	_detail::Proc_wglReleasePbufferDCEXT ReleasePbufferDCEXT = 0;
	
	_detail::Proc_wglChoosePixelFormatEXT ChoosePixelFormatEXT = 0;
	_detail::Proc_wglGetPixelFormatAttribfvEXT GetPixelFormatAttribfvEXT = 0;
	_detail::Proc_wglGetPixelFormatAttribivEXT GetPixelFormatAttribivEXT = 0;
	
	_detail::Proc_wglGetSwapIntervalEXT GetSwapIntervalEXT = 0;
	_detail::Proc_wglSwapIntervalEXT SwapIntervalEXT = 0;
	
	_detail::Proc_wglGetDigitalVideoParametersI3D GetDigitalVideoParametersI3D = 0;
	_detail::Proc_wglSetDigitalVideoParametersI3D SetDigitalVideoParametersI3D = 0;
	
	_detail::Proc_wglGetGammaTableI3D GetGammaTableI3D = 0;
	_detail::Proc_wglGetGammaTableParametersI3D GetGammaTableParametersI3D = 0;
	_detail::Proc_wglSetGammaTableI3D SetGammaTableI3D = 0;
	_detail::Proc_wglSetGammaTableParametersI3D SetGammaTableParametersI3D = 0;
	
	_detail::Proc_wglDisableGenlockI3D DisableGenlockI3D = 0;
	_detail::Proc_wglEnableGenlockI3D EnableGenlockI3D = 0;
	_detail::Proc_wglGenlockSampleRateI3D GenlockSampleRateI3D = 0;
	_detail::Proc_wglGenlockSourceDelayI3D GenlockSourceDelayI3D = 0;
	_detail::Proc_wglGenlockSourceEdgeI3D GenlockSourceEdgeI3D = 0;
	_detail::Proc_wglGenlockSourceI3D GenlockSourceI3D = 0;
	_detail::Proc_wglGetGenlockSampleRateI3D GetGenlockSampleRateI3D = 0;
	_detail::Proc_wglGetGenlockSourceDelayI3D GetGenlockSourceDelayI3D = 0;
	_detail::Proc_wglGetGenlockSourceEdgeI3D GetGenlockSourceEdgeI3D = 0;
	_detail::Proc_wglGetGenlockSourceI3D GetGenlockSourceI3D = 0;
	_detail::Proc_wglIsEnabledGenlockI3D IsEnabledGenlockI3D = 0;
	_detail::Proc_wglQueryGenlockMaxSourceDelayI3D QueryGenlockMaxSourceDelayI3D = 0;
	
	_detail::Proc_wglAssociateImageBufferEventsI3D AssociateImageBufferEventsI3D = 0;
	_detail::Proc_wglCreateImageBufferI3D CreateImageBufferI3D = 0;
	_detail::Proc_wglDestroyImageBufferI3D DestroyImageBufferI3D = 0;
	_detail::Proc_wglReleaseImageBufferEventsI3D ReleaseImageBufferEventsI3D = 0;
	
	_detail::Proc_wglDisableFrameLockI3D DisableFrameLockI3D = 0;
	_detail::Proc_wglEnableFrameLockI3D EnableFrameLockI3D = 0;
	_detail::Proc_wglIsEnabledFrameLockI3D IsEnabledFrameLockI3D = 0;
	_detail::Proc_wglQueryFrameLockMasterI3D QueryFrameLockMasterI3D = 0;
	
	_detail::Proc_wglBeginFrameTrackingI3D BeginFrameTrackingI3D = 0;
	_detail::Proc_wglEndFrameTrackingI3D EndFrameTrackingI3D = 0;
	_detail::Proc_wglGetFrameUsageI3D GetFrameUsageI3D = 0;
	_detail::Proc_wglQueryFrameTrackingI3D QueryFrameTrackingI3D = 0;
	
	_detail::Proc_wglDXCloseDeviceNV DXCloseDeviceNV = 0;
	_detail::Proc_wglDXLockObjectsNV DXLockObjectsNV = 0;
	_detail::Proc_wglDXObjectAccessNV DXObjectAccessNV = 0;
	_detail::Proc_wglDXOpenDeviceNV DXOpenDeviceNV = 0;
	_detail::Proc_wglDXRegisterObjectNV DXRegisterObjectNV = 0;
	_detail::Proc_wglDXSetResourceShareHandleNV DXSetResourceShareHandleNV = 0;
	_detail::Proc_wglDXUnlockObjectsNV DXUnlockObjectsNV = 0;
	_detail::Proc_wglDXUnregisterObjectNV DXUnregisterObjectNV = 0;
	
	_detail::Proc_wglCopyImageSubDataNV CopyImageSubDataNV = 0;
	
	_detail::Proc_wglCreateAffinityDCNV CreateAffinityDCNV = 0;
	_detail::Proc_wglDeleteDCNV DeleteDCNV = 0;
	_detail::Proc_wglEnumGpuDevicesNV EnumGpuDevicesNV = 0;
	_detail::Proc_wglEnumGpusFromAffinityDCNV EnumGpusFromAffinityDCNV = 0;
	_detail::Proc_wglEnumGpusNV EnumGpusNV = 0;
	
	_detail::Proc_wglBindVideoDeviceNV BindVideoDeviceNV = 0;
	_detail::Proc_wglEnumerateVideoDevicesNV EnumerateVideoDevicesNV = 0;
	_detail::Proc_wglQueryCurrentContextNV QueryCurrentContextNV = 0;
	
	_detail::Proc_wglBindSwapBarrierNV BindSwapBarrierNV = 0;
	_detail::Proc_wglJoinSwapGroupNV JoinSwapGroupNV = 0;
	_detail::Proc_wglQueryFrameCountNV QueryFrameCountNV = 0;
	_detail::Proc_wglQueryMaxSwapGroupsNV QueryMaxSwapGroupsNV = 0;
	_detail::Proc_wglQuerySwapGroupNV QuerySwapGroupNV = 0;
	_detail::Proc_wglResetFrameCountNV ResetFrameCountNV = 0;
	
	_detail::Proc_wglAllocateMemoryNV AllocateMemoryNV = 0;
	_detail::Proc_wglFreeMemoryNV FreeMemoryNV = 0;
	
	_detail::Proc_wglBindVideoCaptureDeviceNV BindVideoCaptureDeviceNV = 0;
	_detail::Proc_wglEnumerateVideoCaptureDevicesNV EnumerateVideoCaptureDevicesNV = 0;
	_detail::Proc_wglLockVideoCaptureDeviceNV LockVideoCaptureDeviceNV = 0;
	_detail::Proc_wglQueryVideoCaptureDeviceNV QueryVideoCaptureDeviceNV = 0;
	_detail::Proc_wglReleaseVideoCaptureDeviceNV ReleaseVideoCaptureDeviceNV = 0;
	
	_detail::Proc_wglBindVideoImageNV BindVideoImageNV = 0;
	_detail::Proc_wglGetVideoDeviceNV GetVideoDeviceNV = 0;
	_detail::Proc_wglGetVideoInfoNV GetVideoInfoNV = 0;
	_detail::Proc_wglReleaseVideoDeviceNV ReleaseVideoDeviceNV = 0;
	_detail::Proc_wglReleaseVideoImageNV ReleaseVideoImageNV = 0;
	_detail::Proc_wglSendPbufferToVideoNV SendPbufferToVideoNV = 0;
	
	_detail::Proc_wglGetMscRateOML GetMscRateOML = 0;
	_detail::Proc_wglGetSyncValuesOML GetSyncValuesOML = 0;
	_detail::Proc_wglSwapBuffersMscOML SwapBuffersMscOML = 0;
	_detail::Proc_wglSwapLayerBuffersMscOML SwapLayerBuffersMscOML = 0;
	_detail::Proc_wglWaitForMscOML WaitForMscOML = 0;
	_detail::Proc_wglWaitForSbcOML WaitForSbcOML = 0;
	
	static void CopyExtensionVariables()
	{
		exts::var_3DFX_multisample = glload::LoadTest((::wglext_3DFX_multisample != 0), ::wglext_3DFX_multisample - 1);
		exts::var_3DL_stereo_control = glload::LoadTest((::wglext_3DL_stereo_control != 0), ::wglext_3DL_stereo_control - 1);
		exts::var_AMD_gpu_association = glload::LoadTest((::wglext_AMD_gpu_association != 0), ::wglext_AMD_gpu_association - 1);
		exts::var_ARB_buffer_region = glload::LoadTest((::wglext_ARB_buffer_region != 0), ::wglext_ARB_buffer_region - 1);
		exts::var_ARB_create_context = glload::LoadTest((::wglext_ARB_create_context != 0), ::wglext_ARB_create_context - 1);
		exts::var_ARB_create_context_profile = glload::LoadTest((::wglext_ARB_create_context_profile != 0), ::wglext_ARB_create_context_profile - 1);
		exts::var_ARB_create_context_robustness = glload::LoadTest((::wglext_ARB_create_context_robustness != 0), ::wglext_ARB_create_context_robustness - 1);
		exts::var_ARB_extensions_string = glload::LoadTest((::wglext_ARB_extensions_string != 0), ::wglext_ARB_extensions_string - 1);
		exts::var_ARB_framebuffer_sRGB = glload::LoadTest((::wglext_ARB_framebuffer_sRGB != 0), ::wglext_ARB_framebuffer_sRGB - 1);
		exts::var_ARB_make_current_read = glload::LoadTest((::wglext_ARB_make_current_read != 0), ::wglext_ARB_make_current_read - 1);
		exts::var_ARB_multisample = glload::LoadTest((::wglext_ARB_multisample != 0), ::wglext_ARB_multisample - 1);
		exts::var_ARB_pbuffer = glload::LoadTest((::wglext_ARB_pbuffer != 0), ::wglext_ARB_pbuffer - 1);
		exts::var_ARB_pixel_format = glload::LoadTest((::wglext_ARB_pixel_format != 0), ::wglext_ARB_pixel_format - 1);
		exts::var_ARB_pixel_format_float = glload::LoadTest((::wglext_ARB_pixel_format_float != 0), ::wglext_ARB_pixel_format_float - 1);
		exts::var_ARB_render_texture = glload::LoadTest((::wglext_ARB_render_texture != 0), ::wglext_ARB_render_texture - 1);
		exts::var_ARB_robustness_application_isolation = glload::LoadTest((::wglext_ARB_robustness_application_isolation != 0), ::wglext_ARB_robustness_application_isolation - 1);
		exts::var_ARB_robustness_share_group_isolation = glload::LoadTest((::wglext_ARB_robustness_share_group_isolation != 0), ::wglext_ARB_robustness_share_group_isolation - 1);
		exts::var_ATI_pixel_format_float = glload::LoadTest((::wglext_ATI_pixel_format_float != 0), ::wglext_ATI_pixel_format_float - 1);
		exts::var_EXT_create_context_es2_profile = glload::LoadTest((::wglext_EXT_create_context_es2_profile != 0), ::wglext_EXT_create_context_es2_profile - 1);
		exts::var_EXT_create_context_es_profile = glload::LoadTest((::wglext_EXT_create_context_es_profile != 0), ::wglext_EXT_create_context_es_profile - 1);
		exts::var_EXT_depth_float = glload::LoadTest((::wglext_EXT_depth_float != 0), ::wglext_EXT_depth_float - 1);
		exts::var_EXT_display_color_table = glload::LoadTest((::wglext_EXT_display_color_table != 0), ::wglext_EXT_display_color_table - 1);
		exts::var_EXT_extensions_string = glload::LoadTest((::wglext_EXT_extensions_string != 0), ::wglext_EXT_extensions_string - 1);
		exts::var_EXT_framebuffer_sRGB = glload::LoadTest((::wglext_EXT_framebuffer_sRGB != 0), ::wglext_EXT_framebuffer_sRGB - 1);
		exts::var_EXT_make_current_read = glload::LoadTest((::wglext_EXT_make_current_read != 0), ::wglext_EXT_make_current_read - 1);
		exts::var_EXT_multisample = glload::LoadTest((::wglext_EXT_multisample != 0), ::wglext_EXT_multisample - 1);
		exts::var_EXT_pbuffer = glload::LoadTest((::wglext_EXT_pbuffer != 0), ::wglext_EXT_pbuffer - 1);
		exts::var_EXT_pixel_format = glload::LoadTest((::wglext_EXT_pixel_format != 0), ::wglext_EXT_pixel_format - 1);
		exts::var_EXT_pixel_format_packed_float = glload::LoadTest((::wglext_EXT_pixel_format_packed_float != 0), ::wglext_EXT_pixel_format_packed_float - 1);
		exts::var_EXT_swap_control = glload::LoadTest((::wglext_EXT_swap_control != 0), ::wglext_EXT_swap_control - 1);
		exts::var_EXT_swap_control_tear = glload::LoadTest((::wglext_EXT_swap_control_tear != 0), ::wglext_EXT_swap_control_tear - 1);
		exts::var_I3D_digital_video_control = glload::LoadTest((::wglext_I3D_digital_video_control != 0), ::wglext_I3D_digital_video_control - 1);
		exts::var_I3D_gamma = glload::LoadTest((::wglext_I3D_gamma != 0), ::wglext_I3D_gamma - 1);
		exts::var_I3D_genlock = glload::LoadTest((::wglext_I3D_genlock != 0), ::wglext_I3D_genlock - 1);
		exts::var_I3D_image_buffer = glload::LoadTest((::wglext_I3D_image_buffer != 0), ::wglext_I3D_image_buffer - 1);
		exts::var_I3D_swap_frame_lock = glload::LoadTest((::wglext_I3D_swap_frame_lock != 0), ::wglext_I3D_swap_frame_lock - 1);
		exts::var_I3D_swap_frame_usage = glload::LoadTest((::wglext_I3D_swap_frame_usage != 0), ::wglext_I3D_swap_frame_usage - 1);
		exts::var_NV_DX_interop = glload::LoadTest((::wglext_NV_DX_interop != 0), ::wglext_NV_DX_interop - 1);
		exts::var_NV_DX_interop2 = glload::LoadTest((::wglext_NV_DX_interop2 != 0), ::wglext_NV_DX_interop2 - 1);
		exts::var_NV_copy_image = glload::LoadTest((::wglext_NV_copy_image != 0), ::wglext_NV_copy_image - 1);
		exts::var_NV_float_buffer = glload::LoadTest((::wglext_NV_float_buffer != 0), ::wglext_NV_float_buffer - 1);
		exts::var_NV_gpu_affinity = glload::LoadTest((::wglext_NV_gpu_affinity != 0), ::wglext_NV_gpu_affinity - 1);
		exts::var_NV_multisample_coverage = glload::LoadTest((::wglext_NV_multisample_coverage != 0), ::wglext_NV_multisample_coverage - 1);
		exts::var_NV_present_video = glload::LoadTest((::wglext_NV_present_video != 0), ::wglext_NV_present_video - 1);
		exts::var_NV_render_depth_texture = glload::LoadTest((::wglext_NV_render_depth_texture != 0), ::wglext_NV_render_depth_texture - 1);
		exts::var_NV_render_texture_rectangle = glload::LoadTest((::wglext_NV_render_texture_rectangle != 0), ::wglext_NV_render_texture_rectangle - 1);
		exts::var_NV_swap_group = glload::LoadTest((::wglext_NV_swap_group != 0), ::wglext_NV_swap_group - 1);
		exts::var_NV_vertex_array_range = glload::LoadTest((::wglext_NV_vertex_array_range != 0), ::wglext_NV_vertex_array_range - 1);
		exts::var_NV_video_capture = glload::LoadTest((::wglext_NV_video_capture != 0), ::wglext_NV_video_capture - 1);
		exts::var_NV_video_output = glload::LoadTest((::wglext_NV_video_output != 0), ::wglext_NV_video_output - 1);
		exts::var_OML_sync_control = glload::LoadTest((::wglext_OML_sync_control != 0), ::wglext_OML_sync_control - 1);
	}
	
	static void CopyFunctionPointers()
	{
		SetStereoEmitterState3DL = _funcptr_wglSetStereoEmitterState3DL;
		BlitContextFramebufferAMD = _funcptr_wglBlitContextFramebufferAMD;
		CreateAssociatedContextAMD = _funcptr_wglCreateAssociatedContextAMD;
		CreateAssociatedContextAttribsAMD = _funcptr_wglCreateAssociatedContextAttribsAMD;
		DeleteAssociatedContextAMD = _funcptr_wglDeleteAssociatedContextAMD;
		GetContextGPUIDAMD = _funcptr_wglGetContextGPUIDAMD;
		GetCurrentAssociatedContextAMD = _funcptr_wglGetCurrentAssociatedContextAMD;
		GetGPUIDsAMD = _funcptr_wglGetGPUIDsAMD;
		GetGPUInfoAMD = _funcptr_wglGetGPUInfoAMD;
		MakeAssociatedContextCurrentAMD = _funcptr_wglMakeAssociatedContextCurrentAMD;
		CreateBufferRegionARB = _funcptr_wglCreateBufferRegionARB;
		DeleteBufferRegionARB = _funcptr_wglDeleteBufferRegionARB;
		RestoreBufferRegionARB = _funcptr_wglRestoreBufferRegionARB;
		SaveBufferRegionARB = _funcptr_wglSaveBufferRegionARB;
		CreateContextAttribsARB = _funcptr_wglCreateContextAttribsARB;
		GetExtensionsStringARB = _funcptr_wglGetExtensionsStringARB;
		GetCurrentReadDCARB = _funcptr_wglGetCurrentReadDCARB;
		MakeContextCurrentARB = _funcptr_wglMakeContextCurrentARB;
		CreatePbufferARB = _funcptr_wglCreatePbufferARB;
		DestroyPbufferARB = _funcptr_wglDestroyPbufferARB;
		GetPbufferDCARB = _funcptr_wglGetPbufferDCARB;
		QueryPbufferARB = _funcptr_wglQueryPbufferARB;
		ReleasePbufferDCARB = _funcptr_wglReleasePbufferDCARB;
		ChoosePixelFormatARB = _funcptr_wglChoosePixelFormatARB;
		GetPixelFormatAttribfvARB = _funcptr_wglGetPixelFormatAttribfvARB;
		GetPixelFormatAttribivARB = _funcptr_wglGetPixelFormatAttribivARB;
		BindTexImageARB = _funcptr_wglBindTexImageARB;
		ReleaseTexImageARB = _funcptr_wglReleaseTexImageARB;
		SetPbufferAttribARB = _funcptr_wglSetPbufferAttribARB;
		BindDisplayColorTableEXT = _funcptr_wglBindDisplayColorTableEXT;
		CreateDisplayColorTableEXT = _funcptr_wglCreateDisplayColorTableEXT;
		DestroyDisplayColorTableEXT = _funcptr_wglDestroyDisplayColorTableEXT;
		LoadDisplayColorTableEXT = _funcptr_wglLoadDisplayColorTableEXT;
		GetExtensionsStringEXT = _funcptr_wglGetExtensionsStringEXT;
		GetCurrentReadDCEXT = _funcptr_wglGetCurrentReadDCEXT;
		MakeContextCurrentEXT = _funcptr_wglMakeContextCurrentEXT;
		CreatePbufferEXT = _funcptr_wglCreatePbufferEXT;
		DestroyPbufferEXT = _funcptr_wglDestroyPbufferEXT;
		GetPbufferDCEXT = _funcptr_wglGetPbufferDCEXT;
		QueryPbufferEXT = _funcptr_wglQueryPbufferEXT;
		ReleasePbufferDCEXT = _funcptr_wglReleasePbufferDCEXT;
		ChoosePixelFormatEXT = _funcptr_wglChoosePixelFormatEXT;
		GetPixelFormatAttribfvEXT = _funcptr_wglGetPixelFormatAttribfvEXT;
		GetPixelFormatAttribivEXT = _funcptr_wglGetPixelFormatAttribivEXT;
		GetSwapIntervalEXT = _funcptr_wglGetSwapIntervalEXT;
		SwapIntervalEXT = _funcptr_wglSwapIntervalEXT;
		GetDigitalVideoParametersI3D = _funcptr_wglGetDigitalVideoParametersI3D;
		SetDigitalVideoParametersI3D = _funcptr_wglSetDigitalVideoParametersI3D;
		GetGammaTableI3D = _funcptr_wglGetGammaTableI3D;
		GetGammaTableParametersI3D = _funcptr_wglGetGammaTableParametersI3D;
		SetGammaTableI3D = _funcptr_wglSetGammaTableI3D;
		SetGammaTableParametersI3D = _funcptr_wglSetGammaTableParametersI3D;
		DisableGenlockI3D = _funcptr_wglDisableGenlockI3D;
		EnableGenlockI3D = _funcptr_wglEnableGenlockI3D;
		GenlockSampleRateI3D = _funcptr_wglGenlockSampleRateI3D;
		GenlockSourceDelayI3D = _funcptr_wglGenlockSourceDelayI3D;
		GenlockSourceEdgeI3D = _funcptr_wglGenlockSourceEdgeI3D;
		GenlockSourceI3D = _funcptr_wglGenlockSourceI3D;
		GetGenlockSampleRateI3D = _funcptr_wglGetGenlockSampleRateI3D;
		GetGenlockSourceDelayI3D = _funcptr_wglGetGenlockSourceDelayI3D;
		GetGenlockSourceEdgeI3D = _funcptr_wglGetGenlockSourceEdgeI3D;
		GetGenlockSourceI3D = _funcptr_wglGetGenlockSourceI3D;
		IsEnabledGenlockI3D = _funcptr_wglIsEnabledGenlockI3D;
		QueryGenlockMaxSourceDelayI3D = _funcptr_wglQueryGenlockMaxSourceDelayI3D;
		AssociateImageBufferEventsI3D = _funcptr_wglAssociateImageBufferEventsI3D;
		CreateImageBufferI3D = _funcptr_wglCreateImageBufferI3D;
		DestroyImageBufferI3D = _funcptr_wglDestroyImageBufferI3D;
		ReleaseImageBufferEventsI3D = _funcptr_wglReleaseImageBufferEventsI3D;
		DisableFrameLockI3D = _funcptr_wglDisableFrameLockI3D;
		EnableFrameLockI3D = _funcptr_wglEnableFrameLockI3D;
		IsEnabledFrameLockI3D = _funcptr_wglIsEnabledFrameLockI3D;
		QueryFrameLockMasterI3D = _funcptr_wglQueryFrameLockMasterI3D;
		BeginFrameTrackingI3D = _funcptr_wglBeginFrameTrackingI3D;
		EndFrameTrackingI3D = _funcptr_wglEndFrameTrackingI3D;
		GetFrameUsageI3D = _funcptr_wglGetFrameUsageI3D;
		QueryFrameTrackingI3D = _funcptr_wglQueryFrameTrackingI3D;
		DXCloseDeviceNV = _funcptr_wglDXCloseDeviceNV;
		DXLockObjectsNV = _funcptr_wglDXLockObjectsNV;
		DXObjectAccessNV = _funcptr_wglDXObjectAccessNV;
		DXOpenDeviceNV = _funcptr_wglDXOpenDeviceNV;
		DXRegisterObjectNV = _funcptr_wglDXRegisterObjectNV;
		DXSetResourceShareHandleNV = _funcptr_wglDXSetResourceShareHandleNV;
		DXUnlockObjectsNV = _funcptr_wglDXUnlockObjectsNV;
		DXUnregisterObjectNV = _funcptr_wglDXUnregisterObjectNV;
		CopyImageSubDataNV = _funcptr_wglCopyImageSubDataNV;
		CreateAffinityDCNV = _funcptr_wglCreateAffinityDCNV;
		DeleteDCNV = _funcptr_wglDeleteDCNV;
		EnumGpuDevicesNV = _funcptr_wglEnumGpuDevicesNV;
		EnumGpusFromAffinityDCNV = _funcptr_wglEnumGpusFromAffinityDCNV;
		EnumGpusNV = _funcptr_wglEnumGpusNV;
		BindVideoDeviceNV = _funcptr_wglBindVideoDeviceNV;
		EnumerateVideoDevicesNV = _funcptr_wglEnumerateVideoDevicesNV;
		QueryCurrentContextNV = _funcptr_wglQueryCurrentContextNV;
		BindSwapBarrierNV = _funcptr_wglBindSwapBarrierNV;
		JoinSwapGroupNV = _funcptr_wglJoinSwapGroupNV;
		QueryFrameCountNV = _funcptr_wglQueryFrameCountNV;
		QueryMaxSwapGroupsNV = _funcptr_wglQueryMaxSwapGroupsNV;
		QuerySwapGroupNV = _funcptr_wglQuerySwapGroupNV;
		ResetFrameCountNV = _funcptr_wglResetFrameCountNV;
		AllocateMemoryNV = _funcptr_wglAllocateMemoryNV;
		FreeMemoryNV = _funcptr_wglFreeMemoryNV;
		BindVideoCaptureDeviceNV = _funcptr_wglBindVideoCaptureDeviceNV;
		EnumerateVideoCaptureDevicesNV = _funcptr_wglEnumerateVideoCaptureDevicesNV;
		LockVideoCaptureDeviceNV = _funcptr_wglLockVideoCaptureDeviceNV;
		QueryVideoCaptureDeviceNV = _funcptr_wglQueryVideoCaptureDeviceNV;
		ReleaseVideoCaptureDeviceNV = _funcptr_wglReleaseVideoCaptureDeviceNV;
		BindVideoImageNV = _funcptr_wglBindVideoImageNV;
		GetVideoDeviceNV = _funcptr_wglGetVideoDeviceNV;
		GetVideoInfoNV = _funcptr_wglGetVideoInfoNV;
		ReleaseVideoDeviceNV = _funcptr_wglReleaseVideoDeviceNV;
		ReleaseVideoImageNV = _funcptr_wglReleaseVideoImageNV;
		SendPbufferToVideoNV = _funcptr_wglSendPbufferToVideoNV;
		GetMscRateOML = _funcptr_wglGetMscRateOML;
		GetSyncValuesOML = _funcptr_wglGetSyncValuesOML;
		SwapBuffersMscOML = _funcptr_wglSwapBuffersMscOML;
		SwapLayerBuffersMscOML = _funcptr_wglSwapLayerBuffersMscOML;
		WaitForMscOML = _funcptr_wglWaitForMscOML;
		WaitForSbcOML = _funcptr_wglWaitForSbcOML;
	}
	
}
extern "C" void wgl_CopyFromC()
{
  wgl::CopyExtensionVariables();
  wgl::CopyFunctionPointers();
}

namespace glload
{
	glload::LoadTest LoadFunctions(HDC hdc)
	{
		int test = ::wgl_LoadFunctions(hdc);
		//The C loader will call the above function to copy the variables and such.
		
		if(test == 0)
			return glload::LoadTest(false, 0);
		return glload::LoadTest(true, test - 1);
	}
	
}
