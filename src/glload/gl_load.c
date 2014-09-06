#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#if defined( __clang__ )
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wtypedef-redefinition"
#endif
	#include "glload/gl_all.h"
	#include "glload/gl_load.h"
#if defined( __clang__ )
	#pragma clang diagnostic pop
#endif

#if defined(__APPLE__)
#include <mach-o/dyld.h>

#pragma clang diagnostic ignored "-Wpointer-sign"

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

int glext_ARB_imaging = 0;
int glext_ARB_vertex_array_object = 0;
int glext_ARB_texture_rg = 0;
int glext_ARB_texture_compression_rgtc = 0;
int glext_ARB_map_buffer_range = 0;
int glext_ARB_half_float_vertex = 0;
int glext_ARB_framebuffer_sRGB = 0;
int glext_ARB_framebuffer_object = 0;
int glext_ARB_depth_buffer_float = 0;
int glext_ARB_uniform_buffer_object = 0;
int glext_ARB_copy_buffer = 0;
int glext_ARB_depth_clamp = 0;
int glext_ARB_draw_elements_base_vertex = 0;
int glext_ARB_fragment_coord_conventions = 0;
int glext_ARB_provoking_vertex = 0;
int glext_ARB_seamless_cube_map = 0;
int glext_ARB_sync = 0;
int glext_ARB_texture_multisample = 0;
int glext_ARB_vertex_array_bgra = 0;
int glext_ARB_texture_rgb10_a2ui = 0;
int glext_ARB_texture_swizzle = 0;
int glext_ARB_timer_query = 0;
int glext_ARB_vertex_type_2_10_10_10_rev = 0;
int glext_ARB_blend_func_extended = 0;
int glext_ARB_occlusion_query2 = 0;
int glext_ARB_sampler_objects = 0;
int glext_ARB_draw_indirect = 0;
int glext_ARB_gpu_shader5 = 0;
int glext_ARB_gpu_shader_fp64 = 0;
int glext_ARB_shader_subroutine = 0;
int glext_ARB_tessellation_shader = 0;
int glext_ARB_transform_feedback2 = 0;
int glext_ARB_transform_feedback3 = 0;
int glext_ARB_ES2_compatibility = 0;
int glext_ARB_get_program_binary = 0;
int glext_ARB_separate_shader_objects = 0;
int glext_ARB_vertex_attrib_64bit = 0;
int glext_ARB_viewport_array = 0;
int glext_ARB_base_instance = 0;
int glext_ARB_shading_language_420pack = 0;
int glext_ARB_transform_feedback_instanced = 0;
int glext_ARB_compressed_texture_pixel_storage = 0;
int glext_ARB_conservative_depth = 0;
int glext_ARB_internalformat_query = 0;
int glext_ARB_map_buffer_alignment = 0;
int glext_ARB_shader_atomic_counters = 0;
int glext_ARB_shader_image_load_store = 0;
int glext_ARB_shading_language_packing = 0;
int glext_ARB_texture_storage = 0;
int glext_KHR_debug = 0;
int glext_ARB_arrays_of_arrays = 0;
int glext_ARB_clear_buffer_object = 0;
int glext_ARB_compute_shader = 0;
int glext_ARB_copy_image = 0;
int glext_ARB_ES3_compatibility = 0;
int glext_ARB_explicit_uniform_location = 0;
int glext_ARB_fragment_layer_viewport = 0;
int glext_ARB_framebuffer_no_attachments = 0;
int glext_ARB_internalformat_query2 = 0;
int glext_ARB_invalidate_subdata = 0;
int glext_ARB_multi_draw_indirect = 0;
int glext_ARB_program_interface_query = 0;
int glext_ARB_shader_image_size = 0;
int glext_ARB_shader_storage_buffer_object = 0;
int glext_ARB_stencil_texturing = 0;
int glext_ARB_texture_buffer_range = 0;
int glext_ARB_texture_query_levels = 0;
int glext_ARB_texture_storage_multisample = 0;
int glext_ARB_texture_view = 0;
int glext_ARB_vertex_attrib_binding = 0;
int glext_3DFX_multisample = 0;
int glext_3DFX_tbuffer = 0;
int glext_3DFX_texture_compression_FXT1 = 0;
int glext_AMD_blend_minmax_factor = 0;
int glext_AMD_conservative_depth = 0;
int glext_AMD_debug_output = 0;
int glext_AMD_depth_clamp_separate = 0;
int glext_AMD_draw_buffers_blend = 0;
int glext_AMD_multi_draw_indirect = 0;
int glext_AMD_name_gen_delete = 0;
int glext_AMD_performance_monitor = 0;
int glext_AMD_pinned_memory = 0;
int glext_AMD_query_buffer_object = 0;
int glext_AMD_sample_positions = 0;
int glext_AMD_seamless_cubemap_per_texture = 0;
int glext_AMD_shader_stencil_export = 0;
int glext_AMD_shader_trinary_minmax = 0;
int glext_AMD_sparse_texture = 0;
int glext_AMD_stencil_operation_extended = 0;
int glext_AMD_texture_texture4 = 0;
int glext_AMD_transform_feedback3_lines_triangles = 0;
int glext_AMD_vertex_shader_layer = 0;
int glext_AMD_vertex_shader_tessellator = 0;
int glext_AMD_vertex_shader_viewport_index = 0;
int glext_APPLE_aux_depth_stencil = 0;
int glext_APPLE_client_storage = 0;
int glext_APPLE_element_array = 0;
int glext_APPLE_fence = 0;
int glext_APPLE_float_pixels = 0;
int glext_APPLE_flush_buffer_range = 0;
int glext_APPLE_object_purgeable = 0;
int glext_APPLE_rgb_422 = 0;
int glext_APPLE_row_bytes = 0;
int glext_APPLE_specular_vector = 0;
int glext_APPLE_texture_range = 0;
int glext_APPLE_transform_hint = 0;
int glext_APPLE_vertex_array_object = 0;
int glext_APPLE_vertex_array_range = 0;
int glext_APPLE_vertex_program_evaluators = 0;
int glext_APPLE_ycbcr_422 = 0;
int glext_ARB_cl_event = 0;
int glext_ARB_color_buffer_float = 0;
int glext_ARB_compatibility = 0;
int glext_ARB_debug_output = 0;
int glext_ARB_depth_texture = 0;
int glext_ARB_draw_buffers = 0;
int glext_ARB_draw_buffers_blend = 0;
int glext_ARB_draw_instanced = 0;
int glext_ARB_explicit_attrib_location = 0;
int glext_ARB_fragment_program = 0;
int glext_ARB_fragment_program_shadow = 0;
int glext_ARB_fragment_shader = 0;
int glext_ARB_geometry_shader4 = 0;
int glext_ARB_half_float_pixel = 0;
int glext_ARB_instanced_arrays = 0;
int glext_ARB_matrix_palette = 0;
int glext_ARB_multisample = 0;
int glext_ARB_multitexture = 0;
int glext_ARB_occlusion_query = 0;
int glext_ARB_pixel_buffer_object = 0;
int glext_ARB_point_parameters = 0;
int glext_ARB_point_sprite = 0;
int glext_ARB_robust_buffer_access_behavior = 0;
int glext_ARB_robustness = 0;
int glext_ARB_robustness_isolation = 0;
int glext_ARB_sample_shading = 0;
int glext_ARB_shader_bit_encoding = 0;
int glext_ARB_shader_objects = 0;
int glext_ARB_shader_precision = 0;
int glext_ARB_shader_stencil_export = 0;
int glext_ARB_shader_texture_lod = 0;
int glext_ARB_shading_language_100 = 0;
int glext_ARB_shading_language_include = 0;
int glext_ARB_shadow = 0;
int glext_ARB_shadow_ambient = 0;
int glext_ARB_texture_border_clamp = 0;
int glext_ARB_texture_buffer_object = 0;
int glext_ARB_texture_buffer_object_rgb32 = 0;
int glext_ARB_texture_compression = 0;
int glext_ARB_texture_compression_bptc = 0;
int glext_ARB_texture_cube_map = 0;
int glext_ARB_texture_cube_map_array = 0;
int glext_ARB_texture_env_add = 0;
int glext_ARB_texture_env_combine = 0;
int glext_ARB_texture_env_crossbar = 0;
int glext_ARB_texture_env_dot3 = 0;
int glext_ARB_texture_float = 0;
int glext_ARB_texture_gather = 0;
int glext_ARB_texture_mirrored_repeat = 0;
int glext_ARB_texture_non_power_of_two = 0;
int glext_ARB_texture_query_lod = 0;
int glext_ARB_texture_rectangle = 0;
int glext_ARB_transpose_matrix = 0;
int glext_ARB_vertex_blend = 0;
int glext_ARB_vertex_buffer_object = 0;
int glext_ARB_vertex_program = 0;
int glext_ARB_vertex_shader = 0;
int glext_ARB_window_pos = 0;
int glext_ATI_draw_buffers = 0;
int glext_ATI_element_array = 0;
int glext_ATI_envmap_bumpmap = 0;
int glext_ATI_fragment_shader = 0;
int glext_ATI_map_object_buffer = 0;
int glext_ATI_meminfo = 0;
int glext_ATI_pixel_format_float = 0;
int glext_ATI_pn_triangles = 0;
int glext_ATI_separate_stencil = 0;
int glext_ATI_text_fragment_shader = 0;
int glext_ATI_texture_env_combine3 = 0;
int glext_ATI_texture_float = 0;
int glext_ATI_texture_mirror_once = 0;
int glext_ATI_vertex_array_object = 0;
int glext_ATI_vertex_attrib_array_object = 0;
int glext_ATI_vertex_streams = 0;
int glext_EXT_422_pixels = 0;
int glext_EXT_abgr = 0;
int glext_EXT_bgra = 0;
int glext_EXT_bindable_uniform = 0;
int glext_EXT_blend_color = 0;
int glext_EXT_blend_equation_separate = 0;
int glext_EXT_blend_func_separate = 0;
int glext_EXT_blend_logic_op = 0;
int glext_EXT_blend_minmax = 0;
int glext_EXT_blend_subtract = 0;
int glext_EXT_clip_volume_hint = 0;
int glext_EXT_cmyka = 0;
int glext_EXT_color_subtable = 0;
int glext_EXT_compiled_vertex_array = 0;
int glext_EXT_convolution = 0;
int glext_EXT_coordinate_frame = 0;
int glext_EXT_copy_texture = 0;
int glext_EXT_cull_vertex = 0;
int glext_EXT_depth_bounds_test = 0;
int glext_EXT_direct_state_access = 0;
int glext_EXT_draw_buffers2 = 0;
int glext_EXT_draw_instanced = 0;
int glext_EXT_draw_range_elements = 0;
int glext_EXT_fog_coord = 0;
int glext_EXT_framebuffer_blit = 0;
int glext_EXT_framebuffer_multisample = 0;
int glext_EXT_framebuffer_multisample_blit_scaled = 0;
int glext_EXT_framebuffer_object = 0;
int glext_EXT_framebuffer_sRGB = 0;
int glext_EXT_geometry_shader4 = 0;
int glext_EXT_gpu_program_parameters = 0;
int glext_EXT_gpu_shader4 = 0;
int glext_EXT_histogram = 0;
int glext_EXT_index_array_formats = 0;
int glext_EXT_index_func = 0;
int glext_EXT_index_material = 0;
int glext_EXT_index_texture = 0;
int glext_EXT_light_texture = 0;
int glext_EXT_misc_attribute = 0;
int glext_EXT_multi_draw_arrays = 0;
int glext_EXT_multisample = 0;
int glext_EXT_packed_depth_stencil = 0;
int glext_EXT_packed_float = 0;
int glext_EXT_packed_pixels = 0;
int glext_EXT_paletted_texture = 0;
int glext_EXT_pixel_buffer_object = 0;
int glext_EXT_pixel_transform = 0;
int glext_EXT_pixel_transform_color_table = 0;
int glext_EXT_point_parameters = 0;
int glext_EXT_polygon_offset = 0;
int glext_EXT_provoking_vertex = 0;
int glext_EXT_rescale_normal = 0;
int glext_EXT_secondary_color = 0;
int glext_EXT_separate_shader_objects = 0;
int glext_EXT_separate_specular_color = 0;
int glext_EXT_shader_image_load_store = 0;
int glext_EXT_shadow_funcs = 0;
int glext_EXT_shared_texture_palette = 0;
int glext_EXT_stencil_clear_tag = 0;
int glext_EXT_stencil_two_side = 0;
int glext_EXT_stencil_wrap = 0;
int glext_EXT_subtexture = 0;
int glext_EXT_texture = 0;
int glext_EXT_texture3D = 0;
int glext_EXT_texture_array = 0;
int glext_EXT_texture_buffer_object = 0;
int glext_EXT_texture_compression_latc = 0;
int glext_EXT_texture_compression_rgtc = 0;
int glext_EXT_texture_compression_s3tc = 0;
int glext_EXT_texture_cube_map = 0;
int glext_EXT_texture_env_add = 0;
int glext_EXT_texture_env_combine = 0;
int glext_EXT_texture_env_dot3 = 0;
int glext_EXT_texture_filter_anisotropic = 0;
int glext_EXT_texture_integer = 0;
int glext_EXT_texture_lod_bias = 0;
int glext_EXT_texture_mirror_clamp = 0;
int glext_EXT_texture_object = 0;
int glext_EXT_texture_perturb_normal = 0;
int glext_EXT_texture_sRGB = 0;
int glext_EXT_texture_sRGB_decode = 0;
int glext_EXT_texture_shared_exponent = 0;
int glext_EXT_texture_snorm = 0;
int glext_EXT_texture_swizzle = 0;
int glext_EXT_timer_query = 0;
int glext_EXT_transform_feedback = 0;
int glext_EXT_vertex_array = 0;
int glext_EXT_vertex_array_bgra = 0;
int glext_EXT_vertex_attrib_64bit = 0;
int glext_EXT_vertex_shader = 0;
int glext_EXT_vertex_weighting = 0;
int glext_EXT_x11_sync_object = 0;
int glext_GREMEDY_frame_terminator = 0;
int glext_GREMEDY_string_marker = 0;
int glext_HP_convolution_border_modes = 0;
int glext_HP_image_transform = 0;
int glext_HP_occlusion_test = 0;
int glext_HP_texture_lighting = 0;
int glext_IBM_cull_vertex = 0;
int glext_IBM_multimode_draw_arrays = 0;
int glext_IBM_rasterpos_clip = 0;
int glext_IBM_static_data = 0;
int glext_IBM_texture_mirrored_repeat = 0;
int glext_IBM_vertex_array_lists = 0;
int glext_INGR_color_clamp = 0;
int glext_INGR_interlace_read = 0;
int glext_INGR_palette_buffer = 0;
int glext_INTEL_map_texture = 0;
int glext_INTEL_parallel_arrays = 0;
int glext_INTEL_texture_scissor = 0;
int glext_KHR_texture_compression_astc_ldr = 0;
int glext_MESAX_texture_stack = 0;
int glext_MESA_pack_invert = 0;
int glext_MESA_resize_buffers = 0;
int glext_MESA_window_pos = 0;
int glext_MESA_ycbcr_texture = 0;
int glext_NVX_conditional_render = 0;
int glext_NV_bindless_texture = 0;
int glext_NV_blend_square = 0;
int glext_NV_compute_program5 = 0;
int glext_NV_conditional_render = 0;
int glext_NV_copy_depth_to_color = 0;
int glext_NV_copy_image = 0;
int glext_NV_deep_texture3D = 0;
int glext_NV_depth_buffer_float = 0;
int glext_NV_depth_clamp = 0;
int glext_NV_draw_texture = 0;
int glext_NV_evaluators = 0;
int glext_NV_explicit_multisample = 0;
int glext_NV_fence = 0;
int glext_NV_float_buffer = 0;
int glext_NV_fog_distance = 0;
int glext_NV_fragment_program = 0;
int glext_NV_fragment_program2 = 0;
int glext_NV_fragment_program4 = 0;
int glext_NV_fragment_program_option = 0;
int glext_NV_framebuffer_multisample_coverage = 0;
int glext_NV_geometry_program4 = 0;
int glext_NV_geometry_shader4 = 0;
int glext_NV_gpu_program4 = 0;
int glext_NV_gpu_program5 = 0;
int glext_NV_gpu_shader5 = 0;
int glext_NV_half_float = 0;
int glext_NV_light_max_exponent = 0;
int glext_NV_multisample_coverage = 0;
int glext_NV_multisample_filter_hint = 0;
int glext_NV_occlusion_query = 0;
int glext_NV_packed_depth_stencil = 0;
int glext_NV_parameter_buffer_object = 0;
int glext_NV_parameter_buffer_object2 = 0;
int glext_NV_path_rendering = 0;
int glext_NV_pixel_data_range = 0;
int glext_NV_point_sprite = 0;
int glext_NV_present_video = 0;
int glext_NV_primitive_restart = 0;
int glext_NV_register_combiners = 0;
int glext_NV_register_combiners2 = 0;
int glext_NV_shader_atomic_counters = 0;
int glext_NV_shader_atomic_float = 0;
int glext_NV_shader_buffer_load = 0;
int glext_NV_shader_buffer_store = 0;
int glext_NV_shader_storage_buffer_object = 0;
int glext_NV_tessellation_program5 = 0;
int glext_NV_texgen_emboss = 0;
int glext_NV_texgen_reflection = 0;
int glext_NV_texture_barrier = 0;
int glext_NV_texture_compression_vtc = 0;
int glext_NV_texture_env_combine4 = 0;
int glext_NV_texture_expand_normal = 0;
int glext_NV_texture_multisample = 0;
int glext_NV_texture_rectangle = 0;
int glext_NV_texture_shader = 0;
int glext_NV_texture_shader2 = 0;
int glext_NV_texture_shader3 = 0;
int glext_NV_transform_feedback = 0;
int glext_NV_transform_feedback2 = 0;
int glext_NV_vdpau_interop = 0;
int glext_NV_vertex_array_range = 0;
int glext_NV_vertex_array_range2 = 0;
int glext_NV_vertex_attrib_integer_64bit = 0;
int glext_NV_vertex_buffer_unified_memory = 0;
int glext_NV_vertex_program = 0;
int glext_NV_vertex_program1_1 = 0;
int glext_NV_vertex_program2 = 0;
int glext_NV_vertex_program2_option = 0;
int glext_NV_vertex_program3 = 0;
int glext_NV_vertex_program4 = 0;
int glext_NV_video_capture = 0;
int glext_OES_byte_coordinates = 0;
int glext_OES_compressed_paletted_texture = 0;
int glext_OES_fixed_point = 0;
int glext_OES_query_matrix = 0;
int glext_OES_read_format = 0;
int glext_OES_single_precision = 0;
int glext_OML_interlace = 0;
int glext_OML_resample = 0;
int glext_OML_subsample = 0;
int glext_PGI_misc_hints = 0;
int glext_PGI_vertex_hints = 0;
int glext_REND_screen_coordinates = 0;
int glext_S3_s3tc = 0;
int glext_SGIS_detail_texture = 0;
int glext_SGIS_fog_function = 0;
int glext_SGIS_generate_mipmap = 0;
int glext_SGIS_multisample = 0;
int glext_SGIS_pixel_texture = 0;
int glext_SGIS_point_line_texgen = 0;
int glext_SGIS_point_parameters = 0;
int glext_SGIS_sharpen_texture = 0;
int glext_SGIS_texture4D = 0;
int glext_SGIS_texture_border_clamp = 0;
int glext_SGIS_texture_color_mask = 0;
int glext_SGIS_texture_edge_clamp = 0;
int glext_SGIS_texture_filter4 = 0;
int glext_SGIS_texture_lod = 0;
int glext_SGIS_texture_select = 0;
int glext_SGIX_async = 0;
int glext_SGIX_async_histogram = 0;
int glext_SGIX_async_pixel = 0;
int glext_SGIX_blend_alpha_minmax = 0;
int glext_SGIX_calligraphic_fragment = 0;
int glext_SGIX_clipmap = 0;
int glext_SGIX_convolution_accuracy = 0;
int glext_SGIX_depth_texture = 0;
int glext_SGIX_flush_raster = 0;
int glext_SGIX_fog_offset = 0;
int glext_SGIX_fragment_lighting = 0;
int glext_SGIX_framezoom = 0;
int glext_SGIX_instruments = 0;
int glext_SGIX_interlace = 0;
int glext_SGIX_ir_instrument1 = 0;
int glext_SGIX_list_priority = 0;
int glext_SGIX_pixel_texture = 0;
int glext_SGIX_pixel_tiles = 0;
int glext_SGIX_polynomial_ffd = 0;
int glext_SGIX_reference_plane = 0;
int glext_SGIX_resample = 0;
int glext_SGIX_scalebias_hint = 0;
int glext_SGIX_shadow = 0;
int glext_SGIX_shadow_ambient = 0;
int glext_SGIX_sprite = 0;
int glext_SGIX_subsample = 0;
int glext_SGIX_tag_sample_buffer = 0;
int glext_SGIX_texture_add_env = 0;
int glext_SGIX_texture_coordinate_clamp = 0;
int glext_SGIX_texture_lod_bias = 0;
int glext_SGIX_texture_multi_buffer = 0;
int glext_SGIX_texture_scale_bias = 0;
int glext_SGIX_vertex_preclip = 0;
int glext_SGIX_ycrcb = 0;
int glext_SGIX_ycrcb_subsample = 0;
int glext_SGIX_ycrcba = 0;
int glext_SGI_color_matrix = 0;
int glext_SGI_color_table = 0;
int glext_SGI_texture_color_table = 0;
int glext_SUNX_constant_data = 0;
int glext_SUN_convolution_border_modes = 0;
int glext_SUN_global_alpha = 0;
int glext_SUN_mesh_array = 0;
int glext_SUN_slice_accum = 0;
int glext_SUN_triangle_list = 0;
int glext_SUN_vertex = 0;
int glext_WIN_phong_shading = 0;
int glext_WIN_specular_fog = 0;

PFNGLBINDVERTEXARRAYPROC _funcptr_glBindVertexArray = NULL;
PFNGLDELETEVERTEXARRAYSPROC _funcptr_glDeleteVertexArrays = NULL;
PFNGLGENVERTEXARRAYSPROC _funcptr_glGenVertexArrays = NULL;
PFNGLISVERTEXARRAYPROC _funcptr_glIsVertexArray = NULL;

static int LoadExt_ARB_vertex_array_object()
{
	int numFailed = 0;
	_funcptr_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)IntGetProcAddress("glBindVertexArray");
	if(!_funcptr_glBindVertexArray) ++numFailed;
	_funcptr_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)IntGetProcAddress("glDeleteVertexArrays");
	if(!_funcptr_glDeleteVertexArrays) ++numFailed;
	_funcptr_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)IntGetProcAddress("glGenVertexArrays");
	if(!_funcptr_glGenVertexArrays) ++numFailed;
	_funcptr_glIsVertexArray = (PFNGLISVERTEXARRAYPROC)IntGetProcAddress("glIsVertexArray");
	if(!_funcptr_glIsVertexArray) ++numFailed;
	return numFailed;
}

PFNGLMAPBUFFERRANGEPROC _funcptr_glMapBufferRange = NULL;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC _funcptr_glFlushMappedBufferRange = NULL;

static int LoadExt_ARB_map_buffer_range()
{
	int numFailed = 0;
	_funcptr_glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)IntGetProcAddress("glMapBufferRange");
	if(!_funcptr_glMapBufferRange) ++numFailed;
	_funcptr_glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC)IntGetProcAddress("glFlushMappedBufferRange");
	if(!_funcptr_glFlushMappedBufferRange) ++numFailed;
	return numFailed;
}

PFNGLISRENDERBUFFERPROC _funcptr_glIsRenderbuffer = NULL;
PFNGLBINDRENDERBUFFERPROC _funcptr_glBindRenderbuffer = NULL;
PFNGLDELETERENDERBUFFERSPROC _funcptr_glDeleteRenderbuffers = NULL;
PFNGLGENRENDERBUFFERSPROC _funcptr_glGenRenderbuffers = NULL;
PFNGLRENDERBUFFERSTORAGEPROC _funcptr_glRenderbufferStorage = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVPROC _funcptr_glGetRenderbufferParameteriv = NULL;
PFNGLISFRAMEBUFFERPROC _funcptr_glIsFramebuffer = NULL;
PFNGLBINDFRAMEBUFFERPROC _funcptr_glBindFramebuffer = NULL;
PFNGLDELETEFRAMEBUFFERSPROC _funcptr_glDeleteFramebuffers = NULL;
PFNGLGENFRAMEBUFFERSPROC _funcptr_glGenFramebuffers = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSPROC _funcptr_glCheckFramebufferStatus = NULL;
PFNGLFRAMEBUFFERTEXTURE1DPROC _funcptr_glFramebufferTexture1D = NULL;
PFNGLFRAMEBUFFERTEXTURE2DPROC _funcptr_glFramebufferTexture2D = NULL;
PFNGLFRAMEBUFFERTEXTURE3DPROC _funcptr_glFramebufferTexture3D = NULL;
PFNGLFRAMEBUFFERRENDERBUFFERPROC _funcptr_glFramebufferRenderbuffer = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC _funcptr_glGetFramebufferAttachmentParameteriv = NULL;
PFNGLGENERATEMIPMAPPROC _funcptr_glGenerateMipmap = NULL;
PFNGLBLITFRAMEBUFFERPROC _funcptr_glBlitFramebuffer = NULL;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC _funcptr_glRenderbufferStorageMultisample = NULL;
PFNGLFRAMEBUFFERTEXTURELAYERPROC _funcptr_glFramebufferTextureLayer = NULL;

static int LoadExt_ARB_framebuffer_object()
{
	int numFailed = 0;
	_funcptr_glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)IntGetProcAddress("glIsRenderbuffer");
	if(!_funcptr_glIsRenderbuffer) ++numFailed;
	_funcptr_glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)IntGetProcAddress("glBindRenderbuffer");
	if(!_funcptr_glBindRenderbuffer) ++numFailed;
	_funcptr_glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)IntGetProcAddress("glDeleteRenderbuffers");
	if(!_funcptr_glDeleteRenderbuffers) ++numFailed;
	_funcptr_glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)IntGetProcAddress("glGenRenderbuffers");
	if(!_funcptr_glGenRenderbuffers) ++numFailed;
	_funcptr_glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)IntGetProcAddress("glRenderbufferStorage");
	if(!_funcptr_glRenderbufferStorage) ++numFailed;
	_funcptr_glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)IntGetProcAddress("glGetRenderbufferParameteriv");
	if(!_funcptr_glGetRenderbufferParameteriv) ++numFailed;
	_funcptr_glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)IntGetProcAddress("glIsFramebuffer");
	if(!_funcptr_glIsFramebuffer) ++numFailed;
	_funcptr_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)IntGetProcAddress("glBindFramebuffer");
	if(!_funcptr_glBindFramebuffer) ++numFailed;
	_funcptr_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)IntGetProcAddress("glDeleteFramebuffers");
	if(!_funcptr_glDeleteFramebuffers) ++numFailed;
	_funcptr_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)IntGetProcAddress("glGenFramebuffers");
	if(!_funcptr_glGenFramebuffers) ++numFailed;
	_funcptr_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)IntGetProcAddress("glCheckFramebufferStatus");
	if(!_funcptr_glCheckFramebufferStatus) ++numFailed;
	_funcptr_glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)IntGetProcAddress("glFramebufferTexture1D");
	if(!_funcptr_glFramebufferTexture1D) ++numFailed;
	_funcptr_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)IntGetProcAddress("glFramebufferTexture2D");
	if(!_funcptr_glFramebufferTexture2D) ++numFailed;
	_funcptr_glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)IntGetProcAddress("glFramebufferTexture3D");
	if(!_funcptr_glFramebufferTexture3D) ++numFailed;
	_funcptr_glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)IntGetProcAddress("glFramebufferRenderbuffer");
	if(!_funcptr_glFramebufferRenderbuffer) ++numFailed;
	_funcptr_glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)IntGetProcAddress("glGetFramebufferAttachmentParameteriv");
	if(!_funcptr_glGetFramebufferAttachmentParameteriv) ++numFailed;
	_funcptr_glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)IntGetProcAddress("glGenerateMipmap");
	if(!_funcptr_glGenerateMipmap) ++numFailed;
	_funcptr_glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)IntGetProcAddress("glBlitFramebuffer");
	if(!_funcptr_glBlitFramebuffer) ++numFailed;
	_funcptr_glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)IntGetProcAddress("glRenderbufferStorageMultisample");
	if(!_funcptr_glRenderbufferStorageMultisample) ++numFailed;
	_funcptr_glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)IntGetProcAddress("glFramebufferTextureLayer");
	if(!_funcptr_glFramebufferTextureLayer) ++numFailed;
	return numFailed;
}

PFNGLGETUNIFORMINDICESPROC _funcptr_glGetUniformIndices = NULL;
PFNGLGETACTIVEUNIFORMSIVPROC _funcptr_glGetActiveUniformsiv = NULL;
PFNGLGETACTIVEUNIFORMNAMEPROC _funcptr_glGetActiveUniformName = NULL;
PFNGLGETUNIFORMBLOCKINDEXPROC _funcptr_glGetUniformBlockIndex = NULL;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC _funcptr_glGetActiveUniformBlockiv = NULL;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC _funcptr_glGetActiveUniformBlockName = NULL;
PFNGLUNIFORMBLOCKBINDINGPROC _funcptr_glUniformBlockBinding = NULL;

static int LoadExt_ARB_uniform_buffer_object()
{
	int numFailed = 0;
	_funcptr_glGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC)IntGetProcAddress("glGetUniformIndices");
	if(!_funcptr_glGetUniformIndices) ++numFailed;
	_funcptr_glGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC)IntGetProcAddress("glGetActiveUniformsiv");
	if(!_funcptr_glGetActiveUniformsiv) ++numFailed;
	_funcptr_glGetActiveUniformName = (PFNGLGETACTIVEUNIFORMNAMEPROC)IntGetProcAddress("glGetActiveUniformName");
	if(!_funcptr_glGetActiveUniformName) ++numFailed;
	_funcptr_glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC)IntGetProcAddress("glGetUniformBlockIndex");
	if(!_funcptr_glGetUniformBlockIndex) ++numFailed;
	_funcptr_glGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC)IntGetProcAddress("glGetActiveUniformBlockiv");
	if(!_funcptr_glGetActiveUniformBlockiv) ++numFailed;
	_funcptr_glGetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC)IntGetProcAddress("glGetActiveUniformBlockName");
	if(!_funcptr_glGetActiveUniformBlockName) ++numFailed;
	_funcptr_glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC)IntGetProcAddress("glUniformBlockBinding");
	if(!_funcptr_glUniformBlockBinding) ++numFailed;
	return numFailed;
}

PFNGLCOPYBUFFERSUBDATAPROC _funcptr_glCopyBufferSubData = NULL;

static int LoadExt_ARB_copy_buffer()
{
	int numFailed = 0;
	_funcptr_glCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC)IntGetProcAddress("glCopyBufferSubData");
	if(!_funcptr_glCopyBufferSubData) ++numFailed;
	return numFailed;
}

PFNGLDRAWELEMENTSBASEVERTEXPROC _funcptr_glDrawElementsBaseVertex = NULL;
PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC _funcptr_glDrawRangeElementsBaseVertex = NULL;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC _funcptr_glDrawElementsInstancedBaseVertex = NULL;
PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC _funcptr_glMultiDrawElementsBaseVertex = NULL;

static int LoadExt_ARB_draw_elements_base_vertex()
{
	int numFailed = 0;
	_funcptr_glDrawElementsBaseVertex = (PFNGLDRAWELEMENTSBASEVERTEXPROC)IntGetProcAddress("glDrawElementsBaseVertex");
	if(!_funcptr_glDrawElementsBaseVertex) ++numFailed;
	_funcptr_glDrawRangeElementsBaseVertex = (PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC)IntGetProcAddress("glDrawRangeElementsBaseVertex");
	if(!_funcptr_glDrawRangeElementsBaseVertex) ++numFailed;
	_funcptr_glDrawElementsInstancedBaseVertex = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC)IntGetProcAddress("glDrawElementsInstancedBaseVertex");
	if(!_funcptr_glDrawElementsInstancedBaseVertex) ++numFailed;
	_funcptr_glMultiDrawElementsBaseVertex = (PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC)IntGetProcAddress("glMultiDrawElementsBaseVertex");
	if(!_funcptr_glMultiDrawElementsBaseVertex) ++numFailed;
	return numFailed;
}

PFNGLPROVOKINGVERTEXPROC _funcptr_glProvokingVertex = NULL;

static int LoadExt_ARB_provoking_vertex()
{
	int numFailed = 0;
	_funcptr_glProvokingVertex = (PFNGLPROVOKINGVERTEXPROC)IntGetProcAddress("glProvokingVertex");
	if(!_funcptr_glProvokingVertex) ++numFailed;
	return numFailed;
}

PFNGLFENCESYNCPROC _funcptr_glFenceSync = NULL;
PFNGLISSYNCPROC _funcptr_glIsSync = NULL;
PFNGLDELETESYNCPROC _funcptr_glDeleteSync = NULL;
PFNGLCLIENTWAITSYNCPROC _funcptr_glClientWaitSync = NULL;
PFNGLWAITSYNCPROC _funcptr_glWaitSync = NULL;
PFNGLGETINTEGER64VPROC _funcptr_glGetInteger64v = NULL;
PFNGLGETSYNCIVPROC _funcptr_glGetSynciv = NULL;

static int LoadExt_ARB_sync()
{
	int numFailed = 0;
	_funcptr_glFenceSync = (PFNGLFENCESYNCPROC)IntGetProcAddress("glFenceSync");
	if(!_funcptr_glFenceSync) ++numFailed;
	_funcptr_glIsSync = (PFNGLISSYNCPROC)IntGetProcAddress("glIsSync");
	if(!_funcptr_glIsSync) ++numFailed;
	_funcptr_glDeleteSync = (PFNGLDELETESYNCPROC)IntGetProcAddress("glDeleteSync");
	if(!_funcptr_glDeleteSync) ++numFailed;
	_funcptr_glClientWaitSync = (PFNGLCLIENTWAITSYNCPROC)IntGetProcAddress("glClientWaitSync");
	if(!_funcptr_glClientWaitSync) ++numFailed;
	_funcptr_glWaitSync = (PFNGLWAITSYNCPROC)IntGetProcAddress("glWaitSync");
	if(!_funcptr_glWaitSync) ++numFailed;
	_funcptr_glGetInteger64v = (PFNGLGETINTEGER64VPROC)IntGetProcAddress("glGetInteger64v");
	if(!_funcptr_glGetInteger64v) ++numFailed;
	_funcptr_glGetSynciv = (PFNGLGETSYNCIVPROC)IntGetProcAddress("glGetSynciv");
	if(!_funcptr_glGetSynciv) ++numFailed;
	return numFailed;
}

PFNGLTEXIMAGE2DMULTISAMPLEPROC _funcptr_glTexImage2DMultisample = NULL;
PFNGLTEXIMAGE3DMULTISAMPLEPROC _funcptr_glTexImage3DMultisample = NULL;
PFNGLGETMULTISAMPLEFVPROC _funcptr_glGetMultisamplefv = NULL;
PFNGLSAMPLEMASKIPROC _funcptr_glSampleMaski = NULL;

static int LoadExt_ARB_texture_multisample()
{
	int numFailed = 0;
	_funcptr_glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)IntGetProcAddress("glTexImage2DMultisample");
	if(!_funcptr_glTexImage2DMultisample) ++numFailed;
	_funcptr_glTexImage3DMultisample = (PFNGLTEXIMAGE3DMULTISAMPLEPROC)IntGetProcAddress("glTexImage3DMultisample");
	if(!_funcptr_glTexImage3DMultisample) ++numFailed;
	_funcptr_glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC)IntGetProcAddress("glGetMultisamplefv");
	if(!_funcptr_glGetMultisamplefv) ++numFailed;
	_funcptr_glSampleMaski = (PFNGLSAMPLEMASKIPROC)IntGetProcAddress("glSampleMaski");
	if(!_funcptr_glSampleMaski) ++numFailed;
	return numFailed;
}

PFNGLQUERYCOUNTERPROC _funcptr_glQueryCounter = NULL;
PFNGLGETQUERYOBJECTI64VPROC _funcptr_glGetQueryObjecti64v = NULL;
PFNGLGETQUERYOBJECTUI64VPROC _funcptr_glGetQueryObjectui64v = NULL;

static int LoadExt_ARB_timer_query()
{
	int numFailed = 0;
	_funcptr_glQueryCounter = (PFNGLQUERYCOUNTERPROC)IntGetProcAddress("glQueryCounter");
	if(!_funcptr_glQueryCounter) ++numFailed;
	_funcptr_glGetQueryObjecti64v = (PFNGLGETQUERYOBJECTI64VPROC)IntGetProcAddress("glGetQueryObjecti64v");
	if(!_funcptr_glGetQueryObjecti64v) ++numFailed;
	_funcptr_glGetQueryObjectui64v = (PFNGLGETQUERYOBJECTUI64VPROC)IntGetProcAddress("glGetQueryObjectui64v");
	if(!_funcptr_glGetQueryObjectui64v) ++numFailed;
	return numFailed;
}

PFNGLVERTEXP2UIPROC _funcptr_glVertexP2ui = NULL;
PFNGLVERTEXP2UIVPROC _funcptr_glVertexP2uiv = NULL;
PFNGLVERTEXP3UIPROC _funcptr_glVertexP3ui = NULL;
PFNGLVERTEXP3UIVPROC _funcptr_glVertexP3uiv = NULL;
PFNGLVERTEXP4UIPROC _funcptr_glVertexP4ui = NULL;
PFNGLVERTEXP4UIVPROC _funcptr_glVertexP4uiv = NULL;
PFNGLTEXCOORDP1UIPROC _funcptr_glTexCoordP1ui = NULL;
PFNGLTEXCOORDP1UIVPROC _funcptr_glTexCoordP1uiv = NULL;
PFNGLTEXCOORDP2UIPROC _funcptr_glTexCoordP2ui = NULL;
PFNGLTEXCOORDP2UIVPROC _funcptr_glTexCoordP2uiv = NULL;
PFNGLTEXCOORDP3UIPROC _funcptr_glTexCoordP3ui = NULL;
PFNGLTEXCOORDP3UIVPROC _funcptr_glTexCoordP3uiv = NULL;
PFNGLTEXCOORDP4UIPROC _funcptr_glTexCoordP4ui = NULL;
PFNGLTEXCOORDP4UIVPROC _funcptr_glTexCoordP4uiv = NULL;
PFNGLMULTITEXCOORDP1UIPROC _funcptr_glMultiTexCoordP1ui = NULL;
PFNGLMULTITEXCOORDP1UIVPROC _funcptr_glMultiTexCoordP1uiv = NULL;
PFNGLMULTITEXCOORDP2UIPROC _funcptr_glMultiTexCoordP2ui = NULL;
PFNGLMULTITEXCOORDP2UIVPROC _funcptr_glMultiTexCoordP2uiv = NULL;
PFNGLMULTITEXCOORDP3UIPROC _funcptr_glMultiTexCoordP3ui = NULL;
PFNGLMULTITEXCOORDP3UIVPROC _funcptr_glMultiTexCoordP3uiv = NULL;
PFNGLMULTITEXCOORDP4UIPROC _funcptr_glMultiTexCoordP4ui = NULL;
PFNGLMULTITEXCOORDP4UIVPROC _funcptr_glMultiTexCoordP4uiv = NULL;
PFNGLNORMALP3UIPROC _funcptr_glNormalP3ui = NULL;
PFNGLNORMALP3UIVPROC _funcptr_glNormalP3uiv = NULL;
PFNGLCOLORP3UIPROC _funcptr_glColorP3ui = NULL;
PFNGLCOLORP3UIVPROC _funcptr_glColorP3uiv = NULL;
PFNGLCOLORP4UIPROC _funcptr_glColorP4ui = NULL;
PFNGLCOLORP4UIVPROC _funcptr_glColorP4uiv = NULL;
PFNGLSECONDARYCOLORP3UIPROC _funcptr_glSecondaryColorP3ui = NULL;
PFNGLSECONDARYCOLORP3UIVPROC _funcptr_glSecondaryColorP3uiv = NULL;
PFNGLVERTEXATTRIBP1UIPROC _funcptr_glVertexAttribP1ui = NULL;
PFNGLVERTEXATTRIBP1UIVPROC _funcptr_glVertexAttribP1uiv = NULL;
PFNGLVERTEXATTRIBP2UIPROC _funcptr_glVertexAttribP2ui = NULL;
PFNGLVERTEXATTRIBP2UIVPROC _funcptr_glVertexAttribP2uiv = NULL;
PFNGLVERTEXATTRIBP3UIPROC _funcptr_glVertexAttribP3ui = NULL;
PFNGLVERTEXATTRIBP3UIVPROC _funcptr_glVertexAttribP3uiv = NULL;
PFNGLVERTEXATTRIBP4UIPROC _funcptr_glVertexAttribP4ui = NULL;
PFNGLVERTEXATTRIBP4UIVPROC _funcptr_glVertexAttribP4uiv = NULL;

static int LoadExt_ARB_vertex_type_2_10_10_10_rev()
{
	int numFailed = 0;
	_funcptr_glVertexP2ui = (PFNGLVERTEXP2UIPROC)IntGetProcAddress("glVertexP2ui");
	if(!_funcptr_glVertexP2ui) ++numFailed;
	_funcptr_glVertexP2uiv = (PFNGLVERTEXP2UIVPROC)IntGetProcAddress("glVertexP2uiv");
	if(!_funcptr_glVertexP2uiv) ++numFailed;
	_funcptr_glVertexP3ui = (PFNGLVERTEXP3UIPROC)IntGetProcAddress("glVertexP3ui");
	if(!_funcptr_glVertexP3ui) ++numFailed;
	_funcptr_glVertexP3uiv = (PFNGLVERTEXP3UIVPROC)IntGetProcAddress("glVertexP3uiv");
	if(!_funcptr_glVertexP3uiv) ++numFailed;
	_funcptr_glVertexP4ui = (PFNGLVERTEXP4UIPROC)IntGetProcAddress("glVertexP4ui");
	if(!_funcptr_glVertexP4ui) ++numFailed;
	_funcptr_glVertexP4uiv = (PFNGLVERTEXP4UIVPROC)IntGetProcAddress("glVertexP4uiv");
	if(!_funcptr_glVertexP4uiv) ++numFailed;
	_funcptr_glTexCoordP1ui = (PFNGLTEXCOORDP1UIPROC)IntGetProcAddress("glTexCoordP1ui");
	if(!_funcptr_glTexCoordP1ui) ++numFailed;
	_funcptr_glTexCoordP1uiv = (PFNGLTEXCOORDP1UIVPROC)IntGetProcAddress("glTexCoordP1uiv");
	if(!_funcptr_glTexCoordP1uiv) ++numFailed;
	_funcptr_glTexCoordP2ui = (PFNGLTEXCOORDP2UIPROC)IntGetProcAddress("glTexCoordP2ui");
	if(!_funcptr_glTexCoordP2ui) ++numFailed;
	_funcptr_glTexCoordP2uiv = (PFNGLTEXCOORDP2UIVPROC)IntGetProcAddress("glTexCoordP2uiv");
	if(!_funcptr_glTexCoordP2uiv) ++numFailed;
	_funcptr_glTexCoordP3ui = (PFNGLTEXCOORDP3UIPROC)IntGetProcAddress("glTexCoordP3ui");
	if(!_funcptr_glTexCoordP3ui) ++numFailed;
	_funcptr_glTexCoordP3uiv = (PFNGLTEXCOORDP3UIVPROC)IntGetProcAddress("glTexCoordP3uiv");
	if(!_funcptr_glTexCoordP3uiv) ++numFailed;
	_funcptr_glTexCoordP4ui = (PFNGLTEXCOORDP4UIPROC)IntGetProcAddress("glTexCoordP4ui");
	if(!_funcptr_glTexCoordP4ui) ++numFailed;
	_funcptr_glTexCoordP4uiv = (PFNGLTEXCOORDP4UIVPROC)IntGetProcAddress("glTexCoordP4uiv");
	if(!_funcptr_glTexCoordP4uiv) ++numFailed;
	_funcptr_glMultiTexCoordP1ui = (PFNGLMULTITEXCOORDP1UIPROC)IntGetProcAddress("glMultiTexCoordP1ui");
	if(!_funcptr_glMultiTexCoordP1ui) ++numFailed;
	_funcptr_glMultiTexCoordP1uiv = (PFNGLMULTITEXCOORDP1UIVPROC)IntGetProcAddress("glMultiTexCoordP1uiv");
	if(!_funcptr_glMultiTexCoordP1uiv) ++numFailed;
	_funcptr_glMultiTexCoordP2ui = (PFNGLMULTITEXCOORDP2UIPROC)IntGetProcAddress("glMultiTexCoordP2ui");
	if(!_funcptr_glMultiTexCoordP2ui) ++numFailed;
	_funcptr_glMultiTexCoordP2uiv = (PFNGLMULTITEXCOORDP2UIVPROC)IntGetProcAddress("glMultiTexCoordP2uiv");
	if(!_funcptr_glMultiTexCoordP2uiv) ++numFailed;
	_funcptr_glMultiTexCoordP3ui = (PFNGLMULTITEXCOORDP3UIPROC)IntGetProcAddress("glMultiTexCoordP3ui");
	if(!_funcptr_glMultiTexCoordP3ui) ++numFailed;
	_funcptr_glMultiTexCoordP3uiv = (PFNGLMULTITEXCOORDP3UIVPROC)IntGetProcAddress("glMultiTexCoordP3uiv");
	if(!_funcptr_glMultiTexCoordP3uiv) ++numFailed;
	_funcptr_glMultiTexCoordP4ui = (PFNGLMULTITEXCOORDP4UIPROC)IntGetProcAddress("glMultiTexCoordP4ui");
	if(!_funcptr_glMultiTexCoordP4ui) ++numFailed;
	_funcptr_glMultiTexCoordP4uiv = (PFNGLMULTITEXCOORDP4UIVPROC)IntGetProcAddress("glMultiTexCoordP4uiv");
	if(!_funcptr_glMultiTexCoordP4uiv) ++numFailed;
	_funcptr_glNormalP3ui = (PFNGLNORMALP3UIPROC)IntGetProcAddress("glNormalP3ui");
	if(!_funcptr_glNormalP3ui) ++numFailed;
	_funcptr_glNormalP3uiv = (PFNGLNORMALP3UIVPROC)IntGetProcAddress("glNormalP3uiv");
	if(!_funcptr_glNormalP3uiv) ++numFailed;
	_funcptr_glColorP3ui = (PFNGLCOLORP3UIPROC)IntGetProcAddress("glColorP3ui");
	if(!_funcptr_glColorP3ui) ++numFailed;
	_funcptr_glColorP3uiv = (PFNGLCOLORP3UIVPROC)IntGetProcAddress("glColorP3uiv");
	if(!_funcptr_glColorP3uiv) ++numFailed;
	_funcptr_glColorP4ui = (PFNGLCOLORP4UIPROC)IntGetProcAddress("glColorP4ui");
	if(!_funcptr_glColorP4ui) ++numFailed;
	_funcptr_glColorP4uiv = (PFNGLCOLORP4UIVPROC)IntGetProcAddress("glColorP4uiv");
	if(!_funcptr_glColorP4uiv) ++numFailed;
	_funcptr_glSecondaryColorP3ui = (PFNGLSECONDARYCOLORP3UIPROC)IntGetProcAddress("glSecondaryColorP3ui");
	if(!_funcptr_glSecondaryColorP3ui) ++numFailed;
	_funcptr_glSecondaryColorP3uiv = (PFNGLSECONDARYCOLORP3UIVPROC)IntGetProcAddress("glSecondaryColorP3uiv");
	if(!_funcptr_glSecondaryColorP3uiv) ++numFailed;
	_funcptr_glVertexAttribP1ui = (PFNGLVERTEXATTRIBP1UIPROC)IntGetProcAddress("glVertexAttribP1ui");
	if(!_funcptr_glVertexAttribP1ui) ++numFailed;
	_funcptr_glVertexAttribP1uiv = (PFNGLVERTEXATTRIBP1UIVPROC)IntGetProcAddress("glVertexAttribP1uiv");
	if(!_funcptr_glVertexAttribP1uiv) ++numFailed;
	_funcptr_glVertexAttribP2ui = (PFNGLVERTEXATTRIBP2UIPROC)IntGetProcAddress("glVertexAttribP2ui");
	if(!_funcptr_glVertexAttribP2ui) ++numFailed;
	_funcptr_glVertexAttribP2uiv = (PFNGLVERTEXATTRIBP2UIVPROC)IntGetProcAddress("glVertexAttribP2uiv");
	if(!_funcptr_glVertexAttribP2uiv) ++numFailed;
	_funcptr_glVertexAttribP3ui = (PFNGLVERTEXATTRIBP3UIPROC)IntGetProcAddress("glVertexAttribP3ui");
	if(!_funcptr_glVertexAttribP3ui) ++numFailed;
	_funcptr_glVertexAttribP3uiv = (PFNGLVERTEXATTRIBP3UIVPROC)IntGetProcAddress("glVertexAttribP3uiv");
	if(!_funcptr_glVertexAttribP3uiv) ++numFailed;
	_funcptr_glVertexAttribP4ui = (PFNGLVERTEXATTRIBP4UIPROC)IntGetProcAddress("glVertexAttribP4ui");
	if(!_funcptr_glVertexAttribP4ui) ++numFailed;
	_funcptr_glVertexAttribP4uiv = (PFNGLVERTEXATTRIBP4UIVPROC)IntGetProcAddress("glVertexAttribP4uiv");
	if(!_funcptr_glVertexAttribP4uiv) ++numFailed;
	return numFailed;
}

PFNGLBINDFRAGDATALOCATIONINDEXEDPROC _funcptr_glBindFragDataLocationIndexed = NULL;
PFNGLGETFRAGDATAINDEXPROC _funcptr_glGetFragDataIndex = NULL;

static int LoadExt_ARB_blend_func_extended()
{
	int numFailed = 0;
	_funcptr_glBindFragDataLocationIndexed = (PFNGLBINDFRAGDATALOCATIONINDEXEDPROC)IntGetProcAddress("glBindFragDataLocationIndexed");
	if(!_funcptr_glBindFragDataLocationIndexed) ++numFailed;
	_funcptr_glGetFragDataIndex = (PFNGLGETFRAGDATAINDEXPROC)IntGetProcAddress("glGetFragDataIndex");
	if(!_funcptr_glGetFragDataIndex) ++numFailed;
	return numFailed;
}

PFNGLGENSAMPLERSPROC _funcptr_glGenSamplers = NULL;
PFNGLDELETESAMPLERSPROC _funcptr_glDeleteSamplers = NULL;
PFNGLISSAMPLERPROC _funcptr_glIsSampler = NULL;
PFNGLBINDSAMPLERPROC _funcptr_glBindSampler = NULL;
PFNGLSAMPLERPARAMETERIPROC _funcptr_glSamplerParameteri = NULL;
PFNGLSAMPLERPARAMETERIVPROC _funcptr_glSamplerParameteriv = NULL;
PFNGLSAMPLERPARAMETERFPROC _funcptr_glSamplerParameterf = NULL;
PFNGLSAMPLERPARAMETERFVPROC _funcptr_glSamplerParameterfv = NULL;
PFNGLSAMPLERPARAMETERIIVPROC _funcptr_glSamplerParameterIiv = NULL;
PFNGLSAMPLERPARAMETERIUIVPROC _funcptr_glSamplerParameterIuiv = NULL;
PFNGLGETSAMPLERPARAMETERIVPROC _funcptr_glGetSamplerParameteriv = NULL;
PFNGLGETSAMPLERPARAMETERIIVPROC _funcptr_glGetSamplerParameterIiv = NULL;
PFNGLGETSAMPLERPARAMETERFVPROC _funcptr_glGetSamplerParameterfv = NULL;
PFNGLGETSAMPLERPARAMETERIUIVPROC _funcptr_glGetSamplerParameterIuiv = NULL;

static int LoadExt_ARB_sampler_objects()
{
	int numFailed = 0;
	_funcptr_glGenSamplers = (PFNGLGENSAMPLERSPROC)IntGetProcAddress("glGenSamplers");
	if(!_funcptr_glGenSamplers) ++numFailed;
	_funcptr_glDeleteSamplers = (PFNGLDELETESAMPLERSPROC)IntGetProcAddress("glDeleteSamplers");
	if(!_funcptr_glDeleteSamplers) ++numFailed;
	_funcptr_glIsSampler = (PFNGLISSAMPLERPROC)IntGetProcAddress("glIsSampler");
	if(!_funcptr_glIsSampler) ++numFailed;
	_funcptr_glBindSampler = (PFNGLBINDSAMPLERPROC)IntGetProcAddress("glBindSampler");
	if(!_funcptr_glBindSampler) ++numFailed;
	_funcptr_glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)IntGetProcAddress("glSamplerParameteri");
	if(!_funcptr_glSamplerParameteri) ++numFailed;
	_funcptr_glSamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC)IntGetProcAddress("glSamplerParameteriv");
	if(!_funcptr_glSamplerParameteriv) ++numFailed;
	_funcptr_glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC)IntGetProcAddress("glSamplerParameterf");
	if(!_funcptr_glSamplerParameterf) ++numFailed;
	_funcptr_glSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC)IntGetProcAddress("glSamplerParameterfv");
	if(!_funcptr_glSamplerParameterfv) ++numFailed;
	_funcptr_glSamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIVPROC)IntGetProcAddress("glSamplerParameterIiv");
	if(!_funcptr_glSamplerParameterIiv) ++numFailed;
	_funcptr_glSamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIVPROC)IntGetProcAddress("glSamplerParameterIuiv");
	if(!_funcptr_glSamplerParameterIuiv) ++numFailed;
	_funcptr_glGetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC)IntGetProcAddress("glGetSamplerParameteriv");
	if(!_funcptr_glGetSamplerParameteriv) ++numFailed;
	_funcptr_glGetSamplerParameterIiv = (PFNGLGETSAMPLERPARAMETERIIVPROC)IntGetProcAddress("glGetSamplerParameterIiv");
	if(!_funcptr_glGetSamplerParameterIiv) ++numFailed;
	_funcptr_glGetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC)IntGetProcAddress("glGetSamplerParameterfv");
	if(!_funcptr_glGetSamplerParameterfv) ++numFailed;
	_funcptr_glGetSamplerParameterIuiv = (PFNGLGETSAMPLERPARAMETERIUIVPROC)IntGetProcAddress("glGetSamplerParameterIuiv");
	if(!_funcptr_glGetSamplerParameterIuiv) ++numFailed;
	return numFailed;
}

PFNGLDRAWARRAYSINDIRECTPROC _funcptr_glDrawArraysIndirect = NULL;
PFNGLDRAWELEMENTSINDIRECTPROC _funcptr_glDrawElementsIndirect = NULL;

static int LoadExt_ARB_draw_indirect()
{
	int numFailed = 0;
	_funcptr_glDrawArraysIndirect = (PFNGLDRAWARRAYSINDIRECTPROC)IntGetProcAddress("glDrawArraysIndirect");
	if(!_funcptr_glDrawArraysIndirect) ++numFailed;
	_funcptr_glDrawElementsIndirect = (PFNGLDRAWELEMENTSINDIRECTPROC)IntGetProcAddress("glDrawElementsIndirect");
	if(!_funcptr_glDrawElementsIndirect) ++numFailed;
	return numFailed;
}

PFNGLUNIFORM1DPROC _funcptr_glUniform1d = NULL;
PFNGLUNIFORM2DPROC _funcptr_glUniform2d = NULL;
PFNGLUNIFORM3DPROC _funcptr_glUniform3d = NULL;
PFNGLUNIFORM4DPROC _funcptr_glUniform4d = NULL;
PFNGLUNIFORM1DVPROC _funcptr_glUniform1dv = NULL;
PFNGLUNIFORM2DVPROC _funcptr_glUniform2dv = NULL;
PFNGLUNIFORM3DVPROC _funcptr_glUniform3dv = NULL;
PFNGLUNIFORM4DVPROC _funcptr_glUniform4dv = NULL;
PFNGLUNIFORMMATRIX2DVPROC _funcptr_glUniformMatrix2dv = NULL;
PFNGLUNIFORMMATRIX3DVPROC _funcptr_glUniformMatrix3dv = NULL;
PFNGLUNIFORMMATRIX4DVPROC _funcptr_glUniformMatrix4dv = NULL;
PFNGLUNIFORMMATRIX2X3DVPROC _funcptr_glUniformMatrix2x3dv = NULL;
PFNGLUNIFORMMATRIX2X4DVPROC _funcptr_glUniformMatrix2x4dv = NULL;
PFNGLUNIFORMMATRIX3X2DVPROC _funcptr_glUniformMatrix3x2dv = NULL;
PFNGLUNIFORMMATRIX3X4DVPROC _funcptr_glUniformMatrix3x4dv = NULL;
PFNGLUNIFORMMATRIX4X2DVPROC _funcptr_glUniformMatrix4x2dv = NULL;
PFNGLUNIFORMMATRIX4X3DVPROC _funcptr_glUniformMatrix4x3dv = NULL;
PFNGLGETUNIFORMDVPROC _funcptr_glGetUniformdv = NULL;

static int LoadExt_ARB_gpu_shader_fp64()
{
	int numFailed = 0;
	_funcptr_glUniform1d = (PFNGLUNIFORM1DPROC)IntGetProcAddress("glUniform1d");
	if(!_funcptr_glUniform1d) ++numFailed;
	_funcptr_glUniform2d = (PFNGLUNIFORM2DPROC)IntGetProcAddress("glUniform2d");
	if(!_funcptr_glUniform2d) ++numFailed;
	_funcptr_glUniform3d = (PFNGLUNIFORM3DPROC)IntGetProcAddress("glUniform3d");
	if(!_funcptr_glUniform3d) ++numFailed;
	_funcptr_glUniform4d = (PFNGLUNIFORM4DPROC)IntGetProcAddress("glUniform4d");
	if(!_funcptr_glUniform4d) ++numFailed;
	_funcptr_glUniform1dv = (PFNGLUNIFORM1DVPROC)IntGetProcAddress("glUniform1dv");
	if(!_funcptr_glUniform1dv) ++numFailed;
	_funcptr_glUniform2dv = (PFNGLUNIFORM2DVPROC)IntGetProcAddress("glUniform2dv");
	if(!_funcptr_glUniform2dv) ++numFailed;
	_funcptr_glUniform3dv = (PFNGLUNIFORM3DVPROC)IntGetProcAddress("glUniform3dv");
	if(!_funcptr_glUniform3dv) ++numFailed;
	_funcptr_glUniform4dv = (PFNGLUNIFORM4DVPROC)IntGetProcAddress("glUniform4dv");
	if(!_funcptr_glUniform4dv) ++numFailed;
	_funcptr_glUniformMatrix2dv = (PFNGLUNIFORMMATRIX2DVPROC)IntGetProcAddress("glUniformMatrix2dv");
	if(!_funcptr_glUniformMatrix2dv) ++numFailed;
	_funcptr_glUniformMatrix3dv = (PFNGLUNIFORMMATRIX3DVPROC)IntGetProcAddress("glUniformMatrix3dv");
	if(!_funcptr_glUniformMatrix3dv) ++numFailed;
	_funcptr_glUniformMatrix4dv = (PFNGLUNIFORMMATRIX4DVPROC)IntGetProcAddress("glUniformMatrix4dv");
	if(!_funcptr_glUniformMatrix4dv) ++numFailed;
	_funcptr_glUniformMatrix2x3dv = (PFNGLUNIFORMMATRIX2X3DVPROC)IntGetProcAddress("glUniformMatrix2x3dv");
	if(!_funcptr_glUniformMatrix2x3dv) ++numFailed;
	_funcptr_glUniformMatrix2x4dv = (PFNGLUNIFORMMATRIX2X4DVPROC)IntGetProcAddress("glUniformMatrix2x4dv");
	if(!_funcptr_glUniformMatrix2x4dv) ++numFailed;
	_funcptr_glUniformMatrix3x2dv = (PFNGLUNIFORMMATRIX3X2DVPROC)IntGetProcAddress("glUniformMatrix3x2dv");
	if(!_funcptr_glUniformMatrix3x2dv) ++numFailed;
	_funcptr_glUniformMatrix3x4dv = (PFNGLUNIFORMMATRIX3X4DVPROC)IntGetProcAddress("glUniformMatrix3x4dv");
	if(!_funcptr_glUniformMatrix3x4dv) ++numFailed;
	_funcptr_glUniformMatrix4x2dv = (PFNGLUNIFORMMATRIX4X2DVPROC)IntGetProcAddress("glUniformMatrix4x2dv");
	if(!_funcptr_glUniformMatrix4x2dv) ++numFailed;
	_funcptr_glUniformMatrix4x3dv = (PFNGLUNIFORMMATRIX4X3DVPROC)IntGetProcAddress("glUniformMatrix4x3dv");
	if(!_funcptr_glUniformMatrix4x3dv) ++numFailed;
	_funcptr_glGetUniformdv = (PFNGLGETUNIFORMDVPROC)IntGetProcAddress("glGetUniformdv");
	if(!_funcptr_glGetUniformdv) ++numFailed;
	return numFailed;
}

PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC _funcptr_glGetSubroutineUniformLocation = NULL;
PFNGLGETSUBROUTINEINDEXPROC _funcptr_glGetSubroutineIndex = NULL;
PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC _funcptr_glGetActiveSubroutineUniformiv = NULL;
PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC _funcptr_glGetActiveSubroutineUniformName = NULL;
PFNGLGETACTIVESUBROUTINENAMEPROC _funcptr_glGetActiveSubroutineName = NULL;
PFNGLUNIFORMSUBROUTINESUIVPROC _funcptr_glUniformSubroutinesuiv = NULL;
PFNGLGETUNIFORMSUBROUTINEUIVPROC _funcptr_glGetUniformSubroutineuiv = NULL;
PFNGLGETPROGRAMSTAGEIVPROC _funcptr_glGetProgramStageiv = NULL;

static int LoadExt_ARB_shader_subroutine()
{
	int numFailed = 0;
	_funcptr_glGetSubroutineUniformLocation = (PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC)IntGetProcAddress("glGetSubroutineUniformLocation");
	if(!_funcptr_glGetSubroutineUniformLocation) ++numFailed;
	_funcptr_glGetSubroutineIndex = (PFNGLGETSUBROUTINEINDEXPROC)IntGetProcAddress("glGetSubroutineIndex");
	if(!_funcptr_glGetSubroutineIndex) ++numFailed;
	_funcptr_glGetActiveSubroutineUniformiv = (PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC)IntGetProcAddress("glGetActiveSubroutineUniformiv");
	if(!_funcptr_glGetActiveSubroutineUniformiv) ++numFailed;
	_funcptr_glGetActiveSubroutineUniformName = (PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC)IntGetProcAddress("glGetActiveSubroutineUniformName");
	if(!_funcptr_glGetActiveSubroutineUniformName) ++numFailed;
	_funcptr_glGetActiveSubroutineName = (PFNGLGETACTIVESUBROUTINENAMEPROC)IntGetProcAddress("glGetActiveSubroutineName");
	if(!_funcptr_glGetActiveSubroutineName) ++numFailed;
	_funcptr_glUniformSubroutinesuiv = (PFNGLUNIFORMSUBROUTINESUIVPROC)IntGetProcAddress("glUniformSubroutinesuiv");
	if(!_funcptr_glUniformSubroutinesuiv) ++numFailed;
	_funcptr_glGetUniformSubroutineuiv = (PFNGLGETUNIFORMSUBROUTINEUIVPROC)IntGetProcAddress("glGetUniformSubroutineuiv");
	if(!_funcptr_glGetUniformSubroutineuiv) ++numFailed;
	_funcptr_glGetProgramStageiv = (PFNGLGETPROGRAMSTAGEIVPROC)IntGetProcAddress("glGetProgramStageiv");
	if(!_funcptr_glGetProgramStageiv) ++numFailed;
	return numFailed;
}

PFNGLPATCHPARAMETERIPROC _funcptr_glPatchParameteri = NULL;
PFNGLPATCHPARAMETERFVPROC _funcptr_glPatchParameterfv = NULL;

static int LoadExt_ARB_tessellation_shader()
{
	int numFailed = 0;
	_funcptr_glPatchParameteri = (PFNGLPATCHPARAMETERIPROC)IntGetProcAddress("glPatchParameteri");
	if(!_funcptr_glPatchParameteri) ++numFailed;
	_funcptr_glPatchParameterfv = (PFNGLPATCHPARAMETERFVPROC)IntGetProcAddress("glPatchParameterfv");
	if(!_funcptr_glPatchParameterfv) ++numFailed;
	return numFailed;
}

PFNGLBINDTRANSFORMFEEDBACKPROC _funcptr_glBindTransformFeedback = NULL;
PFNGLDELETETRANSFORMFEEDBACKSPROC _funcptr_glDeleteTransformFeedbacks = NULL;
PFNGLGENTRANSFORMFEEDBACKSPROC _funcptr_glGenTransformFeedbacks = NULL;
PFNGLISTRANSFORMFEEDBACKPROC _funcptr_glIsTransformFeedback = NULL;
PFNGLPAUSETRANSFORMFEEDBACKPROC _funcptr_glPauseTransformFeedback = NULL;
PFNGLRESUMETRANSFORMFEEDBACKPROC _funcptr_glResumeTransformFeedback = NULL;
PFNGLDRAWTRANSFORMFEEDBACKPROC _funcptr_glDrawTransformFeedback = NULL;

static int LoadExt_ARB_transform_feedback2()
{
	int numFailed = 0;
	_funcptr_glBindTransformFeedback = (PFNGLBINDTRANSFORMFEEDBACKPROC)IntGetProcAddress("glBindTransformFeedback");
	if(!_funcptr_glBindTransformFeedback) ++numFailed;
	_funcptr_glDeleteTransformFeedbacks = (PFNGLDELETETRANSFORMFEEDBACKSPROC)IntGetProcAddress("glDeleteTransformFeedbacks");
	if(!_funcptr_glDeleteTransformFeedbacks) ++numFailed;
	_funcptr_glGenTransformFeedbacks = (PFNGLGENTRANSFORMFEEDBACKSPROC)IntGetProcAddress("glGenTransformFeedbacks");
	if(!_funcptr_glGenTransformFeedbacks) ++numFailed;
	_funcptr_glIsTransformFeedback = (PFNGLISTRANSFORMFEEDBACKPROC)IntGetProcAddress("glIsTransformFeedback");
	if(!_funcptr_glIsTransformFeedback) ++numFailed;
	_funcptr_glPauseTransformFeedback = (PFNGLPAUSETRANSFORMFEEDBACKPROC)IntGetProcAddress("glPauseTransformFeedback");
	if(!_funcptr_glPauseTransformFeedback) ++numFailed;
	_funcptr_glResumeTransformFeedback = (PFNGLRESUMETRANSFORMFEEDBACKPROC)IntGetProcAddress("glResumeTransformFeedback");
	if(!_funcptr_glResumeTransformFeedback) ++numFailed;
	_funcptr_glDrawTransformFeedback = (PFNGLDRAWTRANSFORMFEEDBACKPROC)IntGetProcAddress("glDrawTransformFeedback");
	if(!_funcptr_glDrawTransformFeedback) ++numFailed;
	return numFailed;
}

PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC _funcptr_glDrawTransformFeedbackStream = NULL;
PFNGLBEGINQUERYINDEXEDPROC _funcptr_glBeginQueryIndexed = NULL;
PFNGLENDQUERYINDEXEDPROC _funcptr_glEndQueryIndexed = NULL;
PFNGLGETQUERYINDEXEDIVPROC _funcptr_glGetQueryIndexediv = NULL;

static int LoadExt_ARB_transform_feedback3()
{
	int numFailed = 0;
	_funcptr_glDrawTransformFeedbackStream = (PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC)IntGetProcAddress("glDrawTransformFeedbackStream");
	if(!_funcptr_glDrawTransformFeedbackStream) ++numFailed;
	_funcptr_glBeginQueryIndexed = (PFNGLBEGINQUERYINDEXEDPROC)IntGetProcAddress("glBeginQueryIndexed");
	if(!_funcptr_glBeginQueryIndexed) ++numFailed;
	_funcptr_glEndQueryIndexed = (PFNGLENDQUERYINDEXEDPROC)IntGetProcAddress("glEndQueryIndexed");
	if(!_funcptr_glEndQueryIndexed) ++numFailed;
	_funcptr_glGetQueryIndexediv = (PFNGLGETQUERYINDEXEDIVPROC)IntGetProcAddress("glGetQueryIndexediv");
	if(!_funcptr_glGetQueryIndexediv) ++numFailed;
	return numFailed;
}

PFNGLRELEASESHADERCOMPILERPROC _funcptr_glReleaseShaderCompiler = NULL;
PFNGLSHADERBINARYPROC _funcptr_glShaderBinary = NULL;
PFNGLGETSHADERPRECISIONFORMATPROC _funcptr_glGetShaderPrecisionFormat = NULL;
PFNGLDEPTHRANGEFPROC _funcptr_glDepthRangef = NULL;
PFNGLCLEARDEPTHFPROC _funcptr_glClearDepthf = NULL;

static int LoadExt_ARB_ES2_compatibility()
{
	int numFailed = 0;
	_funcptr_glReleaseShaderCompiler = (PFNGLRELEASESHADERCOMPILERPROC)IntGetProcAddress("glReleaseShaderCompiler");
	if(!_funcptr_glReleaseShaderCompiler) ++numFailed;
	_funcptr_glShaderBinary = (PFNGLSHADERBINARYPROC)IntGetProcAddress("glShaderBinary");
	if(!_funcptr_glShaderBinary) ++numFailed;
	_funcptr_glGetShaderPrecisionFormat = (PFNGLGETSHADERPRECISIONFORMATPROC)IntGetProcAddress("glGetShaderPrecisionFormat");
	if(!_funcptr_glGetShaderPrecisionFormat) ++numFailed;
	_funcptr_glDepthRangef = (PFNGLDEPTHRANGEFPROC)IntGetProcAddress("glDepthRangef");
	if(!_funcptr_glDepthRangef) ++numFailed;
	_funcptr_glClearDepthf = (PFNGLCLEARDEPTHFPROC)IntGetProcAddress("glClearDepthf");
	if(!_funcptr_glClearDepthf) ++numFailed;
	return numFailed;
}

PFNGLGETPROGRAMBINARYPROC _funcptr_glGetProgramBinary = NULL;
PFNGLPROGRAMBINARYPROC _funcptr_glProgramBinary = NULL;
PFNGLPROGRAMPARAMETERIPROC _funcptr_glProgramParameteri = NULL;

static int LoadExt_ARB_get_program_binary()
{
	int numFailed = 0;
	_funcptr_glGetProgramBinary = (PFNGLGETPROGRAMBINARYPROC)IntGetProcAddress("glGetProgramBinary");
	if(!_funcptr_glGetProgramBinary) ++numFailed;
	_funcptr_glProgramBinary = (PFNGLPROGRAMBINARYPROC)IntGetProcAddress("glProgramBinary");
	if(!_funcptr_glProgramBinary) ++numFailed;
	_funcptr_glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC)IntGetProcAddress("glProgramParameteri");
	if(!_funcptr_glProgramParameteri) ++numFailed;
	return numFailed;
}

PFNGLUSEPROGRAMSTAGESPROC _funcptr_glUseProgramStages = NULL;
PFNGLACTIVESHADERPROGRAMPROC _funcptr_glActiveShaderProgram = NULL;
PFNGLCREATESHADERPROGRAMVPROC _funcptr_glCreateShaderProgramv = NULL;
PFNGLBINDPROGRAMPIPELINEPROC _funcptr_glBindProgramPipeline = NULL;
PFNGLDELETEPROGRAMPIPELINESPROC _funcptr_glDeleteProgramPipelines = NULL;
PFNGLGENPROGRAMPIPELINESPROC _funcptr_glGenProgramPipelines = NULL;
PFNGLISPROGRAMPIPELINEPROC _funcptr_glIsProgramPipeline = NULL;
PFNGLGETPROGRAMPIPELINEIVPROC _funcptr_glGetProgramPipelineiv = NULL;
PFNGLPROGRAMUNIFORM1IPROC _funcptr_glProgramUniform1i = NULL;
PFNGLPROGRAMUNIFORM1IVPROC _funcptr_glProgramUniform1iv = NULL;
PFNGLPROGRAMUNIFORM1FPROC _funcptr_glProgramUniform1f = NULL;
PFNGLPROGRAMUNIFORM1FVPROC _funcptr_glProgramUniform1fv = NULL;
PFNGLPROGRAMUNIFORM1DPROC _funcptr_glProgramUniform1d = NULL;
PFNGLPROGRAMUNIFORM1DVPROC _funcptr_glProgramUniform1dv = NULL;
PFNGLPROGRAMUNIFORM1UIPROC _funcptr_glProgramUniform1ui = NULL;
PFNGLPROGRAMUNIFORM1UIVPROC _funcptr_glProgramUniform1uiv = NULL;
PFNGLPROGRAMUNIFORM2IPROC _funcptr_glProgramUniform2i = NULL;
PFNGLPROGRAMUNIFORM2IVPROC _funcptr_glProgramUniform2iv = NULL;
PFNGLPROGRAMUNIFORM2FPROC _funcptr_glProgramUniform2f = NULL;
PFNGLPROGRAMUNIFORM2FVPROC _funcptr_glProgramUniform2fv = NULL;
PFNGLPROGRAMUNIFORM2DPROC _funcptr_glProgramUniform2d = NULL;
PFNGLPROGRAMUNIFORM2DVPROC _funcptr_glProgramUniform2dv = NULL;
PFNGLPROGRAMUNIFORM2UIPROC _funcptr_glProgramUniform2ui = NULL;
PFNGLPROGRAMUNIFORM2UIVPROC _funcptr_glProgramUniform2uiv = NULL;
PFNGLPROGRAMUNIFORM3IPROC _funcptr_glProgramUniform3i = NULL;
PFNGLPROGRAMUNIFORM3IVPROC _funcptr_glProgramUniform3iv = NULL;
PFNGLPROGRAMUNIFORM3FPROC _funcptr_glProgramUniform3f = NULL;
PFNGLPROGRAMUNIFORM3FVPROC _funcptr_glProgramUniform3fv = NULL;
PFNGLPROGRAMUNIFORM3DPROC _funcptr_glProgramUniform3d = NULL;
PFNGLPROGRAMUNIFORM3DVPROC _funcptr_glProgramUniform3dv = NULL;
PFNGLPROGRAMUNIFORM3UIPROC _funcptr_glProgramUniform3ui = NULL;
PFNGLPROGRAMUNIFORM3UIVPROC _funcptr_glProgramUniform3uiv = NULL;
PFNGLPROGRAMUNIFORM4IPROC _funcptr_glProgramUniform4i = NULL;
PFNGLPROGRAMUNIFORM4IVPROC _funcptr_glProgramUniform4iv = NULL;
PFNGLPROGRAMUNIFORM4FPROC _funcptr_glProgramUniform4f = NULL;
PFNGLPROGRAMUNIFORM4FVPROC _funcptr_glProgramUniform4fv = NULL;
PFNGLPROGRAMUNIFORM4DPROC _funcptr_glProgramUniform4d = NULL;
PFNGLPROGRAMUNIFORM4DVPROC _funcptr_glProgramUniform4dv = NULL;
PFNGLPROGRAMUNIFORM4UIPROC _funcptr_glProgramUniform4ui = NULL;
PFNGLPROGRAMUNIFORM4UIVPROC _funcptr_glProgramUniform4uiv = NULL;
PFNGLPROGRAMUNIFORMMATRIX2FVPROC _funcptr_glProgramUniformMatrix2fv = NULL;
PFNGLPROGRAMUNIFORMMATRIX3FVPROC _funcptr_glProgramUniformMatrix3fv = NULL;
PFNGLPROGRAMUNIFORMMATRIX4FVPROC _funcptr_glProgramUniformMatrix4fv = NULL;
PFNGLPROGRAMUNIFORMMATRIX2DVPROC _funcptr_glProgramUniformMatrix2dv = NULL;
PFNGLPROGRAMUNIFORMMATRIX3DVPROC _funcptr_glProgramUniformMatrix3dv = NULL;
PFNGLPROGRAMUNIFORMMATRIX4DVPROC _funcptr_glProgramUniformMatrix4dv = NULL;
PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC _funcptr_glProgramUniformMatrix2x3fv = NULL;
PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC _funcptr_glProgramUniformMatrix3x2fv = NULL;
PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC _funcptr_glProgramUniformMatrix2x4fv = NULL;
PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC _funcptr_glProgramUniformMatrix4x2fv = NULL;
PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC _funcptr_glProgramUniformMatrix3x4fv = NULL;
PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC _funcptr_glProgramUniformMatrix4x3fv = NULL;
PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC _funcptr_glProgramUniformMatrix2x3dv = NULL;
PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC _funcptr_glProgramUniformMatrix3x2dv = NULL;
PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC _funcptr_glProgramUniformMatrix2x4dv = NULL;
PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC _funcptr_glProgramUniformMatrix4x2dv = NULL;
PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC _funcptr_glProgramUniformMatrix3x4dv = NULL;
PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC _funcptr_glProgramUniformMatrix4x3dv = NULL;
PFNGLVALIDATEPROGRAMPIPELINEPROC _funcptr_glValidateProgramPipeline = NULL;
PFNGLGETPROGRAMPIPELINEINFOLOGPROC _funcptr_glGetProgramPipelineInfoLog = NULL;

static int LoadExt_ARB_separate_shader_objects()
{
	int numFailed = 0;
	_funcptr_glUseProgramStages = (PFNGLUSEPROGRAMSTAGESPROC)IntGetProcAddress("glUseProgramStages");
	if(!_funcptr_glUseProgramStages) ++numFailed;
	_funcptr_glActiveShaderProgram = (PFNGLACTIVESHADERPROGRAMPROC)IntGetProcAddress("glActiveShaderProgram");
	if(!_funcptr_glActiveShaderProgram) ++numFailed;
	_funcptr_glCreateShaderProgramv = (PFNGLCREATESHADERPROGRAMVPROC)IntGetProcAddress("glCreateShaderProgramv");
	if(!_funcptr_glCreateShaderProgramv) ++numFailed;
	_funcptr_glBindProgramPipeline = (PFNGLBINDPROGRAMPIPELINEPROC)IntGetProcAddress("glBindProgramPipeline");
	if(!_funcptr_glBindProgramPipeline) ++numFailed;
	_funcptr_glDeleteProgramPipelines = (PFNGLDELETEPROGRAMPIPELINESPROC)IntGetProcAddress("glDeleteProgramPipelines");
	if(!_funcptr_glDeleteProgramPipelines) ++numFailed;
	_funcptr_glGenProgramPipelines = (PFNGLGENPROGRAMPIPELINESPROC)IntGetProcAddress("glGenProgramPipelines");
	if(!_funcptr_glGenProgramPipelines) ++numFailed;
	_funcptr_glIsProgramPipeline = (PFNGLISPROGRAMPIPELINEPROC)IntGetProcAddress("glIsProgramPipeline");
	if(!_funcptr_glIsProgramPipeline) ++numFailed;
	_funcptr_glGetProgramPipelineiv = (PFNGLGETPROGRAMPIPELINEIVPROC)IntGetProcAddress("glGetProgramPipelineiv");
	if(!_funcptr_glGetProgramPipelineiv) ++numFailed;
	_funcptr_glProgramUniform1i = (PFNGLPROGRAMUNIFORM1IPROC)IntGetProcAddress("glProgramUniform1i");
	if(!_funcptr_glProgramUniform1i) ++numFailed;
	_funcptr_glProgramUniform1iv = (PFNGLPROGRAMUNIFORM1IVPROC)IntGetProcAddress("glProgramUniform1iv");
	if(!_funcptr_glProgramUniform1iv) ++numFailed;
	_funcptr_glProgramUniform1f = (PFNGLPROGRAMUNIFORM1FPROC)IntGetProcAddress("glProgramUniform1f");
	if(!_funcptr_glProgramUniform1f) ++numFailed;
	_funcptr_glProgramUniform1fv = (PFNGLPROGRAMUNIFORM1FVPROC)IntGetProcAddress("glProgramUniform1fv");
	if(!_funcptr_glProgramUniform1fv) ++numFailed;
	_funcptr_glProgramUniform1d = (PFNGLPROGRAMUNIFORM1DPROC)IntGetProcAddress("glProgramUniform1d");
	if(!_funcptr_glProgramUniform1d) ++numFailed;
	_funcptr_glProgramUniform1dv = (PFNGLPROGRAMUNIFORM1DVPROC)IntGetProcAddress("glProgramUniform1dv");
	if(!_funcptr_glProgramUniform1dv) ++numFailed;
	_funcptr_glProgramUniform1ui = (PFNGLPROGRAMUNIFORM1UIPROC)IntGetProcAddress("glProgramUniform1ui");
	if(!_funcptr_glProgramUniform1ui) ++numFailed;
	_funcptr_glProgramUniform1uiv = (PFNGLPROGRAMUNIFORM1UIVPROC)IntGetProcAddress("glProgramUniform1uiv");
	if(!_funcptr_glProgramUniform1uiv) ++numFailed;
	_funcptr_glProgramUniform2i = (PFNGLPROGRAMUNIFORM2IPROC)IntGetProcAddress("glProgramUniform2i");
	if(!_funcptr_glProgramUniform2i) ++numFailed;
	_funcptr_glProgramUniform2iv = (PFNGLPROGRAMUNIFORM2IVPROC)IntGetProcAddress("glProgramUniform2iv");
	if(!_funcptr_glProgramUniform2iv) ++numFailed;
	_funcptr_glProgramUniform2f = (PFNGLPROGRAMUNIFORM2FPROC)IntGetProcAddress("glProgramUniform2f");
	if(!_funcptr_glProgramUniform2f) ++numFailed;
	_funcptr_glProgramUniform2fv = (PFNGLPROGRAMUNIFORM2FVPROC)IntGetProcAddress("glProgramUniform2fv");
	if(!_funcptr_glProgramUniform2fv) ++numFailed;
	_funcptr_glProgramUniform2d = (PFNGLPROGRAMUNIFORM2DPROC)IntGetProcAddress("glProgramUniform2d");
	if(!_funcptr_glProgramUniform2d) ++numFailed;
	_funcptr_glProgramUniform2dv = (PFNGLPROGRAMUNIFORM2DVPROC)IntGetProcAddress("glProgramUniform2dv");
	if(!_funcptr_glProgramUniform2dv) ++numFailed;
	_funcptr_glProgramUniform2ui = (PFNGLPROGRAMUNIFORM2UIPROC)IntGetProcAddress("glProgramUniform2ui");
	if(!_funcptr_glProgramUniform2ui) ++numFailed;
	_funcptr_glProgramUniform2uiv = (PFNGLPROGRAMUNIFORM2UIVPROC)IntGetProcAddress("glProgramUniform2uiv");
	if(!_funcptr_glProgramUniform2uiv) ++numFailed;
	_funcptr_glProgramUniform3i = (PFNGLPROGRAMUNIFORM3IPROC)IntGetProcAddress("glProgramUniform3i");
	if(!_funcptr_glProgramUniform3i) ++numFailed;
	_funcptr_glProgramUniform3iv = (PFNGLPROGRAMUNIFORM3IVPROC)IntGetProcAddress("glProgramUniform3iv");
	if(!_funcptr_glProgramUniform3iv) ++numFailed;
	_funcptr_glProgramUniform3f = (PFNGLPROGRAMUNIFORM3FPROC)IntGetProcAddress("glProgramUniform3f");
	if(!_funcptr_glProgramUniform3f) ++numFailed;
	_funcptr_glProgramUniform3fv = (PFNGLPROGRAMUNIFORM3FVPROC)IntGetProcAddress("glProgramUniform3fv");
	if(!_funcptr_glProgramUniform3fv) ++numFailed;
	_funcptr_glProgramUniform3d = (PFNGLPROGRAMUNIFORM3DPROC)IntGetProcAddress("glProgramUniform3d");
	if(!_funcptr_glProgramUniform3d) ++numFailed;
	_funcptr_glProgramUniform3dv = (PFNGLPROGRAMUNIFORM3DVPROC)IntGetProcAddress("glProgramUniform3dv");
	if(!_funcptr_glProgramUniform3dv) ++numFailed;
	_funcptr_glProgramUniform3ui = (PFNGLPROGRAMUNIFORM3UIPROC)IntGetProcAddress("glProgramUniform3ui");
	if(!_funcptr_glProgramUniform3ui) ++numFailed;
	_funcptr_glProgramUniform3uiv = (PFNGLPROGRAMUNIFORM3UIVPROC)IntGetProcAddress("glProgramUniform3uiv");
	if(!_funcptr_glProgramUniform3uiv) ++numFailed;
	_funcptr_glProgramUniform4i = (PFNGLPROGRAMUNIFORM4IPROC)IntGetProcAddress("glProgramUniform4i");
	if(!_funcptr_glProgramUniform4i) ++numFailed;
	_funcptr_glProgramUniform4iv = (PFNGLPROGRAMUNIFORM4IVPROC)IntGetProcAddress("glProgramUniform4iv");
	if(!_funcptr_glProgramUniform4iv) ++numFailed;
	_funcptr_glProgramUniform4f = (PFNGLPROGRAMUNIFORM4FPROC)IntGetProcAddress("glProgramUniform4f");
	if(!_funcptr_glProgramUniform4f) ++numFailed;
	_funcptr_glProgramUniform4fv = (PFNGLPROGRAMUNIFORM4FVPROC)IntGetProcAddress("glProgramUniform4fv");
	if(!_funcptr_glProgramUniform4fv) ++numFailed;
	_funcptr_glProgramUniform4d = (PFNGLPROGRAMUNIFORM4DPROC)IntGetProcAddress("glProgramUniform4d");
	if(!_funcptr_glProgramUniform4d) ++numFailed;
	_funcptr_glProgramUniform4dv = (PFNGLPROGRAMUNIFORM4DVPROC)IntGetProcAddress("glProgramUniform4dv");
	if(!_funcptr_glProgramUniform4dv) ++numFailed;
	_funcptr_glProgramUniform4ui = (PFNGLPROGRAMUNIFORM4UIPROC)IntGetProcAddress("glProgramUniform4ui");
	if(!_funcptr_glProgramUniform4ui) ++numFailed;
	_funcptr_glProgramUniform4uiv = (PFNGLPROGRAMUNIFORM4UIVPROC)IntGetProcAddress("glProgramUniform4uiv");
	if(!_funcptr_glProgramUniform4uiv) ++numFailed;
	_funcptr_glProgramUniformMatrix2fv = (PFNGLPROGRAMUNIFORMMATRIX2FVPROC)IntGetProcAddress("glProgramUniformMatrix2fv");
	if(!_funcptr_glProgramUniformMatrix2fv) ++numFailed;
	_funcptr_glProgramUniformMatrix3fv = (PFNGLPROGRAMUNIFORMMATRIX3FVPROC)IntGetProcAddress("glProgramUniformMatrix3fv");
	if(!_funcptr_glProgramUniformMatrix3fv) ++numFailed;
	_funcptr_glProgramUniformMatrix4fv = (PFNGLPROGRAMUNIFORMMATRIX4FVPROC)IntGetProcAddress("glProgramUniformMatrix4fv");
	if(!_funcptr_glProgramUniformMatrix4fv) ++numFailed;
	_funcptr_glProgramUniformMatrix2dv = (PFNGLPROGRAMUNIFORMMATRIX2DVPROC)IntGetProcAddress("glProgramUniformMatrix2dv");
	if(!_funcptr_glProgramUniformMatrix2dv) ++numFailed;
	_funcptr_glProgramUniformMatrix3dv = (PFNGLPROGRAMUNIFORMMATRIX3DVPROC)IntGetProcAddress("glProgramUniformMatrix3dv");
	if(!_funcptr_glProgramUniformMatrix3dv) ++numFailed;
	_funcptr_glProgramUniformMatrix4dv = (PFNGLPROGRAMUNIFORMMATRIX4DVPROC)IntGetProcAddress("glProgramUniformMatrix4dv");
	if(!_funcptr_glProgramUniformMatrix4dv) ++numFailed;
	_funcptr_glProgramUniformMatrix2x3fv = (PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC)IntGetProcAddress("glProgramUniformMatrix2x3fv");
	if(!_funcptr_glProgramUniformMatrix2x3fv) ++numFailed;
	_funcptr_glProgramUniformMatrix3x2fv = (PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC)IntGetProcAddress("glProgramUniformMatrix3x2fv");
	if(!_funcptr_glProgramUniformMatrix3x2fv) ++numFailed;
	_funcptr_glProgramUniformMatrix2x4fv = (PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC)IntGetProcAddress("glProgramUniformMatrix2x4fv");
	if(!_funcptr_glProgramUniformMatrix2x4fv) ++numFailed;
	_funcptr_glProgramUniformMatrix4x2fv = (PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC)IntGetProcAddress("glProgramUniformMatrix4x2fv");
	if(!_funcptr_glProgramUniformMatrix4x2fv) ++numFailed;
	_funcptr_glProgramUniformMatrix3x4fv = (PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC)IntGetProcAddress("glProgramUniformMatrix3x4fv");
	if(!_funcptr_glProgramUniformMatrix3x4fv) ++numFailed;
	_funcptr_glProgramUniformMatrix4x3fv = (PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC)IntGetProcAddress("glProgramUniformMatrix4x3fv");
	if(!_funcptr_glProgramUniformMatrix4x3fv) ++numFailed;
	_funcptr_glProgramUniformMatrix2x3dv = (PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC)IntGetProcAddress("glProgramUniformMatrix2x3dv");
	if(!_funcptr_glProgramUniformMatrix2x3dv) ++numFailed;
	_funcptr_glProgramUniformMatrix3x2dv = (PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC)IntGetProcAddress("glProgramUniformMatrix3x2dv");
	if(!_funcptr_glProgramUniformMatrix3x2dv) ++numFailed;
	_funcptr_glProgramUniformMatrix2x4dv = (PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC)IntGetProcAddress("glProgramUniformMatrix2x4dv");
	if(!_funcptr_glProgramUniformMatrix2x4dv) ++numFailed;
	_funcptr_glProgramUniformMatrix4x2dv = (PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC)IntGetProcAddress("glProgramUniformMatrix4x2dv");
	if(!_funcptr_glProgramUniformMatrix4x2dv) ++numFailed;
	_funcptr_glProgramUniformMatrix3x4dv = (PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC)IntGetProcAddress("glProgramUniformMatrix3x4dv");
	if(!_funcptr_glProgramUniformMatrix3x4dv) ++numFailed;
	_funcptr_glProgramUniformMatrix4x3dv = (PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC)IntGetProcAddress("glProgramUniformMatrix4x3dv");
	if(!_funcptr_glProgramUniformMatrix4x3dv) ++numFailed;
	_funcptr_glValidateProgramPipeline = (PFNGLVALIDATEPROGRAMPIPELINEPROC)IntGetProcAddress("glValidateProgramPipeline");
	if(!_funcptr_glValidateProgramPipeline) ++numFailed;
	_funcptr_glGetProgramPipelineInfoLog = (PFNGLGETPROGRAMPIPELINEINFOLOGPROC)IntGetProcAddress("glGetProgramPipelineInfoLog");
	if(!_funcptr_glGetProgramPipelineInfoLog) ++numFailed;
	return numFailed;
}

PFNGLVERTEXATTRIBL1DPROC _funcptr_glVertexAttribL1d = NULL;
PFNGLVERTEXATTRIBL2DPROC _funcptr_glVertexAttribL2d = NULL;
PFNGLVERTEXATTRIBL3DPROC _funcptr_glVertexAttribL3d = NULL;
PFNGLVERTEXATTRIBL4DPROC _funcptr_glVertexAttribL4d = NULL;
PFNGLVERTEXATTRIBL1DVPROC _funcptr_glVertexAttribL1dv = NULL;
PFNGLVERTEXATTRIBL2DVPROC _funcptr_glVertexAttribL2dv = NULL;
PFNGLVERTEXATTRIBL3DVPROC _funcptr_glVertexAttribL3dv = NULL;
PFNGLVERTEXATTRIBL4DVPROC _funcptr_glVertexAttribL4dv = NULL;
PFNGLVERTEXATTRIBLPOINTERPROC _funcptr_glVertexAttribLPointer = NULL;
PFNGLGETVERTEXATTRIBLDVPROC _funcptr_glGetVertexAttribLdv = NULL;

static int LoadExt_ARB_vertex_attrib_64bit()
{
	int numFailed = 0;
	_funcptr_glVertexAttribL1d = (PFNGLVERTEXATTRIBL1DPROC)IntGetProcAddress("glVertexAttribL1d");
	if(!_funcptr_glVertexAttribL1d) ++numFailed;
	_funcptr_glVertexAttribL2d = (PFNGLVERTEXATTRIBL2DPROC)IntGetProcAddress("glVertexAttribL2d");
	if(!_funcptr_glVertexAttribL2d) ++numFailed;
	_funcptr_glVertexAttribL3d = (PFNGLVERTEXATTRIBL3DPROC)IntGetProcAddress("glVertexAttribL3d");
	if(!_funcptr_glVertexAttribL3d) ++numFailed;
	_funcptr_glVertexAttribL4d = (PFNGLVERTEXATTRIBL4DPROC)IntGetProcAddress("glVertexAttribL4d");
	if(!_funcptr_glVertexAttribL4d) ++numFailed;
	_funcptr_glVertexAttribL1dv = (PFNGLVERTEXATTRIBL1DVPROC)IntGetProcAddress("glVertexAttribL1dv");
	if(!_funcptr_glVertexAttribL1dv) ++numFailed;
	_funcptr_glVertexAttribL2dv = (PFNGLVERTEXATTRIBL2DVPROC)IntGetProcAddress("glVertexAttribL2dv");
	if(!_funcptr_glVertexAttribL2dv) ++numFailed;
	_funcptr_glVertexAttribL3dv = (PFNGLVERTEXATTRIBL3DVPROC)IntGetProcAddress("glVertexAttribL3dv");
	if(!_funcptr_glVertexAttribL3dv) ++numFailed;
	_funcptr_glVertexAttribL4dv = (PFNGLVERTEXATTRIBL4DVPROC)IntGetProcAddress("glVertexAttribL4dv");
	if(!_funcptr_glVertexAttribL4dv) ++numFailed;
	_funcptr_glVertexAttribLPointer = (PFNGLVERTEXATTRIBLPOINTERPROC)IntGetProcAddress("glVertexAttribLPointer");
	if(!_funcptr_glVertexAttribLPointer) ++numFailed;
	_funcptr_glGetVertexAttribLdv = (PFNGLGETVERTEXATTRIBLDVPROC)IntGetProcAddress("glGetVertexAttribLdv");
	if(!_funcptr_glGetVertexAttribLdv) ++numFailed;
	return numFailed;
}

PFNGLVIEWPORTARRAYVPROC _funcptr_glViewportArrayv = NULL;
PFNGLVIEWPORTINDEXEDFPROC _funcptr_glViewportIndexedf = NULL;
PFNGLVIEWPORTINDEXEDFVPROC _funcptr_glViewportIndexedfv = NULL;
PFNGLSCISSORARRAYVPROC _funcptr_glScissorArrayv = NULL;
PFNGLSCISSORINDEXEDPROC _funcptr_glScissorIndexed = NULL;
PFNGLSCISSORINDEXEDVPROC _funcptr_glScissorIndexedv = NULL;
PFNGLDEPTHRANGEARRAYVPROC _funcptr_glDepthRangeArrayv = NULL;
PFNGLDEPTHRANGEINDEXEDPROC _funcptr_glDepthRangeIndexed = NULL;
PFNGLGETFLOATI_VPROC _funcptr_glGetFloati_v = NULL;
PFNGLGETDOUBLEI_VPROC _funcptr_glGetDoublei_v = NULL;

static int LoadExt_ARB_viewport_array()
{
	int numFailed = 0;
	_funcptr_glViewportArrayv = (PFNGLVIEWPORTARRAYVPROC)IntGetProcAddress("glViewportArrayv");
	if(!_funcptr_glViewportArrayv) ++numFailed;
	_funcptr_glViewportIndexedf = (PFNGLVIEWPORTINDEXEDFPROC)IntGetProcAddress("glViewportIndexedf");
	if(!_funcptr_glViewportIndexedf) ++numFailed;
	_funcptr_glViewportIndexedfv = (PFNGLVIEWPORTINDEXEDFVPROC)IntGetProcAddress("glViewportIndexedfv");
	if(!_funcptr_glViewportIndexedfv) ++numFailed;
	_funcptr_glScissorArrayv = (PFNGLSCISSORARRAYVPROC)IntGetProcAddress("glScissorArrayv");
	if(!_funcptr_glScissorArrayv) ++numFailed;
	_funcptr_glScissorIndexed = (PFNGLSCISSORINDEXEDPROC)IntGetProcAddress("glScissorIndexed");
	if(!_funcptr_glScissorIndexed) ++numFailed;
	_funcptr_glScissorIndexedv = (PFNGLSCISSORINDEXEDVPROC)IntGetProcAddress("glScissorIndexedv");
	if(!_funcptr_glScissorIndexedv) ++numFailed;
	_funcptr_glDepthRangeArrayv = (PFNGLDEPTHRANGEARRAYVPROC)IntGetProcAddress("glDepthRangeArrayv");
	if(!_funcptr_glDepthRangeArrayv) ++numFailed;
	_funcptr_glDepthRangeIndexed = (PFNGLDEPTHRANGEINDEXEDPROC)IntGetProcAddress("glDepthRangeIndexed");
	if(!_funcptr_glDepthRangeIndexed) ++numFailed;
	_funcptr_glGetFloati_v = (PFNGLGETFLOATI_VPROC)IntGetProcAddress("glGetFloati_v");
	if(!_funcptr_glGetFloati_v) ++numFailed;
	_funcptr_glGetDoublei_v = (PFNGLGETDOUBLEI_VPROC)IntGetProcAddress("glGetDoublei_v");
	if(!_funcptr_glGetDoublei_v) ++numFailed;
	return numFailed;
}

PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC _funcptr_glDrawArraysInstancedBaseInstance = NULL;
PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC _funcptr_glDrawElementsInstancedBaseInstance = NULL;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC _funcptr_glDrawElementsInstancedBaseVertexBaseInstance = NULL;

static int LoadExt_ARB_base_instance()
{
	int numFailed = 0;
	_funcptr_glDrawArraysInstancedBaseInstance = (PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)IntGetProcAddress("glDrawArraysInstancedBaseInstance");
	if(!_funcptr_glDrawArraysInstancedBaseInstance) ++numFailed;
	_funcptr_glDrawElementsInstancedBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC)IntGetProcAddress("glDrawElementsInstancedBaseInstance");
	if(!_funcptr_glDrawElementsInstancedBaseInstance) ++numFailed;
	_funcptr_glDrawElementsInstancedBaseVertexBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC)IntGetProcAddress("glDrawElementsInstancedBaseVertexBaseInstance");
	if(!_funcptr_glDrawElementsInstancedBaseVertexBaseInstance) ++numFailed;
	return numFailed;
}

PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC _funcptr_glDrawTransformFeedbackInstanced = NULL;
PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC _funcptr_glDrawTransformFeedbackStreamInstanced = NULL;

static int LoadExt_ARB_transform_feedback_instanced()
{
	int numFailed = 0;
	_funcptr_glDrawTransformFeedbackInstanced = (PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC)IntGetProcAddress("glDrawTransformFeedbackInstanced");
	if(!_funcptr_glDrawTransformFeedbackInstanced) ++numFailed;
	_funcptr_glDrawTransformFeedbackStreamInstanced = (PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC)IntGetProcAddress("glDrawTransformFeedbackStreamInstanced");
	if(!_funcptr_glDrawTransformFeedbackStreamInstanced) ++numFailed;
	return numFailed;
}

PFNGLGETINTERNALFORMATIVPROC _funcptr_glGetInternalformativ = NULL;

static int LoadExt_ARB_internalformat_query()
{
	int numFailed = 0;
	_funcptr_glGetInternalformativ = (PFNGLGETINTERNALFORMATIVPROC)IntGetProcAddress("glGetInternalformativ");
	if(!_funcptr_glGetInternalformativ) ++numFailed;
	return numFailed;
}

PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC _funcptr_glGetActiveAtomicCounterBufferiv = NULL;

static int LoadExt_ARB_shader_atomic_counters()
{
	int numFailed = 0;
	_funcptr_glGetActiveAtomicCounterBufferiv = (PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC)IntGetProcAddress("glGetActiveAtomicCounterBufferiv");
	if(!_funcptr_glGetActiveAtomicCounterBufferiv) ++numFailed;
	return numFailed;
}

PFNGLBINDIMAGETEXTUREPROC _funcptr_glBindImageTexture = NULL;
PFNGLMEMORYBARRIERPROC _funcptr_glMemoryBarrier = NULL;

static int LoadExt_ARB_shader_image_load_store()
{
	int numFailed = 0;
	_funcptr_glBindImageTexture = (PFNGLBINDIMAGETEXTUREPROC)IntGetProcAddress("glBindImageTexture");
	if(!_funcptr_glBindImageTexture) ++numFailed;
	_funcptr_glMemoryBarrier = (PFNGLMEMORYBARRIERPROC)IntGetProcAddress("glMemoryBarrier");
	if(!_funcptr_glMemoryBarrier) ++numFailed;
	return numFailed;
}

PFNGLTEXSTORAGE1DPROC _funcptr_glTexStorage1D = NULL;
PFNGLTEXSTORAGE2DPROC _funcptr_glTexStorage2D = NULL;
PFNGLTEXSTORAGE3DPROC _funcptr_glTexStorage3D = NULL;
PFNGLTEXTURESTORAGE1DEXTPROC _funcptr_glTextureStorage1DEXT = NULL;
PFNGLTEXTURESTORAGE2DEXTPROC _funcptr_glTextureStorage2DEXT = NULL;
PFNGLTEXTURESTORAGE3DEXTPROC _funcptr_glTextureStorage3DEXT = NULL;

static int LoadExt_ARB_texture_storage()
{
	int numFailed = 0;
	_funcptr_glTexStorage1D = (PFNGLTEXSTORAGE1DPROC)IntGetProcAddress("glTexStorage1D");
	if(!_funcptr_glTexStorage1D) ++numFailed;
	_funcptr_glTexStorage2D = (PFNGLTEXSTORAGE2DPROC)IntGetProcAddress("glTexStorage2D");
	if(!_funcptr_glTexStorage2D) ++numFailed;
	_funcptr_glTexStorage3D = (PFNGLTEXSTORAGE3DPROC)IntGetProcAddress("glTexStorage3D");
	if(!_funcptr_glTexStorage3D) ++numFailed;
	_funcptr_glTextureStorage1DEXT = (PFNGLTEXTURESTORAGE1DEXTPROC)IntGetProcAddress("glTextureStorage1DEXT");
	if(!_funcptr_glTextureStorage1DEXT) ++numFailed;
	_funcptr_glTextureStorage2DEXT = (PFNGLTEXTURESTORAGE2DEXTPROC)IntGetProcAddress("glTextureStorage2DEXT");
	if(!_funcptr_glTextureStorage2DEXT) ++numFailed;
	_funcptr_glTextureStorage3DEXT = (PFNGLTEXTURESTORAGE3DEXTPROC)IntGetProcAddress("glTextureStorage3DEXT");
	if(!_funcptr_glTextureStorage3DEXT) ++numFailed;
	return numFailed;
}

PFNGLDEBUGMESSAGECONTROLPROC _funcptr_glDebugMessageControl = NULL;
PFNGLDEBUGMESSAGEINSERTPROC _funcptr_glDebugMessageInsert = NULL;
PFNGLDEBUGMESSAGECALLBACKPROC _funcptr_glDebugMessageCallback = NULL;
PFNGLGETDEBUGMESSAGELOGPROC _funcptr_glGetDebugMessageLog = NULL;
PFNGLPUSHDEBUGGROUPPROC _funcptr_glPushDebugGroup = NULL;
PFNGLPOPDEBUGGROUPPROC _funcptr_glPopDebugGroup = NULL;
PFNGLOBJECTLABELPROC _funcptr_glObjectLabel = NULL;
PFNGLGETOBJECTLABELPROC _funcptr_glGetObjectLabel = NULL;
PFNGLOBJECTPTRLABELPROC _funcptr_glObjectPtrLabel = NULL;
PFNGLGETOBJECTPTRLABELPROC _funcptr_glGetObjectPtrLabel = NULL;

static int LoadExt_KHR_debug()
{
	int numFailed = 0;
	_funcptr_glDebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC)IntGetProcAddress("glDebugMessageControl");
	if(!_funcptr_glDebugMessageControl) ++numFailed;
	_funcptr_glDebugMessageInsert = (PFNGLDEBUGMESSAGEINSERTPROC)IntGetProcAddress("glDebugMessageInsert");
	if(!_funcptr_glDebugMessageInsert) ++numFailed;
	_funcptr_glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)IntGetProcAddress("glDebugMessageCallback");
	if(!_funcptr_glDebugMessageCallback) ++numFailed;
	_funcptr_glGetDebugMessageLog = (PFNGLGETDEBUGMESSAGELOGPROC)IntGetProcAddress("glGetDebugMessageLog");
	if(!_funcptr_glGetDebugMessageLog) ++numFailed;
	_funcptr_glPushDebugGroup = (PFNGLPUSHDEBUGGROUPPROC)IntGetProcAddress("glPushDebugGroup");
	if(!_funcptr_glPushDebugGroup) ++numFailed;
	_funcptr_glPopDebugGroup = (PFNGLPOPDEBUGGROUPPROC)IntGetProcAddress("glPopDebugGroup");
	if(!_funcptr_glPopDebugGroup) ++numFailed;
	_funcptr_glObjectLabel = (PFNGLOBJECTLABELPROC)IntGetProcAddress("glObjectLabel");
	if(!_funcptr_glObjectLabel) ++numFailed;
	_funcptr_glGetObjectLabel = (PFNGLGETOBJECTLABELPROC)IntGetProcAddress("glGetObjectLabel");
	if(!_funcptr_glGetObjectLabel) ++numFailed;
	_funcptr_glObjectPtrLabel = (PFNGLOBJECTPTRLABELPROC)IntGetProcAddress("glObjectPtrLabel");
	if(!_funcptr_glObjectPtrLabel) ++numFailed;
	_funcptr_glGetObjectPtrLabel = (PFNGLGETOBJECTPTRLABELPROC)IntGetProcAddress("glGetObjectPtrLabel");
	if(!_funcptr_glGetObjectPtrLabel) ++numFailed;
	return numFailed;
}

PFNGLCLEARBUFFERDATAPROC _funcptr_glClearBufferData = NULL;
PFNGLCLEARBUFFERSUBDATAPROC _funcptr_glClearBufferSubData = NULL;
PFNGLCLEARNAMEDBUFFERDATAEXTPROC _funcptr_glClearNamedBufferDataEXT = NULL;
PFNGLCLEARNAMEDBUFFERSUBDATAEXTPROC _funcptr_glClearNamedBufferSubDataEXT = NULL;

static int LoadExt_ARB_clear_buffer_object()
{
	int numFailed = 0;
	_funcptr_glClearBufferData = (PFNGLCLEARBUFFERDATAPROC)IntGetProcAddress("glClearBufferData");
	if(!_funcptr_glClearBufferData) ++numFailed;
	_funcptr_glClearBufferSubData = (PFNGLCLEARBUFFERSUBDATAPROC)IntGetProcAddress("glClearBufferSubData");
	if(!_funcptr_glClearBufferSubData) ++numFailed;
	_funcptr_glClearNamedBufferDataEXT = (PFNGLCLEARNAMEDBUFFERDATAEXTPROC)IntGetProcAddress("glClearNamedBufferDataEXT");
	if(!_funcptr_glClearNamedBufferDataEXT) ++numFailed;
	_funcptr_glClearNamedBufferSubDataEXT = (PFNGLCLEARNAMEDBUFFERSUBDATAEXTPROC)IntGetProcAddress("glClearNamedBufferSubDataEXT");
	if(!_funcptr_glClearNamedBufferSubDataEXT) ++numFailed;
	return numFailed;
}

PFNGLDISPATCHCOMPUTEPROC _funcptr_glDispatchCompute = NULL;
PFNGLDISPATCHCOMPUTEINDIRECTPROC _funcptr_glDispatchComputeIndirect = NULL;

static int LoadExt_ARB_compute_shader()
{
	int numFailed = 0;
	_funcptr_glDispatchCompute = (PFNGLDISPATCHCOMPUTEPROC)IntGetProcAddress("glDispatchCompute");
	if(!_funcptr_glDispatchCompute) ++numFailed;
	_funcptr_glDispatchComputeIndirect = (PFNGLDISPATCHCOMPUTEINDIRECTPROC)IntGetProcAddress("glDispatchComputeIndirect");
	if(!_funcptr_glDispatchComputeIndirect) ++numFailed;
	return numFailed;
}

PFNGLCOPYIMAGESUBDATAPROC _funcptr_glCopyImageSubData = NULL;

static int LoadExt_ARB_copy_image()
{
	int numFailed = 0;
	_funcptr_glCopyImageSubData = (PFNGLCOPYIMAGESUBDATAPROC)IntGetProcAddress("glCopyImageSubData");
	if(!_funcptr_glCopyImageSubData) ++numFailed;
	return numFailed;
}

PFNGLFRAMEBUFFERPARAMETERIPROC _funcptr_glFramebufferParameteri = NULL;
PFNGLGETFRAMEBUFFERPARAMETERIVPROC _funcptr_glGetFramebufferParameteriv = NULL;
PFNGLNAMEDFRAMEBUFFERPARAMETERIEXTPROC _funcptr_glNamedFramebufferParameteriEXT = NULL;
PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVEXTPROC _funcptr_glGetNamedFramebufferParameterivEXT = NULL;

static int LoadExt_ARB_framebuffer_no_attachments()
{
	int numFailed = 0;
	_funcptr_glFramebufferParameteri = (PFNGLFRAMEBUFFERPARAMETERIPROC)IntGetProcAddress("glFramebufferParameteri");
	if(!_funcptr_glFramebufferParameteri) ++numFailed;
	_funcptr_glGetFramebufferParameteriv = (PFNGLGETFRAMEBUFFERPARAMETERIVPROC)IntGetProcAddress("glGetFramebufferParameteriv");
	if(!_funcptr_glGetFramebufferParameteriv) ++numFailed;
	_funcptr_glNamedFramebufferParameteriEXT = (PFNGLNAMEDFRAMEBUFFERPARAMETERIEXTPROC)IntGetProcAddress("glNamedFramebufferParameteriEXT");
	if(!_funcptr_glNamedFramebufferParameteriEXT) ++numFailed;
	_funcptr_glGetNamedFramebufferParameterivEXT = (PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVEXTPROC)IntGetProcAddress("glGetNamedFramebufferParameterivEXT");
	if(!_funcptr_glGetNamedFramebufferParameterivEXT) ++numFailed;
	return numFailed;
}

PFNGLGETINTERNALFORMATI64VPROC _funcptr_glGetInternalformati64v = NULL;

static int LoadExt_ARB_internalformat_query2()
{
	int numFailed = 0;
	_funcptr_glGetInternalformati64v = (PFNGLGETINTERNALFORMATI64VPROC)IntGetProcAddress("glGetInternalformati64v");
	if(!_funcptr_glGetInternalformati64v) ++numFailed;
	return numFailed;
}

PFNGLINVALIDATETEXSUBIMAGEPROC _funcptr_glInvalidateTexSubImage = NULL;
PFNGLINVALIDATETEXIMAGEPROC _funcptr_glInvalidateTexImage = NULL;
PFNGLINVALIDATEBUFFERSUBDATAPROC _funcptr_glInvalidateBufferSubData = NULL;
PFNGLINVALIDATEBUFFERDATAPROC _funcptr_glInvalidateBufferData = NULL;
PFNGLINVALIDATEFRAMEBUFFERPROC _funcptr_glInvalidateFramebuffer = NULL;
PFNGLINVALIDATESUBFRAMEBUFFERPROC _funcptr_glInvalidateSubFramebuffer = NULL;

static int LoadExt_ARB_invalidate_subdata()
{
	int numFailed = 0;
	_funcptr_glInvalidateTexSubImage = (PFNGLINVALIDATETEXSUBIMAGEPROC)IntGetProcAddress("glInvalidateTexSubImage");
	if(!_funcptr_glInvalidateTexSubImage) ++numFailed;
	_funcptr_glInvalidateTexImage = (PFNGLINVALIDATETEXIMAGEPROC)IntGetProcAddress("glInvalidateTexImage");
	if(!_funcptr_glInvalidateTexImage) ++numFailed;
	_funcptr_glInvalidateBufferSubData = (PFNGLINVALIDATEBUFFERSUBDATAPROC)IntGetProcAddress("glInvalidateBufferSubData");
	if(!_funcptr_glInvalidateBufferSubData) ++numFailed;
	_funcptr_glInvalidateBufferData = (PFNGLINVALIDATEBUFFERDATAPROC)IntGetProcAddress("glInvalidateBufferData");
	if(!_funcptr_glInvalidateBufferData) ++numFailed;
	_funcptr_glInvalidateFramebuffer = (PFNGLINVALIDATEFRAMEBUFFERPROC)IntGetProcAddress("glInvalidateFramebuffer");
	if(!_funcptr_glInvalidateFramebuffer) ++numFailed;
	_funcptr_glInvalidateSubFramebuffer = (PFNGLINVALIDATESUBFRAMEBUFFERPROC)IntGetProcAddress("glInvalidateSubFramebuffer");
	if(!_funcptr_glInvalidateSubFramebuffer) ++numFailed;
	return numFailed;
}

PFNGLMULTIDRAWARRAYSINDIRECTPROC _funcptr_glMultiDrawArraysIndirect = NULL;
PFNGLMULTIDRAWELEMENTSINDIRECTPROC _funcptr_glMultiDrawElementsIndirect = NULL;

static int LoadExt_ARB_multi_draw_indirect()
{
	int numFailed = 0;
	_funcptr_glMultiDrawArraysIndirect = (PFNGLMULTIDRAWARRAYSINDIRECTPROC)IntGetProcAddress("glMultiDrawArraysIndirect");
	if(!_funcptr_glMultiDrawArraysIndirect) ++numFailed;
	_funcptr_glMultiDrawElementsIndirect = (PFNGLMULTIDRAWELEMENTSINDIRECTPROC)IntGetProcAddress("glMultiDrawElementsIndirect");
	if(!_funcptr_glMultiDrawElementsIndirect) ++numFailed;
	return numFailed;
}

PFNGLGETPROGRAMINTERFACEIVPROC _funcptr_glGetProgramInterfaceiv = NULL;
PFNGLGETPROGRAMRESOURCEINDEXPROC _funcptr_glGetProgramResourceIndex = NULL;
PFNGLGETPROGRAMRESOURCENAMEPROC _funcptr_glGetProgramResourceName = NULL;
PFNGLGETPROGRAMRESOURCEIVPROC _funcptr_glGetProgramResourceiv = NULL;
PFNGLGETPROGRAMRESOURCELOCATIONPROC _funcptr_glGetProgramResourceLocation = NULL;
PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC _funcptr_glGetProgramResourceLocationIndex = NULL;

static int LoadExt_ARB_program_interface_query()
{
	int numFailed = 0;
	_funcptr_glGetProgramInterfaceiv = (PFNGLGETPROGRAMINTERFACEIVPROC)IntGetProcAddress("glGetProgramInterfaceiv");
	if(!_funcptr_glGetProgramInterfaceiv) ++numFailed;
	_funcptr_glGetProgramResourceIndex = (PFNGLGETPROGRAMRESOURCEINDEXPROC)IntGetProcAddress("glGetProgramResourceIndex");
	if(!_funcptr_glGetProgramResourceIndex) ++numFailed;
	_funcptr_glGetProgramResourceName = (PFNGLGETPROGRAMRESOURCENAMEPROC)IntGetProcAddress("glGetProgramResourceName");
	if(!_funcptr_glGetProgramResourceName) ++numFailed;
	_funcptr_glGetProgramResourceiv = (PFNGLGETPROGRAMRESOURCEIVPROC)IntGetProcAddress("glGetProgramResourceiv");
	if(!_funcptr_glGetProgramResourceiv) ++numFailed;
	_funcptr_glGetProgramResourceLocation = (PFNGLGETPROGRAMRESOURCELOCATIONPROC)IntGetProcAddress("glGetProgramResourceLocation");
	if(!_funcptr_glGetProgramResourceLocation) ++numFailed;
	_funcptr_glGetProgramResourceLocationIndex = (PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC)IntGetProcAddress("glGetProgramResourceLocationIndex");
	if(!_funcptr_glGetProgramResourceLocationIndex) ++numFailed;
	return numFailed;
}

PFNGLSHADERSTORAGEBLOCKBINDINGPROC _funcptr_glShaderStorageBlockBinding = NULL;

static int LoadExt_ARB_shader_storage_buffer_object()
{
	int numFailed = 0;
	_funcptr_glShaderStorageBlockBinding = (PFNGLSHADERSTORAGEBLOCKBINDINGPROC)IntGetProcAddress("glShaderStorageBlockBinding");
	if(!_funcptr_glShaderStorageBlockBinding) ++numFailed;
	return numFailed;
}

PFNGLTEXBUFFERRANGEPROC _funcptr_glTexBufferRange = NULL;
PFNGLTEXTUREBUFFERRANGEEXTPROC _funcptr_glTextureBufferRangeEXT = NULL;

static int LoadExt_ARB_texture_buffer_range()
{
	int numFailed = 0;
	_funcptr_glTexBufferRange = (PFNGLTEXBUFFERRANGEPROC)IntGetProcAddress("glTexBufferRange");
	if(!_funcptr_glTexBufferRange) ++numFailed;
	_funcptr_glTextureBufferRangeEXT = (PFNGLTEXTUREBUFFERRANGEEXTPROC)IntGetProcAddress("glTextureBufferRangeEXT");
	if(!_funcptr_glTextureBufferRangeEXT) ++numFailed;
	return numFailed;
}

PFNGLTEXSTORAGE2DMULTISAMPLEPROC _funcptr_glTexStorage2DMultisample = NULL;
PFNGLTEXSTORAGE3DMULTISAMPLEPROC _funcptr_glTexStorage3DMultisample = NULL;
PFNGLTEXTURESTORAGE2DMULTISAMPLEEXTPROC _funcptr_glTextureStorage2DMultisampleEXT = NULL;
PFNGLTEXTURESTORAGE3DMULTISAMPLEEXTPROC _funcptr_glTextureStorage3DMultisampleEXT = NULL;

static int LoadExt_ARB_texture_storage_multisample()
{
	int numFailed = 0;
	_funcptr_glTexStorage2DMultisample = (PFNGLTEXSTORAGE2DMULTISAMPLEPROC)IntGetProcAddress("glTexStorage2DMultisample");
	if(!_funcptr_glTexStorage2DMultisample) ++numFailed;
	_funcptr_glTexStorage3DMultisample = (PFNGLTEXSTORAGE3DMULTISAMPLEPROC)IntGetProcAddress("glTexStorage3DMultisample");
	if(!_funcptr_glTexStorage3DMultisample) ++numFailed;
	_funcptr_glTextureStorage2DMultisampleEXT = (PFNGLTEXTURESTORAGE2DMULTISAMPLEEXTPROC)IntGetProcAddress("glTextureStorage2DMultisampleEXT");
	if(!_funcptr_glTextureStorage2DMultisampleEXT) ++numFailed;
	_funcptr_glTextureStorage3DMultisampleEXT = (PFNGLTEXTURESTORAGE3DMULTISAMPLEEXTPROC)IntGetProcAddress("glTextureStorage3DMultisampleEXT");
	if(!_funcptr_glTextureStorage3DMultisampleEXT) ++numFailed;
	return numFailed;
}

PFNGLTEXTUREVIEWPROC _funcptr_glTextureView = NULL;

static int LoadExt_ARB_texture_view()
{
	int numFailed = 0;
	_funcptr_glTextureView = (PFNGLTEXTUREVIEWPROC)IntGetProcAddress("glTextureView");
	if(!_funcptr_glTextureView) ++numFailed;
	return numFailed;
}

PFNGLBINDVERTEXBUFFERPROC _funcptr_glBindVertexBuffer = NULL;
PFNGLVERTEXATTRIBFORMATPROC _funcptr_glVertexAttribFormat = NULL;
PFNGLVERTEXATTRIBIFORMATPROC _funcptr_glVertexAttribIFormat = NULL;
PFNGLVERTEXATTRIBLFORMATPROC _funcptr_glVertexAttribLFormat = NULL;
PFNGLVERTEXATTRIBBINDINGPROC _funcptr_glVertexAttribBinding = NULL;
PFNGLVERTEXBINDINGDIVISORPROC _funcptr_glVertexBindingDivisor = NULL;
PFNGLVERTEXARRAYBINDVERTEXBUFFEREXTPROC _funcptr_glVertexArrayBindVertexBufferEXT = NULL;
PFNGLVERTEXARRAYVERTEXATTRIBFORMATEXTPROC _funcptr_glVertexArrayVertexAttribFormatEXT = NULL;
PFNGLVERTEXARRAYVERTEXATTRIBIFORMATEXTPROC _funcptr_glVertexArrayVertexAttribIFormatEXT = NULL;
PFNGLVERTEXARRAYVERTEXATTRIBLFORMATEXTPROC _funcptr_glVertexArrayVertexAttribLFormatEXT = NULL;
PFNGLVERTEXARRAYVERTEXATTRIBBINDINGEXTPROC _funcptr_glVertexArrayVertexAttribBindingEXT = NULL;
PFNGLVERTEXARRAYVERTEXBINDINGDIVISOREXTPROC _funcptr_glVertexArrayVertexBindingDivisorEXT = NULL;

static int LoadExt_ARB_vertex_attrib_binding()
{
	int numFailed = 0;
	_funcptr_glBindVertexBuffer = (PFNGLBINDVERTEXBUFFERPROC)IntGetProcAddress("glBindVertexBuffer");
	if(!_funcptr_glBindVertexBuffer) ++numFailed;
	_funcptr_glVertexAttribFormat = (PFNGLVERTEXATTRIBFORMATPROC)IntGetProcAddress("glVertexAttribFormat");
	if(!_funcptr_glVertexAttribFormat) ++numFailed;
	_funcptr_glVertexAttribIFormat = (PFNGLVERTEXATTRIBIFORMATPROC)IntGetProcAddress("glVertexAttribIFormat");
	if(!_funcptr_glVertexAttribIFormat) ++numFailed;
	_funcptr_glVertexAttribLFormat = (PFNGLVERTEXATTRIBLFORMATPROC)IntGetProcAddress("glVertexAttribLFormat");
	if(!_funcptr_glVertexAttribLFormat) ++numFailed;
	_funcptr_glVertexAttribBinding = (PFNGLVERTEXATTRIBBINDINGPROC)IntGetProcAddress("glVertexAttribBinding");
	if(!_funcptr_glVertexAttribBinding) ++numFailed;
	_funcptr_glVertexBindingDivisor = (PFNGLVERTEXBINDINGDIVISORPROC)IntGetProcAddress("glVertexBindingDivisor");
	if(!_funcptr_glVertexBindingDivisor) ++numFailed;
	_funcptr_glVertexArrayBindVertexBufferEXT = (PFNGLVERTEXARRAYBINDVERTEXBUFFEREXTPROC)IntGetProcAddress("glVertexArrayBindVertexBufferEXT");
	if(!_funcptr_glVertexArrayBindVertexBufferEXT) ++numFailed;
	_funcptr_glVertexArrayVertexAttribFormatEXT = (PFNGLVERTEXARRAYVERTEXATTRIBFORMATEXTPROC)IntGetProcAddress("glVertexArrayVertexAttribFormatEXT");
	if(!_funcptr_glVertexArrayVertexAttribFormatEXT) ++numFailed;
	_funcptr_glVertexArrayVertexAttribIFormatEXT = (PFNGLVERTEXARRAYVERTEXATTRIBIFORMATEXTPROC)IntGetProcAddress("glVertexArrayVertexAttribIFormatEXT");
	if(!_funcptr_glVertexArrayVertexAttribIFormatEXT) ++numFailed;
	_funcptr_glVertexArrayVertexAttribLFormatEXT = (PFNGLVERTEXARRAYVERTEXATTRIBLFORMATEXTPROC)IntGetProcAddress("glVertexArrayVertexAttribLFormatEXT");
	if(!_funcptr_glVertexArrayVertexAttribLFormatEXT) ++numFailed;
	_funcptr_glVertexArrayVertexAttribBindingEXT = (PFNGLVERTEXARRAYVERTEXATTRIBBINDINGEXTPROC)IntGetProcAddress("glVertexArrayVertexAttribBindingEXT");
	if(!_funcptr_glVertexArrayVertexAttribBindingEXT) ++numFailed;
	_funcptr_glVertexArrayVertexBindingDivisorEXT = (PFNGLVERTEXARRAYVERTEXBINDINGDIVISOREXTPROC)IntGetProcAddress("glVertexArrayVertexBindingDivisorEXT");
	if(!_funcptr_glVertexArrayVertexBindingDivisorEXT) ++numFailed;
	return numFailed;
}

PFNGLTBUFFERMASK3DFXPROC _funcptr_glTbufferMask3DFX = NULL;

static int LoadExt_3DFX_tbuffer()
{
	int numFailed = 0;
	_funcptr_glTbufferMask3DFX = (PFNGLTBUFFERMASK3DFXPROC)IntGetProcAddress("glTbufferMask3DFX");
	if(!_funcptr_glTbufferMask3DFX) ++numFailed;
	return numFailed;
}

PFNGLDEBUGMESSAGEENABLEAMDPROC _funcptr_glDebugMessageEnableAMD = NULL;
PFNGLDEBUGMESSAGEINSERTAMDPROC _funcptr_glDebugMessageInsertAMD = NULL;
PFNGLDEBUGMESSAGECALLBACKAMDPROC _funcptr_glDebugMessageCallbackAMD = NULL;
PFNGLGETDEBUGMESSAGELOGAMDPROC _funcptr_glGetDebugMessageLogAMD = NULL;

static int LoadExt_AMD_debug_output()
{
	int numFailed = 0;
	_funcptr_glDebugMessageEnableAMD = (PFNGLDEBUGMESSAGEENABLEAMDPROC)IntGetProcAddress("glDebugMessageEnableAMD");
	if(!_funcptr_glDebugMessageEnableAMD) ++numFailed;
	_funcptr_glDebugMessageInsertAMD = (PFNGLDEBUGMESSAGEINSERTAMDPROC)IntGetProcAddress("glDebugMessageInsertAMD");
	if(!_funcptr_glDebugMessageInsertAMD) ++numFailed;
	_funcptr_glDebugMessageCallbackAMD = (PFNGLDEBUGMESSAGECALLBACKAMDPROC)IntGetProcAddress("glDebugMessageCallbackAMD");
	if(!_funcptr_glDebugMessageCallbackAMD) ++numFailed;
	_funcptr_glGetDebugMessageLogAMD = (PFNGLGETDEBUGMESSAGELOGAMDPROC)IntGetProcAddress("glGetDebugMessageLogAMD");
	if(!_funcptr_glGetDebugMessageLogAMD) ++numFailed;
	return numFailed;
}

PFNGLBLENDFUNCINDEXEDAMDPROC _funcptr_glBlendFuncIndexedAMD = NULL;
PFNGLBLENDFUNCSEPARATEINDEXEDAMDPROC _funcptr_glBlendFuncSeparateIndexedAMD = NULL;
PFNGLBLENDEQUATIONINDEXEDAMDPROC _funcptr_glBlendEquationIndexedAMD = NULL;
PFNGLBLENDEQUATIONSEPARATEINDEXEDAMDPROC _funcptr_glBlendEquationSeparateIndexedAMD = NULL;

static int LoadExt_AMD_draw_buffers_blend()
{
	int numFailed = 0;
	_funcptr_glBlendFuncIndexedAMD = (PFNGLBLENDFUNCINDEXEDAMDPROC)IntGetProcAddress("glBlendFuncIndexedAMD");
	if(!_funcptr_glBlendFuncIndexedAMD) ++numFailed;
	_funcptr_glBlendFuncSeparateIndexedAMD = (PFNGLBLENDFUNCSEPARATEINDEXEDAMDPROC)IntGetProcAddress("glBlendFuncSeparateIndexedAMD");
	if(!_funcptr_glBlendFuncSeparateIndexedAMD) ++numFailed;
	_funcptr_glBlendEquationIndexedAMD = (PFNGLBLENDEQUATIONINDEXEDAMDPROC)IntGetProcAddress("glBlendEquationIndexedAMD");
	if(!_funcptr_glBlendEquationIndexedAMD) ++numFailed;
	_funcptr_glBlendEquationSeparateIndexedAMD = (PFNGLBLENDEQUATIONSEPARATEINDEXEDAMDPROC)IntGetProcAddress("glBlendEquationSeparateIndexedAMD");
	if(!_funcptr_glBlendEquationSeparateIndexedAMD) ++numFailed;
	return numFailed;
}

PFNGLMULTIDRAWARRAYSINDIRECTAMDPROC _funcptr_glMultiDrawArraysIndirectAMD = NULL;
PFNGLMULTIDRAWELEMENTSINDIRECTAMDPROC _funcptr_glMultiDrawElementsIndirectAMD = NULL;

static int LoadExt_AMD_multi_draw_indirect()
{
	int numFailed = 0;
	_funcptr_glMultiDrawArraysIndirectAMD = (PFNGLMULTIDRAWARRAYSINDIRECTAMDPROC)IntGetProcAddress("glMultiDrawArraysIndirectAMD");
	if(!_funcptr_glMultiDrawArraysIndirectAMD) ++numFailed;
	_funcptr_glMultiDrawElementsIndirectAMD = (PFNGLMULTIDRAWELEMENTSINDIRECTAMDPROC)IntGetProcAddress("glMultiDrawElementsIndirectAMD");
	if(!_funcptr_glMultiDrawElementsIndirectAMD) ++numFailed;
	return numFailed;
}

PFNGLGENNAMESAMDPROC _funcptr_glGenNamesAMD = NULL;
PFNGLDELETENAMESAMDPROC _funcptr_glDeleteNamesAMD = NULL;
PFNGLISNAMEAMDPROC _funcptr_glIsNameAMD = NULL;

static int LoadExt_AMD_name_gen_delete()
{
	int numFailed = 0;
	_funcptr_glGenNamesAMD = (PFNGLGENNAMESAMDPROC)IntGetProcAddress("glGenNamesAMD");
	if(!_funcptr_glGenNamesAMD) ++numFailed;
	_funcptr_glDeleteNamesAMD = (PFNGLDELETENAMESAMDPROC)IntGetProcAddress("glDeleteNamesAMD");
	if(!_funcptr_glDeleteNamesAMD) ++numFailed;
	_funcptr_glIsNameAMD = (PFNGLISNAMEAMDPROC)IntGetProcAddress("glIsNameAMD");
	if(!_funcptr_glIsNameAMD) ++numFailed;
	return numFailed;
}

PFNGLGETPERFMONITORGROUPSAMDPROC _funcptr_glGetPerfMonitorGroupsAMD = NULL;
PFNGLGETPERFMONITORCOUNTERSAMDPROC _funcptr_glGetPerfMonitorCountersAMD = NULL;
PFNGLGETPERFMONITORGROUPSTRINGAMDPROC _funcptr_glGetPerfMonitorGroupStringAMD = NULL;
PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC _funcptr_glGetPerfMonitorCounterStringAMD = NULL;
PFNGLGETPERFMONITORCOUNTERINFOAMDPROC _funcptr_glGetPerfMonitorCounterInfoAMD = NULL;
PFNGLGENPERFMONITORSAMDPROC _funcptr_glGenPerfMonitorsAMD = NULL;
PFNGLDELETEPERFMONITORSAMDPROC _funcptr_glDeletePerfMonitorsAMD = NULL;
PFNGLSELECTPERFMONITORCOUNTERSAMDPROC _funcptr_glSelectPerfMonitorCountersAMD = NULL;
PFNGLBEGINPERFMONITORAMDPROC _funcptr_glBeginPerfMonitorAMD = NULL;
PFNGLENDPERFMONITORAMDPROC _funcptr_glEndPerfMonitorAMD = NULL;
PFNGLGETPERFMONITORCOUNTERDATAAMDPROC _funcptr_glGetPerfMonitorCounterDataAMD = NULL;

static int LoadExt_AMD_performance_monitor()
{
	int numFailed = 0;
	_funcptr_glGetPerfMonitorGroupsAMD = (PFNGLGETPERFMONITORGROUPSAMDPROC)IntGetProcAddress("glGetPerfMonitorGroupsAMD");
	if(!_funcptr_glGetPerfMonitorGroupsAMD) ++numFailed;
	_funcptr_glGetPerfMonitorCountersAMD = (PFNGLGETPERFMONITORCOUNTERSAMDPROC)IntGetProcAddress("glGetPerfMonitorCountersAMD");
	if(!_funcptr_glGetPerfMonitorCountersAMD) ++numFailed;
	_funcptr_glGetPerfMonitorGroupStringAMD = (PFNGLGETPERFMONITORGROUPSTRINGAMDPROC)IntGetProcAddress("glGetPerfMonitorGroupStringAMD");
	if(!_funcptr_glGetPerfMonitorGroupStringAMD) ++numFailed;
	_funcptr_glGetPerfMonitorCounterStringAMD = (PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC)IntGetProcAddress("glGetPerfMonitorCounterStringAMD");
	if(!_funcptr_glGetPerfMonitorCounterStringAMD) ++numFailed;
	_funcptr_glGetPerfMonitorCounterInfoAMD = (PFNGLGETPERFMONITORCOUNTERINFOAMDPROC)IntGetProcAddress("glGetPerfMonitorCounterInfoAMD");
	if(!_funcptr_glGetPerfMonitorCounterInfoAMD) ++numFailed;
	_funcptr_glGenPerfMonitorsAMD = (PFNGLGENPERFMONITORSAMDPROC)IntGetProcAddress("glGenPerfMonitorsAMD");
	if(!_funcptr_glGenPerfMonitorsAMD) ++numFailed;
	_funcptr_glDeletePerfMonitorsAMD = (PFNGLDELETEPERFMONITORSAMDPROC)IntGetProcAddress("glDeletePerfMonitorsAMD");
	if(!_funcptr_glDeletePerfMonitorsAMD) ++numFailed;
	_funcptr_glSelectPerfMonitorCountersAMD = (PFNGLSELECTPERFMONITORCOUNTERSAMDPROC)IntGetProcAddress("glSelectPerfMonitorCountersAMD");
	if(!_funcptr_glSelectPerfMonitorCountersAMD) ++numFailed;
	_funcptr_glBeginPerfMonitorAMD = (PFNGLBEGINPERFMONITORAMDPROC)IntGetProcAddress("glBeginPerfMonitorAMD");
	if(!_funcptr_glBeginPerfMonitorAMD) ++numFailed;
	_funcptr_glEndPerfMonitorAMD = (PFNGLENDPERFMONITORAMDPROC)IntGetProcAddress("glEndPerfMonitorAMD");
	if(!_funcptr_glEndPerfMonitorAMD) ++numFailed;
	_funcptr_glGetPerfMonitorCounterDataAMD = (PFNGLGETPERFMONITORCOUNTERDATAAMDPROC)IntGetProcAddress("glGetPerfMonitorCounterDataAMD");
	if(!_funcptr_glGetPerfMonitorCounterDataAMD) ++numFailed;
	return numFailed;
}

PFNGLSETMULTISAMPLEFVAMDPROC _funcptr_glSetMultisamplefvAMD = NULL;

static int LoadExt_AMD_sample_positions()
{
	int numFailed = 0;
	_funcptr_glSetMultisamplefvAMD = (PFNGLSETMULTISAMPLEFVAMDPROC)IntGetProcAddress("glSetMultisamplefvAMD");
	if(!_funcptr_glSetMultisamplefvAMD) ++numFailed;
	return numFailed;
}

PFNGLTEXSTORAGESPARSEAMDPROC _funcptr_glTexStorageSparseAMD = NULL;
PFNGLTEXTURESTORAGESPARSEAMDPROC _funcptr_glTextureStorageSparseAMD = NULL;

static int LoadExt_AMD_sparse_texture()
{
	int numFailed = 0;
	_funcptr_glTexStorageSparseAMD = (PFNGLTEXSTORAGESPARSEAMDPROC)IntGetProcAddress("glTexStorageSparseAMD");
	if(!_funcptr_glTexStorageSparseAMD) ++numFailed;
	_funcptr_glTextureStorageSparseAMD = (PFNGLTEXTURESTORAGESPARSEAMDPROC)IntGetProcAddress("glTextureStorageSparseAMD");
	if(!_funcptr_glTextureStorageSparseAMD) ++numFailed;
	return numFailed;
}

PFNGLSTENCILOPVALUEAMDPROC _funcptr_glStencilOpValueAMD = NULL;

static int LoadExt_AMD_stencil_operation_extended()
{
	int numFailed = 0;
	_funcptr_glStencilOpValueAMD = (PFNGLSTENCILOPVALUEAMDPROC)IntGetProcAddress("glStencilOpValueAMD");
	if(!_funcptr_glStencilOpValueAMD) ++numFailed;
	return numFailed;
}

PFNGLTESSELLATIONFACTORAMDPROC _funcptr_glTessellationFactorAMD = NULL;
PFNGLTESSELLATIONMODEAMDPROC _funcptr_glTessellationModeAMD = NULL;

static int LoadExt_AMD_vertex_shader_tessellator()
{
	int numFailed = 0;
	_funcptr_glTessellationFactorAMD = (PFNGLTESSELLATIONFACTORAMDPROC)IntGetProcAddress("glTessellationFactorAMD");
	if(!_funcptr_glTessellationFactorAMD) ++numFailed;
	_funcptr_glTessellationModeAMD = (PFNGLTESSELLATIONMODEAMDPROC)IntGetProcAddress("glTessellationModeAMD");
	if(!_funcptr_glTessellationModeAMD) ++numFailed;
	return numFailed;
}

PFNGLELEMENTPOINTERAPPLEPROC _funcptr_glElementPointerAPPLE = NULL;
PFNGLDRAWELEMENTARRAYAPPLEPROC _funcptr_glDrawElementArrayAPPLE = NULL;
PFNGLDRAWRANGEELEMENTARRAYAPPLEPROC _funcptr_glDrawRangeElementArrayAPPLE = NULL;
PFNGLMULTIDRAWELEMENTARRAYAPPLEPROC _funcptr_glMultiDrawElementArrayAPPLE = NULL;
PFNGLMULTIDRAWRANGEELEMENTARRAYAPPLEPROC _funcptr_glMultiDrawRangeElementArrayAPPLE = NULL;

static int LoadExt_APPLE_element_array()
{
	int numFailed = 0;
	_funcptr_glElementPointerAPPLE = (PFNGLELEMENTPOINTERAPPLEPROC)IntGetProcAddress("glElementPointerAPPLE");
	if(!_funcptr_glElementPointerAPPLE) ++numFailed;
	_funcptr_glDrawElementArrayAPPLE = (PFNGLDRAWELEMENTARRAYAPPLEPROC)IntGetProcAddress("glDrawElementArrayAPPLE");
	if(!_funcptr_glDrawElementArrayAPPLE) ++numFailed;
	_funcptr_glDrawRangeElementArrayAPPLE = (PFNGLDRAWRANGEELEMENTARRAYAPPLEPROC)IntGetProcAddress("glDrawRangeElementArrayAPPLE");
	if(!_funcptr_glDrawRangeElementArrayAPPLE) ++numFailed;
	_funcptr_glMultiDrawElementArrayAPPLE = (PFNGLMULTIDRAWELEMENTARRAYAPPLEPROC)IntGetProcAddress("glMultiDrawElementArrayAPPLE");
	if(!_funcptr_glMultiDrawElementArrayAPPLE) ++numFailed;
	_funcptr_glMultiDrawRangeElementArrayAPPLE = (PFNGLMULTIDRAWRANGEELEMENTARRAYAPPLEPROC)IntGetProcAddress("glMultiDrawRangeElementArrayAPPLE");
	if(!_funcptr_glMultiDrawRangeElementArrayAPPLE) ++numFailed;
	return numFailed;
}

PFNGLGENFENCESAPPLEPROC _funcptr_glGenFencesAPPLE = NULL;
PFNGLDELETEFENCESAPPLEPROC _funcptr_glDeleteFencesAPPLE = NULL;
PFNGLSETFENCEAPPLEPROC _funcptr_glSetFenceAPPLE = NULL;
PFNGLISFENCEAPPLEPROC _funcptr_glIsFenceAPPLE = NULL;
PFNGLTESTFENCEAPPLEPROC _funcptr_glTestFenceAPPLE = NULL;
PFNGLFINISHFENCEAPPLEPROC _funcptr_glFinishFenceAPPLE = NULL;
PFNGLTESTOBJECTAPPLEPROC _funcptr_glTestObjectAPPLE = NULL;
PFNGLFINISHOBJECTAPPLEPROC _funcptr_glFinishObjectAPPLE = NULL;

static int LoadExt_APPLE_fence()
{
	int numFailed = 0;
	_funcptr_glGenFencesAPPLE = (PFNGLGENFENCESAPPLEPROC)IntGetProcAddress("glGenFencesAPPLE");
	if(!_funcptr_glGenFencesAPPLE) ++numFailed;
	_funcptr_glDeleteFencesAPPLE = (PFNGLDELETEFENCESAPPLEPROC)IntGetProcAddress("glDeleteFencesAPPLE");
	if(!_funcptr_glDeleteFencesAPPLE) ++numFailed;
	_funcptr_glSetFenceAPPLE = (PFNGLSETFENCEAPPLEPROC)IntGetProcAddress("glSetFenceAPPLE");
	if(!_funcptr_glSetFenceAPPLE) ++numFailed;
	_funcptr_glIsFenceAPPLE = (PFNGLISFENCEAPPLEPROC)IntGetProcAddress("glIsFenceAPPLE");
	if(!_funcptr_glIsFenceAPPLE) ++numFailed;
	_funcptr_glTestFenceAPPLE = (PFNGLTESTFENCEAPPLEPROC)IntGetProcAddress("glTestFenceAPPLE");
	if(!_funcptr_glTestFenceAPPLE) ++numFailed;
	_funcptr_glFinishFenceAPPLE = (PFNGLFINISHFENCEAPPLEPROC)IntGetProcAddress("glFinishFenceAPPLE");
	if(!_funcptr_glFinishFenceAPPLE) ++numFailed;
	_funcptr_glTestObjectAPPLE = (PFNGLTESTOBJECTAPPLEPROC)IntGetProcAddress("glTestObjectAPPLE");
	if(!_funcptr_glTestObjectAPPLE) ++numFailed;
	_funcptr_glFinishObjectAPPLE = (PFNGLFINISHOBJECTAPPLEPROC)IntGetProcAddress("glFinishObjectAPPLE");
	if(!_funcptr_glFinishObjectAPPLE) ++numFailed;
	return numFailed;
}

PFNGLBUFFERPARAMETERIAPPLEPROC _funcptr_glBufferParameteriAPPLE = NULL;
PFNGLFLUSHMAPPEDBUFFERRANGEAPPLEPROC _funcptr_glFlushMappedBufferRangeAPPLE = NULL;

static int LoadExt_APPLE_flush_buffer_range()
{
	int numFailed = 0;
	_funcptr_glBufferParameteriAPPLE = (PFNGLBUFFERPARAMETERIAPPLEPROC)IntGetProcAddress("glBufferParameteriAPPLE");
	if(!_funcptr_glBufferParameteriAPPLE) ++numFailed;
	_funcptr_glFlushMappedBufferRangeAPPLE = (PFNGLFLUSHMAPPEDBUFFERRANGEAPPLEPROC)IntGetProcAddress("glFlushMappedBufferRangeAPPLE");
	if(!_funcptr_glFlushMappedBufferRangeAPPLE) ++numFailed;
	return numFailed;
}

PFNGLOBJECTPURGEABLEAPPLEPROC _funcptr_glObjectPurgeableAPPLE = NULL;
PFNGLOBJECTUNPURGEABLEAPPLEPROC _funcptr_glObjectUnpurgeableAPPLE = NULL;
PFNGLGETOBJECTPARAMETERIVAPPLEPROC _funcptr_glGetObjectParameterivAPPLE = NULL;

static int LoadExt_APPLE_object_purgeable()
{
	int numFailed = 0;
	_funcptr_glObjectPurgeableAPPLE = (PFNGLOBJECTPURGEABLEAPPLEPROC)IntGetProcAddress("glObjectPurgeableAPPLE");
	if(!_funcptr_glObjectPurgeableAPPLE) ++numFailed;
	_funcptr_glObjectUnpurgeableAPPLE = (PFNGLOBJECTUNPURGEABLEAPPLEPROC)IntGetProcAddress("glObjectUnpurgeableAPPLE");
	if(!_funcptr_glObjectUnpurgeableAPPLE) ++numFailed;
	_funcptr_glGetObjectParameterivAPPLE = (PFNGLGETOBJECTPARAMETERIVAPPLEPROC)IntGetProcAddress("glGetObjectParameterivAPPLE");
	if(!_funcptr_glGetObjectParameterivAPPLE) ++numFailed;
	return numFailed;
}

PFNGLTEXTURERANGEAPPLEPROC _funcptr_glTextureRangeAPPLE = NULL;
PFNGLGETTEXPARAMETERPOINTERVAPPLEPROC _funcptr_glGetTexParameterPointervAPPLE = NULL;

static int LoadExt_APPLE_texture_range()
{
	int numFailed = 0;
	_funcptr_glTextureRangeAPPLE = (PFNGLTEXTURERANGEAPPLEPROC)IntGetProcAddress("glTextureRangeAPPLE");
	if(!_funcptr_glTextureRangeAPPLE) ++numFailed;
	_funcptr_glGetTexParameterPointervAPPLE = (PFNGLGETTEXPARAMETERPOINTERVAPPLEPROC)IntGetProcAddress("glGetTexParameterPointervAPPLE");
	if(!_funcptr_glGetTexParameterPointervAPPLE) ++numFailed;
	return numFailed;
}

PFNGLBINDVERTEXARRAYAPPLEPROC _funcptr_glBindVertexArrayAPPLE = NULL;
PFNGLDELETEVERTEXARRAYSAPPLEPROC _funcptr_glDeleteVertexArraysAPPLE = NULL;
PFNGLGENVERTEXARRAYSAPPLEPROC _funcptr_glGenVertexArraysAPPLE = NULL;
PFNGLISVERTEXARRAYAPPLEPROC _funcptr_glIsVertexArrayAPPLE = NULL;

static int LoadExt_APPLE_vertex_array_object()
{
	int numFailed = 0;
	_funcptr_glBindVertexArrayAPPLE = (PFNGLBINDVERTEXARRAYAPPLEPROC)IntGetProcAddress("glBindVertexArrayAPPLE");
	if(!_funcptr_glBindVertexArrayAPPLE) ++numFailed;
	_funcptr_glDeleteVertexArraysAPPLE = (PFNGLDELETEVERTEXARRAYSAPPLEPROC)IntGetProcAddress("glDeleteVertexArraysAPPLE");
	if(!_funcptr_glDeleteVertexArraysAPPLE) ++numFailed;
	_funcptr_glGenVertexArraysAPPLE = (PFNGLGENVERTEXARRAYSAPPLEPROC)IntGetProcAddress("glGenVertexArraysAPPLE");
	if(!_funcptr_glGenVertexArraysAPPLE) ++numFailed;
	_funcptr_glIsVertexArrayAPPLE = (PFNGLISVERTEXARRAYAPPLEPROC)IntGetProcAddress("glIsVertexArrayAPPLE");
	if(!_funcptr_glIsVertexArrayAPPLE) ++numFailed;
	return numFailed;
}

PFNGLVERTEXARRAYRANGEAPPLEPROC _funcptr_glVertexArrayRangeAPPLE = NULL;
PFNGLFLUSHVERTEXARRAYRANGEAPPLEPROC _funcptr_glFlushVertexArrayRangeAPPLE = NULL;
PFNGLVERTEXARRAYPARAMETERIAPPLEPROC _funcptr_glVertexArrayParameteriAPPLE = NULL;

static int LoadExt_APPLE_vertex_array_range()
{
	int numFailed = 0;
	_funcptr_glVertexArrayRangeAPPLE = (PFNGLVERTEXARRAYRANGEAPPLEPROC)IntGetProcAddress("glVertexArrayRangeAPPLE");
	if(!_funcptr_glVertexArrayRangeAPPLE) ++numFailed;
	_funcptr_glFlushVertexArrayRangeAPPLE = (PFNGLFLUSHVERTEXARRAYRANGEAPPLEPROC)IntGetProcAddress("glFlushVertexArrayRangeAPPLE");
	if(!_funcptr_glFlushVertexArrayRangeAPPLE) ++numFailed;
	_funcptr_glVertexArrayParameteriAPPLE = (PFNGLVERTEXARRAYPARAMETERIAPPLEPROC)IntGetProcAddress("glVertexArrayParameteriAPPLE");
	if(!_funcptr_glVertexArrayParameteriAPPLE) ++numFailed;
	return numFailed;
}

PFNGLENABLEVERTEXATTRIBAPPLEPROC _funcptr_glEnableVertexAttribAPPLE = NULL;
PFNGLDISABLEVERTEXATTRIBAPPLEPROC _funcptr_glDisableVertexAttribAPPLE = NULL;
PFNGLISVERTEXATTRIBENABLEDAPPLEPROC _funcptr_glIsVertexAttribEnabledAPPLE = NULL;
PFNGLMAPVERTEXATTRIB1DAPPLEPROC _funcptr_glMapVertexAttrib1dAPPLE = NULL;
PFNGLMAPVERTEXATTRIB1FAPPLEPROC _funcptr_glMapVertexAttrib1fAPPLE = NULL;
PFNGLMAPVERTEXATTRIB2DAPPLEPROC _funcptr_glMapVertexAttrib2dAPPLE = NULL;
PFNGLMAPVERTEXATTRIB2FAPPLEPROC _funcptr_glMapVertexAttrib2fAPPLE = NULL;

static int LoadExt_APPLE_vertex_program_evaluators()
{
	int numFailed = 0;
	_funcptr_glEnableVertexAttribAPPLE = (PFNGLENABLEVERTEXATTRIBAPPLEPROC)IntGetProcAddress("glEnableVertexAttribAPPLE");
	if(!_funcptr_glEnableVertexAttribAPPLE) ++numFailed;
	_funcptr_glDisableVertexAttribAPPLE = (PFNGLDISABLEVERTEXATTRIBAPPLEPROC)IntGetProcAddress("glDisableVertexAttribAPPLE");
	if(!_funcptr_glDisableVertexAttribAPPLE) ++numFailed;
	_funcptr_glIsVertexAttribEnabledAPPLE = (PFNGLISVERTEXATTRIBENABLEDAPPLEPROC)IntGetProcAddress("glIsVertexAttribEnabledAPPLE");
	if(!_funcptr_glIsVertexAttribEnabledAPPLE) ++numFailed;
	_funcptr_glMapVertexAttrib1dAPPLE = (PFNGLMAPVERTEXATTRIB1DAPPLEPROC)IntGetProcAddress("glMapVertexAttrib1dAPPLE");
	if(!_funcptr_glMapVertexAttrib1dAPPLE) ++numFailed;
	_funcptr_glMapVertexAttrib1fAPPLE = (PFNGLMAPVERTEXATTRIB1FAPPLEPROC)IntGetProcAddress("glMapVertexAttrib1fAPPLE");
	if(!_funcptr_glMapVertexAttrib1fAPPLE) ++numFailed;
	_funcptr_glMapVertexAttrib2dAPPLE = (PFNGLMAPVERTEXATTRIB2DAPPLEPROC)IntGetProcAddress("glMapVertexAttrib2dAPPLE");
	if(!_funcptr_glMapVertexAttrib2dAPPLE) ++numFailed;
	_funcptr_glMapVertexAttrib2fAPPLE = (PFNGLMAPVERTEXATTRIB2FAPPLEPROC)IntGetProcAddress("glMapVertexAttrib2fAPPLE");
	if(!_funcptr_glMapVertexAttrib2fAPPLE) ++numFailed;
	return numFailed;
}

PFNGLCREATESYNCFROMCLEVENTARBPROC _funcptr_glCreateSyncFromCLeventARB = NULL;

static int LoadExt_ARB_cl_event()
{
	int numFailed = 0;
	_funcptr_glCreateSyncFromCLeventARB = (PFNGLCREATESYNCFROMCLEVENTARBPROC)IntGetProcAddress("glCreateSyncFromCLeventARB");
	if(!_funcptr_glCreateSyncFromCLeventARB) ++numFailed;
	return numFailed;
}

PFNGLCLAMPCOLORARBPROC _funcptr_glClampColorARB = NULL;

static int LoadExt_ARB_color_buffer_float()
{
	int numFailed = 0;
	_funcptr_glClampColorARB = (PFNGLCLAMPCOLORARBPROC)IntGetProcAddress("glClampColorARB");
	if(!_funcptr_glClampColorARB) ++numFailed;
	return numFailed;
}

PFNGLDEBUGMESSAGECONTROLARBPROC _funcptr_glDebugMessageControlARB = NULL;
PFNGLDEBUGMESSAGEINSERTARBPROC _funcptr_glDebugMessageInsertARB = NULL;
PFNGLDEBUGMESSAGECALLBACKARBPROC _funcptr_glDebugMessageCallbackARB = NULL;
PFNGLGETDEBUGMESSAGELOGARBPROC _funcptr_glGetDebugMessageLogARB = NULL;

static int LoadExt_ARB_debug_output()
{
	int numFailed = 0;
	_funcptr_glDebugMessageControlARB = (PFNGLDEBUGMESSAGECONTROLARBPROC)IntGetProcAddress("glDebugMessageControlARB");
	if(!_funcptr_glDebugMessageControlARB) ++numFailed;
	_funcptr_glDebugMessageInsertARB = (PFNGLDEBUGMESSAGEINSERTARBPROC)IntGetProcAddress("glDebugMessageInsertARB");
	if(!_funcptr_glDebugMessageInsertARB) ++numFailed;
	_funcptr_glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKARBPROC)IntGetProcAddress("glDebugMessageCallbackARB");
	if(!_funcptr_glDebugMessageCallbackARB) ++numFailed;
	_funcptr_glGetDebugMessageLogARB = (PFNGLGETDEBUGMESSAGELOGARBPROC)IntGetProcAddress("glGetDebugMessageLogARB");
	if(!_funcptr_glGetDebugMessageLogARB) ++numFailed;
	return numFailed;
}

PFNGLDRAWBUFFERSARBPROC _funcptr_glDrawBuffersARB = NULL;

static int LoadExt_ARB_draw_buffers()
{
	int numFailed = 0;
	_funcptr_glDrawBuffersARB = (PFNGLDRAWBUFFERSARBPROC)IntGetProcAddress("glDrawBuffersARB");
	if(!_funcptr_glDrawBuffersARB) ++numFailed;
	return numFailed;
}

PFNGLBLENDEQUATIONIARBPROC _funcptr_glBlendEquationiARB = NULL;
PFNGLBLENDEQUATIONSEPARATEIARBPROC _funcptr_glBlendEquationSeparateiARB = NULL;
PFNGLBLENDFUNCIARBPROC _funcptr_glBlendFunciARB = NULL;
PFNGLBLENDFUNCSEPARATEIARBPROC _funcptr_glBlendFuncSeparateiARB = NULL;

static int LoadExt_ARB_draw_buffers_blend()
{
	int numFailed = 0;
	_funcptr_glBlendEquationiARB = (PFNGLBLENDEQUATIONIARBPROC)IntGetProcAddress("glBlendEquationiARB");
	if(!_funcptr_glBlendEquationiARB) ++numFailed;
	_funcptr_glBlendEquationSeparateiARB = (PFNGLBLENDEQUATIONSEPARATEIARBPROC)IntGetProcAddress("glBlendEquationSeparateiARB");
	if(!_funcptr_glBlendEquationSeparateiARB) ++numFailed;
	_funcptr_glBlendFunciARB = (PFNGLBLENDFUNCIARBPROC)IntGetProcAddress("glBlendFunciARB");
	if(!_funcptr_glBlendFunciARB) ++numFailed;
	_funcptr_glBlendFuncSeparateiARB = (PFNGLBLENDFUNCSEPARATEIARBPROC)IntGetProcAddress("glBlendFuncSeparateiARB");
	if(!_funcptr_glBlendFuncSeparateiARB) ++numFailed;
	return numFailed;
}

PFNGLDRAWARRAYSINSTANCEDARBPROC _funcptr_glDrawArraysInstancedARB = NULL;
PFNGLDRAWELEMENTSINSTANCEDARBPROC _funcptr_glDrawElementsInstancedARB = NULL;

static int LoadExt_ARB_draw_instanced()
{
	int numFailed = 0;
	_funcptr_glDrawArraysInstancedARB = (PFNGLDRAWARRAYSINSTANCEDARBPROC)IntGetProcAddress("glDrawArraysInstancedARB");
	if(!_funcptr_glDrawArraysInstancedARB) ++numFailed;
	_funcptr_glDrawElementsInstancedARB = (PFNGLDRAWELEMENTSINSTANCEDARBPROC)IntGetProcAddress("glDrawElementsInstancedARB");
	if(!_funcptr_glDrawElementsInstancedARB) ++numFailed;
	return numFailed;
}

PFNGLPROGRAMPARAMETERIARBPROC _funcptr_glProgramParameteriARB = NULL;
PFNGLFRAMEBUFFERTEXTUREARBPROC _funcptr_glFramebufferTextureARB = NULL;
PFNGLFRAMEBUFFERTEXTURELAYERARBPROC _funcptr_glFramebufferTextureLayerARB = NULL;
PFNGLFRAMEBUFFERTEXTUREFACEARBPROC _funcptr_glFramebufferTextureFaceARB = NULL;

static int LoadExt_ARB_geometry_shader4()
{
	int numFailed = 0;
	_funcptr_glProgramParameteriARB = (PFNGLPROGRAMPARAMETERIARBPROC)IntGetProcAddress("glProgramParameteriARB");
	if(!_funcptr_glProgramParameteriARB) ++numFailed;
	_funcptr_glFramebufferTextureARB = (PFNGLFRAMEBUFFERTEXTUREARBPROC)IntGetProcAddress("glFramebufferTextureARB");
	if(!_funcptr_glFramebufferTextureARB) ++numFailed;
	_funcptr_glFramebufferTextureLayerARB = (PFNGLFRAMEBUFFERTEXTURELAYERARBPROC)IntGetProcAddress("glFramebufferTextureLayerARB");
	if(!_funcptr_glFramebufferTextureLayerARB) ++numFailed;
	_funcptr_glFramebufferTextureFaceARB = (PFNGLFRAMEBUFFERTEXTUREFACEARBPROC)IntGetProcAddress("glFramebufferTextureFaceARB");
	if(!_funcptr_glFramebufferTextureFaceARB) ++numFailed;
	return numFailed;
}

PFNGLVERTEXATTRIBDIVISORARBPROC _funcptr_glVertexAttribDivisorARB = NULL;

static int LoadExt_ARB_instanced_arrays()
{
	int numFailed = 0;
	_funcptr_glVertexAttribDivisorARB = (PFNGLVERTEXATTRIBDIVISORARBPROC)IntGetProcAddress("glVertexAttribDivisorARB");
	if(!_funcptr_glVertexAttribDivisorARB) ++numFailed;
	return numFailed;
}

PFNGLCURRENTPALETTEMATRIXARBPROC _funcptr_glCurrentPaletteMatrixARB = NULL;
PFNGLMATRIXINDEXUBVARBPROC _funcptr_glMatrixIndexubvARB = NULL;
PFNGLMATRIXINDEXUSVARBPROC _funcptr_glMatrixIndexusvARB = NULL;
PFNGLMATRIXINDEXUIVARBPROC _funcptr_glMatrixIndexuivARB = NULL;
PFNGLMATRIXINDEXPOINTERARBPROC _funcptr_glMatrixIndexPointerARB = NULL;

static int LoadExt_ARB_matrix_palette()
{
	int numFailed = 0;
	_funcptr_glCurrentPaletteMatrixARB = (PFNGLCURRENTPALETTEMATRIXARBPROC)IntGetProcAddress("glCurrentPaletteMatrixARB");
	if(!_funcptr_glCurrentPaletteMatrixARB) ++numFailed;
	_funcptr_glMatrixIndexubvARB = (PFNGLMATRIXINDEXUBVARBPROC)IntGetProcAddress("glMatrixIndexubvARB");
	if(!_funcptr_glMatrixIndexubvARB) ++numFailed;
	_funcptr_glMatrixIndexusvARB = (PFNGLMATRIXINDEXUSVARBPROC)IntGetProcAddress("glMatrixIndexusvARB");
	if(!_funcptr_glMatrixIndexusvARB) ++numFailed;
	_funcptr_glMatrixIndexuivARB = (PFNGLMATRIXINDEXUIVARBPROC)IntGetProcAddress("glMatrixIndexuivARB");
	if(!_funcptr_glMatrixIndexuivARB) ++numFailed;
	_funcptr_glMatrixIndexPointerARB = (PFNGLMATRIXINDEXPOINTERARBPROC)IntGetProcAddress("glMatrixIndexPointerARB");
	if(!_funcptr_glMatrixIndexPointerARB) ++numFailed;
	return numFailed;
}

PFNGLSAMPLECOVERAGEARBPROC _funcptr_glSampleCoverageARB = NULL;

static int LoadExt_ARB_multisample()
{
	int numFailed = 0;
	_funcptr_glSampleCoverageARB = (PFNGLSAMPLECOVERAGEARBPROC)IntGetProcAddress("glSampleCoverageARB");
	if(!_funcptr_glSampleCoverageARB) ++numFailed;
	return numFailed;
}

PFNGLACTIVETEXTUREARBPROC _funcptr_glActiveTextureARB = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC _funcptr_glClientActiveTextureARB = NULL;
PFNGLMULTITEXCOORD1DARBPROC _funcptr_glMultiTexCoord1dARB = NULL;
PFNGLMULTITEXCOORD1DVARBPROC _funcptr_glMultiTexCoord1dvARB = NULL;
PFNGLMULTITEXCOORD1FARBPROC _funcptr_glMultiTexCoord1fARB = NULL;
PFNGLMULTITEXCOORD1FVARBPROC _funcptr_glMultiTexCoord1fvARB = NULL;
PFNGLMULTITEXCOORD1IARBPROC _funcptr_glMultiTexCoord1iARB = NULL;
PFNGLMULTITEXCOORD1IVARBPROC _funcptr_glMultiTexCoord1ivARB = NULL;
PFNGLMULTITEXCOORD1SARBPROC _funcptr_glMultiTexCoord1sARB = NULL;
PFNGLMULTITEXCOORD1SVARBPROC _funcptr_glMultiTexCoord1svARB = NULL;
PFNGLMULTITEXCOORD2DARBPROC _funcptr_glMultiTexCoord2dARB = NULL;
PFNGLMULTITEXCOORD2DVARBPROC _funcptr_glMultiTexCoord2dvARB = NULL;
PFNGLMULTITEXCOORD2FARBPROC _funcptr_glMultiTexCoord2fARB = NULL;
PFNGLMULTITEXCOORD2FVARBPROC _funcptr_glMultiTexCoord2fvARB = NULL;
PFNGLMULTITEXCOORD2IARBPROC _funcptr_glMultiTexCoord2iARB = NULL;
PFNGLMULTITEXCOORD2IVARBPROC _funcptr_glMultiTexCoord2ivARB = NULL;
PFNGLMULTITEXCOORD2SARBPROC _funcptr_glMultiTexCoord2sARB = NULL;
PFNGLMULTITEXCOORD2SVARBPROC _funcptr_glMultiTexCoord2svARB = NULL;
PFNGLMULTITEXCOORD3DARBPROC _funcptr_glMultiTexCoord3dARB = NULL;
PFNGLMULTITEXCOORD3DVARBPROC _funcptr_glMultiTexCoord3dvARB = NULL;
PFNGLMULTITEXCOORD3FARBPROC _funcptr_glMultiTexCoord3fARB = NULL;
PFNGLMULTITEXCOORD3FVARBPROC _funcptr_glMultiTexCoord3fvARB = NULL;
PFNGLMULTITEXCOORD3IARBPROC _funcptr_glMultiTexCoord3iARB = NULL;
PFNGLMULTITEXCOORD3IVARBPROC _funcptr_glMultiTexCoord3ivARB = NULL;
PFNGLMULTITEXCOORD3SARBPROC _funcptr_glMultiTexCoord3sARB = NULL;
PFNGLMULTITEXCOORD3SVARBPROC _funcptr_glMultiTexCoord3svARB = NULL;
PFNGLMULTITEXCOORD4DARBPROC _funcptr_glMultiTexCoord4dARB = NULL;
PFNGLMULTITEXCOORD4DVARBPROC _funcptr_glMultiTexCoord4dvARB = NULL;
PFNGLMULTITEXCOORD4FARBPROC _funcptr_glMultiTexCoord4fARB = NULL;
PFNGLMULTITEXCOORD4FVARBPROC _funcptr_glMultiTexCoord4fvARB = NULL;
PFNGLMULTITEXCOORD4IARBPROC _funcptr_glMultiTexCoord4iARB = NULL;
PFNGLMULTITEXCOORD4IVARBPROC _funcptr_glMultiTexCoord4ivARB = NULL;
PFNGLMULTITEXCOORD4SARBPROC _funcptr_glMultiTexCoord4sARB = NULL;
PFNGLMULTITEXCOORD4SVARBPROC _funcptr_glMultiTexCoord4svARB = NULL;

static int LoadExt_ARB_multitexture()
{
	int numFailed = 0;
	_funcptr_glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)IntGetProcAddress("glActiveTextureARB");
	if(!_funcptr_glActiveTextureARB) ++numFailed;
	_funcptr_glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)IntGetProcAddress("glClientActiveTextureARB");
	if(!_funcptr_glClientActiveTextureARB) ++numFailed;
	_funcptr_glMultiTexCoord1dARB = (PFNGLMULTITEXCOORD1DARBPROC)IntGetProcAddress("glMultiTexCoord1dARB");
	if(!_funcptr_glMultiTexCoord1dARB) ++numFailed;
	_funcptr_glMultiTexCoord1dvARB = (PFNGLMULTITEXCOORD1DVARBPROC)IntGetProcAddress("glMultiTexCoord1dvARB");
	if(!_funcptr_glMultiTexCoord1dvARB) ++numFailed;
	_funcptr_glMultiTexCoord1fARB = (PFNGLMULTITEXCOORD1FARBPROC)IntGetProcAddress("glMultiTexCoord1fARB");
	if(!_funcptr_glMultiTexCoord1fARB) ++numFailed;
	_funcptr_glMultiTexCoord1fvARB = (PFNGLMULTITEXCOORD1FVARBPROC)IntGetProcAddress("glMultiTexCoord1fvARB");
	if(!_funcptr_glMultiTexCoord1fvARB) ++numFailed;
	_funcptr_glMultiTexCoord1iARB = (PFNGLMULTITEXCOORD1IARBPROC)IntGetProcAddress("glMultiTexCoord1iARB");
	if(!_funcptr_glMultiTexCoord1iARB) ++numFailed;
	_funcptr_glMultiTexCoord1ivARB = (PFNGLMULTITEXCOORD1IVARBPROC)IntGetProcAddress("glMultiTexCoord1ivARB");
	if(!_funcptr_glMultiTexCoord1ivARB) ++numFailed;
	_funcptr_glMultiTexCoord1sARB = (PFNGLMULTITEXCOORD1SARBPROC)IntGetProcAddress("glMultiTexCoord1sARB");
	if(!_funcptr_glMultiTexCoord1sARB) ++numFailed;
	_funcptr_glMultiTexCoord1svARB = (PFNGLMULTITEXCOORD1SVARBPROC)IntGetProcAddress("glMultiTexCoord1svARB");
	if(!_funcptr_glMultiTexCoord1svARB) ++numFailed;
	_funcptr_glMultiTexCoord2dARB = (PFNGLMULTITEXCOORD2DARBPROC)IntGetProcAddress("glMultiTexCoord2dARB");
	if(!_funcptr_glMultiTexCoord2dARB) ++numFailed;
	_funcptr_glMultiTexCoord2dvARB = (PFNGLMULTITEXCOORD2DVARBPROC)IntGetProcAddress("glMultiTexCoord2dvARB");
	if(!_funcptr_glMultiTexCoord2dvARB) ++numFailed;
	_funcptr_glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)IntGetProcAddress("glMultiTexCoord2fARB");
	if(!_funcptr_glMultiTexCoord2fARB) ++numFailed;
	_funcptr_glMultiTexCoord2fvARB = (PFNGLMULTITEXCOORD2FVARBPROC)IntGetProcAddress("glMultiTexCoord2fvARB");
	if(!_funcptr_glMultiTexCoord2fvARB) ++numFailed;
	_funcptr_glMultiTexCoord2iARB = (PFNGLMULTITEXCOORD2IARBPROC)IntGetProcAddress("glMultiTexCoord2iARB");
	if(!_funcptr_glMultiTexCoord2iARB) ++numFailed;
	_funcptr_glMultiTexCoord2ivARB = (PFNGLMULTITEXCOORD2IVARBPROC)IntGetProcAddress("glMultiTexCoord2ivARB");
	if(!_funcptr_glMultiTexCoord2ivARB) ++numFailed;
	_funcptr_glMultiTexCoord2sARB = (PFNGLMULTITEXCOORD2SARBPROC)IntGetProcAddress("glMultiTexCoord2sARB");
	if(!_funcptr_glMultiTexCoord2sARB) ++numFailed;
	_funcptr_glMultiTexCoord2svARB = (PFNGLMULTITEXCOORD2SVARBPROC)IntGetProcAddress("glMultiTexCoord2svARB");
	if(!_funcptr_glMultiTexCoord2svARB) ++numFailed;
	_funcptr_glMultiTexCoord3dARB = (PFNGLMULTITEXCOORD3DARBPROC)IntGetProcAddress("glMultiTexCoord3dARB");
	if(!_funcptr_glMultiTexCoord3dARB) ++numFailed;
	_funcptr_glMultiTexCoord3dvARB = (PFNGLMULTITEXCOORD3DVARBPROC)IntGetProcAddress("glMultiTexCoord3dvARB");
	if(!_funcptr_glMultiTexCoord3dvARB) ++numFailed;
	_funcptr_glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC)IntGetProcAddress("glMultiTexCoord3fARB");
	if(!_funcptr_glMultiTexCoord3fARB) ++numFailed;
	_funcptr_glMultiTexCoord3fvARB = (PFNGLMULTITEXCOORD3FVARBPROC)IntGetProcAddress("glMultiTexCoord3fvARB");
	if(!_funcptr_glMultiTexCoord3fvARB) ++numFailed;
	_funcptr_glMultiTexCoord3iARB = (PFNGLMULTITEXCOORD3IARBPROC)IntGetProcAddress("glMultiTexCoord3iARB");
	if(!_funcptr_glMultiTexCoord3iARB) ++numFailed;
	_funcptr_glMultiTexCoord3ivARB = (PFNGLMULTITEXCOORD3IVARBPROC)IntGetProcAddress("glMultiTexCoord3ivARB");
	if(!_funcptr_glMultiTexCoord3ivARB) ++numFailed;
	_funcptr_glMultiTexCoord3sARB = (PFNGLMULTITEXCOORD3SARBPROC)IntGetProcAddress("glMultiTexCoord3sARB");
	if(!_funcptr_glMultiTexCoord3sARB) ++numFailed;
	_funcptr_glMultiTexCoord3svARB = (PFNGLMULTITEXCOORD3SVARBPROC)IntGetProcAddress("glMultiTexCoord3svARB");
	if(!_funcptr_glMultiTexCoord3svARB) ++numFailed;
	_funcptr_glMultiTexCoord4dARB = (PFNGLMULTITEXCOORD4DARBPROC)IntGetProcAddress("glMultiTexCoord4dARB");
	if(!_funcptr_glMultiTexCoord4dARB) ++numFailed;
	_funcptr_glMultiTexCoord4dvARB = (PFNGLMULTITEXCOORD4DVARBPROC)IntGetProcAddress("glMultiTexCoord4dvARB");
	if(!_funcptr_glMultiTexCoord4dvARB) ++numFailed;
	_funcptr_glMultiTexCoord4fARB = (PFNGLMULTITEXCOORD4FARBPROC)IntGetProcAddress("glMultiTexCoord4fARB");
	if(!_funcptr_glMultiTexCoord4fARB) ++numFailed;
	_funcptr_glMultiTexCoord4fvARB = (PFNGLMULTITEXCOORD4FVARBPROC)IntGetProcAddress("glMultiTexCoord4fvARB");
	if(!_funcptr_glMultiTexCoord4fvARB) ++numFailed;
	_funcptr_glMultiTexCoord4iARB = (PFNGLMULTITEXCOORD4IARBPROC)IntGetProcAddress("glMultiTexCoord4iARB");
	if(!_funcptr_glMultiTexCoord4iARB) ++numFailed;
	_funcptr_glMultiTexCoord4ivARB = (PFNGLMULTITEXCOORD4IVARBPROC)IntGetProcAddress("glMultiTexCoord4ivARB");
	if(!_funcptr_glMultiTexCoord4ivARB) ++numFailed;
	_funcptr_glMultiTexCoord4sARB = (PFNGLMULTITEXCOORD4SARBPROC)IntGetProcAddress("glMultiTexCoord4sARB");
	if(!_funcptr_glMultiTexCoord4sARB) ++numFailed;
	_funcptr_glMultiTexCoord4svARB = (PFNGLMULTITEXCOORD4SVARBPROC)IntGetProcAddress("glMultiTexCoord4svARB");
	if(!_funcptr_glMultiTexCoord4svARB) ++numFailed;
	return numFailed;
}

PFNGLGENQUERIESARBPROC _funcptr_glGenQueriesARB = NULL;
PFNGLDELETEQUERIESARBPROC _funcptr_glDeleteQueriesARB = NULL;
PFNGLISQUERYARBPROC _funcptr_glIsQueryARB = NULL;
PFNGLBEGINQUERYARBPROC _funcptr_glBeginQueryARB = NULL;
PFNGLENDQUERYARBPROC _funcptr_glEndQueryARB = NULL;
PFNGLGETQUERYIVARBPROC _funcptr_glGetQueryivARB = NULL;
PFNGLGETQUERYOBJECTIVARBPROC _funcptr_glGetQueryObjectivARB = NULL;
PFNGLGETQUERYOBJECTUIVARBPROC _funcptr_glGetQueryObjectuivARB = NULL;

static int LoadExt_ARB_occlusion_query()
{
	int numFailed = 0;
	_funcptr_glGenQueriesARB = (PFNGLGENQUERIESARBPROC)IntGetProcAddress("glGenQueriesARB");
	if(!_funcptr_glGenQueriesARB) ++numFailed;
	_funcptr_glDeleteQueriesARB = (PFNGLDELETEQUERIESARBPROC)IntGetProcAddress("glDeleteQueriesARB");
	if(!_funcptr_glDeleteQueriesARB) ++numFailed;
	_funcptr_glIsQueryARB = (PFNGLISQUERYARBPROC)IntGetProcAddress("glIsQueryARB");
	if(!_funcptr_glIsQueryARB) ++numFailed;
	_funcptr_glBeginQueryARB = (PFNGLBEGINQUERYARBPROC)IntGetProcAddress("glBeginQueryARB");
	if(!_funcptr_glBeginQueryARB) ++numFailed;
	_funcptr_glEndQueryARB = (PFNGLENDQUERYARBPROC)IntGetProcAddress("glEndQueryARB");
	if(!_funcptr_glEndQueryARB) ++numFailed;
	_funcptr_glGetQueryivARB = (PFNGLGETQUERYIVARBPROC)IntGetProcAddress("glGetQueryivARB");
	if(!_funcptr_glGetQueryivARB) ++numFailed;
	_funcptr_glGetQueryObjectivARB = (PFNGLGETQUERYOBJECTIVARBPROC)IntGetProcAddress("glGetQueryObjectivARB");
	if(!_funcptr_glGetQueryObjectivARB) ++numFailed;
	_funcptr_glGetQueryObjectuivARB = (PFNGLGETQUERYOBJECTUIVARBPROC)IntGetProcAddress("glGetQueryObjectuivARB");
	if(!_funcptr_glGetQueryObjectuivARB) ++numFailed;
	return numFailed;
}

PFNGLPOINTPARAMETERFARBPROC _funcptr_glPointParameterfARB = NULL;
PFNGLPOINTPARAMETERFVARBPROC _funcptr_glPointParameterfvARB = NULL;

static int LoadExt_ARB_point_parameters()
{
	int numFailed = 0;
	_funcptr_glPointParameterfARB = (PFNGLPOINTPARAMETERFARBPROC)IntGetProcAddress("glPointParameterfARB");
	if(!_funcptr_glPointParameterfARB) ++numFailed;
	_funcptr_glPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC)IntGetProcAddress("glPointParameterfvARB");
	if(!_funcptr_glPointParameterfvARB) ++numFailed;
	return numFailed;
}

PFNGLGETGRAPHICSRESETSTATUSARBPROC _funcptr_glGetGraphicsResetStatusARB = NULL;
PFNGLGETNMAPDVARBPROC _funcptr_glGetnMapdvARB = NULL;
PFNGLGETNMAPFVARBPROC _funcptr_glGetnMapfvARB = NULL;
PFNGLGETNMAPIVARBPROC _funcptr_glGetnMapivARB = NULL;
PFNGLGETNPIXELMAPFVARBPROC _funcptr_glGetnPixelMapfvARB = NULL;
PFNGLGETNPIXELMAPUIVARBPROC _funcptr_glGetnPixelMapuivARB = NULL;
PFNGLGETNPIXELMAPUSVARBPROC _funcptr_glGetnPixelMapusvARB = NULL;
PFNGLGETNPOLYGONSTIPPLEARBPROC _funcptr_glGetnPolygonStippleARB = NULL;
PFNGLGETNCOLORTABLEARBPROC _funcptr_glGetnColorTableARB = NULL;
PFNGLGETNCONVOLUTIONFILTERARBPROC _funcptr_glGetnConvolutionFilterARB = NULL;
PFNGLGETNSEPARABLEFILTERARBPROC _funcptr_glGetnSeparableFilterARB = NULL;
PFNGLGETNHISTOGRAMARBPROC _funcptr_glGetnHistogramARB = NULL;
PFNGLGETNMINMAXARBPROC _funcptr_glGetnMinmaxARB = NULL;
PFNGLGETNTEXIMAGEARBPROC _funcptr_glGetnTexImageARB = NULL;
PFNGLREADNPIXELSARBPROC _funcptr_glReadnPixelsARB = NULL;
PFNGLGETNCOMPRESSEDTEXIMAGEARBPROC _funcptr_glGetnCompressedTexImageARB = NULL;
PFNGLGETNUNIFORMFVARBPROC _funcptr_glGetnUniformfvARB = NULL;
PFNGLGETNUNIFORMIVARBPROC _funcptr_glGetnUniformivARB = NULL;
PFNGLGETNUNIFORMUIVARBPROC _funcptr_glGetnUniformuivARB = NULL;
PFNGLGETNUNIFORMDVARBPROC _funcptr_glGetnUniformdvARB = NULL;

static int LoadExt_ARB_robustness()
{
	int numFailed = 0;
	_funcptr_glGetGraphicsResetStatusARB = (PFNGLGETGRAPHICSRESETSTATUSARBPROC)IntGetProcAddress("glGetGraphicsResetStatusARB");
	if(!_funcptr_glGetGraphicsResetStatusARB) ++numFailed;
	_funcptr_glGetnMapdvARB = (PFNGLGETNMAPDVARBPROC)IntGetProcAddress("glGetnMapdvARB");
	if(!_funcptr_glGetnMapdvARB) ++numFailed;
	_funcptr_glGetnMapfvARB = (PFNGLGETNMAPFVARBPROC)IntGetProcAddress("glGetnMapfvARB");
	if(!_funcptr_glGetnMapfvARB) ++numFailed;
	_funcptr_glGetnMapivARB = (PFNGLGETNMAPIVARBPROC)IntGetProcAddress("glGetnMapivARB");
	if(!_funcptr_glGetnMapivARB) ++numFailed;
	_funcptr_glGetnPixelMapfvARB = (PFNGLGETNPIXELMAPFVARBPROC)IntGetProcAddress("glGetnPixelMapfvARB");
	if(!_funcptr_glGetnPixelMapfvARB) ++numFailed;
	_funcptr_glGetnPixelMapuivARB = (PFNGLGETNPIXELMAPUIVARBPROC)IntGetProcAddress("glGetnPixelMapuivARB");
	if(!_funcptr_glGetnPixelMapuivARB) ++numFailed;
	_funcptr_glGetnPixelMapusvARB = (PFNGLGETNPIXELMAPUSVARBPROC)IntGetProcAddress("glGetnPixelMapusvARB");
	if(!_funcptr_glGetnPixelMapusvARB) ++numFailed;
	_funcptr_glGetnPolygonStippleARB = (PFNGLGETNPOLYGONSTIPPLEARBPROC)IntGetProcAddress("glGetnPolygonStippleARB");
	if(!_funcptr_glGetnPolygonStippleARB) ++numFailed;
	_funcptr_glGetnColorTableARB = (PFNGLGETNCOLORTABLEARBPROC)IntGetProcAddress("glGetnColorTableARB");
	if(!_funcptr_glGetnColorTableARB) ++numFailed;
	_funcptr_glGetnConvolutionFilterARB = (PFNGLGETNCONVOLUTIONFILTERARBPROC)IntGetProcAddress("glGetnConvolutionFilterARB");
	if(!_funcptr_glGetnConvolutionFilterARB) ++numFailed;
	_funcptr_glGetnSeparableFilterARB = (PFNGLGETNSEPARABLEFILTERARBPROC)IntGetProcAddress("glGetnSeparableFilterARB");
	if(!_funcptr_glGetnSeparableFilterARB) ++numFailed;
	_funcptr_glGetnHistogramARB = (PFNGLGETNHISTOGRAMARBPROC)IntGetProcAddress("glGetnHistogramARB");
	if(!_funcptr_glGetnHistogramARB) ++numFailed;
	_funcptr_glGetnMinmaxARB = (PFNGLGETNMINMAXARBPROC)IntGetProcAddress("glGetnMinmaxARB");
	if(!_funcptr_glGetnMinmaxARB) ++numFailed;
	_funcptr_glGetnTexImageARB = (PFNGLGETNTEXIMAGEARBPROC)IntGetProcAddress("glGetnTexImageARB");
	if(!_funcptr_glGetnTexImageARB) ++numFailed;
	_funcptr_glReadnPixelsARB = (PFNGLREADNPIXELSARBPROC)IntGetProcAddress("glReadnPixelsARB");
	if(!_funcptr_glReadnPixelsARB) ++numFailed;
	_funcptr_glGetnCompressedTexImageARB = (PFNGLGETNCOMPRESSEDTEXIMAGEARBPROC)IntGetProcAddress("glGetnCompressedTexImageARB");
	if(!_funcptr_glGetnCompressedTexImageARB) ++numFailed;
	_funcptr_glGetnUniformfvARB = (PFNGLGETNUNIFORMFVARBPROC)IntGetProcAddress("glGetnUniformfvARB");
	if(!_funcptr_glGetnUniformfvARB) ++numFailed;
	_funcptr_glGetnUniformivARB = (PFNGLGETNUNIFORMIVARBPROC)IntGetProcAddress("glGetnUniformivARB");
	if(!_funcptr_glGetnUniformivARB) ++numFailed;
	_funcptr_glGetnUniformuivARB = (PFNGLGETNUNIFORMUIVARBPROC)IntGetProcAddress("glGetnUniformuivARB");
	if(!_funcptr_glGetnUniformuivARB) ++numFailed;
	_funcptr_glGetnUniformdvARB = (PFNGLGETNUNIFORMDVARBPROC)IntGetProcAddress("glGetnUniformdvARB");
	if(!_funcptr_glGetnUniformdvARB) ++numFailed;
	return numFailed;
}

PFNGLMINSAMPLESHADINGARBPROC _funcptr_glMinSampleShadingARB = NULL;

static int LoadExt_ARB_sample_shading()
{
	int numFailed = 0;
	_funcptr_glMinSampleShadingARB = (PFNGLMINSAMPLESHADINGARBPROC)IntGetProcAddress("glMinSampleShadingARB");
	if(!_funcptr_glMinSampleShadingARB) ++numFailed;
	return numFailed;
}

PFNGLDELETEOBJECTARBPROC _funcptr_glDeleteObjectARB = NULL;
PFNGLGETHANDLEARBPROC _funcptr_glGetHandleARB = NULL;
PFNGLDETACHOBJECTARBPROC _funcptr_glDetachObjectARB = NULL;
PFNGLCREATESHADEROBJECTARBPROC _funcptr_glCreateShaderObjectARB = NULL;
PFNGLSHADERSOURCEARBPROC _funcptr_glShaderSourceARB = NULL;
PFNGLCOMPILESHADERARBPROC _funcptr_glCompileShaderARB = NULL;
PFNGLCREATEPROGRAMOBJECTARBPROC _funcptr_glCreateProgramObjectARB = NULL;
PFNGLATTACHOBJECTARBPROC _funcptr_glAttachObjectARB = NULL;
PFNGLLINKPROGRAMARBPROC _funcptr_glLinkProgramARB = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC _funcptr_glUseProgramObjectARB = NULL;
PFNGLVALIDATEPROGRAMARBPROC _funcptr_glValidateProgramARB = NULL;
PFNGLUNIFORM1FARBPROC _funcptr_glUniform1fARB = NULL;
PFNGLUNIFORM2FARBPROC _funcptr_glUniform2fARB = NULL;
PFNGLUNIFORM3FARBPROC _funcptr_glUniform3fARB = NULL;
PFNGLUNIFORM4FARBPROC _funcptr_glUniform4fARB = NULL;
PFNGLUNIFORM1IARBPROC _funcptr_glUniform1iARB = NULL;
PFNGLUNIFORM2IARBPROC _funcptr_glUniform2iARB = NULL;
PFNGLUNIFORM3IARBPROC _funcptr_glUniform3iARB = NULL;
PFNGLUNIFORM4IARBPROC _funcptr_glUniform4iARB = NULL;
PFNGLUNIFORM1FVARBPROC _funcptr_glUniform1fvARB = NULL;
PFNGLUNIFORM2FVARBPROC _funcptr_glUniform2fvARB = NULL;
PFNGLUNIFORM3FVARBPROC _funcptr_glUniform3fvARB = NULL;
PFNGLUNIFORM4FVARBPROC _funcptr_glUniform4fvARB = NULL;
PFNGLUNIFORM1IVARBPROC _funcptr_glUniform1ivARB = NULL;
PFNGLUNIFORM2IVARBPROC _funcptr_glUniform2ivARB = NULL;
PFNGLUNIFORM3IVARBPROC _funcptr_glUniform3ivARB = NULL;
PFNGLUNIFORM4IVARBPROC _funcptr_glUniform4ivARB = NULL;
PFNGLUNIFORMMATRIX2FVARBPROC _funcptr_glUniformMatrix2fvARB = NULL;
PFNGLUNIFORMMATRIX3FVARBPROC _funcptr_glUniformMatrix3fvARB = NULL;
PFNGLUNIFORMMATRIX4FVARBPROC _funcptr_glUniformMatrix4fvARB = NULL;
PFNGLGETOBJECTPARAMETERFVARBPROC _funcptr_glGetObjectParameterfvARB = NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC _funcptr_glGetObjectParameterivARB = NULL;
PFNGLGETINFOLOGARBPROC _funcptr_glGetInfoLogARB = NULL;
PFNGLGETATTACHEDOBJECTSARBPROC _funcptr_glGetAttachedObjectsARB = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC _funcptr_glGetUniformLocationARB = NULL;
PFNGLGETACTIVEUNIFORMARBPROC _funcptr_glGetActiveUniformARB = NULL;
PFNGLGETUNIFORMFVARBPROC _funcptr_glGetUniformfvARB = NULL;
PFNGLGETUNIFORMIVARBPROC _funcptr_glGetUniformivARB = NULL;
PFNGLGETSHADERSOURCEARBPROC _funcptr_glGetShaderSourceARB = NULL;

static int LoadExt_ARB_shader_objects()
{
	int numFailed = 0;
	_funcptr_glDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)IntGetProcAddress("glDeleteObjectARB");
	if(!_funcptr_glDeleteObjectARB) ++numFailed;
	_funcptr_glGetHandleARB = (PFNGLGETHANDLEARBPROC)IntGetProcAddress("glGetHandleARB");
	if(!_funcptr_glGetHandleARB) ++numFailed;
	_funcptr_glDetachObjectARB = (PFNGLDETACHOBJECTARBPROC)IntGetProcAddress("glDetachObjectARB");
	if(!_funcptr_glDetachObjectARB) ++numFailed;
	_funcptr_glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)IntGetProcAddress("glCreateShaderObjectARB");
	if(!_funcptr_glCreateShaderObjectARB) ++numFailed;
	_funcptr_glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)IntGetProcAddress("glShaderSourceARB");
	if(!_funcptr_glShaderSourceARB) ++numFailed;
	_funcptr_glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)IntGetProcAddress("glCompileShaderARB");
	if(!_funcptr_glCompileShaderARB) ++numFailed;
	_funcptr_glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)IntGetProcAddress("glCreateProgramObjectARB");
	if(!_funcptr_glCreateProgramObjectARB) ++numFailed;
	_funcptr_glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)IntGetProcAddress("glAttachObjectARB");
	if(!_funcptr_glAttachObjectARB) ++numFailed;
	_funcptr_glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)IntGetProcAddress("glLinkProgramARB");
	if(!_funcptr_glLinkProgramARB) ++numFailed;
	_funcptr_glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)IntGetProcAddress("glUseProgramObjectARB");
	if(!_funcptr_glUseProgramObjectARB) ++numFailed;
	_funcptr_glValidateProgramARB = (PFNGLVALIDATEPROGRAMARBPROC)IntGetProcAddress("glValidateProgramARB");
	if(!_funcptr_glValidateProgramARB) ++numFailed;
	_funcptr_glUniform1fARB = (PFNGLUNIFORM1FARBPROC)IntGetProcAddress("glUniform1fARB");
	if(!_funcptr_glUniform1fARB) ++numFailed;
	_funcptr_glUniform2fARB = (PFNGLUNIFORM2FARBPROC)IntGetProcAddress("glUniform2fARB");
	if(!_funcptr_glUniform2fARB) ++numFailed;
	_funcptr_glUniform3fARB = (PFNGLUNIFORM3FARBPROC)IntGetProcAddress("glUniform3fARB");
	if(!_funcptr_glUniform3fARB) ++numFailed;
	_funcptr_glUniform4fARB = (PFNGLUNIFORM4FARBPROC)IntGetProcAddress("glUniform4fARB");
	if(!_funcptr_glUniform4fARB) ++numFailed;
	_funcptr_glUniform1iARB = (PFNGLUNIFORM1IARBPROC)IntGetProcAddress("glUniform1iARB");
	if(!_funcptr_glUniform1iARB) ++numFailed;
	_funcptr_glUniform2iARB = (PFNGLUNIFORM2IARBPROC)IntGetProcAddress("glUniform2iARB");
	if(!_funcptr_glUniform2iARB) ++numFailed;
	_funcptr_glUniform3iARB = (PFNGLUNIFORM3IARBPROC)IntGetProcAddress("glUniform3iARB");
	if(!_funcptr_glUniform3iARB) ++numFailed;
	_funcptr_glUniform4iARB = (PFNGLUNIFORM4IARBPROC)IntGetProcAddress("glUniform4iARB");
	if(!_funcptr_glUniform4iARB) ++numFailed;
	_funcptr_glUniform1fvARB = (PFNGLUNIFORM1FVARBPROC)IntGetProcAddress("glUniform1fvARB");
	if(!_funcptr_glUniform1fvARB) ++numFailed;
	_funcptr_glUniform2fvARB = (PFNGLUNIFORM2FVARBPROC)IntGetProcAddress("glUniform2fvARB");
	if(!_funcptr_glUniform2fvARB) ++numFailed;
	_funcptr_glUniform3fvARB = (PFNGLUNIFORM3FVARBPROC)IntGetProcAddress("glUniform3fvARB");
	if(!_funcptr_glUniform3fvARB) ++numFailed;
	_funcptr_glUniform4fvARB = (PFNGLUNIFORM4FVARBPROC)IntGetProcAddress("glUniform4fvARB");
	if(!_funcptr_glUniform4fvARB) ++numFailed;
	_funcptr_glUniform1ivARB = (PFNGLUNIFORM1IVARBPROC)IntGetProcAddress("glUniform1ivARB");
	if(!_funcptr_glUniform1ivARB) ++numFailed;
	_funcptr_glUniform2ivARB = (PFNGLUNIFORM2IVARBPROC)IntGetProcAddress("glUniform2ivARB");
	if(!_funcptr_glUniform2ivARB) ++numFailed;
	_funcptr_glUniform3ivARB = (PFNGLUNIFORM3IVARBPROC)IntGetProcAddress("glUniform3ivARB");
	if(!_funcptr_glUniform3ivARB) ++numFailed;
	_funcptr_glUniform4ivARB = (PFNGLUNIFORM4IVARBPROC)IntGetProcAddress("glUniform4ivARB");
	if(!_funcptr_glUniform4ivARB) ++numFailed;
	_funcptr_glUniformMatrix2fvARB = (PFNGLUNIFORMMATRIX2FVARBPROC)IntGetProcAddress("glUniformMatrix2fvARB");
	if(!_funcptr_glUniformMatrix2fvARB) ++numFailed;
	_funcptr_glUniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVARBPROC)IntGetProcAddress("glUniformMatrix3fvARB");
	if(!_funcptr_glUniformMatrix3fvARB) ++numFailed;
	_funcptr_glUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC)IntGetProcAddress("glUniformMatrix4fvARB");
	if(!_funcptr_glUniformMatrix4fvARB) ++numFailed;
	_funcptr_glGetObjectParameterfvARB = (PFNGLGETOBJECTPARAMETERFVARBPROC)IntGetProcAddress("glGetObjectParameterfvARB");
	if(!_funcptr_glGetObjectParameterfvARB) ++numFailed;
	_funcptr_glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)IntGetProcAddress("glGetObjectParameterivARB");
	if(!_funcptr_glGetObjectParameterivARB) ++numFailed;
	_funcptr_glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)IntGetProcAddress("glGetInfoLogARB");
	if(!_funcptr_glGetInfoLogARB) ++numFailed;
	_funcptr_glGetAttachedObjectsARB = (PFNGLGETATTACHEDOBJECTSARBPROC)IntGetProcAddress("glGetAttachedObjectsARB");
	if(!_funcptr_glGetAttachedObjectsARB) ++numFailed;
	_funcptr_glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)IntGetProcAddress("glGetUniformLocationARB");
	if(!_funcptr_glGetUniformLocationARB) ++numFailed;
	_funcptr_glGetActiveUniformARB = (PFNGLGETACTIVEUNIFORMARBPROC)IntGetProcAddress("glGetActiveUniformARB");
	if(!_funcptr_glGetActiveUniformARB) ++numFailed;
	_funcptr_glGetUniformfvARB = (PFNGLGETUNIFORMFVARBPROC)IntGetProcAddress("glGetUniformfvARB");
	if(!_funcptr_glGetUniformfvARB) ++numFailed;
	_funcptr_glGetUniformivARB = (PFNGLGETUNIFORMIVARBPROC)IntGetProcAddress("glGetUniformivARB");
	if(!_funcptr_glGetUniformivARB) ++numFailed;
	_funcptr_glGetShaderSourceARB = (PFNGLGETSHADERSOURCEARBPROC)IntGetProcAddress("glGetShaderSourceARB");
	if(!_funcptr_glGetShaderSourceARB) ++numFailed;
	return numFailed;
}

PFNGLNAMEDSTRINGARBPROC _funcptr_glNamedStringARB = NULL;
PFNGLDELETENAMEDSTRINGARBPROC _funcptr_glDeleteNamedStringARB = NULL;
PFNGLCOMPILESHADERINCLUDEARBPROC _funcptr_glCompileShaderIncludeARB = NULL;
PFNGLISNAMEDSTRINGARBPROC _funcptr_glIsNamedStringARB = NULL;
PFNGLGETNAMEDSTRINGARBPROC _funcptr_glGetNamedStringARB = NULL;
PFNGLGETNAMEDSTRINGIVARBPROC _funcptr_glGetNamedStringivARB = NULL;

static int LoadExt_ARB_shading_language_include()
{
	int numFailed = 0;
	_funcptr_glNamedStringARB = (PFNGLNAMEDSTRINGARBPROC)IntGetProcAddress("glNamedStringARB");
	if(!_funcptr_glNamedStringARB) ++numFailed;
	_funcptr_glDeleteNamedStringARB = (PFNGLDELETENAMEDSTRINGARBPROC)IntGetProcAddress("glDeleteNamedStringARB");
	if(!_funcptr_glDeleteNamedStringARB) ++numFailed;
	_funcptr_glCompileShaderIncludeARB = (PFNGLCOMPILESHADERINCLUDEARBPROC)IntGetProcAddress("glCompileShaderIncludeARB");
	if(!_funcptr_glCompileShaderIncludeARB) ++numFailed;
	_funcptr_glIsNamedStringARB = (PFNGLISNAMEDSTRINGARBPROC)IntGetProcAddress("glIsNamedStringARB");
	if(!_funcptr_glIsNamedStringARB) ++numFailed;
	_funcptr_glGetNamedStringARB = (PFNGLGETNAMEDSTRINGARBPROC)IntGetProcAddress("glGetNamedStringARB");
	if(!_funcptr_glGetNamedStringARB) ++numFailed;
	_funcptr_glGetNamedStringivARB = (PFNGLGETNAMEDSTRINGIVARBPROC)IntGetProcAddress("glGetNamedStringivARB");
	if(!_funcptr_glGetNamedStringivARB) ++numFailed;
	return numFailed;
}

PFNGLTEXBUFFERARBPROC _funcptr_glTexBufferARB = NULL;

static int LoadExt_ARB_texture_buffer_object()
{
	int numFailed = 0;
	_funcptr_glTexBufferARB = (PFNGLTEXBUFFERARBPROC)IntGetProcAddress("glTexBufferARB");
	if(!_funcptr_glTexBufferARB) ++numFailed;
	return numFailed;
}

PFNGLCOMPRESSEDTEXIMAGE3DARBPROC _funcptr_glCompressedTexImage3DARB = NULL;
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC _funcptr_glCompressedTexImage2DARB = NULL;
PFNGLCOMPRESSEDTEXIMAGE1DARBPROC _funcptr_glCompressedTexImage1DARB = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC _funcptr_glCompressedTexSubImage3DARB = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC _funcptr_glCompressedTexSubImage2DARB = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC _funcptr_glCompressedTexSubImage1DARB = NULL;
PFNGLGETCOMPRESSEDTEXIMAGEARBPROC _funcptr_glGetCompressedTexImageARB = NULL;

static int LoadExt_ARB_texture_compression()
{
	int numFailed = 0;
	_funcptr_glCompressedTexImage3DARB = (PFNGLCOMPRESSEDTEXIMAGE3DARBPROC)IntGetProcAddress("glCompressedTexImage3DARB");
	if(!_funcptr_glCompressedTexImage3DARB) ++numFailed;
	_funcptr_glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)IntGetProcAddress("glCompressedTexImage2DARB");
	if(!_funcptr_glCompressedTexImage2DARB) ++numFailed;
	_funcptr_glCompressedTexImage1DARB = (PFNGLCOMPRESSEDTEXIMAGE1DARBPROC)IntGetProcAddress("glCompressedTexImage1DARB");
	if(!_funcptr_glCompressedTexImage1DARB) ++numFailed;
	_funcptr_glCompressedTexSubImage3DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC)IntGetProcAddress("glCompressedTexSubImage3DARB");
	if(!_funcptr_glCompressedTexSubImage3DARB) ++numFailed;
	_funcptr_glCompressedTexSubImage2DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC)IntGetProcAddress("glCompressedTexSubImage2DARB");
	if(!_funcptr_glCompressedTexSubImage2DARB) ++numFailed;
	_funcptr_glCompressedTexSubImage1DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC)IntGetProcAddress("glCompressedTexSubImage1DARB");
	if(!_funcptr_glCompressedTexSubImage1DARB) ++numFailed;
	_funcptr_glGetCompressedTexImageARB = (PFNGLGETCOMPRESSEDTEXIMAGEARBPROC)IntGetProcAddress("glGetCompressedTexImageARB");
	if(!_funcptr_glGetCompressedTexImageARB) ++numFailed;
	return numFailed;
}

PFNGLLOADTRANSPOSEMATRIXFARBPROC _funcptr_glLoadTransposeMatrixfARB = NULL;
PFNGLLOADTRANSPOSEMATRIXDARBPROC _funcptr_glLoadTransposeMatrixdARB = NULL;
PFNGLMULTTRANSPOSEMATRIXFARBPROC _funcptr_glMultTransposeMatrixfARB = NULL;
PFNGLMULTTRANSPOSEMATRIXDARBPROC _funcptr_glMultTransposeMatrixdARB = NULL;

static int LoadExt_ARB_transpose_matrix()
{
	int numFailed = 0;
	_funcptr_glLoadTransposeMatrixfARB = (PFNGLLOADTRANSPOSEMATRIXFARBPROC)IntGetProcAddress("glLoadTransposeMatrixfARB");
	if(!_funcptr_glLoadTransposeMatrixfARB) ++numFailed;
	_funcptr_glLoadTransposeMatrixdARB = (PFNGLLOADTRANSPOSEMATRIXDARBPROC)IntGetProcAddress("glLoadTransposeMatrixdARB");
	if(!_funcptr_glLoadTransposeMatrixdARB) ++numFailed;
	_funcptr_glMultTransposeMatrixfARB = (PFNGLMULTTRANSPOSEMATRIXFARBPROC)IntGetProcAddress("glMultTransposeMatrixfARB");
	if(!_funcptr_glMultTransposeMatrixfARB) ++numFailed;
	_funcptr_glMultTransposeMatrixdARB = (PFNGLMULTTRANSPOSEMATRIXDARBPROC)IntGetProcAddress("glMultTransposeMatrixdARB");
	if(!_funcptr_glMultTransposeMatrixdARB) ++numFailed;
	return numFailed;
}

PFNGLWEIGHTBVARBPROC _funcptr_glWeightbvARB = NULL;
PFNGLWEIGHTSVARBPROC _funcptr_glWeightsvARB = NULL;
PFNGLWEIGHTIVARBPROC _funcptr_glWeightivARB = NULL;
PFNGLWEIGHTFVARBPROC _funcptr_glWeightfvARB = NULL;
PFNGLWEIGHTDVARBPROC _funcptr_glWeightdvARB = NULL;
PFNGLWEIGHTUBVARBPROC _funcptr_glWeightubvARB = NULL;
PFNGLWEIGHTUSVARBPROC _funcptr_glWeightusvARB = NULL;
PFNGLWEIGHTUIVARBPROC _funcptr_glWeightuivARB = NULL;
PFNGLWEIGHTPOINTERARBPROC _funcptr_glWeightPointerARB = NULL;
PFNGLVERTEXBLENDARBPROC _funcptr_glVertexBlendARB = NULL;

static int LoadExt_ARB_vertex_blend()
{
	int numFailed = 0;
	_funcptr_glWeightbvARB = (PFNGLWEIGHTBVARBPROC)IntGetProcAddress("glWeightbvARB");
	if(!_funcptr_glWeightbvARB) ++numFailed;
	_funcptr_glWeightsvARB = (PFNGLWEIGHTSVARBPROC)IntGetProcAddress("glWeightsvARB");
	if(!_funcptr_glWeightsvARB) ++numFailed;
	_funcptr_glWeightivARB = (PFNGLWEIGHTIVARBPROC)IntGetProcAddress("glWeightivARB");
	if(!_funcptr_glWeightivARB) ++numFailed;
	_funcptr_glWeightfvARB = (PFNGLWEIGHTFVARBPROC)IntGetProcAddress("glWeightfvARB");
	if(!_funcptr_glWeightfvARB) ++numFailed;
	_funcptr_glWeightdvARB = (PFNGLWEIGHTDVARBPROC)IntGetProcAddress("glWeightdvARB");
	if(!_funcptr_glWeightdvARB) ++numFailed;
	_funcptr_glWeightubvARB = (PFNGLWEIGHTUBVARBPROC)IntGetProcAddress("glWeightubvARB");
	if(!_funcptr_glWeightubvARB) ++numFailed;
	_funcptr_glWeightusvARB = (PFNGLWEIGHTUSVARBPROC)IntGetProcAddress("glWeightusvARB");
	if(!_funcptr_glWeightusvARB) ++numFailed;
	_funcptr_glWeightuivARB = (PFNGLWEIGHTUIVARBPROC)IntGetProcAddress("glWeightuivARB");
	if(!_funcptr_glWeightuivARB) ++numFailed;
	_funcptr_glWeightPointerARB = (PFNGLWEIGHTPOINTERARBPROC)IntGetProcAddress("glWeightPointerARB");
	if(!_funcptr_glWeightPointerARB) ++numFailed;
	_funcptr_glVertexBlendARB = (PFNGLVERTEXBLENDARBPROC)IntGetProcAddress("glVertexBlendARB");
	if(!_funcptr_glVertexBlendARB) ++numFailed;
	return numFailed;
}

PFNGLBINDBUFFERARBPROC _funcptr_glBindBufferARB = NULL;
PFNGLDELETEBUFFERSARBPROC _funcptr_glDeleteBuffersARB = NULL;
PFNGLGENBUFFERSARBPROC _funcptr_glGenBuffersARB = NULL;
PFNGLISBUFFERARBPROC _funcptr_glIsBufferARB = NULL;
PFNGLBUFFERDATAARBPROC _funcptr_glBufferDataARB = NULL;
PFNGLBUFFERSUBDATAARBPROC _funcptr_glBufferSubDataARB = NULL;
PFNGLGETBUFFERSUBDATAARBPROC _funcptr_glGetBufferSubDataARB = NULL;
PFNGLMAPBUFFERARBPROC _funcptr_glMapBufferARB = NULL;
PFNGLUNMAPBUFFERARBPROC _funcptr_glUnmapBufferARB = NULL;
PFNGLGETBUFFERPARAMETERIVARBPROC _funcptr_glGetBufferParameterivARB = NULL;
PFNGLGETBUFFERPOINTERVARBPROC _funcptr_glGetBufferPointervARB = NULL;

static int LoadExt_ARB_vertex_buffer_object()
{
	int numFailed = 0;
	_funcptr_glBindBufferARB = (PFNGLBINDBUFFERARBPROC)IntGetProcAddress("glBindBufferARB");
	if(!_funcptr_glBindBufferARB) ++numFailed;
	_funcptr_glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)IntGetProcAddress("glDeleteBuffersARB");
	if(!_funcptr_glDeleteBuffersARB) ++numFailed;
	_funcptr_glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)IntGetProcAddress("glGenBuffersARB");
	if(!_funcptr_glGenBuffersARB) ++numFailed;
	_funcptr_glIsBufferARB = (PFNGLISBUFFERARBPROC)IntGetProcAddress("glIsBufferARB");
	if(!_funcptr_glIsBufferARB) ++numFailed;
	_funcptr_glBufferDataARB = (PFNGLBUFFERDATAARBPROC)IntGetProcAddress("glBufferDataARB");
	if(!_funcptr_glBufferDataARB) ++numFailed;
	_funcptr_glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)IntGetProcAddress("glBufferSubDataARB");
	if(!_funcptr_glBufferSubDataARB) ++numFailed;
	_funcptr_glGetBufferSubDataARB = (PFNGLGETBUFFERSUBDATAARBPROC)IntGetProcAddress("glGetBufferSubDataARB");
	if(!_funcptr_glGetBufferSubDataARB) ++numFailed;
	_funcptr_glMapBufferARB = (PFNGLMAPBUFFERARBPROC)IntGetProcAddress("glMapBufferARB");
	if(!_funcptr_glMapBufferARB) ++numFailed;
	_funcptr_glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)IntGetProcAddress("glUnmapBufferARB");
	if(!_funcptr_glUnmapBufferARB) ++numFailed;
	_funcptr_glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)IntGetProcAddress("glGetBufferParameterivARB");
	if(!_funcptr_glGetBufferParameterivARB) ++numFailed;
	_funcptr_glGetBufferPointervARB = (PFNGLGETBUFFERPOINTERVARBPROC)IntGetProcAddress("glGetBufferPointervARB");
	if(!_funcptr_glGetBufferPointervARB) ++numFailed;
	return numFailed;
}

PFNGLVERTEXATTRIB1DARBPROC _funcptr_glVertexAttrib1dARB = NULL;
PFNGLVERTEXATTRIB1DVARBPROC _funcptr_glVertexAttrib1dvARB = NULL;
PFNGLVERTEXATTRIB1FARBPROC _funcptr_glVertexAttrib1fARB = NULL;
PFNGLVERTEXATTRIB1FVARBPROC _funcptr_glVertexAttrib1fvARB = NULL;
PFNGLVERTEXATTRIB1SARBPROC _funcptr_glVertexAttrib1sARB = NULL;
PFNGLVERTEXATTRIB1SVARBPROC _funcptr_glVertexAttrib1svARB = NULL;
PFNGLVERTEXATTRIB2DARBPROC _funcptr_glVertexAttrib2dARB = NULL;
PFNGLVERTEXATTRIB2DVARBPROC _funcptr_glVertexAttrib2dvARB = NULL;
PFNGLVERTEXATTRIB2FARBPROC _funcptr_glVertexAttrib2fARB = NULL;
PFNGLVERTEXATTRIB2FVARBPROC _funcptr_glVertexAttrib2fvARB = NULL;
PFNGLVERTEXATTRIB2SARBPROC _funcptr_glVertexAttrib2sARB = NULL;
PFNGLVERTEXATTRIB2SVARBPROC _funcptr_glVertexAttrib2svARB = NULL;
PFNGLVERTEXATTRIB3DARBPROC _funcptr_glVertexAttrib3dARB = NULL;
PFNGLVERTEXATTRIB3DVARBPROC _funcptr_glVertexAttrib3dvARB = NULL;
PFNGLVERTEXATTRIB3FARBPROC _funcptr_glVertexAttrib3fARB = NULL;
PFNGLVERTEXATTRIB3FVARBPROC _funcptr_glVertexAttrib3fvARB = NULL;
PFNGLVERTEXATTRIB3SARBPROC _funcptr_glVertexAttrib3sARB = NULL;
PFNGLVERTEXATTRIB3SVARBPROC _funcptr_glVertexAttrib3svARB = NULL;
PFNGLVERTEXATTRIB4NBVARBPROC _funcptr_glVertexAttrib4NbvARB = NULL;
PFNGLVERTEXATTRIB4NIVARBPROC _funcptr_glVertexAttrib4NivARB = NULL;
PFNGLVERTEXATTRIB4NSVARBPROC _funcptr_glVertexAttrib4NsvARB = NULL;
PFNGLVERTEXATTRIB4NUBARBPROC _funcptr_glVertexAttrib4NubARB = NULL;
PFNGLVERTEXATTRIB4NUBVARBPROC _funcptr_glVertexAttrib4NubvARB = NULL;
PFNGLVERTEXATTRIB4NUIVARBPROC _funcptr_glVertexAttrib4NuivARB = NULL;
PFNGLVERTEXATTRIB4NUSVARBPROC _funcptr_glVertexAttrib4NusvARB = NULL;
PFNGLVERTEXATTRIB4BVARBPROC _funcptr_glVertexAttrib4bvARB = NULL;
PFNGLVERTEXATTRIB4DARBPROC _funcptr_glVertexAttrib4dARB = NULL;
PFNGLVERTEXATTRIB4DVARBPROC _funcptr_glVertexAttrib4dvARB = NULL;
PFNGLVERTEXATTRIB4FARBPROC _funcptr_glVertexAttrib4fARB = NULL;
PFNGLVERTEXATTRIB4FVARBPROC _funcptr_glVertexAttrib4fvARB = NULL;
PFNGLVERTEXATTRIB4IVARBPROC _funcptr_glVertexAttrib4ivARB = NULL;
PFNGLVERTEXATTRIB4SARBPROC _funcptr_glVertexAttrib4sARB = NULL;
PFNGLVERTEXATTRIB4SVARBPROC _funcptr_glVertexAttrib4svARB = NULL;
PFNGLVERTEXATTRIB4UBVARBPROC _funcptr_glVertexAttrib4ubvARB = NULL;
PFNGLVERTEXATTRIB4UIVARBPROC _funcptr_glVertexAttrib4uivARB = NULL;
PFNGLVERTEXATTRIB4USVARBPROC _funcptr_glVertexAttrib4usvARB = NULL;
PFNGLVERTEXATTRIBPOINTERARBPROC _funcptr_glVertexAttribPointerARB = NULL;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC _funcptr_glEnableVertexAttribArrayARB = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC _funcptr_glDisableVertexAttribArrayARB = NULL;
PFNGLPROGRAMSTRINGARBPROC _funcptr_glProgramStringARB = NULL;
PFNGLBINDPROGRAMARBPROC _funcptr_glBindProgramARB = NULL;
PFNGLDELETEPROGRAMSARBPROC _funcptr_glDeleteProgramsARB = NULL;
PFNGLGENPROGRAMSARBPROC _funcptr_glGenProgramsARB = NULL;
PFNGLPROGRAMENVPARAMETER4DARBPROC _funcptr_glProgramEnvParameter4dARB = NULL;
PFNGLPROGRAMENVPARAMETER4DVARBPROC _funcptr_glProgramEnvParameter4dvARB = NULL;
PFNGLPROGRAMENVPARAMETER4FARBPROC _funcptr_glProgramEnvParameter4fARB = NULL;
PFNGLPROGRAMENVPARAMETER4FVARBPROC _funcptr_glProgramEnvParameter4fvARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4DARBPROC _funcptr_glProgramLocalParameter4dARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4DVARBPROC _funcptr_glProgramLocalParameter4dvARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4FARBPROC _funcptr_glProgramLocalParameter4fARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC _funcptr_glProgramLocalParameter4fvARB = NULL;
PFNGLGETPROGRAMENVPARAMETERDVARBPROC _funcptr_glGetProgramEnvParameterdvARB = NULL;
PFNGLGETPROGRAMENVPARAMETERFVARBPROC _funcptr_glGetProgramEnvParameterfvARB = NULL;
PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC _funcptr_glGetProgramLocalParameterdvARB = NULL;
PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC _funcptr_glGetProgramLocalParameterfvARB = NULL;
PFNGLGETPROGRAMIVARBPROC _funcptr_glGetProgramivARB = NULL;
PFNGLGETPROGRAMSTRINGARBPROC _funcptr_glGetProgramStringARB = NULL;
PFNGLGETVERTEXATTRIBDVARBPROC _funcptr_glGetVertexAttribdvARB = NULL;
PFNGLGETVERTEXATTRIBFVARBPROC _funcptr_glGetVertexAttribfvARB = NULL;
PFNGLGETVERTEXATTRIBIVARBPROC _funcptr_glGetVertexAttribivARB = NULL;
PFNGLGETVERTEXATTRIBPOINTERVARBPROC _funcptr_glGetVertexAttribPointervARB = NULL;
PFNGLISPROGRAMARBPROC _funcptr_glIsProgramARB = NULL;

static int LoadExt_ARB_vertex_program()
{
	int numFailed = 0;
	_funcptr_glVertexAttrib1dARB = (PFNGLVERTEXATTRIB1DARBPROC)IntGetProcAddress("glVertexAttrib1dARB");
	if(!_funcptr_glVertexAttrib1dARB) ++numFailed;
	_funcptr_glVertexAttrib1dvARB = (PFNGLVERTEXATTRIB1DVARBPROC)IntGetProcAddress("glVertexAttrib1dvARB");
	if(!_funcptr_glVertexAttrib1dvARB) ++numFailed;
	_funcptr_glVertexAttrib1fARB = (PFNGLVERTEXATTRIB1FARBPROC)IntGetProcAddress("glVertexAttrib1fARB");
	if(!_funcptr_glVertexAttrib1fARB) ++numFailed;
	_funcptr_glVertexAttrib1fvARB = (PFNGLVERTEXATTRIB1FVARBPROC)IntGetProcAddress("glVertexAttrib1fvARB");
	if(!_funcptr_glVertexAttrib1fvARB) ++numFailed;
	_funcptr_glVertexAttrib1sARB = (PFNGLVERTEXATTRIB1SARBPROC)IntGetProcAddress("glVertexAttrib1sARB");
	if(!_funcptr_glVertexAttrib1sARB) ++numFailed;
	_funcptr_glVertexAttrib1svARB = (PFNGLVERTEXATTRIB1SVARBPROC)IntGetProcAddress("glVertexAttrib1svARB");
	if(!_funcptr_glVertexAttrib1svARB) ++numFailed;
	_funcptr_glVertexAttrib2dARB = (PFNGLVERTEXATTRIB2DARBPROC)IntGetProcAddress("glVertexAttrib2dARB");
	if(!_funcptr_glVertexAttrib2dARB) ++numFailed;
	_funcptr_glVertexAttrib2dvARB = (PFNGLVERTEXATTRIB2DVARBPROC)IntGetProcAddress("glVertexAttrib2dvARB");
	if(!_funcptr_glVertexAttrib2dvARB) ++numFailed;
	_funcptr_glVertexAttrib2fARB = (PFNGLVERTEXATTRIB2FARBPROC)IntGetProcAddress("glVertexAttrib2fARB");
	if(!_funcptr_glVertexAttrib2fARB) ++numFailed;
	_funcptr_glVertexAttrib2fvARB = (PFNGLVERTEXATTRIB2FVARBPROC)IntGetProcAddress("glVertexAttrib2fvARB");
	if(!_funcptr_glVertexAttrib2fvARB) ++numFailed;
	_funcptr_glVertexAttrib2sARB = (PFNGLVERTEXATTRIB2SARBPROC)IntGetProcAddress("glVertexAttrib2sARB");
	if(!_funcptr_glVertexAttrib2sARB) ++numFailed;
	_funcptr_glVertexAttrib2svARB = (PFNGLVERTEXATTRIB2SVARBPROC)IntGetProcAddress("glVertexAttrib2svARB");
	if(!_funcptr_glVertexAttrib2svARB) ++numFailed;
	_funcptr_glVertexAttrib3dARB = (PFNGLVERTEXATTRIB3DARBPROC)IntGetProcAddress("glVertexAttrib3dARB");
	if(!_funcptr_glVertexAttrib3dARB) ++numFailed;
	_funcptr_glVertexAttrib3dvARB = (PFNGLVERTEXATTRIB3DVARBPROC)IntGetProcAddress("glVertexAttrib3dvARB");
	if(!_funcptr_glVertexAttrib3dvARB) ++numFailed;
	_funcptr_glVertexAttrib3fARB = (PFNGLVERTEXATTRIB3FARBPROC)IntGetProcAddress("glVertexAttrib3fARB");
	if(!_funcptr_glVertexAttrib3fARB) ++numFailed;
	_funcptr_glVertexAttrib3fvARB = (PFNGLVERTEXATTRIB3FVARBPROC)IntGetProcAddress("glVertexAttrib3fvARB");
	if(!_funcptr_glVertexAttrib3fvARB) ++numFailed;
	_funcptr_glVertexAttrib3sARB = (PFNGLVERTEXATTRIB3SARBPROC)IntGetProcAddress("glVertexAttrib3sARB");
	if(!_funcptr_glVertexAttrib3sARB) ++numFailed;
	_funcptr_glVertexAttrib3svARB = (PFNGLVERTEXATTRIB3SVARBPROC)IntGetProcAddress("glVertexAttrib3svARB");
	if(!_funcptr_glVertexAttrib3svARB) ++numFailed;
	_funcptr_glVertexAttrib4NbvARB = (PFNGLVERTEXATTRIB4NBVARBPROC)IntGetProcAddress("glVertexAttrib4NbvARB");
	if(!_funcptr_glVertexAttrib4NbvARB) ++numFailed;
	_funcptr_glVertexAttrib4NivARB = (PFNGLVERTEXATTRIB4NIVARBPROC)IntGetProcAddress("glVertexAttrib4NivARB");
	if(!_funcptr_glVertexAttrib4NivARB) ++numFailed;
	_funcptr_glVertexAttrib4NsvARB = (PFNGLVERTEXATTRIB4NSVARBPROC)IntGetProcAddress("glVertexAttrib4NsvARB");
	if(!_funcptr_glVertexAttrib4NsvARB) ++numFailed;
	_funcptr_glVertexAttrib4NubARB = (PFNGLVERTEXATTRIB4NUBARBPROC)IntGetProcAddress("glVertexAttrib4NubARB");
	if(!_funcptr_glVertexAttrib4NubARB) ++numFailed;
	_funcptr_glVertexAttrib4NubvARB = (PFNGLVERTEXATTRIB4NUBVARBPROC)IntGetProcAddress("glVertexAttrib4NubvARB");
	if(!_funcptr_glVertexAttrib4NubvARB) ++numFailed;
	_funcptr_glVertexAttrib4NuivARB = (PFNGLVERTEXATTRIB4NUIVARBPROC)IntGetProcAddress("glVertexAttrib4NuivARB");
	if(!_funcptr_glVertexAttrib4NuivARB) ++numFailed;
	_funcptr_glVertexAttrib4NusvARB = (PFNGLVERTEXATTRIB4NUSVARBPROC)IntGetProcAddress("glVertexAttrib4NusvARB");
	if(!_funcptr_glVertexAttrib4NusvARB) ++numFailed;
	_funcptr_glVertexAttrib4bvARB = (PFNGLVERTEXATTRIB4BVARBPROC)IntGetProcAddress("glVertexAttrib4bvARB");
	if(!_funcptr_glVertexAttrib4bvARB) ++numFailed;
	_funcptr_glVertexAttrib4dARB = (PFNGLVERTEXATTRIB4DARBPROC)IntGetProcAddress("glVertexAttrib4dARB");
	if(!_funcptr_glVertexAttrib4dARB) ++numFailed;
	_funcptr_glVertexAttrib4dvARB = (PFNGLVERTEXATTRIB4DVARBPROC)IntGetProcAddress("glVertexAttrib4dvARB");
	if(!_funcptr_glVertexAttrib4dvARB) ++numFailed;
	_funcptr_glVertexAttrib4fARB = (PFNGLVERTEXATTRIB4FARBPROC)IntGetProcAddress("glVertexAttrib4fARB");
	if(!_funcptr_glVertexAttrib4fARB) ++numFailed;
	_funcptr_glVertexAttrib4fvARB = (PFNGLVERTEXATTRIB4FVARBPROC)IntGetProcAddress("glVertexAttrib4fvARB");
	if(!_funcptr_glVertexAttrib4fvARB) ++numFailed;
	_funcptr_glVertexAttrib4ivARB = (PFNGLVERTEXATTRIB4IVARBPROC)IntGetProcAddress("glVertexAttrib4ivARB");
	if(!_funcptr_glVertexAttrib4ivARB) ++numFailed;
	_funcptr_glVertexAttrib4sARB = (PFNGLVERTEXATTRIB4SARBPROC)IntGetProcAddress("glVertexAttrib4sARB");
	if(!_funcptr_glVertexAttrib4sARB) ++numFailed;
	_funcptr_glVertexAttrib4svARB = (PFNGLVERTEXATTRIB4SVARBPROC)IntGetProcAddress("glVertexAttrib4svARB");
	if(!_funcptr_glVertexAttrib4svARB) ++numFailed;
	_funcptr_glVertexAttrib4ubvARB = (PFNGLVERTEXATTRIB4UBVARBPROC)IntGetProcAddress("glVertexAttrib4ubvARB");
	if(!_funcptr_glVertexAttrib4ubvARB) ++numFailed;
	_funcptr_glVertexAttrib4uivARB = (PFNGLVERTEXATTRIB4UIVARBPROC)IntGetProcAddress("glVertexAttrib4uivARB");
	if(!_funcptr_glVertexAttrib4uivARB) ++numFailed;
	_funcptr_glVertexAttrib4usvARB = (PFNGLVERTEXATTRIB4USVARBPROC)IntGetProcAddress("glVertexAttrib4usvARB");
	if(!_funcptr_glVertexAttrib4usvARB) ++numFailed;
	_funcptr_glVertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERARBPROC)IntGetProcAddress("glVertexAttribPointerARB");
	if(!_funcptr_glVertexAttribPointerARB) ++numFailed;
	_funcptr_glEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)IntGetProcAddress("glEnableVertexAttribArrayARB");
	if(!_funcptr_glEnableVertexAttribArrayARB) ++numFailed;
	_funcptr_glDisableVertexAttribArrayARB = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)IntGetProcAddress("glDisableVertexAttribArrayARB");
	if(!_funcptr_glDisableVertexAttribArrayARB) ++numFailed;
	_funcptr_glProgramStringARB = (PFNGLPROGRAMSTRINGARBPROC)IntGetProcAddress("glProgramStringARB");
	if(!_funcptr_glProgramStringARB) ++numFailed;
	_funcptr_glBindProgramARB = (PFNGLBINDPROGRAMARBPROC)IntGetProcAddress("glBindProgramARB");
	if(!_funcptr_glBindProgramARB) ++numFailed;
	_funcptr_glDeleteProgramsARB = (PFNGLDELETEPROGRAMSARBPROC)IntGetProcAddress("glDeleteProgramsARB");
	if(!_funcptr_glDeleteProgramsARB) ++numFailed;
	_funcptr_glGenProgramsARB = (PFNGLGENPROGRAMSARBPROC)IntGetProcAddress("glGenProgramsARB");
	if(!_funcptr_glGenProgramsARB) ++numFailed;
	_funcptr_glProgramEnvParameter4dARB = (PFNGLPROGRAMENVPARAMETER4DARBPROC)IntGetProcAddress("glProgramEnvParameter4dARB");
	if(!_funcptr_glProgramEnvParameter4dARB) ++numFailed;
	_funcptr_glProgramEnvParameter4dvARB = (PFNGLPROGRAMENVPARAMETER4DVARBPROC)IntGetProcAddress("glProgramEnvParameter4dvARB");
	if(!_funcptr_glProgramEnvParameter4dvARB) ++numFailed;
	_funcptr_glProgramEnvParameter4fARB = (PFNGLPROGRAMENVPARAMETER4FARBPROC)IntGetProcAddress("glProgramEnvParameter4fARB");
	if(!_funcptr_glProgramEnvParameter4fARB) ++numFailed;
	_funcptr_glProgramEnvParameter4fvARB = (PFNGLPROGRAMENVPARAMETER4FVARBPROC)IntGetProcAddress("glProgramEnvParameter4fvARB");
	if(!_funcptr_glProgramEnvParameter4fvARB) ++numFailed;
	_funcptr_glProgramLocalParameter4dARB = (PFNGLPROGRAMLOCALPARAMETER4DARBPROC)IntGetProcAddress("glProgramLocalParameter4dARB");
	if(!_funcptr_glProgramLocalParameter4dARB) ++numFailed;
	_funcptr_glProgramLocalParameter4dvARB = (PFNGLPROGRAMLOCALPARAMETER4DVARBPROC)IntGetProcAddress("glProgramLocalParameter4dvARB");
	if(!_funcptr_glProgramLocalParameter4dvARB) ++numFailed;
	_funcptr_glProgramLocalParameter4fARB = (PFNGLPROGRAMLOCALPARAMETER4FARBPROC)IntGetProcAddress("glProgramLocalParameter4fARB");
	if(!_funcptr_glProgramLocalParameter4fARB) ++numFailed;
	_funcptr_glProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)IntGetProcAddress("glProgramLocalParameter4fvARB");
	if(!_funcptr_glProgramLocalParameter4fvARB) ++numFailed;
	_funcptr_glGetProgramEnvParameterdvARB = (PFNGLGETPROGRAMENVPARAMETERDVARBPROC)IntGetProcAddress("glGetProgramEnvParameterdvARB");
	if(!_funcptr_glGetProgramEnvParameterdvARB) ++numFailed;
	_funcptr_glGetProgramEnvParameterfvARB = (PFNGLGETPROGRAMENVPARAMETERFVARBPROC)IntGetProcAddress("glGetProgramEnvParameterfvARB");
	if(!_funcptr_glGetProgramEnvParameterfvARB) ++numFailed;
	_funcptr_glGetProgramLocalParameterdvARB = (PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC)IntGetProcAddress("glGetProgramLocalParameterdvARB");
	if(!_funcptr_glGetProgramLocalParameterdvARB) ++numFailed;
	_funcptr_glGetProgramLocalParameterfvARB = (PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC)IntGetProcAddress("glGetProgramLocalParameterfvARB");
	if(!_funcptr_glGetProgramLocalParameterfvARB) ++numFailed;
	_funcptr_glGetProgramivARB = (PFNGLGETPROGRAMIVARBPROC)IntGetProcAddress("glGetProgramivARB");
	if(!_funcptr_glGetProgramivARB) ++numFailed;
	_funcptr_glGetProgramStringARB = (PFNGLGETPROGRAMSTRINGARBPROC)IntGetProcAddress("glGetProgramStringARB");
	if(!_funcptr_glGetProgramStringARB) ++numFailed;
	_funcptr_glGetVertexAttribdvARB = (PFNGLGETVERTEXATTRIBDVARBPROC)IntGetProcAddress("glGetVertexAttribdvARB");
	if(!_funcptr_glGetVertexAttribdvARB) ++numFailed;
	_funcptr_glGetVertexAttribfvARB = (PFNGLGETVERTEXATTRIBFVARBPROC)IntGetProcAddress("glGetVertexAttribfvARB");
	if(!_funcptr_glGetVertexAttribfvARB) ++numFailed;
	_funcptr_glGetVertexAttribivARB = (PFNGLGETVERTEXATTRIBIVARBPROC)IntGetProcAddress("glGetVertexAttribivARB");
	if(!_funcptr_glGetVertexAttribivARB) ++numFailed;
	_funcptr_glGetVertexAttribPointervARB = (PFNGLGETVERTEXATTRIBPOINTERVARBPROC)IntGetProcAddress("glGetVertexAttribPointervARB");
	if(!_funcptr_glGetVertexAttribPointervARB) ++numFailed;
	_funcptr_glIsProgramARB = (PFNGLISPROGRAMARBPROC)IntGetProcAddress("glIsProgramARB");
	if(!_funcptr_glIsProgramARB) ++numFailed;
	return numFailed;
}

PFNGLBINDATTRIBLOCATIONARBPROC _funcptr_glBindAttribLocationARB = NULL;
PFNGLGETACTIVEATTRIBARBPROC _funcptr_glGetActiveAttribARB = NULL;
PFNGLGETATTRIBLOCATIONARBPROC _funcptr_glGetAttribLocationARB = NULL;

static int LoadExt_ARB_vertex_shader()
{
	int numFailed = 0;
	_funcptr_glBindAttribLocationARB = (PFNGLBINDATTRIBLOCATIONARBPROC)IntGetProcAddress("glBindAttribLocationARB");
	if(!_funcptr_glBindAttribLocationARB) ++numFailed;
	_funcptr_glGetActiveAttribARB = (PFNGLGETACTIVEATTRIBARBPROC)IntGetProcAddress("glGetActiveAttribARB");
	if(!_funcptr_glGetActiveAttribARB) ++numFailed;
	_funcptr_glGetAttribLocationARB = (PFNGLGETATTRIBLOCATIONARBPROC)IntGetProcAddress("glGetAttribLocationARB");
	if(!_funcptr_glGetAttribLocationARB) ++numFailed;
	return numFailed;
}

PFNGLWINDOWPOS2DARBPROC _funcptr_glWindowPos2dARB = NULL;
PFNGLWINDOWPOS2DVARBPROC _funcptr_glWindowPos2dvARB = NULL;
PFNGLWINDOWPOS2FARBPROC _funcptr_glWindowPos2fARB = NULL;
PFNGLWINDOWPOS2FVARBPROC _funcptr_glWindowPos2fvARB = NULL;
PFNGLWINDOWPOS2IARBPROC _funcptr_glWindowPos2iARB = NULL;
PFNGLWINDOWPOS2IVARBPROC _funcptr_glWindowPos2ivARB = NULL;
PFNGLWINDOWPOS2SARBPROC _funcptr_glWindowPos2sARB = NULL;
PFNGLWINDOWPOS2SVARBPROC _funcptr_glWindowPos2svARB = NULL;
PFNGLWINDOWPOS3DARBPROC _funcptr_glWindowPos3dARB = NULL;
PFNGLWINDOWPOS3DVARBPROC _funcptr_glWindowPos3dvARB = NULL;
PFNGLWINDOWPOS3FARBPROC _funcptr_glWindowPos3fARB = NULL;
PFNGLWINDOWPOS3FVARBPROC _funcptr_glWindowPos3fvARB = NULL;
PFNGLWINDOWPOS3IARBPROC _funcptr_glWindowPos3iARB = NULL;
PFNGLWINDOWPOS3IVARBPROC _funcptr_glWindowPos3ivARB = NULL;
PFNGLWINDOWPOS3SARBPROC _funcptr_glWindowPos3sARB = NULL;
PFNGLWINDOWPOS3SVARBPROC _funcptr_glWindowPos3svARB = NULL;

static int LoadExt_ARB_window_pos()
{
	int numFailed = 0;
	_funcptr_glWindowPos2dARB = (PFNGLWINDOWPOS2DARBPROC)IntGetProcAddress("glWindowPos2dARB");
	if(!_funcptr_glWindowPos2dARB) ++numFailed;
	_funcptr_glWindowPos2dvARB = (PFNGLWINDOWPOS2DVARBPROC)IntGetProcAddress("glWindowPos2dvARB");
	if(!_funcptr_glWindowPos2dvARB) ++numFailed;
	_funcptr_glWindowPos2fARB = (PFNGLWINDOWPOS2FARBPROC)IntGetProcAddress("glWindowPos2fARB");
	if(!_funcptr_glWindowPos2fARB) ++numFailed;
	_funcptr_glWindowPos2fvARB = (PFNGLWINDOWPOS2FVARBPROC)IntGetProcAddress("glWindowPos2fvARB");
	if(!_funcptr_glWindowPos2fvARB) ++numFailed;
	_funcptr_glWindowPos2iARB = (PFNGLWINDOWPOS2IARBPROC)IntGetProcAddress("glWindowPos2iARB");
	if(!_funcptr_glWindowPos2iARB) ++numFailed;
	_funcptr_glWindowPos2ivARB = (PFNGLWINDOWPOS2IVARBPROC)IntGetProcAddress("glWindowPos2ivARB");
	if(!_funcptr_glWindowPos2ivARB) ++numFailed;
	_funcptr_glWindowPos2sARB = (PFNGLWINDOWPOS2SARBPROC)IntGetProcAddress("glWindowPos2sARB");
	if(!_funcptr_glWindowPos2sARB) ++numFailed;
	_funcptr_glWindowPos2svARB = (PFNGLWINDOWPOS2SVARBPROC)IntGetProcAddress("glWindowPos2svARB");
	if(!_funcptr_glWindowPos2svARB) ++numFailed;
	_funcptr_glWindowPos3dARB = (PFNGLWINDOWPOS3DARBPROC)IntGetProcAddress("glWindowPos3dARB");
	if(!_funcptr_glWindowPos3dARB) ++numFailed;
	_funcptr_glWindowPos3dvARB = (PFNGLWINDOWPOS3DVARBPROC)IntGetProcAddress("glWindowPos3dvARB");
	if(!_funcptr_glWindowPos3dvARB) ++numFailed;
	_funcptr_glWindowPos3fARB = (PFNGLWINDOWPOS3FARBPROC)IntGetProcAddress("glWindowPos3fARB");
	if(!_funcptr_glWindowPos3fARB) ++numFailed;
	_funcptr_glWindowPos3fvARB = (PFNGLWINDOWPOS3FVARBPROC)IntGetProcAddress("glWindowPos3fvARB");
	if(!_funcptr_glWindowPos3fvARB) ++numFailed;
	_funcptr_glWindowPos3iARB = (PFNGLWINDOWPOS3IARBPROC)IntGetProcAddress("glWindowPos3iARB");
	if(!_funcptr_glWindowPos3iARB) ++numFailed;
	_funcptr_glWindowPos3ivARB = (PFNGLWINDOWPOS3IVARBPROC)IntGetProcAddress("glWindowPos3ivARB");
	if(!_funcptr_glWindowPos3ivARB) ++numFailed;
	_funcptr_glWindowPos3sARB = (PFNGLWINDOWPOS3SARBPROC)IntGetProcAddress("glWindowPos3sARB");
	if(!_funcptr_glWindowPos3sARB) ++numFailed;
	_funcptr_glWindowPos3svARB = (PFNGLWINDOWPOS3SVARBPROC)IntGetProcAddress("glWindowPos3svARB");
	if(!_funcptr_glWindowPos3svARB) ++numFailed;
	return numFailed;
}

PFNGLDRAWBUFFERSATIPROC _funcptr_glDrawBuffersATI = NULL;

static int LoadExt_ATI_draw_buffers()
{
	int numFailed = 0;
	_funcptr_glDrawBuffersATI = (PFNGLDRAWBUFFERSATIPROC)IntGetProcAddress("glDrawBuffersATI");
	if(!_funcptr_glDrawBuffersATI) ++numFailed;
	return numFailed;
}

PFNGLELEMENTPOINTERATIPROC _funcptr_glElementPointerATI = NULL;
PFNGLDRAWELEMENTARRAYATIPROC _funcptr_glDrawElementArrayATI = NULL;
PFNGLDRAWRANGEELEMENTARRAYATIPROC _funcptr_glDrawRangeElementArrayATI = NULL;

static int LoadExt_ATI_element_array()
{
	int numFailed = 0;
	_funcptr_glElementPointerATI = (PFNGLELEMENTPOINTERATIPROC)IntGetProcAddress("glElementPointerATI");
	if(!_funcptr_glElementPointerATI) ++numFailed;
	_funcptr_glDrawElementArrayATI = (PFNGLDRAWELEMENTARRAYATIPROC)IntGetProcAddress("glDrawElementArrayATI");
	if(!_funcptr_glDrawElementArrayATI) ++numFailed;
	_funcptr_glDrawRangeElementArrayATI = (PFNGLDRAWRANGEELEMENTARRAYATIPROC)IntGetProcAddress("glDrawRangeElementArrayATI");
	if(!_funcptr_glDrawRangeElementArrayATI) ++numFailed;
	return numFailed;
}

PFNGLTEXBUMPPARAMETERIVATIPROC _funcptr_glTexBumpParameterivATI = NULL;
PFNGLTEXBUMPPARAMETERFVATIPROC _funcptr_glTexBumpParameterfvATI = NULL;
PFNGLGETTEXBUMPPARAMETERIVATIPROC _funcptr_glGetTexBumpParameterivATI = NULL;
PFNGLGETTEXBUMPPARAMETERFVATIPROC _funcptr_glGetTexBumpParameterfvATI = NULL;

static int LoadExt_ATI_envmap_bumpmap()
{
	int numFailed = 0;
	_funcptr_glTexBumpParameterivATI = (PFNGLTEXBUMPPARAMETERIVATIPROC)IntGetProcAddress("glTexBumpParameterivATI");
	if(!_funcptr_glTexBumpParameterivATI) ++numFailed;
	_funcptr_glTexBumpParameterfvATI = (PFNGLTEXBUMPPARAMETERFVATIPROC)IntGetProcAddress("glTexBumpParameterfvATI");
	if(!_funcptr_glTexBumpParameterfvATI) ++numFailed;
	_funcptr_glGetTexBumpParameterivATI = (PFNGLGETTEXBUMPPARAMETERIVATIPROC)IntGetProcAddress("glGetTexBumpParameterivATI");
	if(!_funcptr_glGetTexBumpParameterivATI) ++numFailed;
	_funcptr_glGetTexBumpParameterfvATI = (PFNGLGETTEXBUMPPARAMETERFVATIPROC)IntGetProcAddress("glGetTexBumpParameterfvATI");
	if(!_funcptr_glGetTexBumpParameterfvATI) ++numFailed;
	return numFailed;
}

PFNGLGENFRAGMENTSHADERSATIPROC _funcptr_glGenFragmentShadersATI = NULL;
PFNGLBINDFRAGMENTSHADERATIPROC _funcptr_glBindFragmentShaderATI = NULL;
PFNGLDELETEFRAGMENTSHADERATIPROC _funcptr_glDeleteFragmentShaderATI = NULL;
PFNGLBEGINFRAGMENTSHADERATIPROC _funcptr_glBeginFragmentShaderATI = NULL;
PFNGLENDFRAGMENTSHADERATIPROC _funcptr_glEndFragmentShaderATI = NULL;
PFNGLPASSTEXCOORDATIPROC _funcptr_glPassTexCoordATI = NULL;
PFNGLSAMPLEMAPATIPROC _funcptr_glSampleMapATI = NULL;
PFNGLCOLORFRAGMENTOP1ATIPROC _funcptr_glColorFragmentOp1ATI = NULL;
PFNGLCOLORFRAGMENTOP2ATIPROC _funcptr_glColorFragmentOp2ATI = NULL;
PFNGLCOLORFRAGMENTOP3ATIPROC _funcptr_glColorFragmentOp3ATI = NULL;
PFNGLALPHAFRAGMENTOP1ATIPROC _funcptr_glAlphaFragmentOp1ATI = NULL;
PFNGLALPHAFRAGMENTOP2ATIPROC _funcptr_glAlphaFragmentOp2ATI = NULL;
PFNGLALPHAFRAGMENTOP3ATIPROC _funcptr_glAlphaFragmentOp3ATI = NULL;
PFNGLSETFRAGMENTSHADERCONSTANTATIPROC _funcptr_glSetFragmentShaderConstantATI = NULL;

static int LoadExt_ATI_fragment_shader()
{
	int numFailed = 0;
	_funcptr_glGenFragmentShadersATI = (PFNGLGENFRAGMENTSHADERSATIPROC)IntGetProcAddress("glGenFragmentShadersATI");
	if(!_funcptr_glGenFragmentShadersATI) ++numFailed;
	_funcptr_glBindFragmentShaderATI = (PFNGLBINDFRAGMENTSHADERATIPROC)IntGetProcAddress("glBindFragmentShaderATI");
	if(!_funcptr_glBindFragmentShaderATI) ++numFailed;
	_funcptr_glDeleteFragmentShaderATI = (PFNGLDELETEFRAGMENTSHADERATIPROC)IntGetProcAddress("glDeleteFragmentShaderATI");
	if(!_funcptr_glDeleteFragmentShaderATI) ++numFailed;
	_funcptr_glBeginFragmentShaderATI = (PFNGLBEGINFRAGMENTSHADERATIPROC)IntGetProcAddress("glBeginFragmentShaderATI");
	if(!_funcptr_glBeginFragmentShaderATI) ++numFailed;
	_funcptr_glEndFragmentShaderATI = (PFNGLENDFRAGMENTSHADERATIPROC)IntGetProcAddress("glEndFragmentShaderATI");
	if(!_funcptr_glEndFragmentShaderATI) ++numFailed;
	_funcptr_glPassTexCoordATI = (PFNGLPASSTEXCOORDATIPROC)IntGetProcAddress("glPassTexCoordATI");
	if(!_funcptr_glPassTexCoordATI) ++numFailed;
	_funcptr_glSampleMapATI = (PFNGLSAMPLEMAPATIPROC)IntGetProcAddress("glSampleMapATI");
	if(!_funcptr_glSampleMapATI) ++numFailed;
	_funcptr_glColorFragmentOp1ATI = (PFNGLCOLORFRAGMENTOP1ATIPROC)IntGetProcAddress("glColorFragmentOp1ATI");
	if(!_funcptr_glColorFragmentOp1ATI) ++numFailed;
	_funcptr_glColorFragmentOp2ATI = (PFNGLCOLORFRAGMENTOP2ATIPROC)IntGetProcAddress("glColorFragmentOp2ATI");
	if(!_funcptr_glColorFragmentOp2ATI) ++numFailed;
	_funcptr_glColorFragmentOp3ATI = (PFNGLCOLORFRAGMENTOP3ATIPROC)IntGetProcAddress("glColorFragmentOp3ATI");
	if(!_funcptr_glColorFragmentOp3ATI) ++numFailed;
	_funcptr_glAlphaFragmentOp1ATI = (PFNGLALPHAFRAGMENTOP1ATIPROC)IntGetProcAddress("glAlphaFragmentOp1ATI");
	if(!_funcptr_glAlphaFragmentOp1ATI) ++numFailed;
	_funcptr_glAlphaFragmentOp2ATI = (PFNGLALPHAFRAGMENTOP2ATIPROC)IntGetProcAddress("glAlphaFragmentOp2ATI");
	if(!_funcptr_glAlphaFragmentOp2ATI) ++numFailed;
	_funcptr_glAlphaFragmentOp3ATI = (PFNGLALPHAFRAGMENTOP3ATIPROC)IntGetProcAddress("glAlphaFragmentOp3ATI");
	if(!_funcptr_glAlphaFragmentOp3ATI) ++numFailed;
	_funcptr_glSetFragmentShaderConstantATI = (PFNGLSETFRAGMENTSHADERCONSTANTATIPROC)IntGetProcAddress("glSetFragmentShaderConstantATI");
	if(!_funcptr_glSetFragmentShaderConstantATI) ++numFailed;
	return numFailed;
}

PFNGLMAPOBJECTBUFFERATIPROC _funcptr_glMapObjectBufferATI = NULL;
PFNGLUNMAPOBJECTBUFFERATIPROC _funcptr_glUnmapObjectBufferATI = NULL;

static int LoadExt_ATI_map_object_buffer()
{
	int numFailed = 0;
	_funcptr_glMapObjectBufferATI = (PFNGLMAPOBJECTBUFFERATIPROC)IntGetProcAddress("glMapObjectBufferATI");
	if(!_funcptr_glMapObjectBufferATI) ++numFailed;
	_funcptr_glUnmapObjectBufferATI = (PFNGLUNMAPOBJECTBUFFERATIPROC)IntGetProcAddress("glUnmapObjectBufferATI");
	if(!_funcptr_glUnmapObjectBufferATI) ++numFailed;
	return numFailed;
}

PFNGLPNTRIANGLESIATIPROC _funcptr_glPNTrianglesiATI = NULL;
PFNGLPNTRIANGLESFATIPROC _funcptr_glPNTrianglesfATI = NULL;

static int LoadExt_ATI_pn_triangles()
{
	int numFailed = 0;
	_funcptr_glPNTrianglesiATI = (PFNGLPNTRIANGLESIATIPROC)IntGetProcAddress("glPNTrianglesiATI");
	if(!_funcptr_glPNTrianglesiATI) ++numFailed;
	_funcptr_glPNTrianglesfATI = (PFNGLPNTRIANGLESFATIPROC)IntGetProcAddress("glPNTrianglesfATI");
	if(!_funcptr_glPNTrianglesfATI) ++numFailed;
	return numFailed;
}

PFNGLSTENCILOPSEPARATEATIPROC _funcptr_glStencilOpSeparateATI = NULL;
PFNGLSTENCILFUNCSEPARATEATIPROC _funcptr_glStencilFuncSeparateATI = NULL;

static int LoadExt_ATI_separate_stencil()
{
	int numFailed = 0;
	_funcptr_glStencilOpSeparateATI = (PFNGLSTENCILOPSEPARATEATIPROC)IntGetProcAddress("glStencilOpSeparateATI");
	if(!_funcptr_glStencilOpSeparateATI) ++numFailed;
	_funcptr_glStencilFuncSeparateATI = (PFNGLSTENCILFUNCSEPARATEATIPROC)IntGetProcAddress("glStencilFuncSeparateATI");
	if(!_funcptr_glStencilFuncSeparateATI) ++numFailed;
	return numFailed;
}

PFNGLNEWOBJECTBUFFERATIPROC _funcptr_glNewObjectBufferATI = NULL;
PFNGLISOBJECTBUFFERATIPROC _funcptr_glIsObjectBufferATI = NULL;
PFNGLUPDATEOBJECTBUFFERATIPROC _funcptr_glUpdateObjectBufferATI = NULL;
PFNGLGETOBJECTBUFFERFVATIPROC _funcptr_glGetObjectBufferfvATI = NULL;
PFNGLGETOBJECTBUFFERIVATIPROC _funcptr_glGetObjectBufferivATI = NULL;
PFNGLFREEOBJECTBUFFERATIPROC _funcptr_glFreeObjectBufferATI = NULL;
PFNGLARRAYOBJECTATIPROC _funcptr_glArrayObjectATI = NULL;
PFNGLGETARRAYOBJECTFVATIPROC _funcptr_glGetArrayObjectfvATI = NULL;
PFNGLGETARRAYOBJECTIVATIPROC _funcptr_glGetArrayObjectivATI = NULL;
PFNGLVARIANTARRAYOBJECTATIPROC _funcptr_glVariantArrayObjectATI = NULL;
PFNGLGETVARIANTARRAYOBJECTFVATIPROC _funcptr_glGetVariantArrayObjectfvATI = NULL;
PFNGLGETVARIANTARRAYOBJECTIVATIPROC _funcptr_glGetVariantArrayObjectivATI = NULL;

static int LoadExt_ATI_vertex_array_object()
{
	int numFailed = 0;
	_funcptr_glNewObjectBufferATI = (PFNGLNEWOBJECTBUFFERATIPROC)IntGetProcAddress("glNewObjectBufferATI");
	if(!_funcptr_glNewObjectBufferATI) ++numFailed;
	_funcptr_glIsObjectBufferATI = (PFNGLISOBJECTBUFFERATIPROC)IntGetProcAddress("glIsObjectBufferATI");
	if(!_funcptr_glIsObjectBufferATI) ++numFailed;
	_funcptr_glUpdateObjectBufferATI = (PFNGLUPDATEOBJECTBUFFERATIPROC)IntGetProcAddress("glUpdateObjectBufferATI");
	if(!_funcptr_glUpdateObjectBufferATI) ++numFailed;
	_funcptr_glGetObjectBufferfvATI = (PFNGLGETOBJECTBUFFERFVATIPROC)IntGetProcAddress("glGetObjectBufferfvATI");
	if(!_funcptr_glGetObjectBufferfvATI) ++numFailed;
	_funcptr_glGetObjectBufferivATI = (PFNGLGETOBJECTBUFFERIVATIPROC)IntGetProcAddress("glGetObjectBufferivATI");
	if(!_funcptr_glGetObjectBufferivATI) ++numFailed;
	_funcptr_glFreeObjectBufferATI = (PFNGLFREEOBJECTBUFFERATIPROC)IntGetProcAddress("glFreeObjectBufferATI");
	if(!_funcptr_glFreeObjectBufferATI) ++numFailed;
	_funcptr_glArrayObjectATI = (PFNGLARRAYOBJECTATIPROC)IntGetProcAddress("glArrayObjectATI");
	if(!_funcptr_glArrayObjectATI) ++numFailed;
	_funcptr_glGetArrayObjectfvATI = (PFNGLGETARRAYOBJECTFVATIPROC)IntGetProcAddress("glGetArrayObjectfvATI");
	if(!_funcptr_glGetArrayObjectfvATI) ++numFailed;
	_funcptr_glGetArrayObjectivATI = (PFNGLGETARRAYOBJECTIVATIPROC)IntGetProcAddress("glGetArrayObjectivATI");
	if(!_funcptr_glGetArrayObjectivATI) ++numFailed;
	_funcptr_glVariantArrayObjectATI = (PFNGLVARIANTARRAYOBJECTATIPROC)IntGetProcAddress("glVariantArrayObjectATI");
	if(!_funcptr_glVariantArrayObjectATI) ++numFailed;
	_funcptr_glGetVariantArrayObjectfvATI = (PFNGLGETVARIANTARRAYOBJECTFVATIPROC)IntGetProcAddress("glGetVariantArrayObjectfvATI");
	if(!_funcptr_glGetVariantArrayObjectfvATI) ++numFailed;
	_funcptr_glGetVariantArrayObjectivATI = (PFNGLGETVARIANTARRAYOBJECTIVATIPROC)IntGetProcAddress("glGetVariantArrayObjectivATI");
	if(!_funcptr_glGetVariantArrayObjectivATI) ++numFailed;
	return numFailed;
}

PFNGLVERTEXATTRIBARRAYOBJECTATIPROC _funcptr_glVertexAttribArrayObjectATI = NULL;
PFNGLGETVERTEXATTRIBARRAYOBJECTFVATIPROC _funcptr_glGetVertexAttribArrayObjectfvATI = NULL;
PFNGLGETVERTEXATTRIBARRAYOBJECTIVATIPROC _funcptr_glGetVertexAttribArrayObjectivATI = NULL;

static int LoadExt_ATI_vertex_attrib_array_object()
{
	int numFailed = 0;
	_funcptr_glVertexAttribArrayObjectATI = (PFNGLVERTEXATTRIBARRAYOBJECTATIPROC)IntGetProcAddress("glVertexAttribArrayObjectATI");
	if(!_funcptr_glVertexAttribArrayObjectATI) ++numFailed;
	_funcptr_glGetVertexAttribArrayObjectfvATI = (PFNGLGETVERTEXATTRIBARRAYOBJECTFVATIPROC)IntGetProcAddress("glGetVertexAttribArrayObjectfvATI");
	if(!_funcptr_glGetVertexAttribArrayObjectfvATI) ++numFailed;
	_funcptr_glGetVertexAttribArrayObjectivATI = (PFNGLGETVERTEXATTRIBARRAYOBJECTIVATIPROC)IntGetProcAddress("glGetVertexAttribArrayObjectivATI");
	if(!_funcptr_glGetVertexAttribArrayObjectivATI) ++numFailed;
	return numFailed;
}

PFNGLVERTEXSTREAM1SATIPROC _funcptr_glVertexStream1sATI = NULL;
PFNGLVERTEXSTREAM1SVATIPROC _funcptr_glVertexStream1svATI = NULL;
PFNGLVERTEXSTREAM1IATIPROC _funcptr_glVertexStream1iATI = NULL;
PFNGLVERTEXSTREAM1IVATIPROC _funcptr_glVertexStream1ivATI = NULL;
PFNGLVERTEXSTREAM1FATIPROC _funcptr_glVertexStream1fATI = NULL;
PFNGLVERTEXSTREAM1FVATIPROC _funcptr_glVertexStream1fvATI = NULL;
PFNGLVERTEXSTREAM1DATIPROC _funcptr_glVertexStream1dATI = NULL;
PFNGLVERTEXSTREAM1DVATIPROC _funcptr_glVertexStream1dvATI = NULL;
PFNGLVERTEXSTREAM2SATIPROC _funcptr_glVertexStream2sATI = NULL;
PFNGLVERTEXSTREAM2SVATIPROC _funcptr_glVertexStream2svATI = NULL;
PFNGLVERTEXSTREAM2IATIPROC _funcptr_glVertexStream2iATI = NULL;
PFNGLVERTEXSTREAM2IVATIPROC _funcptr_glVertexStream2ivATI = NULL;
PFNGLVERTEXSTREAM2FATIPROC _funcptr_glVertexStream2fATI = NULL;
PFNGLVERTEXSTREAM2FVATIPROC _funcptr_glVertexStream2fvATI = NULL;
PFNGLVERTEXSTREAM2DATIPROC _funcptr_glVertexStream2dATI = NULL;
PFNGLVERTEXSTREAM2DVATIPROC _funcptr_glVertexStream2dvATI = NULL;
PFNGLVERTEXSTREAM3SATIPROC _funcptr_glVertexStream3sATI = NULL;
PFNGLVERTEXSTREAM3SVATIPROC _funcptr_glVertexStream3svATI = NULL;
PFNGLVERTEXSTREAM3IATIPROC _funcptr_glVertexStream3iATI = NULL;
PFNGLVERTEXSTREAM3IVATIPROC _funcptr_glVertexStream3ivATI = NULL;
PFNGLVERTEXSTREAM3FATIPROC _funcptr_glVertexStream3fATI = NULL;
PFNGLVERTEXSTREAM3FVATIPROC _funcptr_glVertexStream3fvATI = NULL;
PFNGLVERTEXSTREAM3DATIPROC _funcptr_glVertexStream3dATI = NULL;
PFNGLVERTEXSTREAM3DVATIPROC _funcptr_glVertexStream3dvATI = NULL;
PFNGLVERTEXSTREAM4SATIPROC _funcptr_glVertexStream4sATI = NULL;
PFNGLVERTEXSTREAM4SVATIPROC _funcptr_glVertexStream4svATI = NULL;
PFNGLVERTEXSTREAM4IATIPROC _funcptr_glVertexStream4iATI = NULL;
PFNGLVERTEXSTREAM4IVATIPROC _funcptr_glVertexStream4ivATI = NULL;
PFNGLVERTEXSTREAM4FATIPROC _funcptr_glVertexStream4fATI = NULL;
PFNGLVERTEXSTREAM4FVATIPROC _funcptr_glVertexStream4fvATI = NULL;
PFNGLVERTEXSTREAM4DATIPROC _funcptr_glVertexStream4dATI = NULL;
PFNGLVERTEXSTREAM4DVATIPROC _funcptr_glVertexStream4dvATI = NULL;
PFNGLNORMALSTREAM3BATIPROC _funcptr_glNormalStream3bATI = NULL;
PFNGLNORMALSTREAM3BVATIPROC _funcptr_glNormalStream3bvATI = NULL;
PFNGLNORMALSTREAM3SATIPROC _funcptr_glNormalStream3sATI = NULL;
PFNGLNORMALSTREAM3SVATIPROC _funcptr_glNormalStream3svATI = NULL;
PFNGLNORMALSTREAM3IATIPROC _funcptr_glNormalStream3iATI = NULL;
PFNGLNORMALSTREAM3IVATIPROC _funcptr_glNormalStream3ivATI = NULL;
PFNGLNORMALSTREAM3FATIPROC _funcptr_glNormalStream3fATI = NULL;
PFNGLNORMALSTREAM3FVATIPROC _funcptr_glNormalStream3fvATI = NULL;
PFNGLNORMALSTREAM3DATIPROC _funcptr_glNormalStream3dATI = NULL;
PFNGLNORMALSTREAM3DVATIPROC _funcptr_glNormalStream3dvATI = NULL;
PFNGLCLIENTACTIVEVERTEXSTREAMATIPROC _funcptr_glClientActiveVertexStreamATI = NULL;
PFNGLVERTEXBLENDENVIATIPROC _funcptr_glVertexBlendEnviATI = NULL;
PFNGLVERTEXBLENDENVFATIPROC _funcptr_glVertexBlendEnvfATI = NULL;

static int LoadExt_ATI_vertex_streams()
{
	int numFailed = 0;
	_funcptr_glVertexStream1sATI = (PFNGLVERTEXSTREAM1SATIPROC)IntGetProcAddress("glVertexStream1sATI");
	if(!_funcptr_glVertexStream1sATI) ++numFailed;
	_funcptr_glVertexStream1svATI = (PFNGLVERTEXSTREAM1SVATIPROC)IntGetProcAddress("glVertexStream1svATI");
	if(!_funcptr_glVertexStream1svATI) ++numFailed;
	_funcptr_glVertexStream1iATI = (PFNGLVERTEXSTREAM1IATIPROC)IntGetProcAddress("glVertexStream1iATI");
	if(!_funcptr_glVertexStream1iATI) ++numFailed;
	_funcptr_glVertexStream1ivATI = (PFNGLVERTEXSTREAM1IVATIPROC)IntGetProcAddress("glVertexStream1ivATI");
	if(!_funcptr_glVertexStream1ivATI) ++numFailed;
	_funcptr_glVertexStream1fATI = (PFNGLVERTEXSTREAM1FATIPROC)IntGetProcAddress("glVertexStream1fATI");
	if(!_funcptr_glVertexStream1fATI) ++numFailed;
	_funcptr_glVertexStream1fvATI = (PFNGLVERTEXSTREAM1FVATIPROC)IntGetProcAddress("glVertexStream1fvATI");
	if(!_funcptr_glVertexStream1fvATI) ++numFailed;
	_funcptr_glVertexStream1dATI = (PFNGLVERTEXSTREAM1DATIPROC)IntGetProcAddress("glVertexStream1dATI");
	if(!_funcptr_glVertexStream1dATI) ++numFailed;
	_funcptr_glVertexStream1dvATI = (PFNGLVERTEXSTREAM1DVATIPROC)IntGetProcAddress("glVertexStream1dvATI");
	if(!_funcptr_glVertexStream1dvATI) ++numFailed;
	_funcptr_glVertexStream2sATI = (PFNGLVERTEXSTREAM2SATIPROC)IntGetProcAddress("glVertexStream2sATI");
	if(!_funcptr_glVertexStream2sATI) ++numFailed;
	_funcptr_glVertexStream2svATI = (PFNGLVERTEXSTREAM2SVATIPROC)IntGetProcAddress("glVertexStream2svATI");
	if(!_funcptr_glVertexStream2svATI) ++numFailed;
	_funcptr_glVertexStream2iATI = (PFNGLVERTEXSTREAM2IATIPROC)IntGetProcAddress("glVertexStream2iATI");
	if(!_funcptr_glVertexStream2iATI) ++numFailed;
	_funcptr_glVertexStream2ivATI = (PFNGLVERTEXSTREAM2IVATIPROC)IntGetProcAddress("glVertexStream2ivATI");
	if(!_funcptr_glVertexStream2ivATI) ++numFailed;
	_funcptr_glVertexStream2fATI = (PFNGLVERTEXSTREAM2FATIPROC)IntGetProcAddress("glVertexStream2fATI");
	if(!_funcptr_glVertexStream2fATI) ++numFailed;
	_funcptr_glVertexStream2fvATI = (PFNGLVERTEXSTREAM2FVATIPROC)IntGetProcAddress("glVertexStream2fvATI");
	if(!_funcptr_glVertexStream2fvATI) ++numFailed;
	_funcptr_glVertexStream2dATI = (PFNGLVERTEXSTREAM2DATIPROC)IntGetProcAddress("glVertexStream2dATI");
	if(!_funcptr_glVertexStream2dATI) ++numFailed;
	_funcptr_glVertexStream2dvATI = (PFNGLVERTEXSTREAM2DVATIPROC)IntGetProcAddress("glVertexStream2dvATI");
	if(!_funcptr_glVertexStream2dvATI) ++numFailed;
	_funcptr_glVertexStream3sATI = (PFNGLVERTEXSTREAM3SATIPROC)IntGetProcAddress("glVertexStream3sATI");
	if(!_funcptr_glVertexStream3sATI) ++numFailed;
	_funcptr_glVertexStream3svATI = (PFNGLVERTEXSTREAM3SVATIPROC)IntGetProcAddress("glVertexStream3svATI");
	if(!_funcptr_glVertexStream3svATI) ++numFailed;
	_funcptr_glVertexStream3iATI = (PFNGLVERTEXSTREAM3IATIPROC)IntGetProcAddress("glVertexStream3iATI");
	if(!_funcptr_glVertexStream3iATI) ++numFailed;
	_funcptr_glVertexStream3ivATI = (PFNGLVERTEXSTREAM3IVATIPROC)IntGetProcAddress("glVertexStream3ivATI");
	if(!_funcptr_glVertexStream3ivATI) ++numFailed;
	_funcptr_glVertexStream3fATI = (PFNGLVERTEXSTREAM3FATIPROC)IntGetProcAddress("glVertexStream3fATI");
	if(!_funcptr_glVertexStream3fATI) ++numFailed;
	_funcptr_glVertexStream3fvATI = (PFNGLVERTEXSTREAM3FVATIPROC)IntGetProcAddress("glVertexStream3fvATI");
	if(!_funcptr_glVertexStream3fvATI) ++numFailed;
	_funcptr_glVertexStream3dATI = (PFNGLVERTEXSTREAM3DATIPROC)IntGetProcAddress("glVertexStream3dATI");
	if(!_funcptr_glVertexStream3dATI) ++numFailed;
	_funcptr_glVertexStream3dvATI = (PFNGLVERTEXSTREAM3DVATIPROC)IntGetProcAddress("glVertexStream3dvATI");
	if(!_funcptr_glVertexStream3dvATI) ++numFailed;
	_funcptr_glVertexStream4sATI = (PFNGLVERTEXSTREAM4SATIPROC)IntGetProcAddress("glVertexStream4sATI");
	if(!_funcptr_glVertexStream4sATI) ++numFailed;
	_funcptr_glVertexStream4svATI = (PFNGLVERTEXSTREAM4SVATIPROC)IntGetProcAddress("glVertexStream4svATI");
	if(!_funcptr_glVertexStream4svATI) ++numFailed;
	_funcptr_glVertexStream4iATI = (PFNGLVERTEXSTREAM4IATIPROC)IntGetProcAddress("glVertexStream4iATI");
	if(!_funcptr_glVertexStream4iATI) ++numFailed;
	_funcptr_glVertexStream4ivATI = (PFNGLVERTEXSTREAM4IVATIPROC)IntGetProcAddress("glVertexStream4ivATI");
	if(!_funcptr_glVertexStream4ivATI) ++numFailed;
	_funcptr_glVertexStream4fATI = (PFNGLVERTEXSTREAM4FATIPROC)IntGetProcAddress("glVertexStream4fATI");
	if(!_funcptr_glVertexStream4fATI) ++numFailed;
	_funcptr_glVertexStream4fvATI = (PFNGLVERTEXSTREAM4FVATIPROC)IntGetProcAddress("glVertexStream4fvATI");
	if(!_funcptr_glVertexStream4fvATI) ++numFailed;
	_funcptr_glVertexStream4dATI = (PFNGLVERTEXSTREAM4DATIPROC)IntGetProcAddress("glVertexStream4dATI");
	if(!_funcptr_glVertexStream4dATI) ++numFailed;
	_funcptr_glVertexStream4dvATI = (PFNGLVERTEXSTREAM4DVATIPROC)IntGetProcAddress("glVertexStream4dvATI");
	if(!_funcptr_glVertexStream4dvATI) ++numFailed;
	_funcptr_glNormalStream3bATI = (PFNGLNORMALSTREAM3BATIPROC)IntGetProcAddress("glNormalStream3bATI");
	if(!_funcptr_glNormalStream3bATI) ++numFailed;
	_funcptr_glNormalStream3bvATI = (PFNGLNORMALSTREAM3BVATIPROC)IntGetProcAddress("glNormalStream3bvATI");
	if(!_funcptr_glNormalStream3bvATI) ++numFailed;
	_funcptr_glNormalStream3sATI = (PFNGLNORMALSTREAM3SATIPROC)IntGetProcAddress("glNormalStream3sATI");
	if(!_funcptr_glNormalStream3sATI) ++numFailed;
	_funcptr_glNormalStream3svATI = (PFNGLNORMALSTREAM3SVATIPROC)IntGetProcAddress("glNormalStream3svATI");
	if(!_funcptr_glNormalStream3svATI) ++numFailed;
	_funcptr_glNormalStream3iATI = (PFNGLNORMALSTREAM3IATIPROC)IntGetProcAddress("glNormalStream3iATI");
	if(!_funcptr_glNormalStream3iATI) ++numFailed;
	_funcptr_glNormalStream3ivATI = (PFNGLNORMALSTREAM3IVATIPROC)IntGetProcAddress("glNormalStream3ivATI");
	if(!_funcptr_glNormalStream3ivATI) ++numFailed;
	_funcptr_glNormalStream3fATI = (PFNGLNORMALSTREAM3FATIPROC)IntGetProcAddress("glNormalStream3fATI");
	if(!_funcptr_glNormalStream3fATI) ++numFailed;
	_funcptr_glNormalStream3fvATI = (PFNGLNORMALSTREAM3FVATIPROC)IntGetProcAddress("glNormalStream3fvATI");
	if(!_funcptr_glNormalStream3fvATI) ++numFailed;
	_funcptr_glNormalStream3dATI = (PFNGLNORMALSTREAM3DATIPROC)IntGetProcAddress("glNormalStream3dATI");
	if(!_funcptr_glNormalStream3dATI) ++numFailed;
	_funcptr_glNormalStream3dvATI = (PFNGLNORMALSTREAM3DVATIPROC)IntGetProcAddress("glNormalStream3dvATI");
	if(!_funcptr_glNormalStream3dvATI) ++numFailed;
	_funcptr_glClientActiveVertexStreamATI = (PFNGLCLIENTACTIVEVERTEXSTREAMATIPROC)IntGetProcAddress("glClientActiveVertexStreamATI");
	if(!_funcptr_glClientActiveVertexStreamATI) ++numFailed;
	_funcptr_glVertexBlendEnviATI = (PFNGLVERTEXBLENDENVIATIPROC)IntGetProcAddress("glVertexBlendEnviATI");
	if(!_funcptr_glVertexBlendEnviATI) ++numFailed;
	_funcptr_glVertexBlendEnvfATI = (PFNGLVERTEXBLENDENVFATIPROC)IntGetProcAddress("glVertexBlendEnvfATI");
	if(!_funcptr_glVertexBlendEnvfATI) ++numFailed;
	return numFailed;
}

PFNGLUNIFORMBUFFEREXTPROC _funcptr_glUniformBufferEXT = NULL;
PFNGLGETUNIFORMBUFFERSIZEEXTPROC _funcptr_glGetUniformBufferSizeEXT = NULL;
PFNGLGETUNIFORMOFFSETEXTPROC _funcptr_glGetUniformOffsetEXT = NULL;

static int LoadExt_EXT_bindable_uniform()
{
	int numFailed = 0;
	_funcptr_glUniformBufferEXT = (PFNGLUNIFORMBUFFEREXTPROC)IntGetProcAddress("glUniformBufferEXT");
	if(!_funcptr_glUniformBufferEXT) ++numFailed;
	_funcptr_glGetUniformBufferSizeEXT = (PFNGLGETUNIFORMBUFFERSIZEEXTPROC)IntGetProcAddress("glGetUniformBufferSizeEXT");
	if(!_funcptr_glGetUniformBufferSizeEXT) ++numFailed;
	_funcptr_glGetUniformOffsetEXT = (PFNGLGETUNIFORMOFFSETEXTPROC)IntGetProcAddress("glGetUniformOffsetEXT");
	if(!_funcptr_glGetUniformOffsetEXT) ++numFailed;
	return numFailed;
}

PFNGLBLENDCOLOREXTPROC _funcptr_glBlendColorEXT = NULL;

static int LoadExt_EXT_blend_color()
{
	int numFailed = 0;
	_funcptr_glBlendColorEXT = (PFNGLBLENDCOLOREXTPROC)IntGetProcAddress("glBlendColorEXT");
	if(!_funcptr_glBlendColorEXT) ++numFailed;
	return numFailed;
}

PFNGLBLENDEQUATIONSEPARATEEXTPROC _funcptr_glBlendEquationSeparateEXT = NULL;

static int LoadExt_EXT_blend_equation_separate()
{
	int numFailed = 0;
	_funcptr_glBlendEquationSeparateEXT = (PFNGLBLENDEQUATIONSEPARATEEXTPROC)IntGetProcAddress("glBlendEquationSeparateEXT");
	if(!_funcptr_glBlendEquationSeparateEXT) ++numFailed;
	return numFailed;
}

PFNGLBLENDFUNCSEPARATEEXTPROC _funcptr_glBlendFuncSeparateEXT = NULL;

static int LoadExt_EXT_blend_func_separate()
{
	int numFailed = 0;
	_funcptr_glBlendFuncSeparateEXT = (PFNGLBLENDFUNCSEPARATEEXTPROC)IntGetProcAddress("glBlendFuncSeparateEXT");
	if(!_funcptr_glBlendFuncSeparateEXT) ++numFailed;
	return numFailed;
}

PFNGLBLENDEQUATIONEXTPROC _funcptr_glBlendEquationEXT = NULL;

static int LoadExt_EXT_blend_minmax()
{
	int numFailed = 0;
	_funcptr_glBlendEquationEXT = (PFNGLBLENDEQUATIONEXTPROC)IntGetProcAddress("glBlendEquationEXT");
	if(!_funcptr_glBlendEquationEXT) ++numFailed;
	return numFailed;
}

PFNGLCOLORSUBTABLEEXTPROC _funcptr_glColorSubTableEXT = NULL;
PFNGLCOPYCOLORSUBTABLEEXTPROC _funcptr_glCopyColorSubTableEXT = NULL;

static int LoadExt_EXT_color_subtable()
{
	int numFailed = 0;
	_funcptr_glColorSubTableEXT = (PFNGLCOLORSUBTABLEEXTPROC)IntGetProcAddress("glColorSubTableEXT");
	if(!_funcptr_glColorSubTableEXT) ++numFailed;
	_funcptr_glCopyColorSubTableEXT = (PFNGLCOPYCOLORSUBTABLEEXTPROC)IntGetProcAddress("glCopyColorSubTableEXT");
	if(!_funcptr_glCopyColorSubTableEXT) ++numFailed;
	return numFailed;
}

PFNGLLOCKARRAYSEXTPROC _funcptr_glLockArraysEXT = NULL;
PFNGLUNLOCKARRAYSEXTPROC _funcptr_glUnlockArraysEXT = NULL;

static int LoadExt_EXT_compiled_vertex_array()
{
	int numFailed = 0;
	_funcptr_glLockArraysEXT = (PFNGLLOCKARRAYSEXTPROC)IntGetProcAddress("glLockArraysEXT");
	if(!_funcptr_glLockArraysEXT) ++numFailed;
	_funcptr_glUnlockArraysEXT = (PFNGLUNLOCKARRAYSEXTPROC)IntGetProcAddress("glUnlockArraysEXT");
	if(!_funcptr_glUnlockArraysEXT) ++numFailed;
	return numFailed;
}

PFNGLCONVOLUTIONFILTER1DEXTPROC _funcptr_glConvolutionFilter1DEXT = NULL;
PFNGLCONVOLUTIONFILTER2DEXTPROC _funcptr_glConvolutionFilter2DEXT = NULL;
PFNGLCONVOLUTIONPARAMETERFEXTPROC _funcptr_glConvolutionParameterfEXT = NULL;
PFNGLCONVOLUTIONPARAMETERFVEXTPROC _funcptr_glConvolutionParameterfvEXT = NULL;
PFNGLCONVOLUTIONPARAMETERIEXTPROC _funcptr_glConvolutionParameteriEXT = NULL;
PFNGLCONVOLUTIONPARAMETERIVEXTPROC _funcptr_glConvolutionParameterivEXT = NULL;
PFNGLCOPYCONVOLUTIONFILTER1DEXTPROC _funcptr_glCopyConvolutionFilter1DEXT = NULL;
PFNGLCOPYCONVOLUTIONFILTER2DEXTPROC _funcptr_glCopyConvolutionFilter2DEXT = NULL;
PFNGLGETCONVOLUTIONFILTEREXTPROC _funcptr_glGetConvolutionFilterEXT = NULL;
PFNGLGETCONVOLUTIONPARAMETERFVEXTPROC _funcptr_glGetConvolutionParameterfvEXT = NULL;
PFNGLGETCONVOLUTIONPARAMETERIVEXTPROC _funcptr_glGetConvolutionParameterivEXT = NULL;
PFNGLGETSEPARABLEFILTEREXTPROC _funcptr_glGetSeparableFilterEXT = NULL;
PFNGLSEPARABLEFILTER2DEXTPROC _funcptr_glSeparableFilter2DEXT = NULL;

static int LoadExt_EXT_convolution()
{
	int numFailed = 0;
	_funcptr_glConvolutionFilter1DEXT = (PFNGLCONVOLUTIONFILTER1DEXTPROC)IntGetProcAddress("glConvolutionFilter1DEXT");
	if(!_funcptr_glConvolutionFilter1DEXT) ++numFailed;
	_funcptr_glConvolutionFilter2DEXT = (PFNGLCONVOLUTIONFILTER2DEXTPROC)IntGetProcAddress("glConvolutionFilter2DEXT");
	if(!_funcptr_glConvolutionFilter2DEXT) ++numFailed;
	_funcptr_glConvolutionParameterfEXT = (PFNGLCONVOLUTIONPARAMETERFEXTPROC)IntGetProcAddress("glConvolutionParameterfEXT");
	if(!_funcptr_glConvolutionParameterfEXT) ++numFailed;
	_funcptr_glConvolutionParameterfvEXT = (PFNGLCONVOLUTIONPARAMETERFVEXTPROC)IntGetProcAddress("glConvolutionParameterfvEXT");
	if(!_funcptr_glConvolutionParameterfvEXT) ++numFailed;
	_funcptr_glConvolutionParameteriEXT = (PFNGLCONVOLUTIONPARAMETERIEXTPROC)IntGetProcAddress("glConvolutionParameteriEXT");
	if(!_funcptr_glConvolutionParameteriEXT) ++numFailed;
	_funcptr_glConvolutionParameterivEXT = (PFNGLCONVOLUTIONPARAMETERIVEXTPROC)IntGetProcAddress("glConvolutionParameterivEXT");
	if(!_funcptr_glConvolutionParameterivEXT) ++numFailed;
	_funcptr_glCopyConvolutionFilter1DEXT = (PFNGLCOPYCONVOLUTIONFILTER1DEXTPROC)IntGetProcAddress("glCopyConvolutionFilter1DEXT");
	if(!_funcptr_glCopyConvolutionFilter1DEXT) ++numFailed;
	_funcptr_glCopyConvolutionFilter2DEXT = (PFNGLCOPYCONVOLUTIONFILTER2DEXTPROC)IntGetProcAddress("glCopyConvolutionFilter2DEXT");
	if(!_funcptr_glCopyConvolutionFilter2DEXT) ++numFailed;
	_funcptr_glGetConvolutionFilterEXT = (PFNGLGETCONVOLUTIONFILTEREXTPROC)IntGetProcAddress("glGetConvolutionFilterEXT");
	if(!_funcptr_glGetConvolutionFilterEXT) ++numFailed;
	_funcptr_glGetConvolutionParameterfvEXT = (PFNGLGETCONVOLUTIONPARAMETERFVEXTPROC)IntGetProcAddress("glGetConvolutionParameterfvEXT");
	if(!_funcptr_glGetConvolutionParameterfvEXT) ++numFailed;
	_funcptr_glGetConvolutionParameterivEXT = (PFNGLGETCONVOLUTIONPARAMETERIVEXTPROC)IntGetProcAddress("glGetConvolutionParameterivEXT");
	if(!_funcptr_glGetConvolutionParameterivEXT) ++numFailed;
	_funcptr_glGetSeparableFilterEXT = (PFNGLGETSEPARABLEFILTEREXTPROC)IntGetProcAddress("glGetSeparableFilterEXT");
	if(!_funcptr_glGetSeparableFilterEXT) ++numFailed;
	_funcptr_glSeparableFilter2DEXT = (PFNGLSEPARABLEFILTER2DEXTPROC)IntGetProcAddress("glSeparableFilter2DEXT");
	if(!_funcptr_glSeparableFilter2DEXT) ++numFailed;
	return numFailed;
}

PFNGLTANGENT3BEXTPROC _funcptr_glTangent3bEXT = NULL;
PFNGLTANGENT3BVEXTPROC _funcptr_glTangent3bvEXT = NULL;
PFNGLTANGENT3DEXTPROC _funcptr_glTangent3dEXT = NULL;
PFNGLTANGENT3DVEXTPROC _funcptr_glTangent3dvEXT = NULL;
PFNGLTANGENT3FEXTPROC _funcptr_glTangent3fEXT = NULL;
PFNGLTANGENT3FVEXTPROC _funcptr_glTangent3fvEXT = NULL;
PFNGLTANGENT3IEXTPROC _funcptr_glTangent3iEXT = NULL;
PFNGLTANGENT3IVEXTPROC _funcptr_glTangent3ivEXT = NULL;
PFNGLTANGENT3SEXTPROC _funcptr_glTangent3sEXT = NULL;
PFNGLTANGENT3SVEXTPROC _funcptr_glTangent3svEXT = NULL;
PFNGLBINORMAL3BEXTPROC _funcptr_glBinormal3bEXT = NULL;
PFNGLBINORMAL3BVEXTPROC _funcptr_glBinormal3bvEXT = NULL;
PFNGLBINORMAL3DEXTPROC _funcptr_glBinormal3dEXT = NULL;
PFNGLBINORMAL3DVEXTPROC _funcptr_glBinormal3dvEXT = NULL;
PFNGLBINORMAL3FEXTPROC _funcptr_glBinormal3fEXT = NULL;
PFNGLBINORMAL3FVEXTPROC _funcptr_glBinormal3fvEXT = NULL;
PFNGLBINORMAL3IEXTPROC _funcptr_glBinormal3iEXT = NULL;
PFNGLBINORMAL3IVEXTPROC _funcptr_glBinormal3ivEXT = NULL;
PFNGLBINORMAL3SEXTPROC _funcptr_glBinormal3sEXT = NULL;
PFNGLBINORMAL3SVEXTPROC _funcptr_glBinormal3svEXT = NULL;
PFNGLTANGENTPOINTEREXTPROC _funcptr_glTangentPointerEXT = NULL;
PFNGLBINORMALPOINTEREXTPROC _funcptr_glBinormalPointerEXT = NULL;

static int LoadExt_EXT_coordinate_frame()
{
	int numFailed = 0;
	_funcptr_glTangent3bEXT = (PFNGLTANGENT3BEXTPROC)IntGetProcAddress("glTangent3bEXT");
	if(!_funcptr_glTangent3bEXT) ++numFailed;
	_funcptr_glTangent3bvEXT = (PFNGLTANGENT3BVEXTPROC)IntGetProcAddress("glTangent3bvEXT");
	if(!_funcptr_glTangent3bvEXT) ++numFailed;
	_funcptr_glTangent3dEXT = (PFNGLTANGENT3DEXTPROC)IntGetProcAddress("glTangent3dEXT");
	if(!_funcptr_glTangent3dEXT) ++numFailed;
	_funcptr_glTangent3dvEXT = (PFNGLTANGENT3DVEXTPROC)IntGetProcAddress("glTangent3dvEXT");
	if(!_funcptr_glTangent3dvEXT) ++numFailed;
	_funcptr_glTangent3fEXT = (PFNGLTANGENT3FEXTPROC)IntGetProcAddress("glTangent3fEXT");
	if(!_funcptr_glTangent3fEXT) ++numFailed;
	_funcptr_glTangent3fvEXT = (PFNGLTANGENT3FVEXTPROC)IntGetProcAddress("glTangent3fvEXT");
	if(!_funcptr_glTangent3fvEXT) ++numFailed;
	_funcptr_glTangent3iEXT = (PFNGLTANGENT3IEXTPROC)IntGetProcAddress("glTangent3iEXT");
	if(!_funcptr_glTangent3iEXT) ++numFailed;
	_funcptr_glTangent3ivEXT = (PFNGLTANGENT3IVEXTPROC)IntGetProcAddress("glTangent3ivEXT");
	if(!_funcptr_glTangent3ivEXT) ++numFailed;
	_funcptr_glTangent3sEXT = (PFNGLTANGENT3SEXTPROC)IntGetProcAddress("glTangent3sEXT");
	if(!_funcptr_glTangent3sEXT) ++numFailed;
	_funcptr_glTangent3svEXT = (PFNGLTANGENT3SVEXTPROC)IntGetProcAddress("glTangent3svEXT");
	if(!_funcptr_glTangent3svEXT) ++numFailed;
	_funcptr_glBinormal3bEXT = (PFNGLBINORMAL3BEXTPROC)IntGetProcAddress("glBinormal3bEXT");
	if(!_funcptr_glBinormal3bEXT) ++numFailed;
	_funcptr_glBinormal3bvEXT = (PFNGLBINORMAL3BVEXTPROC)IntGetProcAddress("glBinormal3bvEXT");
	if(!_funcptr_glBinormal3bvEXT) ++numFailed;
	_funcptr_glBinormal3dEXT = (PFNGLBINORMAL3DEXTPROC)IntGetProcAddress("glBinormal3dEXT");
	if(!_funcptr_glBinormal3dEXT) ++numFailed;
	_funcptr_glBinormal3dvEXT = (PFNGLBINORMAL3DVEXTPROC)IntGetProcAddress("glBinormal3dvEXT");
	if(!_funcptr_glBinormal3dvEXT) ++numFailed;
	_funcptr_glBinormal3fEXT = (PFNGLBINORMAL3FEXTPROC)IntGetProcAddress("glBinormal3fEXT");
	if(!_funcptr_glBinormal3fEXT) ++numFailed;
	_funcptr_glBinormal3fvEXT = (PFNGLBINORMAL3FVEXTPROC)IntGetProcAddress("glBinormal3fvEXT");
	if(!_funcptr_glBinormal3fvEXT) ++numFailed;
	_funcptr_glBinormal3iEXT = (PFNGLBINORMAL3IEXTPROC)IntGetProcAddress("glBinormal3iEXT");
	if(!_funcptr_glBinormal3iEXT) ++numFailed;
	_funcptr_glBinormal3ivEXT = (PFNGLBINORMAL3IVEXTPROC)IntGetProcAddress("glBinormal3ivEXT");
	if(!_funcptr_glBinormal3ivEXT) ++numFailed;
	_funcptr_glBinormal3sEXT = (PFNGLBINORMAL3SEXTPROC)IntGetProcAddress("glBinormal3sEXT");
	if(!_funcptr_glBinormal3sEXT) ++numFailed;
	_funcptr_glBinormal3svEXT = (PFNGLBINORMAL3SVEXTPROC)IntGetProcAddress("glBinormal3svEXT");
	if(!_funcptr_glBinormal3svEXT) ++numFailed;
	_funcptr_glTangentPointerEXT = (PFNGLTANGENTPOINTEREXTPROC)IntGetProcAddress("glTangentPointerEXT");
	if(!_funcptr_glTangentPointerEXT) ++numFailed;
	_funcptr_glBinormalPointerEXT = (PFNGLBINORMALPOINTEREXTPROC)IntGetProcAddress("glBinormalPointerEXT");
	if(!_funcptr_glBinormalPointerEXT) ++numFailed;
	return numFailed;
}

PFNGLCOPYTEXIMAGE1DEXTPROC _funcptr_glCopyTexImage1DEXT = NULL;
PFNGLCOPYTEXIMAGE2DEXTPROC _funcptr_glCopyTexImage2DEXT = NULL;
PFNGLCOPYTEXSUBIMAGE1DEXTPROC _funcptr_glCopyTexSubImage1DEXT = NULL;
PFNGLCOPYTEXSUBIMAGE2DEXTPROC _funcptr_glCopyTexSubImage2DEXT = NULL;
PFNGLCOPYTEXSUBIMAGE3DEXTPROC _funcptr_glCopyTexSubImage3DEXT = NULL;

static int LoadExt_EXT_copy_texture()
{
	int numFailed = 0;
	_funcptr_glCopyTexImage1DEXT = (PFNGLCOPYTEXIMAGE1DEXTPROC)IntGetProcAddress("glCopyTexImage1DEXT");
	if(!_funcptr_glCopyTexImage1DEXT) ++numFailed;
	_funcptr_glCopyTexImage2DEXT = (PFNGLCOPYTEXIMAGE2DEXTPROC)IntGetProcAddress("glCopyTexImage2DEXT");
	if(!_funcptr_glCopyTexImage2DEXT) ++numFailed;
	_funcptr_glCopyTexSubImage1DEXT = (PFNGLCOPYTEXSUBIMAGE1DEXTPROC)IntGetProcAddress("glCopyTexSubImage1DEXT");
	if(!_funcptr_glCopyTexSubImage1DEXT) ++numFailed;
	_funcptr_glCopyTexSubImage2DEXT = (PFNGLCOPYTEXSUBIMAGE2DEXTPROC)IntGetProcAddress("glCopyTexSubImage2DEXT");
	if(!_funcptr_glCopyTexSubImage2DEXT) ++numFailed;
	_funcptr_glCopyTexSubImage3DEXT = (PFNGLCOPYTEXSUBIMAGE3DEXTPROC)IntGetProcAddress("glCopyTexSubImage3DEXT");
	if(!_funcptr_glCopyTexSubImage3DEXT) ++numFailed;
	return numFailed;
}

PFNGLCULLPARAMETERDVEXTPROC _funcptr_glCullParameterdvEXT = NULL;
PFNGLCULLPARAMETERFVEXTPROC _funcptr_glCullParameterfvEXT = NULL;

static int LoadExt_EXT_cull_vertex()
{
	int numFailed = 0;
	_funcptr_glCullParameterdvEXT = (PFNGLCULLPARAMETERDVEXTPROC)IntGetProcAddress("glCullParameterdvEXT");
	if(!_funcptr_glCullParameterdvEXT) ++numFailed;
	_funcptr_glCullParameterfvEXT = (PFNGLCULLPARAMETERFVEXTPROC)IntGetProcAddress("glCullParameterfvEXT");
	if(!_funcptr_glCullParameterfvEXT) ++numFailed;
	return numFailed;
}

PFNGLDEPTHBOUNDSEXTPROC _funcptr_glDepthBoundsEXT = NULL;

static int LoadExt_EXT_depth_bounds_test()
{
	int numFailed = 0;
	_funcptr_glDepthBoundsEXT = (PFNGLDEPTHBOUNDSEXTPROC)IntGetProcAddress("glDepthBoundsEXT");
	if(!_funcptr_glDepthBoundsEXT) ++numFailed;
	return numFailed;
}

PFNGLCLIENTATTRIBDEFAULTEXTPROC _funcptr_glClientAttribDefaultEXT = NULL;
PFNGLPUSHCLIENTATTRIBDEFAULTEXTPROC _funcptr_glPushClientAttribDefaultEXT = NULL;
PFNGLMATRIXLOADFEXTPROC _funcptr_glMatrixLoadfEXT = NULL;
PFNGLMATRIXLOADDEXTPROC _funcptr_glMatrixLoaddEXT = NULL;
PFNGLMATRIXMULTFEXTPROC _funcptr_glMatrixMultfEXT = NULL;
PFNGLMATRIXMULTDEXTPROC _funcptr_glMatrixMultdEXT = NULL;
PFNGLMATRIXLOADIDENTITYEXTPROC _funcptr_glMatrixLoadIdentityEXT = NULL;
PFNGLMATRIXROTATEFEXTPROC _funcptr_glMatrixRotatefEXT = NULL;
PFNGLMATRIXROTATEDEXTPROC _funcptr_glMatrixRotatedEXT = NULL;
PFNGLMATRIXSCALEFEXTPROC _funcptr_glMatrixScalefEXT = NULL;
PFNGLMATRIXSCALEDEXTPROC _funcptr_glMatrixScaledEXT = NULL;
PFNGLMATRIXTRANSLATEFEXTPROC _funcptr_glMatrixTranslatefEXT = NULL;
PFNGLMATRIXTRANSLATEDEXTPROC _funcptr_glMatrixTranslatedEXT = NULL;
PFNGLMATRIXFRUSTUMEXTPROC _funcptr_glMatrixFrustumEXT = NULL;
PFNGLMATRIXORTHOEXTPROC _funcptr_glMatrixOrthoEXT = NULL;
PFNGLMATRIXPOPEXTPROC _funcptr_glMatrixPopEXT = NULL;
PFNGLMATRIXPUSHEXTPROC _funcptr_glMatrixPushEXT = NULL;
PFNGLMATRIXLOADTRANSPOSEFEXTPROC _funcptr_glMatrixLoadTransposefEXT = NULL;
PFNGLMATRIXLOADTRANSPOSEDEXTPROC _funcptr_glMatrixLoadTransposedEXT = NULL;
PFNGLMATRIXMULTTRANSPOSEFEXTPROC _funcptr_glMatrixMultTransposefEXT = NULL;
PFNGLMATRIXMULTTRANSPOSEDEXTPROC _funcptr_glMatrixMultTransposedEXT = NULL;
PFNGLTEXTUREPARAMETERFEXTPROC _funcptr_glTextureParameterfEXT = NULL;
PFNGLTEXTUREPARAMETERFVEXTPROC _funcptr_glTextureParameterfvEXT = NULL;
PFNGLTEXTUREPARAMETERIEXTPROC _funcptr_glTextureParameteriEXT = NULL;
PFNGLTEXTUREPARAMETERIVEXTPROC _funcptr_glTextureParameterivEXT = NULL;
PFNGLTEXTUREIMAGE1DEXTPROC _funcptr_glTextureImage1DEXT = NULL;
PFNGLTEXTUREIMAGE2DEXTPROC _funcptr_glTextureImage2DEXT = NULL;
PFNGLTEXTURESUBIMAGE1DEXTPROC _funcptr_glTextureSubImage1DEXT = NULL;
PFNGLTEXTURESUBIMAGE2DEXTPROC _funcptr_glTextureSubImage2DEXT = NULL;
PFNGLCOPYTEXTUREIMAGE1DEXTPROC _funcptr_glCopyTextureImage1DEXT = NULL;
PFNGLCOPYTEXTUREIMAGE2DEXTPROC _funcptr_glCopyTextureImage2DEXT = NULL;
PFNGLCOPYTEXTURESUBIMAGE1DEXTPROC _funcptr_glCopyTextureSubImage1DEXT = NULL;
PFNGLCOPYTEXTURESUBIMAGE2DEXTPROC _funcptr_glCopyTextureSubImage2DEXT = NULL;
PFNGLGETTEXTUREIMAGEEXTPROC _funcptr_glGetTextureImageEXT = NULL;
PFNGLGETTEXTUREPARAMETERFVEXTPROC _funcptr_glGetTextureParameterfvEXT = NULL;
PFNGLGETTEXTUREPARAMETERIVEXTPROC _funcptr_glGetTextureParameterivEXT = NULL;
PFNGLGETTEXTURELEVELPARAMETERFVEXTPROC _funcptr_glGetTextureLevelParameterfvEXT = NULL;
PFNGLGETTEXTURELEVELPARAMETERIVEXTPROC _funcptr_glGetTextureLevelParameterivEXT = NULL;
PFNGLTEXTUREIMAGE3DEXTPROC _funcptr_glTextureImage3DEXT = NULL;
PFNGLTEXTURESUBIMAGE3DEXTPROC _funcptr_glTextureSubImage3DEXT = NULL;
PFNGLCOPYTEXTURESUBIMAGE3DEXTPROC _funcptr_glCopyTextureSubImage3DEXT = NULL;
PFNGLMULTITEXPARAMETERFEXTPROC _funcptr_glMultiTexParameterfEXT = NULL;
PFNGLMULTITEXPARAMETERFVEXTPROC _funcptr_glMultiTexParameterfvEXT = NULL;
PFNGLMULTITEXPARAMETERIEXTPROC _funcptr_glMultiTexParameteriEXT = NULL;
PFNGLMULTITEXPARAMETERIVEXTPROC _funcptr_glMultiTexParameterivEXT = NULL;
PFNGLMULTITEXIMAGE1DEXTPROC _funcptr_glMultiTexImage1DEXT = NULL;
PFNGLMULTITEXIMAGE2DEXTPROC _funcptr_glMultiTexImage2DEXT = NULL;
PFNGLMULTITEXSUBIMAGE1DEXTPROC _funcptr_glMultiTexSubImage1DEXT = NULL;
PFNGLMULTITEXSUBIMAGE2DEXTPROC _funcptr_glMultiTexSubImage2DEXT = NULL;
PFNGLCOPYMULTITEXIMAGE1DEXTPROC _funcptr_glCopyMultiTexImage1DEXT = NULL;
PFNGLCOPYMULTITEXIMAGE2DEXTPROC _funcptr_glCopyMultiTexImage2DEXT = NULL;
PFNGLCOPYMULTITEXSUBIMAGE1DEXTPROC _funcptr_glCopyMultiTexSubImage1DEXT = NULL;
PFNGLCOPYMULTITEXSUBIMAGE2DEXTPROC _funcptr_glCopyMultiTexSubImage2DEXT = NULL;
PFNGLGETMULTITEXIMAGEEXTPROC _funcptr_glGetMultiTexImageEXT = NULL;
PFNGLGETMULTITEXPARAMETERFVEXTPROC _funcptr_glGetMultiTexParameterfvEXT = NULL;
PFNGLGETMULTITEXPARAMETERIVEXTPROC _funcptr_glGetMultiTexParameterivEXT = NULL;
PFNGLGETMULTITEXLEVELPARAMETERFVEXTPROC _funcptr_glGetMultiTexLevelParameterfvEXT = NULL;
PFNGLGETMULTITEXLEVELPARAMETERIVEXTPROC _funcptr_glGetMultiTexLevelParameterivEXT = NULL;
PFNGLMULTITEXIMAGE3DEXTPROC _funcptr_glMultiTexImage3DEXT = NULL;
PFNGLMULTITEXSUBIMAGE3DEXTPROC _funcptr_glMultiTexSubImage3DEXT = NULL;
PFNGLCOPYMULTITEXSUBIMAGE3DEXTPROC _funcptr_glCopyMultiTexSubImage3DEXT = NULL;
PFNGLBINDMULTITEXTUREEXTPROC _funcptr_glBindMultiTextureEXT = NULL;
PFNGLENABLECLIENTSTATEINDEXEDEXTPROC _funcptr_glEnableClientStateIndexedEXT = NULL;
PFNGLDISABLECLIENTSTATEINDEXEDEXTPROC _funcptr_glDisableClientStateIndexedEXT = NULL;
PFNGLENABLECLIENTSTATEIEXTPROC _funcptr_glEnableClientStateiEXT = NULL;
PFNGLDISABLECLIENTSTATEIEXTPROC _funcptr_glDisableClientStateiEXT = NULL;
PFNGLMULTITEXCOORDPOINTEREXTPROC _funcptr_glMultiTexCoordPointerEXT = NULL;
PFNGLMULTITEXENVFEXTPROC _funcptr_glMultiTexEnvfEXT = NULL;
PFNGLMULTITEXENVFVEXTPROC _funcptr_glMultiTexEnvfvEXT = NULL;
PFNGLMULTITEXENVIEXTPROC _funcptr_glMultiTexEnviEXT = NULL;
PFNGLMULTITEXENVIVEXTPROC _funcptr_glMultiTexEnvivEXT = NULL;
PFNGLMULTITEXGENDEXTPROC _funcptr_glMultiTexGendEXT = NULL;
PFNGLMULTITEXGENDVEXTPROC _funcptr_glMultiTexGendvEXT = NULL;
PFNGLMULTITEXGENFEXTPROC _funcptr_glMultiTexGenfEXT = NULL;
PFNGLMULTITEXGENFVEXTPROC _funcptr_glMultiTexGenfvEXT = NULL;
PFNGLMULTITEXGENIEXTPROC _funcptr_glMultiTexGeniEXT = NULL;
PFNGLMULTITEXGENIVEXTPROC _funcptr_glMultiTexGenivEXT = NULL;
PFNGLGETMULTITEXENVFVEXTPROC _funcptr_glGetMultiTexEnvfvEXT = NULL;
PFNGLGETMULTITEXENVIVEXTPROC _funcptr_glGetMultiTexEnvivEXT = NULL;
PFNGLGETMULTITEXGENDVEXTPROC _funcptr_glGetMultiTexGendvEXT = NULL;
PFNGLGETMULTITEXGENFVEXTPROC _funcptr_glGetMultiTexGenfvEXT = NULL;
PFNGLGETMULTITEXGENIVEXTPROC _funcptr_glGetMultiTexGenivEXT = NULL;
PFNGLGETFLOATINDEXEDVEXTPROC _funcptr_glGetFloatIndexedvEXT = NULL;
PFNGLGETDOUBLEINDEXEDVEXTPROC _funcptr_glGetDoubleIndexedvEXT = NULL;
PFNGLGETPOINTERINDEXEDVEXTPROC _funcptr_glGetPointerIndexedvEXT = NULL;
PFNGLGETFLOATI_VEXTPROC _funcptr_glGetFloati_vEXT = NULL;
PFNGLGETDOUBLEI_VEXTPROC _funcptr_glGetDoublei_vEXT = NULL;
PFNGLGETPOINTERI_VEXTPROC _funcptr_glGetPointeri_vEXT = NULL;
PFNGLCOMPRESSEDTEXTUREIMAGE3DEXTPROC _funcptr_glCompressedTextureImage3DEXT = NULL;
PFNGLCOMPRESSEDTEXTUREIMAGE2DEXTPROC _funcptr_glCompressedTextureImage2DEXT = NULL;
PFNGLCOMPRESSEDTEXTUREIMAGE1DEXTPROC _funcptr_glCompressedTextureImage1DEXT = NULL;
PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXTPROC _funcptr_glCompressedTextureSubImage3DEXT = NULL;
PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXTPROC _funcptr_glCompressedTextureSubImage2DEXT = NULL;
PFNGLCOMPRESSEDTEXTURESUBIMAGE1DEXTPROC _funcptr_glCompressedTextureSubImage1DEXT = NULL;
PFNGLGETCOMPRESSEDTEXTUREIMAGEEXTPROC _funcptr_glGetCompressedTextureImageEXT = NULL;
PFNGLCOMPRESSEDMULTITEXIMAGE3DEXTPROC _funcptr_glCompressedMultiTexImage3DEXT = NULL;
PFNGLCOMPRESSEDMULTITEXIMAGE2DEXTPROC _funcptr_glCompressedMultiTexImage2DEXT = NULL;
PFNGLCOMPRESSEDMULTITEXIMAGE1DEXTPROC _funcptr_glCompressedMultiTexImage1DEXT = NULL;
PFNGLCOMPRESSEDMULTITEXSUBIMAGE3DEXTPROC _funcptr_glCompressedMultiTexSubImage3DEXT = NULL;
PFNGLCOMPRESSEDMULTITEXSUBIMAGE2DEXTPROC _funcptr_glCompressedMultiTexSubImage2DEXT = NULL;
PFNGLCOMPRESSEDMULTITEXSUBIMAGE1DEXTPROC _funcptr_glCompressedMultiTexSubImage1DEXT = NULL;
PFNGLGETCOMPRESSEDMULTITEXIMAGEEXTPROC _funcptr_glGetCompressedMultiTexImageEXT = NULL;
PFNGLNAMEDPROGRAMSTRINGEXTPROC _funcptr_glNamedProgramStringEXT = NULL;
PFNGLNAMEDPROGRAMLOCALPARAMETER4DEXTPROC _funcptr_glNamedProgramLocalParameter4dEXT = NULL;
PFNGLNAMEDPROGRAMLOCALPARAMETER4DVEXTPROC _funcptr_glNamedProgramLocalParameter4dvEXT = NULL;
PFNGLNAMEDPROGRAMLOCALPARAMETER4FEXTPROC _funcptr_glNamedProgramLocalParameter4fEXT = NULL;
PFNGLNAMEDPROGRAMLOCALPARAMETER4FVEXTPROC _funcptr_glNamedProgramLocalParameter4fvEXT = NULL;
PFNGLGETNAMEDPROGRAMLOCALPARAMETERDVEXTPROC _funcptr_glGetNamedProgramLocalParameterdvEXT = NULL;
PFNGLGETNAMEDPROGRAMLOCALPARAMETERFVEXTPROC _funcptr_glGetNamedProgramLocalParameterfvEXT = NULL;
PFNGLGETNAMEDPROGRAMIVEXTPROC _funcptr_glGetNamedProgramivEXT = NULL;
PFNGLGETNAMEDPROGRAMSTRINGEXTPROC _funcptr_glGetNamedProgramStringEXT = NULL;
PFNGLNAMEDPROGRAMLOCALPARAMETERS4FVEXTPROC _funcptr_glNamedProgramLocalParameters4fvEXT = NULL;
PFNGLNAMEDPROGRAMLOCALPARAMETERI4IEXTPROC _funcptr_glNamedProgramLocalParameterI4iEXT = NULL;
PFNGLNAMEDPROGRAMLOCALPARAMETERI4IVEXTPROC _funcptr_glNamedProgramLocalParameterI4ivEXT = NULL;
PFNGLNAMEDPROGRAMLOCALPARAMETERSI4IVEXTPROC _funcptr_glNamedProgramLocalParametersI4ivEXT = NULL;
PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIEXTPROC _funcptr_glNamedProgramLocalParameterI4uiEXT = NULL;
PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIVEXTPROC _funcptr_glNamedProgramLocalParameterI4uivEXT = NULL;
PFNGLNAMEDPROGRAMLOCALPARAMETERSI4UIVEXTPROC _funcptr_glNamedProgramLocalParametersI4uivEXT = NULL;
PFNGLGETNAMEDPROGRAMLOCALPARAMETERIIVEXTPROC _funcptr_glGetNamedProgramLocalParameterIivEXT = NULL;
PFNGLGETNAMEDPROGRAMLOCALPARAMETERIUIVEXTPROC _funcptr_glGetNamedProgramLocalParameterIuivEXT = NULL;
PFNGLTEXTUREPARAMETERIIVEXTPROC _funcptr_glTextureParameterIivEXT = NULL;
PFNGLTEXTUREPARAMETERIUIVEXTPROC _funcptr_glTextureParameterIuivEXT = NULL;
PFNGLGETTEXTUREPARAMETERIIVEXTPROC _funcptr_glGetTextureParameterIivEXT = NULL;
PFNGLGETTEXTUREPARAMETERIUIVEXTPROC _funcptr_glGetTextureParameterIuivEXT = NULL;
PFNGLMULTITEXPARAMETERIIVEXTPROC _funcptr_glMultiTexParameterIivEXT = NULL;
PFNGLMULTITEXPARAMETERIUIVEXTPROC _funcptr_glMultiTexParameterIuivEXT = NULL;
PFNGLGETMULTITEXPARAMETERIIVEXTPROC _funcptr_glGetMultiTexParameterIivEXT = NULL;
PFNGLGETMULTITEXPARAMETERIUIVEXTPROC _funcptr_glGetMultiTexParameterIuivEXT = NULL;
PFNGLPROGRAMUNIFORM1FEXTPROC _funcptr_glProgramUniform1fEXT = NULL;
PFNGLPROGRAMUNIFORM2FEXTPROC _funcptr_glProgramUniform2fEXT = NULL;
PFNGLPROGRAMUNIFORM3FEXTPROC _funcptr_glProgramUniform3fEXT = NULL;
PFNGLPROGRAMUNIFORM4FEXTPROC _funcptr_glProgramUniform4fEXT = NULL;
PFNGLPROGRAMUNIFORM1IEXTPROC _funcptr_glProgramUniform1iEXT = NULL;
PFNGLPROGRAMUNIFORM2IEXTPROC _funcptr_glProgramUniform2iEXT = NULL;
PFNGLPROGRAMUNIFORM3IEXTPROC _funcptr_glProgramUniform3iEXT = NULL;
PFNGLPROGRAMUNIFORM4IEXTPROC _funcptr_glProgramUniform4iEXT = NULL;
PFNGLPROGRAMUNIFORM1FVEXTPROC _funcptr_glProgramUniform1fvEXT = NULL;
PFNGLPROGRAMUNIFORM2FVEXTPROC _funcptr_glProgramUniform2fvEXT = NULL;
PFNGLPROGRAMUNIFORM3FVEXTPROC _funcptr_glProgramUniform3fvEXT = NULL;
PFNGLPROGRAMUNIFORM4FVEXTPROC _funcptr_glProgramUniform4fvEXT = NULL;
PFNGLPROGRAMUNIFORM1IVEXTPROC _funcptr_glProgramUniform1ivEXT = NULL;
PFNGLPROGRAMUNIFORM2IVEXTPROC _funcptr_glProgramUniform2ivEXT = NULL;
PFNGLPROGRAMUNIFORM3IVEXTPROC _funcptr_glProgramUniform3ivEXT = NULL;
PFNGLPROGRAMUNIFORM4IVEXTPROC _funcptr_glProgramUniform4ivEXT = NULL;
PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC _funcptr_glProgramUniformMatrix2fvEXT = NULL;
PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC _funcptr_glProgramUniformMatrix3fvEXT = NULL;
PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC _funcptr_glProgramUniformMatrix4fvEXT = NULL;
PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC _funcptr_glProgramUniformMatrix2x3fvEXT = NULL;
PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC _funcptr_glProgramUniformMatrix3x2fvEXT = NULL;
PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC _funcptr_glProgramUniformMatrix2x4fvEXT = NULL;
PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC _funcptr_glProgramUniformMatrix4x2fvEXT = NULL;
PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC _funcptr_glProgramUniformMatrix3x4fvEXT = NULL;
PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC _funcptr_glProgramUniformMatrix4x3fvEXT = NULL;
PFNGLPROGRAMUNIFORM1UIEXTPROC _funcptr_glProgramUniform1uiEXT = NULL;
PFNGLPROGRAMUNIFORM2UIEXTPROC _funcptr_glProgramUniform2uiEXT = NULL;
PFNGLPROGRAMUNIFORM3UIEXTPROC _funcptr_glProgramUniform3uiEXT = NULL;
PFNGLPROGRAMUNIFORM4UIEXTPROC _funcptr_glProgramUniform4uiEXT = NULL;
PFNGLPROGRAMUNIFORM1UIVEXTPROC _funcptr_glProgramUniform1uivEXT = NULL;
PFNGLPROGRAMUNIFORM2UIVEXTPROC _funcptr_glProgramUniform2uivEXT = NULL;
PFNGLPROGRAMUNIFORM3UIVEXTPROC _funcptr_glProgramUniform3uivEXT = NULL;
PFNGLPROGRAMUNIFORM4UIVEXTPROC _funcptr_glProgramUniform4uivEXT = NULL;
PFNGLNAMEDBUFFERDATAEXTPROC _funcptr_glNamedBufferDataEXT = NULL;
PFNGLNAMEDBUFFERSUBDATAEXTPROC _funcptr_glNamedBufferSubDataEXT = NULL;
PFNGLMAPNAMEDBUFFEREXTPROC _funcptr_glMapNamedBufferEXT = NULL;
PFNGLUNMAPNAMEDBUFFEREXTPROC _funcptr_glUnmapNamedBufferEXT = NULL;
PFNGLMAPNAMEDBUFFERRANGEEXTPROC _funcptr_glMapNamedBufferRangeEXT = NULL;
PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEEXTPROC _funcptr_glFlushMappedNamedBufferRangeEXT = NULL;
PFNGLNAMEDCOPYBUFFERSUBDATAEXTPROC _funcptr_glNamedCopyBufferSubDataEXT = NULL;
PFNGLGETNAMEDBUFFERPARAMETERIVEXTPROC _funcptr_glGetNamedBufferParameterivEXT = NULL;
PFNGLGETNAMEDBUFFERPOINTERVEXTPROC _funcptr_glGetNamedBufferPointervEXT = NULL;
PFNGLGETNAMEDBUFFERSUBDATAEXTPROC _funcptr_glGetNamedBufferSubDataEXT = NULL;
PFNGLTEXTUREBUFFEREXTPROC _funcptr_glTextureBufferEXT = NULL;
PFNGLMULTITEXBUFFEREXTPROC _funcptr_glMultiTexBufferEXT = NULL;
PFNGLNAMEDRENDERBUFFERSTORAGEEXTPROC _funcptr_glNamedRenderbufferStorageEXT = NULL;
PFNGLGETNAMEDRENDERBUFFERPARAMETERIVEXTPROC _funcptr_glGetNamedRenderbufferParameterivEXT = NULL;
PFNGLCHECKNAMEDFRAMEBUFFERSTATUSEXTPROC _funcptr_glCheckNamedFramebufferStatusEXT = NULL;
PFNGLNAMEDFRAMEBUFFERTEXTURE1DEXTPROC _funcptr_glNamedFramebufferTexture1DEXT = NULL;
PFNGLNAMEDFRAMEBUFFERTEXTURE2DEXTPROC _funcptr_glNamedFramebufferTexture2DEXT = NULL;
PFNGLNAMEDFRAMEBUFFERTEXTURE3DEXTPROC _funcptr_glNamedFramebufferTexture3DEXT = NULL;
PFNGLNAMEDFRAMEBUFFERRENDERBUFFEREXTPROC _funcptr_glNamedFramebufferRenderbufferEXT = NULL;
PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC _funcptr_glGetNamedFramebufferAttachmentParameterivEXT = NULL;
PFNGLGENERATETEXTUREMIPMAPEXTPROC _funcptr_glGenerateTextureMipmapEXT = NULL;
PFNGLGENERATEMULTITEXMIPMAPEXTPROC _funcptr_glGenerateMultiTexMipmapEXT = NULL;
PFNGLFRAMEBUFFERDRAWBUFFEREXTPROC _funcptr_glFramebufferDrawBufferEXT = NULL;
PFNGLFRAMEBUFFERDRAWBUFFERSEXTPROC _funcptr_glFramebufferDrawBuffersEXT = NULL;
PFNGLFRAMEBUFFERREADBUFFEREXTPROC _funcptr_glFramebufferReadBufferEXT = NULL;
PFNGLGETFRAMEBUFFERPARAMETERIVEXTPROC _funcptr_glGetFramebufferParameterivEXT = NULL;
PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC _funcptr_glNamedRenderbufferStorageMultisampleEXT = NULL;
PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLECOVERAGEEXTPROC _funcptr_glNamedRenderbufferStorageMultisampleCoverageEXT = NULL;
PFNGLNAMEDFRAMEBUFFERTEXTUREEXTPROC _funcptr_glNamedFramebufferTextureEXT = NULL;
PFNGLNAMEDFRAMEBUFFERTEXTURELAYEREXTPROC _funcptr_glNamedFramebufferTextureLayerEXT = NULL;
PFNGLNAMEDFRAMEBUFFERTEXTUREFACEEXTPROC _funcptr_glNamedFramebufferTextureFaceEXT = NULL;
PFNGLTEXTURERENDERBUFFEREXTPROC _funcptr_glTextureRenderbufferEXT = NULL;
PFNGLMULTITEXRENDERBUFFEREXTPROC _funcptr_glMultiTexRenderbufferEXT = NULL;
PFNGLPROGRAMUNIFORM1DEXTPROC _funcptr_glProgramUniform1dEXT = NULL;
PFNGLPROGRAMUNIFORM2DEXTPROC _funcptr_glProgramUniform2dEXT = NULL;
PFNGLPROGRAMUNIFORM3DEXTPROC _funcptr_glProgramUniform3dEXT = NULL;
PFNGLPROGRAMUNIFORM4DEXTPROC _funcptr_glProgramUniform4dEXT = NULL;
PFNGLPROGRAMUNIFORM1DVEXTPROC _funcptr_glProgramUniform1dvEXT = NULL;
PFNGLPROGRAMUNIFORM2DVEXTPROC _funcptr_glProgramUniform2dvEXT = NULL;
PFNGLPROGRAMUNIFORM3DVEXTPROC _funcptr_glProgramUniform3dvEXT = NULL;
PFNGLPROGRAMUNIFORM4DVEXTPROC _funcptr_glProgramUniform4dvEXT = NULL;
PFNGLPROGRAMUNIFORMMATRIX2DVEXTPROC _funcptr_glProgramUniformMatrix2dvEXT = NULL;
PFNGLPROGRAMUNIFORMMATRIX3DVEXTPROC _funcptr_glProgramUniformMatrix3dvEXT = NULL;
PFNGLPROGRAMUNIFORMMATRIX4DVEXTPROC _funcptr_glProgramUniformMatrix4dvEXT = NULL;
PFNGLPROGRAMUNIFORMMATRIX2X3DVEXTPROC _funcptr_glProgramUniformMatrix2x3dvEXT = NULL;
PFNGLPROGRAMUNIFORMMATRIX2X4DVEXTPROC _funcptr_glProgramUniformMatrix2x4dvEXT = NULL;
PFNGLPROGRAMUNIFORMMATRIX3X2DVEXTPROC _funcptr_glProgramUniformMatrix3x2dvEXT = NULL;
PFNGLPROGRAMUNIFORMMATRIX3X4DVEXTPROC _funcptr_glProgramUniformMatrix3x4dvEXT = NULL;
PFNGLPROGRAMUNIFORMMATRIX4X2DVEXTPROC _funcptr_glProgramUniformMatrix4x2dvEXT = NULL;
PFNGLPROGRAMUNIFORMMATRIX4X3DVEXTPROC _funcptr_glProgramUniformMatrix4x3dvEXT = NULL;
PFNGLENABLEVERTEXARRAYATTRIBEXTPROC _funcptr_glEnableVertexArrayAttribEXT = NULL;
PFNGLDISABLEVERTEXARRAYATTRIBEXTPROC _funcptr_glDisableVertexArrayAttribEXT = NULL;
PFNGLENABLEVERTEXARRAYEXTPROC _funcptr_glEnableVertexArrayEXT = NULL;
PFNGLDISABLEVERTEXARRAYEXTPROC _funcptr_glDisableVertexArrayEXT = NULL;
PFNGLVERTEXARRAYCOLOROFFSETEXTPROC _funcptr_glVertexArrayColorOffsetEXT = NULL;
PFNGLVERTEXARRAYEDGEFLAGOFFSETEXTPROC _funcptr_glVertexArrayEdgeFlagOffsetEXT = NULL;
PFNGLVERTEXARRAYFOGCOORDOFFSETEXTPROC _funcptr_glVertexArrayFogCoordOffsetEXT = NULL;
PFNGLVERTEXARRAYINDEXOFFSETEXTPROC _funcptr_glVertexArrayIndexOffsetEXT = NULL;
PFNGLVERTEXARRAYMULTITEXCOORDOFFSETEXTPROC _funcptr_glVertexArrayMultiTexCoordOffsetEXT = NULL;
PFNGLVERTEXARRAYNORMALOFFSETEXTPROC _funcptr_glVertexArrayNormalOffsetEXT = NULL;
PFNGLVERTEXARRAYSECONDARYCOLOROFFSETEXTPROC _funcptr_glVertexArraySecondaryColorOffsetEXT = NULL;
PFNGLVERTEXARRAYTEXCOORDOFFSETEXTPROC _funcptr_glVertexArrayTexCoordOffsetEXT = NULL;
PFNGLVERTEXARRAYVERTEXOFFSETEXTPROC _funcptr_glVertexArrayVertexOffsetEXT = NULL;
PFNGLVERTEXARRAYVERTEXATTRIBIOFFSETEXTPROC _funcptr_glVertexArrayVertexAttribIOffsetEXT = NULL;
PFNGLVERTEXARRAYVERTEXATTRIBOFFSETEXTPROC _funcptr_glVertexArrayVertexAttribOffsetEXT = NULL;
PFNGLGETVERTEXARRAYINTEGERVEXTPROC _funcptr_glGetVertexArrayIntegervEXT = NULL;
PFNGLGETVERTEXARRAYPOINTERVEXTPROC _funcptr_glGetVertexArrayPointervEXT = NULL;
PFNGLGETVERTEXARRAYINTEGERI_VEXTPROC _funcptr_glGetVertexArrayIntegeri_vEXT = NULL;
PFNGLGETVERTEXARRAYPOINTERI_VEXTPROC _funcptr_glGetVertexArrayPointeri_vEXT = NULL;

static int LoadExt_EXT_direct_state_access()
{
	int numFailed = 0;
	_funcptr_glClientAttribDefaultEXT = (PFNGLCLIENTATTRIBDEFAULTEXTPROC)IntGetProcAddress("glClientAttribDefaultEXT");
	if(!_funcptr_glClientAttribDefaultEXT) ++numFailed;
	_funcptr_glPushClientAttribDefaultEXT = (PFNGLPUSHCLIENTATTRIBDEFAULTEXTPROC)IntGetProcAddress("glPushClientAttribDefaultEXT");
	if(!_funcptr_glPushClientAttribDefaultEXT) ++numFailed;
	_funcptr_glMatrixLoadfEXT = (PFNGLMATRIXLOADFEXTPROC)IntGetProcAddress("glMatrixLoadfEXT");
	if(!_funcptr_glMatrixLoadfEXT) ++numFailed;
	_funcptr_glMatrixLoaddEXT = (PFNGLMATRIXLOADDEXTPROC)IntGetProcAddress("glMatrixLoaddEXT");
	if(!_funcptr_glMatrixLoaddEXT) ++numFailed;
	_funcptr_glMatrixMultfEXT = (PFNGLMATRIXMULTFEXTPROC)IntGetProcAddress("glMatrixMultfEXT");
	if(!_funcptr_glMatrixMultfEXT) ++numFailed;
	_funcptr_glMatrixMultdEXT = (PFNGLMATRIXMULTDEXTPROC)IntGetProcAddress("glMatrixMultdEXT");
	if(!_funcptr_glMatrixMultdEXT) ++numFailed;
	_funcptr_glMatrixLoadIdentityEXT = (PFNGLMATRIXLOADIDENTITYEXTPROC)IntGetProcAddress("glMatrixLoadIdentityEXT");
	if(!_funcptr_glMatrixLoadIdentityEXT) ++numFailed;
	_funcptr_glMatrixRotatefEXT = (PFNGLMATRIXROTATEFEXTPROC)IntGetProcAddress("glMatrixRotatefEXT");
	if(!_funcptr_glMatrixRotatefEXT) ++numFailed;
	_funcptr_glMatrixRotatedEXT = (PFNGLMATRIXROTATEDEXTPROC)IntGetProcAddress("glMatrixRotatedEXT");
	if(!_funcptr_glMatrixRotatedEXT) ++numFailed;
	_funcptr_glMatrixScalefEXT = (PFNGLMATRIXSCALEFEXTPROC)IntGetProcAddress("glMatrixScalefEXT");
	if(!_funcptr_glMatrixScalefEXT) ++numFailed;
	_funcptr_glMatrixScaledEXT = (PFNGLMATRIXSCALEDEXTPROC)IntGetProcAddress("glMatrixScaledEXT");
	if(!_funcptr_glMatrixScaledEXT) ++numFailed;
	_funcptr_glMatrixTranslatefEXT = (PFNGLMATRIXTRANSLATEFEXTPROC)IntGetProcAddress("glMatrixTranslatefEXT");
	if(!_funcptr_glMatrixTranslatefEXT) ++numFailed;
	_funcptr_glMatrixTranslatedEXT = (PFNGLMATRIXTRANSLATEDEXTPROC)IntGetProcAddress("glMatrixTranslatedEXT");
	if(!_funcptr_glMatrixTranslatedEXT) ++numFailed;
	_funcptr_glMatrixFrustumEXT = (PFNGLMATRIXFRUSTUMEXTPROC)IntGetProcAddress("glMatrixFrustumEXT");
	if(!_funcptr_glMatrixFrustumEXT) ++numFailed;
	_funcptr_glMatrixOrthoEXT = (PFNGLMATRIXORTHOEXTPROC)IntGetProcAddress("glMatrixOrthoEXT");
	if(!_funcptr_glMatrixOrthoEXT) ++numFailed;
	_funcptr_glMatrixPopEXT = (PFNGLMATRIXPOPEXTPROC)IntGetProcAddress("glMatrixPopEXT");
	if(!_funcptr_glMatrixPopEXT) ++numFailed;
	_funcptr_glMatrixPushEXT = (PFNGLMATRIXPUSHEXTPROC)IntGetProcAddress("glMatrixPushEXT");
	if(!_funcptr_glMatrixPushEXT) ++numFailed;
	_funcptr_glMatrixLoadTransposefEXT = (PFNGLMATRIXLOADTRANSPOSEFEXTPROC)IntGetProcAddress("glMatrixLoadTransposefEXT");
	if(!_funcptr_glMatrixLoadTransposefEXT) ++numFailed;
	_funcptr_glMatrixLoadTransposedEXT = (PFNGLMATRIXLOADTRANSPOSEDEXTPROC)IntGetProcAddress("glMatrixLoadTransposedEXT");
	if(!_funcptr_glMatrixLoadTransposedEXT) ++numFailed;
	_funcptr_glMatrixMultTransposefEXT = (PFNGLMATRIXMULTTRANSPOSEFEXTPROC)IntGetProcAddress("glMatrixMultTransposefEXT");
	if(!_funcptr_glMatrixMultTransposefEXT) ++numFailed;
	_funcptr_glMatrixMultTransposedEXT = (PFNGLMATRIXMULTTRANSPOSEDEXTPROC)IntGetProcAddress("glMatrixMultTransposedEXT");
	if(!_funcptr_glMatrixMultTransposedEXT) ++numFailed;
	_funcptr_glTextureParameterfEXT = (PFNGLTEXTUREPARAMETERFEXTPROC)IntGetProcAddress("glTextureParameterfEXT");
	if(!_funcptr_glTextureParameterfEXT) ++numFailed;
	_funcptr_glTextureParameterfvEXT = (PFNGLTEXTUREPARAMETERFVEXTPROC)IntGetProcAddress("glTextureParameterfvEXT");
	if(!_funcptr_glTextureParameterfvEXT) ++numFailed;
	_funcptr_glTextureParameteriEXT = (PFNGLTEXTUREPARAMETERIEXTPROC)IntGetProcAddress("glTextureParameteriEXT");
	if(!_funcptr_glTextureParameteriEXT) ++numFailed;
	_funcptr_glTextureParameterivEXT = (PFNGLTEXTUREPARAMETERIVEXTPROC)IntGetProcAddress("glTextureParameterivEXT");
	if(!_funcptr_glTextureParameterivEXT) ++numFailed;
	_funcptr_glTextureImage1DEXT = (PFNGLTEXTUREIMAGE1DEXTPROC)IntGetProcAddress("glTextureImage1DEXT");
	if(!_funcptr_glTextureImage1DEXT) ++numFailed;
	_funcptr_glTextureImage2DEXT = (PFNGLTEXTUREIMAGE2DEXTPROC)IntGetProcAddress("glTextureImage2DEXT");
	if(!_funcptr_glTextureImage2DEXT) ++numFailed;
	_funcptr_glTextureSubImage1DEXT = (PFNGLTEXTURESUBIMAGE1DEXTPROC)IntGetProcAddress("glTextureSubImage1DEXT");
	if(!_funcptr_glTextureSubImage1DEXT) ++numFailed;
	_funcptr_glTextureSubImage2DEXT = (PFNGLTEXTURESUBIMAGE2DEXTPROC)IntGetProcAddress("glTextureSubImage2DEXT");
	if(!_funcptr_glTextureSubImage2DEXT) ++numFailed;
	_funcptr_glCopyTextureImage1DEXT = (PFNGLCOPYTEXTUREIMAGE1DEXTPROC)IntGetProcAddress("glCopyTextureImage1DEXT");
	if(!_funcptr_glCopyTextureImage1DEXT) ++numFailed;
	_funcptr_glCopyTextureImage2DEXT = (PFNGLCOPYTEXTUREIMAGE2DEXTPROC)IntGetProcAddress("glCopyTextureImage2DEXT");
	if(!_funcptr_glCopyTextureImage2DEXT) ++numFailed;
	_funcptr_glCopyTextureSubImage1DEXT = (PFNGLCOPYTEXTURESUBIMAGE1DEXTPROC)IntGetProcAddress("glCopyTextureSubImage1DEXT");
	if(!_funcptr_glCopyTextureSubImage1DEXT) ++numFailed;
	_funcptr_glCopyTextureSubImage2DEXT = (PFNGLCOPYTEXTURESUBIMAGE2DEXTPROC)IntGetProcAddress("glCopyTextureSubImage2DEXT");
	if(!_funcptr_glCopyTextureSubImage2DEXT) ++numFailed;
	_funcptr_glGetTextureImageEXT = (PFNGLGETTEXTUREIMAGEEXTPROC)IntGetProcAddress("glGetTextureImageEXT");
	if(!_funcptr_glGetTextureImageEXT) ++numFailed;
	_funcptr_glGetTextureParameterfvEXT = (PFNGLGETTEXTUREPARAMETERFVEXTPROC)IntGetProcAddress("glGetTextureParameterfvEXT");
	if(!_funcptr_glGetTextureParameterfvEXT) ++numFailed;
	_funcptr_glGetTextureParameterivEXT = (PFNGLGETTEXTUREPARAMETERIVEXTPROC)IntGetProcAddress("glGetTextureParameterivEXT");
	if(!_funcptr_glGetTextureParameterivEXT) ++numFailed;
	_funcptr_glGetTextureLevelParameterfvEXT = (PFNGLGETTEXTURELEVELPARAMETERFVEXTPROC)IntGetProcAddress("glGetTextureLevelParameterfvEXT");
	if(!_funcptr_glGetTextureLevelParameterfvEXT) ++numFailed;
	_funcptr_glGetTextureLevelParameterivEXT = (PFNGLGETTEXTURELEVELPARAMETERIVEXTPROC)IntGetProcAddress("glGetTextureLevelParameterivEXT");
	if(!_funcptr_glGetTextureLevelParameterivEXT) ++numFailed;
	_funcptr_glTextureImage3DEXT = (PFNGLTEXTUREIMAGE3DEXTPROC)IntGetProcAddress("glTextureImage3DEXT");
	if(!_funcptr_glTextureImage3DEXT) ++numFailed;
	_funcptr_glTextureSubImage3DEXT = (PFNGLTEXTURESUBIMAGE3DEXTPROC)IntGetProcAddress("glTextureSubImage3DEXT");
	if(!_funcptr_glTextureSubImage3DEXT) ++numFailed;
	_funcptr_glCopyTextureSubImage3DEXT = (PFNGLCOPYTEXTURESUBIMAGE3DEXTPROC)IntGetProcAddress("glCopyTextureSubImage3DEXT");
	if(!_funcptr_glCopyTextureSubImage3DEXT) ++numFailed;
	_funcptr_glMultiTexParameterfEXT = (PFNGLMULTITEXPARAMETERFEXTPROC)IntGetProcAddress("glMultiTexParameterfEXT");
	if(!_funcptr_glMultiTexParameterfEXT) ++numFailed;
	_funcptr_glMultiTexParameterfvEXT = (PFNGLMULTITEXPARAMETERFVEXTPROC)IntGetProcAddress("glMultiTexParameterfvEXT");
	if(!_funcptr_glMultiTexParameterfvEXT) ++numFailed;
	_funcptr_glMultiTexParameteriEXT = (PFNGLMULTITEXPARAMETERIEXTPROC)IntGetProcAddress("glMultiTexParameteriEXT");
	if(!_funcptr_glMultiTexParameteriEXT) ++numFailed;
	_funcptr_glMultiTexParameterivEXT = (PFNGLMULTITEXPARAMETERIVEXTPROC)IntGetProcAddress("glMultiTexParameterivEXT");
	if(!_funcptr_glMultiTexParameterivEXT) ++numFailed;
	_funcptr_glMultiTexImage1DEXT = (PFNGLMULTITEXIMAGE1DEXTPROC)IntGetProcAddress("glMultiTexImage1DEXT");
	if(!_funcptr_glMultiTexImage1DEXT) ++numFailed;
	_funcptr_glMultiTexImage2DEXT = (PFNGLMULTITEXIMAGE2DEXTPROC)IntGetProcAddress("glMultiTexImage2DEXT");
	if(!_funcptr_glMultiTexImage2DEXT) ++numFailed;
	_funcptr_glMultiTexSubImage1DEXT = (PFNGLMULTITEXSUBIMAGE1DEXTPROC)IntGetProcAddress("glMultiTexSubImage1DEXT");
	if(!_funcptr_glMultiTexSubImage1DEXT) ++numFailed;
	_funcptr_glMultiTexSubImage2DEXT = (PFNGLMULTITEXSUBIMAGE2DEXTPROC)IntGetProcAddress("glMultiTexSubImage2DEXT");
	if(!_funcptr_glMultiTexSubImage2DEXT) ++numFailed;
	_funcptr_glCopyMultiTexImage1DEXT = (PFNGLCOPYMULTITEXIMAGE1DEXTPROC)IntGetProcAddress("glCopyMultiTexImage1DEXT");
	if(!_funcptr_glCopyMultiTexImage1DEXT) ++numFailed;
	_funcptr_glCopyMultiTexImage2DEXT = (PFNGLCOPYMULTITEXIMAGE2DEXTPROC)IntGetProcAddress("glCopyMultiTexImage2DEXT");
	if(!_funcptr_glCopyMultiTexImage2DEXT) ++numFailed;
	_funcptr_glCopyMultiTexSubImage1DEXT = (PFNGLCOPYMULTITEXSUBIMAGE1DEXTPROC)IntGetProcAddress("glCopyMultiTexSubImage1DEXT");
	if(!_funcptr_glCopyMultiTexSubImage1DEXT) ++numFailed;
	_funcptr_glCopyMultiTexSubImage2DEXT = (PFNGLCOPYMULTITEXSUBIMAGE2DEXTPROC)IntGetProcAddress("glCopyMultiTexSubImage2DEXT");
	if(!_funcptr_glCopyMultiTexSubImage2DEXT) ++numFailed;
	_funcptr_glGetMultiTexImageEXT = (PFNGLGETMULTITEXIMAGEEXTPROC)IntGetProcAddress("glGetMultiTexImageEXT");
	if(!_funcptr_glGetMultiTexImageEXT) ++numFailed;
	_funcptr_glGetMultiTexParameterfvEXT = (PFNGLGETMULTITEXPARAMETERFVEXTPROC)IntGetProcAddress("glGetMultiTexParameterfvEXT");
	if(!_funcptr_glGetMultiTexParameterfvEXT) ++numFailed;
	_funcptr_glGetMultiTexParameterivEXT = (PFNGLGETMULTITEXPARAMETERIVEXTPROC)IntGetProcAddress("glGetMultiTexParameterivEXT");
	if(!_funcptr_glGetMultiTexParameterivEXT) ++numFailed;
	_funcptr_glGetMultiTexLevelParameterfvEXT = (PFNGLGETMULTITEXLEVELPARAMETERFVEXTPROC)IntGetProcAddress("glGetMultiTexLevelParameterfvEXT");
	if(!_funcptr_glGetMultiTexLevelParameterfvEXT) ++numFailed;
	_funcptr_glGetMultiTexLevelParameterivEXT = (PFNGLGETMULTITEXLEVELPARAMETERIVEXTPROC)IntGetProcAddress("glGetMultiTexLevelParameterivEXT");
	if(!_funcptr_glGetMultiTexLevelParameterivEXT) ++numFailed;
	_funcptr_glMultiTexImage3DEXT = (PFNGLMULTITEXIMAGE3DEXTPROC)IntGetProcAddress("glMultiTexImage3DEXT");
	if(!_funcptr_glMultiTexImage3DEXT) ++numFailed;
	_funcptr_glMultiTexSubImage3DEXT = (PFNGLMULTITEXSUBIMAGE3DEXTPROC)IntGetProcAddress("glMultiTexSubImage3DEXT");
	if(!_funcptr_glMultiTexSubImage3DEXT) ++numFailed;
	_funcptr_glCopyMultiTexSubImage3DEXT = (PFNGLCOPYMULTITEXSUBIMAGE3DEXTPROC)IntGetProcAddress("glCopyMultiTexSubImage3DEXT");
	if(!_funcptr_glCopyMultiTexSubImage3DEXT) ++numFailed;
	_funcptr_glBindMultiTextureEXT = (PFNGLBINDMULTITEXTUREEXTPROC)IntGetProcAddress("glBindMultiTextureEXT");
	if(!_funcptr_glBindMultiTextureEXT) ++numFailed;
	_funcptr_glEnableClientStateIndexedEXT = (PFNGLENABLECLIENTSTATEINDEXEDEXTPROC)IntGetProcAddress("glEnableClientStateIndexedEXT");
	if(!_funcptr_glEnableClientStateIndexedEXT) ++numFailed;
	_funcptr_glDisableClientStateIndexedEXT = (PFNGLDISABLECLIENTSTATEINDEXEDEXTPROC)IntGetProcAddress("glDisableClientStateIndexedEXT");
	if(!_funcptr_glDisableClientStateIndexedEXT) ++numFailed;
	_funcptr_glEnableClientStateiEXT = (PFNGLENABLECLIENTSTATEIEXTPROC)IntGetProcAddress("glEnableClientStateiEXT");
	if(!_funcptr_glEnableClientStateiEXT) ++numFailed;
	_funcptr_glDisableClientStateiEXT = (PFNGLDISABLECLIENTSTATEIEXTPROC)IntGetProcAddress("glDisableClientStateiEXT");
	if(!_funcptr_glDisableClientStateiEXT) ++numFailed;
	_funcptr_glMultiTexCoordPointerEXT = (PFNGLMULTITEXCOORDPOINTEREXTPROC)IntGetProcAddress("glMultiTexCoordPointerEXT");
	if(!_funcptr_glMultiTexCoordPointerEXT) ++numFailed;
	_funcptr_glMultiTexEnvfEXT = (PFNGLMULTITEXENVFEXTPROC)IntGetProcAddress("glMultiTexEnvfEXT");
	if(!_funcptr_glMultiTexEnvfEXT) ++numFailed;
	_funcptr_glMultiTexEnvfvEXT = (PFNGLMULTITEXENVFVEXTPROC)IntGetProcAddress("glMultiTexEnvfvEXT");
	if(!_funcptr_glMultiTexEnvfvEXT) ++numFailed;
	_funcptr_glMultiTexEnviEXT = (PFNGLMULTITEXENVIEXTPROC)IntGetProcAddress("glMultiTexEnviEXT");
	if(!_funcptr_glMultiTexEnviEXT) ++numFailed;
	_funcptr_glMultiTexEnvivEXT = (PFNGLMULTITEXENVIVEXTPROC)IntGetProcAddress("glMultiTexEnvivEXT");
	if(!_funcptr_glMultiTexEnvivEXT) ++numFailed;
	_funcptr_glMultiTexGendEXT = (PFNGLMULTITEXGENDEXTPROC)IntGetProcAddress("glMultiTexGendEXT");
	if(!_funcptr_glMultiTexGendEXT) ++numFailed;
	_funcptr_glMultiTexGendvEXT = (PFNGLMULTITEXGENDVEXTPROC)IntGetProcAddress("glMultiTexGendvEXT");
	if(!_funcptr_glMultiTexGendvEXT) ++numFailed;
	_funcptr_glMultiTexGenfEXT = (PFNGLMULTITEXGENFEXTPROC)IntGetProcAddress("glMultiTexGenfEXT");
	if(!_funcptr_glMultiTexGenfEXT) ++numFailed;
	_funcptr_glMultiTexGenfvEXT = (PFNGLMULTITEXGENFVEXTPROC)IntGetProcAddress("glMultiTexGenfvEXT");
	if(!_funcptr_glMultiTexGenfvEXT) ++numFailed;
	_funcptr_glMultiTexGeniEXT = (PFNGLMULTITEXGENIEXTPROC)IntGetProcAddress("glMultiTexGeniEXT");
	if(!_funcptr_glMultiTexGeniEXT) ++numFailed;
	_funcptr_glMultiTexGenivEXT = (PFNGLMULTITEXGENIVEXTPROC)IntGetProcAddress("glMultiTexGenivEXT");
	if(!_funcptr_glMultiTexGenivEXT) ++numFailed;
	_funcptr_glGetMultiTexEnvfvEXT = (PFNGLGETMULTITEXENVFVEXTPROC)IntGetProcAddress("glGetMultiTexEnvfvEXT");
	if(!_funcptr_glGetMultiTexEnvfvEXT) ++numFailed;
	_funcptr_glGetMultiTexEnvivEXT = (PFNGLGETMULTITEXENVIVEXTPROC)IntGetProcAddress("glGetMultiTexEnvivEXT");
	if(!_funcptr_glGetMultiTexEnvivEXT) ++numFailed;
	_funcptr_glGetMultiTexGendvEXT = (PFNGLGETMULTITEXGENDVEXTPROC)IntGetProcAddress("glGetMultiTexGendvEXT");
	if(!_funcptr_glGetMultiTexGendvEXT) ++numFailed;
	_funcptr_glGetMultiTexGenfvEXT = (PFNGLGETMULTITEXGENFVEXTPROC)IntGetProcAddress("glGetMultiTexGenfvEXT");
	if(!_funcptr_glGetMultiTexGenfvEXT) ++numFailed;
	_funcptr_glGetMultiTexGenivEXT = (PFNGLGETMULTITEXGENIVEXTPROC)IntGetProcAddress("glGetMultiTexGenivEXT");
	if(!_funcptr_glGetMultiTexGenivEXT) ++numFailed;
	_funcptr_glGetFloatIndexedvEXT = (PFNGLGETFLOATINDEXEDVEXTPROC)IntGetProcAddress("glGetFloatIndexedvEXT");
	if(!_funcptr_glGetFloatIndexedvEXT) ++numFailed;
	_funcptr_glGetDoubleIndexedvEXT = (PFNGLGETDOUBLEINDEXEDVEXTPROC)IntGetProcAddress("glGetDoubleIndexedvEXT");
	if(!_funcptr_glGetDoubleIndexedvEXT) ++numFailed;
	_funcptr_glGetPointerIndexedvEXT = (PFNGLGETPOINTERINDEXEDVEXTPROC)IntGetProcAddress("glGetPointerIndexedvEXT");
	if(!_funcptr_glGetPointerIndexedvEXT) ++numFailed;
	_funcptr_glGetFloati_vEXT = (PFNGLGETFLOATI_VEXTPROC)IntGetProcAddress("glGetFloati_vEXT");
	if(!_funcptr_glGetFloati_vEXT) ++numFailed;
	_funcptr_glGetDoublei_vEXT = (PFNGLGETDOUBLEI_VEXTPROC)IntGetProcAddress("glGetDoublei_vEXT");
	if(!_funcptr_glGetDoublei_vEXT) ++numFailed;
	_funcptr_glGetPointeri_vEXT = (PFNGLGETPOINTERI_VEXTPROC)IntGetProcAddress("glGetPointeri_vEXT");
	if(!_funcptr_glGetPointeri_vEXT) ++numFailed;
	_funcptr_glCompressedTextureImage3DEXT = (PFNGLCOMPRESSEDTEXTUREIMAGE3DEXTPROC)IntGetProcAddress("glCompressedTextureImage3DEXT");
	if(!_funcptr_glCompressedTextureImage3DEXT) ++numFailed;
	_funcptr_glCompressedTextureImage2DEXT = (PFNGLCOMPRESSEDTEXTUREIMAGE2DEXTPROC)IntGetProcAddress("glCompressedTextureImage2DEXT");
	if(!_funcptr_glCompressedTextureImage2DEXT) ++numFailed;
	_funcptr_glCompressedTextureImage1DEXT = (PFNGLCOMPRESSEDTEXTUREIMAGE1DEXTPROC)IntGetProcAddress("glCompressedTextureImage1DEXT");
	if(!_funcptr_glCompressedTextureImage1DEXT) ++numFailed;
	_funcptr_glCompressedTextureSubImage3DEXT = (PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXTPROC)IntGetProcAddress("glCompressedTextureSubImage3DEXT");
	if(!_funcptr_glCompressedTextureSubImage3DEXT) ++numFailed;
	_funcptr_glCompressedTextureSubImage2DEXT = (PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXTPROC)IntGetProcAddress("glCompressedTextureSubImage2DEXT");
	if(!_funcptr_glCompressedTextureSubImage2DEXT) ++numFailed;
	_funcptr_glCompressedTextureSubImage1DEXT = (PFNGLCOMPRESSEDTEXTURESUBIMAGE1DEXTPROC)IntGetProcAddress("glCompressedTextureSubImage1DEXT");
	if(!_funcptr_glCompressedTextureSubImage1DEXT) ++numFailed;
	_funcptr_glGetCompressedTextureImageEXT = (PFNGLGETCOMPRESSEDTEXTUREIMAGEEXTPROC)IntGetProcAddress("glGetCompressedTextureImageEXT");
	if(!_funcptr_glGetCompressedTextureImageEXT) ++numFailed;
	_funcptr_glCompressedMultiTexImage3DEXT = (PFNGLCOMPRESSEDMULTITEXIMAGE3DEXTPROC)IntGetProcAddress("glCompressedMultiTexImage3DEXT");
	if(!_funcptr_glCompressedMultiTexImage3DEXT) ++numFailed;
	_funcptr_glCompressedMultiTexImage2DEXT = (PFNGLCOMPRESSEDMULTITEXIMAGE2DEXTPROC)IntGetProcAddress("glCompressedMultiTexImage2DEXT");
	if(!_funcptr_glCompressedMultiTexImage2DEXT) ++numFailed;
	_funcptr_glCompressedMultiTexImage1DEXT = (PFNGLCOMPRESSEDMULTITEXIMAGE1DEXTPROC)IntGetProcAddress("glCompressedMultiTexImage1DEXT");
	if(!_funcptr_glCompressedMultiTexImage1DEXT) ++numFailed;
	_funcptr_glCompressedMultiTexSubImage3DEXT = (PFNGLCOMPRESSEDMULTITEXSUBIMAGE3DEXTPROC)IntGetProcAddress("glCompressedMultiTexSubImage3DEXT");
	if(!_funcptr_glCompressedMultiTexSubImage3DEXT) ++numFailed;
	_funcptr_glCompressedMultiTexSubImage2DEXT = (PFNGLCOMPRESSEDMULTITEXSUBIMAGE2DEXTPROC)IntGetProcAddress("glCompressedMultiTexSubImage2DEXT");
	if(!_funcptr_glCompressedMultiTexSubImage2DEXT) ++numFailed;
	_funcptr_glCompressedMultiTexSubImage1DEXT = (PFNGLCOMPRESSEDMULTITEXSUBIMAGE1DEXTPROC)IntGetProcAddress("glCompressedMultiTexSubImage1DEXT");
	if(!_funcptr_glCompressedMultiTexSubImage1DEXT) ++numFailed;
	_funcptr_glGetCompressedMultiTexImageEXT = (PFNGLGETCOMPRESSEDMULTITEXIMAGEEXTPROC)IntGetProcAddress("glGetCompressedMultiTexImageEXT");
	if(!_funcptr_glGetCompressedMultiTexImageEXT) ++numFailed;
	_funcptr_glNamedProgramStringEXT = (PFNGLNAMEDPROGRAMSTRINGEXTPROC)IntGetProcAddress("glNamedProgramStringEXT");
	if(!_funcptr_glNamedProgramStringEXT) ++numFailed;
	_funcptr_glNamedProgramLocalParameter4dEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETER4DEXTPROC)IntGetProcAddress("glNamedProgramLocalParameter4dEXT");
	if(!_funcptr_glNamedProgramLocalParameter4dEXT) ++numFailed;
	_funcptr_glNamedProgramLocalParameter4dvEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETER4DVEXTPROC)IntGetProcAddress("glNamedProgramLocalParameter4dvEXT");
	if(!_funcptr_glNamedProgramLocalParameter4dvEXT) ++numFailed;
	_funcptr_glNamedProgramLocalParameter4fEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETER4FEXTPROC)IntGetProcAddress("glNamedProgramLocalParameter4fEXT");
	if(!_funcptr_glNamedProgramLocalParameter4fEXT) ++numFailed;
	_funcptr_glNamedProgramLocalParameter4fvEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETER4FVEXTPROC)IntGetProcAddress("glNamedProgramLocalParameter4fvEXT");
	if(!_funcptr_glNamedProgramLocalParameter4fvEXT) ++numFailed;
	_funcptr_glGetNamedProgramLocalParameterdvEXT = (PFNGLGETNAMEDPROGRAMLOCALPARAMETERDVEXTPROC)IntGetProcAddress("glGetNamedProgramLocalParameterdvEXT");
	if(!_funcptr_glGetNamedProgramLocalParameterdvEXT) ++numFailed;
	_funcptr_glGetNamedProgramLocalParameterfvEXT = (PFNGLGETNAMEDPROGRAMLOCALPARAMETERFVEXTPROC)IntGetProcAddress("glGetNamedProgramLocalParameterfvEXT");
	if(!_funcptr_glGetNamedProgramLocalParameterfvEXT) ++numFailed;
	_funcptr_glGetNamedProgramivEXT = (PFNGLGETNAMEDPROGRAMIVEXTPROC)IntGetProcAddress("glGetNamedProgramivEXT");
	if(!_funcptr_glGetNamedProgramivEXT) ++numFailed;
	_funcptr_glGetNamedProgramStringEXT = (PFNGLGETNAMEDPROGRAMSTRINGEXTPROC)IntGetProcAddress("glGetNamedProgramStringEXT");
	if(!_funcptr_glGetNamedProgramStringEXT) ++numFailed;
	_funcptr_glNamedProgramLocalParameters4fvEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERS4FVEXTPROC)IntGetProcAddress("glNamedProgramLocalParameters4fvEXT");
	if(!_funcptr_glNamedProgramLocalParameters4fvEXT) ++numFailed;
	_funcptr_glNamedProgramLocalParameterI4iEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERI4IEXTPROC)IntGetProcAddress("glNamedProgramLocalParameterI4iEXT");
	if(!_funcptr_glNamedProgramLocalParameterI4iEXT) ++numFailed;
	_funcptr_glNamedProgramLocalParameterI4ivEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERI4IVEXTPROC)IntGetProcAddress("glNamedProgramLocalParameterI4ivEXT");
	if(!_funcptr_glNamedProgramLocalParameterI4ivEXT) ++numFailed;
	_funcptr_glNamedProgramLocalParametersI4ivEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERSI4IVEXTPROC)IntGetProcAddress("glNamedProgramLocalParametersI4ivEXT");
	if(!_funcptr_glNamedProgramLocalParametersI4ivEXT) ++numFailed;
	_funcptr_glNamedProgramLocalParameterI4uiEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIEXTPROC)IntGetProcAddress("glNamedProgramLocalParameterI4uiEXT");
	if(!_funcptr_glNamedProgramLocalParameterI4uiEXT) ++numFailed;
	_funcptr_glNamedProgramLocalParameterI4uivEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIVEXTPROC)IntGetProcAddress("glNamedProgramLocalParameterI4uivEXT");
	if(!_funcptr_glNamedProgramLocalParameterI4uivEXT) ++numFailed;
	_funcptr_glNamedProgramLocalParametersI4uivEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERSI4UIVEXTPROC)IntGetProcAddress("glNamedProgramLocalParametersI4uivEXT");
	if(!_funcptr_glNamedProgramLocalParametersI4uivEXT) ++numFailed;
	_funcptr_glGetNamedProgramLocalParameterIivEXT = (PFNGLGETNAMEDPROGRAMLOCALPARAMETERIIVEXTPROC)IntGetProcAddress("glGetNamedProgramLocalParameterIivEXT");
	if(!_funcptr_glGetNamedProgramLocalParameterIivEXT) ++numFailed;
	_funcptr_glGetNamedProgramLocalParameterIuivEXT = (PFNGLGETNAMEDPROGRAMLOCALPARAMETERIUIVEXTPROC)IntGetProcAddress("glGetNamedProgramLocalParameterIuivEXT");
	if(!_funcptr_glGetNamedProgramLocalParameterIuivEXT) ++numFailed;
	_funcptr_glTextureParameterIivEXT = (PFNGLTEXTUREPARAMETERIIVEXTPROC)IntGetProcAddress("glTextureParameterIivEXT");
	if(!_funcptr_glTextureParameterIivEXT) ++numFailed;
	_funcptr_glTextureParameterIuivEXT = (PFNGLTEXTUREPARAMETERIUIVEXTPROC)IntGetProcAddress("glTextureParameterIuivEXT");
	if(!_funcptr_glTextureParameterIuivEXT) ++numFailed;
	_funcptr_glGetTextureParameterIivEXT = (PFNGLGETTEXTUREPARAMETERIIVEXTPROC)IntGetProcAddress("glGetTextureParameterIivEXT");
	if(!_funcptr_glGetTextureParameterIivEXT) ++numFailed;
	_funcptr_glGetTextureParameterIuivEXT = (PFNGLGETTEXTUREPARAMETERIUIVEXTPROC)IntGetProcAddress("glGetTextureParameterIuivEXT");
	if(!_funcptr_glGetTextureParameterIuivEXT) ++numFailed;
	_funcptr_glMultiTexParameterIivEXT = (PFNGLMULTITEXPARAMETERIIVEXTPROC)IntGetProcAddress("glMultiTexParameterIivEXT");
	if(!_funcptr_glMultiTexParameterIivEXT) ++numFailed;
	_funcptr_glMultiTexParameterIuivEXT = (PFNGLMULTITEXPARAMETERIUIVEXTPROC)IntGetProcAddress("glMultiTexParameterIuivEXT");
	if(!_funcptr_glMultiTexParameterIuivEXT) ++numFailed;
	_funcptr_glGetMultiTexParameterIivEXT = (PFNGLGETMULTITEXPARAMETERIIVEXTPROC)IntGetProcAddress("glGetMultiTexParameterIivEXT");
	if(!_funcptr_glGetMultiTexParameterIivEXT) ++numFailed;
	_funcptr_glGetMultiTexParameterIuivEXT = (PFNGLGETMULTITEXPARAMETERIUIVEXTPROC)IntGetProcAddress("glGetMultiTexParameterIuivEXT");
	if(!_funcptr_glGetMultiTexParameterIuivEXT) ++numFailed;
	_funcptr_glProgramUniform1fEXT = (PFNGLPROGRAMUNIFORM1FEXTPROC)IntGetProcAddress("glProgramUniform1fEXT");
	if(!_funcptr_glProgramUniform1fEXT) ++numFailed;
	_funcptr_glProgramUniform2fEXT = (PFNGLPROGRAMUNIFORM2FEXTPROC)IntGetProcAddress("glProgramUniform2fEXT");
	if(!_funcptr_glProgramUniform2fEXT) ++numFailed;
	_funcptr_glProgramUniform3fEXT = (PFNGLPROGRAMUNIFORM3FEXTPROC)IntGetProcAddress("glProgramUniform3fEXT");
	if(!_funcptr_glProgramUniform3fEXT) ++numFailed;
	_funcptr_glProgramUniform4fEXT = (PFNGLPROGRAMUNIFORM4FEXTPROC)IntGetProcAddress("glProgramUniform4fEXT");
	if(!_funcptr_glProgramUniform4fEXT) ++numFailed;
	_funcptr_glProgramUniform1iEXT = (PFNGLPROGRAMUNIFORM1IEXTPROC)IntGetProcAddress("glProgramUniform1iEXT");
	if(!_funcptr_glProgramUniform1iEXT) ++numFailed;
	_funcptr_glProgramUniform2iEXT = (PFNGLPROGRAMUNIFORM2IEXTPROC)IntGetProcAddress("glProgramUniform2iEXT");
	if(!_funcptr_glProgramUniform2iEXT) ++numFailed;
	_funcptr_glProgramUniform3iEXT = (PFNGLPROGRAMUNIFORM3IEXTPROC)IntGetProcAddress("glProgramUniform3iEXT");
	if(!_funcptr_glProgramUniform3iEXT) ++numFailed;
	_funcptr_glProgramUniform4iEXT = (PFNGLPROGRAMUNIFORM4IEXTPROC)IntGetProcAddress("glProgramUniform4iEXT");
	if(!_funcptr_glProgramUniform4iEXT) ++numFailed;
	_funcptr_glProgramUniform1fvEXT = (PFNGLPROGRAMUNIFORM1FVEXTPROC)IntGetProcAddress("glProgramUniform1fvEXT");
	if(!_funcptr_glProgramUniform1fvEXT) ++numFailed;
	_funcptr_glProgramUniform2fvEXT = (PFNGLPROGRAMUNIFORM2FVEXTPROC)IntGetProcAddress("glProgramUniform2fvEXT");
	if(!_funcptr_glProgramUniform2fvEXT) ++numFailed;
	_funcptr_glProgramUniform3fvEXT = (PFNGLPROGRAMUNIFORM3FVEXTPROC)IntGetProcAddress("glProgramUniform3fvEXT");
	if(!_funcptr_glProgramUniform3fvEXT) ++numFailed;
	_funcptr_glProgramUniform4fvEXT = (PFNGLPROGRAMUNIFORM4FVEXTPROC)IntGetProcAddress("glProgramUniform4fvEXT");
	if(!_funcptr_glProgramUniform4fvEXT) ++numFailed;
	_funcptr_glProgramUniform1ivEXT = (PFNGLPROGRAMUNIFORM1IVEXTPROC)IntGetProcAddress("glProgramUniform1ivEXT");
	if(!_funcptr_glProgramUniform1ivEXT) ++numFailed;
	_funcptr_glProgramUniform2ivEXT = (PFNGLPROGRAMUNIFORM2IVEXTPROC)IntGetProcAddress("glProgramUniform2ivEXT");
	if(!_funcptr_glProgramUniform2ivEXT) ++numFailed;
	_funcptr_glProgramUniform3ivEXT = (PFNGLPROGRAMUNIFORM3IVEXTPROC)IntGetProcAddress("glProgramUniform3ivEXT");
	if(!_funcptr_glProgramUniform3ivEXT) ++numFailed;
	_funcptr_glProgramUniform4ivEXT = (PFNGLPROGRAMUNIFORM4IVEXTPROC)IntGetProcAddress("glProgramUniform4ivEXT");
	if(!_funcptr_glProgramUniform4ivEXT) ++numFailed;
	_funcptr_glProgramUniformMatrix2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC)IntGetProcAddress("glProgramUniformMatrix2fvEXT");
	if(!_funcptr_glProgramUniformMatrix2fvEXT) ++numFailed;
	_funcptr_glProgramUniformMatrix3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC)IntGetProcAddress("glProgramUniformMatrix3fvEXT");
	if(!_funcptr_glProgramUniformMatrix3fvEXT) ++numFailed;
	_funcptr_glProgramUniformMatrix4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC)IntGetProcAddress("glProgramUniformMatrix4fvEXT");
	if(!_funcptr_glProgramUniformMatrix4fvEXT) ++numFailed;
	_funcptr_glProgramUniformMatrix2x3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC)IntGetProcAddress("glProgramUniformMatrix2x3fvEXT");
	if(!_funcptr_glProgramUniformMatrix2x3fvEXT) ++numFailed;
	_funcptr_glProgramUniformMatrix3x2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC)IntGetProcAddress("glProgramUniformMatrix3x2fvEXT");
	if(!_funcptr_glProgramUniformMatrix3x2fvEXT) ++numFailed;
	_funcptr_glProgramUniformMatrix2x4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC)IntGetProcAddress("glProgramUniformMatrix2x4fvEXT");
	if(!_funcptr_glProgramUniformMatrix2x4fvEXT) ++numFailed;
	_funcptr_glProgramUniformMatrix4x2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC)IntGetProcAddress("glProgramUniformMatrix4x2fvEXT");
	if(!_funcptr_glProgramUniformMatrix4x2fvEXT) ++numFailed;
	_funcptr_glProgramUniformMatrix3x4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC)IntGetProcAddress("glProgramUniformMatrix3x4fvEXT");
	if(!_funcptr_glProgramUniformMatrix3x4fvEXT) ++numFailed;
	_funcptr_glProgramUniformMatrix4x3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC)IntGetProcAddress("glProgramUniformMatrix4x3fvEXT");
	if(!_funcptr_glProgramUniformMatrix4x3fvEXT) ++numFailed;
	_funcptr_glProgramUniform1uiEXT = (PFNGLPROGRAMUNIFORM1UIEXTPROC)IntGetProcAddress("glProgramUniform1uiEXT");
	if(!_funcptr_glProgramUniform1uiEXT) ++numFailed;
	_funcptr_glProgramUniform2uiEXT = (PFNGLPROGRAMUNIFORM2UIEXTPROC)IntGetProcAddress("glProgramUniform2uiEXT");
	if(!_funcptr_glProgramUniform2uiEXT) ++numFailed;
	_funcptr_glProgramUniform3uiEXT = (PFNGLPROGRAMUNIFORM3UIEXTPROC)IntGetProcAddress("glProgramUniform3uiEXT");
	if(!_funcptr_glProgramUniform3uiEXT) ++numFailed;
	_funcptr_glProgramUniform4uiEXT = (PFNGLPROGRAMUNIFORM4UIEXTPROC)IntGetProcAddress("glProgramUniform4uiEXT");
	if(!_funcptr_glProgramUniform4uiEXT) ++numFailed;
	_funcptr_glProgramUniform1uivEXT = (PFNGLPROGRAMUNIFORM1UIVEXTPROC)IntGetProcAddress("glProgramUniform1uivEXT");
	if(!_funcptr_glProgramUniform1uivEXT) ++numFailed;
	_funcptr_glProgramUniform2uivEXT = (PFNGLPROGRAMUNIFORM2UIVEXTPROC)IntGetProcAddress("glProgramUniform2uivEXT");
	if(!_funcptr_glProgramUniform2uivEXT) ++numFailed;
	_funcptr_glProgramUniform3uivEXT = (PFNGLPROGRAMUNIFORM3UIVEXTPROC)IntGetProcAddress("glProgramUniform3uivEXT");
	if(!_funcptr_glProgramUniform3uivEXT) ++numFailed;
	_funcptr_glProgramUniform4uivEXT = (PFNGLPROGRAMUNIFORM4UIVEXTPROC)IntGetProcAddress("glProgramUniform4uivEXT");
	if(!_funcptr_glProgramUniform4uivEXT) ++numFailed;
	_funcptr_glNamedBufferDataEXT = (PFNGLNAMEDBUFFERDATAEXTPROC)IntGetProcAddress("glNamedBufferDataEXT");
	if(!_funcptr_glNamedBufferDataEXT) ++numFailed;
	_funcptr_glNamedBufferSubDataEXT = (PFNGLNAMEDBUFFERSUBDATAEXTPROC)IntGetProcAddress("glNamedBufferSubDataEXT");
	if(!_funcptr_glNamedBufferSubDataEXT) ++numFailed;
	_funcptr_glMapNamedBufferEXT = (PFNGLMAPNAMEDBUFFEREXTPROC)IntGetProcAddress("glMapNamedBufferEXT");
	if(!_funcptr_glMapNamedBufferEXT) ++numFailed;
	_funcptr_glUnmapNamedBufferEXT = (PFNGLUNMAPNAMEDBUFFEREXTPROC)IntGetProcAddress("glUnmapNamedBufferEXT");
	if(!_funcptr_glUnmapNamedBufferEXT) ++numFailed;
	_funcptr_glMapNamedBufferRangeEXT = (PFNGLMAPNAMEDBUFFERRANGEEXTPROC)IntGetProcAddress("glMapNamedBufferRangeEXT");
	if(!_funcptr_glMapNamedBufferRangeEXT) ++numFailed;
	_funcptr_glFlushMappedNamedBufferRangeEXT = (PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEEXTPROC)IntGetProcAddress("glFlushMappedNamedBufferRangeEXT");
	if(!_funcptr_glFlushMappedNamedBufferRangeEXT) ++numFailed;
	_funcptr_glNamedCopyBufferSubDataEXT = (PFNGLNAMEDCOPYBUFFERSUBDATAEXTPROC)IntGetProcAddress("glNamedCopyBufferSubDataEXT");
	if(!_funcptr_glNamedCopyBufferSubDataEXT) ++numFailed;
	_funcptr_glGetNamedBufferParameterivEXT = (PFNGLGETNAMEDBUFFERPARAMETERIVEXTPROC)IntGetProcAddress("glGetNamedBufferParameterivEXT");
	if(!_funcptr_glGetNamedBufferParameterivEXT) ++numFailed;
	_funcptr_glGetNamedBufferPointervEXT = (PFNGLGETNAMEDBUFFERPOINTERVEXTPROC)IntGetProcAddress("glGetNamedBufferPointervEXT");
	if(!_funcptr_glGetNamedBufferPointervEXT) ++numFailed;
	_funcptr_glGetNamedBufferSubDataEXT = (PFNGLGETNAMEDBUFFERSUBDATAEXTPROC)IntGetProcAddress("glGetNamedBufferSubDataEXT");
	if(!_funcptr_glGetNamedBufferSubDataEXT) ++numFailed;
	_funcptr_glTextureBufferEXT = (PFNGLTEXTUREBUFFEREXTPROC)IntGetProcAddress("glTextureBufferEXT");
	if(!_funcptr_glTextureBufferEXT) ++numFailed;
	_funcptr_glMultiTexBufferEXT = (PFNGLMULTITEXBUFFEREXTPROC)IntGetProcAddress("glMultiTexBufferEXT");
	if(!_funcptr_glMultiTexBufferEXT) ++numFailed;
	_funcptr_glNamedRenderbufferStorageEXT = (PFNGLNAMEDRENDERBUFFERSTORAGEEXTPROC)IntGetProcAddress("glNamedRenderbufferStorageEXT");
	if(!_funcptr_glNamedRenderbufferStorageEXT) ++numFailed;
	_funcptr_glGetNamedRenderbufferParameterivEXT = (PFNGLGETNAMEDRENDERBUFFERPARAMETERIVEXTPROC)IntGetProcAddress("glGetNamedRenderbufferParameterivEXT");
	if(!_funcptr_glGetNamedRenderbufferParameterivEXT) ++numFailed;
	_funcptr_glCheckNamedFramebufferStatusEXT = (PFNGLCHECKNAMEDFRAMEBUFFERSTATUSEXTPROC)IntGetProcAddress("glCheckNamedFramebufferStatusEXT");
	if(!_funcptr_glCheckNamedFramebufferStatusEXT) ++numFailed;
	_funcptr_glNamedFramebufferTexture1DEXT = (PFNGLNAMEDFRAMEBUFFERTEXTURE1DEXTPROC)IntGetProcAddress("glNamedFramebufferTexture1DEXT");
	if(!_funcptr_glNamedFramebufferTexture1DEXT) ++numFailed;
	_funcptr_glNamedFramebufferTexture2DEXT = (PFNGLNAMEDFRAMEBUFFERTEXTURE2DEXTPROC)IntGetProcAddress("glNamedFramebufferTexture2DEXT");
	if(!_funcptr_glNamedFramebufferTexture2DEXT) ++numFailed;
	_funcptr_glNamedFramebufferTexture3DEXT = (PFNGLNAMEDFRAMEBUFFERTEXTURE3DEXTPROC)IntGetProcAddress("glNamedFramebufferTexture3DEXT");
	if(!_funcptr_glNamedFramebufferTexture3DEXT) ++numFailed;
	_funcptr_glNamedFramebufferRenderbufferEXT = (PFNGLNAMEDFRAMEBUFFERRENDERBUFFEREXTPROC)IntGetProcAddress("glNamedFramebufferRenderbufferEXT");
	if(!_funcptr_glNamedFramebufferRenderbufferEXT) ++numFailed;
	_funcptr_glGetNamedFramebufferAttachmentParameterivEXT = (PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)IntGetProcAddress("glGetNamedFramebufferAttachmentParameterivEXT");
	if(!_funcptr_glGetNamedFramebufferAttachmentParameterivEXT) ++numFailed;
	_funcptr_glGenerateTextureMipmapEXT = (PFNGLGENERATETEXTUREMIPMAPEXTPROC)IntGetProcAddress("glGenerateTextureMipmapEXT");
	if(!_funcptr_glGenerateTextureMipmapEXT) ++numFailed;
	_funcptr_glGenerateMultiTexMipmapEXT = (PFNGLGENERATEMULTITEXMIPMAPEXTPROC)IntGetProcAddress("glGenerateMultiTexMipmapEXT");
	if(!_funcptr_glGenerateMultiTexMipmapEXT) ++numFailed;
	_funcptr_glFramebufferDrawBufferEXT = (PFNGLFRAMEBUFFERDRAWBUFFEREXTPROC)IntGetProcAddress("glFramebufferDrawBufferEXT");
	if(!_funcptr_glFramebufferDrawBufferEXT) ++numFailed;
	_funcptr_glFramebufferDrawBuffersEXT = (PFNGLFRAMEBUFFERDRAWBUFFERSEXTPROC)IntGetProcAddress("glFramebufferDrawBuffersEXT");
	if(!_funcptr_glFramebufferDrawBuffersEXT) ++numFailed;
	_funcptr_glFramebufferReadBufferEXT = (PFNGLFRAMEBUFFERREADBUFFEREXTPROC)IntGetProcAddress("glFramebufferReadBufferEXT");
	if(!_funcptr_glFramebufferReadBufferEXT) ++numFailed;
	_funcptr_glGetFramebufferParameterivEXT = (PFNGLGETFRAMEBUFFERPARAMETERIVEXTPROC)IntGetProcAddress("glGetFramebufferParameterivEXT");
	if(!_funcptr_glGetFramebufferParameterivEXT) ++numFailed;
	_funcptr_glNamedRenderbufferStorageMultisampleEXT = (PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)IntGetProcAddress("glNamedRenderbufferStorageMultisampleEXT");
	if(!_funcptr_glNamedRenderbufferStorageMultisampleEXT) ++numFailed;
	_funcptr_glNamedRenderbufferStorageMultisampleCoverageEXT = (PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLECOVERAGEEXTPROC)IntGetProcAddress("glNamedRenderbufferStorageMultisampleCoverageEXT");
	if(!_funcptr_glNamedRenderbufferStorageMultisampleCoverageEXT) ++numFailed;
	_funcptr_glNamedFramebufferTextureEXT = (PFNGLNAMEDFRAMEBUFFERTEXTUREEXTPROC)IntGetProcAddress("glNamedFramebufferTextureEXT");
	if(!_funcptr_glNamedFramebufferTextureEXT) ++numFailed;
	_funcptr_glNamedFramebufferTextureLayerEXT = (PFNGLNAMEDFRAMEBUFFERTEXTURELAYEREXTPROC)IntGetProcAddress("glNamedFramebufferTextureLayerEXT");
	if(!_funcptr_glNamedFramebufferTextureLayerEXT) ++numFailed;
	_funcptr_glNamedFramebufferTextureFaceEXT = (PFNGLNAMEDFRAMEBUFFERTEXTUREFACEEXTPROC)IntGetProcAddress("glNamedFramebufferTextureFaceEXT");
	if(!_funcptr_glNamedFramebufferTextureFaceEXT) ++numFailed;
	_funcptr_glTextureRenderbufferEXT = (PFNGLTEXTURERENDERBUFFEREXTPROC)IntGetProcAddress("glTextureRenderbufferEXT");
	if(!_funcptr_glTextureRenderbufferEXT) ++numFailed;
	_funcptr_glMultiTexRenderbufferEXT = (PFNGLMULTITEXRENDERBUFFEREXTPROC)IntGetProcAddress("glMultiTexRenderbufferEXT");
	if(!_funcptr_glMultiTexRenderbufferEXT) ++numFailed;
	_funcptr_glProgramUniform1dEXT = (PFNGLPROGRAMUNIFORM1DEXTPROC)IntGetProcAddress("glProgramUniform1dEXT");
	if(!_funcptr_glProgramUniform1dEXT) ++numFailed;
	_funcptr_glProgramUniform2dEXT = (PFNGLPROGRAMUNIFORM2DEXTPROC)IntGetProcAddress("glProgramUniform2dEXT");
	if(!_funcptr_glProgramUniform2dEXT) ++numFailed;
	_funcptr_glProgramUniform3dEXT = (PFNGLPROGRAMUNIFORM3DEXTPROC)IntGetProcAddress("glProgramUniform3dEXT");
	if(!_funcptr_glProgramUniform3dEXT) ++numFailed;
	_funcptr_glProgramUniform4dEXT = (PFNGLPROGRAMUNIFORM4DEXTPROC)IntGetProcAddress("glProgramUniform4dEXT");
	if(!_funcptr_glProgramUniform4dEXT) ++numFailed;
	_funcptr_glProgramUniform1dvEXT = (PFNGLPROGRAMUNIFORM1DVEXTPROC)IntGetProcAddress("glProgramUniform1dvEXT");
	if(!_funcptr_glProgramUniform1dvEXT) ++numFailed;
	_funcptr_glProgramUniform2dvEXT = (PFNGLPROGRAMUNIFORM2DVEXTPROC)IntGetProcAddress("glProgramUniform2dvEXT");
	if(!_funcptr_glProgramUniform2dvEXT) ++numFailed;
	_funcptr_glProgramUniform3dvEXT = (PFNGLPROGRAMUNIFORM3DVEXTPROC)IntGetProcAddress("glProgramUniform3dvEXT");
	if(!_funcptr_glProgramUniform3dvEXT) ++numFailed;
	_funcptr_glProgramUniform4dvEXT = (PFNGLPROGRAMUNIFORM4DVEXTPROC)IntGetProcAddress("glProgramUniform4dvEXT");
	if(!_funcptr_glProgramUniform4dvEXT) ++numFailed;
	_funcptr_glProgramUniformMatrix2dvEXT = (PFNGLPROGRAMUNIFORMMATRIX2DVEXTPROC)IntGetProcAddress("glProgramUniformMatrix2dvEXT");
	if(!_funcptr_glProgramUniformMatrix2dvEXT) ++numFailed;
	_funcptr_glProgramUniformMatrix3dvEXT = (PFNGLPROGRAMUNIFORMMATRIX3DVEXTPROC)IntGetProcAddress("glProgramUniformMatrix3dvEXT");
	if(!_funcptr_glProgramUniformMatrix3dvEXT) ++numFailed;
	_funcptr_glProgramUniformMatrix4dvEXT = (PFNGLPROGRAMUNIFORMMATRIX4DVEXTPROC)IntGetProcAddress("glProgramUniformMatrix4dvEXT");
	if(!_funcptr_glProgramUniformMatrix4dvEXT) ++numFailed;
	_funcptr_glProgramUniformMatrix2x3dvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X3DVEXTPROC)IntGetProcAddress("glProgramUniformMatrix2x3dvEXT");
	if(!_funcptr_glProgramUniformMatrix2x3dvEXT) ++numFailed;
	_funcptr_glProgramUniformMatrix2x4dvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X4DVEXTPROC)IntGetProcAddress("glProgramUniformMatrix2x4dvEXT");
	if(!_funcptr_glProgramUniformMatrix2x4dvEXT) ++numFailed;
	_funcptr_glProgramUniformMatrix3x2dvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X2DVEXTPROC)IntGetProcAddress("glProgramUniformMatrix3x2dvEXT");
	if(!_funcptr_glProgramUniformMatrix3x2dvEXT) ++numFailed;
	_funcptr_glProgramUniformMatrix3x4dvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X4DVEXTPROC)IntGetProcAddress("glProgramUniformMatrix3x4dvEXT");
	if(!_funcptr_glProgramUniformMatrix3x4dvEXT) ++numFailed;
	_funcptr_glProgramUniformMatrix4x2dvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X2DVEXTPROC)IntGetProcAddress("glProgramUniformMatrix4x2dvEXT");
	if(!_funcptr_glProgramUniformMatrix4x2dvEXT) ++numFailed;
	_funcptr_glProgramUniformMatrix4x3dvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X3DVEXTPROC)IntGetProcAddress("glProgramUniformMatrix4x3dvEXT");
	if(!_funcptr_glProgramUniformMatrix4x3dvEXT) ++numFailed;
	_funcptr_glEnableVertexArrayAttribEXT = (PFNGLENABLEVERTEXARRAYATTRIBEXTPROC)IntGetProcAddress("glEnableVertexArrayAttribEXT");
	if(!_funcptr_glEnableVertexArrayAttribEXT) ++numFailed;
	_funcptr_glDisableVertexArrayAttribEXT = (PFNGLDISABLEVERTEXARRAYATTRIBEXTPROC)IntGetProcAddress("glDisableVertexArrayAttribEXT");
	if(!_funcptr_glDisableVertexArrayAttribEXT) ++numFailed;
	_funcptr_glEnableVertexArrayEXT = (PFNGLENABLEVERTEXARRAYEXTPROC)IntGetProcAddress("glEnableVertexArrayEXT");
	if(!_funcptr_glEnableVertexArrayEXT) ++numFailed;
	_funcptr_glDisableVertexArrayEXT = (PFNGLDISABLEVERTEXARRAYEXTPROC)IntGetProcAddress("glDisableVertexArrayEXT");
	if(!_funcptr_glDisableVertexArrayEXT) ++numFailed;
	_funcptr_glVertexArrayColorOffsetEXT = (PFNGLVERTEXARRAYCOLOROFFSETEXTPROC)IntGetProcAddress("glVertexArrayColorOffsetEXT");
	if(!_funcptr_glVertexArrayColorOffsetEXT) ++numFailed;
	_funcptr_glVertexArrayEdgeFlagOffsetEXT = (PFNGLVERTEXARRAYEDGEFLAGOFFSETEXTPROC)IntGetProcAddress("glVertexArrayEdgeFlagOffsetEXT");
	if(!_funcptr_glVertexArrayEdgeFlagOffsetEXT) ++numFailed;
	_funcptr_glVertexArrayFogCoordOffsetEXT = (PFNGLVERTEXARRAYFOGCOORDOFFSETEXTPROC)IntGetProcAddress("glVertexArrayFogCoordOffsetEXT");
	if(!_funcptr_glVertexArrayFogCoordOffsetEXT) ++numFailed;
	_funcptr_glVertexArrayIndexOffsetEXT = (PFNGLVERTEXARRAYINDEXOFFSETEXTPROC)IntGetProcAddress("glVertexArrayIndexOffsetEXT");
	if(!_funcptr_glVertexArrayIndexOffsetEXT) ++numFailed;
	_funcptr_glVertexArrayMultiTexCoordOffsetEXT = (PFNGLVERTEXARRAYMULTITEXCOORDOFFSETEXTPROC)IntGetProcAddress("glVertexArrayMultiTexCoordOffsetEXT");
	if(!_funcptr_glVertexArrayMultiTexCoordOffsetEXT) ++numFailed;
	_funcptr_glVertexArrayNormalOffsetEXT = (PFNGLVERTEXARRAYNORMALOFFSETEXTPROC)IntGetProcAddress("glVertexArrayNormalOffsetEXT");
	if(!_funcptr_glVertexArrayNormalOffsetEXT) ++numFailed;
	_funcptr_glVertexArraySecondaryColorOffsetEXT = (PFNGLVERTEXARRAYSECONDARYCOLOROFFSETEXTPROC)IntGetProcAddress("glVertexArraySecondaryColorOffsetEXT");
	if(!_funcptr_glVertexArraySecondaryColorOffsetEXT) ++numFailed;
	_funcptr_glVertexArrayTexCoordOffsetEXT = (PFNGLVERTEXARRAYTEXCOORDOFFSETEXTPROC)IntGetProcAddress("glVertexArrayTexCoordOffsetEXT");
	if(!_funcptr_glVertexArrayTexCoordOffsetEXT) ++numFailed;
	_funcptr_glVertexArrayVertexOffsetEXT = (PFNGLVERTEXARRAYVERTEXOFFSETEXTPROC)IntGetProcAddress("glVertexArrayVertexOffsetEXT");
	if(!_funcptr_glVertexArrayVertexOffsetEXT) ++numFailed;
	_funcptr_glVertexArrayVertexAttribIOffsetEXT = (PFNGLVERTEXARRAYVERTEXATTRIBIOFFSETEXTPROC)IntGetProcAddress("glVertexArrayVertexAttribIOffsetEXT");
	if(!_funcptr_glVertexArrayVertexAttribIOffsetEXT) ++numFailed;
	_funcptr_glVertexArrayVertexAttribOffsetEXT = (PFNGLVERTEXARRAYVERTEXATTRIBOFFSETEXTPROC)IntGetProcAddress("glVertexArrayVertexAttribOffsetEXT");
	if(!_funcptr_glVertexArrayVertexAttribOffsetEXT) ++numFailed;
	_funcptr_glGetVertexArrayIntegervEXT = (PFNGLGETVERTEXARRAYINTEGERVEXTPROC)IntGetProcAddress("glGetVertexArrayIntegervEXT");
	if(!_funcptr_glGetVertexArrayIntegervEXT) ++numFailed;
	_funcptr_glGetVertexArrayPointervEXT = (PFNGLGETVERTEXARRAYPOINTERVEXTPROC)IntGetProcAddress("glGetVertexArrayPointervEXT");
	if(!_funcptr_glGetVertexArrayPointervEXT) ++numFailed;
	_funcptr_glGetVertexArrayIntegeri_vEXT = (PFNGLGETVERTEXARRAYINTEGERI_VEXTPROC)IntGetProcAddress("glGetVertexArrayIntegeri_vEXT");
	if(!_funcptr_glGetVertexArrayIntegeri_vEXT) ++numFailed;
	_funcptr_glGetVertexArrayPointeri_vEXT = (PFNGLGETVERTEXARRAYPOINTERI_VEXTPROC)IntGetProcAddress("glGetVertexArrayPointeri_vEXT");
	if(!_funcptr_glGetVertexArrayPointeri_vEXT) ++numFailed;
	return numFailed;
}

PFNGLCOLORMASKINDEXEDEXTPROC _funcptr_glColorMaskIndexedEXT = NULL;
PFNGLGETBOOLEANINDEXEDVEXTPROC _funcptr_glGetBooleanIndexedvEXT = NULL;
PFNGLGETINTEGERINDEXEDVEXTPROC _funcptr_glGetIntegerIndexedvEXT = NULL;
PFNGLENABLEINDEXEDEXTPROC _funcptr_glEnableIndexedEXT = NULL;
PFNGLDISABLEINDEXEDEXTPROC _funcptr_glDisableIndexedEXT = NULL;
PFNGLISENABLEDINDEXEDEXTPROC _funcptr_glIsEnabledIndexedEXT = NULL;

static int LoadExt_EXT_draw_buffers2()
{
	int numFailed = 0;
	_funcptr_glColorMaskIndexedEXT = (PFNGLCOLORMASKINDEXEDEXTPROC)IntGetProcAddress("glColorMaskIndexedEXT");
	if(!_funcptr_glColorMaskIndexedEXT) ++numFailed;
	_funcptr_glGetBooleanIndexedvEXT = (PFNGLGETBOOLEANINDEXEDVEXTPROC)IntGetProcAddress("glGetBooleanIndexedvEXT");
	if(!_funcptr_glGetBooleanIndexedvEXT) ++numFailed;
	_funcptr_glGetIntegerIndexedvEXT = (PFNGLGETINTEGERINDEXEDVEXTPROC)IntGetProcAddress("glGetIntegerIndexedvEXT");
	if(!_funcptr_glGetIntegerIndexedvEXT) ++numFailed;
	_funcptr_glEnableIndexedEXT = (PFNGLENABLEINDEXEDEXTPROC)IntGetProcAddress("glEnableIndexedEXT");
	if(!_funcptr_glEnableIndexedEXT) ++numFailed;
	_funcptr_glDisableIndexedEXT = (PFNGLDISABLEINDEXEDEXTPROC)IntGetProcAddress("glDisableIndexedEXT");
	if(!_funcptr_glDisableIndexedEXT) ++numFailed;
	_funcptr_glIsEnabledIndexedEXT = (PFNGLISENABLEDINDEXEDEXTPROC)IntGetProcAddress("glIsEnabledIndexedEXT");
	if(!_funcptr_glIsEnabledIndexedEXT) ++numFailed;
	return numFailed;
}

PFNGLDRAWARRAYSINSTANCEDEXTPROC _funcptr_glDrawArraysInstancedEXT = NULL;
PFNGLDRAWELEMENTSINSTANCEDEXTPROC _funcptr_glDrawElementsInstancedEXT = NULL;

static int LoadExt_EXT_draw_instanced()
{
	int numFailed = 0;
	_funcptr_glDrawArraysInstancedEXT = (PFNGLDRAWARRAYSINSTANCEDEXTPROC)IntGetProcAddress("glDrawArraysInstancedEXT");
	if(!_funcptr_glDrawArraysInstancedEXT) ++numFailed;
	_funcptr_glDrawElementsInstancedEXT = (PFNGLDRAWELEMENTSINSTANCEDEXTPROC)IntGetProcAddress("glDrawElementsInstancedEXT");
	if(!_funcptr_glDrawElementsInstancedEXT) ++numFailed;
	return numFailed;
}

PFNGLDRAWRANGEELEMENTSEXTPROC _funcptr_glDrawRangeElementsEXT = NULL;

static int LoadExt_EXT_draw_range_elements()
{
	int numFailed = 0;
	_funcptr_glDrawRangeElementsEXT = (PFNGLDRAWRANGEELEMENTSEXTPROC)IntGetProcAddress("glDrawRangeElementsEXT");
	if(!_funcptr_glDrawRangeElementsEXT) ++numFailed;
	return numFailed;
}

PFNGLFOGCOORDFEXTPROC _funcptr_glFogCoordfEXT = NULL;
PFNGLFOGCOORDFVEXTPROC _funcptr_glFogCoordfvEXT = NULL;
PFNGLFOGCOORDDEXTPROC _funcptr_glFogCoorddEXT = NULL;
PFNGLFOGCOORDDVEXTPROC _funcptr_glFogCoorddvEXT = NULL;
PFNGLFOGCOORDPOINTEREXTPROC _funcptr_glFogCoordPointerEXT = NULL;

static int LoadExt_EXT_fog_coord()
{
	int numFailed = 0;
	_funcptr_glFogCoordfEXT = (PFNGLFOGCOORDFEXTPROC)IntGetProcAddress("glFogCoordfEXT");
	if(!_funcptr_glFogCoordfEXT) ++numFailed;
	_funcptr_glFogCoordfvEXT = (PFNGLFOGCOORDFVEXTPROC)IntGetProcAddress("glFogCoordfvEXT");
	if(!_funcptr_glFogCoordfvEXT) ++numFailed;
	_funcptr_glFogCoorddEXT = (PFNGLFOGCOORDDEXTPROC)IntGetProcAddress("glFogCoorddEXT");
	if(!_funcptr_glFogCoorddEXT) ++numFailed;
	_funcptr_glFogCoorddvEXT = (PFNGLFOGCOORDDVEXTPROC)IntGetProcAddress("glFogCoorddvEXT");
	if(!_funcptr_glFogCoorddvEXT) ++numFailed;
	_funcptr_glFogCoordPointerEXT = (PFNGLFOGCOORDPOINTEREXTPROC)IntGetProcAddress("glFogCoordPointerEXT");
	if(!_funcptr_glFogCoordPointerEXT) ++numFailed;
	return numFailed;
}

PFNGLBLITFRAMEBUFFEREXTPROC _funcptr_glBlitFramebufferEXT = NULL;

static int LoadExt_EXT_framebuffer_blit()
{
	int numFailed = 0;
	_funcptr_glBlitFramebufferEXT = (PFNGLBLITFRAMEBUFFEREXTPROC)IntGetProcAddress("glBlitFramebufferEXT");
	if(!_funcptr_glBlitFramebufferEXT) ++numFailed;
	return numFailed;
}

PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC _funcptr_glRenderbufferStorageMultisampleEXT = NULL;

static int LoadExt_EXT_framebuffer_multisample()
{
	int numFailed = 0;
	_funcptr_glRenderbufferStorageMultisampleEXT = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)IntGetProcAddress("glRenderbufferStorageMultisampleEXT");
	if(!_funcptr_glRenderbufferStorageMultisampleEXT) ++numFailed;
	return numFailed;
}

PFNGLISRENDERBUFFEREXTPROC _funcptr_glIsRenderbufferEXT = NULL;
PFNGLBINDRENDERBUFFEREXTPROC _funcptr_glBindRenderbufferEXT = NULL;
PFNGLDELETERENDERBUFFERSEXTPROC _funcptr_glDeleteRenderbuffersEXT = NULL;
PFNGLGENRENDERBUFFERSEXTPROC _funcptr_glGenRenderbuffersEXT = NULL;
PFNGLRENDERBUFFERSTORAGEEXTPROC _funcptr_glRenderbufferStorageEXT = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC _funcptr_glGetRenderbufferParameterivEXT = NULL;
PFNGLISFRAMEBUFFEREXTPROC _funcptr_glIsFramebufferEXT = NULL;
PFNGLBINDFRAMEBUFFEREXTPROC _funcptr_glBindFramebufferEXT = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC _funcptr_glDeleteFramebuffersEXT = NULL;
PFNGLGENFRAMEBUFFERSEXTPROC _funcptr_glGenFramebuffersEXT = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC _funcptr_glCheckFramebufferStatusEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC _funcptr_glFramebufferTexture1DEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC _funcptr_glFramebufferTexture2DEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC _funcptr_glFramebufferTexture3DEXT = NULL;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC _funcptr_glFramebufferRenderbufferEXT = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC _funcptr_glGetFramebufferAttachmentParameterivEXT = NULL;
PFNGLGENERATEMIPMAPEXTPROC _funcptr_glGenerateMipmapEXT = NULL;

static int LoadExt_EXT_framebuffer_object()
{
	int numFailed = 0;
	_funcptr_glIsRenderbufferEXT = (PFNGLISRENDERBUFFEREXTPROC)IntGetProcAddress("glIsRenderbufferEXT");
	if(!_funcptr_glIsRenderbufferEXT) ++numFailed;
	_funcptr_glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)IntGetProcAddress("glBindRenderbufferEXT");
	if(!_funcptr_glBindRenderbufferEXT) ++numFailed;
	_funcptr_glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)IntGetProcAddress("glDeleteRenderbuffersEXT");
	if(!_funcptr_glDeleteRenderbuffersEXT) ++numFailed;
	_funcptr_glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)IntGetProcAddress("glGenRenderbuffersEXT");
	if(!_funcptr_glGenRenderbuffersEXT) ++numFailed;
	_funcptr_glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)IntGetProcAddress("glRenderbufferStorageEXT");
	if(!_funcptr_glRenderbufferStorageEXT) ++numFailed;
	_funcptr_glGetRenderbufferParameterivEXT = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)IntGetProcAddress("glGetRenderbufferParameterivEXT");
	if(!_funcptr_glGetRenderbufferParameterivEXT) ++numFailed;
	_funcptr_glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC)IntGetProcAddress("glIsFramebufferEXT");
	if(!_funcptr_glIsFramebufferEXT) ++numFailed;
	_funcptr_glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)IntGetProcAddress("glBindFramebufferEXT");
	if(!_funcptr_glBindFramebufferEXT) ++numFailed;
	_funcptr_glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)IntGetProcAddress("glDeleteFramebuffersEXT");
	if(!_funcptr_glDeleteFramebuffersEXT) ++numFailed;
	_funcptr_glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)IntGetProcAddress("glGenFramebuffersEXT");
	if(!_funcptr_glGenFramebuffersEXT) ++numFailed;
	_funcptr_glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)IntGetProcAddress("glCheckFramebufferStatusEXT");
	if(!_funcptr_glCheckFramebufferStatusEXT) ++numFailed;
	_funcptr_glFramebufferTexture1DEXT = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)IntGetProcAddress("glFramebufferTexture1DEXT");
	if(!_funcptr_glFramebufferTexture1DEXT) ++numFailed;
	_funcptr_glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)IntGetProcAddress("glFramebufferTexture2DEXT");
	if(!_funcptr_glFramebufferTexture2DEXT) ++numFailed;
	_funcptr_glFramebufferTexture3DEXT = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)IntGetProcAddress("glFramebufferTexture3DEXT");
	if(!_funcptr_glFramebufferTexture3DEXT) ++numFailed;
	_funcptr_glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)IntGetProcAddress("glFramebufferRenderbufferEXT");
	if(!_funcptr_glFramebufferRenderbufferEXT) ++numFailed;
	_funcptr_glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)IntGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
	if(!_funcptr_glGetFramebufferAttachmentParameterivEXT) ++numFailed;
	_funcptr_glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC)IntGetProcAddress("glGenerateMipmapEXT");
	if(!_funcptr_glGenerateMipmapEXT) ++numFailed;
	return numFailed;
}

PFNGLPROGRAMPARAMETERIEXTPROC _funcptr_glProgramParameteriEXT = NULL;

static int LoadExt_EXT_geometry_shader4()
{
	int numFailed = 0;
	_funcptr_glProgramParameteriEXT = (PFNGLPROGRAMPARAMETERIEXTPROC)IntGetProcAddress("glProgramParameteriEXT");
	if(!_funcptr_glProgramParameteriEXT) ++numFailed;
	return numFailed;
}

PFNGLPROGRAMENVPARAMETERS4FVEXTPROC _funcptr_glProgramEnvParameters4fvEXT = NULL;
PFNGLPROGRAMLOCALPARAMETERS4FVEXTPROC _funcptr_glProgramLocalParameters4fvEXT = NULL;

static int LoadExt_EXT_gpu_program_parameters()
{
	int numFailed = 0;
	_funcptr_glProgramEnvParameters4fvEXT = (PFNGLPROGRAMENVPARAMETERS4FVEXTPROC)IntGetProcAddress("glProgramEnvParameters4fvEXT");
	if(!_funcptr_glProgramEnvParameters4fvEXT) ++numFailed;
	_funcptr_glProgramLocalParameters4fvEXT = (PFNGLPROGRAMLOCALPARAMETERS4FVEXTPROC)IntGetProcAddress("glProgramLocalParameters4fvEXT");
	if(!_funcptr_glProgramLocalParameters4fvEXT) ++numFailed;
	return numFailed;
}

PFNGLGETUNIFORMUIVEXTPROC _funcptr_glGetUniformuivEXT = NULL;
PFNGLBINDFRAGDATALOCATIONEXTPROC _funcptr_glBindFragDataLocationEXT = NULL;
PFNGLGETFRAGDATALOCATIONEXTPROC _funcptr_glGetFragDataLocationEXT = NULL;
PFNGLUNIFORM1UIEXTPROC _funcptr_glUniform1uiEXT = NULL;
PFNGLUNIFORM2UIEXTPROC _funcptr_glUniform2uiEXT = NULL;
PFNGLUNIFORM3UIEXTPROC _funcptr_glUniform3uiEXT = NULL;
PFNGLUNIFORM4UIEXTPROC _funcptr_glUniform4uiEXT = NULL;
PFNGLUNIFORM1UIVEXTPROC _funcptr_glUniform1uivEXT = NULL;
PFNGLUNIFORM2UIVEXTPROC _funcptr_glUniform2uivEXT = NULL;
PFNGLUNIFORM3UIVEXTPROC _funcptr_glUniform3uivEXT = NULL;
PFNGLUNIFORM4UIVEXTPROC _funcptr_glUniform4uivEXT = NULL;

static int LoadExt_EXT_gpu_shader4()
{
	int numFailed = 0;
	_funcptr_glGetUniformuivEXT = (PFNGLGETUNIFORMUIVEXTPROC)IntGetProcAddress("glGetUniformuivEXT");
	if(!_funcptr_glGetUniformuivEXT) ++numFailed;
	_funcptr_glBindFragDataLocationEXT = (PFNGLBINDFRAGDATALOCATIONEXTPROC)IntGetProcAddress("glBindFragDataLocationEXT");
	if(!_funcptr_glBindFragDataLocationEXT) ++numFailed;
	_funcptr_glGetFragDataLocationEXT = (PFNGLGETFRAGDATALOCATIONEXTPROC)IntGetProcAddress("glGetFragDataLocationEXT");
	if(!_funcptr_glGetFragDataLocationEXT) ++numFailed;
	_funcptr_glUniform1uiEXT = (PFNGLUNIFORM1UIEXTPROC)IntGetProcAddress("glUniform1uiEXT");
	if(!_funcptr_glUniform1uiEXT) ++numFailed;
	_funcptr_glUniform2uiEXT = (PFNGLUNIFORM2UIEXTPROC)IntGetProcAddress("glUniform2uiEXT");
	if(!_funcptr_glUniform2uiEXT) ++numFailed;
	_funcptr_glUniform3uiEXT = (PFNGLUNIFORM3UIEXTPROC)IntGetProcAddress("glUniform3uiEXT");
	if(!_funcptr_glUniform3uiEXT) ++numFailed;
	_funcptr_glUniform4uiEXT = (PFNGLUNIFORM4UIEXTPROC)IntGetProcAddress("glUniform4uiEXT");
	if(!_funcptr_glUniform4uiEXT) ++numFailed;
	_funcptr_glUniform1uivEXT = (PFNGLUNIFORM1UIVEXTPROC)IntGetProcAddress("glUniform1uivEXT");
	if(!_funcptr_glUniform1uivEXT) ++numFailed;
	_funcptr_glUniform2uivEXT = (PFNGLUNIFORM2UIVEXTPROC)IntGetProcAddress("glUniform2uivEXT");
	if(!_funcptr_glUniform2uivEXT) ++numFailed;
	_funcptr_glUniform3uivEXT = (PFNGLUNIFORM3UIVEXTPROC)IntGetProcAddress("glUniform3uivEXT");
	if(!_funcptr_glUniform3uivEXT) ++numFailed;
	_funcptr_glUniform4uivEXT = (PFNGLUNIFORM4UIVEXTPROC)IntGetProcAddress("glUniform4uivEXT");
	if(!_funcptr_glUniform4uivEXT) ++numFailed;
	return numFailed;
}

PFNGLGETHISTOGRAMEXTPROC _funcptr_glGetHistogramEXT = NULL;
PFNGLGETHISTOGRAMPARAMETERFVEXTPROC _funcptr_glGetHistogramParameterfvEXT = NULL;
PFNGLGETHISTOGRAMPARAMETERIVEXTPROC _funcptr_glGetHistogramParameterivEXT = NULL;
PFNGLGETMINMAXEXTPROC _funcptr_glGetMinmaxEXT = NULL;
PFNGLGETMINMAXPARAMETERFVEXTPROC _funcptr_glGetMinmaxParameterfvEXT = NULL;
PFNGLGETMINMAXPARAMETERIVEXTPROC _funcptr_glGetMinmaxParameterivEXT = NULL;
PFNGLHISTOGRAMEXTPROC _funcptr_glHistogramEXT = NULL;
PFNGLMINMAXEXTPROC _funcptr_glMinmaxEXT = NULL;
PFNGLRESETHISTOGRAMEXTPROC _funcptr_glResetHistogramEXT = NULL;
PFNGLRESETMINMAXEXTPROC _funcptr_glResetMinmaxEXT = NULL;

static int LoadExt_EXT_histogram()
{
	int numFailed = 0;
	_funcptr_glGetHistogramEXT = (PFNGLGETHISTOGRAMEXTPROC)IntGetProcAddress("glGetHistogramEXT");
	if(!_funcptr_glGetHistogramEXT) ++numFailed;
	_funcptr_glGetHistogramParameterfvEXT = (PFNGLGETHISTOGRAMPARAMETERFVEXTPROC)IntGetProcAddress("glGetHistogramParameterfvEXT");
	if(!_funcptr_glGetHistogramParameterfvEXT) ++numFailed;
	_funcptr_glGetHistogramParameterivEXT = (PFNGLGETHISTOGRAMPARAMETERIVEXTPROC)IntGetProcAddress("glGetHistogramParameterivEXT");
	if(!_funcptr_glGetHistogramParameterivEXT) ++numFailed;
	_funcptr_glGetMinmaxEXT = (PFNGLGETMINMAXEXTPROC)IntGetProcAddress("glGetMinmaxEXT");
	if(!_funcptr_glGetMinmaxEXT) ++numFailed;
	_funcptr_glGetMinmaxParameterfvEXT = (PFNGLGETMINMAXPARAMETERFVEXTPROC)IntGetProcAddress("glGetMinmaxParameterfvEXT");
	if(!_funcptr_glGetMinmaxParameterfvEXT) ++numFailed;
	_funcptr_glGetMinmaxParameterivEXT = (PFNGLGETMINMAXPARAMETERIVEXTPROC)IntGetProcAddress("glGetMinmaxParameterivEXT");
	if(!_funcptr_glGetMinmaxParameterivEXT) ++numFailed;
	_funcptr_glHistogramEXT = (PFNGLHISTOGRAMEXTPROC)IntGetProcAddress("glHistogramEXT");
	if(!_funcptr_glHistogramEXT) ++numFailed;
	_funcptr_glMinmaxEXT = (PFNGLMINMAXEXTPROC)IntGetProcAddress("glMinmaxEXT");
	if(!_funcptr_glMinmaxEXT) ++numFailed;
	_funcptr_glResetHistogramEXT = (PFNGLRESETHISTOGRAMEXTPROC)IntGetProcAddress("glResetHistogramEXT");
	if(!_funcptr_glResetHistogramEXT) ++numFailed;
	_funcptr_glResetMinmaxEXT = (PFNGLRESETMINMAXEXTPROC)IntGetProcAddress("glResetMinmaxEXT");
	if(!_funcptr_glResetMinmaxEXT) ++numFailed;
	return numFailed;
}

PFNGLINDEXFUNCEXTPROC _funcptr_glIndexFuncEXT = NULL;

static int LoadExt_EXT_index_func()
{
	int numFailed = 0;
	_funcptr_glIndexFuncEXT = (PFNGLINDEXFUNCEXTPROC)IntGetProcAddress("glIndexFuncEXT");
	if(!_funcptr_glIndexFuncEXT) ++numFailed;
	return numFailed;
}

PFNGLINDEXMATERIALEXTPROC _funcptr_glIndexMaterialEXT = NULL;

static int LoadExt_EXT_index_material()
{
	int numFailed = 0;
	_funcptr_glIndexMaterialEXT = (PFNGLINDEXMATERIALEXTPROC)IntGetProcAddress("glIndexMaterialEXT");
	if(!_funcptr_glIndexMaterialEXT) ++numFailed;
	return numFailed;
}

PFNGLAPPLYTEXTUREEXTPROC _funcptr_glApplyTextureEXT = NULL;
PFNGLTEXTURELIGHTEXTPROC _funcptr_glTextureLightEXT = NULL;
PFNGLTEXTUREMATERIALEXTPROC _funcptr_glTextureMaterialEXT = NULL;

static int LoadExt_EXT_light_texture()
{
	int numFailed = 0;
	_funcptr_glApplyTextureEXT = (PFNGLAPPLYTEXTUREEXTPROC)IntGetProcAddress("glApplyTextureEXT");
	if(!_funcptr_glApplyTextureEXT) ++numFailed;
	_funcptr_glTextureLightEXT = (PFNGLTEXTURELIGHTEXTPROC)IntGetProcAddress("glTextureLightEXT");
	if(!_funcptr_glTextureLightEXT) ++numFailed;
	_funcptr_glTextureMaterialEXT = (PFNGLTEXTUREMATERIALEXTPROC)IntGetProcAddress("glTextureMaterialEXT");
	if(!_funcptr_glTextureMaterialEXT) ++numFailed;
	return numFailed;
}

PFNGLMULTIDRAWARRAYSEXTPROC _funcptr_glMultiDrawArraysEXT = NULL;
PFNGLMULTIDRAWELEMENTSEXTPROC _funcptr_glMultiDrawElementsEXT = NULL;

static int LoadExt_EXT_multi_draw_arrays()
{
	int numFailed = 0;
	_funcptr_glMultiDrawArraysEXT = (PFNGLMULTIDRAWARRAYSEXTPROC)IntGetProcAddress("glMultiDrawArraysEXT");
	if(!_funcptr_glMultiDrawArraysEXT) ++numFailed;
	_funcptr_glMultiDrawElementsEXT = (PFNGLMULTIDRAWELEMENTSEXTPROC)IntGetProcAddress("glMultiDrawElementsEXT");
	if(!_funcptr_glMultiDrawElementsEXT) ++numFailed;
	return numFailed;
}

PFNGLSAMPLEMASKEXTPROC _funcptr_glSampleMaskEXT = NULL;
PFNGLSAMPLEPATTERNEXTPROC _funcptr_glSamplePatternEXT = NULL;

static int LoadExt_EXT_multisample()
{
	int numFailed = 0;
	_funcptr_glSampleMaskEXT = (PFNGLSAMPLEMASKEXTPROC)IntGetProcAddress("glSampleMaskEXT");
	if(!_funcptr_glSampleMaskEXT) ++numFailed;
	_funcptr_glSamplePatternEXT = (PFNGLSAMPLEPATTERNEXTPROC)IntGetProcAddress("glSamplePatternEXT");
	if(!_funcptr_glSamplePatternEXT) ++numFailed;
	return numFailed;
}

PFNGLCOLORTABLEEXTPROC _funcptr_glColorTableEXT = NULL;
PFNGLGETCOLORTABLEEXTPROC _funcptr_glGetColorTableEXT = NULL;
PFNGLGETCOLORTABLEPARAMETERIVEXTPROC _funcptr_glGetColorTableParameterivEXT = NULL;
PFNGLGETCOLORTABLEPARAMETERFVEXTPROC _funcptr_glGetColorTableParameterfvEXT = NULL;

static int LoadExt_EXT_paletted_texture()
{
	int numFailed = 0;
	_funcptr_glColorTableEXT = (PFNGLCOLORTABLEEXTPROC)IntGetProcAddress("glColorTableEXT");
	if(!_funcptr_glColorTableEXT) ++numFailed;
	_funcptr_glGetColorTableEXT = (PFNGLGETCOLORTABLEEXTPROC)IntGetProcAddress("glGetColorTableEXT");
	if(!_funcptr_glGetColorTableEXT) ++numFailed;
	_funcptr_glGetColorTableParameterivEXT = (PFNGLGETCOLORTABLEPARAMETERIVEXTPROC)IntGetProcAddress("glGetColorTableParameterivEXT");
	if(!_funcptr_glGetColorTableParameterivEXT) ++numFailed;
	_funcptr_glGetColorTableParameterfvEXT = (PFNGLGETCOLORTABLEPARAMETERFVEXTPROC)IntGetProcAddress("glGetColorTableParameterfvEXT");
	if(!_funcptr_glGetColorTableParameterfvEXT) ++numFailed;
	return numFailed;
}

PFNGLPIXELTRANSFORMPARAMETERIEXTPROC _funcptr_glPixelTransformParameteriEXT = NULL;
PFNGLPIXELTRANSFORMPARAMETERFEXTPROC _funcptr_glPixelTransformParameterfEXT = NULL;
PFNGLPIXELTRANSFORMPARAMETERIVEXTPROC _funcptr_glPixelTransformParameterivEXT = NULL;
PFNGLPIXELTRANSFORMPARAMETERFVEXTPROC _funcptr_glPixelTransformParameterfvEXT = NULL;
PFNGLGETPIXELTRANSFORMPARAMETERIVEXTPROC _funcptr_glGetPixelTransformParameterivEXT = NULL;
PFNGLGETPIXELTRANSFORMPARAMETERFVEXTPROC _funcptr_glGetPixelTransformParameterfvEXT = NULL;

static int LoadExt_EXT_pixel_transform()
{
	int numFailed = 0;
	_funcptr_glPixelTransformParameteriEXT = (PFNGLPIXELTRANSFORMPARAMETERIEXTPROC)IntGetProcAddress("glPixelTransformParameteriEXT");
	if(!_funcptr_glPixelTransformParameteriEXT) ++numFailed;
	_funcptr_glPixelTransformParameterfEXT = (PFNGLPIXELTRANSFORMPARAMETERFEXTPROC)IntGetProcAddress("glPixelTransformParameterfEXT");
	if(!_funcptr_glPixelTransformParameterfEXT) ++numFailed;
	_funcptr_glPixelTransformParameterivEXT = (PFNGLPIXELTRANSFORMPARAMETERIVEXTPROC)IntGetProcAddress("glPixelTransformParameterivEXT");
	if(!_funcptr_glPixelTransformParameterivEXT) ++numFailed;
	_funcptr_glPixelTransformParameterfvEXT = (PFNGLPIXELTRANSFORMPARAMETERFVEXTPROC)IntGetProcAddress("glPixelTransformParameterfvEXT");
	if(!_funcptr_glPixelTransformParameterfvEXT) ++numFailed;
	_funcptr_glGetPixelTransformParameterivEXT = (PFNGLGETPIXELTRANSFORMPARAMETERIVEXTPROC)IntGetProcAddress("glGetPixelTransformParameterivEXT");
	if(!_funcptr_glGetPixelTransformParameterivEXT) ++numFailed;
	_funcptr_glGetPixelTransformParameterfvEXT = (PFNGLGETPIXELTRANSFORMPARAMETERFVEXTPROC)IntGetProcAddress("glGetPixelTransformParameterfvEXT");
	if(!_funcptr_glGetPixelTransformParameterfvEXT) ++numFailed;
	return numFailed;
}

PFNGLPOINTPARAMETERFEXTPROC _funcptr_glPointParameterfEXT = NULL;
PFNGLPOINTPARAMETERFVEXTPROC _funcptr_glPointParameterfvEXT = NULL;

static int LoadExt_EXT_point_parameters()
{
	int numFailed = 0;
	_funcptr_glPointParameterfEXT = (PFNGLPOINTPARAMETERFEXTPROC)IntGetProcAddress("glPointParameterfEXT");
	if(!_funcptr_glPointParameterfEXT) ++numFailed;
	_funcptr_glPointParameterfvEXT = (PFNGLPOINTPARAMETERFVEXTPROC)IntGetProcAddress("glPointParameterfvEXT");
	if(!_funcptr_glPointParameterfvEXT) ++numFailed;
	return numFailed;
}

PFNGLPOLYGONOFFSETEXTPROC _funcptr_glPolygonOffsetEXT = NULL;

static int LoadExt_EXT_polygon_offset()
{
	int numFailed = 0;
	_funcptr_glPolygonOffsetEXT = (PFNGLPOLYGONOFFSETEXTPROC)IntGetProcAddress("glPolygonOffsetEXT");
	if(!_funcptr_glPolygonOffsetEXT) ++numFailed;
	return numFailed;
}

PFNGLPROVOKINGVERTEXEXTPROC _funcptr_glProvokingVertexEXT = NULL;

static int LoadExt_EXT_provoking_vertex()
{
	int numFailed = 0;
	_funcptr_glProvokingVertexEXT = (PFNGLPROVOKINGVERTEXEXTPROC)IntGetProcAddress("glProvokingVertexEXT");
	if(!_funcptr_glProvokingVertexEXT) ++numFailed;
	return numFailed;
}

PFNGLSECONDARYCOLOR3BEXTPROC _funcptr_glSecondaryColor3bEXT = NULL;
PFNGLSECONDARYCOLOR3BVEXTPROC _funcptr_glSecondaryColor3bvEXT = NULL;
PFNGLSECONDARYCOLOR3DEXTPROC _funcptr_glSecondaryColor3dEXT = NULL;
PFNGLSECONDARYCOLOR3DVEXTPROC _funcptr_glSecondaryColor3dvEXT = NULL;
PFNGLSECONDARYCOLOR3FEXTPROC _funcptr_glSecondaryColor3fEXT = NULL;
PFNGLSECONDARYCOLOR3FVEXTPROC _funcptr_glSecondaryColor3fvEXT = NULL;
PFNGLSECONDARYCOLOR3IEXTPROC _funcptr_glSecondaryColor3iEXT = NULL;
PFNGLSECONDARYCOLOR3IVEXTPROC _funcptr_glSecondaryColor3ivEXT = NULL;
PFNGLSECONDARYCOLOR3SEXTPROC _funcptr_glSecondaryColor3sEXT = NULL;
PFNGLSECONDARYCOLOR3SVEXTPROC _funcptr_glSecondaryColor3svEXT = NULL;
PFNGLSECONDARYCOLOR3UBEXTPROC _funcptr_glSecondaryColor3ubEXT = NULL;
PFNGLSECONDARYCOLOR3UBVEXTPROC _funcptr_glSecondaryColor3ubvEXT = NULL;
PFNGLSECONDARYCOLOR3UIEXTPROC _funcptr_glSecondaryColor3uiEXT = NULL;
PFNGLSECONDARYCOLOR3UIVEXTPROC _funcptr_glSecondaryColor3uivEXT = NULL;
PFNGLSECONDARYCOLOR3USEXTPROC _funcptr_glSecondaryColor3usEXT = NULL;
PFNGLSECONDARYCOLOR3USVEXTPROC _funcptr_glSecondaryColor3usvEXT = NULL;
PFNGLSECONDARYCOLORPOINTEREXTPROC _funcptr_glSecondaryColorPointerEXT = NULL;

static int LoadExt_EXT_secondary_color()
{
	int numFailed = 0;
	_funcptr_glSecondaryColor3bEXT = (PFNGLSECONDARYCOLOR3BEXTPROC)IntGetProcAddress("glSecondaryColor3bEXT");
	if(!_funcptr_glSecondaryColor3bEXT) ++numFailed;
	_funcptr_glSecondaryColor3bvEXT = (PFNGLSECONDARYCOLOR3BVEXTPROC)IntGetProcAddress("glSecondaryColor3bvEXT");
	if(!_funcptr_glSecondaryColor3bvEXT) ++numFailed;
	_funcptr_glSecondaryColor3dEXT = (PFNGLSECONDARYCOLOR3DEXTPROC)IntGetProcAddress("glSecondaryColor3dEXT");
	if(!_funcptr_glSecondaryColor3dEXT) ++numFailed;
	_funcptr_glSecondaryColor3dvEXT = (PFNGLSECONDARYCOLOR3DVEXTPROC)IntGetProcAddress("glSecondaryColor3dvEXT");
	if(!_funcptr_glSecondaryColor3dvEXT) ++numFailed;
	_funcptr_glSecondaryColor3fEXT = (PFNGLSECONDARYCOLOR3FEXTPROC)IntGetProcAddress("glSecondaryColor3fEXT");
	if(!_funcptr_glSecondaryColor3fEXT) ++numFailed;
	_funcptr_glSecondaryColor3fvEXT = (PFNGLSECONDARYCOLOR3FVEXTPROC)IntGetProcAddress("glSecondaryColor3fvEXT");
	if(!_funcptr_glSecondaryColor3fvEXT) ++numFailed;
	_funcptr_glSecondaryColor3iEXT = (PFNGLSECONDARYCOLOR3IEXTPROC)IntGetProcAddress("glSecondaryColor3iEXT");
	if(!_funcptr_glSecondaryColor3iEXT) ++numFailed;
	_funcptr_glSecondaryColor3ivEXT = (PFNGLSECONDARYCOLOR3IVEXTPROC)IntGetProcAddress("glSecondaryColor3ivEXT");
	if(!_funcptr_glSecondaryColor3ivEXT) ++numFailed;
	_funcptr_glSecondaryColor3sEXT = (PFNGLSECONDARYCOLOR3SEXTPROC)IntGetProcAddress("glSecondaryColor3sEXT");
	if(!_funcptr_glSecondaryColor3sEXT) ++numFailed;
	_funcptr_glSecondaryColor3svEXT = (PFNGLSECONDARYCOLOR3SVEXTPROC)IntGetProcAddress("glSecondaryColor3svEXT");
	if(!_funcptr_glSecondaryColor3svEXT) ++numFailed;
	_funcptr_glSecondaryColor3ubEXT = (PFNGLSECONDARYCOLOR3UBEXTPROC)IntGetProcAddress("glSecondaryColor3ubEXT");
	if(!_funcptr_glSecondaryColor3ubEXT) ++numFailed;
	_funcptr_glSecondaryColor3ubvEXT = (PFNGLSECONDARYCOLOR3UBVEXTPROC)IntGetProcAddress("glSecondaryColor3ubvEXT");
	if(!_funcptr_glSecondaryColor3ubvEXT) ++numFailed;
	_funcptr_glSecondaryColor3uiEXT = (PFNGLSECONDARYCOLOR3UIEXTPROC)IntGetProcAddress("glSecondaryColor3uiEXT");
	if(!_funcptr_glSecondaryColor3uiEXT) ++numFailed;
	_funcptr_glSecondaryColor3uivEXT = (PFNGLSECONDARYCOLOR3UIVEXTPROC)IntGetProcAddress("glSecondaryColor3uivEXT");
	if(!_funcptr_glSecondaryColor3uivEXT) ++numFailed;
	_funcptr_glSecondaryColor3usEXT = (PFNGLSECONDARYCOLOR3USEXTPROC)IntGetProcAddress("glSecondaryColor3usEXT");
	if(!_funcptr_glSecondaryColor3usEXT) ++numFailed;
	_funcptr_glSecondaryColor3usvEXT = (PFNGLSECONDARYCOLOR3USVEXTPROC)IntGetProcAddress("glSecondaryColor3usvEXT");
	if(!_funcptr_glSecondaryColor3usvEXT) ++numFailed;
	_funcptr_glSecondaryColorPointerEXT = (PFNGLSECONDARYCOLORPOINTEREXTPROC)IntGetProcAddress("glSecondaryColorPointerEXT");
	if(!_funcptr_glSecondaryColorPointerEXT) ++numFailed;
	return numFailed;
}

PFNGLUSESHADERPROGRAMEXTPROC _funcptr_glUseShaderProgramEXT = NULL;
PFNGLACTIVEPROGRAMEXTPROC _funcptr_glActiveProgramEXT = NULL;
PFNGLCREATESHADERPROGRAMEXTPROC _funcptr_glCreateShaderProgramEXT = NULL;

static int LoadExt_EXT_separate_shader_objects()
{
	int numFailed = 0;
	_funcptr_glUseShaderProgramEXT = (PFNGLUSESHADERPROGRAMEXTPROC)IntGetProcAddress("glUseShaderProgramEXT");
	if(!_funcptr_glUseShaderProgramEXT) ++numFailed;
	_funcptr_glActiveProgramEXT = (PFNGLACTIVEPROGRAMEXTPROC)IntGetProcAddress("glActiveProgramEXT");
	if(!_funcptr_glActiveProgramEXT) ++numFailed;
	_funcptr_glCreateShaderProgramEXT = (PFNGLCREATESHADERPROGRAMEXTPROC)IntGetProcAddress("glCreateShaderProgramEXT");
	if(!_funcptr_glCreateShaderProgramEXT) ++numFailed;
	return numFailed;
}

PFNGLBINDIMAGETEXTUREEXTPROC _funcptr_glBindImageTextureEXT = NULL;
PFNGLMEMORYBARRIEREXTPROC _funcptr_glMemoryBarrierEXT = NULL;

static int LoadExt_EXT_shader_image_load_store()
{
	int numFailed = 0;
	_funcptr_glBindImageTextureEXT = (PFNGLBINDIMAGETEXTUREEXTPROC)IntGetProcAddress("glBindImageTextureEXT");
	if(!_funcptr_glBindImageTextureEXT) ++numFailed;
	_funcptr_glMemoryBarrierEXT = (PFNGLMEMORYBARRIEREXTPROC)IntGetProcAddress("glMemoryBarrierEXT");
	if(!_funcptr_glMemoryBarrierEXT) ++numFailed;
	return numFailed;
}

PFNGLSTENCILCLEARTAGEXTPROC _funcptr_glStencilClearTagEXT = NULL;

static int LoadExt_EXT_stencil_clear_tag()
{
	int numFailed = 0;
	_funcptr_glStencilClearTagEXT = (PFNGLSTENCILCLEARTAGEXTPROC)IntGetProcAddress("glStencilClearTagEXT");
	if(!_funcptr_glStencilClearTagEXT) ++numFailed;
	return numFailed;
}

PFNGLACTIVESTENCILFACEEXTPROC _funcptr_glActiveStencilFaceEXT = NULL;

static int LoadExt_EXT_stencil_two_side()
{
	int numFailed = 0;
	_funcptr_glActiveStencilFaceEXT = (PFNGLACTIVESTENCILFACEEXTPROC)IntGetProcAddress("glActiveStencilFaceEXT");
	if(!_funcptr_glActiveStencilFaceEXT) ++numFailed;
	return numFailed;
}

PFNGLTEXSUBIMAGE1DEXTPROC _funcptr_glTexSubImage1DEXT = NULL;
PFNGLTEXSUBIMAGE2DEXTPROC _funcptr_glTexSubImage2DEXT = NULL;

static int LoadExt_EXT_subtexture()
{
	int numFailed = 0;
	_funcptr_glTexSubImage1DEXT = (PFNGLTEXSUBIMAGE1DEXTPROC)IntGetProcAddress("glTexSubImage1DEXT");
	if(!_funcptr_glTexSubImage1DEXT) ++numFailed;
	_funcptr_glTexSubImage2DEXT = (PFNGLTEXSUBIMAGE2DEXTPROC)IntGetProcAddress("glTexSubImage2DEXT");
	if(!_funcptr_glTexSubImage2DEXT) ++numFailed;
	return numFailed;
}

PFNGLTEXIMAGE3DEXTPROC _funcptr_glTexImage3DEXT = NULL;
PFNGLTEXSUBIMAGE3DEXTPROC _funcptr_glTexSubImage3DEXT = NULL;

static int LoadExt_EXT_texture3D()
{
	int numFailed = 0;
	_funcptr_glTexImage3DEXT = (PFNGLTEXIMAGE3DEXTPROC)IntGetProcAddress("glTexImage3DEXT");
	if(!_funcptr_glTexImage3DEXT) ++numFailed;
	_funcptr_glTexSubImage3DEXT = (PFNGLTEXSUBIMAGE3DEXTPROC)IntGetProcAddress("glTexSubImage3DEXT");
	if(!_funcptr_glTexSubImage3DEXT) ++numFailed;
	return numFailed;
}

PFNGLTEXBUFFEREXTPROC _funcptr_glTexBufferEXT = NULL;

static int LoadExt_EXT_texture_buffer_object()
{
	int numFailed = 0;
	_funcptr_glTexBufferEXT = (PFNGLTEXBUFFEREXTPROC)IntGetProcAddress("glTexBufferEXT");
	if(!_funcptr_glTexBufferEXT) ++numFailed;
	return numFailed;
}

PFNGLTEXPARAMETERIIVEXTPROC _funcptr_glTexParameterIivEXT = NULL;
PFNGLTEXPARAMETERIUIVEXTPROC _funcptr_glTexParameterIuivEXT = NULL;
PFNGLGETTEXPARAMETERIIVEXTPROC _funcptr_glGetTexParameterIivEXT = NULL;
PFNGLGETTEXPARAMETERIUIVEXTPROC _funcptr_glGetTexParameterIuivEXT = NULL;
PFNGLCLEARCOLORIIEXTPROC _funcptr_glClearColorIiEXT = NULL;
PFNGLCLEARCOLORIUIEXTPROC _funcptr_glClearColorIuiEXT = NULL;

static int LoadExt_EXT_texture_integer()
{
	int numFailed = 0;
	_funcptr_glTexParameterIivEXT = (PFNGLTEXPARAMETERIIVEXTPROC)IntGetProcAddress("glTexParameterIivEXT");
	if(!_funcptr_glTexParameterIivEXT) ++numFailed;
	_funcptr_glTexParameterIuivEXT = (PFNGLTEXPARAMETERIUIVEXTPROC)IntGetProcAddress("glTexParameterIuivEXT");
	if(!_funcptr_glTexParameterIuivEXT) ++numFailed;
	_funcptr_glGetTexParameterIivEXT = (PFNGLGETTEXPARAMETERIIVEXTPROC)IntGetProcAddress("glGetTexParameterIivEXT");
	if(!_funcptr_glGetTexParameterIivEXT) ++numFailed;
	_funcptr_glGetTexParameterIuivEXT = (PFNGLGETTEXPARAMETERIUIVEXTPROC)IntGetProcAddress("glGetTexParameterIuivEXT");
	if(!_funcptr_glGetTexParameterIuivEXT) ++numFailed;
	_funcptr_glClearColorIiEXT = (PFNGLCLEARCOLORIIEXTPROC)IntGetProcAddress("glClearColorIiEXT");
	if(!_funcptr_glClearColorIiEXT) ++numFailed;
	_funcptr_glClearColorIuiEXT = (PFNGLCLEARCOLORIUIEXTPROC)IntGetProcAddress("glClearColorIuiEXT");
	if(!_funcptr_glClearColorIuiEXT) ++numFailed;
	return numFailed;
}

PFNGLARETEXTURESRESIDENTEXTPROC _funcptr_glAreTexturesResidentEXT = NULL;
PFNGLBINDTEXTUREEXTPROC _funcptr_glBindTextureEXT = NULL;
PFNGLDELETETEXTURESEXTPROC _funcptr_glDeleteTexturesEXT = NULL;
PFNGLGENTEXTURESEXTPROC _funcptr_glGenTexturesEXT = NULL;
PFNGLISTEXTUREEXTPROC _funcptr_glIsTextureEXT = NULL;
PFNGLPRIORITIZETEXTURESEXTPROC _funcptr_glPrioritizeTexturesEXT = NULL;

static int LoadExt_EXT_texture_object()
{
	int numFailed = 0;
	_funcptr_glAreTexturesResidentEXT = (PFNGLARETEXTURESRESIDENTEXTPROC)IntGetProcAddress("glAreTexturesResidentEXT");
	if(!_funcptr_glAreTexturesResidentEXT) ++numFailed;
	_funcptr_glBindTextureEXT = (PFNGLBINDTEXTUREEXTPROC)IntGetProcAddress("glBindTextureEXT");
	if(!_funcptr_glBindTextureEXT) ++numFailed;
	_funcptr_glDeleteTexturesEXT = (PFNGLDELETETEXTURESEXTPROC)IntGetProcAddress("glDeleteTexturesEXT");
	if(!_funcptr_glDeleteTexturesEXT) ++numFailed;
	_funcptr_glGenTexturesEXT = (PFNGLGENTEXTURESEXTPROC)IntGetProcAddress("glGenTexturesEXT");
	if(!_funcptr_glGenTexturesEXT) ++numFailed;
	_funcptr_glIsTextureEXT = (PFNGLISTEXTUREEXTPROC)IntGetProcAddress("glIsTextureEXT");
	if(!_funcptr_glIsTextureEXT) ++numFailed;
	_funcptr_glPrioritizeTexturesEXT = (PFNGLPRIORITIZETEXTURESEXTPROC)IntGetProcAddress("glPrioritizeTexturesEXT");
	if(!_funcptr_glPrioritizeTexturesEXT) ++numFailed;
	return numFailed;
}

PFNGLTEXTURENORMALEXTPROC _funcptr_glTextureNormalEXT = NULL;

static int LoadExt_EXT_texture_perturb_normal()
{
	int numFailed = 0;
	_funcptr_glTextureNormalEXT = (PFNGLTEXTURENORMALEXTPROC)IntGetProcAddress("glTextureNormalEXT");
	if(!_funcptr_glTextureNormalEXT) ++numFailed;
	return numFailed;
}

PFNGLGETQUERYOBJECTI64VEXTPROC _funcptr_glGetQueryObjecti64vEXT = NULL;
PFNGLGETQUERYOBJECTUI64VEXTPROC _funcptr_glGetQueryObjectui64vEXT = NULL;

static int LoadExt_EXT_timer_query()
{
	int numFailed = 0;
	_funcptr_glGetQueryObjecti64vEXT = (PFNGLGETQUERYOBJECTI64VEXTPROC)IntGetProcAddress("glGetQueryObjecti64vEXT");
	if(!_funcptr_glGetQueryObjecti64vEXT) ++numFailed;
	_funcptr_glGetQueryObjectui64vEXT = (PFNGLGETQUERYOBJECTUI64VEXTPROC)IntGetProcAddress("glGetQueryObjectui64vEXT");
	if(!_funcptr_glGetQueryObjectui64vEXT) ++numFailed;
	return numFailed;
}

PFNGLBEGINTRANSFORMFEEDBACKEXTPROC _funcptr_glBeginTransformFeedbackEXT = NULL;
PFNGLENDTRANSFORMFEEDBACKEXTPROC _funcptr_glEndTransformFeedbackEXT = NULL;
PFNGLBINDBUFFERRANGEEXTPROC _funcptr_glBindBufferRangeEXT = NULL;
PFNGLBINDBUFFEROFFSETEXTPROC _funcptr_glBindBufferOffsetEXT = NULL;
PFNGLBINDBUFFERBASEEXTPROC _funcptr_glBindBufferBaseEXT = NULL;
PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC _funcptr_glTransformFeedbackVaryingsEXT = NULL;
PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC _funcptr_glGetTransformFeedbackVaryingEXT = NULL;

static int LoadExt_EXT_transform_feedback()
{
	int numFailed = 0;
	_funcptr_glBeginTransformFeedbackEXT = (PFNGLBEGINTRANSFORMFEEDBACKEXTPROC)IntGetProcAddress("glBeginTransformFeedbackEXT");
	if(!_funcptr_glBeginTransformFeedbackEXT) ++numFailed;
	_funcptr_glEndTransformFeedbackEXT = (PFNGLENDTRANSFORMFEEDBACKEXTPROC)IntGetProcAddress("glEndTransformFeedbackEXT");
	if(!_funcptr_glEndTransformFeedbackEXT) ++numFailed;
	_funcptr_glBindBufferRangeEXT = (PFNGLBINDBUFFERRANGEEXTPROC)IntGetProcAddress("glBindBufferRangeEXT");
	if(!_funcptr_glBindBufferRangeEXT) ++numFailed;
	_funcptr_glBindBufferOffsetEXT = (PFNGLBINDBUFFEROFFSETEXTPROC)IntGetProcAddress("glBindBufferOffsetEXT");
	if(!_funcptr_glBindBufferOffsetEXT) ++numFailed;
	_funcptr_glBindBufferBaseEXT = (PFNGLBINDBUFFERBASEEXTPROC)IntGetProcAddress("glBindBufferBaseEXT");
	if(!_funcptr_glBindBufferBaseEXT) ++numFailed;
	_funcptr_glTransformFeedbackVaryingsEXT = (PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC)IntGetProcAddress("glTransformFeedbackVaryingsEXT");
	if(!_funcptr_glTransformFeedbackVaryingsEXT) ++numFailed;
	_funcptr_glGetTransformFeedbackVaryingEXT = (PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC)IntGetProcAddress("glGetTransformFeedbackVaryingEXT");
	if(!_funcptr_glGetTransformFeedbackVaryingEXT) ++numFailed;
	return numFailed;
}

PFNGLARRAYELEMENTEXTPROC _funcptr_glArrayElementEXT = NULL;
PFNGLCOLORPOINTEREXTPROC _funcptr_glColorPointerEXT = NULL;
PFNGLDRAWARRAYSEXTPROC _funcptr_glDrawArraysEXT = NULL;
PFNGLEDGEFLAGPOINTEREXTPROC _funcptr_glEdgeFlagPointerEXT = NULL;
PFNGLGETPOINTERVEXTPROC _funcptr_glGetPointervEXT = NULL;
PFNGLINDEXPOINTEREXTPROC _funcptr_glIndexPointerEXT = NULL;
PFNGLNORMALPOINTEREXTPROC _funcptr_glNormalPointerEXT = NULL;
PFNGLTEXCOORDPOINTEREXTPROC _funcptr_glTexCoordPointerEXT = NULL;
PFNGLVERTEXPOINTEREXTPROC _funcptr_glVertexPointerEXT = NULL;

static int LoadExt_EXT_vertex_array()
{
	int numFailed = 0;
	_funcptr_glArrayElementEXT = (PFNGLARRAYELEMENTEXTPROC)IntGetProcAddress("glArrayElementEXT");
	if(!_funcptr_glArrayElementEXT) ++numFailed;
	_funcptr_glColorPointerEXT = (PFNGLCOLORPOINTEREXTPROC)IntGetProcAddress("glColorPointerEXT");
	if(!_funcptr_glColorPointerEXT) ++numFailed;
	_funcptr_glDrawArraysEXT = (PFNGLDRAWARRAYSEXTPROC)IntGetProcAddress("glDrawArraysEXT");
	if(!_funcptr_glDrawArraysEXT) ++numFailed;
	_funcptr_glEdgeFlagPointerEXT = (PFNGLEDGEFLAGPOINTEREXTPROC)IntGetProcAddress("glEdgeFlagPointerEXT");
	if(!_funcptr_glEdgeFlagPointerEXT) ++numFailed;
	_funcptr_glGetPointervEXT = (PFNGLGETPOINTERVEXTPROC)IntGetProcAddress("glGetPointervEXT");
	if(!_funcptr_glGetPointervEXT) ++numFailed;
	_funcptr_glIndexPointerEXT = (PFNGLINDEXPOINTEREXTPROC)IntGetProcAddress("glIndexPointerEXT");
	if(!_funcptr_glIndexPointerEXT) ++numFailed;
	_funcptr_glNormalPointerEXT = (PFNGLNORMALPOINTEREXTPROC)IntGetProcAddress("glNormalPointerEXT");
	if(!_funcptr_glNormalPointerEXT) ++numFailed;
	_funcptr_glTexCoordPointerEXT = (PFNGLTEXCOORDPOINTEREXTPROC)IntGetProcAddress("glTexCoordPointerEXT");
	if(!_funcptr_glTexCoordPointerEXT) ++numFailed;
	_funcptr_glVertexPointerEXT = (PFNGLVERTEXPOINTEREXTPROC)IntGetProcAddress("glVertexPointerEXT");
	if(!_funcptr_glVertexPointerEXT) ++numFailed;
	return numFailed;
}

PFNGLVERTEXATTRIBL1DEXTPROC _funcptr_glVertexAttribL1dEXT = NULL;
PFNGLVERTEXATTRIBL2DEXTPROC _funcptr_glVertexAttribL2dEXT = NULL;
PFNGLVERTEXATTRIBL3DEXTPROC _funcptr_glVertexAttribL3dEXT = NULL;
PFNGLVERTEXATTRIBL4DEXTPROC _funcptr_glVertexAttribL4dEXT = NULL;
PFNGLVERTEXATTRIBL1DVEXTPROC _funcptr_glVertexAttribL1dvEXT = NULL;
PFNGLVERTEXATTRIBL2DVEXTPROC _funcptr_glVertexAttribL2dvEXT = NULL;
PFNGLVERTEXATTRIBL3DVEXTPROC _funcptr_glVertexAttribL3dvEXT = NULL;
PFNGLVERTEXATTRIBL4DVEXTPROC _funcptr_glVertexAttribL4dvEXT = NULL;
PFNGLVERTEXATTRIBLPOINTEREXTPROC _funcptr_glVertexAttribLPointerEXT = NULL;
PFNGLGETVERTEXATTRIBLDVEXTPROC _funcptr_glGetVertexAttribLdvEXT = NULL;
PFNGLVERTEXARRAYVERTEXATTRIBLOFFSETEXTPROC _funcptr_glVertexArrayVertexAttribLOffsetEXT = NULL;

static int LoadExt_EXT_vertex_attrib_64bit()
{
	int numFailed = 0;
	_funcptr_glVertexAttribL1dEXT = (PFNGLVERTEXATTRIBL1DEXTPROC)IntGetProcAddress("glVertexAttribL1dEXT");
	if(!_funcptr_glVertexAttribL1dEXT) ++numFailed;
	_funcptr_glVertexAttribL2dEXT = (PFNGLVERTEXATTRIBL2DEXTPROC)IntGetProcAddress("glVertexAttribL2dEXT");
	if(!_funcptr_glVertexAttribL2dEXT) ++numFailed;
	_funcptr_glVertexAttribL3dEXT = (PFNGLVERTEXATTRIBL3DEXTPROC)IntGetProcAddress("glVertexAttribL3dEXT");
	if(!_funcptr_glVertexAttribL3dEXT) ++numFailed;
	_funcptr_glVertexAttribL4dEXT = (PFNGLVERTEXATTRIBL4DEXTPROC)IntGetProcAddress("glVertexAttribL4dEXT");
	if(!_funcptr_glVertexAttribL4dEXT) ++numFailed;
	_funcptr_glVertexAttribL1dvEXT = (PFNGLVERTEXATTRIBL1DVEXTPROC)IntGetProcAddress("glVertexAttribL1dvEXT");
	if(!_funcptr_glVertexAttribL1dvEXT) ++numFailed;
	_funcptr_glVertexAttribL2dvEXT = (PFNGLVERTEXATTRIBL2DVEXTPROC)IntGetProcAddress("glVertexAttribL2dvEXT");
	if(!_funcptr_glVertexAttribL2dvEXT) ++numFailed;
	_funcptr_glVertexAttribL3dvEXT = (PFNGLVERTEXATTRIBL3DVEXTPROC)IntGetProcAddress("glVertexAttribL3dvEXT");
	if(!_funcptr_glVertexAttribL3dvEXT) ++numFailed;
	_funcptr_glVertexAttribL4dvEXT = (PFNGLVERTEXATTRIBL4DVEXTPROC)IntGetProcAddress("glVertexAttribL4dvEXT");
	if(!_funcptr_glVertexAttribL4dvEXT) ++numFailed;
	_funcptr_glVertexAttribLPointerEXT = (PFNGLVERTEXATTRIBLPOINTEREXTPROC)IntGetProcAddress("glVertexAttribLPointerEXT");
	if(!_funcptr_glVertexAttribLPointerEXT) ++numFailed;
	_funcptr_glGetVertexAttribLdvEXT = (PFNGLGETVERTEXATTRIBLDVEXTPROC)IntGetProcAddress("glGetVertexAttribLdvEXT");
	if(!_funcptr_glGetVertexAttribLdvEXT) ++numFailed;
	_funcptr_glVertexArrayVertexAttribLOffsetEXT = (PFNGLVERTEXARRAYVERTEXATTRIBLOFFSETEXTPROC)IntGetProcAddress("glVertexArrayVertexAttribLOffsetEXT");
	if(!_funcptr_glVertexArrayVertexAttribLOffsetEXT) ++numFailed;
	return numFailed;
}

PFNGLBEGINVERTEXSHADEREXTPROC _funcptr_glBeginVertexShaderEXT = NULL;
PFNGLENDVERTEXSHADEREXTPROC _funcptr_glEndVertexShaderEXT = NULL;
PFNGLBINDVERTEXSHADEREXTPROC _funcptr_glBindVertexShaderEXT = NULL;
PFNGLGENVERTEXSHADERSEXTPROC _funcptr_glGenVertexShadersEXT = NULL;
PFNGLDELETEVERTEXSHADEREXTPROC _funcptr_glDeleteVertexShaderEXT = NULL;
PFNGLSHADEROP1EXTPROC _funcptr_glShaderOp1EXT = NULL;
PFNGLSHADEROP2EXTPROC _funcptr_glShaderOp2EXT = NULL;
PFNGLSHADEROP3EXTPROC _funcptr_glShaderOp3EXT = NULL;
PFNGLSWIZZLEEXTPROC _funcptr_glSwizzleEXT = NULL;
PFNGLWRITEMASKEXTPROC _funcptr_glWriteMaskEXT = NULL;
PFNGLINSERTCOMPONENTEXTPROC _funcptr_glInsertComponentEXT = NULL;
PFNGLEXTRACTCOMPONENTEXTPROC _funcptr_glExtractComponentEXT = NULL;
PFNGLGENSYMBOLSEXTPROC _funcptr_glGenSymbolsEXT = NULL;
PFNGLSETINVARIANTEXTPROC _funcptr_glSetInvariantEXT = NULL;
PFNGLSETLOCALCONSTANTEXTPROC _funcptr_glSetLocalConstantEXT = NULL;
PFNGLVARIANTBVEXTPROC _funcptr_glVariantbvEXT = NULL;
PFNGLVARIANTSVEXTPROC _funcptr_glVariantsvEXT = NULL;
PFNGLVARIANTIVEXTPROC _funcptr_glVariantivEXT = NULL;
PFNGLVARIANTFVEXTPROC _funcptr_glVariantfvEXT = NULL;
PFNGLVARIANTDVEXTPROC _funcptr_glVariantdvEXT = NULL;
PFNGLVARIANTUBVEXTPROC _funcptr_glVariantubvEXT = NULL;
PFNGLVARIANTUSVEXTPROC _funcptr_glVariantusvEXT = NULL;
PFNGLVARIANTUIVEXTPROC _funcptr_glVariantuivEXT = NULL;
PFNGLVARIANTPOINTEREXTPROC _funcptr_glVariantPointerEXT = NULL;
PFNGLENABLEVARIANTCLIENTSTATEEXTPROC _funcptr_glEnableVariantClientStateEXT = NULL;
PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC _funcptr_glDisableVariantClientStateEXT = NULL;
PFNGLBINDLIGHTPARAMETEREXTPROC _funcptr_glBindLightParameterEXT = NULL;
PFNGLBINDMATERIALPARAMETEREXTPROC _funcptr_glBindMaterialParameterEXT = NULL;
PFNGLBINDTEXGENPARAMETEREXTPROC _funcptr_glBindTexGenParameterEXT = NULL;
PFNGLBINDTEXTUREUNITPARAMETEREXTPROC _funcptr_glBindTextureUnitParameterEXT = NULL;
PFNGLBINDPARAMETEREXTPROC _funcptr_glBindParameterEXT = NULL;
PFNGLISVARIANTENABLEDEXTPROC _funcptr_glIsVariantEnabledEXT = NULL;
PFNGLGETVARIANTBOOLEANVEXTPROC _funcptr_glGetVariantBooleanvEXT = NULL;
PFNGLGETVARIANTINTEGERVEXTPROC _funcptr_glGetVariantIntegervEXT = NULL;
PFNGLGETVARIANTFLOATVEXTPROC _funcptr_glGetVariantFloatvEXT = NULL;
PFNGLGETVARIANTPOINTERVEXTPROC _funcptr_glGetVariantPointervEXT = NULL;
PFNGLGETINVARIANTBOOLEANVEXTPROC _funcptr_glGetInvariantBooleanvEXT = NULL;
PFNGLGETINVARIANTINTEGERVEXTPROC _funcptr_glGetInvariantIntegervEXT = NULL;
PFNGLGETINVARIANTFLOATVEXTPROC _funcptr_glGetInvariantFloatvEXT = NULL;
PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC _funcptr_glGetLocalConstantBooleanvEXT = NULL;
PFNGLGETLOCALCONSTANTINTEGERVEXTPROC _funcptr_glGetLocalConstantIntegervEXT = NULL;
PFNGLGETLOCALCONSTANTFLOATVEXTPROC _funcptr_glGetLocalConstantFloatvEXT = NULL;

static int LoadExt_EXT_vertex_shader()
{
	int numFailed = 0;
	_funcptr_glBeginVertexShaderEXT = (PFNGLBEGINVERTEXSHADEREXTPROC)IntGetProcAddress("glBeginVertexShaderEXT");
	if(!_funcptr_glBeginVertexShaderEXT) ++numFailed;
	_funcptr_glEndVertexShaderEXT = (PFNGLENDVERTEXSHADEREXTPROC)IntGetProcAddress("glEndVertexShaderEXT");
	if(!_funcptr_glEndVertexShaderEXT) ++numFailed;
	_funcptr_glBindVertexShaderEXT = (PFNGLBINDVERTEXSHADEREXTPROC)IntGetProcAddress("glBindVertexShaderEXT");
	if(!_funcptr_glBindVertexShaderEXT) ++numFailed;
	_funcptr_glGenVertexShadersEXT = (PFNGLGENVERTEXSHADERSEXTPROC)IntGetProcAddress("glGenVertexShadersEXT");
	if(!_funcptr_glGenVertexShadersEXT) ++numFailed;
	_funcptr_glDeleteVertexShaderEXT = (PFNGLDELETEVERTEXSHADEREXTPROC)IntGetProcAddress("glDeleteVertexShaderEXT");
	if(!_funcptr_glDeleteVertexShaderEXT) ++numFailed;
	_funcptr_glShaderOp1EXT = (PFNGLSHADEROP1EXTPROC)IntGetProcAddress("glShaderOp1EXT");
	if(!_funcptr_glShaderOp1EXT) ++numFailed;
	_funcptr_glShaderOp2EXT = (PFNGLSHADEROP2EXTPROC)IntGetProcAddress("glShaderOp2EXT");
	if(!_funcptr_glShaderOp2EXT) ++numFailed;
	_funcptr_glShaderOp3EXT = (PFNGLSHADEROP3EXTPROC)IntGetProcAddress("glShaderOp3EXT");
	if(!_funcptr_glShaderOp3EXT) ++numFailed;
	_funcptr_glSwizzleEXT = (PFNGLSWIZZLEEXTPROC)IntGetProcAddress("glSwizzleEXT");
	if(!_funcptr_glSwizzleEXT) ++numFailed;
	_funcptr_glWriteMaskEXT = (PFNGLWRITEMASKEXTPROC)IntGetProcAddress("glWriteMaskEXT");
	if(!_funcptr_glWriteMaskEXT) ++numFailed;
	_funcptr_glInsertComponentEXT = (PFNGLINSERTCOMPONENTEXTPROC)IntGetProcAddress("glInsertComponentEXT");
	if(!_funcptr_glInsertComponentEXT) ++numFailed;
	_funcptr_glExtractComponentEXT = (PFNGLEXTRACTCOMPONENTEXTPROC)IntGetProcAddress("glExtractComponentEXT");
	if(!_funcptr_glExtractComponentEXT) ++numFailed;
	_funcptr_glGenSymbolsEXT = (PFNGLGENSYMBOLSEXTPROC)IntGetProcAddress("glGenSymbolsEXT");
	if(!_funcptr_glGenSymbolsEXT) ++numFailed;
	_funcptr_glSetInvariantEXT = (PFNGLSETINVARIANTEXTPROC)IntGetProcAddress("glSetInvariantEXT");
	if(!_funcptr_glSetInvariantEXT) ++numFailed;
	_funcptr_glSetLocalConstantEXT = (PFNGLSETLOCALCONSTANTEXTPROC)IntGetProcAddress("glSetLocalConstantEXT");
	if(!_funcptr_glSetLocalConstantEXT) ++numFailed;
	_funcptr_glVariantbvEXT = (PFNGLVARIANTBVEXTPROC)IntGetProcAddress("glVariantbvEXT");
	if(!_funcptr_glVariantbvEXT) ++numFailed;
	_funcptr_glVariantsvEXT = (PFNGLVARIANTSVEXTPROC)IntGetProcAddress("glVariantsvEXT");
	if(!_funcptr_glVariantsvEXT) ++numFailed;
	_funcptr_glVariantivEXT = (PFNGLVARIANTIVEXTPROC)IntGetProcAddress("glVariantivEXT");
	if(!_funcptr_glVariantivEXT) ++numFailed;
	_funcptr_glVariantfvEXT = (PFNGLVARIANTFVEXTPROC)IntGetProcAddress("glVariantfvEXT");
	if(!_funcptr_glVariantfvEXT) ++numFailed;
	_funcptr_glVariantdvEXT = (PFNGLVARIANTDVEXTPROC)IntGetProcAddress("glVariantdvEXT");
	if(!_funcptr_glVariantdvEXT) ++numFailed;
	_funcptr_glVariantubvEXT = (PFNGLVARIANTUBVEXTPROC)IntGetProcAddress("glVariantubvEXT");
	if(!_funcptr_glVariantubvEXT) ++numFailed;
	_funcptr_glVariantusvEXT = (PFNGLVARIANTUSVEXTPROC)IntGetProcAddress("glVariantusvEXT");
	if(!_funcptr_glVariantusvEXT) ++numFailed;
	_funcptr_glVariantuivEXT = (PFNGLVARIANTUIVEXTPROC)IntGetProcAddress("glVariantuivEXT");
	if(!_funcptr_glVariantuivEXT) ++numFailed;
	_funcptr_glVariantPointerEXT = (PFNGLVARIANTPOINTEREXTPROC)IntGetProcAddress("glVariantPointerEXT");
	if(!_funcptr_glVariantPointerEXT) ++numFailed;
	_funcptr_glEnableVariantClientStateEXT = (PFNGLENABLEVARIANTCLIENTSTATEEXTPROC)IntGetProcAddress("glEnableVariantClientStateEXT");
	if(!_funcptr_glEnableVariantClientStateEXT) ++numFailed;
	_funcptr_glDisableVariantClientStateEXT = (PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC)IntGetProcAddress("glDisableVariantClientStateEXT");
	if(!_funcptr_glDisableVariantClientStateEXT) ++numFailed;
	_funcptr_glBindLightParameterEXT = (PFNGLBINDLIGHTPARAMETEREXTPROC)IntGetProcAddress("glBindLightParameterEXT");
	if(!_funcptr_glBindLightParameterEXT) ++numFailed;
	_funcptr_glBindMaterialParameterEXT = (PFNGLBINDMATERIALPARAMETEREXTPROC)IntGetProcAddress("glBindMaterialParameterEXT");
	if(!_funcptr_glBindMaterialParameterEXT) ++numFailed;
	_funcptr_glBindTexGenParameterEXT = (PFNGLBINDTEXGENPARAMETEREXTPROC)IntGetProcAddress("glBindTexGenParameterEXT");
	if(!_funcptr_glBindTexGenParameterEXT) ++numFailed;
	_funcptr_glBindTextureUnitParameterEXT = (PFNGLBINDTEXTUREUNITPARAMETEREXTPROC)IntGetProcAddress("glBindTextureUnitParameterEXT");
	if(!_funcptr_glBindTextureUnitParameterEXT) ++numFailed;
	_funcptr_glBindParameterEXT = (PFNGLBINDPARAMETEREXTPROC)IntGetProcAddress("glBindParameterEXT");
	if(!_funcptr_glBindParameterEXT) ++numFailed;
	_funcptr_glIsVariantEnabledEXT = (PFNGLISVARIANTENABLEDEXTPROC)IntGetProcAddress("glIsVariantEnabledEXT");
	if(!_funcptr_glIsVariantEnabledEXT) ++numFailed;
	_funcptr_glGetVariantBooleanvEXT = (PFNGLGETVARIANTBOOLEANVEXTPROC)IntGetProcAddress("glGetVariantBooleanvEXT");
	if(!_funcptr_glGetVariantBooleanvEXT) ++numFailed;
	_funcptr_glGetVariantIntegervEXT = (PFNGLGETVARIANTINTEGERVEXTPROC)IntGetProcAddress("glGetVariantIntegervEXT");
	if(!_funcptr_glGetVariantIntegervEXT) ++numFailed;
	_funcptr_glGetVariantFloatvEXT = (PFNGLGETVARIANTFLOATVEXTPROC)IntGetProcAddress("glGetVariantFloatvEXT");
	if(!_funcptr_glGetVariantFloatvEXT) ++numFailed;
	_funcptr_glGetVariantPointervEXT = (PFNGLGETVARIANTPOINTERVEXTPROC)IntGetProcAddress("glGetVariantPointervEXT");
	if(!_funcptr_glGetVariantPointervEXT) ++numFailed;
	_funcptr_glGetInvariantBooleanvEXT = (PFNGLGETINVARIANTBOOLEANVEXTPROC)IntGetProcAddress("glGetInvariantBooleanvEXT");
	if(!_funcptr_glGetInvariantBooleanvEXT) ++numFailed;
	_funcptr_glGetInvariantIntegervEXT = (PFNGLGETINVARIANTINTEGERVEXTPROC)IntGetProcAddress("glGetInvariantIntegervEXT");
	if(!_funcptr_glGetInvariantIntegervEXT) ++numFailed;
	_funcptr_glGetInvariantFloatvEXT = (PFNGLGETINVARIANTFLOATVEXTPROC)IntGetProcAddress("glGetInvariantFloatvEXT");
	if(!_funcptr_glGetInvariantFloatvEXT) ++numFailed;
	_funcptr_glGetLocalConstantBooleanvEXT = (PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC)IntGetProcAddress("glGetLocalConstantBooleanvEXT");
	if(!_funcptr_glGetLocalConstantBooleanvEXT) ++numFailed;
	_funcptr_glGetLocalConstantIntegervEXT = (PFNGLGETLOCALCONSTANTINTEGERVEXTPROC)IntGetProcAddress("glGetLocalConstantIntegervEXT");
	if(!_funcptr_glGetLocalConstantIntegervEXT) ++numFailed;
	_funcptr_glGetLocalConstantFloatvEXT = (PFNGLGETLOCALCONSTANTFLOATVEXTPROC)IntGetProcAddress("glGetLocalConstantFloatvEXT");
	if(!_funcptr_glGetLocalConstantFloatvEXT) ++numFailed;
	return numFailed;
}

PFNGLVERTEXWEIGHTFEXTPROC _funcptr_glVertexWeightfEXT = NULL;
PFNGLVERTEXWEIGHTFVEXTPROC _funcptr_glVertexWeightfvEXT = NULL;
PFNGLVERTEXWEIGHTPOINTEREXTPROC _funcptr_glVertexWeightPointerEXT = NULL;

static int LoadExt_EXT_vertex_weighting()
{
	int numFailed = 0;
	_funcptr_glVertexWeightfEXT = (PFNGLVERTEXWEIGHTFEXTPROC)IntGetProcAddress("glVertexWeightfEXT");
	if(!_funcptr_glVertexWeightfEXT) ++numFailed;
	_funcptr_glVertexWeightfvEXT = (PFNGLVERTEXWEIGHTFVEXTPROC)IntGetProcAddress("glVertexWeightfvEXT");
	if(!_funcptr_glVertexWeightfvEXT) ++numFailed;
	_funcptr_glVertexWeightPointerEXT = (PFNGLVERTEXWEIGHTPOINTEREXTPROC)IntGetProcAddress("glVertexWeightPointerEXT");
	if(!_funcptr_glVertexWeightPointerEXT) ++numFailed;
	return numFailed;
}

PFNGLIMPORTSYNCEXTPROC _funcptr_glImportSyncEXT = NULL;

static int LoadExt_EXT_x11_sync_object()
{
	int numFailed = 0;
	_funcptr_glImportSyncEXT = (PFNGLIMPORTSYNCEXTPROC)IntGetProcAddress("glImportSyncEXT");
	if(!_funcptr_glImportSyncEXT) ++numFailed;
	return numFailed;
}

PFNGLFRAMETERMINATORGREMEDYPROC _funcptr_glFrameTerminatorGREMEDY = NULL;

static int LoadExt_GREMEDY_frame_terminator()
{
	int numFailed = 0;
	_funcptr_glFrameTerminatorGREMEDY = (PFNGLFRAMETERMINATORGREMEDYPROC)IntGetProcAddress("glFrameTerminatorGREMEDY");
	if(!_funcptr_glFrameTerminatorGREMEDY) ++numFailed;
	return numFailed;
}

PFNGLSTRINGMARKERGREMEDYPROC _funcptr_glStringMarkerGREMEDY = NULL;

static int LoadExt_GREMEDY_string_marker()
{
	int numFailed = 0;
	_funcptr_glStringMarkerGREMEDY = (PFNGLSTRINGMARKERGREMEDYPROC)IntGetProcAddress("glStringMarkerGREMEDY");
	if(!_funcptr_glStringMarkerGREMEDY) ++numFailed;
	return numFailed;
}

PFNGLIMAGETRANSFORMPARAMETERIHPPROC _funcptr_glImageTransformParameteriHP = NULL;
PFNGLIMAGETRANSFORMPARAMETERFHPPROC _funcptr_glImageTransformParameterfHP = NULL;
PFNGLIMAGETRANSFORMPARAMETERIVHPPROC _funcptr_glImageTransformParameterivHP = NULL;
PFNGLIMAGETRANSFORMPARAMETERFVHPPROC _funcptr_glImageTransformParameterfvHP = NULL;
PFNGLGETIMAGETRANSFORMPARAMETERIVHPPROC _funcptr_glGetImageTransformParameterivHP = NULL;
PFNGLGETIMAGETRANSFORMPARAMETERFVHPPROC _funcptr_glGetImageTransformParameterfvHP = NULL;

static int LoadExt_HP_image_transform()
{
	int numFailed = 0;
	_funcptr_glImageTransformParameteriHP = (PFNGLIMAGETRANSFORMPARAMETERIHPPROC)IntGetProcAddress("glImageTransformParameteriHP");
	if(!_funcptr_glImageTransformParameteriHP) ++numFailed;
	_funcptr_glImageTransformParameterfHP = (PFNGLIMAGETRANSFORMPARAMETERFHPPROC)IntGetProcAddress("glImageTransformParameterfHP");
	if(!_funcptr_glImageTransformParameterfHP) ++numFailed;
	_funcptr_glImageTransformParameterivHP = (PFNGLIMAGETRANSFORMPARAMETERIVHPPROC)IntGetProcAddress("glImageTransformParameterivHP");
	if(!_funcptr_glImageTransformParameterivHP) ++numFailed;
	_funcptr_glImageTransformParameterfvHP = (PFNGLIMAGETRANSFORMPARAMETERFVHPPROC)IntGetProcAddress("glImageTransformParameterfvHP");
	if(!_funcptr_glImageTransformParameterfvHP) ++numFailed;
	_funcptr_glGetImageTransformParameterivHP = (PFNGLGETIMAGETRANSFORMPARAMETERIVHPPROC)IntGetProcAddress("glGetImageTransformParameterivHP");
	if(!_funcptr_glGetImageTransformParameterivHP) ++numFailed;
	_funcptr_glGetImageTransformParameterfvHP = (PFNGLGETIMAGETRANSFORMPARAMETERFVHPPROC)IntGetProcAddress("glGetImageTransformParameterfvHP");
	if(!_funcptr_glGetImageTransformParameterfvHP) ++numFailed;
	return numFailed;
}

PFNGLMULTIMODEDRAWARRAYSIBMPROC _funcptr_glMultiModeDrawArraysIBM = NULL;
PFNGLMULTIMODEDRAWELEMENTSIBMPROC _funcptr_glMultiModeDrawElementsIBM = NULL;

static int LoadExt_IBM_multimode_draw_arrays()
{
	int numFailed = 0;
	_funcptr_glMultiModeDrawArraysIBM = (PFNGLMULTIMODEDRAWARRAYSIBMPROC)IntGetProcAddress("glMultiModeDrawArraysIBM");
	if(!_funcptr_glMultiModeDrawArraysIBM) ++numFailed;
	_funcptr_glMultiModeDrawElementsIBM = (PFNGLMULTIMODEDRAWELEMENTSIBMPROC)IntGetProcAddress("glMultiModeDrawElementsIBM");
	if(!_funcptr_glMultiModeDrawElementsIBM) ++numFailed;
	return numFailed;
}

PFNGLFLUSHSTATICDATAIBMPROC _funcptr_glFlushStaticDataIBM = NULL;

static int LoadExt_IBM_static_data()
{
	int numFailed = 0;
	_funcptr_glFlushStaticDataIBM = (PFNGLFLUSHSTATICDATAIBMPROC)IntGetProcAddress("glFlushStaticDataIBM");
	if(!_funcptr_glFlushStaticDataIBM) ++numFailed;
	return numFailed;
}

PFNGLCOLORPOINTERLISTIBMPROC _funcptr_glColorPointerListIBM = NULL;
PFNGLSECONDARYCOLORPOINTERLISTIBMPROC _funcptr_glSecondaryColorPointerListIBM = NULL;
PFNGLEDGEFLAGPOINTERLISTIBMPROC _funcptr_glEdgeFlagPointerListIBM = NULL;
PFNGLFOGCOORDPOINTERLISTIBMPROC _funcptr_glFogCoordPointerListIBM = NULL;
PFNGLINDEXPOINTERLISTIBMPROC _funcptr_glIndexPointerListIBM = NULL;
PFNGLNORMALPOINTERLISTIBMPROC _funcptr_glNormalPointerListIBM = NULL;
PFNGLTEXCOORDPOINTERLISTIBMPROC _funcptr_glTexCoordPointerListIBM = NULL;
PFNGLVERTEXPOINTERLISTIBMPROC _funcptr_glVertexPointerListIBM = NULL;

static int LoadExt_IBM_vertex_array_lists()
{
	int numFailed = 0;
	_funcptr_glColorPointerListIBM = (PFNGLCOLORPOINTERLISTIBMPROC)IntGetProcAddress("glColorPointerListIBM");
	if(!_funcptr_glColorPointerListIBM) ++numFailed;
	_funcptr_glSecondaryColorPointerListIBM = (PFNGLSECONDARYCOLORPOINTERLISTIBMPROC)IntGetProcAddress("glSecondaryColorPointerListIBM");
	if(!_funcptr_glSecondaryColorPointerListIBM) ++numFailed;
	_funcptr_glEdgeFlagPointerListIBM = (PFNGLEDGEFLAGPOINTERLISTIBMPROC)IntGetProcAddress("glEdgeFlagPointerListIBM");
	if(!_funcptr_glEdgeFlagPointerListIBM) ++numFailed;
	_funcptr_glFogCoordPointerListIBM = (PFNGLFOGCOORDPOINTERLISTIBMPROC)IntGetProcAddress("glFogCoordPointerListIBM");
	if(!_funcptr_glFogCoordPointerListIBM) ++numFailed;
	_funcptr_glIndexPointerListIBM = (PFNGLINDEXPOINTERLISTIBMPROC)IntGetProcAddress("glIndexPointerListIBM");
	if(!_funcptr_glIndexPointerListIBM) ++numFailed;
	_funcptr_glNormalPointerListIBM = (PFNGLNORMALPOINTERLISTIBMPROC)IntGetProcAddress("glNormalPointerListIBM");
	if(!_funcptr_glNormalPointerListIBM) ++numFailed;
	_funcptr_glTexCoordPointerListIBM = (PFNGLTEXCOORDPOINTERLISTIBMPROC)IntGetProcAddress("glTexCoordPointerListIBM");
	if(!_funcptr_glTexCoordPointerListIBM) ++numFailed;
	_funcptr_glVertexPointerListIBM = (PFNGLVERTEXPOINTERLISTIBMPROC)IntGetProcAddress("glVertexPointerListIBM");
	if(!_funcptr_glVertexPointerListIBM) ++numFailed;
	return numFailed;
}

PFNGLSYNCTEXTUREINTELPROC _funcptr_glSyncTextureINTEL = NULL;
PFNGLUNMAPTEXTURE2DINTELPROC _funcptr_glUnmapTexture2DINTEL = NULL;
PFNGLMAPTEXTURE2DINTELPROC _funcptr_glMapTexture2DINTEL = NULL;

static int LoadExt_INTEL_map_texture()
{
	int numFailed = 0;
	_funcptr_glSyncTextureINTEL = (PFNGLSYNCTEXTUREINTELPROC)IntGetProcAddress("glSyncTextureINTEL");
	if(!_funcptr_glSyncTextureINTEL) ++numFailed;
	_funcptr_glUnmapTexture2DINTEL = (PFNGLUNMAPTEXTURE2DINTELPROC)IntGetProcAddress("glUnmapTexture2DINTEL");
	if(!_funcptr_glUnmapTexture2DINTEL) ++numFailed;
	_funcptr_glMapTexture2DINTEL = (PFNGLMAPTEXTURE2DINTELPROC)IntGetProcAddress("glMapTexture2DINTEL");
	if(!_funcptr_glMapTexture2DINTEL) ++numFailed;
	return numFailed;
}

PFNGLVERTEXPOINTERVINTELPROC _funcptr_glVertexPointervINTEL = NULL;
PFNGLNORMALPOINTERVINTELPROC _funcptr_glNormalPointervINTEL = NULL;
PFNGLCOLORPOINTERVINTELPROC _funcptr_glColorPointervINTEL = NULL;
PFNGLTEXCOORDPOINTERVINTELPROC _funcptr_glTexCoordPointervINTEL = NULL;

static int LoadExt_INTEL_parallel_arrays()
{
	int numFailed = 0;
	_funcptr_glVertexPointervINTEL = (PFNGLVERTEXPOINTERVINTELPROC)IntGetProcAddress("glVertexPointervINTEL");
	if(!_funcptr_glVertexPointervINTEL) ++numFailed;
	_funcptr_glNormalPointervINTEL = (PFNGLNORMALPOINTERVINTELPROC)IntGetProcAddress("glNormalPointervINTEL");
	if(!_funcptr_glNormalPointervINTEL) ++numFailed;
	_funcptr_glColorPointervINTEL = (PFNGLCOLORPOINTERVINTELPROC)IntGetProcAddress("glColorPointervINTEL");
	if(!_funcptr_glColorPointervINTEL) ++numFailed;
	_funcptr_glTexCoordPointervINTEL = (PFNGLTEXCOORDPOINTERVINTELPROC)IntGetProcAddress("glTexCoordPointervINTEL");
	if(!_funcptr_glTexCoordPointervINTEL) ++numFailed;
	return numFailed;
}

PFNGLRESIZEBUFFERSMESAPROC _funcptr_glResizeBuffersMESA = NULL;

static int LoadExt_MESA_resize_buffers()
{
	int numFailed = 0;
	_funcptr_glResizeBuffersMESA = (PFNGLRESIZEBUFFERSMESAPROC)IntGetProcAddress("glResizeBuffersMESA");
	if(!_funcptr_glResizeBuffersMESA) ++numFailed;
	return numFailed;
}

PFNGLWINDOWPOS2DMESAPROC _funcptr_glWindowPos2dMESA = NULL;
PFNGLWINDOWPOS2DVMESAPROC _funcptr_glWindowPos2dvMESA = NULL;
PFNGLWINDOWPOS2FMESAPROC _funcptr_glWindowPos2fMESA = NULL;
PFNGLWINDOWPOS2FVMESAPROC _funcptr_glWindowPos2fvMESA = NULL;
PFNGLWINDOWPOS2IMESAPROC _funcptr_glWindowPos2iMESA = NULL;
PFNGLWINDOWPOS2IVMESAPROC _funcptr_glWindowPos2ivMESA = NULL;
PFNGLWINDOWPOS2SMESAPROC _funcptr_glWindowPos2sMESA = NULL;
PFNGLWINDOWPOS2SVMESAPROC _funcptr_glWindowPos2svMESA = NULL;
PFNGLWINDOWPOS3DMESAPROC _funcptr_glWindowPos3dMESA = NULL;
PFNGLWINDOWPOS3DVMESAPROC _funcptr_glWindowPos3dvMESA = NULL;
PFNGLWINDOWPOS3FMESAPROC _funcptr_glWindowPos3fMESA = NULL;
PFNGLWINDOWPOS3FVMESAPROC _funcptr_glWindowPos3fvMESA = NULL;
PFNGLWINDOWPOS3IMESAPROC _funcptr_glWindowPos3iMESA = NULL;
PFNGLWINDOWPOS3IVMESAPROC _funcptr_glWindowPos3ivMESA = NULL;
PFNGLWINDOWPOS3SMESAPROC _funcptr_glWindowPos3sMESA = NULL;
PFNGLWINDOWPOS3SVMESAPROC _funcptr_glWindowPos3svMESA = NULL;
PFNGLWINDOWPOS4DMESAPROC _funcptr_glWindowPos4dMESA = NULL;
PFNGLWINDOWPOS4DVMESAPROC _funcptr_glWindowPos4dvMESA = NULL;
PFNGLWINDOWPOS4FMESAPROC _funcptr_glWindowPos4fMESA = NULL;
PFNGLWINDOWPOS4FVMESAPROC _funcptr_glWindowPos4fvMESA = NULL;
PFNGLWINDOWPOS4IMESAPROC _funcptr_glWindowPos4iMESA = NULL;
PFNGLWINDOWPOS4IVMESAPROC _funcptr_glWindowPos4ivMESA = NULL;
PFNGLWINDOWPOS4SMESAPROC _funcptr_glWindowPos4sMESA = NULL;
PFNGLWINDOWPOS4SVMESAPROC _funcptr_glWindowPos4svMESA = NULL;

static int LoadExt_MESA_window_pos()
{
	int numFailed = 0;
	_funcptr_glWindowPos2dMESA = (PFNGLWINDOWPOS2DMESAPROC)IntGetProcAddress("glWindowPos2dMESA");
	if(!_funcptr_glWindowPos2dMESA) ++numFailed;
	_funcptr_glWindowPos2dvMESA = (PFNGLWINDOWPOS2DVMESAPROC)IntGetProcAddress("glWindowPos2dvMESA");
	if(!_funcptr_glWindowPos2dvMESA) ++numFailed;
	_funcptr_glWindowPos2fMESA = (PFNGLWINDOWPOS2FMESAPROC)IntGetProcAddress("glWindowPos2fMESA");
	if(!_funcptr_glWindowPos2fMESA) ++numFailed;
	_funcptr_glWindowPos2fvMESA = (PFNGLWINDOWPOS2FVMESAPROC)IntGetProcAddress("glWindowPos2fvMESA");
	if(!_funcptr_glWindowPos2fvMESA) ++numFailed;
	_funcptr_glWindowPos2iMESA = (PFNGLWINDOWPOS2IMESAPROC)IntGetProcAddress("glWindowPos2iMESA");
	if(!_funcptr_glWindowPos2iMESA) ++numFailed;
	_funcptr_glWindowPos2ivMESA = (PFNGLWINDOWPOS2IVMESAPROC)IntGetProcAddress("glWindowPos2ivMESA");
	if(!_funcptr_glWindowPos2ivMESA) ++numFailed;
	_funcptr_glWindowPos2sMESA = (PFNGLWINDOWPOS2SMESAPROC)IntGetProcAddress("glWindowPos2sMESA");
	if(!_funcptr_glWindowPos2sMESA) ++numFailed;
	_funcptr_glWindowPos2svMESA = (PFNGLWINDOWPOS2SVMESAPROC)IntGetProcAddress("glWindowPos2svMESA");
	if(!_funcptr_glWindowPos2svMESA) ++numFailed;
	_funcptr_glWindowPos3dMESA = (PFNGLWINDOWPOS3DMESAPROC)IntGetProcAddress("glWindowPos3dMESA");
	if(!_funcptr_glWindowPos3dMESA) ++numFailed;
	_funcptr_glWindowPos3dvMESA = (PFNGLWINDOWPOS3DVMESAPROC)IntGetProcAddress("glWindowPos3dvMESA");
	if(!_funcptr_glWindowPos3dvMESA) ++numFailed;
	_funcptr_glWindowPos3fMESA = (PFNGLWINDOWPOS3FMESAPROC)IntGetProcAddress("glWindowPos3fMESA");
	if(!_funcptr_glWindowPos3fMESA) ++numFailed;
	_funcptr_glWindowPos3fvMESA = (PFNGLWINDOWPOS3FVMESAPROC)IntGetProcAddress("glWindowPos3fvMESA");
	if(!_funcptr_glWindowPos3fvMESA) ++numFailed;
	_funcptr_glWindowPos3iMESA = (PFNGLWINDOWPOS3IMESAPROC)IntGetProcAddress("glWindowPos3iMESA");
	if(!_funcptr_glWindowPos3iMESA) ++numFailed;
	_funcptr_glWindowPos3ivMESA = (PFNGLWINDOWPOS3IVMESAPROC)IntGetProcAddress("glWindowPos3ivMESA");
	if(!_funcptr_glWindowPos3ivMESA) ++numFailed;
	_funcptr_glWindowPos3sMESA = (PFNGLWINDOWPOS3SMESAPROC)IntGetProcAddress("glWindowPos3sMESA");
	if(!_funcptr_glWindowPos3sMESA) ++numFailed;
	_funcptr_glWindowPos3svMESA = (PFNGLWINDOWPOS3SVMESAPROC)IntGetProcAddress("glWindowPos3svMESA");
	if(!_funcptr_glWindowPos3svMESA) ++numFailed;
	_funcptr_glWindowPos4dMESA = (PFNGLWINDOWPOS4DMESAPROC)IntGetProcAddress("glWindowPos4dMESA");
	if(!_funcptr_glWindowPos4dMESA) ++numFailed;
	_funcptr_glWindowPos4dvMESA = (PFNGLWINDOWPOS4DVMESAPROC)IntGetProcAddress("glWindowPos4dvMESA");
	if(!_funcptr_glWindowPos4dvMESA) ++numFailed;
	_funcptr_glWindowPos4fMESA = (PFNGLWINDOWPOS4FMESAPROC)IntGetProcAddress("glWindowPos4fMESA");
	if(!_funcptr_glWindowPos4fMESA) ++numFailed;
	_funcptr_glWindowPos4fvMESA = (PFNGLWINDOWPOS4FVMESAPROC)IntGetProcAddress("glWindowPos4fvMESA");
	if(!_funcptr_glWindowPos4fvMESA) ++numFailed;
	_funcptr_glWindowPos4iMESA = (PFNGLWINDOWPOS4IMESAPROC)IntGetProcAddress("glWindowPos4iMESA");
	if(!_funcptr_glWindowPos4iMESA) ++numFailed;
	_funcptr_glWindowPos4ivMESA = (PFNGLWINDOWPOS4IVMESAPROC)IntGetProcAddress("glWindowPos4ivMESA");
	if(!_funcptr_glWindowPos4ivMESA) ++numFailed;
	_funcptr_glWindowPos4sMESA = (PFNGLWINDOWPOS4SMESAPROC)IntGetProcAddress("glWindowPos4sMESA");
	if(!_funcptr_glWindowPos4sMESA) ++numFailed;
	_funcptr_glWindowPos4svMESA = (PFNGLWINDOWPOS4SVMESAPROC)IntGetProcAddress("glWindowPos4svMESA");
	if(!_funcptr_glWindowPos4svMESA) ++numFailed;
	return numFailed;
}

PFNGLBEGINCONDITIONALRENDERNVXPROC _funcptr_glBeginConditionalRenderNVX = NULL;
PFNGLENDCONDITIONALRENDERNVXPROC _funcptr_glEndConditionalRenderNVX = NULL;

static int LoadExt_NVX_conditional_render()
{
	int numFailed = 0;
	_funcptr_glBeginConditionalRenderNVX = (PFNGLBEGINCONDITIONALRENDERNVXPROC)IntGetProcAddress("glBeginConditionalRenderNVX");
	if(!_funcptr_glBeginConditionalRenderNVX) ++numFailed;
	_funcptr_glEndConditionalRenderNVX = (PFNGLENDCONDITIONALRENDERNVXPROC)IntGetProcAddress("glEndConditionalRenderNVX");
	if(!_funcptr_glEndConditionalRenderNVX) ++numFailed;
	return numFailed;
}

PFNGLGETTEXTUREHANDLENVPROC _funcptr_glGetTextureHandleNV = NULL;
PFNGLGETTEXTURESAMPLERHANDLENVPROC _funcptr_glGetTextureSamplerHandleNV = NULL;
PFNGLMAKETEXTUREHANDLERESIDENTNVPROC _funcptr_glMakeTextureHandleResidentNV = NULL;
PFNGLMAKETEXTUREHANDLENONRESIDENTNVPROC _funcptr_glMakeTextureHandleNonResidentNV = NULL;
PFNGLGETIMAGEHANDLENVPROC _funcptr_glGetImageHandleNV = NULL;
PFNGLMAKEIMAGEHANDLERESIDENTNVPROC _funcptr_glMakeImageHandleResidentNV = NULL;
PFNGLMAKEIMAGEHANDLENONRESIDENTNVPROC _funcptr_glMakeImageHandleNonResidentNV = NULL;
PFNGLUNIFORMHANDLEUI64NVPROC _funcptr_glUniformHandleui64NV = NULL;
PFNGLUNIFORMHANDLEUI64VNVPROC _funcptr_glUniformHandleui64vNV = NULL;
PFNGLPROGRAMUNIFORMHANDLEUI64NVPROC _funcptr_glProgramUniformHandleui64NV = NULL;
PFNGLPROGRAMUNIFORMHANDLEUI64VNVPROC _funcptr_glProgramUniformHandleui64vNV = NULL;
PFNGLISTEXTUREHANDLERESIDENTNVPROC _funcptr_glIsTextureHandleResidentNV = NULL;
PFNGLISIMAGEHANDLERESIDENTNVPROC _funcptr_glIsImageHandleResidentNV = NULL;

static int LoadExt_NV_bindless_texture()
{
	int numFailed = 0;
	_funcptr_glGetTextureHandleNV = (PFNGLGETTEXTUREHANDLENVPROC)IntGetProcAddress("glGetTextureHandleNV");
	if(!_funcptr_glGetTextureHandleNV) ++numFailed;
	_funcptr_glGetTextureSamplerHandleNV = (PFNGLGETTEXTURESAMPLERHANDLENVPROC)IntGetProcAddress("glGetTextureSamplerHandleNV");
	if(!_funcptr_glGetTextureSamplerHandleNV) ++numFailed;
	_funcptr_glMakeTextureHandleResidentNV = (PFNGLMAKETEXTUREHANDLERESIDENTNVPROC)IntGetProcAddress("glMakeTextureHandleResidentNV");
	if(!_funcptr_glMakeTextureHandleResidentNV) ++numFailed;
	_funcptr_glMakeTextureHandleNonResidentNV = (PFNGLMAKETEXTUREHANDLENONRESIDENTNVPROC)IntGetProcAddress("glMakeTextureHandleNonResidentNV");
	if(!_funcptr_glMakeTextureHandleNonResidentNV) ++numFailed;
	_funcptr_glGetImageHandleNV = (PFNGLGETIMAGEHANDLENVPROC)IntGetProcAddress("glGetImageHandleNV");
	if(!_funcptr_glGetImageHandleNV) ++numFailed;
	_funcptr_glMakeImageHandleResidentNV = (PFNGLMAKEIMAGEHANDLERESIDENTNVPROC)IntGetProcAddress("glMakeImageHandleResidentNV");
	if(!_funcptr_glMakeImageHandleResidentNV) ++numFailed;
	_funcptr_glMakeImageHandleNonResidentNV = (PFNGLMAKEIMAGEHANDLENONRESIDENTNVPROC)IntGetProcAddress("glMakeImageHandleNonResidentNV");
	if(!_funcptr_glMakeImageHandleNonResidentNV) ++numFailed;
	_funcptr_glUniformHandleui64NV = (PFNGLUNIFORMHANDLEUI64NVPROC)IntGetProcAddress("glUniformHandleui64NV");
	if(!_funcptr_glUniformHandleui64NV) ++numFailed;
	_funcptr_glUniformHandleui64vNV = (PFNGLUNIFORMHANDLEUI64VNVPROC)IntGetProcAddress("glUniformHandleui64vNV");
	if(!_funcptr_glUniformHandleui64vNV) ++numFailed;
	_funcptr_glProgramUniformHandleui64NV = (PFNGLPROGRAMUNIFORMHANDLEUI64NVPROC)IntGetProcAddress("glProgramUniformHandleui64NV");
	if(!_funcptr_glProgramUniformHandleui64NV) ++numFailed;
	_funcptr_glProgramUniformHandleui64vNV = (PFNGLPROGRAMUNIFORMHANDLEUI64VNVPROC)IntGetProcAddress("glProgramUniformHandleui64vNV");
	if(!_funcptr_glProgramUniformHandleui64vNV) ++numFailed;
	_funcptr_glIsTextureHandleResidentNV = (PFNGLISTEXTUREHANDLERESIDENTNVPROC)IntGetProcAddress("glIsTextureHandleResidentNV");
	if(!_funcptr_glIsTextureHandleResidentNV) ++numFailed;
	_funcptr_glIsImageHandleResidentNV = (PFNGLISIMAGEHANDLERESIDENTNVPROC)IntGetProcAddress("glIsImageHandleResidentNV");
	if(!_funcptr_glIsImageHandleResidentNV) ++numFailed;
	return numFailed;
}

PFNGLBEGINCONDITIONALRENDERNVPROC _funcptr_glBeginConditionalRenderNV = NULL;
PFNGLENDCONDITIONALRENDERNVPROC _funcptr_glEndConditionalRenderNV = NULL;

static int LoadExt_NV_conditional_render()
{
	int numFailed = 0;
	_funcptr_glBeginConditionalRenderNV = (PFNGLBEGINCONDITIONALRENDERNVPROC)IntGetProcAddress("glBeginConditionalRenderNV");
	if(!_funcptr_glBeginConditionalRenderNV) ++numFailed;
	_funcptr_glEndConditionalRenderNV = (PFNGLENDCONDITIONALRENDERNVPROC)IntGetProcAddress("glEndConditionalRenderNV");
	if(!_funcptr_glEndConditionalRenderNV) ++numFailed;
	return numFailed;
}

PFNGLCOPYIMAGESUBDATANVPROC _funcptr_glCopyImageSubDataNV = NULL;

static int LoadExt_NV_copy_image()
{
	int numFailed = 0;
	_funcptr_glCopyImageSubDataNV = (PFNGLCOPYIMAGESUBDATANVPROC)IntGetProcAddress("glCopyImageSubDataNV");
	if(!_funcptr_glCopyImageSubDataNV) ++numFailed;
	return numFailed;
}

PFNGLDEPTHRANGEDNVPROC _funcptr_glDepthRangedNV = NULL;
PFNGLCLEARDEPTHDNVPROC _funcptr_glClearDepthdNV = NULL;
PFNGLDEPTHBOUNDSDNVPROC _funcptr_glDepthBoundsdNV = NULL;

static int LoadExt_NV_depth_buffer_float()
{
	int numFailed = 0;
	_funcptr_glDepthRangedNV = (PFNGLDEPTHRANGEDNVPROC)IntGetProcAddress("glDepthRangedNV");
	if(!_funcptr_glDepthRangedNV) ++numFailed;
	_funcptr_glClearDepthdNV = (PFNGLCLEARDEPTHDNVPROC)IntGetProcAddress("glClearDepthdNV");
	if(!_funcptr_glClearDepthdNV) ++numFailed;
	_funcptr_glDepthBoundsdNV = (PFNGLDEPTHBOUNDSDNVPROC)IntGetProcAddress("glDepthBoundsdNV");
	if(!_funcptr_glDepthBoundsdNV) ++numFailed;
	return numFailed;
}

PFNGLDRAWTEXTURENVPROC _funcptr_glDrawTextureNV = NULL;

static int LoadExt_NV_draw_texture()
{
	int numFailed = 0;
	_funcptr_glDrawTextureNV = (PFNGLDRAWTEXTURENVPROC)IntGetProcAddress("glDrawTextureNV");
	if(!_funcptr_glDrawTextureNV) ++numFailed;
	return numFailed;
}

PFNGLMAPCONTROLPOINTSNVPROC _funcptr_glMapControlPointsNV = NULL;
PFNGLMAPPARAMETERIVNVPROC _funcptr_glMapParameterivNV = NULL;
PFNGLMAPPARAMETERFVNVPROC _funcptr_glMapParameterfvNV = NULL;
PFNGLGETMAPCONTROLPOINTSNVPROC _funcptr_glGetMapControlPointsNV = NULL;
PFNGLGETMAPPARAMETERIVNVPROC _funcptr_glGetMapParameterivNV = NULL;
PFNGLGETMAPPARAMETERFVNVPROC _funcptr_glGetMapParameterfvNV = NULL;
PFNGLGETMAPATTRIBPARAMETERIVNVPROC _funcptr_glGetMapAttribParameterivNV = NULL;
PFNGLGETMAPATTRIBPARAMETERFVNVPROC _funcptr_glGetMapAttribParameterfvNV = NULL;
PFNGLEVALMAPSNVPROC _funcptr_glEvalMapsNV = NULL;

static int LoadExt_NV_evaluators()
{
	int numFailed = 0;
	_funcptr_glMapControlPointsNV = (PFNGLMAPCONTROLPOINTSNVPROC)IntGetProcAddress("glMapControlPointsNV");
	if(!_funcptr_glMapControlPointsNV) ++numFailed;
	_funcptr_glMapParameterivNV = (PFNGLMAPPARAMETERIVNVPROC)IntGetProcAddress("glMapParameterivNV");
	if(!_funcptr_glMapParameterivNV) ++numFailed;
	_funcptr_glMapParameterfvNV = (PFNGLMAPPARAMETERFVNVPROC)IntGetProcAddress("glMapParameterfvNV");
	if(!_funcptr_glMapParameterfvNV) ++numFailed;
	_funcptr_glGetMapControlPointsNV = (PFNGLGETMAPCONTROLPOINTSNVPROC)IntGetProcAddress("glGetMapControlPointsNV");
	if(!_funcptr_glGetMapControlPointsNV) ++numFailed;
	_funcptr_glGetMapParameterivNV = (PFNGLGETMAPPARAMETERIVNVPROC)IntGetProcAddress("glGetMapParameterivNV");
	if(!_funcptr_glGetMapParameterivNV) ++numFailed;
	_funcptr_glGetMapParameterfvNV = (PFNGLGETMAPPARAMETERFVNVPROC)IntGetProcAddress("glGetMapParameterfvNV");
	if(!_funcptr_glGetMapParameterfvNV) ++numFailed;
	_funcptr_glGetMapAttribParameterivNV = (PFNGLGETMAPATTRIBPARAMETERIVNVPROC)IntGetProcAddress("glGetMapAttribParameterivNV");
	if(!_funcptr_glGetMapAttribParameterivNV) ++numFailed;
	_funcptr_glGetMapAttribParameterfvNV = (PFNGLGETMAPATTRIBPARAMETERFVNVPROC)IntGetProcAddress("glGetMapAttribParameterfvNV");
	if(!_funcptr_glGetMapAttribParameterfvNV) ++numFailed;
	_funcptr_glEvalMapsNV = (PFNGLEVALMAPSNVPROC)IntGetProcAddress("glEvalMapsNV");
	if(!_funcptr_glEvalMapsNV) ++numFailed;
	return numFailed;
}

PFNGLGETMULTISAMPLEFVNVPROC _funcptr_glGetMultisamplefvNV = NULL;
PFNGLSAMPLEMASKINDEXEDNVPROC _funcptr_glSampleMaskIndexedNV = NULL;
PFNGLTEXRENDERBUFFERNVPROC _funcptr_glTexRenderbufferNV = NULL;

static int LoadExt_NV_explicit_multisample()
{
	int numFailed = 0;
	_funcptr_glGetMultisamplefvNV = (PFNGLGETMULTISAMPLEFVNVPROC)IntGetProcAddress("glGetMultisamplefvNV");
	if(!_funcptr_glGetMultisamplefvNV) ++numFailed;
	_funcptr_glSampleMaskIndexedNV = (PFNGLSAMPLEMASKINDEXEDNVPROC)IntGetProcAddress("glSampleMaskIndexedNV");
	if(!_funcptr_glSampleMaskIndexedNV) ++numFailed;
	_funcptr_glTexRenderbufferNV = (PFNGLTEXRENDERBUFFERNVPROC)IntGetProcAddress("glTexRenderbufferNV");
	if(!_funcptr_glTexRenderbufferNV) ++numFailed;
	return numFailed;
}

PFNGLDELETEFENCESNVPROC _funcptr_glDeleteFencesNV = NULL;
PFNGLGENFENCESNVPROC _funcptr_glGenFencesNV = NULL;
PFNGLISFENCENVPROC _funcptr_glIsFenceNV = NULL;
PFNGLTESTFENCENVPROC _funcptr_glTestFenceNV = NULL;
PFNGLGETFENCEIVNVPROC _funcptr_glGetFenceivNV = NULL;
PFNGLFINISHFENCENVPROC _funcptr_glFinishFenceNV = NULL;
PFNGLSETFENCENVPROC _funcptr_glSetFenceNV = NULL;

static int LoadExt_NV_fence()
{
	int numFailed = 0;
	_funcptr_glDeleteFencesNV = (PFNGLDELETEFENCESNVPROC)IntGetProcAddress("glDeleteFencesNV");
	if(!_funcptr_glDeleteFencesNV) ++numFailed;
	_funcptr_glGenFencesNV = (PFNGLGENFENCESNVPROC)IntGetProcAddress("glGenFencesNV");
	if(!_funcptr_glGenFencesNV) ++numFailed;
	_funcptr_glIsFenceNV = (PFNGLISFENCENVPROC)IntGetProcAddress("glIsFenceNV");
	if(!_funcptr_glIsFenceNV) ++numFailed;
	_funcptr_glTestFenceNV = (PFNGLTESTFENCENVPROC)IntGetProcAddress("glTestFenceNV");
	if(!_funcptr_glTestFenceNV) ++numFailed;
	_funcptr_glGetFenceivNV = (PFNGLGETFENCEIVNVPROC)IntGetProcAddress("glGetFenceivNV");
	if(!_funcptr_glGetFenceivNV) ++numFailed;
	_funcptr_glFinishFenceNV = (PFNGLFINISHFENCENVPROC)IntGetProcAddress("glFinishFenceNV");
	if(!_funcptr_glFinishFenceNV) ++numFailed;
	_funcptr_glSetFenceNV = (PFNGLSETFENCENVPROC)IntGetProcAddress("glSetFenceNV");
	if(!_funcptr_glSetFenceNV) ++numFailed;
	return numFailed;
}

PFNGLPROGRAMNAMEDPARAMETER4FNVPROC _funcptr_glProgramNamedParameter4fNV = NULL;
PFNGLPROGRAMNAMEDPARAMETER4FVNVPROC _funcptr_glProgramNamedParameter4fvNV = NULL;
PFNGLPROGRAMNAMEDPARAMETER4DNVPROC _funcptr_glProgramNamedParameter4dNV = NULL;
PFNGLPROGRAMNAMEDPARAMETER4DVNVPROC _funcptr_glProgramNamedParameter4dvNV = NULL;
PFNGLGETPROGRAMNAMEDPARAMETERFVNVPROC _funcptr_glGetProgramNamedParameterfvNV = NULL;
PFNGLGETPROGRAMNAMEDPARAMETERDVNVPROC _funcptr_glGetProgramNamedParameterdvNV = NULL;

static int LoadExt_NV_fragment_program()
{
	int numFailed = 0;
	_funcptr_glProgramNamedParameter4fNV = (PFNGLPROGRAMNAMEDPARAMETER4FNVPROC)IntGetProcAddress("glProgramNamedParameter4fNV");
	if(!_funcptr_glProgramNamedParameter4fNV) ++numFailed;
	_funcptr_glProgramNamedParameter4fvNV = (PFNGLPROGRAMNAMEDPARAMETER4FVNVPROC)IntGetProcAddress("glProgramNamedParameter4fvNV");
	if(!_funcptr_glProgramNamedParameter4fvNV) ++numFailed;
	_funcptr_glProgramNamedParameter4dNV = (PFNGLPROGRAMNAMEDPARAMETER4DNVPROC)IntGetProcAddress("glProgramNamedParameter4dNV");
	if(!_funcptr_glProgramNamedParameter4dNV) ++numFailed;
	_funcptr_glProgramNamedParameter4dvNV = (PFNGLPROGRAMNAMEDPARAMETER4DVNVPROC)IntGetProcAddress("glProgramNamedParameter4dvNV");
	if(!_funcptr_glProgramNamedParameter4dvNV) ++numFailed;
	_funcptr_glGetProgramNamedParameterfvNV = (PFNGLGETPROGRAMNAMEDPARAMETERFVNVPROC)IntGetProcAddress("glGetProgramNamedParameterfvNV");
	if(!_funcptr_glGetProgramNamedParameterfvNV) ++numFailed;
	_funcptr_glGetProgramNamedParameterdvNV = (PFNGLGETPROGRAMNAMEDPARAMETERDVNVPROC)IntGetProcAddress("glGetProgramNamedParameterdvNV");
	if(!_funcptr_glGetProgramNamedParameterdvNV) ++numFailed;
	return numFailed;
}

PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC _funcptr_glRenderbufferStorageMultisampleCoverageNV = NULL;

static int LoadExt_NV_framebuffer_multisample_coverage()
{
	int numFailed = 0;
	_funcptr_glRenderbufferStorageMultisampleCoverageNV = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC)IntGetProcAddress("glRenderbufferStorageMultisampleCoverageNV");
	if(!_funcptr_glRenderbufferStorageMultisampleCoverageNV) ++numFailed;
	return numFailed;
}

PFNGLPROGRAMVERTEXLIMITNVPROC _funcptr_glProgramVertexLimitNV = NULL;
PFNGLFRAMEBUFFERTEXTUREEXTPROC _funcptr_glFramebufferTextureEXT = NULL;
PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC _funcptr_glFramebufferTextureLayerEXT = NULL;
PFNGLFRAMEBUFFERTEXTUREFACEEXTPROC _funcptr_glFramebufferTextureFaceEXT = NULL;

static int LoadExt_NV_geometry_program4()
{
	int numFailed = 0;
	_funcptr_glProgramVertexLimitNV = (PFNGLPROGRAMVERTEXLIMITNVPROC)IntGetProcAddress("glProgramVertexLimitNV");
	if(!_funcptr_glProgramVertexLimitNV) ++numFailed;
	_funcptr_glFramebufferTextureEXT = (PFNGLFRAMEBUFFERTEXTUREEXTPROC)IntGetProcAddress("glFramebufferTextureEXT");
	if(!_funcptr_glFramebufferTextureEXT) ++numFailed;
	_funcptr_glFramebufferTextureLayerEXT = (PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC)IntGetProcAddress("glFramebufferTextureLayerEXT");
	if(!_funcptr_glFramebufferTextureLayerEXT) ++numFailed;
	_funcptr_glFramebufferTextureFaceEXT = (PFNGLFRAMEBUFFERTEXTUREFACEEXTPROC)IntGetProcAddress("glFramebufferTextureFaceEXT");
	if(!_funcptr_glFramebufferTextureFaceEXT) ++numFailed;
	return numFailed;
}

PFNGLPROGRAMLOCALPARAMETERI4INVPROC _funcptr_glProgramLocalParameterI4iNV = NULL;
PFNGLPROGRAMLOCALPARAMETERI4IVNVPROC _funcptr_glProgramLocalParameterI4ivNV = NULL;
PFNGLPROGRAMLOCALPARAMETERSI4IVNVPROC _funcptr_glProgramLocalParametersI4ivNV = NULL;
PFNGLPROGRAMLOCALPARAMETERI4UINVPROC _funcptr_glProgramLocalParameterI4uiNV = NULL;
PFNGLPROGRAMLOCALPARAMETERI4UIVNVPROC _funcptr_glProgramLocalParameterI4uivNV = NULL;
PFNGLPROGRAMLOCALPARAMETERSI4UIVNVPROC _funcptr_glProgramLocalParametersI4uivNV = NULL;
PFNGLPROGRAMENVPARAMETERI4INVPROC _funcptr_glProgramEnvParameterI4iNV = NULL;
PFNGLPROGRAMENVPARAMETERI4IVNVPROC _funcptr_glProgramEnvParameterI4ivNV = NULL;
PFNGLPROGRAMENVPARAMETERSI4IVNVPROC _funcptr_glProgramEnvParametersI4ivNV = NULL;
PFNGLPROGRAMENVPARAMETERI4UINVPROC _funcptr_glProgramEnvParameterI4uiNV = NULL;
PFNGLPROGRAMENVPARAMETERI4UIVNVPROC _funcptr_glProgramEnvParameterI4uivNV = NULL;
PFNGLPROGRAMENVPARAMETERSI4UIVNVPROC _funcptr_glProgramEnvParametersI4uivNV = NULL;
PFNGLGETPROGRAMLOCALPARAMETERIIVNVPROC _funcptr_glGetProgramLocalParameterIivNV = NULL;
PFNGLGETPROGRAMLOCALPARAMETERIUIVNVPROC _funcptr_glGetProgramLocalParameterIuivNV = NULL;
PFNGLGETPROGRAMENVPARAMETERIIVNVPROC _funcptr_glGetProgramEnvParameterIivNV = NULL;
PFNGLGETPROGRAMENVPARAMETERIUIVNVPROC _funcptr_glGetProgramEnvParameterIuivNV = NULL;

static int LoadExt_NV_gpu_program4()
{
	int numFailed = 0;
	_funcptr_glProgramLocalParameterI4iNV = (PFNGLPROGRAMLOCALPARAMETERI4INVPROC)IntGetProcAddress("glProgramLocalParameterI4iNV");
	if(!_funcptr_glProgramLocalParameterI4iNV) ++numFailed;
	_funcptr_glProgramLocalParameterI4ivNV = (PFNGLPROGRAMLOCALPARAMETERI4IVNVPROC)IntGetProcAddress("glProgramLocalParameterI4ivNV");
	if(!_funcptr_glProgramLocalParameterI4ivNV) ++numFailed;
	_funcptr_glProgramLocalParametersI4ivNV = (PFNGLPROGRAMLOCALPARAMETERSI4IVNVPROC)IntGetProcAddress("glProgramLocalParametersI4ivNV");
	if(!_funcptr_glProgramLocalParametersI4ivNV) ++numFailed;
	_funcptr_glProgramLocalParameterI4uiNV = (PFNGLPROGRAMLOCALPARAMETERI4UINVPROC)IntGetProcAddress("glProgramLocalParameterI4uiNV");
	if(!_funcptr_glProgramLocalParameterI4uiNV) ++numFailed;
	_funcptr_glProgramLocalParameterI4uivNV = (PFNGLPROGRAMLOCALPARAMETERI4UIVNVPROC)IntGetProcAddress("glProgramLocalParameterI4uivNV");
	if(!_funcptr_glProgramLocalParameterI4uivNV) ++numFailed;
	_funcptr_glProgramLocalParametersI4uivNV = (PFNGLPROGRAMLOCALPARAMETERSI4UIVNVPROC)IntGetProcAddress("glProgramLocalParametersI4uivNV");
	if(!_funcptr_glProgramLocalParametersI4uivNV) ++numFailed;
	_funcptr_glProgramEnvParameterI4iNV = (PFNGLPROGRAMENVPARAMETERI4INVPROC)IntGetProcAddress("glProgramEnvParameterI4iNV");
	if(!_funcptr_glProgramEnvParameterI4iNV) ++numFailed;
	_funcptr_glProgramEnvParameterI4ivNV = (PFNGLPROGRAMENVPARAMETERI4IVNVPROC)IntGetProcAddress("glProgramEnvParameterI4ivNV");
	if(!_funcptr_glProgramEnvParameterI4ivNV) ++numFailed;
	_funcptr_glProgramEnvParametersI4ivNV = (PFNGLPROGRAMENVPARAMETERSI4IVNVPROC)IntGetProcAddress("glProgramEnvParametersI4ivNV");
	if(!_funcptr_glProgramEnvParametersI4ivNV) ++numFailed;
	_funcptr_glProgramEnvParameterI4uiNV = (PFNGLPROGRAMENVPARAMETERI4UINVPROC)IntGetProcAddress("glProgramEnvParameterI4uiNV");
	if(!_funcptr_glProgramEnvParameterI4uiNV) ++numFailed;
	_funcptr_glProgramEnvParameterI4uivNV = (PFNGLPROGRAMENVPARAMETERI4UIVNVPROC)IntGetProcAddress("glProgramEnvParameterI4uivNV");
	if(!_funcptr_glProgramEnvParameterI4uivNV) ++numFailed;
	_funcptr_glProgramEnvParametersI4uivNV = (PFNGLPROGRAMENVPARAMETERSI4UIVNVPROC)IntGetProcAddress("glProgramEnvParametersI4uivNV");
	if(!_funcptr_glProgramEnvParametersI4uivNV) ++numFailed;
	_funcptr_glGetProgramLocalParameterIivNV = (PFNGLGETPROGRAMLOCALPARAMETERIIVNVPROC)IntGetProcAddress("glGetProgramLocalParameterIivNV");
	if(!_funcptr_glGetProgramLocalParameterIivNV) ++numFailed;
	_funcptr_glGetProgramLocalParameterIuivNV = (PFNGLGETPROGRAMLOCALPARAMETERIUIVNVPROC)IntGetProcAddress("glGetProgramLocalParameterIuivNV");
	if(!_funcptr_glGetProgramLocalParameterIuivNV) ++numFailed;
	_funcptr_glGetProgramEnvParameterIivNV = (PFNGLGETPROGRAMENVPARAMETERIIVNVPROC)IntGetProcAddress("glGetProgramEnvParameterIivNV");
	if(!_funcptr_glGetProgramEnvParameterIivNV) ++numFailed;
	_funcptr_glGetProgramEnvParameterIuivNV = (PFNGLGETPROGRAMENVPARAMETERIUIVNVPROC)IntGetProcAddress("glGetProgramEnvParameterIuivNV");
	if(!_funcptr_glGetProgramEnvParameterIuivNV) ++numFailed;
	return numFailed;
}

PFNGLPROGRAMSUBROUTINEPARAMETERSUIVNVPROC _funcptr_glProgramSubroutineParametersuivNV = NULL;
PFNGLGETPROGRAMSUBROUTINEPARAMETERUIVNVPROC _funcptr_glGetProgramSubroutineParameteruivNV = NULL;

static int LoadExt_NV_gpu_program5()
{
	int numFailed = 0;
	_funcptr_glProgramSubroutineParametersuivNV = (PFNGLPROGRAMSUBROUTINEPARAMETERSUIVNVPROC)IntGetProcAddress("glProgramSubroutineParametersuivNV");
	if(!_funcptr_glProgramSubroutineParametersuivNV) ++numFailed;
	_funcptr_glGetProgramSubroutineParameteruivNV = (PFNGLGETPROGRAMSUBROUTINEPARAMETERUIVNVPROC)IntGetProcAddress("glGetProgramSubroutineParameteruivNV");
	if(!_funcptr_glGetProgramSubroutineParameteruivNV) ++numFailed;
	return numFailed;
}

PFNGLUNIFORM1I64NVPROC _funcptr_glUniform1i64NV = NULL;
PFNGLUNIFORM2I64NVPROC _funcptr_glUniform2i64NV = NULL;
PFNGLUNIFORM3I64NVPROC _funcptr_glUniform3i64NV = NULL;
PFNGLUNIFORM4I64NVPROC _funcptr_glUniform4i64NV = NULL;
PFNGLUNIFORM1I64VNVPROC _funcptr_glUniform1i64vNV = NULL;
PFNGLUNIFORM2I64VNVPROC _funcptr_glUniform2i64vNV = NULL;
PFNGLUNIFORM3I64VNVPROC _funcptr_glUniform3i64vNV = NULL;
PFNGLUNIFORM4I64VNVPROC _funcptr_glUniform4i64vNV = NULL;
PFNGLUNIFORM1UI64NVPROC _funcptr_glUniform1ui64NV = NULL;
PFNGLUNIFORM2UI64NVPROC _funcptr_glUniform2ui64NV = NULL;
PFNGLUNIFORM3UI64NVPROC _funcptr_glUniform3ui64NV = NULL;
PFNGLUNIFORM4UI64NVPROC _funcptr_glUniform4ui64NV = NULL;
PFNGLUNIFORM1UI64VNVPROC _funcptr_glUniform1ui64vNV = NULL;
PFNGLUNIFORM2UI64VNVPROC _funcptr_glUniform2ui64vNV = NULL;
PFNGLUNIFORM3UI64VNVPROC _funcptr_glUniform3ui64vNV = NULL;
PFNGLUNIFORM4UI64VNVPROC _funcptr_glUniform4ui64vNV = NULL;
PFNGLGETUNIFORMI64VNVPROC _funcptr_glGetUniformi64vNV = NULL;
PFNGLPROGRAMUNIFORM1I64NVPROC _funcptr_glProgramUniform1i64NV = NULL;
PFNGLPROGRAMUNIFORM2I64NVPROC _funcptr_glProgramUniform2i64NV = NULL;
PFNGLPROGRAMUNIFORM3I64NVPROC _funcptr_glProgramUniform3i64NV = NULL;
PFNGLPROGRAMUNIFORM4I64NVPROC _funcptr_glProgramUniform4i64NV = NULL;
PFNGLPROGRAMUNIFORM1I64VNVPROC _funcptr_glProgramUniform1i64vNV = NULL;
PFNGLPROGRAMUNIFORM2I64VNVPROC _funcptr_glProgramUniform2i64vNV = NULL;
PFNGLPROGRAMUNIFORM3I64VNVPROC _funcptr_glProgramUniform3i64vNV = NULL;
PFNGLPROGRAMUNIFORM4I64VNVPROC _funcptr_glProgramUniform4i64vNV = NULL;
PFNGLPROGRAMUNIFORM1UI64NVPROC _funcptr_glProgramUniform1ui64NV = NULL;
PFNGLPROGRAMUNIFORM2UI64NVPROC _funcptr_glProgramUniform2ui64NV = NULL;
PFNGLPROGRAMUNIFORM3UI64NVPROC _funcptr_glProgramUniform3ui64NV = NULL;
PFNGLPROGRAMUNIFORM4UI64NVPROC _funcptr_glProgramUniform4ui64NV = NULL;
PFNGLPROGRAMUNIFORM1UI64VNVPROC _funcptr_glProgramUniform1ui64vNV = NULL;
PFNGLPROGRAMUNIFORM2UI64VNVPROC _funcptr_glProgramUniform2ui64vNV = NULL;
PFNGLPROGRAMUNIFORM3UI64VNVPROC _funcptr_glProgramUniform3ui64vNV = NULL;
PFNGLPROGRAMUNIFORM4UI64VNVPROC _funcptr_glProgramUniform4ui64vNV = NULL;

static int LoadExt_NV_gpu_shader5()
{
	int numFailed = 0;
	_funcptr_glUniform1i64NV = (PFNGLUNIFORM1I64NVPROC)IntGetProcAddress("glUniform1i64NV");
	if(!_funcptr_glUniform1i64NV) ++numFailed;
	_funcptr_glUniform2i64NV = (PFNGLUNIFORM2I64NVPROC)IntGetProcAddress("glUniform2i64NV");
	if(!_funcptr_glUniform2i64NV) ++numFailed;
	_funcptr_glUniform3i64NV = (PFNGLUNIFORM3I64NVPROC)IntGetProcAddress("glUniform3i64NV");
	if(!_funcptr_glUniform3i64NV) ++numFailed;
	_funcptr_glUniform4i64NV = (PFNGLUNIFORM4I64NVPROC)IntGetProcAddress("glUniform4i64NV");
	if(!_funcptr_glUniform4i64NV) ++numFailed;
	_funcptr_glUniform1i64vNV = (PFNGLUNIFORM1I64VNVPROC)IntGetProcAddress("glUniform1i64vNV");
	if(!_funcptr_glUniform1i64vNV) ++numFailed;
	_funcptr_glUniform2i64vNV = (PFNGLUNIFORM2I64VNVPROC)IntGetProcAddress("glUniform2i64vNV");
	if(!_funcptr_glUniform2i64vNV) ++numFailed;
	_funcptr_glUniform3i64vNV = (PFNGLUNIFORM3I64VNVPROC)IntGetProcAddress("glUniform3i64vNV");
	if(!_funcptr_glUniform3i64vNV) ++numFailed;
	_funcptr_glUniform4i64vNV = (PFNGLUNIFORM4I64VNVPROC)IntGetProcAddress("glUniform4i64vNV");
	if(!_funcptr_glUniform4i64vNV) ++numFailed;
	_funcptr_glUniform1ui64NV = (PFNGLUNIFORM1UI64NVPROC)IntGetProcAddress("glUniform1ui64NV");
	if(!_funcptr_glUniform1ui64NV) ++numFailed;
	_funcptr_glUniform2ui64NV = (PFNGLUNIFORM2UI64NVPROC)IntGetProcAddress("glUniform2ui64NV");
	if(!_funcptr_glUniform2ui64NV) ++numFailed;
	_funcptr_glUniform3ui64NV = (PFNGLUNIFORM3UI64NVPROC)IntGetProcAddress("glUniform3ui64NV");
	if(!_funcptr_glUniform3ui64NV) ++numFailed;
	_funcptr_glUniform4ui64NV = (PFNGLUNIFORM4UI64NVPROC)IntGetProcAddress("glUniform4ui64NV");
	if(!_funcptr_glUniform4ui64NV) ++numFailed;
	_funcptr_glUniform1ui64vNV = (PFNGLUNIFORM1UI64VNVPROC)IntGetProcAddress("glUniform1ui64vNV");
	if(!_funcptr_glUniform1ui64vNV) ++numFailed;
	_funcptr_glUniform2ui64vNV = (PFNGLUNIFORM2UI64VNVPROC)IntGetProcAddress("glUniform2ui64vNV");
	if(!_funcptr_glUniform2ui64vNV) ++numFailed;
	_funcptr_glUniform3ui64vNV = (PFNGLUNIFORM3UI64VNVPROC)IntGetProcAddress("glUniform3ui64vNV");
	if(!_funcptr_glUniform3ui64vNV) ++numFailed;
	_funcptr_glUniform4ui64vNV = (PFNGLUNIFORM4UI64VNVPROC)IntGetProcAddress("glUniform4ui64vNV");
	if(!_funcptr_glUniform4ui64vNV) ++numFailed;
	_funcptr_glGetUniformi64vNV = (PFNGLGETUNIFORMI64VNVPROC)IntGetProcAddress("glGetUniformi64vNV");
	if(!_funcptr_glGetUniformi64vNV) ++numFailed;
	_funcptr_glProgramUniform1i64NV = (PFNGLPROGRAMUNIFORM1I64NVPROC)IntGetProcAddress("glProgramUniform1i64NV");
	if(!_funcptr_glProgramUniform1i64NV) ++numFailed;
	_funcptr_glProgramUniform2i64NV = (PFNGLPROGRAMUNIFORM2I64NVPROC)IntGetProcAddress("glProgramUniform2i64NV");
	if(!_funcptr_glProgramUniform2i64NV) ++numFailed;
	_funcptr_glProgramUniform3i64NV = (PFNGLPROGRAMUNIFORM3I64NVPROC)IntGetProcAddress("glProgramUniform3i64NV");
	if(!_funcptr_glProgramUniform3i64NV) ++numFailed;
	_funcptr_glProgramUniform4i64NV = (PFNGLPROGRAMUNIFORM4I64NVPROC)IntGetProcAddress("glProgramUniform4i64NV");
	if(!_funcptr_glProgramUniform4i64NV) ++numFailed;
	_funcptr_glProgramUniform1i64vNV = (PFNGLPROGRAMUNIFORM1I64VNVPROC)IntGetProcAddress("glProgramUniform1i64vNV");
	if(!_funcptr_glProgramUniform1i64vNV) ++numFailed;
	_funcptr_glProgramUniform2i64vNV = (PFNGLPROGRAMUNIFORM2I64VNVPROC)IntGetProcAddress("glProgramUniform2i64vNV");
	if(!_funcptr_glProgramUniform2i64vNV) ++numFailed;
	_funcptr_glProgramUniform3i64vNV = (PFNGLPROGRAMUNIFORM3I64VNVPROC)IntGetProcAddress("glProgramUniform3i64vNV");
	if(!_funcptr_glProgramUniform3i64vNV) ++numFailed;
	_funcptr_glProgramUniform4i64vNV = (PFNGLPROGRAMUNIFORM4I64VNVPROC)IntGetProcAddress("glProgramUniform4i64vNV");
	if(!_funcptr_glProgramUniform4i64vNV) ++numFailed;
	_funcptr_glProgramUniform1ui64NV = (PFNGLPROGRAMUNIFORM1UI64NVPROC)IntGetProcAddress("glProgramUniform1ui64NV");
	if(!_funcptr_glProgramUniform1ui64NV) ++numFailed;
	_funcptr_glProgramUniform2ui64NV = (PFNGLPROGRAMUNIFORM2UI64NVPROC)IntGetProcAddress("glProgramUniform2ui64NV");
	if(!_funcptr_glProgramUniform2ui64NV) ++numFailed;
	_funcptr_glProgramUniform3ui64NV = (PFNGLPROGRAMUNIFORM3UI64NVPROC)IntGetProcAddress("glProgramUniform3ui64NV");
	if(!_funcptr_glProgramUniform3ui64NV) ++numFailed;
	_funcptr_glProgramUniform4ui64NV = (PFNGLPROGRAMUNIFORM4UI64NVPROC)IntGetProcAddress("glProgramUniform4ui64NV");
	if(!_funcptr_glProgramUniform4ui64NV) ++numFailed;
	_funcptr_glProgramUniform1ui64vNV = (PFNGLPROGRAMUNIFORM1UI64VNVPROC)IntGetProcAddress("glProgramUniform1ui64vNV");
	if(!_funcptr_glProgramUniform1ui64vNV) ++numFailed;
	_funcptr_glProgramUniform2ui64vNV = (PFNGLPROGRAMUNIFORM2UI64VNVPROC)IntGetProcAddress("glProgramUniform2ui64vNV");
	if(!_funcptr_glProgramUniform2ui64vNV) ++numFailed;
	_funcptr_glProgramUniform3ui64vNV = (PFNGLPROGRAMUNIFORM3UI64VNVPROC)IntGetProcAddress("glProgramUniform3ui64vNV");
	if(!_funcptr_glProgramUniform3ui64vNV) ++numFailed;
	_funcptr_glProgramUniform4ui64vNV = (PFNGLPROGRAMUNIFORM4UI64VNVPROC)IntGetProcAddress("glProgramUniform4ui64vNV");
	if(!_funcptr_glProgramUniform4ui64vNV) ++numFailed;
	return numFailed;
}

PFNGLVERTEX2HNVPROC _funcptr_glVertex2hNV = NULL;
PFNGLVERTEX2HVNVPROC _funcptr_glVertex2hvNV = NULL;
PFNGLVERTEX3HNVPROC _funcptr_glVertex3hNV = NULL;
PFNGLVERTEX3HVNVPROC _funcptr_glVertex3hvNV = NULL;
PFNGLVERTEX4HNVPROC _funcptr_glVertex4hNV = NULL;
PFNGLVERTEX4HVNVPROC _funcptr_glVertex4hvNV = NULL;
PFNGLNORMAL3HNVPROC _funcptr_glNormal3hNV = NULL;
PFNGLNORMAL3HVNVPROC _funcptr_glNormal3hvNV = NULL;
PFNGLCOLOR3HNVPROC _funcptr_glColor3hNV = NULL;
PFNGLCOLOR3HVNVPROC _funcptr_glColor3hvNV = NULL;
PFNGLCOLOR4HNVPROC _funcptr_glColor4hNV = NULL;
PFNGLCOLOR4HVNVPROC _funcptr_glColor4hvNV = NULL;
PFNGLTEXCOORD1HNVPROC _funcptr_glTexCoord1hNV = NULL;
PFNGLTEXCOORD1HVNVPROC _funcptr_glTexCoord1hvNV = NULL;
PFNGLTEXCOORD2HNVPROC _funcptr_glTexCoord2hNV = NULL;
PFNGLTEXCOORD2HVNVPROC _funcptr_glTexCoord2hvNV = NULL;
PFNGLTEXCOORD3HNVPROC _funcptr_glTexCoord3hNV = NULL;
PFNGLTEXCOORD3HVNVPROC _funcptr_glTexCoord3hvNV = NULL;
PFNGLTEXCOORD4HNVPROC _funcptr_glTexCoord4hNV = NULL;
PFNGLTEXCOORD4HVNVPROC _funcptr_glTexCoord4hvNV = NULL;
PFNGLMULTITEXCOORD1HNVPROC _funcptr_glMultiTexCoord1hNV = NULL;
PFNGLMULTITEXCOORD1HVNVPROC _funcptr_glMultiTexCoord1hvNV = NULL;
PFNGLMULTITEXCOORD2HNVPROC _funcptr_glMultiTexCoord2hNV = NULL;
PFNGLMULTITEXCOORD2HVNVPROC _funcptr_glMultiTexCoord2hvNV = NULL;
PFNGLMULTITEXCOORD3HNVPROC _funcptr_glMultiTexCoord3hNV = NULL;
PFNGLMULTITEXCOORD3HVNVPROC _funcptr_glMultiTexCoord3hvNV = NULL;
PFNGLMULTITEXCOORD4HNVPROC _funcptr_glMultiTexCoord4hNV = NULL;
PFNGLMULTITEXCOORD4HVNVPROC _funcptr_glMultiTexCoord4hvNV = NULL;
PFNGLFOGCOORDHNVPROC _funcptr_glFogCoordhNV = NULL;
PFNGLFOGCOORDHVNVPROC _funcptr_glFogCoordhvNV = NULL;
PFNGLSECONDARYCOLOR3HNVPROC _funcptr_glSecondaryColor3hNV = NULL;
PFNGLSECONDARYCOLOR3HVNVPROC _funcptr_glSecondaryColor3hvNV = NULL;
PFNGLVERTEXWEIGHTHNVPROC _funcptr_glVertexWeighthNV = NULL;
PFNGLVERTEXWEIGHTHVNVPROC _funcptr_glVertexWeighthvNV = NULL;
PFNGLVERTEXATTRIB1HNVPROC _funcptr_glVertexAttrib1hNV = NULL;
PFNGLVERTEXATTRIB1HVNVPROC _funcptr_glVertexAttrib1hvNV = NULL;
PFNGLVERTEXATTRIB2HNVPROC _funcptr_glVertexAttrib2hNV = NULL;
PFNGLVERTEXATTRIB2HVNVPROC _funcptr_glVertexAttrib2hvNV = NULL;
PFNGLVERTEXATTRIB3HNVPROC _funcptr_glVertexAttrib3hNV = NULL;
PFNGLVERTEXATTRIB3HVNVPROC _funcptr_glVertexAttrib3hvNV = NULL;
PFNGLVERTEXATTRIB4HNVPROC _funcptr_glVertexAttrib4hNV = NULL;
PFNGLVERTEXATTRIB4HVNVPROC _funcptr_glVertexAttrib4hvNV = NULL;
PFNGLVERTEXATTRIBS1HVNVPROC _funcptr_glVertexAttribs1hvNV = NULL;
PFNGLVERTEXATTRIBS2HVNVPROC _funcptr_glVertexAttribs2hvNV = NULL;
PFNGLVERTEXATTRIBS3HVNVPROC _funcptr_glVertexAttribs3hvNV = NULL;
PFNGLVERTEXATTRIBS4HVNVPROC _funcptr_glVertexAttribs4hvNV = NULL;

static int LoadExt_NV_half_float()
{
	int numFailed = 0;
	_funcptr_glVertex2hNV = (PFNGLVERTEX2HNVPROC)IntGetProcAddress("glVertex2hNV");
	if(!_funcptr_glVertex2hNV) ++numFailed;
	_funcptr_glVertex2hvNV = (PFNGLVERTEX2HVNVPROC)IntGetProcAddress("glVertex2hvNV");
	if(!_funcptr_glVertex2hvNV) ++numFailed;
	_funcptr_glVertex3hNV = (PFNGLVERTEX3HNVPROC)IntGetProcAddress("glVertex3hNV");
	if(!_funcptr_glVertex3hNV) ++numFailed;
	_funcptr_glVertex3hvNV = (PFNGLVERTEX3HVNVPROC)IntGetProcAddress("glVertex3hvNV");
	if(!_funcptr_glVertex3hvNV) ++numFailed;
	_funcptr_glVertex4hNV = (PFNGLVERTEX4HNVPROC)IntGetProcAddress("glVertex4hNV");
	if(!_funcptr_glVertex4hNV) ++numFailed;
	_funcptr_glVertex4hvNV = (PFNGLVERTEX4HVNVPROC)IntGetProcAddress("glVertex4hvNV");
	if(!_funcptr_glVertex4hvNV) ++numFailed;
	_funcptr_glNormal3hNV = (PFNGLNORMAL3HNVPROC)IntGetProcAddress("glNormal3hNV");
	if(!_funcptr_glNormal3hNV) ++numFailed;
	_funcptr_glNormal3hvNV = (PFNGLNORMAL3HVNVPROC)IntGetProcAddress("glNormal3hvNV");
	if(!_funcptr_glNormal3hvNV) ++numFailed;
	_funcptr_glColor3hNV = (PFNGLCOLOR3HNVPROC)IntGetProcAddress("glColor3hNV");
	if(!_funcptr_glColor3hNV) ++numFailed;
	_funcptr_glColor3hvNV = (PFNGLCOLOR3HVNVPROC)IntGetProcAddress("glColor3hvNV");
	if(!_funcptr_glColor3hvNV) ++numFailed;
	_funcptr_glColor4hNV = (PFNGLCOLOR4HNVPROC)IntGetProcAddress("glColor4hNV");
	if(!_funcptr_glColor4hNV) ++numFailed;
	_funcptr_glColor4hvNV = (PFNGLCOLOR4HVNVPROC)IntGetProcAddress("glColor4hvNV");
	if(!_funcptr_glColor4hvNV) ++numFailed;
	_funcptr_glTexCoord1hNV = (PFNGLTEXCOORD1HNVPROC)IntGetProcAddress("glTexCoord1hNV");
	if(!_funcptr_glTexCoord1hNV) ++numFailed;
	_funcptr_glTexCoord1hvNV = (PFNGLTEXCOORD1HVNVPROC)IntGetProcAddress("glTexCoord1hvNV");
	if(!_funcptr_glTexCoord1hvNV) ++numFailed;
	_funcptr_glTexCoord2hNV = (PFNGLTEXCOORD2HNVPROC)IntGetProcAddress("glTexCoord2hNV");
	if(!_funcptr_glTexCoord2hNV) ++numFailed;
	_funcptr_glTexCoord2hvNV = (PFNGLTEXCOORD2HVNVPROC)IntGetProcAddress("glTexCoord2hvNV");
	if(!_funcptr_glTexCoord2hvNV) ++numFailed;
	_funcptr_glTexCoord3hNV = (PFNGLTEXCOORD3HNVPROC)IntGetProcAddress("glTexCoord3hNV");
	if(!_funcptr_glTexCoord3hNV) ++numFailed;
	_funcptr_glTexCoord3hvNV = (PFNGLTEXCOORD3HVNVPROC)IntGetProcAddress("glTexCoord3hvNV");
	if(!_funcptr_glTexCoord3hvNV) ++numFailed;
	_funcptr_glTexCoord4hNV = (PFNGLTEXCOORD4HNVPROC)IntGetProcAddress("glTexCoord4hNV");
	if(!_funcptr_glTexCoord4hNV) ++numFailed;
	_funcptr_glTexCoord4hvNV = (PFNGLTEXCOORD4HVNVPROC)IntGetProcAddress("glTexCoord4hvNV");
	if(!_funcptr_glTexCoord4hvNV) ++numFailed;
	_funcptr_glMultiTexCoord1hNV = (PFNGLMULTITEXCOORD1HNVPROC)IntGetProcAddress("glMultiTexCoord1hNV");
	if(!_funcptr_glMultiTexCoord1hNV) ++numFailed;
	_funcptr_glMultiTexCoord1hvNV = (PFNGLMULTITEXCOORD1HVNVPROC)IntGetProcAddress("glMultiTexCoord1hvNV");
	if(!_funcptr_glMultiTexCoord1hvNV) ++numFailed;
	_funcptr_glMultiTexCoord2hNV = (PFNGLMULTITEXCOORD2HNVPROC)IntGetProcAddress("glMultiTexCoord2hNV");
	if(!_funcptr_glMultiTexCoord2hNV) ++numFailed;
	_funcptr_glMultiTexCoord2hvNV = (PFNGLMULTITEXCOORD2HVNVPROC)IntGetProcAddress("glMultiTexCoord2hvNV");
	if(!_funcptr_glMultiTexCoord2hvNV) ++numFailed;
	_funcptr_glMultiTexCoord3hNV = (PFNGLMULTITEXCOORD3HNVPROC)IntGetProcAddress("glMultiTexCoord3hNV");
	if(!_funcptr_glMultiTexCoord3hNV) ++numFailed;
	_funcptr_glMultiTexCoord3hvNV = (PFNGLMULTITEXCOORD3HVNVPROC)IntGetProcAddress("glMultiTexCoord3hvNV");
	if(!_funcptr_glMultiTexCoord3hvNV) ++numFailed;
	_funcptr_glMultiTexCoord4hNV = (PFNGLMULTITEXCOORD4HNVPROC)IntGetProcAddress("glMultiTexCoord4hNV");
	if(!_funcptr_glMultiTexCoord4hNV) ++numFailed;
	_funcptr_glMultiTexCoord4hvNV = (PFNGLMULTITEXCOORD4HVNVPROC)IntGetProcAddress("glMultiTexCoord4hvNV");
	if(!_funcptr_glMultiTexCoord4hvNV) ++numFailed;
	_funcptr_glFogCoordhNV = (PFNGLFOGCOORDHNVPROC)IntGetProcAddress("glFogCoordhNV");
	if(!_funcptr_glFogCoordhNV) ++numFailed;
	_funcptr_glFogCoordhvNV = (PFNGLFOGCOORDHVNVPROC)IntGetProcAddress("glFogCoordhvNV");
	if(!_funcptr_glFogCoordhvNV) ++numFailed;
	_funcptr_glSecondaryColor3hNV = (PFNGLSECONDARYCOLOR3HNVPROC)IntGetProcAddress("glSecondaryColor3hNV");
	if(!_funcptr_glSecondaryColor3hNV) ++numFailed;
	_funcptr_glSecondaryColor3hvNV = (PFNGLSECONDARYCOLOR3HVNVPROC)IntGetProcAddress("glSecondaryColor3hvNV");
	if(!_funcptr_glSecondaryColor3hvNV) ++numFailed;
	_funcptr_glVertexWeighthNV = (PFNGLVERTEXWEIGHTHNVPROC)IntGetProcAddress("glVertexWeighthNV");
	if(!_funcptr_glVertexWeighthNV) ++numFailed;
	_funcptr_glVertexWeighthvNV = (PFNGLVERTEXWEIGHTHVNVPROC)IntGetProcAddress("glVertexWeighthvNV");
	if(!_funcptr_glVertexWeighthvNV) ++numFailed;
	_funcptr_glVertexAttrib1hNV = (PFNGLVERTEXATTRIB1HNVPROC)IntGetProcAddress("glVertexAttrib1hNV");
	if(!_funcptr_glVertexAttrib1hNV) ++numFailed;
	_funcptr_glVertexAttrib1hvNV = (PFNGLVERTEXATTRIB1HVNVPROC)IntGetProcAddress("glVertexAttrib1hvNV");
	if(!_funcptr_glVertexAttrib1hvNV) ++numFailed;
	_funcptr_glVertexAttrib2hNV = (PFNGLVERTEXATTRIB2HNVPROC)IntGetProcAddress("glVertexAttrib2hNV");
	if(!_funcptr_glVertexAttrib2hNV) ++numFailed;
	_funcptr_glVertexAttrib2hvNV = (PFNGLVERTEXATTRIB2HVNVPROC)IntGetProcAddress("glVertexAttrib2hvNV");
	if(!_funcptr_glVertexAttrib2hvNV) ++numFailed;
	_funcptr_glVertexAttrib3hNV = (PFNGLVERTEXATTRIB3HNVPROC)IntGetProcAddress("glVertexAttrib3hNV");
	if(!_funcptr_glVertexAttrib3hNV) ++numFailed;
	_funcptr_glVertexAttrib3hvNV = (PFNGLVERTEXATTRIB3HVNVPROC)IntGetProcAddress("glVertexAttrib3hvNV");
	if(!_funcptr_glVertexAttrib3hvNV) ++numFailed;
	_funcptr_glVertexAttrib4hNV = (PFNGLVERTEXATTRIB4HNVPROC)IntGetProcAddress("glVertexAttrib4hNV");
	if(!_funcptr_glVertexAttrib4hNV) ++numFailed;
	_funcptr_glVertexAttrib4hvNV = (PFNGLVERTEXATTRIB4HVNVPROC)IntGetProcAddress("glVertexAttrib4hvNV");
	if(!_funcptr_glVertexAttrib4hvNV) ++numFailed;
	_funcptr_glVertexAttribs1hvNV = (PFNGLVERTEXATTRIBS1HVNVPROC)IntGetProcAddress("glVertexAttribs1hvNV");
	if(!_funcptr_glVertexAttribs1hvNV) ++numFailed;
	_funcptr_glVertexAttribs2hvNV = (PFNGLVERTEXATTRIBS2HVNVPROC)IntGetProcAddress("glVertexAttribs2hvNV");
	if(!_funcptr_glVertexAttribs2hvNV) ++numFailed;
	_funcptr_glVertexAttribs3hvNV = (PFNGLVERTEXATTRIBS3HVNVPROC)IntGetProcAddress("glVertexAttribs3hvNV");
	if(!_funcptr_glVertexAttribs3hvNV) ++numFailed;
	_funcptr_glVertexAttribs4hvNV = (PFNGLVERTEXATTRIBS4HVNVPROC)IntGetProcAddress("glVertexAttribs4hvNV");
	if(!_funcptr_glVertexAttribs4hvNV) ++numFailed;
	return numFailed;
}

PFNGLGENOCCLUSIONQUERIESNVPROC _funcptr_glGenOcclusionQueriesNV = NULL;
PFNGLDELETEOCCLUSIONQUERIESNVPROC _funcptr_glDeleteOcclusionQueriesNV = NULL;
PFNGLISOCCLUSIONQUERYNVPROC _funcptr_glIsOcclusionQueryNV = NULL;
PFNGLBEGINOCCLUSIONQUERYNVPROC _funcptr_glBeginOcclusionQueryNV = NULL;
PFNGLENDOCCLUSIONQUERYNVPROC _funcptr_glEndOcclusionQueryNV = NULL;
PFNGLGETOCCLUSIONQUERYIVNVPROC _funcptr_glGetOcclusionQueryivNV = NULL;
PFNGLGETOCCLUSIONQUERYUIVNVPROC _funcptr_glGetOcclusionQueryuivNV = NULL;

static int LoadExt_NV_occlusion_query()
{
	int numFailed = 0;
	_funcptr_glGenOcclusionQueriesNV = (PFNGLGENOCCLUSIONQUERIESNVPROC)IntGetProcAddress("glGenOcclusionQueriesNV");
	if(!_funcptr_glGenOcclusionQueriesNV) ++numFailed;
	_funcptr_glDeleteOcclusionQueriesNV = (PFNGLDELETEOCCLUSIONQUERIESNVPROC)IntGetProcAddress("glDeleteOcclusionQueriesNV");
	if(!_funcptr_glDeleteOcclusionQueriesNV) ++numFailed;
	_funcptr_glIsOcclusionQueryNV = (PFNGLISOCCLUSIONQUERYNVPROC)IntGetProcAddress("glIsOcclusionQueryNV");
	if(!_funcptr_glIsOcclusionQueryNV) ++numFailed;
	_funcptr_glBeginOcclusionQueryNV = (PFNGLBEGINOCCLUSIONQUERYNVPROC)IntGetProcAddress("glBeginOcclusionQueryNV");
	if(!_funcptr_glBeginOcclusionQueryNV) ++numFailed;
	_funcptr_glEndOcclusionQueryNV = (PFNGLENDOCCLUSIONQUERYNVPROC)IntGetProcAddress("glEndOcclusionQueryNV");
	if(!_funcptr_glEndOcclusionQueryNV) ++numFailed;
	_funcptr_glGetOcclusionQueryivNV = (PFNGLGETOCCLUSIONQUERYIVNVPROC)IntGetProcAddress("glGetOcclusionQueryivNV");
	if(!_funcptr_glGetOcclusionQueryivNV) ++numFailed;
	_funcptr_glGetOcclusionQueryuivNV = (PFNGLGETOCCLUSIONQUERYUIVNVPROC)IntGetProcAddress("glGetOcclusionQueryuivNV");
	if(!_funcptr_glGetOcclusionQueryuivNV) ++numFailed;
	return numFailed;
}

PFNGLPROGRAMBUFFERPARAMETERSFVNVPROC _funcptr_glProgramBufferParametersfvNV = NULL;
PFNGLPROGRAMBUFFERPARAMETERSIIVNVPROC _funcptr_glProgramBufferParametersIivNV = NULL;
PFNGLPROGRAMBUFFERPARAMETERSIUIVNVPROC _funcptr_glProgramBufferParametersIuivNV = NULL;

static int LoadExt_NV_parameter_buffer_object()
{
	int numFailed = 0;
	_funcptr_glProgramBufferParametersfvNV = (PFNGLPROGRAMBUFFERPARAMETERSFVNVPROC)IntGetProcAddress("glProgramBufferParametersfvNV");
	if(!_funcptr_glProgramBufferParametersfvNV) ++numFailed;
	_funcptr_glProgramBufferParametersIivNV = (PFNGLPROGRAMBUFFERPARAMETERSIIVNVPROC)IntGetProcAddress("glProgramBufferParametersIivNV");
	if(!_funcptr_glProgramBufferParametersIivNV) ++numFailed;
	_funcptr_glProgramBufferParametersIuivNV = (PFNGLPROGRAMBUFFERPARAMETERSIUIVNVPROC)IntGetProcAddress("glProgramBufferParametersIuivNV");
	if(!_funcptr_glProgramBufferParametersIuivNV) ++numFailed;
	return numFailed;
}

PFNGLGENPATHSNVPROC _funcptr_glGenPathsNV = NULL;
PFNGLDELETEPATHSNVPROC _funcptr_glDeletePathsNV = NULL;
PFNGLISPATHNVPROC _funcptr_glIsPathNV = NULL;
PFNGLPATHCOMMANDSNVPROC _funcptr_glPathCommandsNV = NULL;
PFNGLPATHCOORDSNVPROC _funcptr_glPathCoordsNV = NULL;
PFNGLPATHSUBCOMMANDSNVPROC _funcptr_glPathSubCommandsNV = NULL;
PFNGLPATHSUBCOORDSNVPROC _funcptr_glPathSubCoordsNV = NULL;
PFNGLPATHSTRINGNVPROC _funcptr_glPathStringNV = NULL;
PFNGLPATHGLYPHSNVPROC _funcptr_glPathGlyphsNV = NULL;
PFNGLPATHGLYPHRANGENVPROC _funcptr_glPathGlyphRangeNV = NULL;
PFNGLWEIGHTPATHSNVPROC _funcptr_glWeightPathsNV = NULL;
PFNGLCOPYPATHNVPROC _funcptr_glCopyPathNV = NULL;
PFNGLINTERPOLATEPATHSNVPROC _funcptr_glInterpolatePathsNV = NULL;
PFNGLTRANSFORMPATHNVPROC _funcptr_glTransformPathNV = NULL;
PFNGLPATHPARAMETERIVNVPROC _funcptr_glPathParameterivNV = NULL;
PFNGLPATHPARAMETERINVPROC _funcptr_glPathParameteriNV = NULL;
PFNGLPATHPARAMETERFVNVPROC _funcptr_glPathParameterfvNV = NULL;
PFNGLPATHPARAMETERFNVPROC _funcptr_glPathParameterfNV = NULL;
PFNGLPATHDASHARRAYNVPROC _funcptr_glPathDashArrayNV = NULL;
PFNGLPATHSTENCILFUNCNVPROC _funcptr_glPathStencilFuncNV = NULL;
PFNGLPATHSTENCILDEPTHOFFSETNVPROC _funcptr_glPathStencilDepthOffsetNV = NULL;
PFNGLSTENCILFILLPATHNVPROC _funcptr_glStencilFillPathNV = NULL;
PFNGLSTENCILSTROKEPATHNVPROC _funcptr_glStencilStrokePathNV = NULL;
PFNGLSTENCILFILLPATHINSTANCEDNVPROC _funcptr_glStencilFillPathInstancedNV = NULL;
PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC _funcptr_glStencilStrokePathInstancedNV = NULL;
PFNGLPATHCOVERDEPTHFUNCNVPROC _funcptr_glPathCoverDepthFuncNV = NULL;
PFNGLPATHCOLORGENNVPROC _funcptr_glPathColorGenNV = NULL;
PFNGLPATHTEXGENNVPROC _funcptr_glPathTexGenNV = NULL;
PFNGLPATHFOGGENNVPROC _funcptr_glPathFogGenNV = NULL;
PFNGLCOVERFILLPATHNVPROC _funcptr_glCoverFillPathNV = NULL;
PFNGLCOVERSTROKEPATHNVPROC _funcptr_glCoverStrokePathNV = NULL;
PFNGLCOVERFILLPATHINSTANCEDNVPROC _funcptr_glCoverFillPathInstancedNV = NULL;
PFNGLCOVERSTROKEPATHINSTANCEDNVPROC _funcptr_glCoverStrokePathInstancedNV = NULL;
PFNGLGETPATHPARAMETERIVNVPROC _funcptr_glGetPathParameterivNV = NULL;
PFNGLGETPATHPARAMETERFVNVPROC _funcptr_glGetPathParameterfvNV = NULL;
PFNGLGETPATHCOMMANDSNVPROC _funcptr_glGetPathCommandsNV = NULL;
PFNGLGETPATHCOORDSNVPROC _funcptr_glGetPathCoordsNV = NULL;
PFNGLGETPATHDASHARRAYNVPROC _funcptr_glGetPathDashArrayNV = NULL;
PFNGLGETPATHMETRICSNVPROC _funcptr_glGetPathMetricsNV = NULL;
PFNGLGETPATHMETRICRANGENVPROC _funcptr_glGetPathMetricRangeNV = NULL;
PFNGLGETPATHSPACINGNVPROC _funcptr_glGetPathSpacingNV = NULL;
PFNGLGETPATHCOLORGENIVNVPROC _funcptr_glGetPathColorGenivNV = NULL;
PFNGLGETPATHCOLORGENFVNVPROC _funcptr_glGetPathColorGenfvNV = NULL;
PFNGLGETPATHTEXGENIVNVPROC _funcptr_glGetPathTexGenivNV = NULL;
PFNGLGETPATHTEXGENFVNVPROC _funcptr_glGetPathTexGenfvNV = NULL;
PFNGLISPOINTINFILLPATHNVPROC _funcptr_glIsPointInFillPathNV = NULL;
PFNGLISPOINTINSTROKEPATHNVPROC _funcptr_glIsPointInStrokePathNV = NULL;
PFNGLGETPATHLENGTHNVPROC _funcptr_glGetPathLengthNV = NULL;
PFNGLPOINTALONGPATHNVPROC _funcptr_glPointAlongPathNV = NULL;

static int LoadExt_NV_path_rendering()
{
	int numFailed = 0;
	_funcptr_glGenPathsNV = (PFNGLGENPATHSNVPROC)IntGetProcAddress("glGenPathsNV");
	if(!_funcptr_glGenPathsNV) ++numFailed;
	_funcptr_glDeletePathsNV = (PFNGLDELETEPATHSNVPROC)IntGetProcAddress("glDeletePathsNV");
	if(!_funcptr_glDeletePathsNV) ++numFailed;
	_funcptr_glIsPathNV = (PFNGLISPATHNVPROC)IntGetProcAddress("glIsPathNV");
	if(!_funcptr_glIsPathNV) ++numFailed;
	_funcptr_glPathCommandsNV = (PFNGLPATHCOMMANDSNVPROC)IntGetProcAddress("glPathCommandsNV");
	if(!_funcptr_glPathCommandsNV) ++numFailed;
	_funcptr_glPathCoordsNV = (PFNGLPATHCOORDSNVPROC)IntGetProcAddress("glPathCoordsNV");
	if(!_funcptr_glPathCoordsNV) ++numFailed;
	_funcptr_glPathSubCommandsNV = (PFNGLPATHSUBCOMMANDSNVPROC)IntGetProcAddress("glPathSubCommandsNV");
	if(!_funcptr_glPathSubCommandsNV) ++numFailed;
	_funcptr_glPathSubCoordsNV = (PFNGLPATHSUBCOORDSNVPROC)IntGetProcAddress("glPathSubCoordsNV");
	if(!_funcptr_glPathSubCoordsNV) ++numFailed;
	_funcptr_glPathStringNV = (PFNGLPATHSTRINGNVPROC)IntGetProcAddress("glPathStringNV");
	if(!_funcptr_glPathStringNV) ++numFailed;
	_funcptr_glPathGlyphsNV = (PFNGLPATHGLYPHSNVPROC)IntGetProcAddress("glPathGlyphsNV");
	if(!_funcptr_glPathGlyphsNV) ++numFailed;
	_funcptr_glPathGlyphRangeNV = (PFNGLPATHGLYPHRANGENVPROC)IntGetProcAddress("glPathGlyphRangeNV");
	if(!_funcptr_glPathGlyphRangeNV) ++numFailed;
	_funcptr_glWeightPathsNV = (PFNGLWEIGHTPATHSNVPROC)IntGetProcAddress("glWeightPathsNV");
	if(!_funcptr_glWeightPathsNV) ++numFailed;
	_funcptr_glCopyPathNV = (PFNGLCOPYPATHNVPROC)IntGetProcAddress("glCopyPathNV");
	if(!_funcptr_glCopyPathNV) ++numFailed;
	_funcptr_glInterpolatePathsNV = (PFNGLINTERPOLATEPATHSNVPROC)IntGetProcAddress("glInterpolatePathsNV");
	if(!_funcptr_glInterpolatePathsNV) ++numFailed;
	_funcptr_glTransformPathNV = (PFNGLTRANSFORMPATHNVPROC)IntGetProcAddress("glTransformPathNV");
	if(!_funcptr_glTransformPathNV) ++numFailed;
	_funcptr_glPathParameterivNV = (PFNGLPATHPARAMETERIVNVPROC)IntGetProcAddress("glPathParameterivNV");
	if(!_funcptr_glPathParameterivNV) ++numFailed;
	_funcptr_glPathParameteriNV = (PFNGLPATHPARAMETERINVPROC)IntGetProcAddress("glPathParameteriNV");
	if(!_funcptr_glPathParameteriNV) ++numFailed;
	_funcptr_glPathParameterfvNV = (PFNGLPATHPARAMETERFVNVPROC)IntGetProcAddress("glPathParameterfvNV");
	if(!_funcptr_glPathParameterfvNV) ++numFailed;
	_funcptr_glPathParameterfNV = (PFNGLPATHPARAMETERFNVPROC)IntGetProcAddress("glPathParameterfNV");
	if(!_funcptr_glPathParameterfNV) ++numFailed;
	_funcptr_glPathDashArrayNV = (PFNGLPATHDASHARRAYNVPROC)IntGetProcAddress("glPathDashArrayNV");
	if(!_funcptr_glPathDashArrayNV) ++numFailed;
	_funcptr_glPathStencilFuncNV = (PFNGLPATHSTENCILFUNCNVPROC)IntGetProcAddress("glPathStencilFuncNV");
	if(!_funcptr_glPathStencilFuncNV) ++numFailed;
	_funcptr_glPathStencilDepthOffsetNV = (PFNGLPATHSTENCILDEPTHOFFSETNVPROC)IntGetProcAddress("glPathStencilDepthOffsetNV");
	if(!_funcptr_glPathStencilDepthOffsetNV) ++numFailed;
	_funcptr_glStencilFillPathNV = (PFNGLSTENCILFILLPATHNVPROC)IntGetProcAddress("glStencilFillPathNV");
	if(!_funcptr_glStencilFillPathNV) ++numFailed;
	_funcptr_glStencilStrokePathNV = (PFNGLSTENCILSTROKEPATHNVPROC)IntGetProcAddress("glStencilStrokePathNV");
	if(!_funcptr_glStencilStrokePathNV) ++numFailed;
	_funcptr_glStencilFillPathInstancedNV = (PFNGLSTENCILFILLPATHINSTANCEDNVPROC)IntGetProcAddress("glStencilFillPathInstancedNV");
	if(!_funcptr_glStencilFillPathInstancedNV) ++numFailed;
	_funcptr_glStencilStrokePathInstancedNV = (PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC)IntGetProcAddress("glStencilStrokePathInstancedNV");
	if(!_funcptr_glStencilStrokePathInstancedNV) ++numFailed;
	_funcptr_glPathCoverDepthFuncNV = (PFNGLPATHCOVERDEPTHFUNCNVPROC)IntGetProcAddress("glPathCoverDepthFuncNV");
	if(!_funcptr_glPathCoverDepthFuncNV) ++numFailed;
	_funcptr_glPathColorGenNV = (PFNGLPATHCOLORGENNVPROC)IntGetProcAddress("glPathColorGenNV");
	if(!_funcptr_glPathColorGenNV) ++numFailed;
	_funcptr_glPathTexGenNV = (PFNGLPATHTEXGENNVPROC)IntGetProcAddress("glPathTexGenNV");
	if(!_funcptr_glPathTexGenNV) ++numFailed;
	_funcptr_glPathFogGenNV = (PFNGLPATHFOGGENNVPROC)IntGetProcAddress("glPathFogGenNV");
	if(!_funcptr_glPathFogGenNV) ++numFailed;
	_funcptr_glCoverFillPathNV = (PFNGLCOVERFILLPATHNVPROC)IntGetProcAddress("glCoverFillPathNV");
	if(!_funcptr_glCoverFillPathNV) ++numFailed;
	_funcptr_glCoverStrokePathNV = (PFNGLCOVERSTROKEPATHNVPROC)IntGetProcAddress("glCoverStrokePathNV");
	if(!_funcptr_glCoverStrokePathNV) ++numFailed;
	_funcptr_glCoverFillPathInstancedNV = (PFNGLCOVERFILLPATHINSTANCEDNVPROC)IntGetProcAddress("glCoverFillPathInstancedNV");
	if(!_funcptr_glCoverFillPathInstancedNV) ++numFailed;
	_funcptr_glCoverStrokePathInstancedNV = (PFNGLCOVERSTROKEPATHINSTANCEDNVPROC)IntGetProcAddress("glCoverStrokePathInstancedNV");
	if(!_funcptr_glCoverStrokePathInstancedNV) ++numFailed;
	_funcptr_glGetPathParameterivNV = (PFNGLGETPATHPARAMETERIVNVPROC)IntGetProcAddress("glGetPathParameterivNV");
	if(!_funcptr_glGetPathParameterivNV) ++numFailed;
	_funcptr_glGetPathParameterfvNV = (PFNGLGETPATHPARAMETERFVNVPROC)IntGetProcAddress("glGetPathParameterfvNV");
	if(!_funcptr_glGetPathParameterfvNV) ++numFailed;
	_funcptr_glGetPathCommandsNV = (PFNGLGETPATHCOMMANDSNVPROC)IntGetProcAddress("glGetPathCommandsNV");
	if(!_funcptr_glGetPathCommandsNV) ++numFailed;
	_funcptr_glGetPathCoordsNV = (PFNGLGETPATHCOORDSNVPROC)IntGetProcAddress("glGetPathCoordsNV");
	if(!_funcptr_glGetPathCoordsNV) ++numFailed;
	_funcptr_glGetPathDashArrayNV = (PFNGLGETPATHDASHARRAYNVPROC)IntGetProcAddress("glGetPathDashArrayNV");
	if(!_funcptr_glGetPathDashArrayNV) ++numFailed;
	_funcptr_glGetPathMetricsNV = (PFNGLGETPATHMETRICSNVPROC)IntGetProcAddress("glGetPathMetricsNV");
	if(!_funcptr_glGetPathMetricsNV) ++numFailed;
	_funcptr_glGetPathMetricRangeNV = (PFNGLGETPATHMETRICRANGENVPROC)IntGetProcAddress("glGetPathMetricRangeNV");
	if(!_funcptr_glGetPathMetricRangeNV) ++numFailed;
	_funcptr_glGetPathSpacingNV = (PFNGLGETPATHSPACINGNVPROC)IntGetProcAddress("glGetPathSpacingNV");
	if(!_funcptr_glGetPathSpacingNV) ++numFailed;
	_funcptr_glGetPathColorGenivNV = (PFNGLGETPATHCOLORGENIVNVPROC)IntGetProcAddress("glGetPathColorGenivNV");
	if(!_funcptr_glGetPathColorGenivNV) ++numFailed;
	_funcptr_glGetPathColorGenfvNV = (PFNGLGETPATHCOLORGENFVNVPROC)IntGetProcAddress("glGetPathColorGenfvNV");
	if(!_funcptr_glGetPathColorGenfvNV) ++numFailed;
	_funcptr_glGetPathTexGenivNV = (PFNGLGETPATHTEXGENIVNVPROC)IntGetProcAddress("glGetPathTexGenivNV");
	if(!_funcptr_glGetPathTexGenivNV) ++numFailed;
	_funcptr_glGetPathTexGenfvNV = (PFNGLGETPATHTEXGENFVNVPROC)IntGetProcAddress("glGetPathTexGenfvNV");
	if(!_funcptr_glGetPathTexGenfvNV) ++numFailed;
	_funcptr_glIsPointInFillPathNV = (PFNGLISPOINTINFILLPATHNVPROC)IntGetProcAddress("glIsPointInFillPathNV");
	if(!_funcptr_glIsPointInFillPathNV) ++numFailed;
	_funcptr_glIsPointInStrokePathNV = (PFNGLISPOINTINSTROKEPATHNVPROC)IntGetProcAddress("glIsPointInStrokePathNV");
	if(!_funcptr_glIsPointInStrokePathNV) ++numFailed;
	_funcptr_glGetPathLengthNV = (PFNGLGETPATHLENGTHNVPROC)IntGetProcAddress("glGetPathLengthNV");
	if(!_funcptr_glGetPathLengthNV) ++numFailed;
	_funcptr_glPointAlongPathNV = (PFNGLPOINTALONGPATHNVPROC)IntGetProcAddress("glPointAlongPathNV");
	if(!_funcptr_glPointAlongPathNV) ++numFailed;
	return numFailed;
}

PFNGLPIXELDATARANGENVPROC _funcptr_glPixelDataRangeNV = NULL;
PFNGLFLUSHPIXELDATARANGENVPROC _funcptr_glFlushPixelDataRangeNV = NULL;

static int LoadExt_NV_pixel_data_range()
{
	int numFailed = 0;
	_funcptr_glPixelDataRangeNV = (PFNGLPIXELDATARANGENVPROC)IntGetProcAddress("glPixelDataRangeNV");
	if(!_funcptr_glPixelDataRangeNV) ++numFailed;
	_funcptr_glFlushPixelDataRangeNV = (PFNGLFLUSHPIXELDATARANGENVPROC)IntGetProcAddress("glFlushPixelDataRangeNV");
	if(!_funcptr_glFlushPixelDataRangeNV) ++numFailed;
	return numFailed;
}

PFNGLPOINTPARAMETERINVPROC _funcptr_glPointParameteriNV = NULL;
PFNGLPOINTPARAMETERIVNVPROC _funcptr_glPointParameterivNV = NULL;

static int LoadExt_NV_point_sprite()
{
	int numFailed = 0;
	_funcptr_glPointParameteriNV = (PFNGLPOINTPARAMETERINVPROC)IntGetProcAddress("glPointParameteriNV");
	if(!_funcptr_glPointParameteriNV) ++numFailed;
	_funcptr_glPointParameterivNV = (PFNGLPOINTPARAMETERIVNVPROC)IntGetProcAddress("glPointParameterivNV");
	if(!_funcptr_glPointParameterivNV) ++numFailed;
	return numFailed;
}

PFNGLPRESENTFRAMEKEYEDNVPROC _funcptr_glPresentFrameKeyedNV = NULL;
PFNGLPRESENTFRAMEDUALFILLNVPROC _funcptr_glPresentFrameDualFillNV = NULL;
PFNGLGETVIDEOIVNVPROC _funcptr_glGetVideoivNV = NULL;
PFNGLGETVIDEOUIVNVPROC _funcptr_glGetVideouivNV = NULL;
PFNGLGETVIDEOI64VNVPROC _funcptr_glGetVideoi64vNV = NULL;
PFNGLGETVIDEOUI64VNVPROC _funcptr_glGetVideoui64vNV = NULL;

static int LoadExt_NV_present_video()
{
	int numFailed = 0;
	_funcptr_glPresentFrameKeyedNV = (PFNGLPRESENTFRAMEKEYEDNVPROC)IntGetProcAddress("glPresentFrameKeyedNV");
	if(!_funcptr_glPresentFrameKeyedNV) ++numFailed;
	_funcptr_glPresentFrameDualFillNV = (PFNGLPRESENTFRAMEDUALFILLNVPROC)IntGetProcAddress("glPresentFrameDualFillNV");
	if(!_funcptr_glPresentFrameDualFillNV) ++numFailed;
	_funcptr_glGetVideoivNV = (PFNGLGETVIDEOIVNVPROC)IntGetProcAddress("glGetVideoivNV");
	if(!_funcptr_glGetVideoivNV) ++numFailed;
	_funcptr_glGetVideouivNV = (PFNGLGETVIDEOUIVNVPROC)IntGetProcAddress("glGetVideouivNV");
	if(!_funcptr_glGetVideouivNV) ++numFailed;
	_funcptr_glGetVideoi64vNV = (PFNGLGETVIDEOI64VNVPROC)IntGetProcAddress("glGetVideoi64vNV");
	if(!_funcptr_glGetVideoi64vNV) ++numFailed;
	_funcptr_glGetVideoui64vNV = (PFNGLGETVIDEOUI64VNVPROC)IntGetProcAddress("glGetVideoui64vNV");
	if(!_funcptr_glGetVideoui64vNV) ++numFailed;
	return numFailed;
}

PFNGLPRIMITIVERESTARTNVPROC _funcptr_glPrimitiveRestartNV = NULL;
PFNGLPRIMITIVERESTARTINDEXNVPROC _funcptr_glPrimitiveRestartIndexNV = NULL;

static int LoadExt_NV_primitive_restart()
{
	int numFailed = 0;
	_funcptr_glPrimitiveRestartNV = (PFNGLPRIMITIVERESTARTNVPROC)IntGetProcAddress("glPrimitiveRestartNV");
	if(!_funcptr_glPrimitiveRestartNV) ++numFailed;
	_funcptr_glPrimitiveRestartIndexNV = (PFNGLPRIMITIVERESTARTINDEXNVPROC)IntGetProcAddress("glPrimitiveRestartIndexNV");
	if(!_funcptr_glPrimitiveRestartIndexNV) ++numFailed;
	return numFailed;
}

PFNGLCOMBINERPARAMETERFVNVPROC _funcptr_glCombinerParameterfvNV = NULL;
PFNGLCOMBINERPARAMETERFNVPROC _funcptr_glCombinerParameterfNV = NULL;
PFNGLCOMBINERPARAMETERIVNVPROC _funcptr_glCombinerParameterivNV = NULL;
PFNGLCOMBINERPARAMETERINVPROC _funcptr_glCombinerParameteriNV = NULL;
PFNGLCOMBINERINPUTNVPROC _funcptr_glCombinerInputNV = NULL;
PFNGLCOMBINEROUTPUTNVPROC _funcptr_glCombinerOutputNV = NULL;
PFNGLFINALCOMBINERINPUTNVPROC _funcptr_glFinalCombinerInputNV = NULL;
PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC _funcptr_glGetCombinerInputParameterfvNV = NULL;
PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC _funcptr_glGetCombinerInputParameterivNV = NULL;
PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC _funcptr_glGetCombinerOutputParameterfvNV = NULL;
PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC _funcptr_glGetCombinerOutputParameterivNV = NULL;
PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC _funcptr_glGetFinalCombinerInputParameterfvNV = NULL;
PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC _funcptr_glGetFinalCombinerInputParameterivNV = NULL;

static int LoadExt_NV_register_combiners()
{
	int numFailed = 0;
	_funcptr_glCombinerParameterfvNV = (PFNGLCOMBINERPARAMETERFVNVPROC)IntGetProcAddress("glCombinerParameterfvNV");
	if(!_funcptr_glCombinerParameterfvNV) ++numFailed;
	_funcptr_glCombinerParameterfNV = (PFNGLCOMBINERPARAMETERFNVPROC)IntGetProcAddress("glCombinerParameterfNV");
	if(!_funcptr_glCombinerParameterfNV) ++numFailed;
	_funcptr_glCombinerParameterivNV = (PFNGLCOMBINERPARAMETERIVNVPROC)IntGetProcAddress("glCombinerParameterivNV");
	if(!_funcptr_glCombinerParameterivNV) ++numFailed;
	_funcptr_glCombinerParameteriNV = (PFNGLCOMBINERPARAMETERINVPROC)IntGetProcAddress("glCombinerParameteriNV");
	if(!_funcptr_glCombinerParameteriNV) ++numFailed;
	_funcptr_glCombinerInputNV = (PFNGLCOMBINERINPUTNVPROC)IntGetProcAddress("glCombinerInputNV");
	if(!_funcptr_glCombinerInputNV) ++numFailed;
	_funcptr_glCombinerOutputNV = (PFNGLCOMBINEROUTPUTNVPROC)IntGetProcAddress("glCombinerOutputNV");
	if(!_funcptr_glCombinerOutputNV) ++numFailed;
	_funcptr_glFinalCombinerInputNV = (PFNGLFINALCOMBINERINPUTNVPROC)IntGetProcAddress("glFinalCombinerInputNV");
	if(!_funcptr_glFinalCombinerInputNV) ++numFailed;
	_funcptr_glGetCombinerInputParameterfvNV = (PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC)IntGetProcAddress("glGetCombinerInputParameterfvNV");
	if(!_funcptr_glGetCombinerInputParameterfvNV) ++numFailed;
	_funcptr_glGetCombinerInputParameterivNV = (PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC)IntGetProcAddress("glGetCombinerInputParameterivNV");
	if(!_funcptr_glGetCombinerInputParameterivNV) ++numFailed;
	_funcptr_glGetCombinerOutputParameterfvNV = (PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC)IntGetProcAddress("glGetCombinerOutputParameterfvNV");
	if(!_funcptr_glGetCombinerOutputParameterfvNV) ++numFailed;
	_funcptr_glGetCombinerOutputParameterivNV = (PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC)IntGetProcAddress("glGetCombinerOutputParameterivNV");
	if(!_funcptr_glGetCombinerOutputParameterivNV) ++numFailed;
	_funcptr_glGetFinalCombinerInputParameterfvNV = (PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC)IntGetProcAddress("glGetFinalCombinerInputParameterfvNV");
	if(!_funcptr_glGetFinalCombinerInputParameterfvNV) ++numFailed;
	_funcptr_glGetFinalCombinerInputParameterivNV = (PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC)IntGetProcAddress("glGetFinalCombinerInputParameterivNV");
	if(!_funcptr_glGetFinalCombinerInputParameterivNV) ++numFailed;
	return numFailed;
}

PFNGLCOMBINERSTAGEPARAMETERFVNVPROC _funcptr_glCombinerStageParameterfvNV = NULL;
PFNGLGETCOMBINERSTAGEPARAMETERFVNVPROC _funcptr_glGetCombinerStageParameterfvNV = NULL;

static int LoadExt_NV_register_combiners2()
{
	int numFailed = 0;
	_funcptr_glCombinerStageParameterfvNV = (PFNGLCOMBINERSTAGEPARAMETERFVNVPROC)IntGetProcAddress("glCombinerStageParameterfvNV");
	if(!_funcptr_glCombinerStageParameterfvNV) ++numFailed;
	_funcptr_glGetCombinerStageParameterfvNV = (PFNGLGETCOMBINERSTAGEPARAMETERFVNVPROC)IntGetProcAddress("glGetCombinerStageParameterfvNV");
	if(!_funcptr_glGetCombinerStageParameterfvNV) ++numFailed;
	return numFailed;
}

PFNGLMAKEBUFFERRESIDENTNVPROC _funcptr_glMakeBufferResidentNV = NULL;
PFNGLMAKEBUFFERNONRESIDENTNVPROC _funcptr_glMakeBufferNonResidentNV = NULL;
PFNGLISBUFFERRESIDENTNVPROC _funcptr_glIsBufferResidentNV = NULL;
PFNGLMAKENAMEDBUFFERRESIDENTNVPROC _funcptr_glMakeNamedBufferResidentNV = NULL;
PFNGLMAKENAMEDBUFFERNONRESIDENTNVPROC _funcptr_glMakeNamedBufferNonResidentNV = NULL;
PFNGLISNAMEDBUFFERRESIDENTNVPROC _funcptr_glIsNamedBufferResidentNV = NULL;
PFNGLGETBUFFERPARAMETERUI64VNVPROC _funcptr_glGetBufferParameterui64vNV = NULL;
PFNGLGETNAMEDBUFFERPARAMETERUI64VNVPROC _funcptr_glGetNamedBufferParameterui64vNV = NULL;
PFNGLGETINTEGERUI64VNVPROC _funcptr_glGetIntegerui64vNV = NULL;
PFNGLUNIFORMUI64NVPROC _funcptr_glUniformui64NV = NULL;
PFNGLUNIFORMUI64VNVPROC _funcptr_glUniformui64vNV = NULL;
PFNGLGETUNIFORMUI64VNVPROC _funcptr_glGetUniformui64vNV = NULL;
PFNGLPROGRAMUNIFORMUI64NVPROC _funcptr_glProgramUniformui64NV = NULL;
PFNGLPROGRAMUNIFORMUI64VNVPROC _funcptr_glProgramUniformui64vNV = NULL;

static int LoadExt_NV_shader_buffer_load()
{
	int numFailed = 0;
	_funcptr_glMakeBufferResidentNV = (PFNGLMAKEBUFFERRESIDENTNVPROC)IntGetProcAddress("glMakeBufferResidentNV");
	if(!_funcptr_glMakeBufferResidentNV) ++numFailed;
	_funcptr_glMakeBufferNonResidentNV = (PFNGLMAKEBUFFERNONRESIDENTNVPROC)IntGetProcAddress("glMakeBufferNonResidentNV");
	if(!_funcptr_glMakeBufferNonResidentNV) ++numFailed;
	_funcptr_glIsBufferResidentNV = (PFNGLISBUFFERRESIDENTNVPROC)IntGetProcAddress("glIsBufferResidentNV");
	if(!_funcptr_glIsBufferResidentNV) ++numFailed;
	_funcptr_glMakeNamedBufferResidentNV = (PFNGLMAKENAMEDBUFFERRESIDENTNVPROC)IntGetProcAddress("glMakeNamedBufferResidentNV");
	if(!_funcptr_glMakeNamedBufferResidentNV) ++numFailed;
	_funcptr_glMakeNamedBufferNonResidentNV = (PFNGLMAKENAMEDBUFFERNONRESIDENTNVPROC)IntGetProcAddress("glMakeNamedBufferNonResidentNV");
	if(!_funcptr_glMakeNamedBufferNonResidentNV) ++numFailed;
	_funcptr_glIsNamedBufferResidentNV = (PFNGLISNAMEDBUFFERRESIDENTNVPROC)IntGetProcAddress("glIsNamedBufferResidentNV");
	if(!_funcptr_glIsNamedBufferResidentNV) ++numFailed;
	_funcptr_glGetBufferParameterui64vNV = (PFNGLGETBUFFERPARAMETERUI64VNVPROC)IntGetProcAddress("glGetBufferParameterui64vNV");
	if(!_funcptr_glGetBufferParameterui64vNV) ++numFailed;
	_funcptr_glGetNamedBufferParameterui64vNV = (PFNGLGETNAMEDBUFFERPARAMETERUI64VNVPROC)IntGetProcAddress("glGetNamedBufferParameterui64vNV");
	if(!_funcptr_glGetNamedBufferParameterui64vNV) ++numFailed;
	_funcptr_glGetIntegerui64vNV = (PFNGLGETINTEGERUI64VNVPROC)IntGetProcAddress("glGetIntegerui64vNV");
	if(!_funcptr_glGetIntegerui64vNV) ++numFailed;
	_funcptr_glUniformui64NV = (PFNGLUNIFORMUI64NVPROC)IntGetProcAddress("glUniformui64NV");
	if(!_funcptr_glUniformui64NV) ++numFailed;
	_funcptr_glUniformui64vNV = (PFNGLUNIFORMUI64VNVPROC)IntGetProcAddress("glUniformui64vNV");
	if(!_funcptr_glUniformui64vNV) ++numFailed;
	_funcptr_glGetUniformui64vNV = (PFNGLGETUNIFORMUI64VNVPROC)IntGetProcAddress("glGetUniformui64vNV");
	if(!_funcptr_glGetUniformui64vNV) ++numFailed;
	_funcptr_glProgramUniformui64NV = (PFNGLPROGRAMUNIFORMUI64NVPROC)IntGetProcAddress("glProgramUniformui64NV");
	if(!_funcptr_glProgramUniformui64NV) ++numFailed;
	_funcptr_glProgramUniformui64vNV = (PFNGLPROGRAMUNIFORMUI64VNVPROC)IntGetProcAddress("glProgramUniformui64vNV");
	if(!_funcptr_glProgramUniformui64vNV) ++numFailed;
	return numFailed;
}

PFNGLTEXTUREBARRIERNVPROC _funcptr_glTextureBarrierNV = NULL;

static int LoadExt_NV_texture_barrier()
{
	int numFailed = 0;
	_funcptr_glTextureBarrierNV = (PFNGLTEXTUREBARRIERNVPROC)IntGetProcAddress("glTextureBarrierNV");
	if(!_funcptr_glTextureBarrierNV) ++numFailed;
	return numFailed;
}

PFNGLTEXIMAGE2DMULTISAMPLECOVERAGENVPROC _funcptr_glTexImage2DMultisampleCoverageNV = NULL;
PFNGLTEXIMAGE3DMULTISAMPLECOVERAGENVPROC _funcptr_glTexImage3DMultisampleCoverageNV = NULL;
PFNGLTEXTUREIMAGE2DMULTISAMPLENVPROC _funcptr_glTextureImage2DMultisampleNV = NULL;
PFNGLTEXTUREIMAGE3DMULTISAMPLENVPROC _funcptr_glTextureImage3DMultisampleNV = NULL;
PFNGLTEXTUREIMAGE2DMULTISAMPLECOVERAGENVPROC _funcptr_glTextureImage2DMultisampleCoverageNV = NULL;
PFNGLTEXTUREIMAGE3DMULTISAMPLECOVERAGENVPROC _funcptr_glTextureImage3DMultisampleCoverageNV = NULL;

static int LoadExt_NV_texture_multisample()
{
	int numFailed = 0;
	_funcptr_glTexImage2DMultisampleCoverageNV = (PFNGLTEXIMAGE2DMULTISAMPLECOVERAGENVPROC)IntGetProcAddress("glTexImage2DMultisampleCoverageNV");
	if(!_funcptr_glTexImage2DMultisampleCoverageNV) ++numFailed;
	_funcptr_glTexImage3DMultisampleCoverageNV = (PFNGLTEXIMAGE3DMULTISAMPLECOVERAGENVPROC)IntGetProcAddress("glTexImage3DMultisampleCoverageNV");
	if(!_funcptr_glTexImage3DMultisampleCoverageNV) ++numFailed;
	_funcptr_glTextureImage2DMultisampleNV = (PFNGLTEXTUREIMAGE2DMULTISAMPLENVPROC)IntGetProcAddress("glTextureImage2DMultisampleNV");
	if(!_funcptr_glTextureImage2DMultisampleNV) ++numFailed;
	_funcptr_glTextureImage3DMultisampleNV = (PFNGLTEXTUREIMAGE3DMULTISAMPLENVPROC)IntGetProcAddress("glTextureImage3DMultisampleNV");
	if(!_funcptr_glTextureImage3DMultisampleNV) ++numFailed;
	_funcptr_glTextureImage2DMultisampleCoverageNV = (PFNGLTEXTUREIMAGE2DMULTISAMPLECOVERAGENVPROC)IntGetProcAddress("glTextureImage2DMultisampleCoverageNV");
	if(!_funcptr_glTextureImage2DMultisampleCoverageNV) ++numFailed;
	_funcptr_glTextureImage3DMultisampleCoverageNV = (PFNGLTEXTUREIMAGE3DMULTISAMPLECOVERAGENVPROC)IntGetProcAddress("glTextureImage3DMultisampleCoverageNV");
	if(!_funcptr_glTextureImage3DMultisampleCoverageNV) ++numFailed;
	return numFailed;
}

PFNGLBEGINTRANSFORMFEEDBACKNVPROC _funcptr_glBeginTransformFeedbackNV = NULL;
PFNGLENDTRANSFORMFEEDBACKNVPROC _funcptr_glEndTransformFeedbackNV = NULL;
PFNGLTRANSFORMFEEDBACKATTRIBSNVPROC _funcptr_glTransformFeedbackAttribsNV = NULL;
PFNGLBINDBUFFERRANGENVPROC _funcptr_glBindBufferRangeNV = NULL;
PFNGLBINDBUFFEROFFSETNVPROC _funcptr_glBindBufferOffsetNV = NULL;
PFNGLBINDBUFFERBASENVPROC _funcptr_glBindBufferBaseNV = NULL;
PFNGLTRANSFORMFEEDBACKVARYINGSNVPROC _funcptr_glTransformFeedbackVaryingsNV = NULL;
PFNGLACTIVEVARYINGNVPROC _funcptr_glActiveVaryingNV = NULL;
PFNGLGETVARYINGLOCATIONNVPROC _funcptr_glGetVaryingLocationNV = NULL;
PFNGLGETACTIVEVARYINGNVPROC _funcptr_glGetActiveVaryingNV = NULL;
PFNGLGETTRANSFORMFEEDBACKVARYINGNVPROC _funcptr_glGetTransformFeedbackVaryingNV = NULL;
PFNGLTRANSFORMFEEDBACKSTREAMATTRIBSNVPROC _funcptr_glTransformFeedbackStreamAttribsNV = NULL;

static int LoadExt_NV_transform_feedback()
{
	int numFailed = 0;
	_funcptr_glBeginTransformFeedbackNV = (PFNGLBEGINTRANSFORMFEEDBACKNVPROC)IntGetProcAddress("glBeginTransformFeedbackNV");
	if(!_funcptr_glBeginTransformFeedbackNV) ++numFailed;
	_funcptr_glEndTransformFeedbackNV = (PFNGLENDTRANSFORMFEEDBACKNVPROC)IntGetProcAddress("glEndTransformFeedbackNV");
	if(!_funcptr_glEndTransformFeedbackNV) ++numFailed;
	_funcptr_glTransformFeedbackAttribsNV = (PFNGLTRANSFORMFEEDBACKATTRIBSNVPROC)IntGetProcAddress("glTransformFeedbackAttribsNV");
	if(!_funcptr_glTransformFeedbackAttribsNV) ++numFailed;
	_funcptr_glBindBufferRangeNV = (PFNGLBINDBUFFERRANGENVPROC)IntGetProcAddress("glBindBufferRangeNV");
	if(!_funcptr_glBindBufferRangeNV) ++numFailed;
	_funcptr_glBindBufferOffsetNV = (PFNGLBINDBUFFEROFFSETNVPROC)IntGetProcAddress("glBindBufferOffsetNV");
	if(!_funcptr_glBindBufferOffsetNV) ++numFailed;
	_funcptr_glBindBufferBaseNV = (PFNGLBINDBUFFERBASENVPROC)IntGetProcAddress("glBindBufferBaseNV");
	if(!_funcptr_glBindBufferBaseNV) ++numFailed;
	_funcptr_glTransformFeedbackVaryingsNV = (PFNGLTRANSFORMFEEDBACKVARYINGSNVPROC)IntGetProcAddress("glTransformFeedbackVaryingsNV");
	if(!_funcptr_glTransformFeedbackVaryingsNV) ++numFailed;
	_funcptr_glActiveVaryingNV = (PFNGLACTIVEVARYINGNVPROC)IntGetProcAddress("glActiveVaryingNV");
	if(!_funcptr_glActiveVaryingNV) ++numFailed;
	_funcptr_glGetVaryingLocationNV = (PFNGLGETVARYINGLOCATIONNVPROC)IntGetProcAddress("glGetVaryingLocationNV");
	if(!_funcptr_glGetVaryingLocationNV) ++numFailed;
	_funcptr_glGetActiveVaryingNV = (PFNGLGETACTIVEVARYINGNVPROC)IntGetProcAddress("glGetActiveVaryingNV");
	if(!_funcptr_glGetActiveVaryingNV) ++numFailed;
	_funcptr_glGetTransformFeedbackVaryingNV = (PFNGLGETTRANSFORMFEEDBACKVARYINGNVPROC)IntGetProcAddress("glGetTransformFeedbackVaryingNV");
	if(!_funcptr_glGetTransformFeedbackVaryingNV) ++numFailed;
	_funcptr_glTransformFeedbackStreamAttribsNV = (PFNGLTRANSFORMFEEDBACKSTREAMATTRIBSNVPROC)IntGetProcAddress("glTransformFeedbackStreamAttribsNV");
	if(!_funcptr_glTransformFeedbackStreamAttribsNV) ++numFailed;
	return numFailed;
}

PFNGLBINDTRANSFORMFEEDBACKNVPROC _funcptr_glBindTransformFeedbackNV = NULL;
PFNGLDELETETRANSFORMFEEDBACKSNVPROC _funcptr_glDeleteTransformFeedbacksNV = NULL;
PFNGLGENTRANSFORMFEEDBACKSNVPROC _funcptr_glGenTransformFeedbacksNV = NULL;
PFNGLISTRANSFORMFEEDBACKNVPROC _funcptr_glIsTransformFeedbackNV = NULL;
PFNGLPAUSETRANSFORMFEEDBACKNVPROC _funcptr_glPauseTransformFeedbackNV = NULL;
PFNGLRESUMETRANSFORMFEEDBACKNVPROC _funcptr_glResumeTransformFeedbackNV = NULL;
PFNGLDRAWTRANSFORMFEEDBACKNVPROC _funcptr_glDrawTransformFeedbackNV = NULL;

static int LoadExt_NV_transform_feedback2()
{
	int numFailed = 0;
	_funcptr_glBindTransformFeedbackNV = (PFNGLBINDTRANSFORMFEEDBACKNVPROC)IntGetProcAddress("glBindTransformFeedbackNV");
	if(!_funcptr_glBindTransformFeedbackNV) ++numFailed;
	_funcptr_glDeleteTransformFeedbacksNV = (PFNGLDELETETRANSFORMFEEDBACKSNVPROC)IntGetProcAddress("glDeleteTransformFeedbacksNV");
	if(!_funcptr_glDeleteTransformFeedbacksNV) ++numFailed;
	_funcptr_glGenTransformFeedbacksNV = (PFNGLGENTRANSFORMFEEDBACKSNVPROC)IntGetProcAddress("glGenTransformFeedbacksNV");
	if(!_funcptr_glGenTransformFeedbacksNV) ++numFailed;
	_funcptr_glIsTransformFeedbackNV = (PFNGLISTRANSFORMFEEDBACKNVPROC)IntGetProcAddress("glIsTransformFeedbackNV");
	if(!_funcptr_glIsTransformFeedbackNV) ++numFailed;
	_funcptr_glPauseTransformFeedbackNV = (PFNGLPAUSETRANSFORMFEEDBACKNVPROC)IntGetProcAddress("glPauseTransformFeedbackNV");
	if(!_funcptr_glPauseTransformFeedbackNV) ++numFailed;
	_funcptr_glResumeTransformFeedbackNV = (PFNGLRESUMETRANSFORMFEEDBACKNVPROC)IntGetProcAddress("glResumeTransformFeedbackNV");
	if(!_funcptr_glResumeTransformFeedbackNV) ++numFailed;
	_funcptr_glDrawTransformFeedbackNV = (PFNGLDRAWTRANSFORMFEEDBACKNVPROC)IntGetProcAddress("glDrawTransformFeedbackNV");
	if(!_funcptr_glDrawTransformFeedbackNV) ++numFailed;
	return numFailed;
}

PFNGLVDPAUINITNVPROC _funcptr_glVDPAUInitNV = NULL;
PFNGLVDPAUFININVPROC _funcptr_glVDPAUFiniNV = NULL;
PFNGLVDPAUREGISTERVIDEOSURFACENVPROC _funcptr_glVDPAURegisterVideoSurfaceNV = NULL;
PFNGLVDPAUREGISTEROUTPUTSURFACENVPROC _funcptr_glVDPAURegisterOutputSurfaceNV = NULL;
PFNGLVDPAUISSURFACENVPROC _funcptr_glVDPAUIsSurfaceNV = NULL;
PFNGLVDPAUUNREGISTERSURFACENVPROC _funcptr_glVDPAUUnregisterSurfaceNV = NULL;
PFNGLVDPAUGETSURFACEIVNVPROC _funcptr_glVDPAUGetSurfaceivNV = NULL;
PFNGLVDPAUSURFACEACCESSNVPROC _funcptr_glVDPAUSurfaceAccessNV = NULL;
PFNGLVDPAUMAPSURFACESNVPROC _funcptr_glVDPAUMapSurfacesNV = NULL;
PFNGLVDPAUUNMAPSURFACESNVPROC _funcptr_glVDPAUUnmapSurfacesNV = NULL;

static int LoadExt_NV_vdpau_interop()
{
	int numFailed = 0;
	_funcptr_glVDPAUInitNV = (PFNGLVDPAUINITNVPROC)IntGetProcAddress("glVDPAUInitNV");
	if(!_funcptr_glVDPAUInitNV) ++numFailed;
	_funcptr_glVDPAUFiniNV = (PFNGLVDPAUFININVPROC)IntGetProcAddress("glVDPAUFiniNV");
	if(!_funcptr_glVDPAUFiniNV) ++numFailed;
	_funcptr_glVDPAURegisterVideoSurfaceNV = (PFNGLVDPAUREGISTERVIDEOSURFACENVPROC)IntGetProcAddress("glVDPAURegisterVideoSurfaceNV");
	if(!_funcptr_glVDPAURegisterVideoSurfaceNV) ++numFailed;
	_funcptr_glVDPAURegisterOutputSurfaceNV = (PFNGLVDPAUREGISTEROUTPUTSURFACENVPROC)IntGetProcAddress("glVDPAURegisterOutputSurfaceNV");
	if(!_funcptr_glVDPAURegisterOutputSurfaceNV) ++numFailed;
	_funcptr_glVDPAUIsSurfaceNV = (PFNGLVDPAUISSURFACENVPROC)IntGetProcAddress("glVDPAUIsSurfaceNV");
	if(!_funcptr_glVDPAUIsSurfaceNV) ++numFailed;
	_funcptr_glVDPAUUnregisterSurfaceNV = (PFNGLVDPAUUNREGISTERSURFACENVPROC)IntGetProcAddress("glVDPAUUnregisterSurfaceNV");
	if(!_funcptr_glVDPAUUnregisterSurfaceNV) ++numFailed;
	_funcptr_glVDPAUGetSurfaceivNV = (PFNGLVDPAUGETSURFACEIVNVPROC)IntGetProcAddress("glVDPAUGetSurfaceivNV");
	if(!_funcptr_glVDPAUGetSurfaceivNV) ++numFailed;
	_funcptr_glVDPAUSurfaceAccessNV = (PFNGLVDPAUSURFACEACCESSNVPROC)IntGetProcAddress("glVDPAUSurfaceAccessNV");
	if(!_funcptr_glVDPAUSurfaceAccessNV) ++numFailed;
	_funcptr_glVDPAUMapSurfacesNV = (PFNGLVDPAUMAPSURFACESNVPROC)IntGetProcAddress("glVDPAUMapSurfacesNV");
	if(!_funcptr_glVDPAUMapSurfacesNV) ++numFailed;
	_funcptr_glVDPAUUnmapSurfacesNV = (PFNGLVDPAUUNMAPSURFACESNVPROC)IntGetProcAddress("glVDPAUUnmapSurfacesNV");
	if(!_funcptr_glVDPAUUnmapSurfacesNV) ++numFailed;
	return numFailed;
}

PFNGLFLUSHVERTEXARRAYRANGENVPROC _funcptr_glFlushVertexArrayRangeNV = NULL;
PFNGLVERTEXARRAYRANGENVPROC _funcptr_glVertexArrayRangeNV = NULL;

static int LoadExt_NV_vertex_array_range()
{
	int numFailed = 0;
	_funcptr_glFlushVertexArrayRangeNV = (PFNGLFLUSHVERTEXARRAYRANGENVPROC)IntGetProcAddress("glFlushVertexArrayRangeNV");
	if(!_funcptr_glFlushVertexArrayRangeNV) ++numFailed;
	_funcptr_glVertexArrayRangeNV = (PFNGLVERTEXARRAYRANGENVPROC)IntGetProcAddress("glVertexArrayRangeNV");
	if(!_funcptr_glVertexArrayRangeNV) ++numFailed;
	return numFailed;
}

PFNGLVERTEXATTRIBL1I64NVPROC _funcptr_glVertexAttribL1i64NV = NULL;
PFNGLVERTEXATTRIBL2I64NVPROC _funcptr_glVertexAttribL2i64NV = NULL;
PFNGLVERTEXATTRIBL3I64NVPROC _funcptr_glVertexAttribL3i64NV = NULL;
PFNGLVERTEXATTRIBL4I64NVPROC _funcptr_glVertexAttribL4i64NV = NULL;
PFNGLVERTEXATTRIBL1I64VNVPROC _funcptr_glVertexAttribL1i64vNV = NULL;
PFNGLVERTEXATTRIBL2I64VNVPROC _funcptr_glVertexAttribL2i64vNV = NULL;
PFNGLVERTEXATTRIBL3I64VNVPROC _funcptr_glVertexAttribL3i64vNV = NULL;
PFNGLVERTEXATTRIBL4I64VNVPROC _funcptr_glVertexAttribL4i64vNV = NULL;
PFNGLVERTEXATTRIBL1UI64NVPROC _funcptr_glVertexAttribL1ui64NV = NULL;
PFNGLVERTEXATTRIBL2UI64NVPROC _funcptr_glVertexAttribL2ui64NV = NULL;
PFNGLVERTEXATTRIBL3UI64NVPROC _funcptr_glVertexAttribL3ui64NV = NULL;
PFNGLVERTEXATTRIBL4UI64NVPROC _funcptr_glVertexAttribL4ui64NV = NULL;
PFNGLVERTEXATTRIBL1UI64VNVPROC _funcptr_glVertexAttribL1ui64vNV = NULL;
PFNGLVERTEXATTRIBL2UI64VNVPROC _funcptr_glVertexAttribL2ui64vNV = NULL;
PFNGLVERTEXATTRIBL3UI64VNVPROC _funcptr_glVertexAttribL3ui64vNV = NULL;
PFNGLVERTEXATTRIBL4UI64VNVPROC _funcptr_glVertexAttribL4ui64vNV = NULL;
PFNGLGETVERTEXATTRIBLI64VNVPROC _funcptr_glGetVertexAttribLi64vNV = NULL;
PFNGLGETVERTEXATTRIBLUI64VNVPROC _funcptr_glGetVertexAttribLui64vNV = NULL;
PFNGLVERTEXATTRIBLFORMATNVPROC _funcptr_glVertexAttribLFormatNV = NULL;

static int LoadExt_NV_vertex_attrib_integer_64bit()
{
	int numFailed = 0;
	_funcptr_glVertexAttribL1i64NV = (PFNGLVERTEXATTRIBL1I64NVPROC)IntGetProcAddress("glVertexAttribL1i64NV");
	if(!_funcptr_glVertexAttribL1i64NV) ++numFailed;
	_funcptr_glVertexAttribL2i64NV = (PFNGLVERTEXATTRIBL2I64NVPROC)IntGetProcAddress("glVertexAttribL2i64NV");
	if(!_funcptr_glVertexAttribL2i64NV) ++numFailed;
	_funcptr_glVertexAttribL3i64NV = (PFNGLVERTEXATTRIBL3I64NVPROC)IntGetProcAddress("glVertexAttribL3i64NV");
	if(!_funcptr_glVertexAttribL3i64NV) ++numFailed;
	_funcptr_glVertexAttribL4i64NV = (PFNGLVERTEXATTRIBL4I64NVPROC)IntGetProcAddress("glVertexAttribL4i64NV");
	if(!_funcptr_glVertexAttribL4i64NV) ++numFailed;
	_funcptr_glVertexAttribL1i64vNV = (PFNGLVERTEXATTRIBL1I64VNVPROC)IntGetProcAddress("glVertexAttribL1i64vNV");
	if(!_funcptr_glVertexAttribL1i64vNV) ++numFailed;
	_funcptr_glVertexAttribL2i64vNV = (PFNGLVERTEXATTRIBL2I64VNVPROC)IntGetProcAddress("glVertexAttribL2i64vNV");
	if(!_funcptr_glVertexAttribL2i64vNV) ++numFailed;
	_funcptr_glVertexAttribL3i64vNV = (PFNGLVERTEXATTRIBL3I64VNVPROC)IntGetProcAddress("glVertexAttribL3i64vNV");
	if(!_funcptr_glVertexAttribL3i64vNV) ++numFailed;
	_funcptr_glVertexAttribL4i64vNV = (PFNGLVERTEXATTRIBL4I64VNVPROC)IntGetProcAddress("glVertexAttribL4i64vNV");
	if(!_funcptr_glVertexAttribL4i64vNV) ++numFailed;
	_funcptr_glVertexAttribL1ui64NV = (PFNGLVERTEXATTRIBL1UI64NVPROC)IntGetProcAddress("glVertexAttribL1ui64NV");
	if(!_funcptr_glVertexAttribL1ui64NV) ++numFailed;
	_funcptr_glVertexAttribL2ui64NV = (PFNGLVERTEXATTRIBL2UI64NVPROC)IntGetProcAddress("glVertexAttribL2ui64NV");
	if(!_funcptr_glVertexAttribL2ui64NV) ++numFailed;
	_funcptr_glVertexAttribL3ui64NV = (PFNGLVERTEXATTRIBL3UI64NVPROC)IntGetProcAddress("glVertexAttribL3ui64NV");
	if(!_funcptr_glVertexAttribL3ui64NV) ++numFailed;
	_funcptr_glVertexAttribL4ui64NV = (PFNGLVERTEXATTRIBL4UI64NVPROC)IntGetProcAddress("glVertexAttribL4ui64NV");
	if(!_funcptr_glVertexAttribL4ui64NV) ++numFailed;
	_funcptr_glVertexAttribL1ui64vNV = (PFNGLVERTEXATTRIBL1UI64VNVPROC)IntGetProcAddress("glVertexAttribL1ui64vNV");
	if(!_funcptr_glVertexAttribL1ui64vNV) ++numFailed;
	_funcptr_glVertexAttribL2ui64vNV = (PFNGLVERTEXATTRIBL2UI64VNVPROC)IntGetProcAddress("glVertexAttribL2ui64vNV");
	if(!_funcptr_glVertexAttribL2ui64vNV) ++numFailed;
	_funcptr_glVertexAttribL3ui64vNV = (PFNGLVERTEXATTRIBL3UI64VNVPROC)IntGetProcAddress("glVertexAttribL3ui64vNV");
	if(!_funcptr_glVertexAttribL3ui64vNV) ++numFailed;
	_funcptr_glVertexAttribL4ui64vNV = (PFNGLVERTEXATTRIBL4UI64VNVPROC)IntGetProcAddress("glVertexAttribL4ui64vNV");
	if(!_funcptr_glVertexAttribL4ui64vNV) ++numFailed;
	_funcptr_glGetVertexAttribLi64vNV = (PFNGLGETVERTEXATTRIBLI64VNVPROC)IntGetProcAddress("glGetVertexAttribLi64vNV");
	if(!_funcptr_glGetVertexAttribLi64vNV) ++numFailed;
	_funcptr_glGetVertexAttribLui64vNV = (PFNGLGETVERTEXATTRIBLUI64VNVPROC)IntGetProcAddress("glGetVertexAttribLui64vNV");
	if(!_funcptr_glGetVertexAttribLui64vNV) ++numFailed;
	_funcptr_glVertexAttribLFormatNV = (PFNGLVERTEXATTRIBLFORMATNVPROC)IntGetProcAddress("glVertexAttribLFormatNV");
	if(!_funcptr_glVertexAttribLFormatNV) ++numFailed;
	return numFailed;
}

PFNGLBUFFERADDRESSRANGENVPROC _funcptr_glBufferAddressRangeNV = NULL;
PFNGLVERTEXFORMATNVPROC _funcptr_glVertexFormatNV = NULL;
PFNGLNORMALFORMATNVPROC _funcptr_glNormalFormatNV = NULL;
PFNGLCOLORFORMATNVPROC _funcptr_glColorFormatNV = NULL;
PFNGLINDEXFORMATNVPROC _funcptr_glIndexFormatNV = NULL;
PFNGLTEXCOORDFORMATNVPROC _funcptr_glTexCoordFormatNV = NULL;
PFNGLEDGEFLAGFORMATNVPROC _funcptr_glEdgeFlagFormatNV = NULL;
PFNGLSECONDARYCOLORFORMATNVPROC _funcptr_glSecondaryColorFormatNV = NULL;
PFNGLFOGCOORDFORMATNVPROC _funcptr_glFogCoordFormatNV = NULL;
PFNGLVERTEXATTRIBFORMATNVPROC _funcptr_glVertexAttribFormatNV = NULL;
PFNGLVERTEXATTRIBIFORMATNVPROC _funcptr_glVertexAttribIFormatNV = NULL;
PFNGLGETINTEGERUI64I_VNVPROC _funcptr_glGetIntegerui64i_vNV = NULL;

static int LoadExt_NV_vertex_buffer_unified_memory()
{
	int numFailed = 0;
	_funcptr_glBufferAddressRangeNV = (PFNGLBUFFERADDRESSRANGENVPROC)IntGetProcAddress("glBufferAddressRangeNV");
	if(!_funcptr_glBufferAddressRangeNV) ++numFailed;
	_funcptr_glVertexFormatNV = (PFNGLVERTEXFORMATNVPROC)IntGetProcAddress("glVertexFormatNV");
	if(!_funcptr_glVertexFormatNV) ++numFailed;
	_funcptr_glNormalFormatNV = (PFNGLNORMALFORMATNVPROC)IntGetProcAddress("glNormalFormatNV");
	if(!_funcptr_glNormalFormatNV) ++numFailed;
	_funcptr_glColorFormatNV = (PFNGLCOLORFORMATNVPROC)IntGetProcAddress("glColorFormatNV");
	if(!_funcptr_glColorFormatNV) ++numFailed;
	_funcptr_glIndexFormatNV = (PFNGLINDEXFORMATNVPROC)IntGetProcAddress("glIndexFormatNV");
	if(!_funcptr_glIndexFormatNV) ++numFailed;
	_funcptr_glTexCoordFormatNV = (PFNGLTEXCOORDFORMATNVPROC)IntGetProcAddress("glTexCoordFormatNV");
	if(!_funcptr_glTexCoordFormatNV) ++numFailed;
	_funcptr_glEdgeFlagFormatNV = (PFNGLEDGEFLAGFORMATNVPROC)IntGetProcAddress("glEdgeFlagFormatNV");
	if(!_funcptr_glEdgeFlagFormatNV) ++numFailed;
	_funcptr_glSecondaryColorFormatNV = (PFNGLSECONDARYCOLORFORMATNVPROC)IntGetProcAddress("glSecondaryColorFormatNV");
	if(!_funcptr_glSecondaryColorFormatNV) ++numFailed;
	_funcptr_glFogCoordFormatNV = (PFNGLFOGCOORDFORMATNVPROC)IntGetProcAddress("glFogCoordFormatNV");
	if(!_funcptr_glFogCoordFormatNV) ++numFailed;
	_funcptr_glVertexAttribFormatNV = (PFNGLVERTEXATTRIBFORMATNVPROC)IntGetProcAddress("glVertexAttribFormatNV");
	if(!_funcptr_glVertexAttribFormatNV) ++numFailed;
	_funcptr_glVertexAttribIFormatNV = (PFNGLVERTEXATTRIBIFORMATNVPROC)IntGetProcAddress("glVertexAttribIFormatNV");
	if(!_funcptr_glVertexAttribIFormatNV) ++numFailed;
	_funcptr_glGetIntegerui64i_vNV = (PFNGLGETINTEGERUI64I_VNVPROC)IntGetProcAddress("glGetIntegerui64i_vNV");
	if(!_funcptr_glGetIntegerui64i_vNV) ++numFailed;
	return numFailed;
}

PFNGLAREPROGRAMSRESIDENTNVPROC _funcptr_glAreProgramsResidentNV = NULL;
PFNGLBINDPROGRAMNVPROC _funcptr_glBindProgramNV = NULL;
PFNGLDELETEPROGRAMSNVPROC _funcptr_glDeleteProgramsNV = NULL;
PFNGLEXECUTEPROGRAMNVPROC _funcptr_glExecuteProgramNV = NULL;
PFNGLGENPROGRAMSNVPROC _funcptr_glGenProgramsNV = NULL;
PFNGLGETPROGRAMPARAMETERDVNVPROC _funcptr_glGetProgramParameterdvNV = NULL;
PFNGLGETPROGRAMPARAMETERFVNVPROC _funcptr_glGetProgramParameterfvNV = NULL;
PFNGLGETPROGRAMIVNVPROC _funcptr_glGetProgramivNV = NULL;
PFNGLGETPROGRAMSTRINGNVPROC _funcptr_glGetProgramStringNV = NULL;
PFNGLGETTRACKMATRIXIVNVPROC _funcptr_glGetTrackMatrixivNV = NULL;
PFNGLGETVERTEXATTRIBDVNVPROC _funcptr_glGetVertexAttribdvNV = NULL;
PFNGLGETVERTEXATTRIBFVNVPROC _funcptr_glGetVertexAttribfvNV = NULL;
PFNGLGETVERTEXATTRIBIVNVPROC _funcptr_glGetVertexAttribivNV = NULL;
PFNGLGETVERTEXATTRIBPOINTERVNVPROC _funcptr_glGetVertexAttribPointervNV = NULL;
PFNGLISPROGRAMNVPROC _funcptr_glIsProgramNV = NULL;
PFNGLLOADPROGRAMNVPROC _funcptr_glLoadProgramNV = NULL;
PFNGLPROGRAMPARAMETER4DNVPROC _funcptr_glProgramParameter4dNV = NULL;
PFNGLPROGRAMPARAMETER4DVNVPROC _funcptr_glProgramParameter4dvNV = NULL;
PFNGLPROGRAMPARAMETER4FNVPROC _funcptr_glProgramParameter4fNV = NULL;
PFNGLPROGRAMPARAMETER4FVNVPROC _funcptr_glProgramParameter4fvNV = NULL;
PFNGLPROGRAMPARAMETERS4DVNVPROC _funcptr_glProgramParameters4dvNV = NULL;
PFNGLPROGRAMPARAMETERS4FVNVPROC _funcptr_glProgramParameters4fvNV = NULL;
PFNGLREQUESTRESIDENTPROGRAMSNVPROC _funcptr_glRequestResidentProgramsNV = NULL;
PFNGLTRACKMATRIXNVPROC _funcptr_glTrackMatrixNV = NULL;
PFNGLVERTEXATTRIBPOINTERNVPROC _funcptr_glVertexAttribPointerNV = NULL;
PFNGLVERTEXATTRIB1DNVPROC _funcptr_glVertexAttrib1dNV = NULL;
PFNGLVERTEXATTRIB1DVNVPROC _funcptr_glVertexAttrib1dvNV = NULL;
PFNGLVERTEXATTRIB1FNVPROC _funcptr_glVertexAttrib1fNV = NULL;
PFNGLVERTEXATTRIB1FVNVPROC _funcptr_glVertexAttrib1fvNV = NULL;
PFNGLVERTEXATTRIB1SNVPROC _funcptr_glVertexAttrib1sNV = NULL;
PFNGLVERTEXATTRIB1SVNVPROC _funcptr_glVertexAttrib1svNV = NULL;
PFNGLVERTEXATTRIB2DNVPROC _funcptr_glVertexAttrib2dNV = NULL;
PFNGLVERTEXATTRIB2DVNVPROC _funcptr_glVertexAttrib2dvNV = NULL;
PFNGLVERTEXATTRIB2FNVPROC _funcptr_glVertexAttrib2fNV = NULL;
PFNGLVERTEXATTRIB2FVNVPROC _funcptr_glVertexAttrib2fvNV = NULL;
PFNGLVERTEXATTRIB2SNVPROC _funcptr_glVertexAttrib2sNV = NULL;
PFNGLVERTEXATTRIB2SVNVPROC _funcptr_glVertexAttrib2svNV = NULL;
PFNGLVERTEXATTRIB3DNVPROC _funcptr_glVertexAttrib3dNV = NULL;
PFNGLVERTEXATTRIB3DVNVPROC _funcptr_glVertexAttrib3dvNV = NULL;
PFNGLVERTEXATTRIB3FNVPROC _funcptr_glVertexAttrib3fNV = NULL;
PFNGLVERTEXATTRIB3FVNVPROC _funcptr_glVertexAttrib3fvNV = NULL;
PFNGLVERTEXATTRIB3SNVPROC _funcptr_glVertexAttrib3sNV = NULL;
PFNGLVERTEXATTRIB3SVNVPROC _funcptr_glVertexAttrib3svNV = NULL;
PFNGLVERTEXATTRIB4DNVPROC _funcptr_glVertexAttrib4dNV = NULL;
PFNGLVERTEXATTRIB4DVNVPROC _funcptr_glVertexAttrib4dvNV = NULL;
PFNGLVERTEXATTRIB4FNVPROC _funcptr_glVertexAttrib4fNV = NULL;
PFNGLVERTEXATTRIB4FVNVPROC _funcptr_glVertexAttrib4fvNV = NULL;
PFNGLVERTEXATTRIB4SNVPROC _funcptr_glVertexAttrib4sNV = NULL;
PFNGLVERTEXATTRIB4SVNVPROC _funcptr_glVertexAttrib4svNV = NULL;
PFNGLVERTEXATTRIB4UBNVPROC _funcptr_glVertexAttrib4ubNV = NULL;
PFNGLVERTEXATTRIB4UBVNVPROC _funcptr_glVertexAttrib4ubvNV = NULL;
PFNGLVERTEXATTRIBS1DVNVPROC _funcptr_glVertexAttribs1dvNV = NULL;
PFNGLVERTEXATTRIBS1FVNVPROC _funcptr_glVertexAttribs1fvNV = NULL;
PFNGLVERTEXATTRIBS1SVNVPROC _funcptr_glVertexAttribs1svNV = NULL;
PFNGLVERTEXATTRIBS2DVNVPROC _funcptr_glVertexAttribs2dvNV = NULL;
PFNGLVERTEXATTRIBS2FVNVPROC _funcptr_glVertexAttribs2fvNV = NULL;
PFNGLVERTEXATTRIBS2SVNVPROC _funcptr_glVertexAttribs2svNV = NULL;
PFNGLVERTEXATTRIBS3DVNVPROC _funcptr_glVertexAttribs3dvNV = NULL;
PFNGLVERTEXATTRIBS3FVNVPROC _funcptr_glVertexAttribs3fvNV = NULL;
PFNGLVERTEXATTRIBS3SVNVPROC _funcptr_glVertexAttribs3svNV = NULL;
PFNGLVERTEXATTRIBS4DVNVPROC _funcptr_glVertexAttribs4dvNV = NULL;
PFNGLVERTEXATTRIBS4FVNVPROC _funcptr_glVertexAttribs4fvNV = NULL;
PFNGLVERTEXATTRIBS4SVNVPROC _funcptr_glVertexAttribs4svNV = NULL;
PFNGLVERTEXATTRIBS4UBVNVPROC _funcptr_glVertexAttribs4ubvNV = NULL;

static int LoadExt_NV_vertex_program()
{
	int numFailed = 0;
	_funcptr_glAreProgramsResidentNV = (PFNGLAREPROGRAMSRESIDENTNVPROC)IntGetProcAddress("glAreProgramsResidentNV");
	if(!_funcptr_glAreProgramsResidentNV) ++numFailed;
	_funcptr_glBindProgramNV = (PFNGLBINDPROGRAMNVPROC)IntGetProcAddress("glBindProgramNV");
	if(!_funcptr_glBindProgramNV) ++numFailed;
	_funcptr_glDeleteProgramsNV = (PFNGLDELETEPROGRAMSNVPROC)IntGetProcAddress("glDeleteProgramsNV");
	if(!_funcptr_glDeleteProgramsNV) ++numFailed;
	_funcptr_glExecuteProgramNV = (PFNGLEXECUTEPROGRAMNVPROC)IntGetProcAddress("glExecuteProgramNV");
	if(!_funcptr_glExecuteProgramNV) ++numFailed;
	_funcptr_glGenProgramsNV = (PFNGLGENPROGRAMSNVPROC)IntGetProcAddress("glGenProgramsNV");
	if(!_funcptr_glGenProgramsNV) ++numFailed;
	_funcptr_glGetProgramParameterdvNV = (PFNGLGETPROGRAMPARAMETERDVNVPROC)IntGetProcAddress("glGetProgramParameterdvNV");
	if(!_funcptr_glGetProgramParameterdvNV) ++numFailed;
	_funcptr_glGetProgramParameterfvNV = (PFNGLGETPROGRAMPARAMETERFVNVPROC)IntGetProcAddress("glGetProgramParameterfvNV");
	if(!_funcptr_glGetProgramParameterfvNV) ++numFailed;
	_funcptr_glGetProgramivNV = (PFNGLGETPROGRAMIVNVPROC)IntGetProcAddress("glGetProgramivNV");
	if(!_funcptr_glGetProgramivNV) ++numFailed;
	_funcptr_glGetProgramStringNV = (PFNGLGETPROGRAMSTRINGNVPROC)IntGetProcAddress("glGetProgramStringNV");
	if(!_funcptr_glGetProgramStringNV) ++numFailed;
	_funcptr_glGetTrackMatrixivNV = (PFNGLGETTRACKMATRIXIVNVPROC)IntGetProcAddress("glGetTrackMatrixivNV");
	if(!_funcptr_glGetTrackMatrixivNV) ++numFailed;
	_funcptr_glGetVertexAttribdvNV = (PFNGLGETVERTEXATTRIBDVNVPROC)IntGetProcAddress("glGetVertexAttribdvNV");
	if(!_funcptr_glGetVertexAttribdvNV) ++numFailed;
	_funcptr_glGetVertexAttribfvNV = (PFNGLGETVERTEXATTRIBFVNVPROC)IntGetProcAddress("glGetVertexAttribfvNV");
	if(!_funcptr_glGetVertexAttribfvNV) ++numFailed;
	_funcptr_glGetVertexAttribivNV = (PFNGLGETVERTEXATTRIBIVNVPROC)IntGetProcAddress("glGetVertexAttribivNV");
	if(!_funcptr_glGetVertexAttribivNV) ++numFailed;
	_funcptr_glGetVertexAttribPointervNV = (PFNGLGETVERTEXATTRIBPOINTERVNVPROC)IntGetProcAddress("glGetVertexAttribPointervNV");
	if(!_funcptr_glGetVertexAttribPointervNV) ++numFailed;
	_funcptr_glIsProgramNV = (PFNGLISPROGRAMNVPROC)IntGetProcAddress("glIsProgramNV");
	if(!_funcptr_glIsProgramNV) ++numFailed;
	_funcptr_glLoadProgramNV = (PFNGLLOADPROGRAMNVPROC)IntGetProcAddress("glLoadProgramNV");
	if(!_funcptr_glLoadProgramNV) ++numFailed;
	_funcptr_glProgramParameter4dNV = (PFNGLPROGRAMPARAMETER4DNVPROC)IntGetProcAddress("glProgramParameter4dNV");
	if(!_funcptr_glProgramParameter4dNV) ++numFailed;
	_funcptr_glProgramParameter4dvNV = (PFNGLPROGRAMPARAMETER4DVNVPROC)IntGetProcAddress("glProgramParameter4dvNV");
	if(!_funcptr_glProgramParameter4dvNV) ++numFailed;
	_funcptr_glProgramParameter4fNV = (PFNGLPROGRAMPARAMETER4FNVPROC)IntGetProcAddress("glProgramParameter4fNV");
	if(!_funcptr_glProgramParameter4fNV) ++numFailed;
	_funcptr_glProgramParameter4fvNV = (PFNGLPROGRAMPARAMETER4FVNVPROC)IntGetProcAddress("glProgramParameter4fvNV");
	if(!_funcptr_glProgramParameter4fvNV) ++numFailed;
	_funcptr_glProgramParameters4dvNV = (PFNGLPROGRAMPARAMETERS4DVNVPROC)IntGetProcAddress("glProgramParameters4dvNV");
	if(!_funcptr_glProgramParameters4dvNV) ++numFailed;
	_funcptr_glProgramParameters4fvNV = (PFNGLPROGRAMPARAMETERS4FVNVPROC)IntGetProcAddress("glProgramParameters4fvNV");
	if(!_funcptr_glProgramParameters4fvNV) ++numFailed;
	_funcptr_glRequestResidentProgramsNV = (PFNGLREQUESTRESIDENTPROGRAMSNVPROC)IntGetProcAddress("glRequestResidentProgramsNV");
	if(!_funcptr_glRequestResidentProgramsNV) ++numFailed;
	_funcptr_glTrackMatrixNV = (PFNGLTRACKMATRIXNVPROC)IntGetProcAddress("glTrackMatrixNV");
	if(!_funcptr_glTrackMatrixNV) ++numFailed;
	_funcptr_glVertexAttribPointerNV = (PFNGLVERTEXATTRIBPOINTERNVPROC)IntGetProcAddress("glVertexAttribPointerNV");
	if(!_funcptr_glVertexAttribPointerNV) ++numFailed;
	_funcptr_glVertexAttrib1dNV = (PFNGLVERTEXATTRIB1DNVPROC)IntGetProcAddress("glVertexAttrib1dNV");
	if(!_funcptr_glVertexAttrib1dNV) ++numFailed;
	_funcptr_glVertexAttrib1dvNV = (PFNGLVERTEXATTRIB1DVNVPROC)IntGetProcAddress("glVertexAttrib1dvNV");
	if(!_funcptr_glVertexAttrib1dvNV) ++numFailed;
	_funcptr_glVertexAttrib1fNV = (PFNGLVERTEXATTRIB1FNVPROC)IntGetProcAddress("glVertexAttrib1fNV");
	if(!_funcptr_glVertexAttrib1fNV) ++numFailed;
	_funcptr_glVertexAttrib1fvNV = (PFNGLVERTEXATTRIB1FVNVPROC)IntGetProcAddress("glVertexAttrib1fvNV");
	if(!_funcptr_glVertexAttrib1fvNV) ++numFailed;
	_funcptr_glVertexAttrib1sNV = (PFNGLVERTEXATTRIB1SNVPROC)IntGetProcAddress("glVertexAttrib1sNV");
	if(!_funcptr_glVertexAttrib1sNV) ++numFailed;
	_funcptr_glVertexAttrib1svNV = (PFNGLVERTEXATTRIB1SVNVPROC)IntGetProcAddress("glVertexAttrib1svNV");
	if(!_funcptr_glVertexAttrib1svNV) ++numFailed;
	_funcptr_glVertexAttrib2dNV = (PFNGLVERTEXATTRIB2DNVPROC)IntGetProcAddress("glVertexAttrib2dNV");
	if(!_funcptr_glVertexAttrib2dNV) ++numFailed;
	_funcptr_glVertexAttrib2dvNV = (PFNGLVERTEXATTRIB2DVNVPROC)IntGetProcAddress("glVertexAttrib2dvNV");
	if(!_funcptr_glVertexAttrib2dvNV) ++numFailed;
	_funcptr_glVertexAttrib2fNV = (PFNGLVERTEXATTRIB2FNVPROC)IntGetProcAddress("glVertexAttrib2fNV");
	if(!_funcptr_glVertexAttrib2fNV) ++numFailed;
	_funcptr_glVertexAttrib2fvNV = (PFNGLVERTEXATTRIB2FVNVPROC)IntGetProcAddress("glVertexAttrib2fvNV");
	if(!_funcptr_glVertexAttrib2fvNV) ++numFailed;
	_funcptr_glVertexAttrib2sNV = (PFNGLVERTEXATTRIB2SNVPROC)IntGetProcAddress("glVertexAttrib2sNV");
	if(!_funcptr_glVertexAttrib2sNV) ++numFailed;
	_funcptr_glVertexAttrib2svNV = (PFNGLVERTEXATTRIB2SVNVPROC)IntGetProcAddress("glVertexAttrib2svNV");
	if(!_funcptr_glVertexAttrib2svNV) ++numFailed;
	_funcptr_glVertexAttrib3dNV = (PFNGLVERTEXATTRIB3DNVPROC)IntGetProcAddress("glVertexAttrib3dNV");
	if(!_funcptr_glVertexAttrib3dNV) ++numFailed;
	_funcptr_glVertexAttrib3dvNV = (PFNGLVERTEXATTRIB3DVNVPROC)IntGetProcAddress("glVertexAttrib3dvNV");
	if(!_funcptr_glVertexAttrib3dvNV) ++numFailed;
	_funcptr_glVertexAttrib3fNV = (PFNGLVERTEXATTRIB3FNVPROC)IntGetProcAddress("glVertexAttrib3fNV");
	if(!_funcptr_glVertexAttrib3fNV) ++numFailed;
	_funcptr_glVertexAttrib3fvNV = (PFNGLVERTEXATTRIB3FVNVPROC)IntGetProcAddress("glVertexAttrib3fvNV");
	if(!_funcptr_glVertexAttrib3fvNV) ++numFailed;
	_funcptr_glVertexAttrib3sNV = (PFNGLVERTEXATTRIB3SNVPROC)IntGetProcAddress("glVertexAttrib3sNV");
	if(!_funcptr_glVertexAttrib3sNV) ++numFailed;
	_funcptr_glVertexAttrib3svNV = (PFNGLVERTEXATTRIB3SVNVPROC)IntGetProcAddress("glVertexAttrib3svNV");
	if(!_funcptr_glVertexAttrib3svNV) ++numFailed;
	_funcptr_glVertexAttrib4dNV = (PFNGLVERTEXATTRIB4DNVPROC)IntGetProcAddress("glVertexAttrib4dNV");
	if(!_funcptr_glVertexAttrib4dNV) ++numFailed;
	_funcptr_glVertexAttrib4dvNV = (PFNGLVERTEXATTRIB4DVNVPROC)IntGetProcAddress("glVertexAttrib4dvNV");
	if(!_funcptr_glVertexAttrib4dvNV) ++numFailed;
	_funcptr_glVertexAttrib4fNV = (PFNGLVERTEXATTRIB4FNVPROC)IntGetProcAddress("glVertexAttrib4fNV");
	if(!_funcptr_glVertexAttrib4fNV) ++numFailed;
	_funcptr_glVertexAttrib4fvNV = (PFNGLVERTEXATTRIB4FVNVPROC)IntGetProcAddress("glVertexAttrib4fvNV");
	if(!_funcptr_glVertexAttrib4fvNV) ++numFailed;
	_funcptr_glVertexAttrib4sNV = (PFNGLVERTEXATTRIB4SNVPROC)IntGetProcAddress("glVertexAttrib4sNV");
	if(!_funcptr_glVertexAttrib4sNV) ++numFailed;
	_funcptr_glVertexAttrib4svNV = (PFNGLVERTEXATTRIB4SVNVPROC)IntGetProcAddress("glVertexAttrib4svNV");
	if(!_funcptr_glVertexAttrib4svNV) ++numFailed;
	_funcptr_glVertexAttrib4ubNV = (PFNGLVERTEXATTRIB4UBNVPROC)IntGetProcAddress("glVertexAttrib4ubNV");
	if(!_funcptr_glVertexAttrib4ubNV) ++numFailed;
	_funcptr_glVertexAttrib4ubvNV = (PFNGLVERTEXATTRIB4UBVNVPROC)IntGetProcAddress("glVertexAttrib4ubvNV");
	if(!_funcptr_glVertexAttrib4ubvNV) ++numFailed;
	_funcptr_glVertexAttribs1dvNV = (PFNGLVERTEXATTRIBS1DVNVPROC)IntGetProcAddress("glVertexAttribs1dvNV");
	if(!_funcptr_glVertexAttribs1dvNV) ++numFailed;
	_funcptr_glVertexAttribs1fvNV = (PFNGLVERTEXATTRIBS1FVNVPROC)IntGetProcAddress("glVertexAttribs1fvNV");
	if(!_funcptr_glVertexAttribs1fvNV) ++numFailed;
	_funcptr_glVertexAttribs1svNV = (PFNGLVERTEXATTRIBS1SVNVPROC)IntGetProcAddress("glVertexAttribs1svNV");
	if(!_funcptr_glVertexAttribs1svNV) ++numFailed;
	_funcptr_glVertexAttribs2dvNV = (PFNGLVERTEXATTRIBS2DVNVPROC)IntGetProcAddress("glVertexAttribs2dvNV");
	if(!_funcptr_glVertexAttribs2dvNV) ++numFailed;
	_funcptr_glVertexAttribs2fvNV = (PFNGLVERTEXATTRIBS2FVNVPROC)IntGetProcAddress("glVertexAttribs2fvNV");
	if(!_funcptr_glVertexAttribs2fvNV) ++numFailed;
	_funcptr_glVertexAttribs2svNV = (PFNGLVERTEXATTRIBS2SVNVPROC)IntGetProcAddress("glVertexAttribs2svNV");
	if(!_funcptr_glVertexAttribs2svNV) ++numFailed;
	_funcptr_glVertexAttribs3dvNV = (PFNGLVERTEXATTRIBS3DVNVPROC)IntGetProcAddress("glVertexAttribs3dvNV");
	if(!_funcptr_glVertexAttribs3dvNV) ++numFailed;
	_funcptr_glVertexAttribs3fvNV = (PFNGLVERTEXATTRIBS3FVNVPROC)IntGetProcAddress("glVertexAttribs3fvNV");
	if(!_funcptr_glVertexAttribs3fvNV) ++numFailed;
	_funcptr_glVertexAttribs3svNV = (PFNGLVERTEXATTRIBS3SVNVPROC)IntGetProcAddress("glVertexAttribs3svNV");
	if(!_funcptr_glVertexAttribs3svNV) ++numFailed;
	_funcptr_glVertexAttribs4dvNV = (PFNGLVERTEXATTRIBS4DVNVPROC)IntGetProcAddress("glVertexAttribs4dvNV");
	if(!_funcptr_glVertexAttribs4dvNV) ++numFailed;
	_funcptr_glVertexAttribs4fvNV = (PFNGLVERTEXATTRIBS4FVNVPROC)IntGetProcAddress("glVertexAttribs4fvNV");
	if(!_funcptr_glVertexAttribs4fvNV) ++numFailed;
	_funcptr_glVertexAttribs4svNV = (PFNGLVERTEXATTRIBS4SVNVPROC)IntGetProcAddress("glVertexAttribs4svNV");
	if(!_funcptr_glVertexAttribs4svNV) ++numFailed;
	_funcptr_glVertexAttribs4ubvNV = (PFNGLVERTEXATTRIBS4UBVNVPROC)IntGetProcAddress("glVertexAttribs4ubvNV");
	if(!_funcptr_glVertexAttribs4ubvNV) ++numFailed;
	return numFailed;
}

PFNGLVERTEXATTRIBI1IEXTPROC _funcptr_glVertexAttribI1iEXT = NULL;
PFNGLVERTEXATTRIBI2IEXTPROC _funcptr_glVertexAttribI2iEXT = NULL;
PFNGLVERTEXATTRIBI3IEXTPROC _funcptr_glVertexAttribI3iEXT = NULL;
PFNGLVERTEXATTRIBI4IEXTPROC _funcptr_glVertexAttribI4iEXT = NULL;
PFNGLVERTEXATTRIBI1UIEXTPROC _funcptr_glVertexAttribI1uiEXT = NULL;
PFNGLVERTEXATTRIBI2UIEXTPROC _funcptr_glVertexAttribI2uiEXT = NULL;
PFNGLVERTEXATTRIBI3UIEXTPROC _funcptr_glVertexAttribI3uiEXT = NULL;
PFNGLVERTEXATTRIBI4UIEXTPROC _funcptr_glVertexAttribI4uiEXT = NULL;
PFNGLVERTEXATTRIBI1IVEXTPROC _funcptr_glVertexAttribI1ivEXT = NULL;
PFNGLVERTEXATTRIBI2IVEXTPROC _funcptr_glVertexAttribI2ivEXT = NULL;
PFNGLVERTEXATTRIBI3IVEXTPROC _funcptr_glVertexAttribI3ivEXT = NULL;
PFNGLVERTEXATTRIBI4IVEXTPROC _funcptr_glVertexAttribI4ivEXT = NULL;
PFNGLVERTEXATTRIBI1UIVEXTPROC _funcptr_glVertexAttribI1uivEXT = NULL;
PFNGLVERTEXATTRIBI2UIVEXTPROC _funcptr_glVertexAttribI2uivEXT = NULL;
PFNGLVERTEXATTRIBI3UIVEXTPROC _funcptr_glVertexAttribI3uivEXT = NULL;
PFNGLVERTEXATTRIBI4UIVEXTPROC _funcptr_glVertexAttribI4uivEXT = NULL;
PFNGLVERTEXATTRIBI4BVEXTPROC _funcptr_glVertexAttribI4bvEXT = NULL;
PFNGLVERTEXATTRIBI4SVEXTPROC _funcptr_glVertexAttribI4svEXT = NULL;
PFNGLVERTEXATTRIBI4UBVEXTPROC _funcptr_glVertexAttribI4ubvEXT = NULL;
PFNGLVERTEXATTRIBI4USVEXTPROC _funcptr_glVertexAttribI4usvEXT = NULL;
PFNGLVERTEXATTRIBIPOINTEREXTPROC _funcptr_glVertexAttribIPointerEXT = NULL;
PFNGLGETVERTEXATTRIBIIVEXTPROC _funcptr_glGetVertexAttribIivEXT = NULL;
PFNGLGETVERTEXATTRIBIUIVEXTPROC _funcptr_glGetVertexAttribIuivEXT = NULL;

static int LoadExt_NV_vertex_program4()
{
	int numFailed = 0;
	_funcptr_glVertexAttribI1iEXT = (PFNGLVERTEXATTRIBI1IEXTPROC)IntGetProcAddress("glVertexAttribI1iEXT");
	if(!_funcptr_glVertexAttribI1iEXT) ++numFailed;
	_funcptr_glVertexAttribI2iEXT = (PFNGLVERTEXATTRIBI2IEXTPROC)IntGetProcAddress("glVertexAttribI2iEXT");
	if(!_funcptr_glVertexAttribI2iEXT) ++numFailed;
	_funcptr_glVertexAttribI3iEXT = (PFNGLVERTEXATTRIBI3IEXTPROC)IntGetProcAddress("glVertexAttribI3iEXT");
	if(!_funcptr_glVertexAttribI3iEXT) ++numFailed;
	_funcptr_glVertexAttribI4iEXT = (PFNGLVERTEXATTRIBI4IEXTPROC)IntGetProcAddress("glVertexAttribI4iEXT");
	if(!_funcptr_glVertexAttribI4iEXT) ++numFailed;
	_funcptr_glVertexAttribI1uiEXT = (PFNGLVERTEXATTRIBI1UIEXTPROC)IntGetProcAddress("glVertexAttribI1uiEXT");
	if(!_funcptr_glVertexAttribI1uiEXT) ++numFailed;
	_funcptr_glVertexAttribI2uiEXT = (PFNGLVERTEXATTRIBI2UIEXTPROC)IntGetProcAddress("glVertexAttribI2uiEXT");
	if(!_funcptr_glVertexAttribI2uiEXT) ++numFailed;
	_funcptr_glVertexAttribI3uiEXT = (PFNGLVERTEXATTRIBI3UIEXTPROC)IntGetProcAddress("glVertexAttribI3uiEXT");
	if(!_funcptr_glVertexAttribI3uiEXT) ++numFailed;
	_funcptr_glVertexAttribI4uiEXT = (PFNGLVERTEXATTRIBI4UIEXTPROC)IntGetProcAddress("glVertexAttribI4uiEXT");
	if(!_funcptr_glVertexAttribI4uiEXT) ++numFailed;
	_funcptr_glVertexAttribI1ivEXT = (PFNGLVERTEXATTRIBI1IVEXTPROC)IntGetProcAddress("glVertexAttribI1ivEXT");
	if(!_funcptr_glVertexAttribI1ivEXT) ++numFailed;
	_funcptr_glVertexAttribI2ivEXT = (PFNGLVERTEXATTRIBI2IVEXTPROC)IntGetProcAddress("glVertexAttribI2ivEXT");
	if(!_funcptr_glVertexAttribI2ivEXT) ++numFailed;
	_funcptr_glVertexAttribI3ivEXT = (PFNGLVERTEXATTRIBI3IVEXTPROC)IntGetProcAddress("glVertexAttribI3ivEXT");
	if(!_funcptr_glVertexAttribI3ivEXT) ++numFailed;
	_funcptr_glVertexAttribI4ivEXT = (PFNGLVERTEXATTRIBI4IVEXTPROC)IntGetProcAddress("glVertexAttribI4ivEXT");
	if(!_funcptr_glVertexAttribI4ivEXT) ++numFailed;
	_funcptr_glVertexAttribI1uivEXT = (PFNGLVERTEXATTRIBI1UIVEXTPROC)IntGetProcAddress("glVertexAttribI1uivEXT");
	if(!_funcptr_glVertexAttribI1uivEXT) ++numFailed;
	_funcptr_glVertexAttribI2uivEXT = (PFNGLVERTEXATTRIBI2UIVEXTPROC)IntGetProcAddress("glVertexAttribI2uivEXT");
	if(!_funcptr_glVertexAttribI2uivEXT) ++numFailed;
	_funcptr_glVertexAttribI3uivEXT = (PFNGLVERTEXATTRIBI3UIVEXTPROC)IntGetProcAddress("glVertexAttribI3uivEXT");
	if(!_funcptr_glVertexAttribI3uivEXT) ++numFailed;
	_funcptr_glVertexAttribI4uivEXT = (PFNGLVERTEXATTRIBI4UIVEXTPROC)IntGetProcAddress("glVertexAttribI4uivEXT");
	if(!_funcptr_glVertexAttribI4uivEXT) ++numFailed;
	_funcptr_glVertexAttribI4bvEXT = (PFNGLVERTEXATTRIBI4BVEXTPROC)IntGetProcAddress("glVertexAttribI4bvEXT");
	if(!_funcptr_glVertexAttribI4bvEXT) ++numFailed;
	_funcptr_glVertexAttribI4svEXT = (PFNGLVERTEXATTRIBI4SVEXTPROC)IntGetProcAddress("glVertexAttribI4svEXT");
	if(!_funcptr_glVertexAttribI4svEXT) ++numFailed;
	_funcptr_glVertexAttribI4ubvEXT = (PFNGLVERTEXATTRIBI4UBVEXTPROC)IntGetProcAddress("glVertexAttribI4ubvEXT");
	if(!_funcptr_glVertexAttribI4ubvEXT) ++numFailed;
	_funcptr_glVertexAttribI4usvEXT = (PFNGLVERTEXATTRIBI4USVEXTPROC)IntGetProcAddress("glVertexAttribI4usvEXT");
	if(!_funcptr_glVertexAttribI4usvEXT) ++numFailed;
	_funcptr_glVertexAttribIPointerEXT = (PFNGLVERTEXATTRIBIPOINTEREXTPROC)IntGetProcAddress("glVertexAttribIPointerEXT");
	if(!_funcptr_glVertexAttribIPointerEXT) ++numFailed;
	_funcptr_glGetVertexAttribIivEXT = (PFNGLGETVERTEXATTRIBIIVEXTPROC)IntGetProcAddress("glGetVertexAttribIivEXT");
	if(!_funcptr_glGetVertexAttribIivEXT) ++numFailed;
	_funcptr_glGetVertexAttribIuivEXT = (PFNGLGETVERTEXATTRIBIUIVEXTPROC)IntGetProcAddress("glGetVertexAttribIuivEXT");
	if(!_funcptr_glGetVertexAttribIuivEXT) ++numFailed;
	return numFailed;
}

PFNGLBEGINVIDEOCAPTURENVPROC _funcptr_glBeginVideoCaptureNV = NULL;
PFNGLBINDVIDEOCAPTURESTREAMBUFFERNVPROC _funcptr_glBindVideoCaptureStreamBufferNV = NULL;
PFNGLBINDVIDEOCAPTURESTREAMTEXTURENVPROC _funcptr_glBindVideoCaptureStreamTextureNV = NULL;
PFNGLENDVIDEOCAPTURENVPROC _funcptr_glEndVideoCaptureNV = NULL;
PFNGLGETVIDEOCAPTUREIVNVPROC _funcptr_glGetVideoCaptureivNV = NULL;
PFNGLGETVIDEOCAPTURESTREAMIVNVPROC _funcptr_glGetVideoCaptureStreamivNV = NULL;
PFNGLGETVIDEOCAPTURESTREAMFVNVPROC _funcptr_glGetVideoCaptureStreamfvNV = NULL;
PFNGLGETVIDEOCAPTURESTREAMDVNVPROC _funcptr_glGetVideoCaptureStreamdvNV = NULL;
PFNGLVIDEOCAPTURENVPROC _funcptr_glVideoCaptureNV = NULL;
PFNGLVIDEOCAPTURESTREAMPARAMETERIVNVPROC _funcptr_glVideoCaptureStreamParameterivNV = NULL;
PFNGLVIDEOCAPTURESTREAMPARAMETERFVNVPROC _funcptr_glVideoCaptureStreamParameterfvNV = NULL;
PFNGLVIDEOCAPTURESTREAMPARAMETERDVNVPROC _funcptr_glVideoCaptureStreamParameterdvNV = NULL;

static int LoadExt_NV_video_capture()
{
	int numFailed = 0;
	_funcptr_glBeginVideoCaptureNV = (PFNGLBEGINVIDEOCAPTURENVPROC)IntGetProcAddress("glBeginVideoCaptureNV");
	if(!_funcptr_glBeginVideoCaptureNV) ++numFailed;
	_funcptr_glBindVideoCaptureStreamBufferNV = (PFNGLBINDVIDEOCAPTURESTREAMBUFFERNVPROC)IntGetProcAddress("glBindVideoCaptureStreamBufferNV");
	if(!_funcptr_glBindVideoCaptureStreamBufferNV) ++numFailed;
	_funcptr_glBindVideoCaptureStreamTextureNV = (PFNGLBINDVIDEOCAPTURESTREAMTEXTURENVPROC)IntGetProcAddress("glBindVideoCaptureStreamTextureNV");
	if(!_funcptr_glBindVideoCaptureStreamTextureNV) ++numFailed;
	_funcptr_glEndVideoCaptureNV = (PFNGLENDVIDEOCAPTURENVPROC)IntGetProcAddress("glEndVideoCaptureNV");
	if(!_funcptr_glEndVideoCaptureNV) ++numFailed;
	_funcptr_glGetVideoCaptureivNV = (PFNGLGETVIDEOCAPTUREIVNVPROC)IntGetProcAddress("glGetVideoCaptureivNV");
	if(!_funcptr_glGetVideoCaptureivNV) ++numFailed;
	_funcptr_glGetVideoCaptureStreamivNV = (PFNGLGETVIDEOCAPTURESTREAMIVNVPROC)IntGetProcAddress("glGetVideoCaptureStreamivNV");
	if(!_funcptr_glGetVideoCaptureStreamivNV) ++numFailed;
	_funcptr_glGetVideoCaptureStreamfvNV = (PFNGLGETVIDEOCAPTURESTREAMFVNVPROC)IntGetProcAddress("glGetVideoCaptureStreamfvNV");
	if(!_funcptr_glGetVideoCaptureStreamfvNV) ++numFailed;
	_funcptr_glGetVideoCaptureStreamdvNV = (PFNGLGETVIDEOCAPTURESTREAMDVNVPROC)IntGetProcAddress("glGetVideoCaptureStreamdvNV");
	if(!_funcptr_glGetVideoCaptureStreamdvNV) ++numFailed;
	_funcptr_glVideoCaptureNV = (PFNGLVIDEOCAPTURENVPROC)IntGetProcAddress("glVideoCaptureNV");
	if(!_funcptr_glVideoCaptureNV) ++numFailed;
	_funcptr_glVideoCaptureStreamParameterivNV = (PFNGLVIDEOCAPTURESTREAMPARAMETERIVNVPROC)IntGetProcAddress("glVideoCaptureStreamParameterivNV");
	if(!_funcptr_glVideoCaptureStreamParameterivNV) ++numFailed;
	_funcptr_glVideoCaptureStreamParameterfvNV = (PFNGLVIDEOCAPTURESTREAMPARAMETERFVNVPROC)IntGetProcAddress("glVideoCaptureStreamParameterfvNV");
	if(!_funcptr_glVideoCaptureStreamParameterfvNV) ++numFailed;
	_funcptr_glVideoCaptureStreamParameterdvNV = (PFNGLVIDEOCAPTURESTREAMPARAMETERDVNVPROC)IntGetProcAddress("glVideoCaptureStreamParameterdvNV");
	if(!_funcptr_glVideoCaptureStreamParameterdvNV) ++numFailed;
	return numFailed;
}

PFNGLMULTITEXCOORD1BOESPROC _funcptr_glMultiTexCoord1bOES = NULL;
PFNGLMULTITEXCOORD1BVOESPROC _funcptr_glMultiTexCoord1bvOES = NULL;
PFNGLMULTITEXCOORD2BOESPROC _funcptr_glMultiTexCoord2bOES = NULL;
PFNGLMULTITEXCOORD2BVOESPROC _funcptr_glMultiTexCoord2bvOES = NULL;
PFNGLMULTITEXCOORD3BOESPROC _funcptr_glMultiTexCoord3bOES = NULL;
PFNGLMULTITEXCOORD3BVOESPROC _funcptr_glMultiTexCoord3bvOES = NULL;
PFNGLMULTITEXCOORD4BOESPROC _funcptr_glMultiTexCoord4bOES = NULL;
PFNGLMULTITEXCOORD4BVOESPROC _funcptr_glMultiTexCoord4bvOES = NULL;
PFNGLTEXCOORD1BOESPROC _funcptr_glTexCoord1bOES = NULL;
PFNGLTEXCOORD1BVOESPROC _funcptr_glTexCoord1bvOES = NULL;
PFNGLTEXCOORD2BOESPROC _funcptr_glTexCoord2bOES = NULL;
PFNGLTEXCOORD2BVOESPROC _funcptr_glTexCoord2bvOES = NULL;
PFNGLTEXCOORD3BOESPROC _funcptr_glTexCoord3bOES = NULL;
PFNGLTEXCOORD3BVOESPROC _funcptr_glTexCoord3bvOES = NULL;
PFNGLTEXCOORD4BOESPROC _funcptr_glTexCoord4bOES = NULL;
PFNGLTEXCOORD4BVOESPROC _funcptr_glTexCoord4bvOES = NULL;
PFNGLVERTEX2BOESPROC _funcptr_glVertex2bOES = NULL;
PFNGLVERTEX2BVOESPROC _funcptr_glVertex2bvOES = NULL;
PFNGLVERTEX3BOESPROC _funcptr_glVertex3bOES = NULL;
PFNGLVERTEX3BVOESPROC _funcptr_glVertex3bvOES = NULL;
PFNGLVERTEX4BOESPROC _funcptr_glVertex4bOES = NULL;
PFNGLVERTEX4BVOESPROC _funcptr_glVertex4bvOES = NULL;

static int LoadExt_OES_byte_coordinates()
{
	int numFailed = 0;
	_funcptr_glMultiTexCoord1bOES = (PFNGLMULTITEXCOORD1BOESPROC)IntGetProcAddress("glMultiTexCoord1bOES");
	if(!_funcptr_glMultiTexCoord1bOES) ++numFailed;
	_funcptr_glMultiTexCoord1bvOES = (PFNGLMULTITEXCOORD1BVOESPROC)IntGetProcAddress("glMultiTexCoord1bvOES");
	if(!_funcptr_glMultiTexCoord1bvOES) ++numFailed;
	_funcptr_glMultiTexCoord2bOES = (PFNGLMULTITEXCOORD2BOESPROC)IntGetProcAddress("glMultiTexCoord2bOES");
	if(!_funcptr_glMultiTexCoord2bOES) ++numFailed;
	_funcptr_glMultiTexCoord2bvOES = (PFNGLMULTITEXCOORD2BVOESPROC)IntGetProcAddress("glMultiTexCoord2bvOES");
	if(!_funcptr_glMultiTexCoord2bvOES) ++numFailed;
	_funcptr_glMultiTexCoord3bOES = (PFNGLMULTITEXCOORD3BOESPROC)IntGetProcAddress("glMultiTexCoord3bOES");
	if(!_funcptr_glMultiTexCoord3bOES) ++numFailed;
	_funcptr_glMultiTexCoord3bvOES = (PFNGLMULTITEXCOORD3BVOESPROC)IntGetProcAddress("glMultiTexCoord3bvOES");
	if(!_funcptr_glMultiTexCoord3bvOES) ++numFailed;
	_funcptr_glMultiTexCoord4bOES = (PFNGLMULTITEXCOORD4BOESPROC)IntGetProcAddress("glMultiTexCoord4bOES");
	if(!_funcptr_glMultiTexCoord4bOES) ++numFailed;
	_funcptr_glMultiTexCoord4bvOES = (PFNGLMULTITEXCOORD4BVOESPROC)IntGetProcAddress("glMultiTexCoord4bvOES");
	if(!_funcptr_glMultiTexCoord4bvOES) ++numFailed;
	_funcptr_glTexCoord1bOES = (PFNGLTEXCOORD1BOESPROC)IntGetProcAddress("glTexCoord1bOES");
	if(!_funcptr_glTexCoord1bOES) ++numFailed;
	_funcptr_glTexCoord1bvOES = (PFNGLTEXCOORD1BVOESPROC)IntGetProcAddress("glTexCoord1bvOES");
	if(!_funcptr_glTexCoord1bvOES) ++numFailed;
	_funcptr_glTexCoord2bOES = (PFNGLTEXCOORD2BOESPROC)IntGetProcAddress("glTexCoord2bOES");
	if(!_funcptr_glTexCoord2bOES) ++numFailed;
	_funcptr_glTexCoord2bvOES = (PFNGLTEXCOORD2BVOESPROC)IntGetProcAddress("glTexCoord2bvOES");
	if(!_funcptr_glTexCoord2bvOES) ++numFailed;
	_funcptr_glTexCoord3bOES = (PFNGLTEXCOORD3BOESPROC)IntGetProcAddress("glTexCoord3bOES");
	if(!_funcptr_glTexCoord3bOES) ++numFailed;
	_funcptr_glTexCoord3bvOES = (PFNGLTEXCOORD3BVOESPROC)IntGetProcAddress("glTexCoord3bvOES");
	if(!_funcptr_glTexCoord3bvOES) ++numFailed;
	_funcptr_glTexCoord4bOES = (PFNGLTEXCOORD4BOESPROC)IntGetProcAddress("glTexCoord4bOES");
	if(!_funcptr_glTexCoord4bOES) ++numFailed;
	_funcptr_glTexCoord4bvOES = (PFNGLTEXCOORD4BVOESPROC)IntGetProcAddress("glTexCoord4bvOES");
	if(!_funcptr_glTexCoord4bvOES) ++numFailed;
	_funcptr_glVertex2bOES = (PFNGLVERTEX2BOESPROC)IntGetProcAddress("glVertex2bOES");
	if(!_funcptr_glVertex2bOES) ++numFailed;
	_funcptr_glVertex2bvOES = (PFNGLVERTEX2BVOESPROC)IntGetProcAddress("glVertex2bvOES");
	if(!_funcptr_glVertex2bvOES) ++numFailed;
	_funcptr_glVertex3bOES = (PFNGLVERTEX3BOESPROC)IntGetProcAddress("glVertex3bOES");
	if(!_funcptr_glVertex3bOES) ++numFailed;
	_funcptr_glVertex3bvOES = (PFNGLVERTEX3BVOESPROC)IntGetProcAddress("glVertex3bvOES");
	if(!_funcptr_glVertex3bvOES) ++numFailed;
	_funcptr_glVertex4bOES = (PFNGLVERTEX4BOESPROC)IntGetProcAddress("glVertex4bOES");
	if(!_funcptr_glVertex4bOES) ++numFailed;
	_funcptr_glVertex4bvOES = (PFNGLVERTEX4BVOESPROC)IntGetProcAddress("glVertex4bvOES");
	if(!_funcptr_glVertex4bvOES) ++numFailed;
	return numFailed;
}

PFNGLACCUMXOESPROC _funcptr_glAccumxOES = NULL;
PFNGLALPHAFUNCXOESPROC _funcptr_glAlphaFuncxOES = NULL;
PFNGLBITMAPXOESPROC _funcptr_glBitmapxOES = NULL;
PFNGLBLENDCOLORXOESPROC _funcptr_glBlendColorxOES = NULL;
PFNGLCLEARACCUMXOESPROC _funcptr_glClearAccumxOES = NULL;
PFNGLCLEARCOLORXOESPROC _funcptr_glClearColorxOES = NULL;
PFNGLCLEARDEPTHXOESPROC _funcptr_glClearDepthxOES = NULL;
PFNGLCLIPPLANEXOESPROC _funcptr_glClipPlanexOES = NULL;
PFNGLCOLOR3XOESPROC _funcptr_glColor3xOES = NULL;
PFNGLCOLOR4XOESPROC _funcptr_glColor4xOES = NULL;
PFNGLCOLOR3XVOESPROC _funcptr_glColor3xvOES = NULL;
PFNGLCOLOR4XVOESPROC _funcptr_glColor4xvOES = NULL;
PFNGLCONVOLUTIONPARAMETERXOESPROC _funcptr_glConvolutionParameterxOES = NULL;
PFNGLCONVOLUTIONPARAMETERXVOESPROC _funcptr_glConvolutionParameterxvOES = NULL;
PFNGLDEPTHRANGEXOESPROC _funcptr_glDepthRangexOES = NULL;
PFNGLEVALCOORD1XOESPROC _funcptr_glEvalCoord1xOES = NULL;
PFNGLEVALCOORD2XOESPROC _funcptr_glEvalCoord2xOES = NULL;
PFNGLEVALCOORD1XVOESPROC _funcptr_glEvalCoord1xvOES = NULL;
PFNGLEVALCOORD2XVOESPROC _funcptr_glEvalCoord2xvOES = NULL;
PFNGLFEEDBACKBUFFERXOESPROC _funcptr_glFeedbackBufferxOES = NULL;
PFNGLFOGXOESPROC _funcptr_glFogxOES = NULL;
PFNGLFOGXVOESPROC _funcptr_glFogxvOES = NULL;
PFNGLFRUSTUMXOESPROC _funcptr_glFrustumxOES = NULL;
PFNGLGETCLIPPLANEXOESPROC _funcptr_glGetClipPlanexOES = NULL;
PFNGLGETCONVOLUTIONPARAMETERXVOESPROC _funcptr_glGetConvolutionParameterxvOES = NULL;
PFNGLGETFIXEDVOESPROC _funcptr_glGetFixedvOES = NULL;
PFNGLGETHISTOGRAMPARAMETERXVOESPROC _funcptr_glGetHistogramParameterxvOES = NULL;
PFNGLGETLIGHTXOESPROC _funcptr_glGetLightxOES = NULL;
PFNGLGETMAPXVOESPROC _funcptr_glGetMapxvOES = NULL;
PFNGLGETMATERIALXOESPROC _funcptr_glGetMaterialxOES = NULL;
PFNGLGETPIXELMAPXVPROC _funcptr_glGetPixelMapxv = NULL;
PFNGLGETTEXENVXVOESPROC _funcptr_glGetTexEnvxvOES = NULL;
PFNGLGETTEXGENXVOESPROC _funcptr_glGetTexGenxvOES = NULL;
PFNGLGETTEXLEVELPARAMETERXVOESPROC _funcptr_glGetTexLevelParameterxvOES = NULL;
PFNGLGETTEXPARAMETERXVOESPROC _funcptr_glGetTexParameterxvOES = NULL;
PFNGLINDEXXOESPROC _funcptr_glIndexxOES = NULL;
PFNGLINDEXXVOESPROC _funcptr_glIndexxvOES = NULL;
PFNGLLIGHTMODELXOESPROC _funcptr_glLightModelxOES = NULL;
PFNGLLIGHTMODELXVOESPROC _funcptr_glLightModelxvOES = NULL;
PFNGLLIGHTXOESPROC _funcptr_glLightxOES = NULL;
PFNGLLIGHTXVOESPROC _funcptr_glLightxvOES = NULL;
PFNGLLINEWIDTHXOESPROC _funcptr_glLineWidthxOES = NULL;
PFNGLLOADMATRIXXOESPROC _funcptr_glLoadMatrixxOES = NULL;
PFNGLLOADTRANSPOSEMATRIXXOESPROC _funcptr_glLoadTransposeMatrixxOES = NULL;
PFNGLMAP1XOESPROC _funcptr_glMap1xOES = NULL;
PFNGLMAP2XOESPROC _funcptr_glMap2xOES = NULL;
PFNGLMAPGRID1XOESPROC _funcptr_glMapGrid1xOES = NULL;
PFNGLMAPGRID2XOESPROC _funcptr_glMapGrid2xOES = NULL;
PFNGLMATERIALXOESPROC _funcptr_glMaterialxOES = NULL;
PFNGLMATERIALXVOESPROC _funcptr_glMaterialxvOES = NULL;
PFNGLMULTMATRIXXOESPROC _funcptr_glMultMatrixxOES = NULL;
PFNGLMULTTRANSPOSEMATRIXXOESPROC _funcptr_glMultTransposeMatrixxOES = NULL;
PFNGLMULTITEXCOORD1XOESPROC _funcptr_glMultiTexCoord1xOES = NULL;
PFNGLMULTITEXCOORD2XOESPROC _funcptr_glMultiTexCoord2xOES = NULL;
PFNGLMULTITEXCOORD3XOESPROC _funcptr_glMultiTexCoord3xOES = NULL;
PFNGLMULTITEXCOORD4XOESPROC _funcptr_glMultiTexCoord4xOES = NULL;
PFNGLMULTITEXCOORD1XVOESPROC _funcptr_glMultiTexCoord1xvOES = NULL;
PFNGLMULTITEXCOORD2XVOESPROC _funcptr_glMultiTexCoord2xvOES = NULL;
PFNGLMULTITEXCOORD3XVOESPROC _funcptr_glMultiTexCoord3xvOES = NULL;
PFNGLMULTITEXCOORD4XVOESPROC _funcptr_glMultiTexCoord4xvOES = NULL;
PFNGLNORMAL3XOESPROC _funcptr_glNormal3xOES = NULL;
PFNGLNORMAL3XVOESPROC _funcptr_glNormal3xvOES = NULL;
PFNGLORTHOXOESPROC _funcptr_glOrthoxOES = NULL;
PFNGLPASSTHROUGHXOESPROC _funcptr_glPassThroughxOES = NULL;
PFNGLPIXELMAPXPROC _funcptr_glPixelMapx = NULL;
PFNGLPIXELSTOREXPROC _funcptr_glPixelStorex = NULL;
PFNGLPIXELTRANSFERXOESPROC _funcptr_glPixelTransferxOES = NULL;
PFNGLPIXELZOOMXOESPROC _funcptr_glPixelZoomxOES = NULL;
PFNGLPOINTPARAMETERXVOESPROC _funcptr_glPointParameterxvOES = NULL;
PFNGLPOINTSIZEXOESPROC _funcptr_glPointSizexOES = NULL;
PFNGLPOLYGONOFFSETXOESPROC _funcptr_glPolygonOffsetxOES = NULL;
PFNGLPRIORITIZETEXTURESXOESPROC _funcptr_glPrioritizeTexturesxOES = NULL;
PFNGLRASTERPOS2XOESPROC _funcptr_glRasterPos2xOES = NULL;
PFNGLRASTERPOS3XOESPROC _funcptr_glRasterPos3xOES = NULL;
PFNGLRASTERPOS4XOESPROC _funcptr_glRasterPos4xOES = NULL;
PFNGLRASTERPOS2XVOESPROC _funcptr_glRasterPos2xvOES = NULL;
PFNGLRASTERPOS3XVOESPROC _funcptr_glRasterPos3xvOES = NULL;
PFNGLRASTERPOS4XVOESPROC _funcptr_glRasterPos4xvOES = NULL;
PFNGLRECTXOESPROC _funcptr_glRectxOES = NULL;
PFNGLRECTXVOESPROC _funcptr_glRectxvOES = NULL;
PFNGLROTATEXOESPROC _funcptr_glRotatexOES = NULL;
PFNGLSAMPLECOVERAGEOESPROC _funcptr_glSampleCoverageOES = NULL;
PFNGLSCALEXOESPROC _funcptr_glScalexOES = NULL;
PFNGLTEXCOORD1XOESPROC _funcptr_glTexCoord1xOES = NULL;
PFNGLTEXCOORD2XOESPROC _funcptr_glTexCoord2xOES = NULL;
PFNGLTEXCOORD3XOESPROC _funcptr_glTexCoord3xOES = NULL;
PFNGLTEXCOORD4XOESPROC _funcptr_glTexCoord4xOES = NULL;
PFNGLTEXCOORD1XVOESPROC _funcptr_glTexCoord1xvOES = NULL;
PFNGLTEXCOORD2XVOESPROC _funcptr_glTexCoord2xvOES = NULL;
PFNGLTEXCOORD3XVOESPROC _funcptr_glTexCoord3xvOES = NULL;
PFNGLTEXCOORD4XVOESPROC _funcptr_glTexCoord4xvOES = NULL;
PFNGLTEXENVXOESPROC _funcptr_glTexEnvxOES = NULL;
PFNGLTEXENVXVOESPROC _funcptr_glTexEnvxvOES = NULL;
PFNGLTEXGENXOESPROC _funcptr_glTexGenxOES = NULL;
PFNGLTEXGENXVOESPROC _funcptr_glTexGenxvOES = NULL;
PFNGLTEXPARAMETERXOESPROC _funcptr_glTexParameterxOES = NULL;
PFNGLTEXPARAMETERXVOESPROC _funcptr_glTexParameterxvOES = NULL;
PFNGLTRANSLATEXOESPROC _funcptr_glTranslatexOES = NULL;
PFNGLVERTEX2XOESPROC _funcptr_glVertex2xOES = NULL;
PFNGLVERTEX3XOESPROC _funcptr_glVertex3xOES = NULL;
PFNGLVERTEX4XOESPROC _funcptr_glVertex4xOES = NULL;
PFNGLVERTEX2XVOESPROC _funcptr_glVertex2xvOES = NULL;
PFNGLVERTEX3XVOESPROC _funcptr_glVertex3xvOES = NULL;
PFNGLVERTEX4XVOESPROC _funcptr_glVertex4xvOES = NULL;

static int LoadExt_OES_fixed_point()
{
	int numFailed = 0;
	_funcptr_glAccumxOES = (PFNGLACCUMXOESPROC)IntGetProcAddress("glAccumxOES");
	if(!_funcptr_glAccumxOES) ++numFailed;
	_funcptr_glAlphaFuncxOES = (PFNGLALPHAFUNCXOESPROC)IntGetProcAddress("glAlphaFuncxOES");
	if(!_funcptr_glAlphaFuncxOES) ++numFailed;
	_funcptr_glBitmapxOES = (PFNGLBITMAPXOESPROC)IntGetProcAddress("glBitmapxOES");
	if(!_funcptr_glBitmapxOES) ++numFailed;
	_funcptr_glBlendColorxOES = (PFNGLBLENDCOLORXOESPROC)IntGetProcAddress("glBlendColorxOES");
	if(!_funcptr_glBlendColorxOES) ++numFailed;
	_funcptr_glClearAccumxOES = (PFNGLCLEARACCUMXOESPROC)IntGetProcAddress("glClearAccumxOES");
	if(!_funcptr_glClearAccumxOES) ++numFailed;
	_funcptr_glClearColorxOES = (PFNGLCLEARCOLORXOESPROC)IntGetProcAddress("glClearColorxOES");
	if(!_funcptr_glClearColorxOES) ++numFailed;
	_funcptr_glClearDepthxOES = (PFNGLCLEARDEPTHXOESPROC)IntGetProcAddress("glClearDepthxOES");
	if(!_funcptr_glClearDepthxOES) ++numFailed;
	_funcptr_glClipPlanexOES = (PFNGLCLIPPLANEXOESPROC)IntGetProcAddress("glClipPlanexOES");
	if(!_funcptr_glClipPlanexOES) ++numFailed;
	_funcptr_glColor3xOES = (PFNGLCOLOR3XOESPROC)IntGetProcAddress("glColor3xOES");
	if(!_funcptr_glColor3xOES) ++numFailed;
	_funcptr_glColor4xOES = (PFNGLCOLOR4XOESPROC)IntGetProcAddress("glColor4xOES");
	if(!_funcptr_glColor4xOES) ++numFailed;
	_funcptr_glColor3xvOES = (PFNGLCOLOR3XVOESPROC)IntGetProcAddress("glColor3xvOES");
	if(!_funcptr_glColor3xvOES) ++numFailed;
	_funcptr_glColor4xvOES = (PFNGLCOLOR4XVOESPROC)IntGetProcAddress("glColor4xvOES");
	if(!_funcptr_glColor4xvOES) ++numFailed;
	_funcptr_glConvolutionParameterxOES = (PFNGLCONVOLUTIONPARAMETERXOESPROC)IntGetProcAddress("glConvolutionParameterxOES");
	if(!_funcptr_glConvolutionParameterxOES) ++numFailed;
	_funcptr_glConvolutionParameterxvOES = (PFNGLCONVOLUTIONPARAMETERXVOESPROC)IntGetProcAddress("glConvolutionParameterxvOES");
	if(!_funcptr_glConvolutionParameterxvOES) ++numFailed;
	_funcptr_glDepthRangexOES = (PFNGLDEPTHRANGEXOESPROC)IntGetProcAddress("glDepthRangexOES");
	if(!_funcptr_glDepthRangexOES) ++numFailed;
	_funcptr_glEvalCoord1xOES = (PFNGLEVALCOORD1XOESPROC)IntGetProcAddress("glEvalCoord1xOES");
	if(!_funcptr_glEvalCoord1xOES) ++numFailed;
	_funcptr_glEvalCoord2xOES = (PFNGLEVALCOORD2XOESPROC)IntGetProcAddress("glEvalCoord2xOES");
	if(!_funcptr_glEvalCoord2xOES) ++numFailed;
	_funcptr_glEvalCoord1xvOES = (PFNGLEVALCOORD1XVOESPROC)IntGetProcAddress("glEvalCoord1xvOES");
	if(!_funcptr_glEvalCoord1xvOES) ++numFailed;
	_funcptr_glEvalCoord2xvOES = (PFNGLEVALCOORD2XVOESPROC)IntGetProcAddress("glEvalCoord2xvOES");
	if(!_funcptr_glEvalCoord2xvOES) ++numFailed;
	_funcptr_glFeedbackBufferxOES = (PFNGLFEEDBACKBUFFERXOESPROC)IntGetProcAddress("glFeedbackBufferxOES");
	if(!_funcptr_glFeedbackBufferxOES) ++numFailed;
	_funcptr_glFogxOES = (PFNGLFOGXOESPROC)IntGetProcAddress("glFogxOES");
	if(!_funcptr_glFogxOES) ++numFailed;
	_funcptr_glFogxvOES = (PFNGLFOGXVOESPROC)IntGetProcAddress("glFogxvOES");
	if(!_funcptr_glFogxvOES) ++numFailed;
	_funcptr_glFrustumxOES = (PFNGLFRUSTUMXOESPROC)IntGetProcAddress("glFrustumxOES");
	if(!_funcptr_glFrustumxOES) ++numFailed;
	_funcptr_glGetClipPlanexOES = (PFNGLGETCLIPPLANEXOESPROC)IntGetProcAddress("glGetClipPlanexOES");
	if(!_funcptr_glGetClipPlanexOES) ++numFailed;
	_funcptr_glGetConvolutionParameterxvOES = (PFNGLGETCONVOLUTIONPARAMETERXVOESPROC)IntGetProcAddress("glGetConvolutionParameterxvOES");
	if(!_funcptr_glGetConvolutionParameterxvOES) ++numFailed;
	_funcptr_glGetFixedvOES = (PFNGLGETFIXEDVOESPROC)IntGetProcAddress("glGetFixedvOES");
	if(!_funcptr_glGetFixedvOES) ++numFailed;
	_funcptr_glGetHistogramParameterxvOES = (PFNGLGETHISTOGRAMPARAMETERXVOESPROC)IntGetProcAddress("glGetHistogramParameterxvOES");
	if(!_funcptr_glGetHistogramParameterxvOES) ++numFailed;
	_funcptr_glGetLightxOES = (PFNGLGETLIGHTXOESPROC)IntGetProcAddress("glGetLightxOES");
	if(!_funcptr_glGetLightxOES) ++numFailed;
	_funcptr_glGetMapxvOES = (PFNGLGETMAPXVOESPROC)IntGetProcAddress("glGetMapxvOES");
	if(!_funcptr_glGetMapxvOES) ++numFailed;
	_funcptr_glGetMaterialxOES = (PFNGLGETMATERIALXOESPROC)IntGetProcAddress("glGetMaterialxOES");
	if(!_funcptr_glGetMaterialxOES) ++numFailed;
	_funcptr_glGetPixelMapxv = (PFNGLGETPIXELMAPXVPROC)IntGetProcAddress("glGetPixelMapxv");
	if(!_funcptr_glGetPixelMapxv) ++numFailed;
	_funcptr_glGetTexEnvxvOES = (PFNGLGETTEXENVXVOESPROC)IntGetProcAddress("glGetTexEnvxvOES");
	if(!_funcptr_glGetTexEnvxvOES) ++numFailed;
	_funcptr_glGetTexGenxvOES = (PFNGLGETTEXGENXVOESPROC)IntGetProcAddress("glGetTexGenxvOES");
	if(!_funcptr_glGetTexGenxvOES) ++numFailed;
	_funcptr_glGetTexLevelParameterxvOES = (PFNGLGETTEXLEVELPARAMETERXVOESPROC)IntGetProcAddress("glGetTexLevelParameterxvOES");
	if(!_funcptr_glGetTexLevelParameterxvOES) ++numFailed;
	_funcptr_glGetTexParameterxvOES = (PFNGLGETTEXPARAMETERXVOESPROC)IntGetProcAddress("glGetTexParameterxvOES");
	if(!_funcptr_glGetTexParameterxvOES) ++numFailed;
	_funcptr_glIndexxOES = (PFNGLINDEXXOESPROC)IntGetProcAddress("glIndexxOES");
	if(!_funcptr_glIndexxOES) ++numFailed;
	_funcptr_glIndexxvOES = (PFNGLINDEXXVOESPROC)IntGetProcAddress("glIndexxvOES");
	if(!_funcptr_glIndexxvOES) ++numFailed;
	_funcptr_glLightModelxOES = (PFNGLLIGHTMODELXOESPROC)IntGetProcAddress("glLightModelxOES");
	if(!_funcptr_glLightModelxOES) ++numFailed;
	_funcptr_glLightModelxvOES = (PFNGLLIGHTMODELXVOESPROC)IntGetProcAddress("glLightModelxvOES");
	if(!_funcptr_glLightModelxvOES) ++numFailed;
	_funcptr_glLightxOES = (PFNGLLIGHTXOESPROC)IntGetProcAddress("glLightxOES");
	if(!_funcptr_glLightxOES) ++numFailed;
	_funcptr_glLightxvOES = (PFNGLLIGHTXVOESPROC)IntGetProcAddress("glLightxvOES");
	if(!_funcptr_glLightxvOES) ++numFailed;
	_funcptr_glLineWidthxOES = (PFNGLLINEWIDTHXOESPROC)IntGetProcAddress("glLineWidthxOES");
	if(!_funcptr_glLineWidthxOES) ++numFailed;
	_funcptr_glLoadMatrixxOES = (PFNGLLOADMATRIXXOESPROC)IntGetProcAddress("glLoadMatrixxOES");
	if(!_funcptr_glLoadMatrixxOES) ++numFailed;
	_funcptr_glLoadTransposeMatrixxOES = (PFNGLLOADTRANSPOSEMATRIXXOESPROC)IntGetProcAddress("glLoadTransposeMatrixxOES");
	if(!_funcptr_glLoadTransposeMatrixxOES) ++numFailed;
	_funcptr_glMap1xOES = (PFNGLMAP1XOESPROC)IntGetProcAddress("glMap1xOES");
	if(!_funcptr_glMap1xOES) ++numFailed;
	_funcptr_glMap2xOES = (PFNGLMAP2XOESPROC)IntGetProcAddress("glMap2xOES");
	if(!_funcptr_glMap2xOES) ++numFailed;
	_funcptr_glMapGrid1xOES = (PFNGLMAPGRID1XOESPROC)IntGetProcAddress("glMapGrid1xOES");
	if(!_funcptr_glMapGrid1xOES) ++numFailed;
	_funcptr_glMapGrid2xOES = (PFNGLMAPGRID2XOESPROC)IntGetProcAddress("glMapGrid2xOES");
	if(!_funcptr_glMapGrid2xOES) ++numFailed;
	_funcptr_glMaterialxOES = (PFNGLMATERIALXOESPROC)IntGetProcAddress("glMaterialxOES");
	if(!_funcptr_glMaterialxOES) ++numFailed;
	_funcptr_glMaterialxvOES = (PFNGLMATERIALXVOESPROC)IntGetProcAddress("glMaterialxvOES");
	if(!_funcptr_glMaterialxvOES) ++numFailed;
	_funcptr_glMultMatrixxOES = (PFNGLMULTMATRIXXOESPROC)IntGetProcAddress("glMultMatrixxOES");
	if(!_funcptr_glMultMatrixxOES) ++numFailed;
	_funcptr_glMultTransposeMatrixxOES = (PFNGLMULTTRANSPOSEMATRIXXOESPROC)IntGetProcAddress("glMultTransposeMatrixxOES");
	if(!_funcptr_glMultTransposeMatrixxOES) ++numFailed;
	_funcptr_glMultiTexCoord1xOES = (PFNGLMULTITEXCOORD1XOESPROC)IntGetProcAddress("glMultiTexCoord1xOES");
	if(!_funcptr_glMultiTexCoord1xOES) ++numFailed;
	_funcptr_glMultiTexCoord2xOES = (PFNGLMULTITEXCOORD2XOESPROC)IntGetProcAddress("glMultiTexCoord2xOES");
	if(!_funcptr_glMultiTexCoord2xOES) ++numFailed;
	_funcptr_glMultiTexCoord3xOES = (PFNGLMULTITEXCOORD3XOESPROC)IntGetProcAddress("glMultiTexCoord3xOES");
	if(!_funcptr_glMultiTexCoord3xOES) ++numFailed;
	_funcptr_glMultiTexCoord4xOES = (PFNGLMULTITEXCOORD4XOESPROC)IntGetProcAddress("glMultiTexCoord4xOES");
	if(!_funcptr_glMultiTexCoord4xOES) ++numFailed;
	_funcptr_glMultiTexCoord1xvOES = (PFNGLMULTITEXCOORD1XVOESPROC)IntGetProcAddress("glMultiTexCoord1xvOES");
	if(!_funcptr_glMultiTexCoord1xvOES) ++numFailed;
	_funcptr_glMultiTexCoord2xvOES = (PFNGLMULTITEXCOORD2XVOESPROC)IntGetProcAddress("glMultiTexCoord2xvOES");
	if(!_funcptr_glMultiTexCoord2xvOES) ++numFailed;
	_funcptr_glMultiTexCoord3xvOES = (PFNGLMULTITEXCOORD3XVOESPROC)IntGetProcAddress("glMultiTexCoord3xvOES");
	if(!_funcptr_glMultiTexCoord3xvOES) ++numFailed;
	_funcptr_glMultiTexCoord4xvOES = (PFNGLMULTITEXCOORD4XVOESPROC)IntGetProcAddress("glMultiTexCoord4xvOES");
	if(!_funcptr_glMultiTexCoord4xvOES) ++numFailed;
	_funcptr_glNormal3xOES = (PFNGLNORMAL3XOESPROC)IntGetProcAddress("glNormal3xOES");
	if(!_funcptr_glNormal3xOES) ++numFailed;
	_funcptr_glNormal3xvOES = (PFNGLNORMAL3XVOESPROC)IntGetProcAddress("glNormal3xvOES");
	if(!_funcptr_glNormal3xvOES) ++numFailed;
	_funcptr_glOrthoxOES = (PFNGLORTHOXOESPROC)IntGetProcAddress("glOrthoxOES");
	if(!_funcptr_glOrthoxOES) ++numFailed;
	_funcptr_glPassThroughxOES = (PFNGLPASSTHROUGHXOESPROC)IntGetProcAddress("glPassThroughxOES");
	if(!_funcptr_glPassThroughxOES) ++numFailed;
	_funcptr_glPixelMapx = (PFNGLPIXELMAPXPROC)IntGetProcAddress("glPixelMapx");
	if(!_funcptr_glPixelMapx) ++numFailed;
	_funcptr_glPixelStorex = (PFNGLPIXELSTOREXPROC)IntGetProcAddress("glPixelStorex");
	if(!_funcptr_glPixelStorex) ++numFailed;
	_funcptr_glPixelTransferxOES = (PFNGLPIXELTRANSFERXOESPROC)IntGetProcAddress("glPixelTransferxOES");
	if(!_funcptr_glPixelTransferxOES) ++numFailed;
	_funcptr_glPixelZoomxOES = (PFNGLPIXELZOOMXOESPROC)IntGetProcAddress("glPixelZoomxOES");
	if(!_funcptr_glPixelZoomxOES) ++numFailed;
	_funcptr_glPointParameterxvOES = (PFNGLPOINTPARAMETERXVOESPROC)IntGetProcAddress("glPointParameterxvOES");
	if(!_funcptr_glPointParameterxvOES) ++numFailed;
	_funcptr_glPointSizexOES = (PFNGLPOINTSIZEXOESPROC)IntGetProcAddress("glPointSizexOES");
	if(!_funcptr_glPointSizexOES) ++numFailed;
	_funcptr_glPolygonOffsetxOES = (PFNGLPOLYGONOFFSETXOESPROC)IntGetProcAddress("glPolygonOffsetxOES");
	if(!_funcptr_glPolygonOffsetxOES) ++numFailed;
	_funcptr_glPrioritizeTexturesxOES = (PFNGLPRIORITIZETEXTURESXOESPROC)IntGetProcAddress("glPrioritizeTexturesxOES");
	if(!_funcptr_glPrioritizeTexturesxOES) ++numFailed;
	_funcptr_glRasterPos2xOES = (PFNGLRASTERPOS2XOESPROC)IntGetProcAddress("glRasterPos2xOES");
	if(!_funcptr_glRasterPos2xOES) ++numFailed;
	_funcptr_glRasterPos3xOES = (PFNGLRASTERPOS3XOESPROC)IntGetProcAddress("glRasterPos3xOES");
	if(!_funcptr_glRasterPos3xOES) ++numFailed;
	_funcptr_glRasterPos4xOES = (PFNGLRASTERPOS4XOESPROC)IntGetProcAddress("glRasterPos4xOES");
	if(!_funcptr_glRasterPos4xOES) ++numFailed;
	_funcptr_glRasterPos2xvOES = (PFNGLRASTERPOS2XVOESPROC)IntGetProcAddress("glRasterPos2xvOES");
	if(!_funcptr_glRasterPos2xvOES) ++numFailed;
	_funcptr_glRasterPos3xvOES = (PFNGLRASTERPOS3XVOESPROC)IntGetProcAddress("glRasterPos3xvOES");
	if(!_funcptr_glRasterPos3xvOES) ++numFailed;
	_funcptr_glRasterPos4xvOES = (PFNGLRASTERPOS4XVOESPROC)IntGetProcAddress("glRasterPos4xvOES");
	if(!_funcptr_glRasterPos4xvOES) ++numFailed;
	_funcptr_glRectxOES = (PFNGLRECTXOESPROC)IntGetProcAddress("glRectxOES");
	if(!_funcptr_glRectxOES) ++numFailed;
	_funcptr_glRectxvOES = (PFNGLRECTXVOESPROC)IntGetProcAddress("glRectxvOES");
	if(!_funcptr_glRectxvOES) ++numFailed;
	_funcptr_glRotatexOES = (PFNGLROTATEXOESPROC)IntGetProcAddress("glRotatexOES");
	if(!_funcptr_glRotatexOES) ++numFailed;
	_funcptr_glSampleCoverageOES = (PFNGLSAMPLECOVERAGEOESPROC)IntGetProcAddress("glSampleCoverageOES");
	if(!_funcptr_glSampleCoverageOES) ++numFailed;
	_funcptr_glScalexOES = (PFNGLSCALEXOESPROC)IntGetProcAddress("glScalexOES");
	if(!_funcptr_glScalexOES) ++numFailed;
	_funcptr_glTexCoord1xOES = (PFNGLTEXCOORD1XOESPROC)IntGetProcAddress("glTexCoord1xOES");
	if(!_funcptr_glTexCoord1xOES) ++numFailed;
	_funcptr_glTexCoord2xOES = (PFNGLTEXCOORD2XOESPROC)IntGetProcAddress("glTexCoord2xOES");
	if(!_funcptr_glTexCoord2xOES) ++numFailed;
	_funcptr_glTexCoord3xOES = (PFNGLTEXCOORD3XOESPROC)IntGetProcAddress("glTexCoord3xOES");
	if(!_funcptr_glTexCoord3xOES) ++numFailed;
	_funcptr_glTexCoord4xOES = (PFNGLTEXCOORD4XOESPROC)IntGetProcAddress("glTexCoord4xOES");
	if(!_funcptr_glTexCoord4xOES) ++numFailed;
	_funcptr_glTexCoord1xvOES = (PFNGLTEXCOORD1XVOESPROC)IntGetProcAddress("glTexCoord1xvOES");
	if(!_funcptr_glTexCoord1xvOES) ++numFailed;
	_funcptr_glTexCoord2xvOES = (PFNGLTEXCOORD2XVOESPROC)IntGetProcAddress("glTexCoord2xvOES");
	if(!_funcptr_glTexCoord2xvOES) ++numFailed;
	_funcptr_glTexCoord3xvOES = (PFNGLTEXCOORD3XVOESPROC)IntGetProcAddress("glTexCoord3xvOES");
	if(!_funcptr_glTexCoord3xvOES) ++numFailed;
	_funcptr_glTexCoord4xvOES = (PFNGLTEXCOORD4XVOESPROC)IntGetProcAddress("glTexCoord4xvOES");
	if(!_funcptr_glTexCoord4xvOES) ++numFailed;
	_funcptr_glTexEnvxOES = (PFNGLTEXENVXOESPROC)IntGetProcAddress("glTexEnvxOES");
	if(!_funcptr_glTexEnvxOES) ++numFailed;
	_funcptr_glTexEnvxvOES = (PFNGLTEXENVXVOESPROC)IntGetProcAddress("glTexEnvxvOES");
	if(!_funcptr_glTexEnvxvOES) ++numFailed;
	_funcptr_glTexGenxOES = (PFNGLTEXGENXOESPROC)IntGetProcAddress("glTexGenxOES");
	if(!_funcptr_glTexGenxOES) ++numFailed;
	_funcptr_glTexGenxvOES = (PFNGLTEXGENXVOESPROC)IntGetProcAddress("glTexGenxvOES");
	if(!_funcptr_glTexGenxvOES) ++numFailed;
	_funcptr_glTexParameterxOES = (PFNGLTEXPARAMETERXOESPROC)IntGetProcAddress("glTexParameterxOES");
	if(!_funcptr_glTexParameterxOES) ++numFailed;
	_funcptr_glTexParameterxvOES = (PFNGLTEXPARAMETERXVOESPROC)IntGetProcAddress("glTexParameterxvOES");
	if(!_funcptr_glTexParameterxvOES) ++numFailed;
	_funcptr_glTranslatexOES = (PFNGLTRANSLATEXOESPROC)IntGetProcAddress("glTranslatexOES");
	if(!_funcptr_glTranslatexOES) ++numFailed;
	_funcptr_glVertex2xOES = (PFNGLVERTEX2XOESPROC)IntGetProcAddress("glVertex2xOES");
	if(!_funcptr_glVertex2xOES) ++numFailed;
	_funcptr_glVertex3xOES = (PFNGLVERTEX3XOESPROC)IntGetProcAddress("glVertex3xOES");
	if(!_funcptr_glVertex3xOES) ++numFailed;
	_funcptr_glVertex4xOES = (PFNGLVERTEX4XOESPROC)IntGetProcAddress("glVertex4xOES");
	if(!_funcptr_glVertex4xOES) ++numFailed;
	_funcptr_glVertex2xvOES = (PFNGLVERTEX2XVOESPROC)IntGetProcAddress("glVertex2xvOES");
	if(!_funcptr_glVertex2xvOES) ++numFailed;
	_funcptr_glVertex3xvOES = (PFNGLVERTEX3XVOESPROC)IntGetProcAddress("glVertex3xvOES");
	if(!_funcptr_glVertex3xvOES) ++numFailed;
	_funcptr_glVertex4xvOES = (PFNGLVERTEX4XVOESPROC)IntGetProcAddress("glVertex4xvOES");
	if(!_funcptr_glVertex4xvOES) ++numFailed;
	return numFailed;
}

PFNGLQUERYMATRIXXOESPROC _funcptr_glQueryMatrixxOES = NULL;

static int LoadExt_OES_query_matrix()
{
	int numFailed = 0;
	_funcptr_glQueryMatrixxOES = (PFNGLQUERYMATRIXXOESPROC)IntGetProcAddress("glQueryMatrixxOES");
	if(!_funcptr_glQueryMatrixxOES) ++numFailed;
	return numFailed;
}

PFNGLDEPTHRANGEFOESPROC _funcptr_glDepthRangefOES = NULL;
PFNGLFRUSTUMFOESPROC _funcptr_glFrustumfOES = NULL;
PFNGLORTHOFOESPROC _funcptr_glOrthofOES = NULL;
PFNGLCLIPPLANEFOESPROC _funcptr_glClipPlanefOES = NULL;
PFNGLCLEARDEPTHFOESPROC _funcptr_glClearDepthfOES = NULL;
PFNGLGETCLIPPLANEFOESPROC _funcptr_glGetClipPlanefOES = NULL;

static int LoadExt_OES_single_precision()
{
	int numFailed = 0;
	_funcptr_glDepthRangefOES = (PFNGLDEPTHRANGEFOESPROC)IntGetProcAddress("glDepthRangefOES");
	if(!_funcptr_glDepthRangefOES) ++numFailed;
	_funcptr_glFrustumfOES = (PFNGLFRUSTUMFOESPROC)IntGetProcAddress("glFrustumfOES");
	if(!_funcptr_glFrustumfOES) ++numFailed;
	_funcptr_glOrthofOES = (PFNGLORTHOFOESPROC)IntGetProcAddress("glOrthofOES");
	if(!_funcptr_glOrthofOES) ++numFailed;
	_funcptr_glClipPlanefOES = (PFNGLCLIPPLANEFOESPROC)IntGetProcAddress("glClipPlanefOES");
	if(!_funcptr_glClipPlanefOES) ++numFailed;
	_funcptr_glClearDepthfOES = (PFNGLCLEARDEPTHFOESPROC)IntGetProcAddress("glClearDepthfOES");
	if(!_funcptr_glClearDepthfOES) ++numFailed;
	_funcptr_glGetClipPlanefOES = (PFNGLGETCLIPPLANEFOESPROC)IntGetProcAddress("glGetClipPlanefOES");
	if(!_funcptr_glGetClipPlanefOES) ++numFailed;
	return numFailed;
}

PFNGLHINTPGIPROC _funcptr_glHintPGI = NULL;

static int LoadExt_PGI_misc_hints()
{
	int numFailed = 0;
	_funcptr_glHintPGI = (PFNGLHINTPGIPROC)IntGetProcAddress("glHintPGI");
	if(!_funcptr_glHintPGI) ++numFailed;
	return numFailed;
}

PFNGLDETAILTEXFUNCSGISPROC _funcptr_glDetailTexFuncSGIS = NULL;
PFNGLGETDETAILTEXFUNCSGISPROC _funcptr_glGetDetailTexFuncSGIS = NULL;

static int LoadExt_SGIS_detail_texture()
{
	int numFailed = 0;
	_funcptr_glDetailTexFuncSGIS = (PFNGLDETAILTEXFUNCSGISPROC)IntGetProcAddress("glDetailTexFuncSGIS");
	if(!_funcptr_glDetailTexFuncSGIS) ++numFailed;
	_funcptr_glGetDetailTexFuncSGIS = (PFNGLGETDETAILTEXFUNCSGISPROC)IntGetProcAddress("glGetDetailTexFuncSGIS");
	if(!_funcptr_glGetDetailTexFuncSGIS) ++numFailed;
	return numFailed;
}

PFNGLFOGFUNCSGISPROC _funcptr_glFogFuncSGIS = NULL;
PFNGLGETFOGFUNCSGISPROC _funcptr_glGetFogFuncSGIS = NULL;

static int LoadExt_SGIS_fog_function()
{
	int numFailed = 0;
	_funcptr_glFogFuncSGIS = (PFNGLFOGFUNCSGISPROC)IntGetProcAddress("glFogFuncSGIS");
	if(!_funcptr_glFogFuncSGIS) ++numFailed;
	_funcptr_glGetFogFuncSGIS = (PFNGLGETFOGFUNCSGISPROC)IntGetProcAddress("glGetFogFuncSGIS");
	if(!_funcptr_glGetFogFuncSGIS) ++numFailed;
	return numFailed;
}

PFNGLSAMPLEMASKSGISPROC _funcptr_glSampleMaskSGIS = NULL;
PFNGLSAMPLEPATTERNSGISPROC _funcptr_glSamplePatternSGIS = NULL;

static int LoadExt_SGIS_multisample()
{
	int numFailed = 0;
	_funcptr_glSampleMaskSGIS = (PFNGLSAMPLEMASKSGISPROC)IntGetProcAddress("glSampleMaskSGIS");
	if(!_funcptr_glSampleMaskSGIS) ++numFailed;
	_funcptr_glSamplePatternSGIS = (PFNGLSAMPLEPATTERNSGISPROC)IntGetProcAddress("glSamplePatternSGIS");
	if(!_funcptr_glSamplePatternSGIS) ++numFailed;
	return numFailed;
}

PFNGLPIXELTEXGENPARAMETERISGISPROC _funcptr_glPixelTexGenParameteriSGIS = NULL;
PFNGLPIXELTEXGENPARAMETERIVSGISPROC _funcptr_glPixelTexGenParameterivSGIS = NULL;
PFNGLPIXELTEXGENPARAMETERFSGISPROC _funcptr_glPixelTexGenParameterfSGIS = NULL;
PFNGLPIXELTEXGENPARAMETERFVSGISPROC _funcptr_glPixelTexGenParameterfvSGIS = NULL;
PFNGLGETPIXELTEXGENPARAMETERIVSGISPROC _funcptr_glGetPixelTexGenParameterivSGIS = NULL;
PFNGLGETPIXELTEXGENPARAMETERFVSGISPROC _funcptr_glGetPixelTexGenParameterfvSGIS = NULL;

static int LoadExt_SGIS_pixel_texture()
{
	int numFailed = 0;
	_funcptr_glPixelTexGenParameteriSGIS = (PFNGLPIXELTEXGENPARAMETERISGISPROC)IntGetProcAddress("glPixelTexGenParameteriSGIS");
	if(!_funcptr_glPixelTexGenParameteriSGIS) ++numFailed;
	_funcptr_glPixelTexGenParameterivSGIS = (PFNGLPIXELTEXGENPARAMETERIVSGISPROC)IntGetProcAddress("glPixelTexGenParameterivSGIS");
	if(!_funcptr_glPixelTexGenParameterivSGIS) ++numFailed;
	_funcptr_glPixelTexGenParameterfSGIS = (PFNGLPIXELTEXGENPARAMETERFSGISPROC)IntGetProcAddress("glPixelTexGenParameterfSGIS");
	if(!_funcptr_glPixelTexGenParameterfSGIS) ++numFailed;
	_funcptr_glPixelTexGenParameterfvSGIS = (PFNGLPIXELTEXGENPARAMETERFVSGISPROC)IntGetProcAddress("glPixelTexGenParameterfvSGIS");
	if(!_funcptr_glPixelTexGenParameterfvSGIS) ++numFailed;
	_funcptr_glGetPixelTexGenParameterivSGIS = (PFNGLGETPIXELTEXGENPARAMETERIVSGISPROC)IntGetProcAddress("glGetPixelTexGenParameterivSGIS");
	if(!_funcptr_glGetPixelTexGenParameterivSGIS) ++numFailed;
	_funcptr_glGetPixelTexGenParameterfvSGIS = (PFNGLGETPIXELTEXGENPARAMETERFVSGISPROC)IntGetProcAddress("glGetPixelTexGenParameterfvSGIS");
	if(!_funcptr_glGetPixelTexGenParameterfvSGIS) ++numFailed;
	return numFailed;
}

PFNGLPOINTPARAMETERFSGISPROC _funcptr_glPointParameterfSGIS = NULL;
PFNGLPOINTPARAMETERFVSGISPROC _funcptr_glPointParameterfvSGIS = NULL;

static int LoadExt_SGIS_point_parameters()
{
	int numFailed = 0;
	_funcptr_glPointParameterfSGIS = (PFNGLPOINTPARAMETERFSGISPROC)IntGetProcAddress("glPointParameterfSGIS");
	if(!_funcptr_glPointParameterfSGIS) ++numFailed;
	_funcptr_glPointParameterfvSGIS = (PFNGLPOINTPARAMETERFVSGISPROC)IntGetProcAddress("glPointParameterfvSGIS");
	if(!_funcptr_glPointParameterfvSGIS) ++numFailed;
	return numFailed;
}

PFNGLSHARPENTEXFUNCSGISPROC _funcptr_glSharpenTexFuncSGIS = NULL;
PFNGLGETSHARPENTEXFUNCSGISPROC _funcptr_glGetSharpenTexFuncSGIS = NULL;

static int LoadExt_SGIS_sharpen_texture()
{
	int numFailed = 0;
	_funcptr_glSharpenTexFuncSGIS = (PFNGLSHARPENTEXFUNCSGISPROC)IntGetProcAddress("glSharpenTexFuncSGIS");
	if(!_funcptr_glSharpenTexFuncSGIS) ++numFailed;
	_funcptr_glGetSharpenTexFuncSGIS = (PFNGLGETSHARPENTEXFUNCSGISPROC)IntGetProcAddress("glGetSharpenTexFuncSGIS");
	if(!_funcptr_glGetSharpenTexFuncSGIS) ++numFailed;
	return numFailed;
}

PFNGLTEXIMAGE4DSGISPROC _funcptr_glTexImage4DSGIS = NULL;
PFNGLTEXSUBIMAGE4DSGISPROC _funcptr_glTexSubImage4DSGIS = NULL;

static int LoadExt_SGIS_texture4D()
{
	int numFailed = 0;
	_funcptr_glTexImage4DSGIS = (PFNGLTEXIMAGE4DSGISPROC)IntGetProcAddress("glTexImage4DSGIS");
	if(!_funcptr_glTexImage4DSGIS) ++numFailed;
	_funcptr_glTexSubImage4DSGIS = (PFNGLTEXSUBIMAGE4DSGISPROC)IntGetProcAddress("glTexSubImage4DSGIS");
	if(!_funcptr_glTexSubImage4DSGIS) ++numFailed;
	return numFailed;
}

PFNGLTEXTURECOLORMASKSGISPROC _funcptr_glTextureColorMaskSGIS = NULL;

static int LoadExt_SGIS_texture_color_mask()
{
	int numFailed = 0;
	_funcptr_glTextureColorMaskSGIS = (PFNGLTEXTURECOLORMASKSGISPROC)IntGetProcAddress("glTextureColorMaskSGIS");
	if(!_funcptr_glTextureColorMaskSGIS) ++numFailed;
	return numFailed;
}

PFNGLGETTEXFILTERFUNCSGISPROC _funcptr_glGetTexFilterFuncSGIS = NULL;
PFNGLTEXFILTERFUNCSGISPROC _funcptr_glTexFilterFuncSGIS = NULL;

static int LoadExt_SGIS_texture_filter4()
{
	int numFailed = 0;
	_funcptr_glGetTexFilterFuncSGIS = (PFNGLGETTEXFILTERFUNCSGISPROC)IntGetProcAddress("glGetTexFilterFuncSGIS");
	if(!_funcptr_glGetTexFilterFuncSGIS) ++numFailed;
	_funcptr_glTexFilterFuncSGIS = (PFNGLTEXFILTERFUNCSGISPROC)IntGetProcAddress("glTexFilterFuncSGIS");
	if(!_funcptr_glTexFilterFuncSGIS) ++numFailed;
	return numFailed;
}

PFNGLASYNCMARKERSGIXPROC _funcptr_glAsyncMarkerSGIX = NULL;
PFNGLFINISHASYNCSGIXPROC _funcptr_glFinishAsyncSGIX = NULL;
PFNGLPOLLASYNCSGIXPROC _funcptr_glPollAsyncSGIX = NULL;
PFNGLGENASYNCMARKERSSGIXPROC _funcptr_glGenAsyncMarkersSGIX = NULL;
PFNGLDELETEASYNCMARKERSSGIXPROC _funcptr_glDeleteAsyncMarkersSGIX = NULL;
PFNGLISASYNCMARKERSGIXPROC _funcptr_glIsAsyncMarkerSGIX = NULL;

static int LoadExt_SGIX_async()
{
	int numFailed = 0;
	_funcptr_glAsyncMarkerSGIX = (PFNGLASYNCMARKERSGIXPROC)IntGetProcAddress("glAsyncMarkerSGIX");
	if(!_funcptr_glAsyncMarkerSGIX) ++numFailed;
	_funcptr_glFinishAsyncSGIX = (PFNGLFINISHASYNCSGIXPROC)IntGetProcAddress("glFinishAsyncSGIX");
	if(!_funcptr_glFinishAsyncSGIX) ++numFailed;
	_funcptr_glPollAsyncSGIX = (PFNGLPOLLASYNCSGIXPROC)IntGetProcAddress("glPollAsyncSGIX");
	if(!_funcptr_glPollAsyncSGIX) ++numFailed;
	_funcptr_glGenAsyncMarkersSGIX = (PFNGLGENASYNCMARKERSSGIXPROC)IntGetProcAddress("glGenAsyncMarkersSGIX");
	if(!_funcptr_glGenAsyncMarkersSGIX) ++numFailed;
	_funcptr_glDeleteAsyncMarkersSGIX = (PFNGLDELETEASYNCMARKERSSGIXPROC)IntGetProcAddress("glDeleteAsyncMarkersSGIX");
	if(!_funcptr_glDeleteAsyncMarkersSGIX) ++numFailed;
	_funcptr_glIsAsyncMarkerSGIX = (PFNGLISASYNCMARKERSGIXPROC)IntGetProcAddress("glIsAsyncMarkerSGIX");
	if(!_funcptr_glIsAsyncMarkerSGIX) ++numFailed;
	return numFailed;
}

PFNGLFLUSHRASTERSGIXPROC _funcptr_glFlushRasterSGIX = NULL;

static int LoadExt_SGIX_flush_raster()
{
	int numFailed = 0;
	_funcptr_glFlushRasterSGIX = (PFNGLFLUSHRASTERSGIXPROC)IntGetProcAddress("glFlushRasterSGIX");
	if(!_funcptr_glFlushRasterSGIX) ++numFailed;
	return numFailed;
}

PFNGLFRAGMENTCOLORMATERIALSGIXPROC _funcptr_glFragmentColorMaterialSGIX = NULL;
PFNGLFRAGMENTLIGHTFSGIXPROC _funcptr_glFragmentLightfSGIX = NULL;
PFNGLFRAGMENTLIGHTFVSGIXPROC _funcptr_glFragmentLightfvSGIX = NULL;
PFNGLFRAGMENTLIGHTISGIXPROC _funcptr_glFragmentLightiSGIX = NULL;
PFNGLFRAGMENTLIGHTIVSGIXPROC _funcptr_glFragmentLightivSGIX = NULL;
PFNGLFRAGMENTLIGHTMODELFSGIXPROC _funcptr_glFragmentLightModelfSGIX = NULL;
PFNGLFRAGMENTLIGHTMODELFVSGIXPROC _funcptr_glFragmentLightModelfvSGIX = NULL;
PFNGLFRAGMENTLIGHTMODELISGIXPROC _funcptr_glFragmentLightModeliSGIX = NULL;
PFNGLFRAGMENTLIGHTMODELIVSGIXPROC _funcptr_glFragmentLightModelivSGIX = NULL;
PFNGLFRAGMENTMATERIALFSGIXPROC _funcptr_glFragmentMaterialfSGIX = NULL;
PFNGLFRAGMENTMATERIALFVSGIXPROC _funcptr_glFragmentMaterialfvSGIX = NULL;
PFNGLFRAGMENTMATERIALISGIXPROC _funcptr_glFragmentMaterialiSGIX = NULL;
PFNGLFRAGMENTMATERIALIVSGIXPROC _funcptr_glFragmentMaterialivSGIX = NULL;
PFNGLGETFRAGMENTLIGHTFVSGIXPROC _funcptr_glGetFragmentLightfvSGIX = NULL;
PFNGLGETFRAGMENTLIGHTIVSGIXPROC _funcptr_glGetFragmentLightivSGIX = NULL;
PFNGLGETFRAGMENTMATERIALFVSGIXPROC _funcptr_glGetFragmentMaterialfvSGIX = NULL;
PFNGLGETFRAGMENTMATERIALIVSGIXPROC _funcptr_glGetFragmentMaterialivSGIX = NULL;
PFNGLLIGHTENVISGIXPROC _funcptr_glLightEnviSGIX = NULL;

static int LoadExt_SGIX_fragment_lighting()
{
	int numFailed = 0;
	_funcptr_glFragmentColorMaterialSGIX = (PFNGLFRAGMENTCOLORMATERIALSGIXPROC)IntGetProcAddress("glFragmentColorMaterialSGIX");
	if(!_funcptr_glFragmentColorMaterialSGIX) ++numFailed;
	_funcptr_glFragmentLightfSGIX = (PFNGLFRAGMENTLIGHTFSGIXPROC)IntGetProcAddress("glFragmentLightfSGIX");
	if(!_funcptr_glFragmentLightfSGIX) ++numFailed;
	_funcptr_glFragmentLightfvSGIX = (PFNGLFRAGMENTLIGHTFVSGIXPROC)IntGetProcAddress("glFragmentLightfvSGIX");
	if(!_funcptr_glFragmentLightfvSGIX) ++numFailed;
	_funcptr_glFragmentLightiSGIX = (PFNGLFRAGMENTLIGHTISGIXPROC)IntGetProcAddress("glFragmentLightiSGIX");
	if(!_funcptr_glFragmentLightiSGIX) ++numFailed;
	_funcptr_glFragmentLightivSGIX = (PFNGLFRAGMENTLIGHTIVSGIXPROC)IntGetProcAddress("glFragmentLightivSGIX");
	if(!_funcptr_glFragmentLightivSGIX) ++numFailed;
	_funcptr_glFragmentLightModelfSGIX = (PFNGLFRAGMENTLIGHTMODELFSGIXPROC)IntGetProcAddress("glFragmentLightModelfSGIX");
	if(!_funcptr_glFragmentLightModelfSGIX) ++numFailed;
	_funcptr_glFragmentLightModelfvSGIX = (PFNGLFRAGMENTLIGHTMODELFVSGIXPROC)IntGetProcAddress("glFragmentLightModelfvSGIX");
	if(!_funcptr_glFragmentLightModelfvSGIX) ++numFailed;
	_funcptr_glFragmentLightModeliSGIX = (PFNGLFRAGMENTLIGHTMODELISGIXPROC)IntGetProcAddress("glFragmentLightModeliSGIX");
	if(!_funcptr_glFragmentLightModeliSGIX) ++numFailed;
	_funcptr_glFragmentLightModelivSGIX = (PFNGLFRAGMENTLIGHTMODELIVSGIXPROC)IntGetProcAddress("glFragmentLightModelivSGIX");
	if(!_funcptr_glFragmentLightModelivSGIX) ++numFailed;
	_funcptr_glFragmentMaterialfSGIX = (PFNGLFRAGMENTMATERIALFSGIXPROC)IntGetProcAddress("glFragmentMaterialfSGIX");
	if(!_funcptr_glFragmentMaterialfSGIX) ++numFailed;
	_funcptr_glFragmentMaterialfvSGIX = (PFNGLFRAGMENTMATERIALFVSGIXPROC)IntGetProcAddress("glFragmentMaterialfvSGIX");
	if(!_funcptr_glFragmentMaterialfvSGIX) ++numFailed;
	_funcptr_glFragmentMaterialiSGIX = (PFNGLFRAGMENTMATERIALISGIXPROC)IntGetProcAddress("glFragmentMaterialiSGIX");
	if(!_funcptr_glFragmentMaterialiSGIX) ++numFailed;
	_funcptr_glFragmentMaterialivSGIX = (PFNGLFRAGMENTMATERIALIVSGIXPROC)IntGetProcAddress("glFragmentMaterialivSGIX");
	if(!_funcptr_glFragmentMaterialivSGIX) ++numFailed;
	_funcptr_glGetFragmentLightfvSGIX = (PFNGLGETFRAGMENTLIGHTFVSGIXPROC)IntGetProcAddress("glGetFragmentLightfvSGIX");
	if(!_funcptr_glGetFragmentLightfvSGIX) ++numFailed;
	_funcptr_glGetFragmentLightivSGIX = (PFNGLGETFRAGMENTLIGHTIVSGIXPROC)IntGetProcAddress("glGetFragmentLightivSGIX");
	if(!_funcptr_glGetFragmentLightivSGIX) ++numFailed;
	_funcptr_glGetFragmentMaterialfvSGIX = (PFNGLGETFRAGMENTMATERIALFVSGIXPROC)IntGetProcAddress("glGetFragmentMaterialfvSGIX");
	if(!_funcptr_glGetFragmentMaterialfvSGIX) ++numFailed;
	_funcptr_glGetFragmentMaterialivSGIX = (PFNGLGETFRAGMENTMATERIALIVSGIXPROC)IntGetProcAddress("glGetFragmentMaterialivSGIX");
	if(!_funcptr_glGetFragmentMaterialivSGIX) ++numFailed;
	_funcptr_glLightEnviSGIX = (PFNGLLIGHTENVISGIXPROC)IntGetProcAddress("glLightEnviSGIX");
	if(!_funcptr_glLightEnviSGIX) ++numFailed;
	return numFailed;
}

PFNGLFRAMEZOOMSGIXPROC _funcptr_glFrameZoomSGIX = NULL;

static int LoadExt_SGIX_framezoom()
{
	int numFailed = 0;
	_funcptr_glFrameZoomSGIX = (PFNGLFRAMEZOOMSGIXPROC)IntGetProcAddress("glFrameZoomSGIX");
	if(!_funcptr_glFrameZoomSGIX) ++numFailed;
	return numFailed;
}

PFNGLGETINSTRUMENTSSGIXPROC _funcptr_glGetInstrumentsSGIX = NULL;
PFNGLINSTRUMENTSBUFFERSGIXPROC _funcptr_glInstrumentsBufferSGIX = NULL;
PFNGLPOLLINSTRUMENTSSGIXPROC _funcptr_glPollInstrumentsSGIX = NULL;
PFNGLREADINSTRUMENTSSGIXPROC _funcptr_glReadInstrumentsSGIX = NULL;
PFNGLSTARTINSTRUMENTSSGIXPROC _funcptr_glStartInstrumentsSGIX = NULL;
PFNGLSTOPINSTRUMENTSSGIXPROC _funcptr_glStopInstrumentsSGIX = NULL;

static int LoadExt_SGIX_instruments()
{
	int numFailed = 0;
	_funcptr_glGetInstrumentsSGIX = (PFNGLGETINSTRUMENTSSGIXPROC)IntGetProcAddress("glGetInstrumentsSGIX");
	if(!_funcptr_glGetInstrumentsSGIX) ++numFailed;
	_funcptr_glInstrumentsBufferSGIX = (PFNGLINSTRUMENTSBUFFERSGIXPROC)IntGetProcAddress("glInstrumentsBufferSGIX");
	if(!_funcptr_glInstrumentsBufferSGIX) ++numFailed;
	_funcptr_glPollInstrumentsSGIX = (PFNGLPOLLINSTRUMENTSSGIXPROC)IntGetProcAddress("glPollInstrumentsSGIX");
	if(!_funcptr_glPollInstrumentsSGIX) ++numFailed;
	_funcptr_glReadInstrumentsSGIX = (PFNGLREADINSTRUMENTSSGIXPROC)IntGetProcAddress("glReadInstrumentsSGIX");
	if(!_funcptr_glReadInstrumentsSGIX) ++numFailed;
	_funcptr_glStartInstrumentsSGIX = (PFNGLSTARTINSTRUMENTSSGIXPROC)IntGetProcAddress("glStartInstrumentsSGIX");
	if(!_funcptr_glStartInstrumentsSGIX) ++numFailed;
	_funcptr_glStopInstrumentsSGIX = (PFNGLSTOPINSTRUMENTSSGIXPROC)IntGetProcAddress("glStopInstrumentsSGIX");
	if(!_funcptr_glStopInstrumentsSGIX) ++numFailed;
	return numFailed;
}

PFNGLGETLISTPARAMETERFVSGIXPROC _funcptr_glGetListParameterfvSGIX = NULL;
PFNGLGETLISTPARAMETERIVSGIXPROC _funcptr_glGetListParameterivSGIX = NULL;
PFNGLLISTPARAMETERFSGIXPROC _funcptr_glListParameterfSGIX = NULL;
PFNGLLISTPARAMETERFVSGIXPROC _funcptr_glListParameterfvSGIX = NULL;
PFNGLLISTPARAMETERISGIXPROC _funcptr_glListParameteriSGIX = NULL;
PFNGLLISTPARAMETERIVSGIXPROC _funcptr_glListParameterivSGIX = NULL;

static int LoadExt_SGIX_list_priority()
{
	int numFailed = 0;
	_funcptr_glGetListParameterfvSGIX = (PFNGLGETLISTPARAMETERFVSGIXPROC)IntGetProcAddress("glGetListParameterfvSGIX");
	if(!_funcptr_glGetListParameterfvSGIX) ++numFailed;
	_funcptr_glGetListParameterivSGIX = (PFNGLGETLISTPARAMETERIVSGIXPROC)IntGetProcAddress("glGetListParameterivSGIX");
	if(!_funcptr_glGetListParameterivSGIX) ++numFailed;
	_funcptr_glListParameterfSGIX = (PFNGLLISTPARAMETERFSGIXPROC)IntGetProcAddress("glListParameterfSGIX");
	if(!_funcptr_glListParameterfSGIX) ++numFailed;
	_funcptr_glListParameterfvSGIX = (PFNGLLISTPARAMETERFVSGIXPROC)IntGetProcAddress("glListParameterfvSGIX");
	if(!_funcptr_glListParameterfvSGIX) ++numFailed;
	_funcptr_glListParameteriSGIX = (PFNGLLISTPARAMETERISGIXPROC)IntGetProcAddress("glListParameteriSGIX");
	if(!_funcptr_glListParameteriSGIX) ++numFailed;
	_funcptr_glListParameterivSGIX = (PFNGLLISTPARAMETERIVSGIXPROC)IntGetProcAddress("glListParameterivSGIX");
	if(!_funcptr_glListParameterivSGIX) ++numFailed;
	return numFailed;
}

PFNGLPIXELTEXGENSGIXPROC _funcptr_glPixelTexGenSGIX = NULL;

static int LoadExt_SGIX_pixel_texture()
{
	int numFailed = 0;
	_funcptr_glPixelTexGenSGIX = (PFNGLPIXELTEXGENSGIXPROC)IntGetProcAddress("glPixelTexGenSGIX");
	if(!_funcptr_glPixelTexGenSGIX) ++numFailed;
	return numFailed;
}

PFNGLDEFORMATIONMAP3DSGIXPROC _funcptr_glDeformationMap3dSGIX = NULL;
PFNGLDEFORMATIONMAP3FSGIXPROC _funcptr_glDeformationMap3fSGIX = NULL;
PFNGLDEFORMSGIXPROC _funcptr_glDeformSGIX = NULL;
PFNGLLOADIDENTITYDEFORMATIONMAPSGIXPROC _funcptr_glLoadIdentityDeformationMapSGIX = NULL;

static int LoadExt_SGIX_polynomial_ffd()
{
	int numFailed = 0;
	_funcptr_glDeformationMap3dSGIX = (PFNGLDEFORMATIONMAP3DSGIXPROC)IntGetProcAddress("glDeformationMap3dSGIX");
	if(!_funcptr_glDeformationMap3dSGIX) ++numFailed;
	_funcptr_glDeformationMap3fSGIX = (PFNGLDEFORMATIONMAP3FSGIXPROC)IntGetProcAddress("glDeformationMap3fSGIX");
	if(!_funcptr_glDeformationMap3fSGIX) ++numFailed;
	_funcptr_glDeformSGIX = (PFNGLDEFORMSGIXPROC)IntGetProcAddress("glDeformSGIX");
	if(!_funcptr_glDeformSGIX) ++numFailed;
	_funcptr_glLoadIdentityDeformationMapSGIX = (PFNGLLOADIDENTITYDEFORMATIONMAPSGIXPROC)IntGetProcAddress("glLoadIdentityDeformationMapSGIX");
	if(!_funcptr_glLoadIdentityDeformationMapSGIX) ++numFailed;
	return numFailed;
}

PFNGLREFERENCEPLANESGIXPROC _funcptr_glReferencePlaneSGIX = NULL;

static int LoadExt_SGIX_reference_plane()
{
	int numFailed = 0;
	_funcptr_glReferencePlaneSGIX = (PFNGLREFERENCEPLANESGIXPROC)IntGetProcAddress("glReferencePlaneSGIX");
	if(!_funcptr_glReferencePlaneSGIX) ++numFailed;
	return numFailed;
}

PFNGLSPRITEPARAMETERFSGIXPROC _funcptr_glSpriteParameterfSGIX = NULL;
PFNGLSPRITEPARAMETERFVSGIXPROC _funcptr_glSpriteParameterfvSGIX = NULL;
PFNGLSPRITEPARAMETERISGIXPROC _funcptr_glSpriteParameteriSGIX = NULL;
PFNGLSPRITEPARAMETERIVSGIXPROC _funcptr_glSpriteParameterivSGIX = NULL;

static int LoadExt_SGIX_sprite()
{
	int numFailed = 0;
	_funcptr_glSpriteParameterfSGIX = (PFNGLSPRITEPARAMETERFSGIXPROC)IntGetProcAddress("glSpriteParameterfSGIX");
	if(!_funcptr_glSpriteParameterfSGIX) ++numFailed;
	_funcptr_glSpriteParameterfvSGIX = (PFNGLSPRITEPARAMETERFVSGIXPROC)IntGetProcAddress("glSpriteParameterfvSGIX");
	if(!_funcptr_glSpriteParameterfvSGIX) ++numFailed;
	_funcptr_glSpriteParameteriSGIX = (PFNGLSPRITEPARAMETERISGIXPROC)IntGetProcAddress("glSpriteParameteriSGIX");
	if(!_funcptr_glSpriteParameteriSGIX) ++numFailed;
	_funcptr_glSpriteParameterivSGIX = (PFNGLSPRITEPARAMETERIVSGIXPROC)IntGetProcAddress("glSpriteParameterivSGIX");
	if(!_funcptr_glSpriteParameterivSGIX) ++numFailed;
	return numFailed;
}

PFNGLTAGSAMPLEBUFFERSGIXPROC _funcptr_glTagSampleBufferSGIX = NULL;

static int LoadExt_SGIX_tag_sample_buffer()
{
	int numFailed = 0;
	_funcptr_glTagSampleBufferSGIX = (PFNGLTAGSAMPLEBUFFERSGIXPROC)IntGetProcAddress("glTagSampleBufferSGIX");
	if(!_funcptr_glTagSampleBufferSGIX) ++numFailed;
	return numFailed;
}

PFNGLCOLORTABLESGIPROC _funcptr_glColorTableSGI = NULL;
PFNGLCOLORTABLEPARAMETERFVSGIPROC _funcptr_glColorTableParameterfvSGI = NULL;
PFNGLCOLORTABLEPARAMETERIVSGIPROC _funcptr_glColorTableParameterivSGI = NULL;
PFNGLCOPYCOLORTABLESGIPROC _funcptr_glCopyColorTableSGI = NULL;
PFNGLGETCOLORTABLESGIPROC _funcptr_glGetColorTableSGI = NULL;
PFNGLGETCOLORTABLEPARAMETERFVSGIPROC _funcptr_glGetColorTableParameterfvSGI = NULL;
PFNGLGETCOLORTABLEPARAMETERIVSGIPROC _funcptr_glGetColorTableParameterivSGI = NULL;

static int LoadExt_SGI_color_table()
{
	int numFailed = 0;
	_funcptr_glColorTableSGI = (PFNGLCOLORTABLESGIPROC)IntGetProcAddress("glColorTableSGI");
	if(!_funcptr_glColorTableSGI) ++numFailed;
	_funcptr_glColorTableParameterfvSGI = (PFNGLCOLORTABLEPARAMETERFVSGIPROC)IntGetProcAddress("glColorTableParameterfvSGI");
	if(!_funcptr_glColorTableParameterfvSGI) ++numFailed;
	_funcptr_glColorTableParameterivSGI = (PFNGLCOLORTABLEPARAMETERIVSGIPROC)IntGetProcAddress("glColorTableParameterivSGI");
	if(!_funcptr_glColorTableParameterivSGI) ++numFailed;
	_funcptr_glCopyColorTableSGI = (PFNGLCOPYCOLORTABLESGIPROC)IntGetProcAddress("glCopyColorTableSGI");
	if(!_funcptr_glCopyColorTableSGI) ++numFailed;
	_funcptr_glGetColorTableSGI = (PFNGLGETCOLORTABLESGIPROC)IntGetProcAddress("glGetColorTableSGI");
	if(!_funcptr_glGetColorTableSGI) ++numFailed;
	_funcptr_glGetColorTableParameterfvSGI = (PFNGLGETCOLORTABLEPARAMETERFVSGIPROC)IntGetProcAddress("glGetColorTableParameterfvSGI");
	if(!_funcptr_glGetColorTableParameterfvSGI) ++numFailed;
	_funcptr_glGetColorTableParameterivSGI = (PFNGLGETCOLORTABLEPARAMETERIVSGIPROC)IntGetProcAddress("glGetColorTableParameterivSGI");
	if(!_funcptr_glGetColorTableParameterivSGI) ++numFailed;
	return numFailed;
}

PFNGLFINISHTEXTURESUNXPROC _funcptr_glFinishTextureSUNX = NULL;

static int LoadExt_SUNX_constant_data()
{
	int numFailed = 0;
	_funcptr_glFinishTextureSUNX = (PFNGLFINISHTEXTURESUNXPROC)IntGetProcAddress("glFinishTextureSUNX");
	if(!_funcptr_glFinishTextureSUNX) ++numFailed;
	return numFailed;
}

PFNGLGLOBALALPHAFACTORBSUNPROC _funcptr_glGlobalAlphaFactorbSUN = NULL;
PFNGLGLOBALALPHAFACTORSSUNPROC _funcptr_glGlobalAlphaFactorsSUN = NULL;
PFNGLGLOBALALPHAFACTORISUNPROC _funcptr_glGlobalAlphaFactoriSUN = NULL;
PFNGLGLOBALALPHAFACTORFSUNPROC _funcptr_glGlobalAlphaFactorfSUN = NULL;
PFNGLGLOBALALPHAFACTORDSUNPROC _funcptr_glGlobalAlphaFactordSUN = NULL;
PFNGLGLOBALALPHAFACTORUBSUNPROC _funcptr_glGlobalAlphaFactorubSUN = NULL;
PFNGLGLOBALALPHAFACTORUSSUNPROC _funcptr_glGlobalAlphaFactorusSUN = NULL;
PFNGLGLOBALALPHAFACTORUISUNPROC _funcptr_glGlobalAlphaFactoruiSUN = NULL;

static int LoadExt_SUN_global_alpha()
{
	int numFailed = 0;
	_funcptr_glGlobalAlphaFactorbSUN = (PFNGLGLOBALALPHAFACTORBSUNPROC)IntGetProcAddress("glGlobalAlphaFactorbSUN");
	if(!_funcptr_glGlobalAlphaFactorbSUN) ++numFailed;
	_funcptr_glGlobalAlphaFactorsSUN = (PFNGLGLOBALALPHAFACTORSSUNPROC)IntGetProcAddress("glGlobalAlphaFactorsSUN");
	if(!_funcptr_glGlobalAlphaFactorsSUN) ++numFailed;
	_funcptr_glGlobalAlphaFactoriSUN = (PFNGLGLOBALALPHAFACTORISUNPROC)IntGetProcAddress("glGlobalAlphaFactoriSUN");
	if(!_funcptr_glGlobalAlphaFactoriSUN) ++numFailed;
	_funcptr_glGlobalAlphaFactorfSUN = (PFNGLGLOBALALPHAFACTORFSUNPROC)IntGetProcAddress("glGlobalAlphaFactorfSUN");
	if(!_funcptr_glGlobalAlphaFactorfSUN) ++numFailed;
	_funcptr_glGlobalAlphaFactordSUN = (PFNGLGLOBALALPHAFACTORDSUNPROC)IntGetProcAddress("glGlobalAlphaFactordSUN");
	if(!_funcptr_glGlobalAlphaFactordSUN) ++numFailed;
	_funcptr_glGlobalAlphaFactorubSUN = (PFNGLGLOBALALPHAFACTORUBSUNPROC)IntGetProcAddress("glGlobalAlphaFactorubSUN");
	if(!_funcptr_glGlobalAlphaFactorubSUN) ++numFailed;
	_funcptr_glGlobalAlphaFactorusSUN = (PFNGLGLOBALALPHAFACTORUSSUNPROC)IntGetProcAddress("glGlobalAlphaFactorusSUN");
	if(!_funcptr_glGlobalAlphaFactorusSUN) ++numFailed;
	_funcptr_glGlobalAlphaFactoruiSUN = (PFNGLGLOBALALPHAFACTORUISUNPROC)IntGetProcAddress("glGlobalAlphaFactoruiSUN");
	if(!_funcptr_glGlobalAlphaFactoruiSUN) ++numFailed;
	return numFailed;
}

PFNGLDRAWMESHARRAYSSUNPROC _funcptr_glDrawMeshArraysSUN = NULL;

static int LoadExt_SUN_mesh_array()
{
	int numFailed = 0;
	_funcptr_glDrawMeshArraysSUN = (PFNGLDRAWMESHARRAYSSUNPROC)IntGetProcAddress("glDrawMeshArraysSUN");
	if(!_funcptr_glDrawMeshArraysSUN) ++numFailed;
	return numFailed;
}

PFNGLREPLACEMENTCODEUISUNPROC _funcptr_glReplacementCodeuiSUN = NULL;
PFNGLREPLACEMENTCODEUSSUNPROC _funcptr_glReplacementCodeusSUN = NULL;
PFNGLREPLACEMENTCODEUBSUNPROC _funcptr_glReplacementCodeubSUN = NULL;
PFNGLREPLACEMENTCODEUIVSUNPROC _funcptr_glReplacementCodeuivSUN = NULL;
PFNGLREPLACEMENTCODEUSVSUNPROC _funcptr_glReplacementCodeusvSUN = NULL;
PFNGLREPLACEMENTCODEUBVSUNPROC _funcptr_glReplacementCodeubvSUN = NULL;
PFNGLREPLACEMENTCODEPOINTERSUNPROC _funcptr_glReplacementCodePointerSUN = NULL;

static int LoadExt_SUN_triangle_list()
{
	int numFailed = 0;
	_funcptr_glReplacementCodeuiSUN = (PFNGLREPLACEMENTCODEUISUNPROC)IntGetProcAddress("glReplacementCodeuiSUN");
	if(!_funcptr_glReplacementCodeuiSUN) ++numFailed;
	_funcptr_glReplacementCodeusSUN = (PFNGLREPLACEMENTCODEUSSUNPROC)IntGetProcAddress("glReplacementCodeusSUN");
	if(!_funcptr_glReplacementCodeusSUN) ++numFailed;
	_funcptr_glReplacementCodeubSUN = (PFNGLREPLACEMENTCODEUBSUNPROC)IntGetProcAddress("glReplacementCodeubSUN");
	if(!_funcptr_glReplacementCodeubSUN) ++numFailed;
	_funcptr_glReplacementCodeuivSUN = (PFNGLREPLACEMENTCODEUIVSUNPROC)IntGetProcAddress("glReplacementCodeuivSUN");
	if(!_funcptr_glReplacementCodeuivSUN) ++numFailed;
	_funcptr_glReplacementCodeusvSUN = (PFNGLREPLACEMENTCODEUSVSUNPROC)IntGetProcAddress("glReplacementCodeusvSUN");
	if(!_funcptr_glReplacementCodeusvSUN) ++numFailed;
	_funcptr_glReplacementCodeubvSUN = (PFNGLREPLACEMENTCODEUBVSUNPROC)IntGetProcAddress("glReplacementCodeubvSUN");
	if(!_funcptr_glReplacementCodeubvSUN) ++numFailed;
	_funcptr_glReplacementCodePointerSUN = (PFNGLREPLACEMENTCODEPOINTERSUNPROC)IntGetProcAddress("glReplacementCodePointerSUN");
	if(!_funcptr_glReplacementCodePointerSUN) ++numFailed;
	return numFailed;
}

PFNGLCOLOR4UBVERTEX2FSUNPROC _funcptr_glColor4ubVertex2fSUN = NULL;
PFNGLCOLOR4UBVERTEX2FVSUNPROC _funcptr_glColor4ubVertex2fvSUN = NULL;
PFNGLCOLOR4UBVERTEX3FSUNPROC _funcptr_glColor4ubVertex3fSUN = NULL;
PFNGLCOLOR4UBVERTEX3FVSUNPROC _funcptr_glColor4ubVertex3fvSUN = NULL;
PFNGLCOLOR3FVERTEX3FSUNPROC _funcptr_glColor3fVertex3fSUN = NULL;
PFNGLCOLOR3FVERTEX3FVSUNPROC _funcptr_glColor3fVertex3fvSUN = NULL;
PFNGLNORMAL3FVERTEX3FSUNPROC _funcptr_glNormal3fVertex3fSUN = NULL;
PFNGLNORMAL3FVERTEX3FVSUNPROC _funcptr_glNormal3fVertex3fvSUN = NULL;
PFNGLCOLOR4FNORMAL3FVERTEX3FSUNPROC _funcptr_glColor4fNormal3fVertex3fSUN = NULL;
PFNGLCOLOR4FNORMAL3FVERTEX3FVSUNPROC _funcptr_glColor4fNormal3fVertex3fvSUN = NULL;
PFNGLTEXCOORD2FVERTEX3FSUNPROC _funcptr_glTexCoord2fVertex3fSUN = NULL;
PFNGLTEXCOORD2FVERTEX3FVSUNPROC _funcptr_glTexCoord2fVertex3fvSUN = NULL;
PFNGLTEXCOORD4FVERTEX4FSUNPROC _funcptr_glTexCoord4fVertex4fSUN = NULL;
PFNGLTEXCOORD4FVERTEX4FVSUNPROC _funcptr_glTexCoord4fVertex4fvSUN = NULL;
PFNGLTEXCOORD2FCOLOR4UBVERTEX3FSUNPROC _funcptr_glTexCoord2fColor4ubVertex3fSUN = NULL;
PFNGLTEXCOORD2FCOLOR4UBVERTEX3FVSUNPROC _funcptr_glTexCoord2fColor4ubVertex3fvSUN = NULL;
PFNGLTEXCOORD2FCOLOR3FVERTEX3FSUNPROC _funcptr_glTexCoord2fColor3fVertex3fSUN = NULL;
PFNGLTEXCOORD2FCOLOR3FVERTEX3FVSUNPROC _funcptr_glTexCoord2fColor3fVertex3fvSUN = NULL;
PFNGLTEXCOORD2FNORMAL3FVERTEX3FSUNPROC _funcptr_glTexCoord2fNormal3fVertex3fSUN = NULL;
PFNGLTEXCOORD2FNORMAL3FVERTEX3FVSUNPROC _funcptr_glTexCoord2fNormal3fVertex3fvSUN = NULL;
PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC _funcptr_glTexCoord2fColor4fNormal3fVertex3fSUN = NULL;
PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC _funcptr_glTexCoord2fColor4fNormal3fVertex3fvSUN = NULL;
PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FSUNPROC _funcptr_glTexCoord4fColor4fNormal3fVertex4fSUN = NULL;
PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FVSUNPROC _funcptr_glTexCoord4fColor4fNormal3fVertex4fvSUN = NULL;
PFNGLREPLACEMENTCODEUIVERTEX3FSUNPROC _funcptr_glReplacementCodeuiVertex3fSUN = NULL;
PFNGLREPLACEMENTCODEUIVERTEX3FVSUNPROC _funcptr_glReplacementCodeuiVertex3fvSUN = NULL;
PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FSUNPROC _funcptr_glReplacementCodeuiColor4ubVertex3fSUN = NULL;
PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FVSUNPROC _funcptr_glReplacementCodeuiColor4ubVertex3fvSUN = NULL;
PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FSUNPROC _funcptr_glReplacementCodeuiColor3fVertex3fSUN = NULL;
PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FVSUNPROC _funcptr_glReplacementCodeuiColor3fVertex3fvSUN = NULL;
PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FSUNPROC _funcptr_glReplacementCodeuiNormal3fVertex3fSUN = NULL;
PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FVSUNPROC _funcptr_glReplacementCodeuiNormal3fVertex3fvSUN = NULL;
PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FSUNPROC _funcptr_glReplacementCodeuiColor4fNormal3fVertex3fSUN = NULL;
PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FVSUNPROC _funcptr_glReplacementCodeuiColor4fNormal3fVertex3fvSUN = NULL;
PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FSUNPROC _funcptr_glReplacementCodeuiTexCoord2fVertex3fSUN = NULL;
PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FVSUNPROC _funcptr_glReplacementCodeuiTexCoord2fVertex3fvSUN = NULL;
PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FSUNPROC _funcptr_glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN = NULL;
PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FVSUNPROC _funcptr_glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN = NULL;
PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC _funcptr_glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN = NULL;
PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC _funcptr_glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN = NULL;

static int LoadExt_SUN_vertex()
{
	int numFailed = 0;
	_funcptr_glColor4ubVertex2fSUN = (PFNGLCOLOR4UBVERTEX2FSUNPROC)IntGetProcAddress("glColor4ubVertex2fSUN");
	if(!_funcptr_glColor4ubVertex2fSUN) ++numFailed;
	_funcptr_glColor4ubVertex2fvSUN = (PFNGLCOLOR4UBVERTEX2FVSUNPROC)IntGetProcAddress("glColor4ubVertex2fvSUN");
	if(!_funcptr_glColor4ubVertex2fvSUN) ++numFailed;
	_funcptr_glColor4ubVertex3fSUN = (PFNGLCOLOR4UBVERTEX3FSUNPROC)IntGetProcAddress("glColor4ubVertex3fSUN");
	if(!_funcptr_glColor4ubVertex3fSUN) ++numFailed;
	_funcptr_glColor4ubVertex3fvSUN = (PFNGLCOLOR4UBVERTEX3FVSUNPROC)IntGetProcAddress("glColor4ubVertex3fvSUN");
	if(!_funcptr_glColor4ubVertex3fvSUN) ++numFailed;
	_funcptr_glColor3fVertex3fSUN = (PFNGLCOLOR3FVERTEX3FSUNPROC)IntGetProcAddress("glColor3fVertex3fSUN");
	if(!_funcptr_glColor3fVertex3fSUN) ++numFailed;
	_funcptr_glColor3fVertex3fvSUN = (PFNGLCOLOR3FVERTEX3FVSUNPROC)IntGetProcAddress("glColor3fVertex3fvSUN");
	if(!_funcptr_glColor3fVertex3fvSUN) ++numFailed;
	_funcptr_glNormal3fVertex3fSUN = (PFNGLNORMAL3FVERTEX3FSUNPROC)IntGetProcAddress("glNormal3fVertex3fSUN");
	if(!_funcptr_glNormal3fVertex3fSUN) ++numFailed;
	_funcptr_glNormal3fVertex3fvSUN = (PFNGLNORMAL3FVERTEX3FVSUNPROC)IntGetProcAddress("glNormal3fVertex3fvSUN");
	if(!_funcptr_glNormal3fVertex3fvSUN) ++numFailed;
	_funcptr_glColor4fNormal3fVertex3fSUN = (PFNGLCOLOR4FNORMAL3FVERTEX3FSUNPROC)IntGetProcAddress("glColor4fNormal3fVertex3fSUN");
	if(!_funcptr_glColor4fNormal3fVertex3fSUN) ++numFailed;
	_funcptr_glColor4fNormal3fVertex3fvSUN = (PFNGLCOLOR4FNORMAL3FVERTEX3FVSUNPROC)IntGetProcAddress("glColor4fNormal3fVertex3fvSUN");
	if(!_funcptr_glColor4fNormal3fVertex3fvSUN) ++numFailed;
	_funcptr_glTexCoord2fVertex3fSUN = (PFNGLTEXCOORD2FVERTEX3FSUNPROC)IntGetProcAddress("glTexCoord2fVertex3fSUN");
	if(!_funcptr_glTexCoord2fVertex3fSUN) ++numFailed;
	_funcptr_glTexCoord2fVertex3fvSUN = (PFNGLTEXCOORD2FVERTEX3FVSUNPROC)IntGetProcAddress("glTexCoord2fVertex3fvSUN");
	if(!_funcptr_glTexCoord2fVertex3fvSUN) ++numFailed;
	_funcptr_glTexCoord4fVertex4fSUN = (PFNGLTEXCOORD4FVERTEX4FSUNPROC)IntGetProcAddress("glTexCoord4fVertex4fSUN");
	if(!_funcptr_glTexCoord4fVertex4fSUN) ++numFailed;
	_funcptr_glTexCoord4fVertex4fvSUN = (PFNGLTEXCOORD4FVERTEX4FVSUNPROC)IntGetProcAddress("glTexCoord4fVertex4fvSUN");
	if(!_funcptr_glTexCoord4fVertex4fvSUN) ++numFailed;
	_funcptr_glTexCoord2fColor4ubVertex3fSUN = (PFNGLTEXCOORD2FCOLOR4UBVERTEX3FSUNPROC)IntGetProcAddress("glTexCoord2fColor4ubVertex3fSUN");
	if(!_funcptr_glTexCoord2fColor4ubVertex3fSUN) ++numFailed;
	_funcptr_glTexCoord2fColor4ubVertex3fvSUN = (PFNGLTEXCOORD2FCOLOR4UBVERTEX3FVSUNPROC)IntGetProcAddress("glTexCoord2fColor4ubVertex3fvSUN");
	if(!_funcptr_glTexCoord2fColor4ubVertex3fvSUN) ++numFailed;
	_funcptr_glTexCoord2fColor3fVertex3fSUN = (PFNGLTEXCOORD2FCOLOR3FVERTEX3FSUNPROC)IntGetProcAddress("glTexCoord2fColor3fVertex3fSUN");
	if(!_funcptr_glTexCoord2fColor3fVertex3fSUN) ++numFailed;
	_funcptr_glTexCoord2fColor3fVertex3fvSUN = (PFNGLTEXCOORD2FCOLOR3FVERTEX3FVSUNPROC)IntGetProcAddress("glTexCoord2fColor3fVertex3fvSUN");
	if(!_funcptr_glTexCoord2fColor3fVertex3fvSUN) ++numFailed;
	_funcptr_glTexCoord2fNormal3fVertex3fSUN = (PFNGLTEXCOORD2FNORMAL3FVERTEX3FSUNPROC)IntGetProcAddress("glTexCoord2fNormal3fVertex3fSUN");
	if(!_funcptr_glTexCoord2fNormal3fVertex3fSUN) ++numFailed;
	_funcptr_glTexCoord2fNormal3fVertex3fvSUN = (PFNGLTEXCOORD2FNORMAL3FVERTEX3FVSUNPROC)IntGetProcAddress("glTexCoord2fNormal3fVertex3fvSUN");
	if(!_funcptr_glTexCoord2fNormal3fVertex3fvSUN) ++numFailed;
	_funcptr_glTexCoord2fColor4fNormal3fVertex3fSUN = (PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC)IntGetProcAddress("glTexCoord2fColor4fNormal3fVertex3fSUN");
	if(!_funcptr_glTexCoord2fColor4fNormal3fVertex3fSUN) ++numFailed;
	_funcptr_glTexCoord2fColor4fNormal3fVertex3fvSUN = (PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC)IntGetProcAddress("glTexCoord2fColor4fNormal3fVertex3fvSUN");
	if(!_funcptr_glTexCoord2fColor4fNormal3fVertex3fvSUN) ++numFailed;
	_funcptr_glTexCoord4fColor4fNormal3fVertex4fSUN = (PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FSUNPROC)IntGetProcAddress("glTexCoord4fColor4fNormal3fVertex4fSUN");
	if(!_funcptr_glTexCoord4fColor4fNormal3fVertex4fSUN) ++numFailed;
	_funcptr_glTexCoord4fColor4fNormal3fVertex4fvSUN = (PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FVSUNPROC)IntGetProcAddress("glTexCoord4fColor4fNormal3fVertex4fvSUN");
	if(!_funcptr_glTexCoord4fColor4fNormal3fVertex4fvSUN) ++numFailed;
	_funcptr_glReplacementCodeuiVertex3fSUN = (PFNGLREPLACEMENTCODEUIVERTEX3FSUNPROC)IntGetProcAddress("glReplacementCodeuiVertex3fSUN");
	if(!_funcptr_glReplacementCodeuiVertex3fSUN) ++numFailed;
	_funcptr_glReplacementCodeuiVertex3fvSUN = (PFNGLREPLACEMENTCODEUIVERTEX3FVSUNPROC)IntGetProcAddress("glReplacementCodeuiVertex3fvSUN");
	if(!_funcptr_glReplacementCodeuiVertex3fvSUN) ++numFailed;
	_funcptr_glReplacementCodeuiColor4ubVertex3fSUN = (PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FSUNPROC)IntGetProcAddress("glReplacementCodeuiColor4ubVertex3fSUN");
	if(!_funcptr_glReplacementCodeuiColor4ubVertex3fSUN) ++numFailed;
	_funcptr_glReplacementCodeuiColor4ubVertex3fvSUN = (PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FVSUNPROC)IntGetProcAddress("glReplacementCodeuiColor4ubVertex3fvSUN");
	if(!_funcptr_glReplacementCodeuiColor4ubVertex3fvSUN) ++numFailed;
	_funcptr_glReplacementCodeuiColor3fVertex3fSUN = (PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FSUNPROC)IntGetProcAddress("glReplacementCodeuiColor3fVertex3fSUN");
	if(!_funcptr_glReplacementCodeuiColor3fVertex3fSUN) ++numFailed;
	_funcptr_glReplacementCodeuiColor3fVertex3fvSUN = (PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FVSUNPROC)IntGetProcAddress("glReplacementCodeuiColor3fVertex3fvSUN");
	if(!_funcptr_glReplacementCodeuiColor3fVertex3fvSUN) ++numFailed;
	_funcptr_glReplacementCodeuiNormal3fVertex3fSUN = (PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FSUNPROC)IntGetProcAddress("glReplacementCodeuiNormal3fVertex3fSUN");
	if(!_funcptr_glReplacementCodeuiNormal3fVertex3fSUN) ++numFailed;
	_funcptr_glReplacementCodeuiNormal3fVertex3fvSUN = (PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FVSUNPROC)IntGetProcAddress("glReplacementCodeuiNormal3fVertex3fvSUN");
	if(!_funcptr_glReplacementCodeuiNormal3fVertex3fvSUN) ++numFailed;
	_funcptr_glReplacementCodeuiColor4fNormal3fVertex3fSUN = (PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FSUNPROC)IntGetProcAddress("glReplacementCodeuiColor4fNormal3fVertex3fSUN");
	if(!_funcptr_glReplacementCodeuiColor4fNormal3fVertex3fSUN) ++numFailed;
	_funcptr_glReplacementCodeuiColor4fNormal3fVertex3fvSUN = (PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FVSUNPROC)IntGetProcAddress("glReplacementCodeuiColor4fNormal3fVertex3fvSUN");
	if(!_funcptr_glReplacementCodeuiColor4fNormal3fVertex3fvSUN) ++numFailed;
	_funcptr_glReplacementCodeuiTexCoord2fVertex3fSUN = (PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FSUNPROC)IntGetProcAddress("glReplacementCodeuiTexCoord2fVertex3fSUN");
	if(!_funcptr_glReplacementCodeuiTexCoord2fVertex3fSUN) ++numFailed;
	_funcptr_glReplacementCodeuiTexCoord2fVertex3fvSUN = (PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FVSUNPROC)IntGetProcAddress("glReplacementCodeuiTexCoord2fVertex3fvSUN");
	if(!_funcptr_glReplacementCodeuiTexCoord2fVertex3fvSUN) ++numFailed;
	_funcptr_glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN = (PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FSUNPROC)IntGetProcAddress("glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN");
	if(!_funcptr_glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN) ++numFailed;
	_funcptr_glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN = (PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FVSUNPROC)IntGetProcAddress("glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN");
	if(!_funcptr_glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN) ++numFailed;
	_funcptr_glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN = (PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC)IntGetProcAddress("glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN");
	if(!_funcptr_glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN) ++numFailed;
	_funcptr_glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN = (PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC)IntGetProcAddress("glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN");
	if(!_funcptr_glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN) ++numFailed;
	return numFailed;
}


PFNGLCULLFACEPROC _funcptr_glCullFace = NULL;
PFNGLFRONTFACEPROC _funcptr_glFrontFace = NULL;
PFNGLHINTPROC _funcptr_glHint = NULL;
PFNGLLINEWIDTHPROC _funcptr_glLineWidth = NULL;
PFNGLPOINTSIZEPROC _funcptr_glPointSize = NULL;
PFNGLPOLYGONMODEPROC _funcptr_glPolygonMode = NULL;
PFNGLSCISSORPROC _funcptr_glScissor = NULL;
PFNGLTEXPARAMETERFPROC _funcptr_glTexParameterf = NULL;
PFNGLTEXPARAMETERFVPROC _funcptr_glTexParameterfv = NULL;
PFNGLTEXPARAMETERIPROC _funcptr_glTexParameteri = NULL;
PFNGLTEXPARAMETERIVPROC _funcptr_glTexParameteriv = NULL;
PFNGLTEXIMAGE1DPROC _funcptr_glTexImage1D = NULL;
PFNGLTEXIMAGE2DPROC _funcptr_glTexImage2D = NULL;
PFNGLDRAWBUFFERPROC _funcptr_glDrawBuffer = NULL;
PFNGLCLEARPROC _funcptr_glClear = NULL;
PFNGLCLEARCOLORPROC _funcptr_glClearColor = NULL;
PFNGLCLEARSTENCILPROC _funcptr_glClearStencil = NULL;
PFNGLCLEARDEPTHPROC _funcptr_glClearDepth = NULL;
PFNGLSTENCILMASKPROC _funcptr_glStencilMask = NULL;
PFNGLCOLORMASKPROC _funcptr_glColorMask = NULL;
PFNGLDEPTHMASKPROC _funcptr_glDepthMask = NULL;
PFNGLDISABLEPROC _funcptr_glDisable = NULL;
PFNGLENABLEPROC _funcptr_glEnable = NULL;
PFNGLFINISHPROC _funcptr_glFinish = NULL;
PFNGLFLUSHPROC _funcptr_glFlush = NULL;
PFNGLBLENDFUNCPROC _funcptr_glBlendFunc = NULL;
PFNGLLOGICOPPROC _funcptr_glLogicOp = NULL;
PFNGLSTENCILFUNCPROC _funcptr_glStencilFunc = NULL;
PFNGLSTENCILOPPROC _funcptr_glStencilOp = NULL;
PFNGLDEPTHFUNCPROC _funcptr_glDepthFunc = NULL;
PFNGLPIXELSTOREFPROC _funcptr_glPixelStoref = NULL;
PFNGLPIXELSTOREIPROC _funcptr_glPixelStorei = NULL;
PFNGLREADBUFFERPROC _funcptr_glReadBuffer = NULL;
PFNGLREADPIXELSPROC _funcptr_glReadPixels = NULL;
PFNGLGETBOOLEANVPROC _funcptr_glGetBooleanv = NULL;
PFNGLGETDOUBLEVPROC _funcptr_glGetDoublev = NULL;
PFNGLGETERRORPROC _funcptr_glGetError = NULL;
PFNGLGETFLOATVPROC _funcptr_glGetFloatv = NULL;
PFNGLGETINTEGERVPROC _funcptr_glGetIntegerv = NULL;
PFNGLGETSTRINGPROC _funcptr_glGetString = NULL;
PFNGLGETTEXIMAGEPROC _funcptr_glGetTexImage = NULL;
PFNGLGETTEXPARAMETERFVPROC _funcptr_glGetTexParameterfv = NULL;
PFNGLGETTEXPARAMETERIVPROC _funcptr_glGetTexParameteriv = NULL;
PFNGLGETTEXLEVELPARAMETERFVPROC _funcptr_glGetTexLevelParameterfv = NULL;
PFNGLGETTEXLEVELPARAMETERIVPROC _funcptr_glGetTexLevelParameteriv = NULL;
PFNGLISENABLEDPROC _funcptr_glIsEnabled = NULL;
PFNGLDEPTHRANGEPROC _funcptr_glDepthRange = NULL;
PFNGLVIEWPORTPROC _funcptr_glViewport = NULL;
PFNGLNEWLISTPROC _funcptr_glNewList = NULL;
PFNGLENDLISTPROC _funcptr_glEndList = NULL;
PFNGLCALLLISTPROC _funcptr_glCallList = NULL;
PFNGLCALLLISTSPROC _funcptr_glCallLists = NULL;
PFNGLDELETELISTSPROC _funcptr_glDeleteLists = NULL;
PFNGLGENLISTSPROC _funcptr_glGenLists = NULL;
PFNGLLISTBASEPROC _funcptr_glListBase = NULL;
PFNGLBEGINPROC _funcptr_glBegin = NULL;
PFNGLBITMAPPROC _funcptr_glBitmap = NULL;
PFNGLCOLOR3BPROC _funcptr_glColor3b = NULL;
PFNGLCOLOR3BVPROC _funcptr_glColor3bv = NULL;
PFNGLCOLOR3DPROC _funcptr_glColor3d = NULL;
PFNGLCOLOR3DVPROC _funcptr_glColor3dv = NULL;
PFNGLCOLOR3FPROC _funcptr_glColor3f = NULL;
PFNGLCOLOR3FVPROC _funcptr_glColor3fv = NULL;
PFNGLCOLOR3IPROC _funcptr_glColor3i = NULL;
PFNGLCOLOR3IVPROC _funcptr_glColor3iv = NULL;
PFNGLCOLOR3SPROC _funcptr_glColor3s = NULL;
PFNGLCOLOR3SVPROC _funcptr_glColor3sv = NULL;
PFNGLCOLOR3UBPROC _funcptr_glColor3ub = NULL;
PFNGLCOLOR3UBVPROC _funcptr_glColor3ubv = NULL;
PFNGLCOLOR3UIPROC _funcptr_glColor3ui = NULL;
PFNGLCOLOR3UIVPROC _funcptr_glColor3uiv = NULL;
PFNGLCOLOR3USPROC _funcptr_glColor3us = NULL;
PFNGLCOLOR3USVPROC _funcptr_glColor3usv = NULL;
PFNGLCOLOR4BPROC _funcptr_glColor4b = NULL;
PFNGLCOLOR4BVPROC _funcptr_glColor4bv = NULL;
PFNGLCOLOR4DPROC _funcptr_glColor4d = NULL;
PFNGLCOLOR4DVPROC _funcptr_glColor4dv = NULL;
PFNGLCOLOR4FPROC _funcptr_glColor4f = NULL;
PFNGLCOLOR4FVPROC _funcptr_glColor4fv = NULL;
PFNGLCOLOR4IPROC _funcptr_glColor4i = NULL;
PFNGLCOLOR4IVPROC _funcptr_glColor4iv = NULL;
PFNGLCOLOR4SPROC _funcptr_glColor4s = NULL;
PFNGLCOLOR4SVPROC _funcptr_glColor4sv = NULL;
PFNGLCOLOR4UBPROC _funcptr_glColor4ub = NULL;
PFNGLCOLOR4UBVPROC _funcptr_glColor4ubv = NULL;
PFNGLCOLOR4UIPROC _funcptr_glColor4ui = NULL;
PFNGLCOLOR4UIVPROC _funcptr_glColor4uiv = NULL;
PFNGLCOLOR4USPROC _funcptr_glColor4us = NULL;
PFNGLCOLOR4USVPROC _funcptr_glColor4usv = NULL;
PFNGLEDGEFLAGPROC _funcptr_glEdgeFlag = NULL;
PFNGLEDGEFLAGVPROC _funcptr_glEdgeFlagv = NULL;
PFNGLENDPROC _funcptr_glEnd = NULL;
PFNGLINDEXDPROC _funcptr_glIndexd = NULL;
PFNGLINDEXDVPROC _funcptr_glIndexdv = NULL;
PFNGLINDEXFPROC _funcptr_glIndexf = NULL;
PFNGLINDEXFVPROC _funcptr_glIndexfv = NULL;
PFNGLINDEXIPROC _funcptr_glIndexi = NULL;
PFNGLINDEXIVPROC _funcptr_glIndexiv = NULL;
PFNGLINDEXSPROC _funcptr_glIndexs = NULL;
PFNGLINDEXSVPROC _funcptr_glIndexsv = NULL;
PFNGLNORMAL3BPROC _funcptr_glNormal3b = NULL;
PFNGLNORMAL3BVPROC _funcptr_glNormal3bv = NULL;
PFNGLNORMAL3DPROC _funcptr_glNormal3d = NULL;
PFNGLNORMAL3DVPROC _funcptr_glNormal3dv = NULL;
PFNGLNORMAL3FPROC _funcptr_glNormal3f = NULL;
PFNGLNORMAL3FVPROC _funcptr_glNormal3fv = NULL;
PFNGLNORMAL3IPROC _funcptr_glNormal3i = NULL;
PFNGLNORMAL3IVPROC _funcptr_glNormal3iv = NULL;
PFNGLNORMAL3SPROC _funcptr_glNormal3s = NULL;
PFNGLNORMAL3SVPROC _funcptr_glNormal3sv = NULL;
PFNGLRASTERPOS2DPROC _funcptr_glRasterPos2d = NULL;
PFNGLRASTERPOS2DVPROC _funcptr_glRasterPos2dv = NULL;
PFNGLRASTERPOS2FPROC _funcptr_glRasterPos2f = NULL;
PFNGLRASTERPOS2FVPROC _funcptr_glRasterPos2fv = NULL;
PFNGLRASTERPOS2IPROC _funcptr_glRasterPos2i = NULL;
PFNGLRASTERPOS2IVPROC _funcptr_glRasterPos2iv = NULL;
PFNGLRASTERPOS2SPROC _funcptr_glRasterPos2s = NULL;
PFNGLRASTERPOS2SVPROC _funcptr_glRasterPos2sv = NULL;
PFNGLRASTERPOS3DPROC _funcptr_glRasterPos3d = NULL;
PFNGLRASTERPOS3DVPROC _funcptr_glRasterPos3dv = NULL;
PFNGLRASTERPOS3FPROC _funcptr_glRasterPos3f = NULL;
PFNGLRASTERPOS3FVPROC _funcptr_glRasterPos3fv = NULL;
PFNGLRASTERPOS3IPROC _funcptr_glRasterPos3i = NULL;
PFNGLRASTERPOS3IVPROC _funcptr_glRasterPos3iv = NULL;
PFNGLRASTERPOS3SPROC _funcptr_glRasterPos3s = NULL;
PFNGLRASTERPOS3SVPROC _funcptr_glRasterPos3sv = NULL;
PFNGLRASTERPOS4DPROC _funcptr_glRasterPos4d = NULL;
PFNGLRASTERPOS4DVPROC _funcptr_glRasterPos4dv = NULL;
PFNGLRASTERPOS4FPROC _funcptr_glRasterPos4f = NULL;
PFNGLRASTERPOS4FVPROC _funcptr_glRasterPos4fv = NULL;
PFNGLRASTERPOS4IPROC _funcptr_glRasterPos4i = NULL;
PFNGLRASTERPOS4IVPROC _funcptr_glRasterPos4iv = NULL;
PFNGLRASTERPOS4SPROC _funcptr_glRasterPos4s = NULL;
PFNGLRASTERPOS4SVPROC _funcptr_glRasterPos4sv = NULL;
PFNGLRECTDPROC _funcptr_glRectd = NULL;
PFNGLRECTDVPROC _funcptr_glRectdv = NULL;
PFNGLRECTFPROC _funcptr_glRectf = NULL;
PFNGLRECTFVPROC _funcptr_glRectfv = NULL;
PFNGLRECTIPROC _funcptr_glRecti = NULL;
PFNGLRECTIVPROC _funcptr_glRectiv = NULL;
PFNGLRECTSPROC _funcptr_glRects = NULL;
PFNGLRECTSVPROC _funcptr_glRectsv = NULL;
PFNGLTEXCOORD1DPROC _funcptr_glTexCoord1d = NULL;
PFNGLTEXCOORD1DVPROC _funcptr_glTexCoord1dv = NULL;
PFNGLTEXCOORD1FPROC _funcptr_glTexCoord1f = NULL;
PFNGLTEXCOORD1FVPROC _funcptr_glTexCoord1fv = NULL;
PFNGLTEXCOORD1IPROC _funcptr_glTexCoord1i = NULL;
PFNGLTEXCOORD1IVPROC _funcptr_glTexCoord1iv = NULL;
PFNGLTEXCOORD1SPROC _funcptr_glTexCoord1s = NULL;
PFNGLTEXCOORD1SVPROC _funcptr_glTexCoord1sv = NULL;
PFNGLTEXCOORD2DPROC _funcptr_glTexCoord2d = NULL;
PFNGLTEXCOORD2DVPROC _funcptr_glTexCoord2dv = NULL;
PFNGLTEXCOORD2FPROC _funcptr_glTexCoord2f = NULL;
PFNGLTEXCOORD2FVPROC _funcptr_glTexCoord2fv = NULL;
PFNGLTEXCOORD2IPROC _funcptr_glTexCoord2i = NULL;
PFNGLTEXCOORD2IVPROC _funcptr_glTexCoord2iv = NULL;
PFNGLTEXCOORD2SPROC _funcptr_glTexCoord2s = NULL;
PFNGLTEXCOORD2SVPROC _funcptr_glTexCoord2sv = NULL;
PFNGLTEXCOORD3DPROC _funcptr_glTexCoord3d = NULL;
PFNGLTEXCOORD3DVPROC _funcptr_glTexCoord3dv = NULL;
PFNGLTEXCOORD3FPROC _funcptr_glTexCoord3f = NULL;
PFNGLTEXCOORD3FVPROC _funcptr_glTexCoord3fv = NULL;
PFNGLTEXCOORD3IPROC _funcptr_glTexCoord3i = NULL;
PFNGLTEXCOORD3IVPROC _funcptr_glTexCoord3iv = NULL;
PFNGLTEXCOORD3SPROC _funcptr_glTexCoord3s = NULL;
PFNGLTEXCOORD3SVPROC _funcptr_glTexCoord3sv = NULL;
PFNGLTEXCOORD4DPROC _funcptr_glTexCoord4d = NULL;
PFNGLTEXCOORD4DVPROC _funcptr_glTexCoord4dv = NULL;
PFNGLTEXCOORD4FPROC _funcptr_glTexCoord4f = NULL;
PFNGLTEXCOORD4FVPROC _funcptr_glTexCoord4fv = NULL;
PFNGLTEXCOORD4IPROC _funcptr_glTexCoord4i = NULL;
PFNGLTEXCOORD4IVPROC _funcptr_glTexCoord4iv = NULL;
PFNGLTEXCOORD4SPROC _funcptr_glTexCoord4s = NULL;
PFNGLTEXCOORD4SVPROC _funcptr_glTexCoord4sv = NULL;
PFNGLVERTEX2DPROC _funcptr_glVertex2d = NULL;
PFNGLVERTEX2DVPROC _funcptr_glVertex2dv = NULL;
PFNGLVERTEX2FPROC _funcptr_glVertex2f = NULL;
PFNGLVERTEX2FVPROC _funcptr_glVertex2fv = NULL;
PFNGLVERTEX2IPROC _funcptr_glVertex2i = NULL;
PFNGLVERTEX2IVPROC _funcptr_glVertex2iv = NULL;
PFNGLVERTEX2SPROC _funcptr_glVertex2s = NULL;
PFNGLVERTEX2SVPROC _funcptr_glVertex2sv = NULL;
PFNGLVERTEX3DPROC _funcptr_glVertex3d = NULL;
PFNGLVERTEX3DVPROC _funcptr_glVertex3dv = NULL;
PFNGLVERTEX3FPROC _funcptr_glVertex3f = NULL;
PFNGLVERTEX3FVPROC _funcptr_glVertex3fv = NULL;
PFNGLVERTEX3IPROC _funcptr_glVertex3i = NULL;
PFNGLVERTEX3IVPROC _funcptr_glVertex3iv = NULL;
PFNGLVERTEX3SPROC _funcptr_glVertex3s = NULL;
PFNGLVERTEX3SVPROC _funcptr_glVertex3sv = NULL;
PFNGLVERTEX4DPROC _funcptr_glVertex4d = NULL;
PFNGLVERTEX4DVPROC _funcptr_glVertex4dv = NULL;
PFNGLVERTEX4FPROC _funcptr_glVertex4f = NULL;
PFNGLVERTEX4FVPROC _funcptr_glVertex4fv = NULL;
PFNGLVERTEX4IPROC _funcptr_glVertex4i = NULL;
PFNGLVERTEX4IVPROC _funcptr_glVertex4iv = NULL;
PFNGLVERTEX4SPROC _funcptr_glVertex4s = NULL;
PFNGLVERTEX4SVPROC _funcptr_glVertex4sv = NULL;
PFNGLCLIPPLANEPROC _funcptr_glClipPlane = NULL;
PFNGLCOLORMATERIALPROC _funcptr_glColorMaterial = NULL;
PFNGLFOGFPROC _funcptr_glFogf = NULL;
PFNGLFOGFVPROC _funcptr_glFogfv = NULL;
PFNGLFOGIPROC _funcptr_glFogi = NULL;
PFNGLFOGIVPROC _funcptr_glFogiv = NULL;
PFNGLLIGHTFPROC _funcptr_glLightf = NULL;
PFNGLLIGHTFVPROC _funcptr_glLightfv = NULL;
PFNGLLIGHTIPROC _funcptr_glLighti = NULL;
PFNGLLIGHTIVPROC _funcptr_glLightiv = NULL;
PFNGLLIGHTMODELFPROC _funcptr_glLightModelf = NULL;
PFNGLLIGHTMODELFVPROC _funcptr_glLightModelfv = NULL;
PFNGLLIGHTMODELIPROC _funcptr_glLightModeli = NULL;
PFNGLLIGHTMODELIVPROC _funcptr_glLightModeliv = NULL;
PFNGLLINESTIPPLEPROC _funcptr_glLineStipple = NULL;
PFNGLMATERIALFPROC _funcptr_glMaterialf = NULL;
PFNGLMATERIALFVPROC _funcptr_glMaterialfv = NULL;
PFNGLMATERIALIPROC _funcptr_glMateriali = NULL;
PFNGLMATERIALIVPROC _funcptr_glMaterialiv = NULL;
PFNGLPOLYGONSTIPPLEPROC _funcptr_glPolygonStipple = NULL;
PFNGLSHADEMODELPROC _funcptr_glShadeModel = NULL;
PFNGLTEXENVFPROC _funcptr_glTexEnvf = NULL;
PFNGLTEXENVFVPROC _funcptr_glTexEnvfv = NULL;
PFNGLTEXENVIPROC _funcptr_glTexEnvi = NULL;
PFNGLTEXENVIVPROC _funcptr_glTexEnviv = NULL;
PFNGLTEXGENDPROC _funcptr_glTexGend = NULL;
PFNGLTEXGENDVPROC _funcptr_glTexGendv = NULL;
PFNGLTEXGENFPROC _funcptr_glTexGenf = NULL;
PFNGLTEXGENFVPROC _funcptr_glTexGenfv = NULL;
PFNGLTEXGENIPROC _funcptr_glTexGeni = NULL;
PFNGLTEXGENIVPROC _funcptr_glTexGeniv = NULL;
PFNGLFEEDBACKBUFFERPROC _funcptr_glFeedbackBuffer = NULL;
PFNGLSELECTBUFFERPROC _funcptr_glSelectBuffer = NULL;
PFNGLRENDERMODEPROC _funcptr_glRenderMode = NULL;
PFNGLINITNAMESPROC _funcptr_glInitNames = NULL;
PFNGLLOADNAMEPROC _funcptr_glLoadName = NULL;
PFNGLPASSTHROUGHPROC _funcptr_glPassThrough = NULL;
PFNGLPOPNAMEPROC _funcptr_glPopName = NULL;
PFNGLPUSHNAMEPROC _funcptr_glPushName = NULL;
PFNGLCLEARACCUMPROC _funcptr_glClearAccum = NULL;
PFNGLCLEARINDEXPROC _funcptr_glClearIndex = NULL;
PFNGLINDEXMASKPROC _funcptr_glIndexMask = NULL;
PFNGLACCUMPROC _funcptr_glAccum = NULL;
PFNGLPOPATTRIBPROC _funcptr_glPopAttrib = NULL;
PFNGLPUSHATTRIBPROC _funcptr_glPushAttrib = NULL;
PFNGLMAP1DPROC _funcptr_glMap1d = NULL;
PFNGLMAP1FPROC _funcptr_glMap1f = NULL;
PFNGLMAP2DPROC _funcptr_glMap2d = NULL;
PFNGLMAP2FPROC _funcptr_glMap2f = NULL;
PFNGLMAPGRID1DPROC _funcptr_glMapGrid1d = NULL;
PFNGLMAPGRID1FPROC _funcptr_glMapGrid1f = NULL;
PFNGLMAPGRID2DPROC _funcptr_glMapGrid2d = NULL;
PFNGLMAPGRID2FPROC _funcptr_glMapGrid2f = NULL;
PFNGLEVALCOORD1DPROC _funcptr_glEvalCoord1d = NULL;
PFNGLEVALCOORD1DVPROC _funcptr_glEvalCoord1dv = NULL;
PFNGLEVALCOORD1FPROC _funcptr_glEvalCoord1f = NULL;
PFNGLEVALCOORD1FVPROC _funcptr_glEvalCoord1fv = NULL;
PFNGLEVALCOORD2DPROC _funcptr_glEvalCoord2d = NULL;
PFNGLEVALCOORD2DVPROC _funcptr_glEvalCoord2dv = NULL;
PFNGLEVALCOORD2FPROC _funcptr_glEvalCoord2f = NULL;
PFNGLEVALCOORD2FVPROC _funcptr_glEvalCoord2fv = NULL;
PFNGLEVALMESH1PROC _funcptr_glEvalMesh1 = NULL;
PFNGLEVALPOINT1PROC _funcptr_glEvalPoint1 = NULL;
PFNGLEVALMESH2PROC _funcptr_glEvalMesh2 = NULL;
PFNGLEVALPOINT2PROC _funcptr_glEvalPoint2 = NULL;
PFNGLALPHAFUNCPROC _funcptr_glAlphaFunc = NULL;
PFNGLPIXELZOOMPROC _funcptr_glPixelZoom = NULL;
PFNGLPIXELTRANSFERFPROC _funcptr_glPixelTransferf = NULL;
PFNGLPIXELTRANSFERIPROC _funcptr_glPixelTransferi = NULL;
PFNGLPIXELMAPFVPROC _funcptr_glPixelMapfv = NULL;
PFNGLPIXELMAPUIVPROC _funcptr_glPixelMapuiv = NULL;
PFNGLPIXELMAPUSVPROC _funcptr_glPixelMapusv = NULL;
PFNGLCOPYPIXELSPROC _funcptr_glCopyPixels = NULL;
PFNGLDRAWPIXELSPROC _funcptr_glDrawPixels = NULL;
PFNGLGETCLIPPLANEPROC _funcptr_glGetClipPlane = NULL;
PFNGLGETLIGHTFVPROC _funcptr_glGetLightfv = NULL;
PFNGLGETLIGHTIVPROC _funcptr_glGetLightiv = NULL;
PFNGLGETMAPDVPROC _funcptr_glGetMapdv = NULL;
PFNGLGETMAPFVPROC _funcptr_glGetMapfv = NULL;
PFNGLGETMAPIVPROC _funcptr_glGetMapiv = NULL;
PFNGLGETMATERIALFVPROC _funcptr_glGetMaterialfv = NULL;
PFNGLGETMATERIALIVPROC _funcptr_glGetMaterialiv = NULL;
PFNGLGETPIXELMAPFVPROC _funcptr_glGetPixelMapfv = NULL;
PFNGLGETPIXELMAPUIVPROC _funcptr_glGetPixelMapuiv = NULL;
PFNGLGETPIXELMAPUSVPROC _funcptr_glGetPixelMapusv = NULL;
PFNGLGETPOLYGONSTIPPLEPROC _funcptr_glGetPolygonStipple = NULL;
PFNGLGETTEXENVFVPROC _funcptr_glGetTexEnvfv = NULL;
PFNGLGETTEXENVIVPROC _funcptr_glGetTexEnviv = NULL;
PFNGLGETTEXGENDVPROC _funcptr_glGetTexGendv = NULL;
PFNGLGETTEXGENFVPROC _funcptr_glGetTexGenfv = NULL;
PFNGLGETTEXGENIVPROC _funcptr_glGetTexGeniv = NULL;
PFNGLISLISTPROC _funcptr_glIsList = NULL;
PFNGLFRUSTUMPROC _funcptr_glFrustum = NULL;
PFNGLLOADIDENTITYPROC _funcptr_glLoadIdentity = NULL;
PFNGLLOADMATRIXFPROC _funcptr_glLoadMatrixf = NULL;
PFNGLLOADMATRIXDPROC _funcptr_glLoadMatrixd = NULL;
PFNGLMATRIXMODEPROC _funcptr_glMatrixMode = NULL;
PFNGLMULTMATRIXFPROC _funcptr_glMultMatrixf = NULL;
PFNGLMULTMATRIXDPROC _funcptr_glMultMatrixd = NULL;
PFNGLORTHOPROC _funcptr_glOrtho = NULL;
PFNGLPOPMATRIXPROC _funcptr_glPopMatrix = NULL;
PFNGLPUSHMATRIXPROC _funcptr_glPushMatrix = NULL;
PFNGLROTATEDPROC _funcptr_glRotated = NULL;
PFNGLROTATEFPROC _funcptr_glRotatef = NULL;
PFNGLSCALEDPROC _funcptr_glScaled = NULL;
PFNGLSCALEFPROC _funcptr_glScalef = NULL;
PFNGLTRANSLATEDPROC _funcptr_glTranslated = NULL;
PFNGLTRANSLATEFPROC _funcptr_glTranslatef = NULL;
PFNGLDRAWARRAYSPROC _funcptr_glDrawArrays = NULL;
PFNGLDRAWELEMENTSPROC _funcptr_glDrawElements = NULL;
PFNGLGETPOINTERVPROC _funcptr_glGetPointerv = NULL;
PFNGLPOLYGONOFFSETPROC _funcptr_glPolygonOffset = NULL;
PFNGLCOPYTEXIMAGE1DPROC _funcptr_glCopyTexImage1D = NULL;
PFNGLCOPYTEXIMAGE2DPROC _funcptr_glCopyTexImage2D = NULL;
PFNGLCOPYTEXSUBIMAGE1DPROC _funcptr_glCopyTexSubImage1D = NULL;
PFNGLCOPYTEXSUBIMAGE2DPROC _funcptr_glCopyTexSubImage2D = NULL;
PFNGLTEXSUBIMAGE1DPROC _funcptr_glTexSubImage1D = NULL;
PFNGLTEXSUBIMAGE2DPROC _funcptr_glTexSubImage2D = NULL;
PFNGLBINDTEXTUREPROC _funcptr_glBindTexture = NULL;
PFNGLDELETETEXTURESPROC _funcptr_glDeleteTextures = NULL;
PFNGLGENTEXTURESPROC _funcptr_glGenTextures = NULL;
PFNGLISTEXTUREPROC _funcptr_glIsTexture = NULL;
PFNGLARRAYELEMENTPROC _funcptr_glArrayElement = NULL;
PFNGLCOLORPOINTERPROC _funcptr_glColorPointer = NULL;
PFNGLDISABLECLIENTSTATEPROC _funcptr_glDisableClientState = NULL;
PFNGLEDGEFLAGPOINTERPROC _funcptr_glEdgeFlagPointer = NULL;
PFNGLENABLECLIENTSTATEPROC _funcptr_glEnableClientState = NULL;
PFNGLINDEXPOINTERPROC _funcptr_glIndexPointer = NULL;
PFNGLINTERLEAVEDARRAYSPROC _funcptr_glInterleavedArrays = NULL;
PFNGLNORMALPOINTERPROC _funcptr_glNormalPointer = NULL;
PFNGLTEXCOORDPOINTERPROC _funcptr_glTexCoordPointer = NULL;
PFNGLVERTEXPOINTERPROC _funcptr_glVertexPointer = NULL;
PFNGLARETEXTURESRESIDENTPROC _funcptr_glAreTexturesResident = NULL;
PFNGLPRIORITIZETEXTURESPROC _funcptr_glPrioritizeTextures = NULL;
PFNGLINDEXUBPROC _funcptr_glIndexub = NULL;
PFNGLINDEXUBVPROC _funcptr_glIndexubv = NULL;
PFNGLPOPCLIENTATTRIBPROC _funcptr_glPopClientAttrib = NULL;
PFNGLPUSHCLIENTATTRIBPROC _funcptr_glPushClientAttrib = NULL;
PFNGLBLENDCOLORPROC _funcptr_glBlendColor = NULL;
PFNGLBLENDEQUATIONPROC _funcptr_glBlendEquation = NULL;
PFNGLDRAWRANGEELEMENTSPROC _funcptr_glDrawRangeElements = NULL;
PFNGLTEXIMAGE3DPROC _funcptr_glTexImage3D = NULL;
PFNGLTEXSUBIMAGE3DPROC _funcptr_glTexSubImage3D = NULL;
PFNGLCOPYTEXSUBIMAGE3DPROC _funcptr_glCopyTexSubImage3D = NULL;
PFNGLCOLORTABLEPROC _funcptr_glColorTable = NULL;
PFNGLCOLORTABLEPARAMETERFVPROC _funcptr_glColorTableParameterfv = NULL;
PFNGLCOLORTABLEPARAMETERIVPROC _funcptr_glColorTableParameteriv = NULL;
PFNGLCOPYCOLORTABLEPROC _funcptr_glCopyColorTable = NULL;
PFNGLGETCOLORTABLEPROC _funcptr_glGetColorTable = NULL;
PFNGLGETCOLORTABLEPARAMETERFVPROC _funcptr_glGetColorTableParameterfv = NULL;
PFNGLGETCOLORTABLEPARAMETERIVPROC _funcptr_glGetColorTableParameteriv = NULL;
PFNGLCOLORSUBTABLEPROC _funcptr_glColorSubTable = NULL;
PFNGLCOPYCOLORSUBTABLEPROC _funcptr_glCopyColorSubTable = NULL;
PFNGLCONVOLUTIONFILTER1DPROC _funcptr_glConvolutionFilter1D = NULL;
PFNGLCONVOLUTIONFILTER2DPROC _funcptr_glConvolutionFilter2D = NULL;
PFNGLCONVOLUTIONPARAMETERFPROC _funcptr_glConvolutionParameterf = NULL;
PFNGLCONVOLUTIONPARAMETERFVPROC _funcptr_glConvolutionParameterfv = NULL;
PFNGLCONVOLUTIONPARAMETERIPROC _funcptr_glConvolutionParameteri = NULL;
PFNGLCONVOLUTIONPARAMETERIVPROC _funcptr_glConvolutionParameteriv = NULL;
PFNGLCOPYCONVOLUTIONFILTER1DPROC _funcptr_glCopyConvolutionFilter1D = NULL;
PFNGLCOPYCONVOLUTIONFILTER2DPROC _funcptr_glCopyConvolutionFilter2D = NULL;
PFNGLGETCONVOLUTIONFILTERPROC _funcptr_glGetConvolutionFilter = NULL;
PFNGLGETCONVOLUTIONPARAMETERFVPROC _funcptr_glGetConvolutionParameterfv = NULL;
PFNGLGETCONVOLUTIONPARAMETERIVPROC _funcptr_glGetConvolutionParameteriv = NULL;
PFNGLGETSEPARABLEFILTERPROC _funcptr_glGetSeparableFilter = NULL;
PFNGLSEPARABLEFILTER2DPROC _funcptr_glSeparableFilter2D = NULL;
PFNGLGETHISTOGRAMPROC _funcptr_glGetHistogram = NULL;
PFNGLGETHISTOGRAMPARAMETERFVPROC _funcptr_glGetHistogramParameterfv = NULL;
PFNGLGETHISTOGRAMPARAMETERIVPROC _funcptr_glGetHistogramParameteriv = NULL;
PFNGLGETMINMAXPROC _funcptr_glGetMinmax = NULL;
PFNGLGETMINMAXPARAMETERFVPROC _funcptr_glGetMinmaxParameterfv = NULL;
PFNGLGETMINMAXPARAMETERIVPROC _funcptr_glGetMinmaxParameteriv = NULL;
PFNGLHISTOGRAMPROC _funcptr_glHistogram = NULL;
PFNGLMINMAXPROC _funcptr_glMinmax = NULL;
PFNGLRESETHISTOGRAMPROC _funcptr_glResetHistogram = NULL;
PFNGLRESETMINMAXPROC _funcptr_glResetMinmax = NULL;
PFNGLACTIVETEXTUREPROC _funcptr_glActiveTexture = NULL;
PFNGLSAMPLECOVERAGEPROC _funcptr_glSampleCoverage = NULL;
PFNGLCOMPRESSEDTEXIMAGE3DPROC _funcptr_glCompressedTexImage3D = NULL;
PFNGLCOMPRESSEDTEXIMAGE2DPROC _funcptr_glCompressedTexImage2D = NULL;
PFNGLCOMPRESSEDTEXIMAGE1DPROC _funcptr_glCompressedTexImage1D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC _funcptr_glCompressedTexSubImage3D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC _funcptr_glCompressedTexSubImage2D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC _funcptr_glCompressedTexSubImage1D = NULL;
PFNGLGETCOMPRESSEDTEXIMAGEPROC _funcptr_glGetCompressedTexImage = NULL;
PFNGLCLIENTACTIVETEXTUREPROC _funcptr_glClientActiveTexture = NULL;
PFNGLMULTITEXCOORD1DPROC _funcptr_glMultiTexCoord1d = NULL;
PFNGLMULTITEXCOORD1DVPROC _funcptr_glMultiTexCoord1dv = NULL;
PFNGLMULTITEXCOORD1FPROC _funcptr_glMultiTexCoord1f = NULL;
PFNGLMULTITEXCOORD1FVPROC _funcptr_glMultiTexCoord1fv = NULL;
PFNGLMULTITEXCOORD1IPROC _funcptr_glMultiTexCoord1i = NULL;
PFNGLMULTITEXCOORD1IVPROC _funcptr_glMultiTexCoord1iv = NULL;
PFNGLMULTITEXCOORD1SPROC _funcptr_glMultiTexCoord1s = NULL;
PFNGLMULTITEXCOORD1SVPROC _funcptr_glMultiTexCoord1sv = NULL;
PFNGLMULTITEXCOORD2DPROC _funcptr_glMultiTexCoord2d = NULL;
PFNGLMULTITEXCOORD2DVPROC _funcptr_glMultiTexCoord2dv = NULL;
PFNGLMULTITEXCOORD2FPROC _funcptr_glMultiTexCoord2f = NULL;
PFNGLMULTITEXCOORD2FVPROC _funcptr_glMultiTexCoord2fv = NULL;
PFNGLMULTITEXCOORD2IPROC _funcptr_glMultiTexCoord2i = NULL;
PFNGLMULTITEXCOORD2IVPROC _funcptr_glMultiTexCoord2iv = NULL;
PFNGLMULTITEXCOORD2SPROC _funcptr_glMultiTexCoord2s = NULL;
PFNGLMULTITEXCOORD2SVPROC _funcptr_glMultiTexCoord2sv = NULL;
PFNGLMULTITEXCOORD3DPROC _funcptr_glMultiTexCoord3d = NULL;
PFNGLMULTITEXCOORD3DVPROC _funcptr_glMultiTexCoord3dv = NULL;
PFNGLMULTITEXCOORD3FPROC _funcptr_glMultiTexCoord3f = NULL;
PFNGLMULTITEXCOORD3FVPROC _funcptr_glMultiTexCoord3fv = NULL;
PFNGLMULTITEXCOORD3IPROC _funcptr_glMultiTexCoord3i = NULL;
PFNGLMULTITEXCOORD3IVPROC _funcptr_glMultiTexCoord3iv = NULL;
PFNGLMULTITEXCOORD3SPROC _funcptr_glMultiTexCoord3s = NULL;
PFNGLMULTITEXCOORD3SVPROC _funcptr_glMultiTexCoord3sv = NULL;
PFNGLMULTITEXCOORD4DPROC _funcptr_glMultiTexCoord4d = NULL;
PFNGLMULTITEXCOORD4DVPROC _funcptr_glMultiTexCoord4dv = NULL;
PFNGLMULTITEXCOORD4FPROC _funcptr_glMultiTexCoord4f = NULL;
PFNGLMULTITEXCOORD4FVPROC _funcptr_glMultiTexCoord4fv = NULL;
PFNGLMULTITEXCOORD4IPROC _funcptr_glMultiTexCoord4i = NULL;
PFNGLMULTITEXCOORD4IVPROC _funcptr_glMultiTexCoord4iv = NULL;
PFNGLMULTITEXCOORD4SPROC _funcptr_glMultiTexCoord4s = NULL;
PFNGLMULTITEXCOORD4SVPROC _funcptr_glMultiTexCoord4sv = NULL;
PFNGLLOADTRANSPOSEMATRIXFPROC _funcptr_glLoadTransposeMatrixf = NULL;
PFNGLLOADTRANSPOSEMATRIXDPROC _funcptr_glLoadTransposeMatrixd = NULL;
PFNGLMULTTRANSPOSEMATRIXFPROC _funcptr_glMultTransposeMatrixf = NULL;
PFNGLMULTTRANSPOSEMATRIXDPROC _funcptr_glMultTransposeMatrixd = NULL;
PFNGLBLENDFUNCSEPARATEPROC _funcptr_glBlendFuncSeparate = NULL;
PFNGLMULTIDRAWARRAYSPROC _funcptr_glMultiDrawArrays = NULL;
PFNGLMULTIDRAWELEMENTSPROC _funcptr_glMultiDrawElements = NULL;
PFNGLPOINTPARAMETERFPROC _funcptr_glPointParameterf = NULL;
PFNGLPOINTPARAMETERFVPROC _funcptr_glPointParameterfv = NULL;
PFNGLPOINTPARAMETERIPROC _funcptr_glPointParameteri = NULL;
PFNGLPOINTPARAMETERIVPROC _funcptr_glPointParameteriv = NULL;
PFNGLFOGCOORDFPROC _funcptr_glFogCoordf = NULL;
PFNGLFOGCOORDFVPROC _funcptr_glFogCoordfv = NULL;
PFNGLFOGCOORDDPROC _funcptr_glFogCoordd = NULL;
PFNGLFOGCOORDDVPROC _funcptr_glFogCoorddv = NULL;
PFNGLFOGCOORDPOINTERPROC _funcptr_glFogCoordPointer = NULL;
PFNGLSECONDARYCOLOR3BPROC _funcptr_glSecondaryColor3b = NULL;
PFNGLSECONDARYCOLOR3BVPROC _funcptr_glSecondaryColor3bv = NULL;
PFNGLSECONDARYCOLOR3DPROC _funcptr_glSecondaryColor3d = NULL;
PFNGLSECONDARYCOLOR3DVPROC _funcptr_glSecondaryColor3dv = NULL;
PFNGLSECONDARYCOLOR3FPROC _funcptr_glSecondaryColor3f = NULL;
PFNGLSECONDARYCOLOR3FVPROC _funcptr_glSecondaryColor3fv = NULL;
PFNGLSECONDARYCOLOR3IPROC _funcptr_glSecondaryColor3i = NULL;
PFNGLSECONDARYCOLOR3IVPROC _funcptr_glSecondaryColor3iv = NULL;
PFNGLSECONDARYCOLOR3SPROC _funcptr_glSecondaryColor3s = NULL;
PFNGLSECONDARYCOLOR3SVPROC _funcptr_glSecondaryColor3sv = NULL;
PFNGLSECONDARYCOLOR3UBPROC _funcptr_glSecondaryColor3ub = NULL;
PFNGLSECONDARYCOLOR3UBVPROC _funcptr_glSecondaryColor3ubv = NULL;
PFNGLSECONDARYCOLOR3UIPROC _funcptr_glSecondaryColor3ui = NULL;
PFNGLSECONDARYCOLOR3UIVPROC _funcptr_glSecondaryColor3uiv = NULL;
PFNGLSECONDARYCOLOR3USPROC _funcptr_glSecondaryColor3us = NULL;
PFNGLSECONDARYCOLOR3USVPROC _funcptr_glSecondaryColor3usv = NULL;
PFNGLSECONDARYCOLORPOINTERPROC _funcptr_glSecondaryColorPointer = NULL;
PFNGLWINDOWPOS2DPROC _funcptr_glWindowPos2d = NULL;
PFNGLWINDOWPOS2DVPROC _funcptr_glWindowPos2dv = NULL;
PFNGLWINDOWPOS2FPROC _funcptr_glWindowPos2f = NULL;
PFNGLWINDOWPOS2FVPROC _funcptr_glWindowPos2fv = NULL;
PFNGLWINDOWPOS2IPROC _funcptr_glWindowPos2i = NULL;
PFNGLWINDOWPOS2IVPROC _funcptr_glWindowPos2iv = NULL;
PFNGLWINDOWPOS2SPROC _funcptr_glWindowPos2s = NULL;
PFNGLWINDOWPOS2SVPROC _funcptr_glWindowPos2sv = NULL;
PFNGLWINDOWPOS3DPROC _funcptr_glWindowPos3d = NULL;
PFNGLWINDOWPOS3DVPROC _funcptr_glWindowPos3dv = NULL;
PFNGLWINDOWPOS3FPROC _funcptr_glWindowPos3f = NULL;
PFNGLWINDOWPOS3FVPROC _funcptr_glWindowPos3fv = NULL;
PFNGLWINDOWPOS3IPROC _funcptr_glWindowPos3i = NULL;
PFNGLWINDOWPOS3IVPROC _funcptr_glWindowPos3iv = NULL;
PFNGLWINDOWPOS3SPROC _funcptr_glWindowPos3s = NULL;
PFNGLWINDOWPOS3SVPROC _funcptr_glWindowPos3sv = NULL;
PFNGLGENQUERIESPROC _funcptr_glGenQueries = NULL;
PFNGLDELETEQUERIESPROC _funcptr_glDeleteQueries = NULL;
PFNGLISQUERYPROC _funcptr_glIsQuery = NULL;
PFNGLBEGINQUERYPROC _funcptr_glBeginQuery = NULL;
PFNGLENDQUERYPROC _funcptr_glEndQuery = NULL;
PFNGLGETQUERYIVPROC _funcptr_glGetQueryiv = NULL;
PFNGLGETQUERYOBJECTIVPROC _funcptr_glGetQueryObjectiv = NULL;
PFNGLGETQUERYOBJECTUIVPROC _funcptr_glGetQueryObjectuiv = NULL;
PFNGLBINDBUFFERPROC _funcptr_glBindBuffer = NULL;
PFNGLDELETEBUFFERSPROC _funcptr_glDeleteBuffers = NULL;
PFNGLGENBUFFERSPROC _funcptr_glGenBuffers = NULL;
PFNGLISBUFFERPROC _funcptr_glIsBuffer = NULL;
PFNGLBUFFERDATAPROC _funcptr_glBufferData = NULL;
PFNGLBUFFERSUBDATAPROC _funcptr_glBufferSubData = NULL;
PFNGLGETBUFFERSUBDATAPROC _funcptr_glGetBufferSubData = NULL;
PFNGLMAPBUFFERPROC _funcptr_glMapBuffer = NULL;
PFNGLUNMAPBUFFERPROC _funcptr_glUnmapBuffer = NULL;
PFNGLGETBUFFERPARAMETERIVPROC _funcptr_glGetBufferParameteriv = NULL;
PFNGLGETBUFFERPOINTERVPROC _funcptr_glGetBufferPointerv = NULL;
PFNGLBLENDEQUATIONSEPARATEPROC _funcptr_glBlendEquationSeparate = NULL;
PFNGLDRAWBUFFERSPROC _funcptr_glDrawBuffers = NULL;
PFNGLSTENCILOPSEPARATEPROC _funcptr_glStencilOpSeparate = NULL;
PFNGLSTENCILFUNCSEPARATEPROC _funcptr_glStencilFuncSeparate = NULL;
PFNGLSTENCILMASKSEPARATEPROC _funcptr_glStencilMaskSeparate = NULL;
PFNGLATTACHSHADERPROC _funcptr_glAttachShader = NULL;
PFNGLBINDATTRIBLOCATIONPROC _funcptr_glBindAttribLocation = NULL;
PFNGLCOMPILESHADERPROC _funcptr_glCompileShader = NULL;
PFNGLCREATEPROGRAMPROC _funcptr_glCreateProgram = NULL;
PFNGLCREATESHADERPROC _funcptr_glCreateShader = NULL;
PFNGLDELETEPROGRAMPROC _funcptr_glDeleteProgram = NULL;
PFNGLDELETESHADERPROC _funcptr_glDeleteShader = NULL;
PFNGLDETACHSHADERPROC _funcptr_glDetachShader = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC _funcptr_glDisableVertexAttribArray = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC _funcptr_glEnableVertexAttribArray = NULL;
PFNGLGETACTIVEATTRIBPROC _funcptr_glGetActiveAttrib = NULL;
PFNGLGETACTIVEUNIFORMPROC _funcptr_glGetActiveUniform = NULL;
PFNGLGETATTACHEDSHADERSPROC _funcptr_glGetAttachedShaders = NULL;
PFNGLGETATTRIBLOCATIONPROC _funcptr_glGetAttribLocation = NULL;
PFNGLGETPROGRAMIVPROC _funcptr_glGetProgramiv = NULL;
PFNGLGETPROGRAMINFOLOGPROC _funcptr_glGetProgramInfoLog = NULL;
PFNGLGETSHADERIVPROC _funcptr_glGetShaderiv = NULL;
PFNGLGETSHADERINFOLOGPROC _funcptr_glGetShaderInfoLog = NULL;
PFNGLGETSHADERSOURCEPROC _funcptr_glGetShaderSource = NULL;
PFNGLGETUNIFORMLOCATIONPROC _funcptr_glGetUniformLocation = NULL;
PFNGLGETUNIFORMFVPROC _funcptr_glGetUniformfv = NULL;
PFNGLGETUNIFORMIVPROC _funcptr_glGetUniformiv = NULL;
PFNGLGETVERTEXATTRIBDVPROC _funcptr_glGetVertexAttribdv = NULL;
PFNGLGETVERTEXATTRIBFVPROC _funcptr_glGetVertexAttribfv = NULL;
PFNGLGETVERTEXATTRIBIVPROC _funcptr_glGetVertexAttribiv = NULL;
PFNGLGETVERTEXATTRIBPOINTERVPROC _funcptr_glGetVertexAttribPointerv = NULL;
PFNGLISPROGRAMPROC _funcptr_glIsProgram = NULL;
PFNGLISSHADERPROC _funcptr_glIsShader = NULL;
PFNGLLINKPROGRAMPROC _funcptr_glLinkProgram = NULL;
PFNGLSHADERSOURCEPROC _funcptr_glShaderSource = NULL;
PFNGLUSEPROGRAMPROC _funcptr_glUseProgram = NULL;
PFNGLUNIFORM1FPROC _funcptr_glUniform1f = NULL;
PFNGLUNIFORM2FPROC _funcptr_glUniform2f = NULL;
PFNGLUNIFORM3FPROC _funcptr_glUniform3f = NULL;
PFNGLUNIFORM4FPROC _funcptr_glUniform4f = NULL;
PFNGLUNIFORM1IPROC _funcptr_glUniform1i = NULL;
PFNGLUNIFORM2IPROC _funcptr_glUniform2i = NULL;
PFNGLUNIFORM3IPROC _funcptr_glUniform3i = NULL;
PFNGLUNIFORM4IPROC _funcptr_glUniform4i = NULL;
PFNGLUNIFORM1FVPROC _funcptr_glUniform1fv = NULL;
PFNGLUNIFORM2FVPROC _funcptr_glUniform2fv = NULL;
PFNGLUNIFORM3FVPROC _funcptr_glUniform3fv = NULL;
PFNGLUNIFORM4FVPROC _funcptr_glUniform4fv = NULL;
PFNGLUNIFORM1IVPROC _funcptr_glUniform1iv = NULL;
PFNGLUNIFORM2IVPROC _funcptr_glUniform2iv = NULL;
PFNGLUNIFORM3IVPROC _funcptr_glUniform3iv = NULL;
PFNGLUNIFORM4IVPROC _funcptr_glUniform4iv = NULL;
PFNGLUNIFORMMATRIX2FVPROC _funcptr_glUniformMatrix2fv = NULL;
PFNGLUNIFORMMATRIX3FVPROC _funcptr_glUniformMatrix3fv = NULL;
PFNGLUNIFORMMATRIX4FVPROC _funcptr_glUniformMatrix4fv = NULL;
PFNGLVALIDATEPROGRAMPROC _funcptr_glValidateProgram = NULL;
PFNGLVERTEXATTRIB1DPROC _funcptr_glVertexAttrib1d = NULL;
PFNGLVERTEXATTRIB1DVPROC _funcptr_glVertexAttrib1dv = NULL;
PFNGLVERTEXATTRIB1FPROC _funcptr_glVertexAttrib1f = NULL;
PFNGLVERTEXATTRIB1FVPROC _funcptr_glVertexAttrib1fv = NULL;
PFNGLVERTEXATTRIB1SPROC _funcptr_glVertexAttrib1s = NULL;
PFNGLVERTEXATTRIB1SVPROC _funcptr_glVertexAttrib1sv = NULL;
PFNGLVERTEXATTRIB2DPROC _funcptr_glVertexAttrib2d = NULL;
PFNGLVERTEXATTRIB2DVPROC _funcptr_glVertexAttrib2dv = NULL;
PFNGLVERTEXATTRIB2FPROC _funcptr_glVertexAttrib2f = NULL;
PFNGLVERTEXATTRIB2FVPROC _funcptr_glVertexAttrib2fv = NULL;
PFNGLVERTEXATTRIB2SPROC _funcptr_glVertexAttrib2s = NULL;
PFNGLVERTEXATTRIB2SVPROC _funcptr_glVertexAttrib2sv = NULL;
PFNGLVERTEXATTRIB3DPROC _funcptr_glVertexAttrib3d = NULL;
PFNGLVERTEXATTRIB3DVPROC _funcptr_glVertexAttrib3dv = NULL;
PFNGLVERTEXATTRIB3FPROC _funcptr_glVertexAttrib3f = NULL;
PFNGLVERTEXATTRIB3FVPROC _funcptr_glVertexAttrib3fv = NULL;
PFNGLVERTEXATTRIB3SPROC _funcptr_glVertexAttrib3s = NULL;
PFNGLVERTEXATTRIB3SVPROC _funcptr_glVertexAttrib3sv = NULL;
PFNGLVERTEXATTRIB4NBVPROC _funcptr_glVertexAttrib4Nbv = NULL;
PFNGLVERTEXATTRIB4NIVPROC _funcptr_glVertexAttrib4Niv = NULL;
PFNGLVERTEXATTRIB4NSVPROC _funcptr_glVertexAttrib4Nsv = NULL;
PFNGLVERTEXATTRIB4NUBPROC _funcptr_glVertexAttrib4Nub = NULL;
PFNGLVERTEXATTRIB4NUBVPROC _funcptr_glVertexAttrib4Nubv = NULL;
PFNGLVERTEXATTRIB4NUIVPROC _funcptr_glVertexAttrib4Nuiv = NULL;
PFNGLVERTEXATTRIB4NUSVPROC _funcptr_glVertexAttrib4Nusv = NULL;
PFNGLVERTEXATTRIB4BVPROC _funcptr_glVertexAttrib4bv = NULL;
PFNGLVERTEXATTRIB4DPROC _funcptr_glVertexAttrib4d = NULL;
PFNGLVERTEXATTRIB4DVPROC _funcptr_glVertexAttrib4dv = NULL;
PFNGLVERTEXATTRIB4FPROC _funcptr_glVertexAttrib4f = NULL;
PFNGLVERTEXATTRIB4FVPROC _funcptr_glVertexAttrib4fv = NULL;
PFNGLVERTEXATTRIB4IVPROC _funcptr_glVertexAttrib4iv = NULL;
PFNGLVERTEXATTRIB4SPROC _funcptr_glVertexAttrib4s = NULL;
PFNGLVERTEXATTRIB4SVPROC _funcptr_glVertexAttrib4sv = NULL;
PFNGLVERTEXATTRIB4UBVPROC _funcptr_glVertexAttrib4ubv = NULL;
PFNGLVERTEXATTRIB4UIVPROC _funcptr_glVertexAttrib4uiv = NULL;
PFNGLVERTEXATTRIB4USVPROC _funcptr_glVertexAttrib4usv = NULL;
PFNGLVERTEXATTRIBPOINTERPROC _funcptr_glVertexAttribPointer = NULL;
PFNGLUNIFORMMATRIX2X3FVPROC _funcptr_glUniformMatrix2x3fv = NULL;
PFNGLUNIFORMMATRIX3X2FVPROC _funcptr_glUniformMatrix3x2fv = NULL;
PFNGLUNIFORMMATRIX2X4FVPROC _funcptr_glUniformMatrix2x4fv = NULL;
PFNGLUNIFORMMATRIX4X2FVPROC _funcptr_glUniformMatrix4x2fv = NULL;
PFNGLUNIFORMMATRIX3X4FVPROC _funcptr_glUniformMatrix3x4fv = NULL;
PFNGLUNIFORMMATRIX4X3FVPROC _funcptr_glUniformMatrix4x3fv = NULL;
PFNGLCOLORMASKIPROC _funcptr_glColorMaski = NULL;
PFNGLGETBOOLEANI_VPROC _funcptr_glGetBooleani_v = NULL;
PFNGLGETINTEGERI_VPROC _funcptr_glGetIntegeri_v = NULL;
PFNGLENABLEIPROC _funcptr_glEnablei = NULL;
PFNGLDISABLEIPROC _funcptr_glDisablei = NULL;
PFNGLISENABLEDIPROC _funcptr_glIsEnabledi = NULL;
PFNGLBEGINTRANSFORMFEEDBACKPROC _funcptr_glBeginTransformFeedback = NULL;
PFNGLENDTRANSFORMFEEDBACKPROC _funcptr_glEndTransformFeedback = NULL;
PFNGLBINDBUFFERRANGEPROC _funcptr_glBindBufferRange = NULL;
PFNGLBINDBUFFERBASEPROC _funcptr_glBindBufferBase = NULL;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC _funcptr_glTransformFeedbackVaryings = NULL;
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC _funcptr_glGetTransformFeedbackVarying = NULL;
PFNGLCLAMPCOLORPROC _funcptr_glClampColor = NULL;
PFNGLBEGINCONDITIONALRENDERPROC _funcptr_glBeginConditionalRender = NULL;
PFNGLENDCONDITIONALRENDERPROC _funcptr_glEndConditionalRender = NULL;
PFNGLVERTEXATTRIBIPOINTERPROC _funcptr_glVertexAttribIPointer = NULL;
PFNGLGETVERTEXATTRIBIIVPROC _funcptr_glGetVertexAttribIiv = NULL;
PFNGLGETVERTEXATTRIBIUIVPROC _funcptr_glGetVertexAttribIuiv = NULL;
PFNGLVERTEXATTRIBI1IPROC _funcptr_glVertexAttribI1i = NULL;
PFNGLVERTEXATTRIBI2IPROC _funcptr_glVertexAttribI2i = NULL;
PFNGLVERTEXATTRIBI3IPROC _funcptr_glVertexAttribI3i = NULL;
PFNGLVERTEXATTRIBI4IPROC _funcptr_glVertexAttribI4i = NULL;
PFNGLVERTEXATTRIBI1UIPROC _funcptr_glVertexAttribI1ui = NULL;
PFNGLVERTEXATTRIBI2UIPROC _funcptr_glVertexAttribI2ui = NULL;
PFNGLVERTEXATTRIBI3UIPROC _funcptr_glVertexAttribI3ui = NULL;
PFNGLVERTEXATTRIBI4UIPROC _funcptr_glVertexAttribI4ui = NULL;
PFNGLVERTEXATTRIBI1IVPROC _funcptr_glVertexAttribI1iv = NULL;
PFNGLVERTEXATTRIBI2IVPROC _funcptr_glVertexAttribI2iv = NULL;
PFNGLVERTEXATTRIBI3IVPROC _funcptr_glVertexAttribI3iv = NULL;
PFNGLVERTEXATTRIBI4IVPROC _funcptr_glVertexAttribI4iv = NULL;
PFNGLVERTEXATTRIBI1UIVPROC _funcptr_glVertexAttribI1uiv = NULL;
PFNGLVERTEXATTRIBI2UIVPROC _funcptr_glVertexAttribI2uiv = NULL;
PFNGLVERTEXATTRIBI3UIVPROC _funcptr_glVertexAttribI3uiv = NULL;
PFNGLVERTEXATTRIBI4UIVPROC _funcptr_glVertexAttribI4uiv = NULL;
PFNGLVERTEXATTRIBI4BVPROC _funcptr_glVertexAttribI4bv = NULL;
PFNGLVERTEXATTRIBI4SVPROC _funcptr_glVertexAttribI4sv = NULL;
PFNGLVERTEXATTRIBI4UBVPROC _funcptr_glVertexAttribI4ubv = NULL;
PFNGLVERTEXATTRIBI4USVPROC _funcptr_glVertexAttribI4usv = NULL;
PFNGLGETUNIFORMUIVPROC _funcptr_glGetUniformuiv = NULL;
PFNGLBINDFRAGDATALOCATIONPROC _funcptr_glBindFragDataLocation = NULL;
PFNGLGETFRAGDATALOCATIONPROC _funcptr_glGetFragDataLocation = NULL;
PFNGLUNIFORM1UIPROC _funcptr_glUniform1ui = NULL;
PFNGLUNIFORM2UIPROC _funcptr_glUniform2ui = NULL;
PFNGLUNIFORM3UIPROC _funcptr_glUniform3ui = NULL;
PFNGLUNIFORM4UIPROC _funcptr_glUniform4ui = NULL;
PFNGLUNIFORM1UIVPROC _funcptr_glUniform1uiv = NULL;
PFNGLUNIFORM2UIVPROC _funcptr_glUniform2uiv = NULL;
PFNGLUNIFORM3UIVPROC _funcptr_glUniform3uiv = NULL;
PFNGLUNIFORM4UIVPROC _funcptr_glUniform4uiv = NULL;
PFNGLTEXPARAMETERIIVPROC _funcptr_glTexParameterIiv = NULL;
PFNGLTEXPARAMETERIUIVPROC _funcptr_glTexParameterIuiv = NULL;
PFNGLGETTEXPARAMETERIIVPROC _funcptr_glGetTexParameterIiv = NULL;
PFNGLGETTEXPARAMETERIUIVPROC _funcptr_glGetTexParameterIuiv = NULL;
PFNGLCLEARBUFFERIVPROC _funcptr_glClearBufferiv = NULL;
PFNGLCLEARBUFFERUIVPROC _funcptr_glClearBufferuiv = NULL;
PFNGLCLEARBUFFERFVPROC _funcptr_glClearBufferfv = NULL;
PFNGLCLEARBUFFERFIPROC _funcptr_glClearBufferfi = NULL;
PFNGLGETSTRINGIPROC _funcptr_glGetStringi = NULL;
PFNGLDRAWARRAYSINSTANCEDPROC _funcptr_glDrawArraysInstanced = NULL;
PFNGLDRAWELEMENTSINSTANCEDPROC _funcptr_glDrawElementsInstanced = NULL;
PFNGLTEXBUFFERPROC _funcptr_glTexBuffer = NULL;
PFNGLPRIMITIVERESTARTINDEXPROC _funcptr_glPrimitiveRestartIndex = NULL;
PFNGLGETINTEGER64I_VPROC _funcptr_glGetInteger64i_v = NULL;
PFNGLGETBUFFERPARAMETERI64VPROC _funcptr_glGetBufferParameteri64v = NULL;
PFNGLFRAMEBUFFERTEXTUREPROC _funcptr_glFramebufferTexture = NULL;
PFNGLVERTEXATTRIBDIVISORPROC _funcptr_glVertexAttribDivisor = NULL;
PFNGLMINSAMPLESHADINGPROC _funcptr_glMinSampleShading = NULL;
PFNGLBLENDEQUATIONIPROC _funcptr_glBlendEquationi = NULL;
PFNGLBLENDEQUATIONSEPARATEIPROC _funcptr_glBlendEquationSeparatei = NULL;
PFNGLBLENDFUNCIPROC _funcptr_glBlendFunci = NULL;
PFNGLBLENDFUNCSEPARATEIPROC _funcptr_glBlendFuncSeparatei = NULL;

static int LoadCore_Version_1_1()
{
	int numFailed = 0;
	_funcptr_glCullFace = (PFNGLCULLFACEPROC)IntGetProcAddress("glCullFace");
	if(!_funcptr_glCullFace) ++numFailed;
	_funcptr_glFrontFace = (PFNGLFRONTFACEPROC)IntGetProcAddress("glFrontFace");
	if(!_funcptr_glFrontFace) ++numFailed;
	_funcptr_glHint = (PFNGLHINTPROC)IntGetProcAddress("glHint");
	if(!_funcptr_glHint) ++numFailed;
	_funcptr_glLineWidth = (PFNGLLINEWIDTHPROC)IntGetProcAddress("glLineWidth");
	if(!_funcptr_glLineWidth) ++numFailed;
	_funcptr_glPointSize = (PFNGLPOINTSIZEPROC)IntGetProcAddress("glPointSize");
	if(!_funcptr_glPointSize) ++numFailed;
	_funcptr_glPolygonMode = (PFNGLPOLYGONMODEPROC)IntGetProcAddress("glPolygonMode");
	if(!_funcptr_glPolygonMode) ++numFailed;
	_funcptr_glScissor = (PFNGLSCISSORPROC)IntGetProcAddress("glScissor");
	if(!_funcptr_glScissor) ++numFailed;
	_funcptr_glTexParameterf = (PFNGLTEXPARAMETERFPROC)IntGetProcAddress("glTexParameterf");
	if(!_funcptr_glTexParameterf) ++numFailed;
	_funcptr_glTexParameterfv = (PFNGLTEXPARAMETERFVPROC)IntGetProcAddress("glTexParameterfv");
	if(!_funcptr_glTexParameterfv) ++numFailed;
	_funcptr_glTexParameteri = (PFNGLTEXPARAMETERIPROC)IntGetProcAddress("glTexParameteri");
	if(!_funcptr_glTexParameteri) ++numFailed;
	_funcptr_glTexParameteriv = (PFNGLTEXPARAMETERIVPROC)IntGetProcAddress("glTexParameteriv");
	if(!_funcptr_glTexParameteriv) ++numFailed;
	_funcptr_glTexImage1D = (PFNGLTEXIMAGE1DPROC)IntGetProcAddress("glTexImage1D");
	if(!_funcptr_glTexImage1D) ++numFailed;
	_funcptr_glTexImage2D = (PFNGLTEXIMAGE2DPROC)IntGetProcAddress("glTexImage2D");
	if(!_funcptr_glTexImage2D) ++numFailed;
	_funcptr_glDrawBuffer = (PFNGLDRAWBUFFERPROC)IntGetProcAddress("glDrawBuffer");
	if(!_funcptr_glDrawBuffer) ++numFailed;
	_funcptr_glClear = (PFNGLCLEARPROC)IntGetProcAddress("glClear");
	if(!_funcptr_glClear) ++numFailed;
	_funcptr_glClearColor = (PFNGLCLEARCOLORPROC)IntGetProcAddress("glClearColor");
	if(!_funcptr_glClearColor) ++numFailed;
	_funcptr_glClearStencil = (PFNGLCLEARSTENCILPROC)IntGetProcAddress("glClearStencil");
	if(!_funcptr_glClearStencil) ++numFailed;
	_funcptr_glClearDepth = (PFNGLCLEARDEPTHPROC)IntGetProcAddress("glClearDepth");
	if(!_funcptr_glClearDepth) ++numFailed;
	_funcptr_glStencilMask = (PFNGLSTENCILMASKPROC)IntGetProcAddress("glStencilMask");
	if(!_funcptr_glStencilMask) ++numFailed;
	_funcptr_glColorMask = (PFNGLCOLORMASKPROC)IntGetProcAddress("glColorMask");
	if(!_funcptr_glColorMask) ++numFailed;
	_funcptr_glDepthMask = (PFNGLDEPTHMASKPROC)IntGetProcAddress("glDepthMask");
	if(!_funcptr_glDepthMask) ++numFailed;
	_funcptr_glDisable = (PFNGLDISABLEPROC)IntGetProcAddress("glDisable");
	if(!_funcptr_glDisable) ++numFailed;
	_funcptr_glEnable = (PFNGLENABLEPROC)IntGetProcAddress("glEnable");
	if(!_funcptr_glEnable) ++numFailed;
	_funcptr_glFinish = (PFNGLFINISHPROC)IntGetProcAddress("glFinish");
	if(!_funcptr_glFinish) ++numFailed;
	_funcptr_glFlush = (PFNGLFLUSHPROC)IntGetProcAddress("glFlush");
	if(!_funcptr_glFlush) ++numFailed;
	_funcptr_glBlendFunc = (PFNGLBLENDFUNCPROC)IntGetProcAddress("glBlendFunc");
	if(!_funcptr_glBlendFunc) ++numFailed;
	_funcptr_glLogicOp = (PFNGLLOGICOPPROC)IntGetProcAddress("glLogicOp");
	if(!_funcptr_glLogicOp) ++numFailed;
	_funcptr_glStencilFunc = (PFNGLSTENCILFUNCPROC)IntGetProcAddress("glStencilFunc");
	if(!_funcptr_glStencilFunc) ++numFailed;
	_funcptr_glStencilOp = (PFNGLSTENCILOPPROC)IntGetProcAddress("glStencilOp");
	if(!_funcptr_glStencilOp) ++numFailed;
	_funcptr_glDepthFunc = (PFNGLDEPTHFUNCPROC)IntGetProcAddress("glDepthFunc");
	if(!_funcptr_glDepthFunc) ++numFailed;
	_funcptr_glPixelStoref = (PFNGLPIXELSTOREFPROC)IntGetProcAddress("glPixelStoref");
	if(!_funcptr_glPixelStoref) ++numFailed;
	_funcptr_glPixelStorei = (PFNGLPIXELSTOREIPROC)IntGetProcAddress("glPixelStorei");
	if(!_funcptr_glPixelStorei) ++numFailed;
	_funcptr_glReadBuffer = (PFNGLREADBUFFERPROC)IntGetProcAddress("glReadBuffer");
	if(!_funcptr_glReadBuffer) ++numFailed;
	_funcptr_glReadPixels = (PFNGLREADPIXELSPROC)IntGetProcAddress("glReadPixels");
	if(!_funcptr_glReadPixels) ++numFailed;
	_funcptr_glGetBooleanv = (PFNGLGETBOOLEANVPROC)IntGetProcAddress("glGetBooleanv");
	if(!_funcptr_glGetBooleanv) ++numFailed;
	_funcptr_glGetDoublev = (PFNGLGETDOUBLEVPROC)IntGetProcAddress("glGetDoublev");
	if(!_funcptr_glGetDoublev) ++numFailed;
	_funcptr_glGetError = (PFNGLGETERRORPROC)IntGetProcAddress("glGetError");
	if(!_funcptr_glGetError) ++numFailed;
	_funcptr_glGetFloatv = (PFNGLGETFLOATVPROC)IntGetProcAddress("glGetFloatv");
	if(!_funcptr_glGetFloatv) ++numFailed;
	_funcptr_glGetIntegerv = (PFNGLGETINTEGERVPROC)IntGetProcAddress("glGetIntegerv");
	if(!_funcptr_glGetIntegerv) ++numFailed;
	_funcptr_glGetString = (PFNGLGETSTRINGPROC)IntGetProcAddress("glGetString");
	if(!_funcptr_glGetString) ++numFailed;
	_funcptr_glGetTexImage = (PFNGLGETTEXIMAGEPROC)IntGetProcAddress("glGetTexImage");
	if(!_funcptr_glGetTexImage) ++numFailed;
	_funcptr_glGetTexParameterfv = (PFNGLGETTEXPARAMETERFVPROC)IntGetProcAddress("glGetTexParameterfv");
	if(!_funcptr_glGetTexParameterfv) ++numFailed;
	_funcptr_glGetTexParameteriv = (PFNGLGETTEXPARAMETERIVPROC)IntGetProcAddress("glGetTexParameteriv");
	if(!_funcptr_glGetTexParameteriv) ++numFailed;
	_funcptr_glGetTexLevelParameterfv = (PFNGLGETTEXLEVELPARAMETERFVPROC)IntGetProcAddress("glGetTexLevelParameterfv");
	if(!_funcptr_glGetTexLevelParameterfv) ++numFailed;
	_funcptr_glGetTexLevelParameteriv = (PFNGLGETTEXLEVELPARAMETERIVPROC)IntGetProcAddress("glGetTexLevelParameteriv");
	if(!_funcptr_glGetTexLevelParameteriv) ++numFailed;
	_funcptr_glIsEnabled = (PFNGLISENABLEDPROC)IntGetProcAddress("glIsEnabled");
	if(!_funcptr_glIsEnabled) ++numFailed;
	_funcptr_glDepthRange = (PFNGLDEPTHRANGEPROC)IntGetProcAddress("glDepthRange");
	if(!_funcptr_glDepthRange) ++numFailed;
	_funcptr_glViewport = (PFNGLVIEWPORTPROC)IntGetProcAddress("glViewport");
	if(!_funcptr_glViewport) ++numFailed;
	_funcptr_glDrawArrays = (PFNGLDRAWARRAYSPROC)IntGetProcAddress("glDrawArrays");
	if(!_funcptr_glDrawArrays) ++numFailed;
	_funcptr_glDrawElements = (PFNGLDRAWELEMENTSPROC)IntGetProcAddress("glDrawElements");
	if(!_funcptr_glDrawElements) ++numFailed;
	_funcptr_glGetPointerv = (PFNGLGETPOINTERVPROC)IntGetProcAddress("glGetPointerv");
	if(!_funcptr_glGetPointerv) ++numFailed;
	_funcptr_glPolygonOffset = (PFNGLPOLYGONOFFSETPROC)IntGetProcAddress("glPolygonOffset");
	if(!_funcptr_glPolygonOffset) ++numFailed;
	_funcptr_glCopyTexImage1D = (PFNGLCOPYTEXIMAGE1DPROC)IntGetProcAddress("glCopyTexImage1D");
	if(!_funcptr_glCopyTexImage1D) ++numFailed;
	_funcptr_glCopyTexImage2D = (PFNGLCOPYTEXIMAGE2DPROC)IntGetProcAddress("glCopyTexImage2D");
	if(!_funcptr_glCopyTexImage2D) ++numFailed;
	_funcptr_glCopyTexSubImage1D = (PFNGLCOPYTEXSUBIMAGE1DPROC)IntGetProcAddress("glCopyTexSubImage1D");
	if(!_funcptr_glCopyTexSubImage1D) ++numFailed;
	_funcptr_glCopyTexSubImage2D = (PFNGLCOPYTEXSUBIMAGE2DPROC)IntGetProcAddress("glCopyTexSubImage2D");
	if(!_funcptr_glCopyTexSubImage2D) ++numFailed;
	_funcptr_glTexSubImage1D = (PFNGLTEXSUBIMAGE1DPROC)IntGetProcAddress("glTexSubImage1D");
	if(!_funcptr_glTexSubImage1D) ++numFailed;
	_funcptr_glTexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC)IntGetProcAddress("glTexSubImage2D");
	if(!_funcptr_glTexSubImage2D) ++numFailed;
	_funcptr_glBindTexture = (PFNGLBINDTEXTUREPROC)IntGetProcAddress("glBindTexture");
	if(!_funcptr_glBindTexture) ++numFailed;
	_funcptr_glDeleteTextures = (PFNGLDELETETEXTURESPROC)IntGetProcAddress("glDeleteTextures");
	if(!_funcptr_glDeleteTextures) ++numFailed;
	_funcptr_glGenTextures = (PFNGLGENTEXTURESPROC)IntGetProcAddress("glGenTextures");
	if(!_funcptr_glGenTextures) ++numFailed;
	_funcptr_glIsTexture = (PFNGLISTEXTUREPROC)IntGetProcAddress("glIsTexture");
	if(!_funcptr_glIsTexture) ++numFailed;
	_funcptr_glIndexub = (PFNGLINDEXUBPROC)IntGetProcAddress("glIndexub");
	if(!_funcptr_glIndexub) ++numFailed;
	_funcptr_glIndexubv = (PFNGLINDEXUBVPROC)IntGetProcAddress("glIndexubv");
	if(!_funcptr_glIndexubv) ++numFailed;
	return numFailed;
}

static int LoadCore_Version_1_1_Comp()
{
	int numFailed = 0;
	_funcptr_glNewList = (PFNGLNEWLISTPROC)IntGetProcAddress("glNewList");
	if(!_funcptr_glNewList) ++numFailed;
	_funcptr_glEndList = (PFNGLENDLISTPROC)IntGetProcAddress("glEndList");
	if(!_funcptr_glEndList) ++numFailed;
	_funcptr_glCallList = (PFNGLCALLLISTPROC)IntGetProcAddress("glCallList");
	if(!_funcptr_glCallList) ++numFailed;
	_funcptr_glCallLists = (PFNGLCALLLISTSPROC)IntGetProcAddress("glCallLists");
	if(!_funcptr_glCallLists) ++numFailed;
	_funcptr_glDeleteLists = (PFNGLDELETELISTSPROC)IntGetProcAddress("glDeleteLists");
	if(!_funcptr_glDeleteLists) ++numFailed;
	_funcptr_glGenLists = (PFNGLGENLISTSPROC)IntGetProcAddress("glGenLists");
	if(!_funcptr_glGenLists) ++numFailed;
	_funcptr_glListBase = (PFNGLLISTBASEPROC)IntGetProcAddress("glListBase");
	if(!_funcptr_glListBase) ++numFailed;
	_funcptr_glBegin = (PFNGLBEGINPROC)IntGetProcAddress("glBegin");
	if(!_funcptr_glBegin) ++numFailed;
	_funcptr_glBitmap = (PFNGLBITMAPPROC)IntGetProcAddress("glBitmap");
	if(!_funcptr_glBitmap) ++numFailed;
	_funcptr_glColor3b = (PFNGLCOLOR3BPROC)IntGetProcAddress("glColor3b");
	if(!_funcptr_glColor3b) ++numFailed;
	_funcptr_glColor3bv = (PFNGLCOLOR3BVPROC)IntGetProcAddress("glColor3bv");
	if(!_funcptr_glColor3bv) ++numFailed;
	_funcptr_glColor3d = (PFNGLCOLOR3DPROC)IntGetProcAddress("glColor3d");
	if(!_funcptr_glColor3d) ++numFailed;
	_funcptr_glColor3dv = (PFNGLCOLOR3DVPROC)IntGetProcAddress("glColor3dv");
	if(!_funcptr_glColor3dv) ++numFailed;
	_funcptr_glColor3f = (PFNGLCOLOR3FPROC)IntGetProcAddress("glColor3f");
	if(!_funcptr_glColor3f) ++numFailed;
	_funcptr_glColor3fv = (PFNGLCOLOR3FVPROC)IntGetProcAddress("glColor3fv");
	if(!_funcptr_glColor3fv) ++numFailed;
	_funcptr_glColor3i = (PFNGLCOLOR3IPROC)IntGetProcAddress("glColor3i");
	if(!_funcptr_glColor3i) ++numFailed;
	_funcptr_glColor3iv = (PFNGLCOLOR3IVPROC)IntGetProcAddress("glColor3iv");
	if(!_funcptr_glColor3iv) ++numFailed;
	_funcptr_glColor3s = (PFNGLCOLOR3SPROC)IntGetProcAddress("glColor3s");
	if(!_funcptr_glColor3s) ++numFailed;
	_funcptr_glColor3sv = (PFNGLCOLOR3SVPROC)IntGetProcAddress("glColor3sv");
	if(!_funcptr_glColor3sv) ++numFailed;
	_funcptr_glColor3ub = (PFNGLCOLOR3UBPROC)IntGetProcAddress("glColor3ub");
	if(!_funcptr_glColor3ub) ++numFailed;
	_funcptr_glColor3ubv = (PFNGLCOLOR3UBVPROC)IntGetProcAddress("glColor3ubv");
	if(!_funcptr_glColor3ubv) ++numFailed;
	_funcptr_glColor3ui = (PFNGLCOLOR3UIPROC)IntGetProcAddress("glColor3ui");
	if(!_funcptr_glColor3ui) ++numFailed;
	_funcptr_glColor3uiv = (PFNGLCOLOR3UIVPROC)IntGetProcAddress("glColor3uiv");
	if(!_funcptr_glColor3uiv) ++numFailed;
	_funcptr_glColor3us = (PFNGLCOLOR3USPROC)IntGetProcAddress("glColor3us");
	if(!_funcptr_glColor3us) ++numFailed;
	_funcptr_glColor3usv = (PFNGLCOLOR3USVPROC)IntGetProcAddress("glColor3usv");
	if(!_funcptr_glColor3usv) ++numFailed;
	_funcptr_glColor4b = (PFNGLCOLOR4BPROC)IntGetProcAddress("glColor4b");
	if(!_funcptr_glColor4b) ++numFailed;
	_funcptr_glColor4bv = (PFNGLCOLOR4BVPROC)IntGetProcAddress("glColor4bv");
	if(!_funcptr_glColor4bv) ++numFailed;
	_funcptr_glColor4d = (PFNGLCOLOR4DPROC)IntGetProcAddress("glColor4d");
	if(!_funcptr_glColor4d) ++numFailed;
	_funcptr_glColor4dv = (PFNGLCOLOR4DVPROC)IntGetProcAddress("glColor4dv");
	if(!_funcptr_glColor4dv) ++numFailed;
	_funcptr_glColor4f = (PFNGLCOLOR4FPROC)IntGetProcAddress("glColor4f");
	if(!_funcptr_glColor4f) ++numFailed;
	_funcptr_glColor4fv = (PFNGLCOLOR4FVPROC)IntGetProcAddress("glColor4fv");
	if(!_funcptr_glColor4fv) ++numFailed;
	_funcptr_glColor4i = (PFNGLCOLOR4IPROC)IntGetProcAddress("glColor4i");
	if(!_funcptr_glColor4i) ++numFailed;
	_funcptr_glColor4iv = (PFNGLCOLOR4IVPROC)IntGetProcAddress("glColor4iv");
	if(!_funcptr_glColor4iv) ++numFailed;
	_funcptr_glColor4s = (PFNGLCOLOR4SPROC)IntGetProcAddress("glColor4s");
	if(!_funcptr_glColor4s) ++numFailed;
	_funcptr_glColor4sv = (PFNGLCOLOR4SVPROC)IntGetProcAddress("glColor4sv");
	if(!_funcptr_glColor4sv) ++numFailed;
	_funcptr_glColor4ub = (PFNGLCOLOR4UBPROC)IntGetProcAddress("glColor4ub");
	if(!_funcptr_glColor4ub) ++numFailed;
	_funcptr_glColor4ubv = (PFNGLCOLOR4UBVPROC)IntGetProcAddress("glColor4ubv");
	if(!_funcptr_glColor4ubv) ++numFailed;
	_funcptr_glColor4ui = (PFNGLCOLOR4UIPROC)IntGetProcAddress("glColor4ui");
	if(!_funcptr_glColor4ui) ++numFailed;
	_funcptr_glColor4uiv = (PFNGLCOLOR4UIVPROC)IntGetProcAddress("glColor4uiv");
	if(!_funcptr_glColor4uiv) ++numFailed;
	_funcptr_glColor4us = (PFNGLCOLOR4USPROC)IntGetProcAddress("glColor4us");
	if(!_funcptr_glColor4us) ++numFailed;
	_funcptr_glColor4usv = (PFNGLCOLOR4USVPROC)IntGetProcAddress("glColor4usv");
	if(!_funcptr_glColor4usv) ++numFailed;
	_funcptr_glEdgeFlag = (PFNGLEDGEFLAGPROC)IntGetProcAddress("glEdgeFlag");
	if(!_funcptr_glEdgeFlag) ++numFailed;
	_funcptr_glEdgeFlagv = (PFNGLEDGEFLAGVPROC)IntGetProcAddress("glEdgeFlagv");
	if(!_funcptr_glEdgeFlagv) ++numFailed;
	_funcptr_glEnd = (PFNGLENDPROC)IntGetProcAddress("glEnd");
	if(!_funcptr_glEnd) ++numFailed;
	_funcptr_glIndexd = (PFNGLINDEXDPROC)IntGetProcAddress("glIndexd");
	if(!_funcptr_glIndexd) ++numFailed;
	_funcptr_glIndexdv = (PFNGLINDEXDVPROC)IntGetProcAddress("glIndexdv");
	if(!_funcptr_glIndexdv) ++numFailed;
	_funcptr_glIndexf = (PFNGLINDEXFPROC)IntGetProcAddress("glIndexf");
	if(!_funcptr_glIndexf) ++numFailed;
	_funcptr_glIndexfv = (PFNGLINDEXFVPROC)IntGetProcAddress("glIndexfv");
	if(!_funcptr_glIndexfv) ++numFailed;
	_funcptr_glIndexi = (PFNGLINDEXIPROC)IntGetProcAddress("glIndexi");
	if(!_funcptr_glIndexi) ++numFailed;
	_funcptr_glIndexiv = (PFNGLINDEXIVPROC)IntGetProcAddress("glIndexiv");
	if(!_funcptr_glIndexiv) ++numFailed;
	_funcptr_glIndexs = (PFNGLINDEXSPROC)IntGetProcAddress("glIndexs");
	if(!_funcptr_glIndexs) ++numFailed;
	_funcptr_glIndexsv = (PFNGLINDEXSVPROC)IntGetProcAddress("glIndexsv");
	if(!_funcptr_glIndexsv) ++numFailed;
	_funcptr_glNormal3b = (PFNGLNORMAL3BPROC)IntGetProcAddress("glNormal3b");
	if(!_funcptr_glNormal3b) ++numFailed;
	_funcptr_glNormal3bv = (PFNGLNORMAL3BVPROC)IntGetProcAddress("glNormal3bv");
	if(!_funcptr_glNormal3bv) ++numFailed;
	_funcptr_glNormal3d = (PFNGLNORMAL3DPROC)IntGetProcAddress("glNormal3d");
	if(!_funcptr_glNormal3d) ++numFailed;
	_funcptr_glNormal3dv = (PFNGLNORMAL3DVPROC)IntGetProcAddress("glNormal3dv");
	if(!_funcptr_glNormal3dv) ++numFailed;
	_funcptr_glNormal3f = (PFNGLNORMAL3FPROC)IntGetProcAddress("glNormal3f");
	if(!_funcptr_glNormal3f) ++numFailed;
	_funcptr_glNormal3fv = (PFNGLNORMAL3FVPROC)IntGetProcAddress("glNormal3fv");
	if(!_funcptr_glNormal3fv) ++numFailed;
	_funcptr_glNormal3i = (PFNGLNORMAL3IPROC)IntGetProcAddress("glNormal3i");
	if(!_funcptr_glNormal3i) ++numFailed;
	_funcptr_glNormal3iv = (PFNGLNORMAL3IVPROC)IntGetProcAddress("glNormal3iv");
	if(!_funcptr_glNormal3iv) ++numFailed;
	_funcptr_glNormal3s = (PFNGLNORMAL3SPROC)IntGetProcAddress("glNormal3s");
	if(!_funcptr_glNormal3s) ++numFailed;
	_funcptr_glNormal3sv = (PFNGLNORMAL3SVPROC)IntGetProcAddress("glNormal3sv");
	if(!_funcptr_glNormal3sv) ++numFailed;
	_funcptr_glRasterPos2d = (PFNGLRASTERPOS2DPROC)IntGetProcAddress("glRasterPos2d");
	if(!_funcptr_glRasterPos2d) ++numFailed;
	_funcptr_glRasterPos2dv = (PFNGLRASTERPOS2DVPROC)IntGetProcAddress("glRasterPos2dv");
	if(!_funcptr_glRasterPos2dv) ++numFailed;
	_funcptr_glRasterPos2f = (PFNGLRASTERPOS2FPROC)IntGetProcAddress("glRasterPos2f");
	if(!_funcptr_glRasterPos2f) ++numFailed;
	_funcptr_glRasterPos2fv = (PFNGLRASTERPOS2FVPROC)IntGetProcAddress("glRasterPos2fv");
	if(!_funcptr_glRasterPos2fv) ++numFailed;
	_funcptr_glRasterPos2i = (PFNGLRASTERPOS2IPROC)IntGetProcAddress("glRasterPos2i");
	if(!_funcptr_glRasterPos2i) ++numFailed;
	_funcptr_glRasterPos2iv = (PFNGLRASTERPOS2IVPROC)IntGetProcAddress("glRasterPos2iv");
	if(!_funcptr_glRasterPos2iv) ++numFailed;
	_funcptr_glRasterPos2s = (PFNGLRASTERPOS2SPROC)IntGetProcAddress("glRasterPos2s");
	if(!_funcptr_glRasterPos2s) ++numFailed;
	_funcptr_glRasterPos2sv = (PFNGLRASTERPOS2SVPROC)IntGetProcAddress("glRasterPos2sv");
	if(!_funcptr_glRasterPos2sv) ++numFailed;
	_funcptr_glRasterPos3d = (PFNGLRASTERPOS3DPROC)IntGetProcAddress("glRasterPos3d");
	if(!_funcptr_glRasterPos3d) ++numFailed;
	_funcptr_glRasterPos3dv = (PFNGLRASTERPOS3DVPROC)IntGetProcAddress("glRasterPos3dv");
	if(!_funcptr_glRasterPos3dv) ++numFailed;
	_funcptr_glRasterPos3f = (PFNGLRASTERPOS3FPROC)IntGetProcAddress("glRasterPos3f");
	if(!_funcptr_glRasterPos3f) ++numFailed;
	_funcptr_glRasterPos3fv = (PFNGLRASTERPOS3FVPROC)IntGetProcAddress("glRasterPos3fv");
	if(!_funcptr_glRasterPos3fv) ++numFailed;
	_funcptr_glRasterPos3i = (PFNGLRASTERPOS3IPROC)IntGetProcAddress("glRasterPos3i");
	if(!_funcptr_glRasterPos3i) ++numFailed;
	_funcptr_glRasterPos3iv = (PFNGLRASTERPOS3IVPROC)IntGetProcAddress("glRasterPos3iv");
	if(!_funcptr_glRasterPos3iv) ++numFailed;
	_funcptr_glRasterPos3s = (PFNGLRASTERPOS3SPROC)IntGetProcAddress("glRasterPos3s");
	if(!_funcptr_glRasterPos3s) ++numFailed;
	_funcptr_glRasterPos3sv = (PFNGLRASTERPOS3SVPROC)IntGetProcAddress("glRasterPos3sv");
	if(!_funcptr_glRasterPos3sv) ++numFailed;
	_funcptr_glRasterPos4d = (PFNGLRASTERPOS4DPROC)IntGetProcAddress("glRasterPos4d");
	if(!_funcptr_glRasterPos4d) ++numFailed;
	_funcptr_glRasterPos4dv = (PFNGLRASTERPOS4DVPROC)IntGetProcAddress("glRasterPos4dv");
	if(!_funcptr_glRasterPos4dv) ++numFailed;
	_funcptr_glRasterPos4f = (PFNGLRASTERPOS4FPROC)IntGetProcAddress("glRasterPos4f");
	if(!_funcptr_glRasterPos4f) ++numFailed;
	_funcptr_glRasterPos4fv = (PFNGLRASTERPOS4FVPROC)IntGetProcAddress("glRasterPos4fv");
	if(!_funcptr_glRasterPos4fv) ++numFailed;
	_funcptr_glRasterPos4i = (PFNGLRASTERPOS4IPROC)IntGetProcAddress("glRasterPos4i");
	if(!_funcptr_glRasterPos4i) ++numFailed;
	_funcptr_glRasterPos4iv = (PFNGLRASTERPOS4IVPROC)IntGetProcAddress("glRasterPos4iv");
	if(!_funcptr_glRasterPos4iv) ++numFailed;
	_funcptr_glRasterPos4s = (PFNGLRASTERPOS4SPROC)IntGetProcAddress("glRasterPos4s");
	if(!_funcptr_glRasterPos4s) ++numFailed;
	_funcptr_glRasterPos4sv = (PFNGLRASTERPOS4SVPROC)IntGetProcAddress("glRasterPos4sv");
	if(!_funcptr_glRasterPos4sv) ++numFailed;
	_funcptr_glRectd = (PFNGLRECTDPROC)IntGetProcAddress("glRectd");
	if(!_funcptr_glRectd) ++numFailed;
	_funcptr_glRectdv = (PFNGLRECTDVPROC)IntGetProcAddress("glRectdv");
	if(!_funcptr_glRectdv) ++numFailed;
	_funcptr_glRectf = (PFNGLRECTFPROC)IntGetProcAddress("glRectf");
	if(!_funcptr_glRectf) ++numFailed;
	_funcptr_glRectfv = (PFNGLRECTFVPROC)IntGetProcAddress("glRectfv");
	if(!_funcptr_glRectfv) ++numFailed;
	_funcptr_glRecti = (PFNGLRECTIPROC)IntGetProcAddress("glRecti");
	if(!_funcptr_glRecti) ++numFailed;
	_funcptr_glRectiv = (PFNGLRECTIVPROC)IntGetProcAddress("glRectiv");
	if(!_funcptr_glRectiv) ++numFailed;
	_funcptr_glRects = (PFNGLRECTSPROC)IntGetProcAddress("glRects");
	if(!_funcptr_glRects) ++numFailed;
	_funcptr_glRectsv = (PFNGLRECTSVPROC)IntGetProcAddress("glRectsv");
	if(!_funcptr_glRectsv) ++numFailed;
	_funcptr_glTexCoord1d = (PFNGLTEXCOORD1DPROC)IntGetProcAddress("glTexCoord1d");
	if(!_funcptr_glTexCoord1d) ++numFailed;
	_funcptr_glTexCoord1dv = (PFNGLTEXCOORD1DVPROC)IntGetProcAddress("glTexCoord1dv");
	if(!_funcptr_glTexCoord1dv) ++numFailed;
	_funcptr_glTexCoord1f = (PFNGLTEXCOORD1FPROC)IntGetProcAddress("glTexCoord1f");
	if(!_funcptr_glTexCoord1f) ++numFailed;
	_funcptr_glTexCoord1fv = (PFNGLTEXCOORD1FVPROC)IntGetProcAddress("glTexCoord1fv");
	if(!_funcptr_glTexCoord1fv) ++numFailed;
	_funcptr_glTexCoord1i = (PFNGLTEXCOORD1IPROC)IntGetProcAddress("glTexCoord1i");
	if(!_funcptr_glTexCoord1i) ++numFailed;
	_funcptr_glTexCoord1iv = (PFNGLTEXCOORD1IVPROC)IntGetProcAddress("glTexCoord1iv");
	if(!_funcptr_glTexCoord1iv) ++numFailed;
	_funcptr_glTexCoord1s = (PFNGLTEXCOORD1SPROC)IntGetProcAddress("glTexCoord1s");
	if(!_funcptr_glTexCoord1s) ++numFailed;
	_funcptr_glTexCoord1sv = (PFNGLTEXCOORD1SVPROC)IntGetProcAddress("glTexCoord1sv");
	if(!_funcptr_glTexCoord1sv) ++numFailed;
	_funcptr_glTexCoord2d = (PFNGLTEXCOORD2DPROC)IntGetProcAddress("glTexCoord2d");
	if(!_funcptr_glTexCoord2d) ++numFailed;
	_funcptr_glTexCoord2dv = (PFNGLTEXCOORD2DVPROC)IntGetProcAddress("glTexCoord2dv");
	if(!_funcptr_glTexCoord2dv) ++numFailed;
	_funcptr_glTexCoord2f = (PFNGLTEXCOORD2FPROC)IntGetProcAddress("glTexCoord2f");
	if(!_funcptr_glTexCoord2f) ++numFailed;
	_funcptr_glTexCoord2fv = (PFNGLTEXCOORD2FVPROC)IntGetProcAddress("glTexCoord2fv");
	if(!_funcptr_glTexCoord2fv) ++numFailed;
	_funcptr_glTexCoord2i = (PFNGLTEXCOORD2IPROC)IntGetProcAddress("glTexCoord2i");
	if(!_funcptr_glTexCoord2i) ++numFailed;
	_funcptr_glTexCoord2iv = (PFNGLTEXCOORD2IVPROC)IntGetProcAddress("glTexCoord2iv");
	if(!_funcptr_glTexCoord2iv) ++numFailed;
	_funcptr_glTexCoord2s = (PFNGLTEXCOORD2SPROC)IntGetProcAddress("glTexCoord2s");
	if(!_funcptr_glTexCoord2s) ++numFailed;
	_funcptr_glTexCoord2sv = (PFNGLTEXCOORD2SVPROC)IntGetProcAddress("glTexCoord2sv");
	if(!_funcptr_glTexCoord2sv) ++numFailed;
	_funcptr_glTexCoord3d = (PFNGLTEXCOORD3DPROC)IntGetProcAddress("glTexCoord3d");
	if(!_funcptr_glTexCoord3d) ++numFailed;
	_funcptr_glTexCoord3dv = (PFNGLTEXCOORD3DVPROC)IntGetProcAddress("glTexCoord3dv");
	if(!_funcptr_glTexCoord3dv) ++numFailed;
	_funcptr_glTexCoord3f = (PFNGLTEXCOORD3FPROC)IntGetProcAddress("glTexCoord3f");
	if(!_funcptr_glTexCoord3f) ++numFailed;
	_funcptr_glTexCoord3fv = (PFNGLTEXCOORD3FVPROC)IntGetProcAddress("glTexCoord3fv");
	if(!_funcptr_glTexCoord3fv) ++numFailed;
	_funcptr_glTexCoord3i = (PFNGLTEXCOORD3IPROC)IntGetProcAddress("glTexCoord3i");
	if(!_funcptr_glTexCoord3i) ++numFailed;
	_funcptr_glTexCoord3iv = (PFNGLTEXCOORD3IVPROC)IntGetProcAddress("glTexCoord3iv");
	if(!_funcptr_glTexCoord3iv) ++numFailed;
	_funcptr_glTexCoord3s = (PFNGLTEXCOORD3SPROC)IntGetProcAddress("glTexCoord3s");
	if(!_funcptr_glTexCoord3s) ++numFailed;
	_funcptr_glTexCoord3sv = (PFNGLTEXCOORD3SVPROC)IntGetProcAddress("glTexCoord3sv");
	if(!_funcptr_glTexCoord3sv) ++numFailed;
	_funcptr_glTexCoord4d = (PFNGLTEXCOORD4DPROC)IntGetProcAddress("glTexCoord4d");
	if(!_funcptr_glTexCoord4d) ++numFailed;
	_funcptr_glTexCoord4dv = (PFNGLTEXCOORD4DVPROC)IntGetProcAddress("glTexCoord4dv");
	if(!_funcptr_glTexCoord4dv) ++numFailed;
	_funcptr_glTexCoord4f = (PFNGLTEXCOORD4FPROC)IntGetProcAddress("glTexCoord4f");
	if(!_funcptr_glTexCoord4f) ++numFailed;
	_funcptr_glTexCoord4fv = (PFNGLTEXCOORD4FVPROC)IntGetProcAddress("glTexCoord4fv");
	if(!_funcptr_glTexCoord4fv) ++numFailed;
	_funcptr_glTexCoord4i = (PFNGLTEXCOORD4IPROC)IntGetProcAddress("glTexCoord4i");
	if(!_funcptr_glTexCoord4i) ++numFailed;
	_funcptr_glTexCoord4iv = (PFNGLTEXCOORD4IVPROC)IntGetProcAddress("glTexCoord4iv");
	if(!_funcptr_glTexCoord4iv) ++numFailed;
	_funcptr_glTexCoord4s = (PFNGLTEXCOORD4SPROC)IntGetProcAddress("glTexCoord4s");
	if(!_funcptr_glTexCoord4s) ++numFailed;
	_funcptr_glTexCoord4sv = (PFNGLTEXCOORD4SVPROC)IntGetProcAddress("glTexCoord4sv");
	if(!_funcptr_glTexCoord4sv) ++numFailed;
	_funcptr_glVertex2d = (PFNGLVERTEX2DPROC)IntGetProcAddress("glVertex2d");
	if(!_funcptr_glVertex2d) ++numFailed;
	_funcptr_glVertex2dv = (PFNGLVERTEX2DVPROC)IntGetProcAddress("glVertex2dv");
	if(!_funcptr_glVertex2dv) ++numFailed;
	_funcptr_glVertex2f = (PFNGLVERTEX2FPROC)IntGetProcAddress("glVertex2f");
	if(!_funcptr_glVertex2f) ++numFailed;
	_funcptr_glVertex2fv = (PFNGLVERTEX2FVPROC)IntGetProcAddress("glVertex2fv");
	if(!_funcptr_glVertex2fv) ++numFailed;
	_funcptr_glVertex2i = (PFNGLVERTEX2IPROC)IntGetProcAddress("glVertex2i");
	if(!_funcptr_glVertex2i) ++numFailed;
	_funcptr_glVertex2iv = (PFNGLVERTEX2IVPROC)IntGetProcAddress("glVertex2iv");
	if(!_funcptr_glVertex2iv) ++numFailed;
	_funcptr_glVertex2s = (PFNGLVERTEX2SPROC)IntGetProcAddress("glVertex2s");
	if(!_funcptr_glVertex2s) ++numFailed;
	_funcptr_glVertex2sv = (PFNGLVERTEX2SVPROC)IntGetProcAddress("glVertex2sv");
	if(!_funcptr_glVertex2sv) ++numFailed;
	_funcptr_glVertex3d = (PFNGLVERTEX3DPROC)IntGetProcAddress("glVertex3d");
	if(!_funcptr_glVertex3d) ++numFailed;
	_funcptr_glVertex3dv = (PFNGLVERTEX3DVPROC)IntGetProcAddress("glVertex3dv");
	if(!_funcptr_glVertex3dv) ++numFailed;
	_funcptr_glVertex3f = (PFNGLVERTEX3FPROC)IntGetProcAddress("glVertex3f");
	if(!_funcptr_glVertex3f) ++numFailed;
	_funcptr_glVertex3fv = (PFNGLVERTEX3FVPROC)IntGetProcAddress("glVertex3fv");
	if(!_funcptr_glVertex3fv) ++numFailed;
	_funcptr_glVertex3i = (PFNGLVERTEX3IPROC)IntGetProcAddress("glVertex3i");
	if(!_funcptr_glVertex3i) ++numFailed;
	_funcptr_glVertex3iv = (PFNGLVERTEX3IVPROC)IntGetProcAddress("glVertex3iv");
	if(!_funcptr_glVertex3iv) ++numFailed;
	_funcptr_glVertex3s = (PFNGLVERTEX3SPROC)IntGetProcAddress("glVertex3s");
	if(!_funcptr_glVertex3s) ++numFailed;
	_funcptr_glVertex3sv = (PFNGLVERTEX3SVPROC)IntGetProcAddress("glVertex3sv");
	if(!_funcptr_glVertex3sv) ++numFailed;
	_funcptr_glVertex4d = (PFNGLVERTEX4DPROC)IntGetProcAddress("glVertex4d");
	if(!_funcptr_glVertex4d) ++numFailed;
	_funcptr_glVertex4dv = (PFNGLVERTEX4DVPROC)IntGetProcAddress("glVertex4dv");
	if(!_funcptr_glVertex4dv) ++numFailed;
	_funcptr_glVertex4f = (PFNGLVERTEX4FPROC)IntGetProcAddress("glVertex4f");
	if(!_funcptr_glVertex4f) ++numFailed;
	_funcptr_glVertex4fv = (PFNGLVERTEX4FVPROC)IntGetProcAddress("glVertex4fv");
	if(!_funcptr_glVertex4fv) ++numFailed;
	_funcptr_glVertex4i = (PFNGLVERTEX4IPROC)IntGetProcAddress("glVertex4i");
	if(!_funcptr_glVertex4i) ++numFailed;
	_funcptr_glVertex4iv = (PFNGLVERTEX4IVPROC)IntGetProcAddress("glVertex4iv");
	if(!_funcptr_glVertex4iv) ++numFailed;
	_funcptr_glVertex4s = (PFNGLVERTEX4SPROC)IntGetProcAddress("glVertex4s");
	if(!_funcptr_glVertex4s) ++numFailed;
	_funcptr_glVertex4sv = (PFNGLVERTEX4SVPROC)IntGetProcAddress("glVertex4sv");
	if(!_funcptr_glVertex4sv) ++numFailed;
	_funcptr_glClipPlane = (PFNGLCLIPPLANEPROC)IntGetProcAddress("glClipPlane");
	if(!_funcptr_glClipPlane) ++numFailed;
	_funcptr_glColorMaterial = (PFNGLCOLORMATERIALPROC)IntGetProcAddress("glColorMaterial");
	if(!_funcptr_glColorMaterial) ++numFailed;
	_funcptr_glFogf = (PFNGLFOGFPROC)IntGetProcAddress("glFogf");
	if(!_funcptr_glFogf) ++numFailed;
	_funcptr_glFogfv = (PFNGLFOGFVPROC)IntGetProcAddress("glFogfv");
	if(!_funcptr_glFogfv) ++numFailed;
	_funcptr_glFogi = (PFNGLFOGIPROC)IntGetProcAddress("glFogi");
	if(!_funcptr_glFogi) ++numFailed;
	_funcptr_glFogiv = (PFNGLFOGIVPROC)IntGetProcAddress("glFogiv");
	if(!_funcptr_glFogiv) ++numFailed;
	_funcptr_glLightf = (PFNGLLIGHTFPROC)IntGetProcAddress("glLightf");
	if(!_funcptr_glLightf) ++numFailed;
	_funcptr_glLightfv = (PFNGLLIGHTFVPROC)IntGetProcAddress("glLightfv");
	if(!_funcptr_glLightfv) ++numFailed;
	_funcptr_glLighti = (PFNGLLIGHTIPROC)IntGetProcAddress("glLighti");
	if(!_funcptr_glLighti) ++numFailed;
	_funcptr_glLightiv = (PFNGLLIGHTIVPROC)IntGetProcAddress("glLightiv");
	if(!_funcptr_glLightiv) ++numFailed;
	_funcptr_glLightModelf = (PFNGLLIGHTMODELFPROC)IntGetProcAddress("glLightModelf");
	if(!_funcptr_glLightModelf) ++numFailed;
	_funcptr_glLightModelfv = (PFNGLLIGHTMODELFVPROC)IntGetProcAddress("glLightModelfv");
	if(!_funcptr_glLightModelfv) ++numFailed;
	_funcptr_glLightModeli = (PFNGLLIGHTMODELIPROC)IntGetProcAddress("glLightModeli");
	if(!_funcptr_glLightModeli) ++numFailed;
	_funcptr_glLightModeliv = (PFNGLLIGHTMODELIVPROC)IntGetProcAddress("glLightModeliv");
	if(!_funcptr_glLightModeliv) ++numFailed;
	_funcptr_glLineStipple = (PFNGLLINESTIPPLEPROC)IntGetProcAddress("glLineStipple");
	if(!_funcptr_glLineStipple) ++numFailed;
	_funcptr_glMaterialf = (PFNGLMATERIALFPROC)IntGetProcAddress("glMaterialf");
	if(!_funcptr_glMaterialf) ++numFailed;
	_funcptr_glMaterialfv = (PFNGLMATERIALFVPROC)IntGetProcAddress("glMaterialfv");
	if(!_funcptr_glMaterialfv) ++numFailed;
	_funcptr_glMateriali = (PFNGLMATERIALIPROC)IntGetProcAddress("glMateriali");
	if(!_funcptr_glMateriali) ++numFailed;
	_funcptr_glMaterialiv = (PFNGLMATERIALIVPROC)IntGetProcAddress("glMaterialiv");
	if(!_funcptr_glMaterialiv) ++numFailed;
	_funcptr_glPolygonStipple = (PFNGLPOLYGONSTIPPLEPROC)IntGetProcAddress("glPolygonStipple");
	if(!_funcptr_glPolygonStipple) ++numFailed;
	_funcptr_glShadeModel = (PFNGLSHADEMODELPROC)IntGetProcAddress("glShadeModel");
	if(!_funcptr_glShadeModel) ++numFailed;
	_funcptr_glTexEnvf = (PFNGLTEXENVFPROC)IntGetProcAddress("glTexEnvf");
	if(!_funcptr_glTexEnvf) ++numFailed;
	_funcptr_glTexEnvfv = (PFNGLTEXENVFVPROC)IntGetProcAddress("glTexEnvfv");
	if(!_funcptr_glTexEnvfv) ++numFailed;
	_funcptr_glTexEnvi = (PFNGLTEXENVIPROC)IntGetProcAddress("glTexEnvi");
	if(!_funcptr_glTexEnvi) ++numFailed;
	_funcptr_glTexEnviv = (PFNGLTEXENVIVPROC)IntGetProcAddress("glTexEnviv");
	if(!_funcptr_glTexEnviv) ++numFailed;
	_funcptr_glTexGend = (PFNGLTEXGENDPROC)IntGetProcAddress("glTexGend");
	if(!_funcptr_glTexGend) ++numFailed;
	_funcptr_glTexGendv = (PFNGLTEXGENDVPROC)IntGetProcAddress("glTexGendv");
	if(!_funcptr_glTexGendv) ++numFailed;
	_funcptr_glTexGenf = (PFNGLTEXGENFPROC)IntGetProcAddress("glTexGenf");
	if(!_funcptr_glTexGenf) ++numFailed;
	_funcptr_glTexGenfv = (PFNGLTEXGENFVPROC)IntGetProcAddress("glTexGenfv");
	if(!_funcptr_glTexGenfv) ++numFailed;
	_funcptr_glTexGeni = (PFNGLTEXGENIPROC)IntGetProcAddress("glTexGeni");
	if(!_funcptr_glTexGeni) ++numFailed;
	_funcptr_glTexGeniv = (PFNGLTEXGENIVPROC)IntGetProcAddress("glTexGeniv");
	if(!_funcptr_glTexGeniv) ++numFailed;
	_funcptr_glFeedbackBuffer = (PFNGLFEEDBACKBUFFERPROC)IntGetProcAddress("glFeedbackBuffer");
	if(!_funcptr_glFeedbackBuffer) ++numFailed;
	_funcptr_glSelectBuffer = (PFNGLSELECTBUFFERPROC)IntGetProcAddress("glSelectBuffer");
	if(!_funcptr_glSelectBuffer) ++numFailed;
	_funcptr_glRenderMode = (PFNGLRENDERMODEPROC)IntGetProcAddress("glRenderMode");
	if(!_funcptr_glRenderMode) ++numFailed;
	_funcptr_glInitNames = (PFNGLINITNAMESPROC)IntGetProcAddress("glInitNames");
	if(!_funcptr_glInitNames) ++numFailed;
	_funcptr_glLoadName = (PFNGLLOADNAMEPROC)IntGetProcAddress("glLoadName");
	if(!_funcptr_glLoadName) ++numFailed;
	_funcptr_glPassThrough = (PFNGLPASSTHROUGHPROC)IntGetProcAddress("glPassThrough");
	if(!_funcptr_glPassThrough) ++numFailed;
	_funcptr_glPopName = (PFNGLPOPNAMEPROC)IntGetProcAddress("glPopName");
	if(!_funcptr_glPopName) ++numFailed;
	_funcptr_glPushName = (PFNGLPUSHNAMEPROC)IntGetProcAddress("glPushName");
	if(!_funcptr_glPushName) ++numFailed;
	_funcptr_glClearAccum = (PFNGLCLEARACCUMPROC)IntGetProcAddress("glClearAccum");
	if(!_funcptr_glClearAccum) ++numFailed;
	_funcptr_glClearIndex = (PFNGLCLEARINDEXPROC)IntGetProcAddress("glClearIndex");
	if(!_funcptr_glClearIndex) ++numFailed;
	_funcptr_glIndexMask = (PFNGLINDEXMASKPROC)IntGetProcAddress("glIndexMask");
	if(!_funcptr_glIndexMask) ++numFailed;
	_funcptr_glAccum = (PFNGLACCUMPROC)IntGetProcAddress("glAccum");
	if(!_funcptr_glAccum) ++numFailed;
	_funcptr_glPopAttrib = (PFNGLPOPATTRIBPROC)IntGetProcAddress("glPopAttrib");
	if(!_funcptr_glPopAttrib) ++numFailed;
	_funcptr_glPushAttrib = (PFNGLPUSHATTRIBPROC)IntGetProcAddress("glPushAttrib");
	if(!_funcptr_glPushAttrib) ++numFailed;
	_funcptr_glMap1d = (PFNGLMAP1DPROC)IntGetProcAddress("glMap1d");
	if(!_funcptr_glMap1d) ++numFailed;
	_funcptr_glMap1f = (PFNGLMAP1FPROC)IntGetProcAddress("glMap1f");
	if(!_funcptr_glMap1f) ++numFailed;
	_funcptr_glMap2d = (PFNGLMAP2DPROC)IntGetProcAddress("glMap2d");
	if(!_funcptr_glMap2d) ++numFailed;
	_funcptr_glMap2f = (PFNGLMAP2FPROC)IntGetProcAddress("glMap2f");
	if(!_funcptr_glMap2f) ++numFailed;
	_funcptr_glMapGrid1d = (PFNGLMAPGRID1DPROC)IntGetProcAddress("glMapGrid1d");
	if(!_funcptr_glMapGrid1d) ++numFailed;
	_funcptr_glMapGrid1f = (PFNGLMAPGRID1FPROC)IntGetProcAddress("glMapGrid1f");
	if(!_funcptr_glMapGrid1f) ++numFailed;
	_funcptr_glMapGrid2d = (PFNGLMAPGRID2DPROC)IntGetProcAddress("glMapGrid2d");
	if(!_funcptr_glMapGrid2d) ++numFailed;
	_funcptr_glMapGrid2f = (PFNGLMAPGRID2FPROC)IntGetProcAddress("glMapGrid2f");
	if(!_funcptr_glMapGrid2f) ++numFailed;
	_funcptr_glEvalCoord1d = (PFNGLEVALCOORD1DPROC)IntGetProcAddress("glEvalCoord1d");
	if(!_funcptr_glEvalCoord1d) ++numFailed;
	_funcptr_glEvalCoord1dv = (PFNGLEVALCOORD1DVPROC)IntGetProcAddress("glEvalCoord1dv");
	if(!_funcptr_glEvalCoord1dv) ++numFailed;
	_funcptr_glEvalCoord1f = (PFNGLEVALCOORD1FPROC)IntGetProcAddress("glEvalCoord1f");
	if(!_funcptr_glEvalCoord1f) ++numFailed;
	_funcptr_glEvalCoord1fv = (PFNGLEVALCOORD1FVPROC)IntGetProcAddress("glEvalCoord1fv");
	if(!_funcptr_glEvalCoord1fv) ++numFailed;
	_funcptr_glEvalCoord2d = (PFNGLEVALCOORD2DPROC)IntGetProcAddress("glEvalCoord2d");
	if(!_funcptr_glEvalCoord2d) ++numFailed;
	_funcptr_glEvalCoord2dv = (PFNGLEVALCOORD2DVPROC)IntGetProcAddress("glEvalCoord2dv");
	if(!_funcptr_glEvalCoord2dv) ++numFailed;
	_funcptr_glEvalCoord2f = (PFNGLEVALCOORD2FPROC)IntGetProcAddress("glEvalCoord2f");
	if(!_funcptr_glEvalCoord2f) ++numFailed;
	_funcptr_glEvalCoord2fv = (PFNGLEVALCOORD2FVPROC)IntGetProcAddress("glEvalCoord2fv");
	if(!_funcptr_glEvalCoord2fv) ++numFailed;
	_funcptr_glEvalMesh1 = (PFNGLEVALMESH1PROC)IntGetProcAddress("glEvalMesh1");
	if(!_funcptr_glEvalMesh1) ++numFailed;
	_funcptr_glEvalPoint1 = (PFNGLEVALPOINT1PROC)IntGetProcAddress("glEvalPoint1");
	if(!_funcptr_glEvalPoint1) ++numFailed;
	_funcptr_glEvalMesh2 = (PFNGLEVALMESH2PROC)IntGetProcAddress("glEvalMesh2");
	if(!_funcptr_glEvalMesh2) ++numFailed;
	_funcptr_glEvalPoint2 = (PFNGLEVALPOINT2PROC)IntGetProcAddress("glEvalPoint2");
	if(!_funcptr_glEvalPoint2) ++numFailed;
	_funcptr_glAlphaFunc = (PFNGLALPHAFUNCPROC)IntGetProcAddress("glAlphaFunc");
	if(!_funcptr_glAlphaFunc) ++numFailed;
	_funcptr_glPixelZoom = (PFNGLPIXELZOOMPROC)IntGetProcAddress("glPixelZoom");
	if(!_funcptr_glPixelZoom) ++numFailed;
	_funcptr_glPixelTransferf = (PFNGLPIXELTRANSFERFPROC)IntGetProcAddress("glPixelTransferf");
	if(!_funcptr_glPixelTransferf) ++numFailed;
	_funcptr_glPixelTransferi = (PFNGLPIXELTRANSFERIPROC)IntGetProcAddress("glPixelTransferi");
	if(!_funcptr_glPixelTransferi) ++numFailed;
	_funcptr_glPixelMapfv = (PFNGLPIXELMAPFVPROC)IntGetProcAddress("glPixelMapfv");
	if(!_funcptr_glPixelMapfv) ++numFailed;
	_funcptr_glPixelMapuiv = (PFNGLPIXELMAPUIVPROC)IntGetProcAddress("glPixelMapuiv");
	if(!_funcptr_glPixelMapuiv) ++numFailed;
	_funcptr_glPixelMapusv = (PFNGLPIXELMAPUSVPROC)IntGetProcAddress("glPixelMapusv");
	if(!_funcptr_glPixelMapusv) ++numFailed;
	_funcptr_glCopyPixels = (PFNGLCOPYPIXELSPROC)IntGetProcAddress("glCopyPixels");
	if(!_funcptr_glCopyPixels) ++numFailed;
	_funcptr_glDrawPixels = (PFNGLDRAWPIXELSPROC)IntGetProcAddress("glDrawPixels");
	if(!_funcptr_glDrawPixels) ++numFailed;
	_funcptr_glGetClipPlane = (PFNGLGETCLIPPLANEPROC)IntGetProcAddress("glGetClipPlane");
	if(!_funcptr_glGetClipPlane) ++numFailed;
	_funcptr_glGetLightfv = (PFNGLGETLIGHTFVPROC)IntGetProcAddress("glGetLightfv");
	if(!_funcptr_glGetLightfv) ++numFailed;
	_funcptr_glGetLightiv = (PFNGLGETLIGHTIVPROC)IntGetProcAddress("glGetLightiv");
	if(!_funcptr_glGetLightiv) ++numFailed;
	_funcptr_glGetMapdv = (PFNGLGETMAPDVPROC)IntGetProcAddress("glGetMapdv");
	if(!_funcptr_glGetMapdv) ++numFailed;
	_funcptr_glGetMapfv = (PFNGLGETMAPFVPROC)IntGetProcAddress("glGetMapfv");
	if(!_funcptr_glGetMapfv) ++numFailed;
	_funcptr_glGetMapiv = (PFNGLGETMAPIVPROC)IntGetProcAddress("glGetMapiv");
	if(!_funcptr_glGetMapiv) ++numFailed;
	_funcptr_glGetMaterialfv = (PFNGLGETMATERIALFVPROC)IntGetProcAddress("glGetMaterialfv");
	if(!_funcptr_glGetMaterialfv) ++numFailed;
	_funcptr_glGetMaterialiv = (PFNGLGETMATERIALIVPROC)IntGetProcAddress("glGetMaterialiv");
	if(!_funcptr_glGetMaterialiv) ++numFailed;
	_funcptr_glGetPixelMapfv = (PFNGLGETPIXELMAPFVPROC)IntGetProcAddress("glGetPixelMapfv");
	if(!_funcptr_glGetPixelMapfv) ++numFailed;
	_funcptr_glGetPixelMapuiv = (PFNGLGETPIXELMAPUIVPROC)IntGetProcAddress("glGetPixelMapuiv");
	if(!_funcptr_glGetPixelMapuiv) ++numFailed;
	_funcptr_glGetPixelMapusv = (PFNGLGETPIXELMAPUSVPROC)IntGetProcAddress("glGetPixelMapusv");
	if(!_funcptr_glGetPixelMapusv) ++numFailed;
	_funcptr_glGetPolygonStipple = (PFNGLGETPOLYGONSTIPPLEPROC)IntGetProcAddress("glGetPolygonStipple");
	if(!_funcptr_glGetPolygonStipple) ++numFailed;
	_funcptr_glGetTexEnvfv = (PFNGLGETTEXENVFVPROC)IntGetProcAddress("glGetTexEnvfv");
	if(!_funcptr_glGetTexEnvfv) ++numFailed;
	_funcptr_glGetTexEnviv = (PFNGLGETTEXENVIVPROC)IntGetProcAddress("glGetTexEnviv");
	if(!_funcptr_glGetTexEnviv) ++numFailed;
	_funcptr_glGetTexGendv = (PFNGLGETTEXGENDVPROC)IntGetProcAddress("glGetTexGendv");
	if(!_funcptr_glGetTexGendv) ++numFailed;
	_funcptr_glGetTexGenfv = (PFNGLGETTEXGENFVPROC)IntGetProcAddress("glGetTexGenfv");
	if(!_funcptr_glGetTexGenfv) ++numFailed;
	_funcptr_glGetTexGeniv = (PFNGLGETTEXGENIVPROC)IntGetProcAddress("glGetTexGeniv");
	if(!_funcptr_glGetTexGeniv) ++numFailed;
	_funcptr_glIsList = (PFNGLISLISTPROC)IntGetProcAddress("glIsList");
	if(!_funcptr_glIsList) ++numFailed;
	_funcptr_glFrustum = (PFNGLFRUSTUMPROC)IntGetProcAddress("glFrustum");
	if(!_funcptr_glFrustum) ++numFailed;
	_funcptr_glLoadIdentity = (PFNGLLOADIDENTITYPROC)IntGetProcAddress("glLoadIdentity");
	if(!_funcptr_glLoadIdentity) ++numFailed;
	_funcptr_glLoadMatrixf = (PFNGLLOADMATRIXFPROC)IntGetProcAddress("glLoadMatrixf");
	if(!_funcptr_glLoadMatrixf) ++numFailed;
	_funcptr_glLoadMatrixd = (PFNGLLOADMATRIXDPROC)IntGetProcAddress("glLoadMatrixd");
	if(!_funcptr_glLoadMatrixd) ++numFailed;
	_funcptr_glMatrixMode = (PFNGLMATRIXMODEPROC)IntGetProcAddress("glMatrixMode");
	if(!_funcptr_glMatrixMode) ++numFailed;
	_funcptr_glMultMatrixf = (PFNGLMULTMATRIXFPROC)IntGetProcAddress("glMultMatrixf");
	if(!_funcptr_glMultMatrixf) ++numFailed;
	_funcptr_glMultMatrixd = (PFNGLMULTMATRIXDPROC)IntGetProcAddress("glMultMatrixd");
	if(!_funcptr_glMultMatrixd) ++numFailed;
	_funcptr_glOrtho = (PFNGLORTHOPROC)IntGetProcAddress("glOrtho");
	if(!_funcptr_glOrtho) ++numFailed;
	_funcptr_glPopMatrix = (PFNGLPOPMATRIXPROC)IntGetProcAddress("glPopMatrix");
	if(!_funcptr_glPopMatrix) ++numFailed;
	_funcptr_glPushMatrix = (PFNGLPUSHMATRIXPROC)IntGetProcAddress("glPushMatrix");
	if(!_funcptr_glPushMatrix) ++numFailed;
	_funcptr_glRotated = (PFNGLROTATEDPROC)IntGetProcAddress("glRotated");
	if(!_funcptr_glRotated) ++numFailed;
	_funcptr_glRotatef = (PFNGLROTATEFPROC)IntGetProcAddress("glRotatef");
	if(!_funcptr_glRotatef) ++numFailed;
	_funcptr_glScaled = (PFNGLSCALEDPROC)IntGetProcAddress("glScaled");
	if(!_funcptr_glScaled) ++numFailed;
	_funcptr_glScalef = (PFNGLSCALEFPROC)IntGetProcAddress("glScalef");
	if(!_funcptr_glScalef) ++numFailed;
	_funcptr_glTranslated = (PFNGLTRANSLATEDPROC)IntGetProcAddress("glTranslated");
	if(!_funcptr_glTranslated) ++numFailed;
	_funcptr_glTranslatef = (PFNGLTRANSLATEFPROC)IntGetProcAddress("glTranslatef");
	if(!_funcptr_glTranslatef) ++numFailed;
	_funcptr_glArrayElement = (PFNGLARRAYELEMENTPROC)IntGetProcAddress("glArrayElement");
	if(!_funcptr_glArrayElement) ++numFailed;
	_funcptr_glColorPointer = (PFNGLCOLORPOINTERPROC)IntGetProcAddress("glColorPointer");
	if(!_funcptr_glColorPointer) ++numFailed;
	_funcptr_glDisableClientState = (PFNGLDISABLECLIENTSTATEPROC)IntGetProcAddress("glDisableClientState");
	if(!_funcptr_glDisableClientState) ++numFailed;
	_funcptr_glEdgeFlagPointer = (PFNGLEDGEFLAGPOINTERPROC)IntGetProcAddress("glEdgeFlagPointer");
	if(!_funcptr_glEdgeFlagPointer) ++numFailed;
	_funcptr_glEnableClientState = (PFNGLENABLECLIENTSTATEPROC)IntGetProcAddress("glEnableClientState");
	if(!_funcptr_glEnableClientState) ++numFailed;
	_funcptr_glIndexPointer = (PFNGLINDEXPOINTERPROC)IntGetProcAddress("glIndexPointer");
	if(!_funcptr_glIndexPointer) ++numFailed;
	_funcptr_glInterleavedArrays = (PFNGLINTERLEAVEDARRAYSPROC)IntGetProcAddress("glInterleavedArrays");
	if(!_funcptr_glInterleavedArrays) ++numFailed;
	_funcptr_glNormalPointer = (PFNGLNORMALPOINTERPROC)IntGetProcAddress("glNormalPointer");
	if(!_funcptr_glNormalPointer) ++numFailed;
	_funcptr_glTexCoordPointer = (PFNGLTEXCOORDPOINTERPROC)IntGetProcAddress("glTexCoordPointer");
	if(!_funcptr_glTexCoordPointer) ++numFailed;
	_funcptr_glVertexPointer = (PFNGLVERTEXPOINTERPROC)IntGetProcAddress("glVertexPointer");
	if(!_funcptr_glVertexPointer) ++numFailed;
	_funcptr_glAreTexturesResident = (PFNGLARETEXTURESRESIDENTPROC)IntGetProcAddress("glAreTexturesResident");
	if(!_funcptr_glAreTexturesResident) ++numFailed;
	_funcptr_glPrioritizeTextures = (PFNGLPRIORITIZETEXTURESPROC)IntGetProcAddress("glPrioritizeTextures");
	if(!_funcptr_glPrioritizeTextures) ++numFailed;
	_funcptr_glPopClientAttrib = (PFNGLPOPCLIENTATTRIBPROC)IntGetProcAddress("glPopClientAttrib");
	if(!_funcptr_glPopClientAttrib) ++numFailed;
	_funcptr_glPushClientAttrib = (PFNGLPUSHCLIENTATTRIBPROC)IntGetProcAddress("glPushClientAttrib");
	if(!_funcptr_glPushClientAttrib) ++numFailed;
	return numFailed;
}
static int LoadCore_Version_1_2()
{
	int numFailed = 0;
	_funcptr_glBlendColor = (PFNGLBLENDCOLORPROC)IntGetProcAddress("glBlendColor");
	if(!_funcptr_glBlendColor) ++numFailed;
	_funcptr_glBlendEquation = (PFNGLBLENDEQUATIONPROC)IntGetProcAddress("glBlendEquation");
	if(!_funcptr_glBlendEquation) ++numFailed;
	_funcptr_glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC)IntGetProcAddress("glDrawRangeElements");
	if(!_funcptr_glDrawRangeElements) ++numFailed;
	_funcptr_glTexImage3D = (PFNGLTEXIMAGE3DPROC)IntGetProcAddress("glTexImage3D");
	if(!_funcptr_glTexImage3D) ++numFailed;
	_funcptr_glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC)IntGetProcAddress("glTexSubImage3D");
	if(!_funcptr_glTexSubImage3D) ++numFailed;
	_funcptr_glCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC)IntGetProcAddress("glCopyTexSubImage3D");
	if(!_funcptr_glCopyTexSubImage3D) ++numFailed;
	return numFailed;
}

static int LoadCore_Version_1_2_Comp()
{
	int numFailed = 0;
	_funcptr_glColorTable = (PFNGLCOLORTABLEPROC)IntGetProcAddress("glColorTable");
	if(!_funcptr_glColorTable) ++numFailed;
	_funcptr_glColorTableParameterfv = (PFNGLCOLORTABLEPARAMETERFVPROC)IntGetProcAddress("glColorTableParameterfv");
	if(!_funcptr_glColorTableParameterfv) ++numFailed;
	_funcptr_glColorTableParameteriv = (PFNGLCOLORTABLEPARAMETERIVPROC)IntGetProcAddress("glColorTableParameteriv");
	if(!_funcptr_glColorTableParameteriv) ++numFailed;
	_funcptr_glCopyColorTable = (PFNGLCOPYCOLORTABLEPROC)IntGetProcAddress("glCopyColorTable");
	if(!_funcptr_glCopyColorTable) ++numFailed;
	_funcptr_glGetColorTable = (PFNGLGETCOLORTABLEPROC)IntGetProcAddress("glGetColorTable");
	if(!_funcptr_glGetColorTable) ++numFailed;
	_funcptr_glGetColorTableParameterfv = (PFNGLGETCOLORTABLEPARAMETERFVPROC)IntGetProcAddress("glGetColorTableParameterfv");
	if(!_funcptr_glGetColorTableParameterfv) ++numFailed;
	_funcptr_glGetColorTableParameteriv = (PFNGLGETCOLORTABLEPARAMETERIVPROC)IntGetProcAddress("glGetColorTableParameteriv");
	if(!_funcptr_glGetColorTableParameteriv) ++numFailed;
	_funcptr_glColorSubTable = (PFNGLCOLORSUBTABLEPROC)IntGetProcAddress("glColorSubTable");
	if(!_funcptr_glColorSubTable) ++numFailed;
	_funcptr_glCopyColorSubTable = (PFNGLCOPYCOLORSUBTABLEPROC)IntGetProcAddress("glCopyColorSubTable");
	if(!_funcptr_glCopyColorSubTable) ++numFailed;
	_funcptr_glConvolutionFilter1D = (PFNGLCONVOLUTIONFILTER1DPROC)IntGetProcAddress("glConvolutionFilter1D");
	if(!_funcptr_glConvolutionFilter1D) ++numFailed;
	_funcptr_glConvolutionFilter2D = (PFNGLCONVOLUTIONFILTER2DPROC)IntGetProcAddress("glConvolutionFilter2D");
	if(!_funcptr_glConvolutionFilter2D) ++numFailed;
	_funcptr_glConvolutionParameterf = (PFNGLCONVOLUTIONPARAMETERFPROC)IntGetProcAddress("glConvolutionParameterf");
	if(!_funcptr_glConvolutionParameterf) ++numFailed;
	_funcptr_glConvolutionParameterfv = (PFNGLCONVOLUTIONPARAMETERFVPROC)IntGetProcAddress("glConvolutionParameterfv");
	if(!_funcptr_glConvolutionParameterfv) ++numFailed;
	_funcptr_glConvolutionParameteri = (PFNGLCONVOLUTIONPARAMETERIPROC)IntGetProcAddress("glConvolutionParameteri");
	if(!_funcptr_glConvolutionParameteri) ++numFailed;
	_funcptr_glConvolutionParameteriv = (PFNGLCONVOLUTIONPARAMETERIVPROC)IntGetProcAddress("glConvolutionParameteriv");
	if(!_funcptr_glConvolutionParameteriv) ++numFailed;
	_funcptr_glCopyConvolutionFilter1D = (PFNGLCOPYCONVOLUTIONFILTER1DPROC)IntGetProcAddress("glCopyConvolutionFilter1D");
	if(!_funcptr_glCopyConvolutionFilter1D) ++numFailed;
	_funcptr_glCopyConvolutionFilter2D = (PFNGLCOPYCONVOLUTIONFILTER2DPROC)IntGetProcAddress("glCopyConvolutionFilter2D");
	if(!_funcptr_glCopyConvolutionFilter2D) ++numFailed;
	_funcptr_glGetConvolutionFilter = (PFNGLGETCONVOLUTIONFILTERPROC)IntGetProcAddress("glGetConvolutionFilter");
	if(!_funcptr_glGetConvolutionFilter) ++numFailed;
	_funcptr_glGetConvolutionParameterfv = (PFNGLGETCONVOLUTIONPARAMETERFVPROC)IntGetProcAddress("glGetConvolutionParameterfv");
	if(!_funcptr_glGetConvolutionParameterfv) ++numFailed;
	_funcptr_glGetConvolutionParameteriv = (PFNGLGETCONVOLUTIONPARAMETERIVPROC)IntGetProcAddress("glGetConvolutionParameteriv");
	if(!_funcptr_glGetConvolutionParameteriv) ++numFailed;
	_funcptr_glGetSeparableFilter = (PFNGLGETSEPARABLEFILTERPROC)IntGetProcAddress("glGetSeparableFilter");
	if(!_funcptr_glGetSeparableFilter) ++numFailed;
	_funcptr_glSeparableFilter2D = (PFNGLSEPARABLEFILTER2DPROC)IntGetProcAddress("glSeparableFilter2D");
	if(!_funcptr_glSeparableFilter2D) ++numFailed;
	_funcptr_glGetHistogram = (PFNGLGETHISTOGRAMPROC)IntGetProcAddress("glGetHistogram");
	if(!_funcptr_glGetHistogram) ++numFailed;
	_funcptr_glGetHistogramParameterfv = (PFNGLGETHISTOGRAMPARAMETERFVPROC)IntGetProcAddress("glGetHistogramParameterfv");
	if(!_funcptr_glGetHistogramParameterfv) ++numFailed;
	_funcptr_glGetHistogramParameteriv = (PFNGLGETHISTOGRAMPARAMETERIVPROC)IntGetProcAddress("glGetHistogramParameteriv");
	if(!_funcptr_glGetHistogramParameteriv) ++numFailed;
	_funcptr_glGetMinmax = (PFNGLGETMINMAXPROC)IntGetProcAddress("glGetMinmax");
	if(!_funcptr_glGetMinmax) ++numFailed;
	_funcptr_glGetMinmaxParameterfv = (PFNGLGETMINMAXPARAMETERFVPROC)IntGetProcAddress("glGetMinmaxParameterfv");
	if(!_funcptr_glGetMinmaxParameterfv) ++numFailed;
	_funcptr_glGetMinmaxParameteriv = (PFNGLGETMINMAXPARAMETERIVPROC)IntGetProcAddress("glGetMinmaxParameteriv");
	if(!_funcptr_glGetMinmaxParameteriv) ++numFailed;
	_funcptr_glHistogram = (PFNGLHISTOGRAMPROC)IntGetProcAddress("glHistogram");
	if(!_funcptr_glHistogram) ++numFailed;
	_funcptr_glMinmax = (PFNGLMINMAXPROC)IntGetProcAddress("glMinmax");
	if(!_funcptr_glMinmax) ++numFailed;
	_funcptr_glResetHistogram = (PFNGLRESETHISTOGRAMPROC)IntGetProcAddress("glResetHistogram");
	if(!_funcptr_glResetHistogram) ++numFailed;
	_funcptr_glResetMinmax = (PFNGLRESETMINMAXPROC)IntGetProcAddress("glResetMinmax");
	if(!_funcptr_glResetMinmax) ++numFailed;
	return numFailed;
}
static int LoadCore_Version_1_3()
{
	int numFailed = 0;
	_funcptr_glActiveTexture = (PFNGLACTIVETEXTUREPROC)IntGetProcAddress("glActiveTexture");
	if(!_funcptr_glActiveTexture) ++numFailed;
	_funcptr_glSampleCoverage = (PFNGLSAMPLECOVERAGEPROC)IntGetProcAddress("glSampleCoverage");
	if(!_funcptr_glSampleCoverage) ++numFailed;
	_funcptr_glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC)IntGetProcAddress("glCompressedTexImage3D");
	if(!_funcptr_glCompressedTexImage3D) ++numFailed;
	_funcptr_glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)IntGetProcAddress("glCompressedTexImage2D");
	if(!_funcptr_glCompressedTexImage2D) ++numFailed;
	_funcptr_glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC)IntGetProcAddress("glCompressedTexImage1D");
	if(!_funcptr_glCompressedTexImage1D) ++numFailed;
	_funcptr_glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)IntGetProcAddress("glCompressedTexSubImage3D");
	if(!_funcptr_glCompressedTexSubImage3D) ++numFailed;
	_funcptr_glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)IntGetProcAddress("glCompressedTexSubImage2D");
	if(!_funcptr_glCompressedTexSubImage2D) ++numFailed;
	_funcptr_glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)IntGetProcAddress("glCompressedTexSubImage1D");
	if(!_funcptr_glCompressedTexSubImage1D) ++numFailed;
	_funcptr_glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC)IntGetProcAddress("glGetCompressedTexImage");
	if(!_funcptr_glGetCompressedTexImage) ++numFailed;
	return numFailed;
}

static int LoadCore_Version_1_3_Comp()
{
	int numFailed = 0;
	_funcptr_glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC)IntGetProcAddress("glClientActiveTexture");
	if(!_funcptr_glClientActiveTexture) ++numFailed;
	_funcptr_glMultiTexCoord1d = (PFNGLMULTITEXCOORD1DPROC)IntGetProcAddress("glMultiTexCoord1d");
	if(!_funcptr_glMultiTexCoord1d) ++numFailed;
	_funcptr_glMultiTexCoord1dv = (PFNGLMULTITEXCOORD1DVPROC)IntGetProcAddress("glMultiTexCoord1dv");
	if(!_funcptr_glMultiTexCoord1dv) ++numFailed;
	_funcptr_glMultiTexCoord1f = (PFNGLMULTITEXCOORD1FPROC)IntGetProcAddress("glMultiTexCoord1f");
	if(!_funcptr_glMultiTexCoord1f) ++numFailed;
	_funcptr_glMultiTexCoord1fv = (PFNGLMULTITEXCOORD1FVPROC)IntGetProcAddress("glMultiTexCoord1fv");
	if(!_funcptr_glMultiTexCoord1fv) ++numFailed;
	_funcptr_glMultiTexCoord1i = (PFNGLMULTITEXCOORD1IPROC)IntGetProcAddress("glMultiTexCoord1i");
	if(!_funcptr_glMultiTexCoord1i) ++numFailed;
	_funcptr_glMultiTexCoord1iv = (PFNGLMULTITEXCOORD1IVPROC)IntGetProcAddress("glMultiTexCoord1iv");
	if(!_funcptr_glMultiTexCoord1iv) ++numFailed;
	_funcptr_glMultiTexCoord1s = (PFNGLMULTITEXCOORD1SPROC)IntGetProcAddress("glMultiTexCoord1s");
	if(!_funcptr_glMultiTexCoord1s) ++numFailed;
	_funcptr_glMultiTexCoord1sv = (PFNGLMULTITEXCOORD1SVPROC)IntGetProcAddress("glMultiTexCoord1sv");
	if(!_funcptr_glMultiTexCoord1sv) ++numFailed;
	_funcptr_glMultiTexCoord2d = (PFNGLMULTITEXCOORD2DPROC)IntGetProcAddress("glMultiTexCoord2d");
	if(!_funcptr_glMultiTexCoord2d) ++numFailed;
	_funcptr_glMultiTexCoord2dv = (PFNGLMULTITEXCOORD2DVPROC)IntGetProcAddress("glMultiTexCoord2dv");
	if(!_funcptr_glMultiTexCoord2dv) ++numFailed;
	_funcptr_glMultiTexCoord2f = (PFNGLMULTITEXCOORD2FPROC)IntGetProcAddress("glMultiTexCoord2f");
	if(!_funcptr_glMultiTexCoord2f) ++numFailed;
	_funcptr_glMultiTexCoord2fv = (PFNGLMULTITEXCOORD2FVPROC)IntGetProcAddress("glMultiTexCoord2fv");
	if(!_funcptr_glMultiTexCoord2fv) ++numFailed;
	_funcptr_glMultiTexCoord2i = (PFNGLMULTITEXCOORD2IPROC)IntGetProcAddress("glMultiTexCoord2i");
	if(!_funcptr_glMultiTexCoord2i) ++numFailed;
	_funcptr_glMultiTexCoord2iv = (PFNGLMULTITEXCOORD2IVPROC)IntGetProcAddress("glMultiTexCoord2iv");
	if(!_funcptr_glMultiTexCoord2iv) ++numFailed;
	_funcptr_glMultiTexCoord2s = (PFNGLMULTITEXCOORD2SPROC)IntGetProcAddress("glMultiTexCoord2s");
	if(!_funcptr_glMultiTexCoord2s) ++numFailed;
	_funcptr_glMultiTexCoord2sv = (PFNGLMULTITEXCOORD2SVPROC)IntGetProcAddress("glMultiTexCoord2sv");
	if(!_funcptr_glMultiTexCoord2sv) ++numFailed;
	_funcptr_glMultiTexCoord3d = (PFNGLMULTITEXCOORD3DPROC)IntGetProcAddress("glMultiTexCoord3d");
	if(!_funcptr_glMultiTexCoord3d) ++numFailed;
	_funcptr_glMultiTexCoord3dv = (PFNGLMULTITEXCOORD3DVPROC)IntGetProcAddress("glMultiTexCoord3dv");
	if(!_funcptr_glMultiTexCoord3dv) ++numFailed;
	_funcptr_glMultiTexCoord3f = (PFNGLMULTITEXCOORD3FPROC)IntGetProcAddress("glMultiTexCoord3f");
	if(!_funcptr_glMultiTexCoord3f) ++numFailed;
	_funcptr_glMultiTexCoord3fv = (PFNGLMULTITEXCOORD3FVPROC)IntGetProcAddress("glMultiTexCoord3fv");
	if(!_funcptr_glMultiTexCoord3fv) ++numFailed;
	_funcptr_glMultiTexCoord3i = (PFNGLMULTITEXCOORD3IPROC)IntGetProcAddress("glMultiTexCoord3i");
	if(!_funcptr_glMultiTexCoord3i) ++numFailed;
	_funcptr_glMultiTexCoord3iv = (PFNGLMULTITEXCOORD3IVPROC)IntGetProcAddress("glMultiTexCoord3iv");
	if(!_funcptr_glMultiTexCoord3iv) ++numFailed;
	_funcptr_glMultiTexCoord3s = (PFNGLMULTITEXCOORD3SPROC)IntGetProcAddress("glMultiTexCoord3s");
	if(!_funcptr_glMultiTexCoord3s) ++numFailed;
	_funcptr_glMultiTexCoord3sv = (PFNGLMULTITEXCOORD3SVPROC)IntGetProcAddress("glMultiTexCoord3sv");
	if(!_funcptr_glMultiTexCoord3sv) ++numFailed;
	_funcptr_glMultiTexCoord4d = (PFNGLMULTITEXCOORD4DPROC)IntGetProcAddress("glMultiTexCoord4d");
	if(!_funcptr_glMultiTexCoord4d) ++numFailed;
	_funcptr_glMultiTexCoord4dv = (PFNGLMULTITEXCOORD4DVPROC)IntGetProcAddress("glMultiTexCoord4dv");
	if(!_funcptr_glMultiTexCoord4dv) ++numFailed;
	_funcptr_glMultiTexCoord4f = (PFNGLMULTITEXCOORD4FPROC)IntGetProcAddress("glMultiTexCoord4f");
	if(!_funcptr_glMultiTexCoord4f) ++numFailed;
	_funcptr_glMultiTexCoord4fv = (PFNGLMULTITEXCOORD4FVPROC)IntGetProcAddress("glMultiTexCoord4fv");
	if(!_funcptr_glMultiTexCoord4fv) ++numFailed;
	_funcptr_glMultiTexCoord4i = (PFNGLMULTITEXCOORD4IPROC)IntGetProcAddress("glMultiTexCoord4i");
	if(!_funcptr_glMultiTexCoord4i) ++numFailed;
	_funcptr_glMultiTexCoord4iv = (PFNGLMULTITEXCOORD4IVPROC)IntGetProcAddress("glMultiTexCoord4iv");
	if(!_funcptr_glMultiTexCoord4iv) ++numFailed;
	_funcptr_glMultiTexCoord4s = (PFNGLMULTITEXCOORD4SPROC)IntGetProcAddress("glMultiTexCoord4s");
	if(!_funcptr_glMultiTexCoord4s) ++numFailed;
	_funcptr_glMultiTexCoord4sv = (PFNGLMULTITEXCOORD4SVPROC)IntGetProcAddress("glMultiTexCoord4sv");
	if(!_funcptr_glMultiTexCoord4sv) ++numFailed;
	_funcptr_glLoadTransposeMatrixf = (PFNGLLOADTRANSPOSEMATRIXFPROC)IntGetProcAddress("glLoadTransposeMatrixf");
	if(!_funcptr_glLoadTransposeMatrixf) ++numFailed;
	_funcptr_glLoadTransposeMatrixd = (PFNGLLOADTRANSPOSEMATRIXDPROC)IntGetProcAddress("glLoadTransposeMatrixd");
	if(!_funcptr_glLoadTransposeMatrixd) ++numFailed;
	_funcptr_glMultTransposeMatrixf = (PFNGLMULTTRANSPOSEMATRIXFPROC)IntGetProcAddress("glMultTransposeMatrixf");
	if(!_funcptr_glMultTransposeMatrixf) ++numFailed;
	_funcptr_glMultTransposeMatrixd = (PFNGLMULTTRANSPOSEMATRIXDPROC)IntGetProcAddress("glMultTransposeMatrixd");
	if(!_funcptr_glMultTransposeMatrixd) ++numFailed;
	return numFailed;
}
static int LoadCore_Version_1_4()
{
	int numFailed = 0;
	_funcptr_glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC)IntGetProcAddress("glBlendFuncSeparate");
	if(!_funcptr_glBlendFuncSeparate) ++numFailed;
	_funcptr_glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSPROC)IntGetProcAddress("glMultiDrawArrays");
	if(!_funcptr_glMultiDrawArrays) ++numFailed;
	_funcptr_glMultiDrawElements = (PFNGLMULTIDRAWELEMENTSPROC)IntGetProcAddress("glMultiDrawElements");
	if(!_funcptr_glMultiDrawElements) ++numFailed;
	_funcptr_glPointParameterf = (PFNGLPOINTPARAMETERFPROC)IntGetProcAddress("glPointParameterf");
	if(!_funcptr_glPointParameterf) ++numFailed;
	_funcptr_glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC)IntGetProcAddress("glPointParameterfv");
	if(!_funcptr_glPointParameterfv) ++numFailed;
	_funcptr_glPointParameteri = (PFNGLPOINTPARAMETERIPROC)IntGetProcAddress("glPointParameteri");
	if(!_funcptr_glPointParameteri) ++numFailed;
	_funcptr_glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC)IntGetProcAddress("glPointParameteriv");
	if(!_funcptr_glPointParameteriv) ++numFailed;
	return numFailed;
}

static int LoadCore_Version_1_4_Comp()
{
	int numFailed = 0;
	_funcptr_glFogCoordf = (PFNGLFOGCOORDFPROC)IntGetProcAddress("glFogCoordf");
	if(!_funcptr_glFogCoordf) ++numFailed;
	_funcptr_glFogCoordfv = (PFNGLFOGCOORDFVPROC)IntGetProcAddress("glFogCoordfv");
	if(!_funcptr_glFogCoordfv) ++numFailed;
	_funcptr_glFogCoordd = (PFNGLFOGCOORDDPROC)IntGetProcAddress("glFogCoordd");
	if(!_funcptr_glFogCoordd) ++numFailed;
	_funcptr_glFogCoorddv = (PFNGLFOGCOORDDVPROC)IntGetProcAddress("glFogCoorddv");
	if(!_funcptr_glFogCoorddv) ++numFailed;
	_funcptr_glFogCoordPointer = (PFNGLFOGCOORDPOINTERPROC)IntGetProcAddress("glFogCoordPointer");
	if(!_funcptr_glFogCoordPointer) ++numFailed;
	_funcptr_glSecondaryColor3b = (PFNGLSECONDARYCOLOR3BPROC)IntGetProcAddress("glSecondaryColor3b");
	if(!_funcptr_glSecondaryColor3b) ++numFailed;
	_funcptr_glSecondaryColor3bv = (PFNGLSECONDARYCOLOR3BVPROC)IntGetProcAddress("glSecondaryColor3bv");
	if(!_funcptr_glSecondaryColor3bv) ++numFailed;
	_funcptr_glSecondaryColor3d = (PFNGLSECONDARYCOLOR3DPROC)IntGetProcAddress("glSecondaryColor3d");
	if(!_funcptr_glSecondaryColor3d) ++numFailed;
	_funcptr_glSecondaryColor3dv = (PFNGLSECONDARYCOLOR3DVPROC)IntGetProcAddress("glSecondaryColor3dv");
	if(!_funcptr_glSecondaryColor3dv) ++numFailed;
	_funcptr_glSecondaryColor3f = (PFNGLSECONDARYCOLOR3FPROC)IntGetProcAddress("glSecondaryColor3f");
	if(!_funcptr_glSecondaryColor3f) ++numFailed;
	_funcptr_glSecondaryColor3fv = (PFNGLSECONDARYCOLOR3FVPROC)IntGetProcAddress("glSecondaryColor3fv");
	if(!_funcptr_glSecondaryColor3fv) ++numFailed;
	_funcptr_glSecondaryColor3i = (PFNGLSECONDARYCOLOR3IPROC)IntGetProcAddress("glSecondaryColor3i");
	if(!_funcptr_glSecondaryColor3i) ++numFailed;
	_funcptr_glSecondaryColor3iv = (PFNGLSECONDARYCOLOR3IVPROC)IntGetProcAddress("glSecondaryColor3iv");
	if(!_funcptr_glSecondaryColor3iv) ++numFailed;
	_funcptr_glSecondaryColor3s = (PFNGLSECONDARYCOLOR3SPROC)IntGetProcAddress("glSecondaryColor3s");
	if(!_funcptr_glSecondaryColor3s) ++numFailed;
	_funcptr_glSecondaryColor3sv = (PFNGLSECONDARYCOLOR3SVPROC)IntGetProcAddress("glSecondaryColor3sv");
	if(!_funcptr_glSecondaryColor3sv) ++numFailed;
	_funcptr_glSecondaryColor3ub = (PFNGLSECONDARYCOLOR3UBPROC)IntGetProcAddress("glSecondaryColor3ub");
	if(!_funcptr_glSecondaryColor3ub) ++numFailed;
	_funcptr_glSecondaryColor3ubv = (PFNGLSECONDARYCOLOR3UBVPROC)IntGetProcAddress("glSecondaryColor3ubv");
	if(!_funcptr_glSecondaryColor3ubv) ++numFailed;
	_funcptr_glSecondaryColor3ui = (PFNGLSECONDARYCOLOR3UIPROC)IntGetProcAddress("glSecondaryColor3ui");
	if(!_funcptr_glSecondaryColor3ui) ++numFailed;
	_funcptr_glSecondaryColor3uiv = (PFNGLSECONDARYCOLOR3UIVPROC)IntGetProcAddress("glSecondaryColor3uiv");
	if(!_funcptr_glSecondaryColor3uiv) ++numFailed;
	_funcptr_glSecondaryColor3us = (PFNGLSECONDARYCOLOR3USPROC)IntGetProcAddress("glSecondaryColor3us");
	if(!_funcptr_glSecondaryColor3us) ++numFailed;
	_funcptr_glSecondaryColor3usv = (PFNGLSECONDARYCOLOR3USVPROC)IntGetProcAddress("glSecondaryColor3usv");
	if(!_funcptr_glSecondaryColor3usv) ++numFailed;
	_funcptr_glSecondaryColorPointer = (PFNGLSECONDARYCOLORPOINTERPROC)IntGetProcAddress("glSecondaryColorPointer");
	if(!_funcptr_glSecondaryColorPointer) ++numFailed;
	_funcptr_glWindowPos2d = (PFNGLWINDOWPOS2DPROC)IntGetProcAddress("glWindowPos2d");
	if(!_funcptr_glWindowPos2d) ++numFailed;
	_funcptr_glWindowPos2dv = (PFNGLWINDOWPOS2DVPROC)IntGetProcAddress("glWindowPos2dv");
	if(!_funcptr_glWindowPos2dv) ++numFailed;
	_funcptr_glWindowPos2f = (PFNGLWINDOWPOS2FPROC)IntGetProcAddress("glWindowPos2f");
	if(!_funcptr_glWindowPos2f) ++numFailed;
	_funcptr_glWindowPos2fv = (PFNGLWINDOWPOS2FVPROC)IntGetProcAddress("glWindowPos2fv");
	if(!_funcptr_glWindowPos2fv) ++numFailed;
	_funcptr_glWindowPos2i = (PFNGLWINDOWPOS2IPROC)IntGetProcAddress("glWindowPos2i");
	if(!_funcptr_glWindowPos2i) ++numFailed;
	_funcptr_glWindowPos2iv = (PFNGLWINDOWPOS2IVPROC)IntGetProcAddress("glWindowPos2iv");
	if(!_funcptr_glWindowPos2iv) ++numFailed;
	_funcptr_glWindowPos2s = (PFNGLWINDOWPOS2SPROC)IntGetProcAddress("glWindowPos2s");
	if(!_funcptr_glWindowPos2s) ++numFailed;
	_funcptr_glWindowPos2sv = (PFNGLWINDOWPOS2SVPROC)IntGetProcAddress("glWindowPos2sv");
	if(!_funcptr_glWindowPos2sv) ++numFailed;
	_funcptr_glWindowPos3d = (PFNGLWINDOWPOS3DPROC)IntGetProcAddress("glWindowPos3d");
	if(!_funcptr_glWindowPos3d) ++numFailed;
	_funcptr_glWindowPos3dv = (PFNGLWINDOWPOS3DVPROC)IntGetProcAddress("glWindowPos3dv");
	if(!_funcptr_glWindowPos3dv) ++numFailed;
	_funcptr_glWindowPos3f = (PFNGLWINDOWPOS3FPROC)IntGetProcAddress("glWindowPos3f");
	if(!_funcptr_glWindowPos3f) ++numFailed;
	_funcptr_glWindowPos3fv = (PFNGLWINDOWPOS3FVPROC)IntGetProcAddress("glWindowPos3fv");
	if(!_funcptr_glWindowPos3fv) ++numFailed;
	_funcptr_glWindowPos3i = (PFNGLWINDOWPOS3IPROC)IntGetProcAddress("glWindowPos3i");
	if(!_funcptr_glWindowPos3i) ++numFailed;
	_funcptr_glWindowPos3iv = (PFNGLWINDOWPOS3IVPROC)IntGetProcAddress("glWindowPos3iv");
	if(!_funcptr_glWindowPos3iv) ++numFailed;
	_funcptr_glWindowPos3s = (PFNGLWINDOWPOS3SPROC)IntGetProcAddress("glWindowPos3s");
	if(!_funcptr_glWindowPos3s) ++numFailed;
	_funcptr_glWindowPos3sv = (PFNGLWINDOWPOS3SVPROC)IntGetProcAddress("glWindowPos3sv");
	if(!_funcptr_glWindowPos3sv) ++numFailed;
	return numFailed;
}
static int LoadCore_Version_1_5()
{
	int numFailed = 0;
	_funcptr_glGenQueries = (PFNGLGENQUERIESPROC)IntGetProcAddress("glGenQueries");
	if(!_funcptr_glGenQueries) ++numFailed;
	_funcptr_glDeleteQueries = (PFNGLDELETEQUERIESPROC)IntGetProcAddress("glDeleteQueries");
	if(!_funcptr_glDeleteQueries) ++numFailed;
	_funcptr_glIsQuery = (PFNGLISQUERYPROC)IntGetProcAddress("glIsQuery");
	if(!_funcptr_glIsQuery) ++numFailed;
	_funcptr_glBeginQuery = (PFNGLBEGINQUERYPROC)IntGetProcAddress("glBeginQuery");
	if(!_funcptr_glBeginQuery) ++numFailed;
	_funcptr_glEndQuery = (PFNGLENDQUERYPROC)IntGetProcAddress("glEndQuery");
	if(!_funcptr_glEndQuery) ++numFailed;
	_funcptr_glGetQueryiv = (PFNGLGETQUERYIVPROC)IntGetProcAddress("glGetQueryiv");
	if(!_funcptr_glGetQueryiv) ++numFailed;
	_funcptr_glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)IntGetProcAddress("glGetQueryObjectiv");
	if(!_funcptr_glGetQueryObjectiv) ++numFailed;
	_funcptr_glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)IntGetProcAddress("glGetQueryObjectuiv");
	if(!_funcptr_glGetQueryObjectuiv) ++numFailed;
	_funcptr_glBindBuffer = (PFNGLBINDBUFFERPROC)IntGetProcAddress("glBindBuffer");
	if(!_funcptr_glBindBuffer) ++numFailed;
	_funcptr_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)IntGetProcAddress("glDeleteBuffers");
	if(!_funcptr_glDeleteBuffers) ++numFailed;
	_funcptr_glGenBuffers = (PFNGLGENBUFFERSPROC)IntGetProcAddress("glGenBuffers");
	if(!_funcptr_glGenBuffers) ++numFailed;
	_funcptr_glIsBuffer = (PFNGLISBUFFERPROC)IntGetProcAddress("glIsBuffer");
	if(!_funcptr_glIsBuffer) ++numFailed;
	_funcptr_glBufferData = (PFNGLBUFFERDATAPROC)IntGetProcAddress("glBufferData");
	if(!_funcptr_glBufferData) ++numFailed;
	_funcptr_glBufferSubData = (PFNGLBUFFERSUBDATAPROC)IntGetProcAddress("glBufferSubData");
	if(!_funcptr_glBufferSubData) ++numFailed;
	_funcptr_glGetBufferSubData = (PFNGLGETBUFFERSUBDATAPROC)IntGetProcAddress("glGetBufferSubData");
	if(!_funcptr_glGetBufferSubData) ++numFailed;
	_funcptr_glMapBuffer = (PFNGLMAPBUFFERPROC)IntGetProcAddress("glMapBuffer");
	if(!_funcptr_glMapBuffer) ++numFailed;
	_funcptr_glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)IntGetProcAddress("glUnmapBuffer");
	if(!_funcptr_glUnmapBuffer) ++numFailed;
	_funcptr_glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC)IntGetProcAddress("glGetBufferParameteriv");
	if(!_funcptr_glGetBufferParameteriv) ++numFailed;
	_funcptr_glGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC)IntGetProcAddress("glGetBufferPointerv");
	if(!_funcptr_glGetBufferPointerv) ++numFailed;
	return numFailed;
}

static int LoadCore_Version_2_0()
{
	int numFailed = 0;
	_funcptr_glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC)IntGetProcAddress("glBlendEquationSeparate");
	if(!_funcptr_glBlendEquationSeparate) ++numFailed;
	_funcptr_glDrawBuffers = (PFNGLDRAWBUFFERSPROC)IntGetProcAddress("glDrawBuffers");
	if(!_funcptr_glDrawBuffers) ++numFailed;
	_funcptr_glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)IntGetProcAddress("glStencilOpSeparate");
	if(!_funcptr_glStencilOpSeparate) ++numFailed;
	_funcptr_glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)IntGetProcAddress("glStencilFuncSeparate");
	if(!_funcptr_glStencilFuncSeparate) ++numFailed;
	_funcptr_glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC)IntGetProcAddress("glStencilMaskSeparate");
	if(!_funcptr_glStencilMaskSeparate) ++numFailed;
	_funcptr_glAttachShader = (PFNGLATTACHSHADERPROC)IntGetProcAddress("glAttachShader");
	if(!_funcptr_glAttachShader) ++numFailed;
	_funcptr_glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)IntGetProcAddress("glBindAttribLocation");
	if(!_funcptr_glBindAttribLocation) ++numFailed;
	_funcptr_glCompileShader = (PFNGLCOMPILESHADERPROC)IntGetProcAddress("glCompileShader");
	if(!_funcptr_glCompileShader) ++numFailed;
	_funcptr_glCreateProgram = (PFNGLCREATEPROGRAMPROC)IntGetProcAddress("glCreateProgram");
	if(!_funcptr_glCreateProgram) ++numFailed;
	_funcptr_glCreateShader = (PFNGLCREATESHADERPROC)IntGetProcAddress("glCreateShader");
	if(!_funcptr_glCreateShader) ++numFailed;
	_funcptr_glDeleteProgram = (PFNGLDELETEPROGRAMPROC)IntGetProcAddress("glDeleteProgram");
	if(!_funcptr_glDeleteProgram) ++numFailed;
	_funcptr_glDeleteShader = (PFNGLDELETESHADERPROC)IntGetProcAddress("glDeleteShader");
	if(!_funcptr_glDeleteShader) ++numFailed;
	_funcptr_glDetachShader = (PFNGLDETACHSHADERPROC)IntGetProcAddress("glDetachShader");
	if(!_funcptr_glDetachShader) ++numFailed;
	_funcptr_glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)IntGetProcAddress("glDisableVertexAttribArray");
	if(!_funcptr_glDisableVertexAttribArray) ++numFailed;
	_funcptr_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)IntGetProcAddress("glEnableVertexAttribArray");
	if(!_funcptr_glEnableVertexAttribArray) ++numFailed;
	_funcptr_glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC)IntGetProcAddress("glGetActiveAttrib");
	if(!_funcptr_glGetActiveAttrib) ++numFailed;
	_funcptr_glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)IntGetProcAddress("glGetActiveUniform");
	if(!_funcptr_glGetActiveUniform) ++numFailed;
	_funcptr_glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC)IntGetProcAddress("glGetAttachedShaders");
	if(!_funcptr_glGetAttachedShaders) ++numFailed;
	_funcptr_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)IntGetProcAddress("glGetAttribLocation");
	if(!_funcptr_glGetAttribLocation) ++numFailed;
	_funcptr_glGetProgramiv = (PFNGLGETPROGRAMIVPROC)IntGetProcAddress("glGetProgramiv");
	if(!_funcptr_glGetProgramiv) ++numFailed;
	_funcptr_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)IntGetProcAddress("glGetProgramInfoLog");
	if(!_funcptr_glGetProgramInfoLog) ++numFailed;
	_funcptr_glGetShaderiv = (PFNGLGETSHADERIVPROC)IntGetProcAddress("glGetShaderiv");
	if(!_funcptr_glGetShaderiv) ++numFailed;
	_funcptr_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)IntGetProcAddress("glGetShaderInfoLog");
	if(!_funcptr_glGetShaderInfoLog) ++numFailed;
	_funcptr_glGetShaderSource = (PFNGLGETSHADERSOURCEPROC)IntGetProcAddress("glGetShaderSource");
	if(!_funcptr_glGetShaderSource) ++numFailed;
	_funcptr_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)IntGetProcAddress("glGetUniformLocation");
	if(!_funcptr_glGetUniformLocation) ++numFailed;
	_funcptr_glGetUniformfv = (PFNGLGETUNIFORMFVPROC)IntGetProcAddress("glGetUniformfv");
	if(!_funcptr_glGetUniformfv) ++numFailed;
	_funcptr_glGetUniformiv = (PFNGLGETUNIFORMIVPROC)IntGetProcAddress("glGetUniformiv");
	if(!_funcptr_glGetUniformiv) ++numFailed;
	_funcptr_glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC)IntGetProcAddress("glGetVertexAttribdv");
	if(!_funcptr_glGetVertexAttribdv) ++numFailed;
	_funcptr_glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC)IntGetProcAddress("glGetVertexAttribfv");
	if(!_funcptr_glGetVertexAttribfv) ++numFailed;
	_funcptr_glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC)IntGetProcAddress("glGetVertexAttribiv");
	if(!_funcptr_glGetVertexAttribiv) ++numFailed;
	_funcptr_glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC)IntGetProcAddress("glGetVertexAttribPointerv");
	if(!_funcptr_glGetVertexAttribPointerv) ++numFailed;
	_funcptr_glIsProgram = (PFNGLISPROGRAMPROC)IntGetProcAddress("glIsProgram");
	if(!_funcptr_glIsProgram) ++numFailed;
	_funcptr_glIsShader = (PFNGLISSHADERPROC)IntGetProcAddress("glIsShader");
	if(!_funcptr_glIsShader) ++numFailed;
	_funcptr_glLinkProgram = (PFNGLLINKPROGRAMPROC)IntGetProcAddress("glLinkProgram");
	if(!_funcptr_glLinkProgram) ++numFailed;
	_funcptr_glShaderSource = (PFNGLSHADERSOURCEPROC)IntGetProcAddress("glShaderSource");
	if(!_funcptr_glShaderSource) ++numFailed;
	_funcptr_glUseProgram = (PFNGLUSEPROGRAMPROC)IntGetProcAddress("glUseProgram");
	if(!_funcptr_glUseProgram) ++numFailed;
	_funcptr_glUniform1f = (PFNGLUNIFORM1FPROC)IntGetProcAddress("glUniform1f");
	if(!_funcptr_glUniform1f) ++numFailed;
	_funcptr_glUniform2f = (PFNGLUNIFORM2FPROC)IntGetProcAddress("glUniform2f");
	if(!_funcptr_glUniform2f) ++numFailed;
	_funcptr_glUniform3f = (PFNGLUNIFORM3FPROC)IntGetProcAddress("glUniform3f");
	if(!_funcptr_glUniform3f) ++numFailed;
	_funcptr_glUniform4f = (PFNGLUNIFORM4FPROC)IntGetProcAddress("glUniform4f");
	if(!_funcptr_glUniform4f) ++numFailed;
	_funcptr_glUniform1i = (PFNGLUNIFORM1IPROC)IntGetProcAddress("glUniform1i");
	if(!_funcptr_glUniform1i) ++numFailed;
	_funcptr_glUniform2i = (PFNGLUNIFORM2IPROC)IntGetProcAddress("glUniform2i");
	if(!_funcptr_glUniform2i) ++numFailed;
	_funcptr_glUniform3i = (PFNGLUNIFORM3IPROC)IntGetProcAddress("glUniform3i");
	if(!_funcptr_glUniform3i) ++numFailed;
	_funcptr_glUniform4i = (PFNGLUNIFORM4IPROC)IntGetProcAddress("glUniform4i");
	if(!_funcptr_glUniform4i) ++numFailed;
	_funcptr_glUniform1fv = (PFNGLUNIFORM1FVPROC)IntGetProcAddress("glUniform1fv");
	if(!_funcptr_glUniform1fv) ++numFailed;
	_funcptr_glUniform2fv = (PFNGLUNIFORM2FVPROC)IntGetProcAddress("glUniform2fv");
	if(!_funcptr_glUniform2fv) ++numFailed;
	_funcptr_glUniform3fv = (PFNGLUNIFORM3FVPROC)IntGetProcAddress("glUniform3fv");
	if(!_funcptr_glUniform3fv) ++numFailed;
	_funcptr_glUniform4fv = (PFNGLUNIFORM4FVPROC)IntGetProcAddress("glUniform4fv");
	if(!_funcptr_glUniform4fv) ++numFailed;
	_funcptr_glUniform1iv = (PFNGLUNIFORM1IVPROC)IntGetProcAddress("glUniform1iv");
	if(!_funcptr_glUniform1iv) ++numFailed;
	_funcptr_glUniform2iv = (PFNGLUNIFORM2IVPROC)IntGetProcAddress("glUniform2iv");
	if(!_funcptr_glUniform2iv) ++numFailed;
	_funcptr_glUniform3iv = (PFNGLUNIFORM3IVPROC)IntGetProcAddress("glUniform3iv");
	if(!_funcptr_glUniform3iv) ++numFailed;
	_funcptr_glUniform4iv = (PFNGLUNIFORM4IVPROC)IntGetProcAddress("glUniform4iv");
	if(!_funcptr_glUniform4iv) ++numFailed;
	_funcptr_glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC)IntGetProcAddress("glUniformMatrix2fv");
	if(!_funcptr_glUniformMatrix2fv) ++numFailed;
	_funcptr_glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)IntGetProcAddress("glUniformMatrix3fv");
	if(!_funcptr_glUniformMatrix3fv) ++numFailed;
	_funcptr_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)IntGetProcAddress("glUniformMatrix4fv");
	if(!_funcptr_glUniformMatrix4fv) ++numFailed;
	_funcptr_glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)IntGetProcAddress("glValidateProgram");
	if(!_funcptr_glValidateProgram) ++numFailed;
	_funcptr_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)IntGetProcAddress("glVertexAttribPointer");
	if(!_funcptr_glVertexAttribPointer) ++numFailed;

// moved here by cinder:andrewfb
	_funcptr_glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC)IntGetProcAddress("glVertexAttrib1d");
	if(!_funcptr_glVertexAttrib1d) ++numFailed;
	_funcptr_glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC)IntGetProcAddress("glVertexAttrib1dv");
	if(!_funcptr_glVertexAttrib1dv) ++numFailed;
	_funcptr_glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)IntGetProcAddress("glVertexAttrib1f");
	if(!_funcptr_glVertexAttrib1f) ++numFailed;
	_funcptr_glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)IntGetProcAddress("glVertexAttrib1fv");
	if(!_funcptr_glVertexAttrib1fv) ++numFailed;
	_funcptr_glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC)IntGetProcAddress("glVertexAttrib1s");
	if(!_funcptr_glVertexAttrib1s) ++numFailed;
	_funcptr_glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC)IntGetProcAddress("glVertexAttrib1sv");
	if(!_funcptr_glVertexAttrib1sv) ++numFailed;
	_funcptr_glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC)IntGetProcAddress("glVertexAttrib2d");
	if(!_funcptr_glVertexAttrib2d) ++numFailed;
	_funcptr_glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC)IntGetProcAddress("glVertexAttrib2dv");
	if(!_funcptr_glVertexAttrib2dv) ++numFailed;
	_funcptr_glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC)IntGetProcAddress("glVertexAttrib2f");
	if(!_funcptr_glVertexAttrib2f) ++numFailed;
	_funcptr_glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)IntGetProcAddress("glVertexAttrib2fv");
	if(!_funcptr_glVertexAttrib2fv) ++numFailed;
	_funcptr_glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC)IntGetProcAddress("glVertexAttrib2s");
	if(!_funcptr_glVertexAttrib2s) ++numFailed;
	_funcptr_glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC)IntGetProcAddress("glVertexAttrib2sv");
	if(!_funcptr_glVertexAttrib2sv) ++numFailed;
	_funcptr_glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC)IntGetProcAddress("glVertexAttrib3d");
	if(!_funcptr_glVertexAttrib3d) ++numFailed;
	_funcptr_glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC)IntGetProcAddress("glVertexAttrib3dv");
	if(!_funcptr_glVertexAttrib3dv) ++numFailed;
	_funcptr_glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)IntGetProcAddress("glVertexAttrib3f");
	if(!_funcptr_glVertexAttrib3f) ++numFailed;
	_funcptr_glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)IntGetProcAddress("glVertexAttrib3fv");
	if(!_funcptr_glVertexAttrib3fv) ++numFailed;
	_funcptr_glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC)IntGetProcAddress("glVertexAttrib3s");
	if(!_funcptr_glVertexAttrib3s) ++numFailed;
	_funcptr_glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC)IntGetProcAddress("glVertexAttrib3sv");
	if(!_funcptr_glVertexAttrib3sv) ++numFailed;
	_funcptr_glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC)IntGetProcAddress("glVertexAttrib4Nbv");
	if(!_funcptr_glVertexAttrib4Nbv) ++numFailed;
	_funcptr_glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC)IntGetProcAddress("glVertexAttrib4Niv");
	if(!_funcptr_glVertexAttrib4Niv) ++numFailed;
	_funcptr_glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC)IntGetProcAddress("glVertexAttrib4Nsv");
	if(!_funcptr_glVertexAttrib4Nsv) ++numFailed;
	_funcptr_glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC)IntGetProcAddress("glVertexAttrib4Nub");
	if(!_funcptr_glVertexAttrib4Nub) ++numFailed;
	_funcptr_glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC)IntGetProcAddress("glVertexAttrib4Nubv");
	if(!_funcptr_glVertexAttrib4Nubv) ++numFailed;
	_funcptr_glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC)IntGetProcAddress("glVertexAttrib4Nuiv");
	if(!_funcptr_glVertexAttrib4Nuiv) ++numFailed;
	_funcptr_glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC)IntGetProcAddress("glVertexAttrib4Nusv");
	if(!_funcptr_glVertexAttrib4Nusv) ++numFailed;
	_funcptr_glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC)IntGetProcAddress("glVertexAttrib4bv");
	if(!_funcptr_glVertexAttrib4bv) ++numFailed;
	_funcptr_glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC)IntGetProcAddress("glVertexAttrib4d");
	if(!_funcptr_glVertexAttrib4d) ++numFailed;
	_funcptr_glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC)IntGetProcAddress("glVertexAttrib4dv");
	if(!_funcptr_glVertexAttrib4dv) ++numFailed;
	_funcptr_glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)IntGetProcAddress("glVertexAttrib4f");
	if(!_funcptr_glVertexAttrib4f) ++numFailed;
	_funcptr_glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)IntGetProcAddress("glVertexAttrib4fv");
	if(!_funcptr_glVertexAttrib4fv) ++numFailed;
	_funcptr_glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC)IntGetProcAddress("glVertexAttrib4iv");
	if(!_funcptr_glVertexAttrib4iv) ++numFailed;
	_funcptr_glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC)IntGetProcAddress("glVertexAttrib4s");
	if(!_funcptr_glVertexAttrib4s) ++numFailed;
	_funcptr_glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC)IntGetProcAddress("glVertexAttrib4sv");
	if(!_funcptr_glVertexAttrib4sv) ++numFailed;
	_funcptr_glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC)IntGetProcAddress("glVertexAttrib4ubv");
	if(!_funcptr_glVertexAttrib4ubv) ++numFailed;
	_funcptr_glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC)IntGetProcAddress("glVertexAttrib4uiv");
	if(!_funcptr_glVertexAttrib4uiv) ++numFailed;
	_funcptr_glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC)IntGetProcAddress("glVertexAttrib4usv");
	if(!_funcptr_glVertexAttrib4usv) ++numFailed;
// end cinder:andrewfb move
	
	return numFailed;
}

static int LoadCore_Version_2_0_Comp()
{
	int numFailed = 0;
/*	_funcptr_glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC)IntGetProcAddress("glVertexAttrib1d");
	if(!_funcptr_glVertexAttrib1d) ++numFailed;
	_funcptr_glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC)IntGetProcAddress("glVertexAttrib1dv");
	if(!_funcptr_glVertexAttrib1dv) ++numFailed;
	_funcptr_glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)IntGetProcAddress("glVertexAttrib1f");
	if(!_funcptr_glVertexAttrib1f) ++numFailed;
	_funcptr_glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)IntGetProcAddress("glVertexAttrib1fv");
	if(!_funcptr_glVertexAttrib1fv) ++numFailed;
	_funcptr_glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC)IntGetProcAddress("glVertexAttrib1s");
	if(!_funcptr_glVertexAttrib1s) ++numFailed;
	_funcptr_glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC)IntGetProcAddress("glVertexAttrib1sv");
	if(!_funcptr_glVertexAttrib1sv) ++numFailed;
	_funcptr_glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC)IntGetProcAddress("glVertexAttrib2d");
	if(!_funcptr_glVertexAttrib2d) ++numFailed;
	_funcptr_glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC)IntGetProcAddress("glVertexAttrib2dv");
	if(!_funcptr_glVertexAttrib2dv) ++numFailed;
	_funcptr_glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC)IntGetProcAddress("glVertexAttrib2f");
	if(!_funcptr_glVertexAttrib2f) ++numFailed;
	_funcptr_glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)IntGetProcAddress("glVertexAttrib2fv");
	if(!_funcptr_glVertexAttrib2fv) ++numFailed;
	_funcptr_glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC)IntGetProcAddress("glVertexAttrib2s");
	if(!_funcptr_glVertexAttrib2s) ++numFailed;
	_funcptr_glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC)IntGetProcAddress("glVertexAttrib2sv");
	if(!_funcptr_glVertexAttrib2sv) ++numFailed;
	_funcptr_glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC)IntGetProcAddress("glVertexAttrib3d");
	if(!_funcptr_glVertexAttrib3d) ++numFailed;
	_funcptr_glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC)IntGetProcAddress("glVertexAttrib3dv");
	if(!_funcptr_glVertexAttrib3dv) ++numFailed;
	_funcptr_glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)IntGetProcAddress("glVertexAttrib3f");
	if(!_funcptr_glVertexAttrib3f) ++numFailed;
	_funcptr_glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)IntGetProcAddress("glVertexAttrib3fv");
	if(!_funcptr_glVertexAttrib3fv) ++numFailed;
	_funcptr_glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC)IntGetProcAddress("glVertexAttrib3s");
	if(!_funcptr_glVertexAttrib3s) ++numFailed;
	_funcptr_glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC)IntGetProcAddress("glVertexAttrib3sv");
	if(!_funcptr_glVertexAttrib3sv) ++numFailed;
	_funcptr_glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC)IntGetProcAddress("glVertexAttrib4Nbv");
	if(!_funcptr_glVertexAttrib4Nbv) ++numFailed;
	_funcptr_glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC)IntGetProcAddress("glVertexAttrib4Niv");
	if(!_funcptr_glVertexAttrib4Niv) ++numFailed;
	_funcptr_glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC)IntGetProcAddress("glVertexAttrib4Nsv");
	if(!_funcptr_glVertexAttrib4Nsv) ++numFailed;
	_funcptr_glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC)IntGetProcAddress("glVertexAttrib4Nub");
	if(!_funcptr_glVertexAttrib4Nub) ++numFailed;
	_funcptr_glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC)IntGetProcAddress("glVertexAttrib4Nubv");
	if(!_funcptr_glVertexAttrib4Nubv) ++numFailed;
	_funcptr_glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC)IntGetProcAddress("glVertexAttrib4Nuiv");
	if(!_funcptr_glVertexAttrib4Nuiv) ++numFailed;
	_funcptr_glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC)IntGetProcAddress("glVertexAttrib4Nusv");
	if(!_funcptr_glVertexAttrib4Nusv) ++numFailed;
	_funcptr_glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC)IntGetProcAddress("glVertexAttrib4bv");
	if(!_funcptr_glVertexAttrib4bv) ++numFailed;
	_funcptr_glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC)IntGetProcAddress("glVertexAttrib4d");
	if(!_funcptr_glVertexAttrib4d) ++numFailed;
	_funcptr_glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC)IntGetProcAddress("glVertexAttrib4dv");
	if(!_funcptr_glVertexAttrib4dv) ++numFailed;
	_funcptr_glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)IntGetProcAddress("glVertexAttrib4f");
	if(!_funcptr_glVertexAttrib4f) ++numFailed;
	_funcptr_glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)IntGetProcAddress("glVertexAttrib4fv");
	if(!_funcptr_glVertexAttrib4fv) ++numFailed;
	_funcptr_glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC)IntGetProcAddress("glVertexAttrib4iv");
	if(!_funcptr_glVertexAttrib4iv) ++numFailed;
	_funcptr_glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC)IntGetProcAddress("glVertexAttrib4s");
	if(!_funcptr_glVertexAttrib4s) ++numFailed;
	_funcptr_glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC)IntGetProcAddress("glVertexAttrib4sv");
	if(!_funcptr_glVertexAttrib4sv) ++numFailed;
	_funcptr_glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC)IntGetProcAddress("glVertexAttrib4ubv");
	if(!_funcptr_glVertexAttrib4ubv) ++numFailed;
	_funcptr_glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC)IntGetProcAddress("glVertexAttrib4uiv");
	if(!_funcptr_glVertexAttrib4uiv) ++numFailed;
	_funcptr_glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC)IntGetProcAddress("glVertexAttrib4usv");
	if(!_funcptr_glVertexAttrib4usv) ++numFailed;*/
	return numFailed;
}
static int LoadCore_Version_2_1()
{
	int numFailed = 0;
	_funcptr_glUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC)IntGetProcAddress("glUniformMatrix2x3fv");
	if(!_funcptr_glUniformMatrix2x3fv) ++numFailed;
	_funcptr_glUniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC)IntGetProcAddress("glUniformMatrix3x2fv");
	if(!_funcptr_glUniformMatrix3x2fv) ++numFailed;
	_funcptr_glUniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC)IntGetProcAddress("glUniformMatrix2x4fv");
	if(!_funcptr_glUniformMatrix2x4fv) ++numFailed;
	_funcptr_glUniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC)IntGetProcAddress("glUniformMatrix4x2fv");
	if(!_funcptr_glUniformMatrix4x2fv) ++numFailed;
	_funcptr_glUniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC)IntGetProcAddress("glUniformMatrix3x4fv");
	if(!_funcptr_glUniformMatrix3x4fv) ++numFailed;
	_funcptr_glUniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FVPROC)IntGetProcAddress("glUniformMatrix4x3fv");
	if(!_funcptr_glUniformMatrix4x3fv) ++numFailed;
	return numFailed;
}

static int LoadCore_Version_3_0()
{
	int numFailed = 0;
	_funcptr_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)IntGetProcAddress("glBindVertexArray");
	if(!_funcptr_glBindVertexArray) ++numFailed;
	_funcptr_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)IntGetProcAddress("glDeleteVertexArrays");
	if(!_funcptr_glDeleteVertexArrays) ++numFailed;
	_funcptr_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)IntGetProcAddress("glGenVertexArrays");
	if(!_funcptr_glGenVertexArrays) ++numFailed;
	_funcptr_glIsVertexArray = (PFNGLISVERTEXARRAYPROC)IntGetProcAddress("glIsVertexArray");
	if(!_funcptr_glIsVertexArray) ++numFailed;
	_funcptr_glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)IntGetProcAddress("glMapBufferRange");
	if(!_funcptr_glMapBufferRange) ++numFailed;
	_funcptr_glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC)IntGetProcAddress("glFlushMappedBufferRange");
	if(!_funcptr_glFlushMappedBufferRange) ++numFailed;
	_funcptr_glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)IntGetProcAddress("glIsRenderbuffer");
	if(!_funcptr_glIsRenderbuffer) ++numFailed;
	_funcptr_glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)IntGetProcAddress("glBindRenderbuffer");
	if(!_funcptr_glBindRenderbuffer) ++numFailed;
	_funcptr_glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)IntGetProcAddress("glDeleteRenderbuffers");
	if(!_funcptr_glDeleteRenderbuffers) ++numFailed;
	_funcptr_glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)IntGetProcAddress("glGenRenderbuffers");
	if(!_funcptr_glGenRenderbuffers) ++numFailed;
	_funcptr_glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)IntGetProcAddress("glRenderbufferStorage");
	if(!_funcptr_glRenderbufferStorage) ++numFailed;
	_funcptr_glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)IntGetProcAddress("glGetRenderbufferParameteriv");
	if(!_funcptr_glGetRenderbufferParameteriv) ++numFailed;
	_funcptr_glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)IntGetProcAddress("glIsFramebuffer");
	if(!_funcptr_glIsFramebuffer) ++numFailed;
	_funcptr_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)IntGetProcAddress("glBindFramebuffer");
	if(!_funcptr_glBindFramebuffer) ++numFailed;
	_funcptr_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)IntGetProcAddress("glDeleteFramebuffers");
	if(!_funcptr_glDeleteFramebuffers) ++numFailed;
	_funcptr_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)IntGetProcAddress("glGenFramebuffers");
	if(!_funcptr_glGenFramebuffers) ++numFailed;
	_funcptr_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)IntGetProcAddress("glCheckFramebufferStatus");
	if(!_funcptr_glCheckFramebufferStatus) ++numFailed;
	_funcptr_glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)IntGetProcAddress("glFramebufferTexture1D");
	if(!_funcptr_glFramebufferTexture1D) ++numFailed;
	_funcptr_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)IntGetProcAddress("glFramebufferTexture2D");
	if(!_funcptr_glFramebufferTexture2D) ++numFailed;
	_funcptr_glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)IntGetProcAddress("glFramebufferTexture3D");
	if(!_funcptr_glFramebufferTexture3D) ++numFailed;
	_funcptr_glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)IntGetProcAddress("glFramebufferRenderbuffer");
	if(!_funcptr_glFramebufferRenderbuffer) ++numFailed;
	_funcptr_glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)IntGetProcAddress("glGetFramebufferAttachmentParameteriv");
	if(!_funcptr_glGetFramebufferAttachmentParameteriv) ++numFailed;
	_funcptr_glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)IntGetProcAddress("glGenerateMipmap");
	if(!_funcptr_glGenerateMipmap) ++numFailed;
	_funcptr_glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)IntGetProcAddress("glBlitFramebuffer");
	if(!_funcptr_glBlitFramebuffer) ++numFailed;
	_funcptr_glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)IntGetProcAddress("glRenderbufferStorageMultisample");
	if(!_funcptr_glRenderbufferStorageMultisample) ++numFailed;
	_funcptr_glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)IntGetProcAddress("glFramebufferTextureLayer");
	if(!_funcptr_glFramebufferTextureLayer) ++numFailed;
	_funcptr_glColorMaski = (PFNGLCOLORMASKIPROC)IntGetProcAddress("glColorMaski");
	if(!_funcptr_glColorMaski) ++numFailed;
	_funcptr_glGetBooleani_v = (PFNGLGETBOOLEANI_VPROC)IntGetProcAddress("glGetBooleani_v");
	if(!_funcptr_glGetBooleani_v) ++numFailed;
	_funcptr_glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC)IntGetProcAddress("glGetIntegeri_v");
	if(!_funcptr_glGetIntegeri_v) ++numFailed;
	_funcptr_glEnablei = (PFNGLENABLEIPROC)IntGetProcAddress("glEnablei");
	if(!_funcptr_glEnablei) ++numFailed;
	_funcptr_glDisablei = (PFNGLDISABLEIPROC)IntGetProcAddress("glDisablei");
	if(!_funcptr_glDisablei) ++numFailed;
	_funcptr_glIsEnabledi = (PFNGLISENABLEDIPROC)IntGetProcAddress("glIsEnabledi");
	if(!_funcptr_glIsEnabledi) ++numFailed;
	_funcptr_glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC)IntGetProcAddress("glBeginTransformFeedback");
	if(!_funcptr_glBeginTransformFeedback) ++numFailed;
	_funcptr_glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC)IntGetProcAddress("glEndTransformFeedback");
	if(!_funcptr_glEndTransformFeedback) ++numFailed;
	_funcptr_glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)IntGetProcAddress("glBindBufferRange");
	if(!_funcptr_glBindBufferRange) ++numFailed;
	_funcptr_glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)IntGetProcAddress("glBindBufferBase");
	if(!_funcptr_glBindBufferBase) ++numFailed;
	_funcptr_glTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC)IntGetProcAddress("glTransformFeedbackVaryings");
	if(!_funcptr_glTransformFeedbackVaryings) ++numFailed;
	_funcptr_glGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC)IntGetProcAddress("glGetTransformFeedbackVarying");
	if(!_funcptr_glGetTransformFeedbackVarying) ++numFailed;
	_funcptr_glClampColor = (PFNGLCLAMPCOLORPROC)IntGetProcAddress("glClampColor");
	if(!_funcptr_glClampColor) ++numFailed;
	_funcptr_glBeginConditionalRender = (PFNGLBEGINCONDITIONALRENDERPROC)IntGetProcAddress("glBeginConditionalRender");
	if(!_funcptr_glBeginConditionalRender) ++numFailed;
	_funcptr_glEndConditionalRender = (PFNGLENDCONDITIONALRENDERPROC)IntGetProcAddress("glEndConditionalRender");
	if(!_funcptr_glEndConditionalRender) ++numFailed;
	_funcptr_glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)IntGetProcAddress("glVertexAttribIPointer");
	if(!_funcptr_glVertexAttribIPointer) ++numFailed;
	_funcptr_glGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC)IntGetProcAddress("glGetVertexAttribIiv");
	if(!_funcptr_glGetVertexAttribIiv) ++numFailed;
	_funcptr_glGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC)IntGetProcAddress("glGetVertexAttribIuiv");
	if(!_funcptr_glGetVertexAttribIuiv) ++numFailed;
	_funcptr_glVertexAttribI1i = (PFNGLVERTEXATTRIBI1IPROC)IntGetProcAddress("glVertexAttribI1i");
	if(!_funcptr_glVertexAttribI1i) ++numFailed;
	_funcptr_glVertexAttribI2i = (PFNGLVERTEXATTRIBI2IPROC)IntGetProcAddress("glVertexAttribI2i");
	if(!_funcptr_glVertexAttribI2i) ++numFailed;
	_funcptr_glVertexAttribI3i = (PFNGLVERTEXATTRIBI3IPROC)IntGetProcAddress("glVertexAttribI3i");
	if(!_funcptr_glVertexAttribI3i) ++numFailed;
	_funcptr_glVertexAttribI4i = (PFNGLVERTEXATTRIBI4IPROC)IntGetProcAddress("glVertexAttribI4i");
	if(!_funcptr_glVertexAttribI4i) ++numFailed;
	_funcptr_glVertexAttribI1ui = (PFNGLVERTEXATTRIBI1UIPROC)IntGetProcAddress("glVertexAttribI1ui");
	if(!_funcptr_glVertexAttribI1ui) ++numFailed;
	_funcptr_glVertexAttribI2ui = (PFNGLVERTEXATTRIBI2UIPROC)IntGetProcAddress("glVertexAttribI2ui");
	if(!_funcptr_glVertexAttribI2ui) ++numFailed;
	_funcptr_glVertexAttribI3ui = (PFNGLVERTEXATTRIBI3UIPROC)IntGetProcAddress("glVertexAttribI3ui");
	if(!_funcptr_glVertexAttribI3ui) ++numFailed;
	_funcptr_glVertexAttribI4ui = (PFNGLVERTEXATTRIBI4UIPROC)IntGetProcAddress("glVertexAttribI4ui");
	if(!_funcptr_glVertexAttribI4ui) ++numFailed;
	_funcptr_glVertexAttribI1iv = (PFNGLVERTEXATTRIBI1IVPROC)IntGetProcAddress("glVertexAttribI1iv");
	if(!_funcptr_glVertexAttribI1iv) ++numFailed;
	_funcptr_glVertexAttribI2iv = (PFNGLVERTEXATTRIBI2IVPROC)IntGetProcAddress("glVertexAttribI2iv");
	if(!_funcptr_glVertexAttribI2iv) ++numFailed;
	_funcptr_glVertexAttribI3iv = (PFNGLVERTEXATTRIBI3IVPROC)IntGetProcAddress("glVertexAttribI3iv");
	if(!_funcptr_glVertexAttribI3iv) ++numFailed;
	_funcptr_glVertexAttribI4iv = (PFNGLVERTEXATTRIBI4IVPROC)IntGetProcAddress("glVertexAttribI4iv");
	if(!_funcptr_glVertexAttribI4iv) ++numFailed;
	_funcptr_glVertexAttribI1uiv = (PFNGLVERTEXATTRIBI1UIVPROC)IntGetProcAddress("glVertexAttribI1uiv");
	if(!_funcptr_glVertexAttribI1uiv) ++numFailed;
	_funcptr_glVertexAttribI2uiv = (PFNGLVERTEXATTRIBI2UIVPROC)IntGetProcAddress("glVertexAttribI2uiv");
	if(!_funcptr_glVertexAttribI2uiv) ++numFailed;
	_funcptr_glVertexAttribI3uiv = (PFNGLVERTEXATTRIBI3UIVPROC)IntGetProcAddress("glVertexAttribI3uiv");
	if(!_funcptr_glVertexAttribI3uiv) ++numFailed;
	_funcptr_glVertexAttribI4uiv = (PFNGLVERTEXATTRIBI4UIVPROC)IntGetProcAddress("glVertexAttribI4uiv");
	if(!_funcptr_glVertexAttribI4uiv) ++numFailed;
	_funcptr_glVertexAttribI4bv = (PFNGLVERTEXATTRIBI4BVPROC)IntGetProcAddress("glVertexAttribI4bv");
	if(!_funcptr_glVertexAttribI4bv) ++numFailed;
	_funcptr_glVertexAttribI4sv = (PFNGLVERTEXATTRIBI4SVPROC)IntGetProcAddress("glVertexAttribI4sv");
	if(!_funcptr_glVertexAttribI4sv) ++numFailed;
	_funcptr_glVertexAttribI4ubv = (PFNGLVERTEXATTRIBI4UBVPROC)IntGetProcAddress("glVertexAttribI4ubv");
	if(!_funcptr_glVertexAttribI4ubv) ++numFailed;
	_funcptr_glVertexAttribI4usv = (PFNGLVERTEXATTRIBI4USVPROC)IntGetProcAddress("glVertexAttribI4usv");
	if(!_funcptr_glVertexAttribI4usv) ++numFailed;
	_funcptr_glGetUniformuiv = (PFNGLGETUNIFORMUIVPROC)IntGetProcAddress("glGetUniformuiv");
	if(!_funcptr_glGetUniformuiv) ++numFailed;
	_funcptr_glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC)IntGetProcAddress("glBindFragDataLocation");
	if(!_funcptr_glBindFragDataLocation) ++numFailed;
	_funcptr_glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC)IntGetProcAddress("glGetFragDataLocation");
	if(!_funcptr_glGetFragDataLocation) ++numFailed;
	_funcptr_glUniform1ui = (PFNGLUNIFORM1UIPROC)IntGetProcAddress("glUniform1ui");
	if(!_funcptr_glUniform1ui) ++numFailed;
	_funcptr_glUniform2ui = (PFNGLUNIFORM2UIPROC)IntGetProcAddress("glUniform2ui");
	if(!_funcptr_glUniform2ui) ++numFailed;
	_funcptr_glUniform3ui = (PFNGLUNIFORM3UIPROC)IntGetProcAddress("glUniform3ui");
	if(!_funcptr_glUniform3ui) ++numFailed;
	_funcptr_glUniform4ui = (PFNGLUNIFORM4UIPROC)IntGetProcAddress("glUniform4ui");
	if(!_funcptr_glUniform4ui) ++numFailed;
	_funcptr_glUniform1uiv = (PFNGLUNIFORM1UIVPROC)IntGetProcAddress("glUniform1uiv");
	if(!_funcptr_glUniform1uiv) ++numFailed;
	_funcptr_glUniform2uiv = (PFNGLUNIFORM2UIVPROC)IntGetProcAddress("glUniform2uiv");
	if(!_funcptr_glUniform2uiv) ++numFailed;
	_funcptr_glUniform3uiv = (PFNGLUNIFORM3UIVPROC)IntGetProcAddress("glUniform3uiv");
	if(!_funcptr_glUniform3uiv) ++numFailed;
	_funcptr_glUniform4uiv = (PFNGLUNIFORM4UIVPROC)IntGetProcAddress("glUniform4uiv");
	if(!_funcptr_glUniform4uiv) ++numFailed;
	_funcptr_glTexParameterIiv = (PFNGLTEXPARAMETERIIVPROC)IntGetProcAddress("glTexParameterIiv");
	if(!_funcptr_glTexParameterIiv) ++numFailed;
	_funcptr_glTexParameterIuiv = (PFNGLTEXPARAMETERIUIVPROC)IntGetProcAddress("glTexParameterIuiv");
	if(!_funcptr_glTexParameterIuiv) ++numFailed;
	_funcptr_glGetTexParameterIiv = (PFNGLGETTEXPARAMETERIIVPROC)IntGetProcAddress("glGetTexParameterIiv");
	if(!_funcptr_glGetTexParameterIiv) ++numFailed;
	_funcptr_glGetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIVPROC)IntGetProcAddress("glGetTexParameterIuiv");
	if(!_funcptr_glGetTexParameterIuiv) ++numFailed;
	_funcptr_glClearBufferiv = (PFNGLCLEARBUFFERIVPROC)IntGetProcAddress("glClearBufferiv");
	if(!_funcptr_glClearBufferiv) ++numFailed;
	_funcptr_glClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC)IntGetProcAddress("glClearBufferuiv");
	if(!_funcptr_glClearBufferuiv) ++numFailed;
	_funcptr_glClearBufferfv = (PFNGLCLEARBUFFERFVPROC)IntGetProcAddress("glClearBufferfv");
	if(!_funcptr_glClearBufferfv) ++numFailed;
	_funcptr_glClearBufferfi = (PFNGLCLEARBUFFERFIPROC)IntGetProcAddress("glClearBufferfi");
	if(!_funcptr_glClearBufferfi) ++numFailed;
	_funcptr_glGetStringi = (PFNGLGETSTRINGIPROC)IntGetProcAddress("glGetStringi");
	if(!_funcptr_glGetStringi) ++numFailed;
	return numFailed;
}

static int LoadCore_Version_3_1()
{
	int numFailed = 0;
	_funcptr_glGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC)IntGetProcAddress("glGetUniformIndices");
	if(!_funcptr_glGetUniformIndices) ++numFailed;
	_funcptr_glGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC)IntGetProcAddress("glGetActiveUniformsiv");
	if(!_funcptr_glGetActiveUniformsiv) ++numFailed;
	_funcptr_glGetActiveUniformName = (PFNGLGETACTIVEUNIFORMNAMEPROC)IntGetProcAddress("glGetActiveUniformName");
	if(!_funcptr_glGetActiveUniformName) ++numFailed;
	_funcptr_glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC)IntGetProcAddress("glGetUniformBlockIndex");
	if(!_funcptr_glGetUniformBlockIndex) ++numFailed;
	_funcptr_glGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC)IntGetProcAddress("glGetActiveUniformBlockiv");
	if(!_funcptr_glGetActiveUniformBlockiv) ++numFailed;
	_funcptr_glGetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC)IntGetProcAddress("glGetActiveUniformBlockName");
	if(!_funcptr_glGetActiveUniformBlockName) ++numFailed;
	_funcptr_glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC)IntGetProcAddress("glUniformBlockBinding");
	if(!_funcptr_glUniformBlockBinding) ++numFailed;
	_funcptr_glCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC)IntGetProcAddress("glCopyBufferSubData");
	if(!_funcptr_glCopyBufferSubData) ++numFailed;
	_funcptr_glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC)IntGetProcAddress("glDrawArraysInstanced");
	if(!_funcptr_glDrawArraysInstanced) ++numFailed;
	_funcptr_glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC)IntGetProcAddress("glDrawElementsInstanced");
	if(!_funcptr_glDrawElementsInstanced) ++numFailed;
	_funcptr_glTexBuffer = (PFNGLTEXBUFFERPROC)IntGetProcAddress("glTexBuffer");
	if(!_funcptr_glTexBuffer) ++numFailed;
	_funcptr_glPrimitiveRestartIndex = (PFNGLPRIMITIVERESTARTINDEXPROC)IntGetProcAddress("glPrimitiveRestartIndex");
	if(!_funcptr_glPrimitiveRestartIndex) ++numFailed;
	return numFailed;
}

static int LoadCore_Version_3_2()
{
	int numFailed = 0;
	_funcptr_glDrawElementsBaseVertex = (PFNGLDRAWELEMENTSBASEVERTEXPROC)IntGetProcAddress("glDrawElementsBaseVertex");
	if(!_funcptr_glDrawElementsBaseVertex) ++numFailed;
	_funcptr_glDrawRangeElementsBaseVertex = (PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC)IntGetProcAddress("glDrawRangeElementsBaseVertex");
	if(!_funcptr_glDrawRangeElementsBaseVertex) ++numFailed;
	_funcptr_glDrawElementsInstancedBaseVertex = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC)IntGetProcAddress("glDrawElementsInstancedBaseVertex");
	if(!_funcptr_glDrawElementsInstancedBaseVertex) ++numFailed;
	_funcptr_glMultiDrawElementsBaseVertex = (PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC)IntGetProcAddress("glMultiDrawElementsBaseVertex");
	if(!_funcptr_glMultiDrawElementsBaseVertex) ++numFailed;
	_funcptr_glProvokingVertex = (PFNGLPROVOKINGVERTEXPROC)IntGetProcAddress("glProvokingVertex");
	if(!_funcptr_glProvokingVertex) ++numFailed;
	_funcptr_glFenceSync = (PFNGLFENCESYNCPROC)IntGetProcAddress("glFenceSync");
	if(!_funcptr_glFenceSync) ++numFailed;
	_funcptr_glIsSync = (PFNGLISSYNCPROC)IntGetProcAddress("glIsSync");
	if(!_funcptr_glIsSync) ++numFailed;
	_funcptr_glDeleteSync = (PFNGLDELETESYNCPROC)IntGetProcAddress("glDeleteSync");
	if(!_funcptr_glDeleteSync) ++numFailed;
	_funcptr_glClientWaitSync = (PFNGLCLIENTWAITSYNCPROC)IntGetProcAddress("glClientWaitSync");
	if(!_funcptr_glClientWaitSync) ++numFailed;
	_funcptr_glWaitSync = (PFNGLWAITSYNCPROC)IntGetProcAddress("glWaitSync");
	if(!_funcptr_glWaitSync) ++numFailed;
	_funcptr_glGetInteger64v = (PFNGLGETINTEGER64VPROC)IntGetProcAddress("glGetInteger64v");
	if(!_funcptr_glGetInteger64v) ++numFailed;
	_funcptr_glGetSynciv = (PFNGLGETSYNCIVPROC)IntGetProcAddress("glGetSynciv");
	if(!_funcptr_glGetSynciv) ++numFailed;
	_funcptr_glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)IntGetProcAddress("glTexImage2DMultisample");
	if(!_funcptr_glTexImage2DMultisample) ++numFailed;
	_funcptr_glTexImage3DMultisample = (PFNGLTEXIMAGE3DMULTISAMPLEPROC)IntGetProcAddress("glTexImage3DMultisample");
	if(!_funcptr_glTexImage3DMultisample) ++numFailed;
	_funcptr_glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC)IntGetProcAddress("glGetMultisamplefv");
	if(!_funcptr_glGetMultisamplefv) ++numFailed;
	_funcptr_glSampleMaski = (PFNGLSAMPLEMASKIPROC)IntGetProcAddress("glSampleMaski");
	if(!_funcptr_glSampleMaski) ++numFailed;
	_funcptr_glGetInteger64i_v = (PFNGLGETINTEGER64I_VPROC)IntGetProcAddress("glGetInteger64i_v");
	if(!_funcptr_glGetInteger64i_v) ++numFailed;
	_funcptr_glGetBufferParameteri64v = (PFNGLGETBUFFERPARAMETERI64VPROC)IntGetProcAddress("glGetBufferParameteri64v");
	if(!_funcptr_glGetBufferParameteri64v) ++numFailed;
	_funcptr_glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)IntGetProcAddress("glFramebufferTexture");
	if(!_funcptr_glFramebufferTexture) ++numFailed;
	return numFailed;
}

static int LoadCore_Version_3_3()
{
	int numFailed = 0;
	_funcptr_glQueryCounter = (PFNGLQUERYCOUNTERPROC)IntGetProcAddress("glQueryCounter");
	if(!_funcptr_glQueryCounter) ++numFailed;
	_funcptr_glGetQueryObjecti64v = (PFNGLGETQUERYOBJECTI64VPROC)IntGetProcAddress("glGetQueryObjecti64v");
	if(!_funcptr_glGetQueryObjecti64v) ++numFailed;
	_funcptr_glGetQueryObjectui64v = (PFNGLGETQUERYOBJECTUI64VPROC)IntGetProcAddress("glGetQueryObjectui64v");
	if(!_funcptr_glGetQueryObjectui64v) ++numFailed;
	_funcptr_glVertexP2ui = (PFNGLVERTEXP2UIPROC)IntGetProcAddress("glVertexP2ui");
	if(!_funcptr_glVertexP2ui) ++numFailed;
	_funcptr_glVertexP2uiv = (PFNGLVERTEXP2UIVPROC)IntGetProcAddress("glVertexP2uiv");
	if(!_funcptr_glVertexP2uiv) ++numFailed;
	_funcptr_glVertexP3ui = (PFNGLVERTEXP3UIPROC)IntGetProcAddress("glVertexP3ui");
	if(!_funcptr_glVertexP3ui) ++numFailed;
	_funcptr_glVertexP3uiv = (PFNGLVERTEXP3UIVPROC)IntGetProcAddress("glVertexP3uiv");
	if(!_funcptr_glVertexP3uiv) ++numFailed;
	_funcptr_glVertexP4ui = (PFNGLVERTEXP4UIPROC)IntGetProcAddress("glVertexP4ui");
	if(!_funcptr_glVertexP4ui) ++numFailed;
	_funcptr_glVertexP4uiv = (PFNGLVERTEXP4UIVPROC)IntGetProcAddress("glVertexP4uiv");
	if(!_funcptr_glVertexP4uiv) ++numFailed;
	_funcptr_glTexCoordP1ui = (PFNGLTEXCOORDP1UIPROC)IntGetProcAddress("glTexCoordP1ui");
	if(!_funcptr_glTexCoordP1ui) ++numFailed;
	_funcptr_glTexCoordP1uiv = (PFNGLTEXCOORDP1UIVPROC)IntGetProcAddress("glTexCoordP1uiv");
	if(!_funcptr_glTexCoordP1uiv) ++numFailed;
	_funcptr_glTexCoordP2ui = (PFNGLTEXCOORDP2UIPROC)IntGetProcAddress("glTexCoordP2ui");
	if(!_funcptr_glTexCoordP2ui) ++numFailed;
	_funcptr_glTexCoordP2uiv = (PFNGLTEXCOORDP2UIVPROC)IntGetProcAddress("glTexCoordP2uiv");
	if(!_funcptr_glTexCoordP2uiv) ++numFailed;
	_funcptr_glTexCoordP3ui = (PFNGLTEXCOORDP3UIPROC)IntGetProcAddress("glTexCoordP3ui");
	if(!_funcptr_glTexCoordP3ui) ++numFailed;
	_funcptr_glTexCoordP3uiv = (PFNGLTEXCOORDP3UIVPROC)IntGetProcAddress("glTexCoordP3uiv");
	if(!_funcptr_glTexCoordP3uiv) ++numFailed;
	_funcptr_glTexCoordP4ui = (PFNGLTEXCOORDP4UIPROC)IntGetProcAddress("glTexCoordP4ui");
	if(!_funcptr_glTexCoordP4ui) ++numFailed;
	_funcptr_glTexCoordP4uiv = (PFNGLTEXCOORDP4UIVPROC)IntGetProcAddress("glTexCoordP4uiv");
	if(!_funcptr_glTexCoordP4uiv) ++numFailed;
	_funcptr_glMultiTexCoordP1ui = (PFNGLMULTITEXCOORDP1UIPROC)IntGetProcAddress("glMultiTexCoordP1ui");
	if(!_funcptr_glMultiTexCoordP1ui) ++numFailed;
	_funcptr_glMultiTexCoordP1uiv = (PFNGLMULTITEXCOORDP1UIVPROC)IntGetProcAddress("glMultiTexCoordP1uiv");
	if(!_funcptr_glMultiTexCoordP1uiv) ++numFailed;
	_funcptr_glMultiTexCoordP2ui = (PFNGLMULTITEXCOORDP2UIPROC)IntGetProcAddress("glMultiTexCoordP2ui");
	if(!_funcptr_glMultiTexCoordP2ui) ++numFailed;
	_funcptr_glMultiTexCoordP2uiv = (PFNGLMULTITEXCOORDP2UIVPROC)IntGetProcAddress("glMultiTexCoordP2uiv");
	if(!_funcptr_glMultiTexCoordP2uiv) ++numFailed;
	_funcptr_glMultiTexCoordP3ui = (PFNGLMULTITEXCOORDP3UIPROC)IntGetProcAddress("glMultiTexCoordP3ui");
	if(!_funcptr_glMultiTexCoordP3ui) ++numFailed;
	_funcptr_glMultiTexCoordP3uiv = (PFNGLMULTITEXCOORDP3UIVPROC)IntGetProcAddress("glMultiTexCoordP3uiv");
	if(!_funcptr_glMultiTexCoordP3uiv) ++numFailed;
	_funcptr_glMultiTexCoordP4ui = (PFNGLMULTITEXCOORDP4UIPROC)IntGetProcAddress("glMultiTexCoordP4ui");
	if(!_funcptr_glMultiTexCoordP4ui) ++numFailed;
	_funcptr_glMultiTexCoordP4uiv = (PFNGLMULTITEXCOORDP4UIVPROC)IntGetProcAddress("glMultiTexCoordP4uiv");
	if(!_funcptr_glMultiTexCoordP4uiv) ++numFailed;
	_funcptr_glNormalP3ui = (PFNGLNORMALP3UIPROC)IntGetProcAddress("glNormalP3ui");
	if(!_funcptr_glNormalP3ui) ++numFailed;
	_funcptr_glNormalP3uiv = (PFNGLNORMALP3UIVPROC)IntGetProcAddress("glNormalP3uiv");
	if(!_funcptr_glNormalP3uiv) ++numFailed;
	_funcptr_glColorP3ui = (PFNGLCOLORP3UIPROC)IntGetProcAddress("glColorP3ui");
	if(!_funcptr_glColorP3ui) ++numFailed;
	_funcptr_glColorP3uiv = (PFNGLCOLORP3UIVPROC)IntGetProcAddress("glColorP3uiv");
	if(!_funcptr_glColorP3uiv) ++numFailed;
	_funcptr_glColorP4ui = (PFNGLCOLORP4UIPROC)IntGetProcAddress("glColorP4ui");
	if(!_funcptr_glColorP4ui) ++numFailed;
	_funcptr_glColorP4uiv = (PFNGLCOLORP4UIVPROC)IntGetProcAddress("glColorP4uiv");
	if(!_funcptr_glColorP4uiv) ++numFailed;
	_funcptr_glSecondaryColorP3ui = (PFNGLSECONDARYCOLORP3UIPROC)IntGetProcAddress("glSecondaryColorP3ui");
	if(!_funcptr_glSecondaryColorP3ui) ++numFailed;
	_funcptr_glSecondaryColorP3uiv = (PFNGLSECONDARYCOLORP3UIVPROC)IntGetProcAddress("glSecondaryColorP3uiv");
	if(!_funcptr_glSecondaryColorP3uiv) ++numFailed;
	_funcptr_glVertexAttribP1ui = (PFNGLVERTEXATTRIBP1UIPROC)IntGetProcAddress("glVertexAttribP1ui");
	if(!_funcptr_glVertexAttribP1ui) ++numFailed;
	_funcptr_glVertexAttribP1uiv = (PFNGLVERTEXATTRIBP1UIVPROC)IntGetProcAddress("glVertexAttribP1uiv");
	if(!_funcptr_glVertexAttribP1uiv) ++numFailed;
	_funcptr_glVertexAttribP2ui = (PFNGLVERTEXATTRIBP2UIPROC)IntGetProcAddress("glVertexAttribP2ui");
	if(!_funcptr_glVertexAttribP2ui) ++numFailed;
	_funcptr_glVertexAttribP2uiv = (PFNGLVERTEXATTRIBP2UIVPROC)IntGetProcAddress("glVertexAttribP2uiv");
	if(!_funcptr_glVertexAttribP2uiv) ++numFailed;
	_funcptr_glVertexAttribP3ui = (PFNGLVERTEXATTRIBP3UIPROC)IntGetProcAddress("glVertexAttribP3ui");
	if(!_funcptr_glVertexAttribP3ui) ++numFailed;
	_funcptr_glVertexAttribP3uiv = (PFNGLVERTEXATTRIBP3UIVPROC)IntGetProcAddress("glVertexAttribP3uiv");
	if(!_funcptr_glVertexAttribP3uiv) ++numFailed;
	_funcptr_glVertexAttribP4ui = (PFNGLVERTEXATTRIBP4UIPROC)IntGetProcAddress("glVertexAttribP4ui");
	if(!_funcptr_glVertexAttribP4ui) ++numFailed;
	_funcptr_glVertexAttribP4uiv = (PFNGLVERTEXATTRIBP4UIVPROC)IntGetProcAddress("glVertexAttribP4uiv");
	if(!_funcptr_glVertexAttribP4uiv) ++numFailed;
	_funcptr_glBindFragDataLocationIndexed = (PFNGLBINDFRAGDATALOCATIONINDEXEDPROC)IntGetProcAddress("glBindFragDataLocationIndexed");
	if(!_funcptr_glBindFragDataLocationIndexed) ++numFailed;
	_funcptr_glGetFragDataIndex = (PFNGLGETFRAGDATAINDEXPROC)IntGetProcAddress("glGetFragDataIndex");
	if(!_funcptr_glGetFragDataIndex) ++numFailed;
	_funcptr_glGenSamplers = (PFNGLGENSAMPLERSPROC)IntGetProcAddress("glGenSamplers");
	if(!_funcptr_glGenSamplers) ++numFailed;
	_funcptr_glDeleteSamplers = (PFNGLDELETESAMPLERSPROC)IntGetProcAddress("glDeleteSamplers");
	if(!_funcptr_glDeleteSamplers) ++numFailed;
	_funcptr_glIsSampler = (PFNGLISSAMPLERPROC)IntGetProcAddress("glIsSampler");
	if(!_funcptr_glIsSampler) ++numFailed;
	_funcptr_glBindSampler = (PFNGLBINDSAMPLERPROC)IntGetProcAddress("glBindSampler");
	if(!_funcptr_glBindSampler) ++numFailed;
	_funcptr_glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)IntGetProcAddress("glSamplerParameteri");
	if(!_funcptr_glSamplerParameteri) ++numFailed;
	_funcptr_glSamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC)IntGetProcAddress("glSamplerParameteriv");
	if(!_funcptr_glSamplerParameteriv) ++numFailed;
	_funcptr_glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC)IntGetProcAddress("glSamplerParameterf");
	if(!_funcptr_glSamplerParameterf) ++numFailed;
	_funcptr_glSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC)IntGetProcAddress("glSamplerParameterfv");
	if(!_funcptr_glSamplerParameterfv) ++numFailed;
	_funcptr_glSamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIVPROC)IntGetProcAddress("glSamplerParameterIiv");
	if(!_funcptr_glSamplerParameterIiv) ++numFailed;
	_funcptr_glSamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIVPROC)IntGetProcAddress("glSamplerParameterIuiv");
	if(!_funcptr_glSamplerParameterIuiv) ++numFailed;
	_funcptr_glGetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC)IntGetProcAddress("glGetSamplerParameteriv");
	if(!_funcptr_glGetSamplerParameteriv) ++numFailed;
	_funcptr_glGetSamplerParameterIiv = (PFNGLGETSAMPLERPARAMETERIIVPROC)IntGetProcAddress("glGetSamplerParameterIiv");
	if(!_funcptr_glGetSamplerParameterIiv) ++numFailed;
	_funcptr_glGetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC)IntGetProcAddress("glGetSamplerParameterfv");
	if(!_funcptr_glGetSamplerParameterfv) ++numFailed;
	_funcptr_glGetSamplerParameterIuiv = (PFNGLGETSAMPLERPARAMETERIUIVPROC)IntGetProcAddress("glGetSamplerParameterIuiv");
	if(!_funcptr_glGetSamplerParameterIuiv) ++numFailed;
	_funcptr_glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)IntGetProcAddress("glVertexAttribDivisor");
	if(!_funcptr_glVertexAttribDivisor) ++numFailed;
	return numFailed;
}

static int LoadCore_Version_4_0()
{
	int numFailed = 0;
	_funcptr_glDrawArraysIndirect = (PFNGLDRAWARRAYSINDIRECTPROC)IntGetProcAddress("glDrawArraysIndirect");
	if(!_funcptr_glDrawArraysIndirect) ++numFailed;
	_funcptr_glDrawElementsIndirect = (PFNGLDRAWELEMENTSINDIRECTPROC)IntGetProcAddress("glDrawElementsIndirect");
	if(!_funcptr_glDrawElementsIndirect) ++numFailed;
	_funcptr_glUniform1d = (PFNGLUNIFORM1DPROC)IntGetProcAddress("glUniform1d");
	if(!_funcptr_glUniform1d) ++numFailed;
	_funcptr_glUniform2d = (PFNGLUNIFORM2DPROC)IntGetProcAddress("glUniform2d");
	if(!_funcptr_glUniform2d) ++numFailed;
	_funcptr_glUniform3d = (PFNGLUNIFORM3DPROC)IntGetProcAddress("glUniform3d");
	if(!_funcptr_glUniform3d) ++numFailed;
	_funcptr_glUniform4d = (PFNGLUNIFORM4DPROC)IntGetProcAddress("glUniform4d");
	if(!_funcptr_glUniform4d) ++numFailed;
	_funcptr_glUniform1dv = (PFNGLUNIFORM1DVPROC)IntGetProcAddress("glUniform1dv");
	if(!_funcptr_glUniform1dv) ++numFailed;
	_funcptr_glUniform2dv = (PFNGLUNIFORM2DVPROC)IntGetProcAddress("glUniform2dv");
	if(!_funcptr_glUniform2dv) ++numFailed;
	_funcptr_glUniform3dv = (PFNGLUNIFORM3DVPROC)IntGetProcAddress("glUniform3dv");
	if(!_funcptr_glUniform3dv) ++numFailed;
	_funcptr_glUniform4dv = (PFNGLUNIFORM4DVPROC)IntGetProcAddress("glUniform4dv");
	if(!_funcptr_glUniform4dv) ++numFailed;
	_funcptr_glUniformMatrix2dv = (PFNGLUNIFORMMATRIX2DVPROC)IntGetProcAddress("glUniformMatrix2dv");
	if(!_funcptr_glUniformMatrix2dv) ++numFailed;
	_funcptr_glUniformMatrix3dv = (PFNGLUNIFORMMATRIX3DVPROC)IntGetProcAddress("glUniformMatrix3dv");
	if(!_funcptr_glUniformMatrix3dv) ++numFailed;
	_funcptr_glUniformMatrix4dv = (PFNGLUNIFORMMATRIX4DVPROC)IntGetProcAddress("glUniformMatrix4dv");
	if(!_funcptr_glUniformMatrix4dv) ++numFailed;
	_funcptr_glUniformMatrix2x3dv = (PFNGLUNIFORMMATRIX2X3DVPROC)IntGetProcAddress("glUniformMatrix2x3dv");
	if(!_funcptr_glUniformMatrix2x3dv) ++numFailed;
	_funcptr_glUniformMatrix2x4dv = (PFNGLUNIFORMMATRIX2X4DVPROC)IntGetProcAddress("glUniformMatrix2x4dv");
	if(!_funcptr_glUniformMatrix2x4dv) ++numFailed;
	_funcptr_glUniformMatrix3x2dv = (PFNGLUNIFORMMATRIX3X2DVPROC)IntGetProcAddress("glUniformMatrix3x2dv");
	if(!_funcptr_glUniformMatrix3x2dv) ++numFailed;
	_funcptr_glUniformMatrix3x4dv = (PFNGLUNIFORMMATRIX3X4DVPROC)IntGetProcAddress("glUniformMatrix3x4dv");
	if(!_funcptr_glUniformMatrix3x4dv) ++numFailed;
	_funcptr_glUniformMatrix4x2dv = (PFNGLUNIFORMMATRIX4X2DVPROC)IntGetProcAddress("glUniformMatrix4x2dv");
	if(!_funcptr_glUniformMatrix4x2dv) ++numFailed;
	_funcptr_glUniformMatrix4x3dv = (PFNGLUNIFORMMATRIX4X3DVPROC)IntGetProcAddress("glUniformMatrix4x3dv");
	if(!_funcptr_glUniformMatrix4x3dv) ++numFailed;
	_funcptr_glGetUniformdv = (PFNGLGETUNIFORMDVPROC)IntGetProcAddress("glGetUniformdv");
	if(!_funcptr_glGetUniformdv) ++numFailed;
	_funcptr_glGetSubroutineUniformLocation = (PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC)IntGetProcAddress("glGetSubroutineUniformLocation");
	if(!_funcptr_glGetSubroutineUniformLocation) ++numFailed;
	_funcptr_glGetSubroutineIndex = (PFNGLGETSUBROUTINEINDEXPROC)IntGetProcAddress("glGetSubroutineIndex");
	if(!_funcptr_glGetSubroutineIndex) ++numFailed;
	_funcptr_glGetActiveSubroutineUniformiv = (PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC)IntGetProcAddress("glGetActiveSubroutineUniformiv");
	if(!_funcptr_glGetActiveSubroutineUniformiv) ++numFailed;
	_funcptr_glGetActiveSubroutineUniformName = (PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC)IntGetProcAddress("glGetActiveSubroutineUniformName");
	if(!_funcptr_glGetActiveSubroutineUniformName) ++numFailed;
	_funcptr_glGetActiveSubroutineName = (PFNGLGETACTIVESUBROUTINENAMEPROC)IntGetProcAddress("glGetActiveSubroutineName");
	if(!_funcptr_glGetActiveSubroutineName) ++numFailed;
	_funcptr_glUniformSubroutinesuiv = (PFNGLUNIFORMSUBROUTINESUIVPROC)IntGetProcAddress("glUniformSubroutinesuiv");
	if(!_funcptr_glUniformSubroutinesuiv) ++numFailed;
	_funcptr_glGetUniformSubroutineuiv = (PFNGLGETUNIFORMSUBROUTINEUIVPROC)IntGetProcAddress("glGetUniformSubroutineuiv");
	if(!_funcptr_glGetUniformSubroutineuiv) ++numFailed;
	_funcptr_glGetProgramStageiv = (PFNGLGETPROGRAMSTAGEIVPROC)IntGetProcAddress("glGetProgramStageiv");
	if(!_funcptr_glGetProgramStageiv) ++numFailed;
	_funcptr_glPatchParameteri = (PFNGLPATCHPARAMETERIPROC)IntGetProcAddress("glPatchParameteri");
	if(!_funcptr_glPatchParameteri) ++numFailed;
	_funcptr_glPatchParameterfv = (PFNGLPATCHPARAMETERFVPROC)IntGetProcAddress("glPatchParameterfv");
	if(!_funcptr_glPatchParameterfv) ++numFailed;
	_funcptr_glBindTransformFeedback = (PFNGLBINDTRANSFORMFEEDBACKPROC)IntGetProcAddress("glBindTransformFeedback");
	if(!_funcptr_glBindTransformFeedback) ++numFailed;
	_funcptr_glDeleteTransformFeedbacks = (PFNGLDELETETRANSFORMFEEDBACKSPROC)IntGetProcAddress("glDeleteTransformFeedbacks");
	if(!_funcptr_glDeleteTransformFeedbacks) ++numFailed;
	_funcptr_glGenTransformFeedbacks = (PFNGLGENTRANSFORMFEEDBACKSPROC)IntGetProcAddress("glGenTransformFeedbacks");
	if(!_funcptr_glGenTransformFeedbacks) ++numFailed;
	_funcptr_glIsTransformFeedback = (PFNGLISTRANSFORMFEEDBACKPROC)IntGetProcAddress("glIsTransformFeedback");
	if(!_funcptr_glIsTransformFeedback) ++numFailed;
	_funcptr_glPauseTransformFeedback = (PFNGLPAUSETRANSFORMFEEDBACKPROC)IntGetProcAddress("glPauseTransformFeedback");
	if(!_funcptr_glPauseTransformFeedback) ++numFailed;
	_funcptr_glResumeTransformFeedback = (PFNGLRESUMETRANSFORMFEEDBACKPROC)IntGetProcAddress("glResumeTransformFeedback");
	if(!_funcptr_glResumeTransformFeedback) ++numFailed;
	_funcptr_glDrawTransformFeedback = (PFNGLDRAWTRANSFORMFEEDBACKPROC)IntGetProcAddress("glDrawTransformFeedback");
	if(!_funcptr_glDrawTransformFeedback) ++numFailed;
	_funcptr_glDrawTransformFeedbackStream = (PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC)IntGetProcAddress("glDrawTransformFeedbackStream");
	if(!_funcptr_glDrawTransformFeedbackStream) ++numFailed;
	_funcptr_glBeginQueryIndexed = (PFNGLBEGINQUERYINDEXEDPROC)IntGetProcAddress("glBeginQueryIndexed");
	if(!_funcptr_glBeginQueryIndexed) ++numFailed;
	_funcptr_glEndQueryIndexed = (PFNGLENDQUERYINDEXEDPROC)IntGetProcAddress("glEndQueryIndexed");
	if(!_funcptr_glEndQueryIndexed) ++numFailed;
	_funcptr_glGetQueryIndexediv = (PFNGLGETQUERYINDEXEDIVPROC)IntGetProcAddress("glGetQueryIndexediv");
	if(!_funcptr_glGetQueryIndexediv) ++numFailed;
	_funcptr_glMinSampleShading = (PFNGLMINSAMPLESHADINGPROC)IntGetProcAddress("glMinSampleShading");
	if(!_funcptr_glMinSampleShading) ++numFailed;
	_funcptr_glBlendEquationi = (PFNGLBLENDEQUATIONIPROC)IntGetProcAddress("glBlendEquationi");
	if(!_funcptr_glBlendEquationi) ++numFailed;
	_funcptr_glBlendEquationSeparatei = (PFNGLBLENDEQUATIONSEPARATEIPROC)IntGetProcAddress("glBlendEquationSeparatei");
	if(!_funcptr_glBlendEquationSeparatei) ++numFailed;
	_funcptr_glBlendFunci = (PFNGLBLENDFUNCIPROC)IntGetProcAddress("glBlendFunci");
	if(!_funcptr_glBlendFunci) ++numFailed;
	_funcptr_glBlendFuncSeparatei = (PFNGLBLENDFUNCSEPARATEIPROC)IntGetProcAddress("glBlendFuncSeparatei");
	if(!_funcptr_glBlendFuncSeparatei) ++numFailed;
	return numFailed;
}

static int LoadCore_Version_4_1()
{
	int numFailed = 0;
	_funcptr_glReleaseShaderCompiler = (PFNGLRELEASESHADERCOMPILERPROC)IntGetProcAddress("glReleaseShaderCompiler");
	if(!_funcptr_glReleaseShaderCompiler) ++numFailed;
	_funcptr_glShaderBinary = (PFNGLSHADERBINARYPROC)IntGetProcAddress("glShaderBinary");
	if(!_funcptr_glShaderBinary) ++numFailed;
	_funcptr_glGetShaderPrecisionFormat = (PFNGLGETSHADERPRECISIONFORMATPROC)IntGetProcAddress("glGetShaderPrecisionFormat");
	if(!_funcptr_glGetShaderPrecisionFormat) ++numFailed;
	_funcptr_glDepthRangef = (PFNGLDEPTHRANGEFPROC)IntGetProcAddress("glDepthRangef");
	if(!_funcptr_glDepthRangef) ++numFailed;
	_funcptr_glClearDepthf = (PFNGLCLEARDEPTHFPROC)IntGetProcAddress("glClearDepthf");
	if(!_funcptr_glClearDepthf) ++numFailed;
	_funcptr_glGetProgramBinary = (PFNGLGETPROGRAMBINARYPROC)IntGetProcAddress("glGetProgramBinary");
	if(!_funcptr_glGetProgramBinary) ++numFailed;
	_funcptr_glProgramBinary = (PFNGLPROGRAMBINARYPROC)IntGetProcAddress("glProgramBinary");
	if(!_funcptr_glProgramBinary) ++numFailed;
	_funcptr_glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC)IntGetProcAddress("glProgramParameteri");
	if(!_funcptr_glProgramParameteri) ++numFailed;
	_funcptr_glUseProgramStages = (PFNGLUSEPROGRAMSTAGESPROC)IntGetProcAddress("glUseProgramStages");
	if(!_funcptr_glUseProgramStages) ++numFailed;
	_funcptr_glActiveShaderProgram = (PFNGLACTIVESHADERPROGRAMPROC)IntGetProcAddress("glActiveShaderProgram");
	if(!_funcptr_glActiveShaderProgram) ++numFailed;
	_funcptr_glCreateShaderProgramv = (PFNGLCREATESHADERPROGRAMVPROC)IntGetProcAddress("glCreateShaderProgramv");
	if(!_funcptr_glCreateShaderProgramv) ++numFailed;
	_funcptr_glBindProgramPipeline = (PFNGLBINDPROGRAMPIPELINEPROC)IntGetProcAddress("glBindProgramPipeline");
	if(!_funcptr_glBindProgramPipeline) ++numFailed;
	_funcptr_glDeleteProgramPipelines = (PFNGLDELETEPROGRAMPIPELINESPROC)IntGetProcAddress("glDeleteProgramPipelines");
	if(!_funcptr_glDeleteProgramPipelines) ++numFailed;
	_funcptr_glGenProgramPipelines = (PFNGLGENPROGRAMPIPELINESPROC)IntGetProcAddress("glGenProgramPipelines");
	if(!_funcptr_glGenProgramPipelines) ++numFailed;
	_funcptr_glIsProgramPipeline = (PFNGLISPROGRAMPIPELINEPROC)IntGetProcAddress("glIsProgramPipeline");
	if(!_funcptr_glIsProgramPipeline) ++numFailed;
	_funcptr_glGetProgramPipelineiv = (PFNGLGETPROGRAMPIPELINEIVPROC)IntGetProcAddress("glGetProgramPipelineiv");
	if(!_funcptr_glGetProgramPipelineiv) ++numFailed;
	_funcptr_glProgramUniform1i = (PFNGLPROGRAMUNIFORM1IPROC)IntGetProcAddress("glProgramUniform1i");
	if(!_funcptr_glProgramUniform1i) ++numFailed;
	_funcptr_glProgramUniform1iv = (PFNGLPROGRAMUNIFORM1IVPROC)IntGetProcAddress("glProgramUniform1iv");
	if(!_funcptr_glProgramUniform1iv) ++numFailed;
	_funcptr_glProgramUniform1f = (PFNGLPROGRAMUNIFORM1FPROC)IntGetProcAddress("glProgramUniform1f");
	if(!_funcptr_glProgramUniform1f) ++numFailed;
	_funcptr_glProgramUniform1fv = (PFNGLPROGRAMUNIFORM1FVPROC)IntGetProcAddress("glProgramUniform1fv");
	if(!_funcptr_glProgramUniform1fv) ++numFailed;
	_funcptr_glProgramUniform1d = (PFNGLPROGRAMUNIFORM1DPROC)IntGetProcAddress("glProgramUniform1d");
	if(!_funcptr_glProgramUniform1d) ++numFailed;
	_funcptr_glProgramUniform1dv = (PFNGLPROGRAMUNIFORM1DVPROC)IntGetProcAddress("glProgramUniform1dv");
	if(!_funcptr_glProgramUniform1dv) ++numFailed;
	_funcptr_glProgramUniform1ui = (PFNGLPROGRAMUNIFORM1UIPROC)IntGetProcAddress("glProgramUniform1ui");
	if(!_funcptr_glProgramUniform1ui) ++numFailed;
	_funcptr_glProgramUniform1uiv = (PFNGLPROGRAMUNIFORM1UIVPROC)IntGetProcAddress("glProgramUniform1uiv");
	if(!_funcptr_glProgramUniform1uiv) ++numFailed;
	_funcptr_glProgramUniform2i = (PFNGLPROGRAMUNIFORM2IPROC)IntGetProcAddress("glProgramUniform2i");
	if(!_funcptr_glProgramUniform2i) ++numFailed;
	_funcptr_glProgramUniform2iv = (PFNGLPROGRAMUNIFORM2IVPROC)IntGetProcAddress("glProgramUniform2iv");
	if(!_funcptr_glProgramUniform2iv) ++numFailed;
	_funcptr_glProgramUniform2f = (PFNGLPROGRAMUNIFORM2FPROC)IntGetProcAddress("glProgramUniform2f");
	if(!_funcptr_glProgramUniform2f) ++numFailed;
	_funcptr_glProgramUniform2fv = (PFNGLPROGRAMUNIFORM2FVPROC)IntGetProcAddress("glProgramUniform2fv");
	if(!_funcptr_glProgramUniform2fv) ++numFailed;
	_funcptr_glProgramUniform2d = (PFNGLPROGRAMUNIFORM2DPROC)IntGetProcAddress("glProgramUniform2d");
	if(!_funcptr_glProgramUniform2d) ++numFailed;
	_funcptr_glProgramUniform2dv = (PFNGLPROGRAMUNIFORM2DVPROC)IntGetProcAddress("glProgramUniform2dv");
	if(!_funcptr_glProgramUniform2dv) ++numFailed;
	_funcptr_glProgramUniform2ui = (PFNGLPROGRAMUNIFORM2UIPROC)IntGetProcAddress("glProgramUniform2ui");
	if(!_funcptr_glProgramUniform2ui) ++numFailed;
	_funcptr_glProgramUniform2uiv = (PFNGLPROGRAMUNIFORM2UIVPROC)IntGetProcAddress("glProgramUniform2uiv");
	if(!_funcptr_glProgramUniform2uiv) ++numFailed;
	_funcptr_glProgramUniform3i = (PFNGLPROGRAMUNIFORM3IPROC)IntGetProcAddress("glProgramUniform3i");
	if(!_funcptr_glProgramUniform3i) ++numFailed;
	_funcptr_glProgramUniform3iv = (PFNGLPROGRAMUNIFORM3IVPROC)IntGetProcAddress("glProgramUniform3iv");
	if(!_funcptr_glProgramUniform3iv) ++numFailed;
	_funcptr_glProgramUniform3f = (PFNGLPROGRAMUNIFORM3FPROC)IntGetProcAddress("glProgramUniform3f");
	if(!_funcptr_glProgramUniform3f) ++numFailed;
	_funcptr_glProgramUniform3fv = (PFNGLPROGRAMUNIFORM3FVPROC)IntGetProcAddress("glProgramUniform3fv");
	if(!_funcptr_glProgramUniform3fv) ++numFailed;
	_funcptr_glProgramUniform3d = (PFNGLPROGRAMUNIFORM3DPROC)IntGetProcAddress("glProgramUniform3d");
	if(!_funcptr_glProgramUniform3d) ++numFailed;
	_funcptr_glProgramUniform3dv = (PFNGLPROGRAMUNIFORM3DVPROC)IntGetProcAddress("glProgramUniform3dv");
	if(!_funcptr_glProgramUniform3dv) ++numFailed;
	_funcptr_glProgramUniform3ui = (PFNGLPROGRAMUNIFORM3UIPROC)IntGetProcAddress("glProgramUniform3ui");
	if(!_funcptr_glProgramUniform3ui) ++numFailed;
	_funcptr_glProgramUniform3uiv = (PFNGLPROGRAMUNIFORM3UIVPROC)IntGetProcAddress("glProgramUniform3uiv");
	if(!_funcptr_glProgramUniform3uiv) ++numFailed;
	_funcptr_glProgramUniform4i = (PFNGLPROGRAMUNIFORM4IPROC)IntGetProcAddress("glProgramUniform4i");
	if(!_funcptr_glProgramUniform4i) ++numFailed;
	_funcptr_glProgramUniform4iv = (PFNGLPROGRAMUNIFORM4IVPROC)IntGetProcAddress("glProgramUniform4iv");
	if(!_funcptr_glProgramUniform4iv) ++numFailed;
	_funcptr_glProgramUniform4f = (PFNGLPROGRAMUNIFORM4FPROC)IntGetProcAddress("glProgramUniform4f");
	if(!_funcptr_glProgramUniform4f) ++numFailed;
	_funcptr_glProgramUniform4fv = (PFNGLPROGRAMUNIFORM4FVPROC)IntGetProcAddress("glProgramUniform4fv");
	if(!_funcptr_glProgramUniform4fv) ++numFailed;
	_funcptr_glProgramUniform4d = (PFNGLPROGRAMUNIFORM4DPROC)IntGetProcAddress("glProgramUniform4d");
	if(!_funcptr_glProgramUniform4d) ++numFailed;
	_funcptr_glProgramUniform4dv = (PFNGLPROGRAMUNIFORM4DVPROC)IntGetProcAddress("glProgramUniform4dv");
	if(!_funcptr_glProgramUniform4dv) ++numFailed;
	_funcptr_glProgramUniform4ui = (PFNGLPROGRAMUNIFORM4UIPROC)IntGetProcAddress("glProgramUniform4ui");
	if(!_funcptr_glProgramUniform4ui) ++numFailed;
	_funcptr_glProgramUniform4uiv = (PFNGLPROGRAMUNIFORM4UIVPROC)IntGetProcAddress("glProgramUniform4uiv");
	if(!_funcptr_glProgramUniform4uiv) ++numFailed;
	_funcptr_glProgramUniformMatrix2fv = (PFNGLPROGRAMUNIFORMMATRIX2FVPROC)IntGetProcAddress("glProgramUniformMatrix2fv");
	if(!_funcptr_glProgramUniformMatrix2fv) ++numFailed;
	_funcptr_glProgramUniformMatrix3fv = (PFNGLPROGRAMUNIFORMMATRIX3FVPROC)IntGetProcAddress("glProgramUniformMatrix3fv");
	if(!_funcptr_glProgramUniformMatrix3fv) ++numFailed;
	_funcptr_glProgramUniformMatrix4fv = (PFNGLPROGRAMUNIFORMMATRIX4FVPROC)IntGetProcAddress("glProgramUniformMatrix4fv");
	if(!_funcptr_glProgramUniformMatrix4fv) ++numFailed;
	_funcptr_glProgramUniformMatrix2dv = (PFNGLPROGRAMUNIFORMMATRIX2DVPROC)IntGetProcAddress("glProgramUniformMatrix2dv");
	if(!_funcptr_glProgramUniformMatrix2dv) ++numFailed;
	_funcptr_glProgramUniformMatrix3dv = (PFNGLPROGRAMUNIFORMMATRIX3DVPROC)IntGetProcAddress("glProgramUniformMatrix3dv");
	if(!_funcptr_glProgramUniformMatrix3dv) ++numFailed;
	_funcptr_glProgramUniformMatrix4dv = (PFNGLPROGRAMUNIFORMMATRIX4DVPROC)IntGetProcAddress("glProgramUniformMatrix4dv");
	if(!_funcptr_glProgramUniformMatrix4dv) ++numFailed;
	_funcptr_glProgramUniformMatrix2x3fv = (PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC)IntGetProcAddress("glProgramUniformMatrix2x3fv");
	if(!_funcptr_glProgramUniformMatrix2x3fv) ++numFailed;
	_funcptr_glProgramUniformMatrix3x2fv = (PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC)IntGetProcAddress("glProgramUniformMatrix3x2fv");
	if(!_funcptr_glProgramUniformMatrix3x2fv) ++numFailed;
	_funcptr_glProgramUniformMatrix2x4fv = (PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC)IntGetProcAddress("glProgramUniformMatrix2x4fv");
	if(!_funcptr_glProgramUniformMatrix2x4fv) ++numFailed;
	_funcptr_glProgramUniformMatrix4x2fv = (PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC)IntGetProcAddress("glProgramUniformMatrix4x2fv");
	if(!_funcptr_glProgramUniformMatrix4x2fv) ++numFailed;
	_funcptr_glProgramUniformMatrix3x4fv = (PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC)IntGetProcAddress("glProgramUniformMatrix3x4fv");
	if(!_funcptr_glProgramUniformMatrix3x4fv) ++numFailed;
	_funcptr_glProgramUniformMatrix4x3fv = (PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC)IntGetProcAddress("glProgramUniformMatrix4x3fv");
	if(!_funcptr_glProgramUniformMatrix4x3fv) ++numFailed;
	_funcptr_glProgramUniformMatrix2x3dv = (PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC)IntGetProcAddress("glProgramUniformMatrix2x3dv");
	if(!_funcptr_glProgramUniformMatrix2x3dv) ++numFailed;
	_funcptr_glProgramUniformMatrix3x2dv = (PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC)IntGetProcAddress("glProgramUniformMatrix3x2dv");
	if(!_funcptr_glProgramUniformMatrix3x2dv) ++numFailed;
	_funcptr_glProgramUniformMatrix2x4dv = (PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC)IntGetProcAddress("glProgramUniformMatrix2x4dv");
	if(!_funcptr_glProgramUniformMatrix2x4dv) ++numFailed;
	_funcptr_glProgramUniformMatrix4x2dv = (PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC)IntGetProcAddress("glProgramUniformMatrix4x2dv");
	if(!_funcptr_glProgramUniformMatrix4x2dv) ++numFailed;
	_funcptr_glProgramUniformMatrix3x4dv = (PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC)IntGetProcAddress("glProgramUniformMatrix3x4dv");
	if(!_funcptr_glProgramUniformMatrix3x4dv) ++numFailed;
	_funcptr_glProgramUniformMatrix4x3dv = (PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC)IntGetProcAddress("glProgramUniformMatrix4x3dv");
	if(!_funcptr_glProgramUniformMatrix4x3dv) ++numFailed;
	_funcptr_glValidateProgramPipeline = (PFNGLVALIDATEPROGRAMPIPELINEPROC)IntGetProcAddress("glValidateProgramPipeline");
	if(!_funcptr_glValidateProgramPipeline) ++numFailed;
	_funcptr_glGetProgramPipelineInfoLog = (PFNGLGETPROGRAMPIPELINEINFOLOGPROC)IntGetProcAddress("glGetProgramPipelineInfoLog");
	if(!_funcptr_glGetProgramPipelineInfoLog) ++numFailed;
	_funcptr_glVertexAttribL1d = (PFNGLVERTEXATTRIBL1DPROC)IntGetProcAddress("glVertexAttribL1d");
	if(!_funcptr_glVertexAttribL1d) ++numFailed;
	_funcptr_glVertexAttribL2d = (PFNGLVERTEXATTRIBL2DPROC)IntGetProcAddress("glVertexAttribL2d");
	if(!_funcptr_glVertexAttribL2d) ++numFailed;
	_funcptr_glVertexAttribL3d = (PFNGLVERTEXATTRIBL3DPROC)IntGetProcAddress("glVertexAttribL3d");
	if(!_funcptr_glVertexAttribL3d) ++numFailed;
	_funcptr_glVertexAttribL4d = (PFNGLVERTEXATTRIBL4DPROC)IntGetProcAddress("glVertexAttribL4d");
	if(!_funcptr_glVertexAttribL4d) ++numFailed;
	_funcptr_glVertexAttribL1dv = (PFNGLVERTEXATTRIBL1DVPROC)IntGetProcAddress("glVertexAttribL1dv");
	if(!_funcptr_glVertexAttribL1dv) ++numFailed;
	_funcptr_glVertexAttribL2dv = (PFNGLVERTEXATTRIBL2DVPROC)IntGetProcAddress("glVertexAttribL2dv");
	if(!_funcptr_glVertexAttribL2dv) ++numFailed;
	_funcptr_glVertexAttribL3dv = (PFNGLVERTEXATTRIBL3DVPROC)IntGetProcAddress("glVertexAttribL3dv");
	if(!_funcptr_glVertexAttribL3dv) ++numFailed;
	_funcptr_glVertexAttribL4dv = (PFNGLVERTEXATTRIBL4DVPROC)IntGetProcAddress("glVertexAttribL4dv");
	if(!_funcptr_glVertexAttribL4dv) ++numFailed;
	_funcptr_glVertexAttribLPointer = (PFNGLVERTEXATTRIBLPOINTERPROC)IntGetProcAddress("glVertexAttribLPointer");
	if(!_funcptr_glVertexAttribLPointer) ++numFailed;
	_funcptr_glGetVertexAttribLdv = (PFNGLGETVERTEXATTRIBLDVPROC)IntGetProcAddress("glGetVertexAttribLdv");
	if(!_funcptr_glGetVertexAttribLdv) ++numFailed;
	_funcptr_glViewportArrayv = (PFNGLVIEWPORTARRAYVPROC)IntGetProcAddress("glViewportArrayv");
	if(!_funcptr_glViewportArrayv) ++numFailed;
	_funcptr_glViewportIndexedf = (PFNGLVIEWPORTINDEXEDFPROC)IntGetProcAddress("glViewportIndexedf");
	if(!_funcptr_glViewportIndexedf) ++numFailed;
	_funcptr_glViewportIndexedfv = (PFNGLVIEWPORTINDEXEDFVPROC)IntGetProcAddress("glViewportIndexedfv");
	if(!_funcptr_glViewportIndexedfv) ++numFailed;
	_funcptr_glScissorArrayv = (PFNGLSCISSORARRAYVPROC)IntGetProcAddress("glScissorArrayv");
	if(!_funcptr_glScissorArrayv) ++numFailed;
	_funcptr_glScissorIndexed = (PFNGLSCISSORINDEXEDPROC)IntGetProcAddress("glScissorIndexed");
	if(!_funcptr_glScissorIndexed) ++numFailed;
	_funcptr_glScissorIndexedv = (PFNGLSCISSORINDEXEDVPROC)IntGetProcAddress("glScissorIndexedv");
	if(!_funcptr_glScissorIndexedv) ++numFailed;
	_funcptr_glDepthRangeArrayv = (PFNGLDEPTHRANGEARRAYVPROC)IntGetProcAddress("glDepthRangeArrayv");
	if(!_funcptr_glDepthRangeArrayv) ++numFailed;
	_funcptr_glDepthRangeIndexed = (PFNGLDEPTHRANGEINDEXEDPROC)IntGetProcAddress("glDepthRangeIndexed");
	if(!_funcptr_glDepthRangeIndexed) ++numFailed;
	_funcptr_glGetFloati_v = (PFNGLGETFLOATI_VPROC)IntGetProcAddress("glGetFloati_v");
	if(!_funcptr_glGetFloati_v) ++numFailed;
	_funcptr_glGetDoublei_v = (PFNGLGETDOUBLEI_VPROC)IntGetProcAddress("glGetDoublei_v");
	if(!_funcptr_glGetDoublei_v) ++numFailed;
	return numFailed;
}

static int LoadCore_Version_4_2()
{
	int numFailed = 0;
	_funcptr_glDrawArraysInstancedBaseInstance = (PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)IntGetProcAddress("glDrawArraysInstancedBaseInstance");
	if(!_funcptr_glDrawArraysInstancedBaseInstance) ++numFailed;
	_funcptr_glDrawElementsInstancedBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC)IntGetProcAddress("glDrawElementsInstancedBaseInstance");
	if(!_funcptr_glDrawElementsInstancedBaseInstance) ++numFailed;
	_funcptr_glDrawElementsInstancedBaseVertexBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC)IntGetProcAddress("glDrawElementsInstancedBaseVertexBaseInstance");
	if(!_funcptr_glDrawElementsInstancedBaseVertexBaseInstance) ++numFailed;
	_funcptr_glDrawTransformFeedbackInstanced = (PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC)IntGetProcAddress("glDrawTransformFeedbackInstanced");
	if(!_funcptr_glDrawTransformFeedbackInstanced) ++numFailed;
	_funcptr_glDrawTransformFeedbackStreamInstanced = (PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC)IntGetProcAddress("glDrawTransformFeedbackStreamInstanced");
	if(!_funcptr_glDrawTransformFeedbackStreamInstanced) ++numFailed;
	_funcptr_glGetInternalformativ = (PFNGLGETINTERNALFORMATIVPROC)IntGetProcAddress("glGetInternalformativ");
	if(!_funcptr_glGetInternalformativ) ++numFailed;
	_funcptr_glGetActiveAtomicCounterBufferiv = (PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC)IntGetProcAddress("glGetActiveAtomicCounterBufferiv");
	if(!_funcptr_glGetActiveAtomicCounterBufferiv) ++numFailed;
	_funcptr_glBindImageTexture = (PFNGLBINDIMAGETEXTUREPROC)IntGetProcAddress("glBindImageTexture");
	if(!_funcptr_glBindImageTexture) ++numFailed;
	_funcptr_glMemoryBarrier = (PFNGLMEMORYBARRIERPROC)IntGetProcAddress("glMemoryBarrier");
	if(!_funcptr_glMemoryBarrier) ++numFailed;
	_funcptr_glTexStorage1D = (PFNGLTEXSTORAGE1DPROC)IntGetProcAddress("glTexStorage1D");
	if(!_funcptr_glTexStorage1D) ++numFailed;
	_funcptr_glTexStorage2D = (PFNGLTEXSTORAGE2DPROC)IntGetProcAddress("glTexStorage2D");
	if(!_funcptr_glTexStorage2D) ++numFailed;
	_funcptr_glTexStorage3D = (PFNGLTEXSTORAGE3DPROC)IntGetProcAddress("glTexStorage3D");
	if(!_funcptr_glTexStorage3D) ++numFailed;
	_funcptr_glTextureStorage1DEXT = (PFNGLTEXTURESTORAGE1DEXTPROC)IntGetProcAddress("glTextureStorage1DEXT");
	/* glTextureStorage1DEXT comes from DSA.*/
	_funcptr_glTextureStorage2DEXT = (PFNGLTEXTURESTORAGE2DEXTPROC)IntGetProcAddress("glTextureStorage2DEXT");
	/* glTextureStorage2DEXT comes from DSA.*/
	_funcptr_glTextureStorage3DEXT = (PFNGLTEXTURESTORAGE3DEXTPROC)IntGetProcAddress("glTextureStorage3DEXT");
	/* glTextureStorage3DEXT comes from DSA.*/
	return numFailed;
}

static int LoadCore_Version_4_3()
{
	int numFailed = 0;
	_funcptr_glDebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC)IntGetProcAddress("glDebugMessageControl");
	if(!_funcptr_glDebugMessageControl) ++numFailed;
	_funcptr_glDebugMessageInsert = (PFNGLDEBUGMESSAGEINSERTPROC)IntGetProcAddress("glDebugMessageInsert");
	if(!_funcptr_glDebugMessageInsert) ++numFailed;
	_funcptr_glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)IntGetProcAddress("glDebugMessageCallback");
	if(!_funcptr_glDebugMessageCallback) ++numFailed;
	_funcptr_glGetDebugMessageLog = (PFNGLGETDEBUGMESSAGELOGPROC)IntGetProcAddress("glGetDebugMessageLog");
	if(!_funcptr_glGetDebugMessageLog) ++numFailed;
	_funcptr_glPushDebugGroup = (PFNGLPUSHDEBUGGROUPPROC)IntGetProcAddress("glPushDebugGroup");
	if(!_funcptr_glPushDebugGroup) ++numFailed;
	_funcptr_glPopDebugGroup = (PFNGLPOPDEBUGGROUPPROC)IntGetProcAddress("glPopDebugGroup");
	if(!_funcptr_glPopDebugGroup) ++numFailed;
	_funcptr_glObjectLabel = (PFNGLOBJECTLABELPROC)IntGetProcAddress("glObjectLabel");
	if(!_funcptr_glObjectLabel) ++numFailed;
	_funcptr_glGetObjectLabel = (PFNGLGETOBJECTLABELPROC)IntGetProcAddress("glGetObjectLabel");
	if(!_funcptr_glGetObjectLabel) ++numFailed;
	_funcptr_glObjectPtrLabel = (PFNGLOBJECTPTRLABELPROC)IntGetProcAddress("glObjectPtrLabel");
	if(!_funcptr_glObjectPtrLabel) ++numFailed;
	_funcptr_glGetObjectPtrLabel = (PFNGLGETOBJECTPTRLABELPROC)IntGetProcAddress("glGetObjectPtrLabel");
	if(!_funcptr_glGetObjectPtrLabel) ++numFailed;
	_funcptr_glClearBufferData = (PFNGLCLEARBUFFERDATAPROC)IntGetProcAddress("glClearBufferData");
	if(!_funcptr_glClearBufferData) ++numFailed;
	_funcptr_glClearBufferSubData = (PFNGLCLEARBUFFERSUBDATAPROC)IntGetProcAddress("glClearBufferSubData");
	if(!_funcptr_glClearBufferSubData) ++numFailed;
	_funcptr_glClearNamedBufferDataEXT = (PFNGLCLEARNAMEDBUFFERDATAEXTPROC)IntGetProcAddress("glClearNamedBufferDataEXT");
	/* glClearNamedBufferDataEXT comes from DSA.*/
	_funcptr_glClearNamedBufferSubDataEXT = (PFNGLCLEARNAMEDBUFFERSUBDATAEXTPROC)IntGetProcAddress("glClearNamedBufferSubDataEXT");
	/* glClearNamedBufferSubDataEXT comes from DSA.*/
	_funcptr_glDispatchCompute = (PFNGLDISPATCHCOMPUTEPROC)IntGetProcAddress("glDispatchCompute");
	if(!_funcptr_glDispatchCompute) ++numFailed;
	_funcptr_glDispatchComputeIndirect = (PFNGLDISPATCHCOMPUTEINDIRECTPROC)IntGetProcAddress("glDispatchComputeIndirect");
	if(!_funcptr_glDispatchComputeIndirect) ++numFailed;
	_funcptr_glCopyImageSubData = (PFNGLCOPYIMAGESUBDATAPROC)IntGetProcAddress("glCopyImageSubData");
	if(!_funcptr_glCopyImageSubData) ++numFailed;
	_funcptr_glFramebufferParameteri = (PFNGLFRAMEBUFFERPARAMETERIPROC)IntGetProcAddress("glFramebufferParameteri");
	if(!_funcptr_glFramebufferParameteri) ++numFailed;
	_funcptr_glGetFramebufferParameteriv = (PFNGLGETFRAMEBUFFERPARAMETERIVPROC)IntGetProcAddress("glGetFramebufferParameteriv");
	if(!_funcptr_glGetFramebufferParameteriv) ++numFailed;
	_funcptr_glNamedFramebufferParameteriEXT = (PFNGLNAMEDFRAMEBUFFERPARAMETERIEXTPROC)IntGetProcAddress("glNamedFramebufferParameteriEXT");
	/* glNamedFramebufferParameteriEXT comes from DSA.*/
	_funcptr_glGetNamedFramebufferParameterivEXT = (PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVEXTPROC)IntGetProcAddress("glGetNamedFramebufferParameterivEXT");
	/* glGetNamedFramebufferParameterivEXT comes from DSA.*/
	_funcptr_glGetInternalformati64v = (PFNGLGETINTERNALFORMATI64VPROC)IntGetProcAddress("glGetInternalformati64v");
	if(!_funcptr_glGetInternalformati64v) ++numFailed;
	_funcptr_glInvalidateTexSubImage = (PFNGLINVALIDATETEXSUBIMAGEPROC)IntGetProcAddress("glInvalidateTexSubImage");
	if(!_funcptr_glInvalidateTexSubImage) ++numFailed;
	_funcptr_glInvalidateTexImage = (PFNGLINVALIDATETEXIMAGEPROC)IntGetProcAddress("glInvalidateTexImage");
	if(!_funcptr_glInvalidateTexImage) ++numFailed;
	_funcptr_glInvalidateBufferSubData = (PFNGLINVALIDATEBUFFERSUBDATAPROC)IntGetProcAddress("glInvalidateBufferSubData");
	if(!_funcptr_glInvalidateBufferSubData) ++numFailed;
	_funcptr_glInvalidateBufferData = (PFNGLINVALIDATEBUFFERDATAPROC)IntGetProcAddress("glInvalidateBufferData");
	if(!_funcptr_glInvalidateBufferData) ++numFailed;
	_funcptr_glInvalidateFramebuffer = (PFNGLINVALIDATEFRAMEBUFFERPROC)IntGetProcAddress("glInvalidateFramebuffer");
	if(!_funcptr_glInvalidateFramebuffer) ++numFailed;
	_funcptr_glInvalidateSubFramebuffer = (PFNGLINVALIDATESUBFRAMEBUFFERPROC)IntGetProcAddress("glInvalidateSubFramebuffer");
	if(!_funcptr_glInvalidateSubFramebuffer) ++numFailed;
	_funcptr_glMultiDrawArraysIndirect = (PFNGLMULTIDRAWARRAYSINDIRECTPROC)IntGetProcAddress("glMultiDrawArraysIndirect");
	if(!_funcptr_glMultiDrawArraysIndirect) ++numFailed;
	_funcptr_glMultiDrawElementsIndirect = (PFNGLMULTIDRAWELEMENTSINDIRECTPROC)IntGetProcAddress("glMultiDrawElementsIndirect");
	if(!_funcptr_glMultiDrawElementsIndirect) ++numFailed;
	_funcptr_glGetProgramInterfaceiv = (PFNGLGETPROGRAMINTERFACEIVPROC)IntGetProcAddress("glGetProgramInterfaceiv");
	if(!_funcptr_glGetProgramInterfaceiv) ++numFailed;
	_funcptr_glGetProgramResourceIndex = (PFNGLGETPROGRAMRESOURCEINDEXPROC)IntGetProcAddress("glGetProgramResourceIndex");
	if(!_funcptr_glGetProgramResourceIndex) ++numFailed;
	_funcptr_glGetProgramResourceName = (PFNGLGETPROGRAMRESOURCENAMEPROC)IntGetProcAddress("glGetProgramResourceName");
	if(!_funcptr_glGetProgramResourceName) ++numFailed;
	_funcptr_glGetProgramResourceiv = (PFNGLGETPROGRAMRESOURCEIVPROC)IntGetProcAddress("glGetProgramResourceiv");
	if(!_funcptr_glGetProgramResourceiv) ++numFailed;
	_funcptr_glGetProgramResourceLocation = (PFNGLGETPROGRAMRESOURCELOCATIONPROC)IntGetProcAddress("glGetProgramResourceLocation");
	if(!_funcptr_glGetProgramResourceLocation) ++numFailed;
	_funcptr_glGetProgramResourceLocationIndex = (PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC)IntGetProcAddress("glGetProgramResourceLocationIndex");
	if(!_funcptr_glGetProgramResourceLocationIndex) ++numFailed;
	_funcptr_glShaderStorageBlockBinding = (PFNGLSHADERSTORAGEBLOCKBINDINGPROC)IntGetProcAddress("glShaderStorageBlockBinding");
	if(!_funcptr_glShaderStorageBlockBinding) ++numFailed;
	_funcptr_glTexBufferRange = (PFNGLTEXBUFFERRANGEPROC)IntGetProcAddress("glTexBufferRange");
	if(!_funcptr_glTexBufferRange) ++numFailed;
	_funcptr_glTextureBufferRangeEXT = (PFNGLTEXTUREBUFFERRANGEEXTPROC)IntGetProcAddress("glTextureBufferRangeEXT");
	/* glTextureBufferRangeEXT comes from DSA.*/
	_funcptr_glTexStorage2DMultisample = (PFNGLTEXSTORAGE2DMULTISAMPLEPROC)IntGetProcAddress("glTexStorage2DMultisample");
	if(!_funcptr_glTexStorage2DMultisample) ++numFailed;
	_funcptr_glTexStorage3DMultisample = (PFNGLTEXSTORAGE3DMULTISAMPLEPROC)IntGetProcAddress("glTexStorage3DMultisample");
	if(!_funcptr_glTexStorage3DMultisample) ++numFailed;
	_funcptr_glTextureStorage2DMultisampleEXT = (PFNGLTEXTURESTORAGE2DMULTISAMPLEEXTPROC)IntGetProcAddress("glTextureStorage2DMultisampleEXT");
	/* glTextureStorage2DMultisampleEXT comes from DSA.*/
	_funcptr_glTextureStorage3DMultisampleEXT = (PFNGLTEXTURESTORAGE3DMULTISAMPLEEXTPROC)IntGetProcAddress("glTextureStorage3DMultisampleEXT");
	/* glTextureStorage3DMultisampleEXT comes from DSA.*/
	_funcptr_glTextureView = (PFNGLTEXTUREVIEWPROC)IntGetProcAddress("glTextureView");
	if(!_funcptr_glTextureView) ++numFailed;
	_funcptr_glBindVertexBuffer = (PFNGLBINDVERTEXBUFFERPROC)IntGetProcAddress("glBindVertexBuffer");
	if(!_funcptr_glBindVertexBuffer) ++numFailed;
	_funcptr_glVertexAttribFormat = (PFNGLVERTEXATTRIBFORMATPROC)IntGetProcAddress("glVertexAttribFormat");
	if(!_funcptr_glVertexAttribFormat) ++numFailed;
	_funcptr_glVertexAttribIFormat = (PFNGLVERTEXATTRIBIFORMATPROC)IntGetProcAddress("glVertexAttribIFormat");
	if(!_funcptr_glVertexAttribIFormat) ++numFailed;
	_funcptr_glVertexAttribLFormat = (PFNGLVERTEXATTRIBLFORMATPROC)IntGetProcAddress("glVertexAttribLFormat");
	if(!_funcptr_glVertexAttribLFormat) ++numFailed;
	_funcptr_glVertexAttribBinding = (PFNGLVERTEXATTRIBBINDINGPROC)IntGetProcAddress("glVertexAttribBinding");
	if(!_funcptr_glVertexAttribBinding) ++numFailed;
	_funcptr_glVertexBindingDivisor = (PFNGLVERTEXBINDINGDIVISORPROC)IntGetProcAddress("glVertexBindingDivisor");
	if(!_funcptr_glVertexBindingDivisor) ++numFailed;
	_funcptr_glVertexArrayBindVertexBufferEXT = (PFNGLVERTEXARRAYBINDVERTEXBUFFEREXTPROC)IntGetProcAddress("glVertexArrayBindVertexBufferEXT");
	/* glVertexArrayBindVertexBufferEXT comes from DSA.*/
	_funcptr_glVertexArrayVertexAttribFormatEXT = (PFNGLVERTEXARRAYVERTEXATTRIBFORMATEXTPROC)IntGetProcAddress("glVertexArrayVertexAttribFormatEXT");
	/* glVertexArrayVertexAttribFormatEXT comes from DSA.*/
	_funcptr_glVertexArrayVertexAttribIFormatEXT = (PFNGLVERTEXARRAYVERTEXATTRIBIFORMATEXTPROC)IntGetProcAddress("glVertexArrayVertexAttribIFormatEXT");
	/* glVertexArrayVertexAttribIFormatEXT comes from DSA.*/
	_funcptr_glVertexArrayVertexAttribLFormatEXT = (PFNGLVERTEXARRAYVERTEXATTRIBLFORMATEXTPROC)IntGetProcAddress("glVertexArrayVertexAttribLFormatEXT");
	/* glVertexArrayVertexAttribLFormatEXT comes from DSA.*/
	_funcptr_glVertexArrayVertexAttribBindingEXT = (PFNGLVERTEXARRAYVERTEXATTRIBBINDINGEXTPROC)IntGetProcAddress("glVertexArrayVertexAttribBindingEXT");
	/* glVertexArrayVertexAttribBindingEXT comes from DSA.*/
	_funcptr_glVertexArrayVertexBindingDivisorEXT = (PFNGLVERTEXARRAYVERTEXBINDINGDIVISOREXTPROC)IntGetProcAddress("glVertexArrayVertexBindingDivisorEXT");
	/* glVertexArrayVertexBindingDivisorEXT comes from DSA.*/
	return numFailed;
}


static int LoadVersion_1_1()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_1_Comp();
	return numFailed;
}

static int LoadVersion_1_2()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_1_Comp();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_2_Comp();
	return numFailed;
}

static int LoadVersion_1_3()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_1_Comp();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_2_Comp();
	numFailed += LoadCore_Version_1_3();
	numFailed += LoadCore_Version_1_3_Comp();
	return numFailed;
}

static int LoadVersion_1_4()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_1_Comp();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_2_Comp();
	numFailed += LoadCore_Version_1_3();
	numFailed += LoadCore_Version_1_3_Comp();
	numFailed += LoadCore_Version_1_4();
	numFailed += LoadCore_Version_1_4_Comp();
	return numFailed;
}

static int LoadVersion_1_5()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_1_Comp();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_2_Comp();
	numFailed += LoadCore_Version_1_3();
	numFailed += LoadCore_Version_1_3_Comp();
	numFailed += LoadCore_Version_1_4();
	numFailed += LoadCore_Version_1_4_Comp();
	numFailed += LoadCore_Version_1_5();
	return numFailed;
}

static int LoadVersion_2_0()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_1_Comp();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_2_Comp();
	numFailed += LoadCore_Version_1_3();
	numFailed += LoadCore_Version_1_3_Comp();
	numFailed += LoadCore_Version_1_4();
	numFailed += LoadCore_Version_1_4_Comp();
	numFailed += LoadCore_Version_1_5();
	numFailed += LoadCore_Version_2_0();
	numFailed += LoadCore_Version_2_0_Comp();
	return numFailed;
}

static int LoadVersion_2_1()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_1_Comp();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_2_Comp();
	numFailed += LoadCore_Version_1_3();
	numFailed += LoadCore_Version_1_3_Comp();
	numFailed += LoadCore_Version_1_4();
	numFailed += LoadCore_Version_1_4_Comp();
	numFailed += LoadCore_Version_1_5();
	numFailed += LoadCore_Version_2_0();
	numFailed += LoadCore_Version_2_0_Comp();
	numFailed += LoadCore_Version_2_1();
	return numFailed;
}

static int LoadVersion_3_0()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_1_Comp();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_2_Comp();
	numFailed += LoadCore_Version_1_3();
	numFailed += LoadCore_Version_1_3_Comp();
	numFailed += LoadCore_Version_1_4();
	numFailed += LoadCore_Version_1_4_Comp();
	numFailed += LoadCore_Version_1_5();
	numFailed += LoadCore_Version_2_0();
	numFailed += LoadCore_Version_2_0_Comp();
	numFailed += LoadCore_Version_2_1();
	numFailed += LoadCore_Version_3_0();
	return numFailed;
}

static int LoadVersion_3_1()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_3();
	numFailed += LoadCore_Version_1_4();
	numFailed += LoadCore_Version_1_5();
	numFailed += LoadCore_Version_2_0();
	numFailed += LoadCore_Version_2_1();
	numFailed += LoadCore_Version_3_0();
	numFailed += LoadCore_Version_3_1();
	return numFailed;
}

static int LoadVersion_3_1_Comp()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_1_Comp();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_2_Comp();
	numFailed += LoadCore_Version_1_3();
	numFailed += LoadCore_Version_1_3_Comp();
	numFailed += LoadCore_Version_1_4();
	numFailed += LoadCore_Version_1_4_Comp();
	numFailed += LoadCore_Version_1_5();
	numFailed += LoadCore_Version_2_0();
	numFailed += LoadCore_Version_2_0_Comp();
	numFailed += LoadCore_Version_2_1();
	numFailed += LoadCore_Version_3_0();
	numFailed += LoadCore_Version_3_1();
	return numFailed;
}

static int LoadVersion_3_2()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_3();
	numFailed += LoadCore_Version_1_4();
	numFailed += LoadCore_Version_1_5();
	numFailed += LoadCore_Version_2_0();
	numFailed += LoadCore_Version_2_1();
	numFailed += LoadCore_Version_3_0();
	numFailed += LoadCore_Version_3_1();
	numFailed += LoadCore_Version_3_2();
	return numFailed;
}

static int LoadVersion_3_2_Comp()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_1_Comp();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_2_Comp();
	numFailed += LoadCore_Version_1_3();
	numFailed += LoadCore_Version_1_3_Comp();
	numFailed += LoadCore_Version_1_4();
	numFailed += LoadCore_Version_1_4_Comp();
	numFailed += LoadCore_Version_1_5();
	numFailed += LoadCore_Version_2_0();
	numFailed += LoadCore_Version_2_0_Comp();
	numFailed += LoadCore_Version_2_1();
	numFailed += LoadCore_Version_3_0();
	numFailed += LoadCore_Version_3_1();
	numFailed += LoadCore_Version_3_2();
	return numFailed;
}

static int LoadVersion_3_3()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_3();
	numFailed += LoadCore_Version_1_4();
	numFailed += LoadCore_Version_1_5();
	numFailed += LoadCore_Version_2_0();
	numFailed += LoadCore_Version_2_1();
	numFailed += LoadCore_Version_3_0();
	numFailed += LoadCore_Version_3_1();
	numFailed += LoadCore_Version_3_2();
	numFailed += LoadCore_Version_3_3();
	return numFailed;
}

static int LoadVersion_3_3_Comp()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_1_Comp();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_2_Comp();
	numFailed += LoadCore_Version_1_3();
	numFailed += LoadCore_Version_1_3_Comp();
	numFailed += LoadCore_Version_1_4();
	numFailed += LoadCore_Version_1_4_Comp();
	numFailed += LoadCore_Version_1_5();
	numFailed += LoadCore_Version_2_0();
	numFailed += LoadCore_Version_2_0_Comp();
	numFailed += LoadCore_Version_2_1();
	numFailed += LoadCore_Version_3_0();
	numFailed += LoadCore_Version_3_1();
	numFailed += LoadCore_Version_3_2();
	numFailed += LoadCore_Version_3_3();
	return numFailed;
}

static int LoadVersion_4_0()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_3();
	numFailed += LoadCore_Version_1_4();
	numFailed += LoadCore_Version_1_5();
	numFailed += LoadCore_Version_2_0();
	numFailed += LoadCore_Version_2_1();
	numFailed += LoadCore_Version_3_0();
	numFailed += LoadCore_Version_3_1();
	numFailed += LoadCore_Version_3_2();
	numFailed += LoadCore_Version_3_3();
	numFailed += LoadCore_Version_4_0();
	return numFailed;
}

static int LoadVersion_4_0_Comp()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_1_Comp();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_2_Comp();
	numFailed += LoadCore_Version_1_3();
	numFailed += LoadCore_Version_1_3_Comp();
	numFailed += LoadCore_Version_1_4();
	numFailed += LoadCore_Version_1_4_Comp();
	numFailed += LoadCore_Version_1_5();
	numFailed += LoadCore_Version_2_0();
	numFailed += LoadCore_Version_2_0_Comp();
	numFailed += LoadCore_Version_2_1();
	numFailed += LoadCore_Version_3_0();
	numFailed += LoadCore_Version_3_1();
	numFailed += LoadCore_Version_3_2();
	numFailed += LoadCore_Version_3_3();
	numFailed += LoadCore_Version_4_0();
	return numFailed;
}

static int LoadVersion_4_1()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_3();
	numFailed += LoadCore_Version_1_4();
	numFailed += LoadCore_Version_1_5();
	numFailed += LoadCore_Version_2_0();
	numFailed += LoadCore_Version_2_1();
	numFailed += LoadCore_Version_3_0();
	numFailed += LoadCore_Version_3_1();
	numFailed += LoadCore_Version_3_2();
	numFailed += LoadCore_Version_3_3();
	numFailed += LoadCore_Version_4_0();
	numFailed += LoadCore_Version_4_1();
	return numFailed;
}

static int LoadVersion_4_1_Comp()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_1_Comp();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_2_Comp();
	numFailed += LoadCore_Version_1_3();
	numFailed += LoadCore_Version_1_3_Comp();
	numFailed += LoadCore_Version_1_4();
	numFailed += LoadCore_Version_1_4_Comp();
	numFailed += LoadCore_Version_1_5();
	numFailed += LoadCore_Version_2_0();
	numFailed += LoadCore_Version_2_0_Comp();
	numFailed += LoadCore_Version_2_1();
	numFailed += LoadCore_Version_3_0();
	numFailed += LoadCore_Version_3_1();
	numFailed += LoadCore_Version_3_2();
	numFailed += LoadCore_Version_3_3();
	numFailed += LoadCore_Version_4_0();
	numFailed += LoadCore_Version_4_1();
	return numFailed;
}

static int LoadVersion_4_2()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_3();
	numFailed += LoadCore_Version_1_4();
	numFailed += LoadCore_Version_1_5();
	numFailed += LoadCore_Version_2_0();
	numFailed += LoadCore_Version_2_1();
	numFailed += LoadCore_Version_3_0();
	numFailed += LoadCore_Version_3_1();
	numFailed += LoadCore_Version_3_2();
	numFailed += LoadCore_Version_3_3();
	numFailed += LoadCore_Version_4_0();
	numFailed += LoadCore_Version_4_1();
	numFailed += LoadCore_Version_4_2();
	return numFailed;
}

static int LoadVersion_4_2_Comp()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_1_Comp();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_2_Comp();
	numFailed += LoadCore_Version_1_3();
	numFailed += LoadCore_Version_1_3_Comp();
	numFailed += LoadCore_Version_1_4();
	numFailed += LoadCore_Version_1_4_Comp();
	numFailed += LoadCore_Version_1_5();
	numFailed += LoadCore_Version_2_0();
	numFailed += LoadCore_Version_2_0_Comp();
	numFailed += LoadCore_Version_2_1();
	numFailed += LoadCore_Version_3_0();
	numFailed += LoadCore_Version_3_1();
	numFailed += LoadCore_Version_3_2();
	numFailed += LoadCore_Version_3_3();
	numFailed += LoadCore_Version_4_0();
	numFailed += LoadCore_Version_4_1();
	numFailed += LoadCore_Version_4_2();
	return numFailed;
}

static int LoadVersion_4_3()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_3();
	numFailed += LoadCore_Version_1_4();
	numFailed += LoadCore_Version_1_5();
	numFailed += LoadCore_Version_2_0();
	numFailed += LoadCore_Version_2_1();
	numFailed += LoadCore_Version_3_0();
	numFailed += LoadCore_Version_3_1();
	numFailed += LoadCore_Version_3_2();
	numFailed += LoadCore_Version_3_3();
	numFailed += LoadCore_Version_4_0();
	numFailed += LoadCore_Version_4_1();
	numFailed += LoadCore_Version_4_2();
	numFailed += LoadCore_Version_4_3();
	return numFailed;
}

static int LoadVersion_4_3_Comp()
{
	int numFailed = 0;
	numFailed += LoadCore_Version_1_1();
	numFailed += LoadCore_Version_1_1_Comp();
	numFailed += LoadCore_Version_1_2();
	numFailed += LoadCore_Version_1_2_Comp();
	numFailed += LoadCore_Version_1_3();
	numFailed += LoadCore_Version_1_3_Comp();
	numFailed += LoadCore_Version_1_4();
	numFailed += LoadCore_Version_1_4_Comp();
	numFailed += LoadCore_Version_1_5();
	numFailed += LoadCore_Version_2_0();
	numFailed += LoadCore_Version_2_0_Comp();
	numFailed += LoadCore_Version_2_1();
	numFailed += LoadCore_Version_3_0();
	numFailed += LoadCore_Version_3_1();
	numFailed += LoadCore_Version_3_2();
	numFailed += LoadCore_Version_3_3();
	numFailed += LoadCore_Version_4_0();
	numFailed += LoadCore_Version_4_1();
	numFailed += LoadCore_Version_4_2();
	numFailed += LoadCore_Version_4_3();
	return numFailed;
}

typedef int (*PFN_LOADFUNCPOINTERS)();
typedef struct ogl_StrToExtMap_s
{
	char *extensionName;
	int *extensionVariable;
	PFN_LOADFUNCPOINTERS LoadExtension;
} ogl_StrToExtMap;

static ogl_StrToExtMap ExtensionTable[448] = {
	{"GL_ARB_imaging", &glext_ARB_imaging, NULL},
	{"GL_ARB_vertex_array_object", &glext_ARB_vertex_array_object, LoadExt_ARB_vertex_array_object},
	{"GL_ARB_texture_rg", &glext_ARB_texture_rg, NULL},
	{"GL_ARB_texture_compression_rgtc", &glext_ARB_texture_compression_rgtc, NULL},
	{"GL_ARB_map_buffer_range", &glext_ARB_map_buffer_range, LoadExt_ARB_map_buffer_range},
	{"GL_ARB_half_float_vertex", &glext_ARB_half_float_vertex, NULL},
	{"GL_ARB_framebuffer_sRGB", &glext_ARB_framebuffer_sRGB, NULL},
	{"GL_ARB_framebuffer_object", &glext_ARB_framebuffer_object, LoadExt_ARB_framebuffer_object},
	{"GL_ARB_depth_buffer_float", &glext_ARB_depth_buffer_float, NULL},
	{"GL_ARB_uniform_buffer_object", &glext_ARB_uniform_buffer_object, LoadExt_ARB_uniform_buffer_object},
	{"GL_ARB_copy_buffer", &glext_ARB_copy_buffer, LoadExt_ARB_copy_buffer},
	{"GL_ARB_depth_clamp", &glext_ARB_depth_clamp, NULL},
	{"GL_ARB_draw_elements_base_vertex", &glext_ARB_draw_elements_base_vertex, LoadExt_ARB_draw_elements_base_vertex},
	{"GL_ARB_fragment_coord_conventions", &glext_ARB_fragment_coord_conventions, NULL},
	{"GL_ARB_provoking_vertex", &glext_ARB_provoking_vertex, LoadExt_ARB_provoking_vertex},
	{"GL_ARB_seamless_cube_map", &glext_ARB_seamless_cube_map, NULL},
	{"GL_ARB_sync", &glext_ARB_sync, LoadExt_ARB_sync},
	{"GL_ARB_texture_multisample", &glext_ARB_texture_multisample, LoadExt_ARB_texture_multisample},
	{"GL_ARB_vertex_array_bgra", &glext_ARB_vertex_array_bgra, NULL},
	{"GL_ARB_texture_rgb10_a2ui", &glext_ARB_texture_rgb10_a2ui, NULL},
	{"GL_ARB_texture_swizzle", &glext_ARB_texture_swizzle, NULL},
	{"GL_ARB_timer_query", &glext_ARB_timer_query, LoadExt_ARB_timer_query},
	{"GL_ARB_vertex_type_2_10_10_10_rev", &glext_ARB_vertex_type_2_10_10_10_rev, LoadExt_ARB_vertex_type_2_10_10_10_rev},
	{"GL_ARB_blend_func_extended", &glext_ARB_blend_func_extended, LoadExt_ARB_blend_func_extended},
	{"GL_ARB_occlusion_query2", &glext_ARB_occlusion_query2, NULL},
	{"GL_ARB_sampler_objects", &glext_ARB_sampler_objects, LoadExt_ARB_sampler_objects},
	{"GL_ARB_draw_indirect", &glext_ARB_draw_indirect, LoadExt_ARB_draw_indirect},
	{"GL_ARB_gpu_shader5", &glext_ARB_gpu_shader5, NULL},
	{"GL_ARB_gpu_shader_fp64", &glext_ARB_gpu_shader_fp64, LoadExt_ARB_gpu_shader_fp64},
	{"GL_ARB_shader_subroutine", &glext_ARB_shader_subroutine, LoadExt_ARB_shader_subroutine},
	{"GL_ARB_tessellation_shader", &glext_ARB_tessellation_shader, LoadExt_ARB_tessellation_shader},
	{"GL_ARB_transform_feedback2", &glext_ARB_transform_feedback2, LoadExt_ARB_transform_feedback2},
	{"GL_ARB_transform_feedback3", &glext_ARB_transform_feedback3, LoadExt_ARB_transform_feedback3},
	{"GL_ARB_ES2_compatibility", &glext_ARB_ES2_compatibility, LoadExt_ARB_ES2_compatibility},
	{"GL_ARB_get_program_binary", &glext_ARB_get_program_binary, LoadExt_ARB_get_program_binary},
	{"GL_ARB_separate_shader_objects", &glext_ARB_separate_shader_objects, LoadExt_ARB_separate_shader_objects},
	{"GL_ARB_vertex_attrib_64bit", &glext_ARB_vertex_attrib_64bit, LoadExt_ARB_vertex_attrib_64bit},
	{"GL_ARB_viewport_array", &glext_ARB_viewport_array, LoadExt_ARB_viewport_array},
	{"GL_ARB_base_instance", &glext_ARB_base_instance, LoadExt_ARB_base_instance},
	{"GL_ARB_shading_language_420pack", &glext_ARB_shading_language_420pack, NULL},
	{"GL_ARB_transform_feedback_instanced", &glext_ARB_transform_feedback_instanced, LoadExt_ARB_transform_feedback_instanced},
	{"GL_ARB_compressed_texture_pixel_storage", &glext_ARB_compressed_texture_pixel_storage, NULL},
	{"GL_ARB_conservative_depth", &glext_ARB_conservative_depth, NULL},
	{"GL_ARB_internalformat_query", &glext_ARB_internalformat_query, LoadExt_ARB_internalformat_query},
	{"GL_ARB_map_buffer_alignment", &glext_ARB_map_buffer_alignment, NULL},
	{"GL_ARB_shader_atomic_counters", &glext_ARB_shader_atomic_counters, LoadExt_ARB_shader_atomic_counters},
	{"GL_ARB_shader_image_load_store", &glext_ARB_shader_image_load_store, LoadExt_ARB_shader_image_load_store},
	{"GL_ARB_shading_language_packing", &glext_ARB_shading_language_packing, NULL},
	{"GL_ARB_texture_storage", &glext_ARB_texture_storage, LoadExt_ARB_texture_storage},
	{"GL_KHR_debug", &glext_KHR_debug, LoadExt_KHR_debug},
	{"GL_ARB_arrays_of_arrays", &glext_ARB_arrays_of_arrays, NULL},
	{"GL_ARB_clear_buffer_object", &glext_ARB_clear_buffer_object, LoadExt_ARB_clear_buffer_object},
	{"GL_ARB_compute_shader", &glext_ARB_compute_shader, LoadExt_ARB_compute_shader},
	{"GL_ARB_copy_image", &glext_ARB_copy_image, LoadExt_ARB_copy_image},
	{"GL_ARB_ES3_compatibility", &glext_ARB_ES3_compatibility, NULL},
	{"GL_ARB_explicit_uniform_location", &glext_ARB_explicit_uniform_location, NULL},
	{"GL_ARB_fragment_layer_viewport", &glext_ARB_fragment_layer_viewport, NULL},
	{"GL_ARB_framebuffer_no_attachments", &glext_ARB_framebuffer_no_attachments, LoadExt_ARB_framebuffer_no_attachments},
	{"GL_ARB_internalformat_query2", &glext_ARB_internalformat_query2, LoadExt_ARB_internalformat_query2},
	{"GL_ARB_invalidate_subdata", &glext_ARB_invalidate_subdata, LoadExt_ARB_invalidate_subdata},
	{"GL_ARB_multi_draw_indirect", &glext_ARB_multi_draw_indirect, LoadExt_ARB_multi_draw_indirect},
	{"GL_ARB_program_interface_query", &glext_ARB_program_interface_query, LoadExt_ARB_program_interface_query},
	{"GL_ARB_shader_image_size", &glext_ARB_shader_image_size, NULL},
	{"GL_ARB_shader_storage_buffer_object", &glext_ARB_shader_storage_buffer_object, LoadExt_ARB_shader_storage_buffer_object},
	{"GL_ARB_stencil_texturing", &glext_ARB_stencil_texturing, NULL},
	{"GL_ARB_texture_buffer_range", &glext_ARB_texture_buffer_range, LoadExt_ARB_texture_buffer_range},
	{"GL_ARB_texture_query_levels", &glext_ARB_texture_query_levels, NULL},
	{"GL_ARB_texture_storage_multisample", &glext_ARB_texture_storage_multisample, LoadExt_ARB_texture_storage_multisample},
	{"GL_ARB_texture_view", &glext_ARB_texture_view, LoadExt_ARB_texture_view},
	{"GL_ARB_vertex_attrib_binding", &glext_ARB_vertex_attrib_binding, LoadExt_ARB_vertex_attrib_binding},
	{"GL_3DFX_multisample", &glext_3DFX_multisample, NULL},
	{"GL_3DFX_tbuffer", &glext_3DFX_tbuffer, LoadExt_3DFX_tbuffer},
	{"GL_3DFX_texture_compression_FXT1", &glext_3DFX_texture_compression_FXT1, NULL},
	{"GL_AMD_blend_minmax_factor", &glext_AMD_blend_minmax_factor, NULL},
	{"GL_AMD_conservative_depth", &glext_AMD_conservative_depth, NULL},
	{"GL_AMD_debug_output", &glext_AMD_debug_output, LoadExt_AMD_debug_output},
	{"GL_AMD_depth_clamp_separate", &glext_AMD_depth_clamp_separate, NULL},
	{"GL_AMD_draw_buffers_blend", &glext_AMD_draw_buffers_blend, LoadExt_AMD_draw_buffers_blend},
	{"GL_AMD_multi_draw_indirect", &glext_AMD_multi_draw_indirect, LoadExt_AMD_multi_draw_indirect},
	{"GL_AMD_name_gen_delete", &glext_AMD_name_gen_delete, LoadExt_AMD_name_gen_delete},
	{"GL_AMD_performance_monitor", &glext_AMD_performance_monitor, LoadExt_AMD_performance_monitor},
	{"GL_AMD_pinned_memory", &glext_AMD_pinned_memory, NULL},
	{"GL_AMD_query_buffer_object", &glext_AMD_query_buffer_object, NULL},
	{"GL_AMD_sample_positions", &glext_AMD_sample_positions, LoadExt_AMD_sample_positions},
	{"GL_AMD_seamless_cubemap_per_texture", &glext_AMD_seamless_cubemap_per_texture, NULL},
	{"GL_AMD_shader_stencil_export", &glext_AMD_shader_stencil_export, NULL},
	{"GL_AMD_shader_trinary_minmax", &glext_AMD_shader_trinary_minmax, NULL},
	{"GL_AMD_sparse_texture", &glext_AMD_sparse_texture, LoadExt_AMD_sparse_texture},
	{"GL_AMD_stencil_operation_extended", &glext_AMD_stencil_operation_extended, LoadExt_AMD_stencil_operation_extended},
	{"GL_AMD_texture_texture4", &glext_AMD_texture_texture4, NULL},
	{"GL_AMD_transform_feedback3_lines_triangles", &glext_AMD_transform_feedback3_lines_triangles, NULL},
	{"GL_AMD_vertex_shader_layer", &glext_AMD_vertex_shader_layer, NULL},
	{"GL_AMD_vertex_shader_tessellator", &glext_AMD_vertex_shader_tessellator, LoadExt_AMD_vertex_shader_tessellator},
	{"GL_AMD_vertex_shader_viewport_index", &glext_AMD_vertex_shader_viewport_index, NULL},
	{"GL_APPLE_aux_depth_stencil", &glext_APPLE_aux_depth_stencil, NULL},
	{"GL_APPLE_client_storage", &glext_APPLE_client_storage, NULL},
	{"GL_APPLE_element_array", &glext_APPLE_element_array, LoadExt_APPLE_element_array},
	{"GL_APPLE_fence", &glext_APPLE_fence, LoadExt_APPLE_fence},
	{"GL_APPLE_float_pixels", &glext_APPLE_float_pixels, NULL},
	{"GL_APPLE_flush_buffer_range", &glext_APPLE_flush_buffer_range, LoadExt_APPLE_flush_buffer_range},
	{"GL_APPLE_object_purgeable", &glext_APPLE_object_purgeable, LoadExt_APPLE_object_purgeable},
	{"GL_APPLE_rgb_422", &glext_APPLE_rgb_422, NULL},
	{"GL_APPLE_row_bytes", &glext_APPLE_row_bytes, NULL},
	{"GL_APPLE_specular_vector", &glext_APPLE_specular_vector, NULL},
	{"GL_APPLE_texture_range", &glext_APPLE_texture_range, LoadExt_APPLE_texture_range},
	{"GL_APPLE_transform_hint", &glext_APPLE_transform_hint, NULL},
	{"GL_APPLE_vertex_array_object", &glext_APPLE_vertex_array_object, LoadExt_APPLE_vertex_array_object},
	{"GL_APPLE_vertex_array_range", &glext_APPLE_vertex_array_range, LoadExt_APPLE_vertex_array_range},
	{"GL_APPLE_vertex_program_evaluators", &glext_APPLE_vertex_program_evaluators, LoadExt_APPLE_vertex_program_evaluators},
	{"GL_APPLE_ycbcr_422", &glext_APPLE_ycbcr_422, NULL},
	{"GL_ARB_cl_event", &glext_ARB_cl_event, LoadExt_ARB_cl_event},
	{"GL_ARB_color_buffer_float", &glext_ARB_color_buffer_float, LoadExt_ARB_color_buffer_float},
	{"GL_ARB_compatibility", &glext_ARB_compatibility, NULL},
	{"GL_ARB_debug_output", &glext_ARB_debug_output, LoadExt_ARB_debug_output},
	{"GL_ARB_depth_texture", &glext_ARB_depth_texture, NULL},
	{"GL_ARB_draw_buffers", &glext_ARB_draw_buffers, LoadExt_ARB_draw_buffers},
	{"GL_ARB_draw_buffers_blend", &glext_ARB_draw_buffers_blend, LoadExt_ARB_draw_buffers_blend},
	{"GL_ARB_draw_instanced", &glext_ARB_draw_instanced, LoadExt_ARB_draw_instanced},
	{"GL_ARB_explicit_attrib_location", &glext_ARB_explicit_attrib_location, NULL},
	{"GL_ARB_fragment_program", &glext_ARB_fragment_program, NULL},
	{"GL_ARB_fragment_program_shadow", &glext_ARB_fragment_program_shadow, NULL},
	{"GL_ARB_fragment_shader", &glext_ARB_fragment_shader, NULL},
	{"GL_ARB_geometry_shader4", &glext_ARB_geometry_shader4, LoadExt_ARB_geometry_shader4},
	{"GL_ARB_half_float_pixel", &glext_ARB_half_float_pixel, NULL},
	{"GL_ARB_instanced_arrays", &glext_ARB_instanced_arrays, LoadExt_ARB_instanced_arrays},
	{"GL_ARB_matrix_palette", &glext_ARB_matrix_palette, LoadExt_ARB_matrix_palette},
	{"GL_ARB_multisample", &glext_ARB_multisample, LoadExt_ARB_multisample},
	{"GL_ARB_multitexture", &glext_ARB_multitexture, LoadExt_ARB_multitexture},
	{"GL_ARB_occlusion_query", &glext_ARB_occlusion_query, LoadExt_ARB_occlusion_query},
	{"GL_ARB_pixel_buffer_object", &glext_ARB_pixel_buffer_object, NULL},
	{"GL_ARB_point_parameters", &glext_ARB_point_parameters, LoadExt_ARB_point_parameters},
	{"GL_ARB_point_sprite", &glext_ARB_point_sprite, NULL},
	{"GL_ARB_robust_buffer_access_behavior", &glext_ARB_robust_buffer_access_behavior, NULL},
	{"GL_ARB_robustness", &glext_ARB_robustness, LoadExt_ARB_robustness},
	{"GL_ARB_robustness_isolation", &glext_ARB_robustness_isolation, NULL},
	{"GL_ARB_sample_shading", &glext_ARB_sample_shading, LoadExt_ARB_sample_shading},
	{"GL_ARB_shader_bit_encoding", &glext_ARB_shader_bit_encoding, NULL},
	{"GL_ARB_shader_objects", &glext_ARB_shader_objects, LoadExt_ARB_shader_objects},
	{"GL_ARB_shader_precision", &glext_ARB_shader_precision, NULL},
	{"GL_ARB_shader_stencil_export", &glext_ARB_shader_stencil_export, NULL},
	{"GL_ARB_shader_texture_lod", &glext_ARB_shader_texture_lod, NULL},
	{"GL_ARB_shading_language_100", &glext_ARB_shading_language_100, NULL},
	{"GL_ARB_shading_language_include", &glext_ARB_shading_language_include, LoadExt_ARB_shading_language_include},
	{"GL_ARB_shadow", &glext_ARB_shadow, NULL},
	{"GL_ARB_shadow_ambient", &glext_ARB_shadow_ambient, NULL},
	{"GL_ARB_texture_border_clamp", &glext_ARB_texture_border_clamp, NULL},
	{"GL_ARB_texture_buffer_object", &glext_ARB_texture_buffer_object, LoadExt_ARB_texture_buffer_object},
	{"GL_ARB_texture_buffer_object_rgb32", &glext_ARB_texture_buffer_object_rgb32, NULL},
	{"GL_ARB_texture_compression", &glext_ARB_texture_compression, LoadExt_ARB_texture_compression},
	{"GL_ARB_texture_compression_bptc", &glext_ARB_texture_compression_bptc, NULL},
	{"GL_ARB_texture_cube_map", &glext_ARB_texture_cube_map, NULL},
	{"GL_ARB_texture_cube_map_array", &glext_ARB_texture_cube_map_array, NULL},
	{"GL_ARB_texture_env_add", &glext_ARB_texture_env_add, NULL},
	{"GL_ARB_texture_env_combine", &glext_ARB_texture_env_combine, NULL},
	{"GL_ARB_texture_env_crossbar", &glext_ARB_texture_env_crossbar, NULL},
	{"GL_ARB_texture_env_dot3", &glext_ARB_texture_env_dot3, NULL},
	{"GL_ARB_texture_float", &glext_ARB_texture_float, NULL},
	{"GL_ARB_texture_gather", &glext_ARB_texture_gather, NULL},
	{"GL_ARB_texture_mirrored_repeat", &glext_ARB_texture_mirrored_repeat, NULL},
	{"GL_ARB_texture_non_power_of_two", &glext_ARB_texture_non_power_of_two, NULL},
	{"GL_ARB_texture_query_lod", &glext_ARB_texture_query_lod, NULL},
	{"GL_ARB_texture_rectangle", &glext_ARB_texture_rectangle, NULL},
	{"GL_ARB_transpose_matrix", &glext_ARB_transpose_matrix, LoadExt_ARB_transpose_matrix},
	{"GL_ARB_vertex_blend", &glext_ARB_vertex_blend, LoadExt_ARB_vertex_blend},
	{"GL_ARB_vertex_buffer_object", &glext_ARB_vertex_buffer_object, LoadExt_ARB_vertex_buffer_object},
	{"GL_ARB_vertex_program", &glext_ARB_vertex_program, LoadExt_ARB_vertex_program},
	{"GL_ARB_vertex_shader", &glext_ARB_vertex_shader, LoadExt_ARB_vertex_shader},
	{"GL_ARB_window_pos", &glext_ARB_window_pos, LoadExt_ARB_window_pos},
	{"GL_ATI_draw_buffers", &glext_ATI_draw_buffers, LoadExt_ATI_draw_buffers},
	{"GL_ATI_element_array", &glext_ATI_element_array, LoadExt_ATI_element_array},
	{"GL_ATI_envmap_bumpmap", &glext_ATI_envmap_bumpmap, LoadExt_ATI_envmap_bumpmap},
	{"GL_ATI_fragment_shader", &glext_ATI_fragment_shader, LoadExt_ATI_fragment_shader},
	{"GL_ATI_map_object_buffer", &glext_ATI_map_object_buffer, LoadExt_ATI_map_object_buffer},
	{"GL_ATI_meminfo", &glext_ATI_meminfo, NULL},
	{"GL_ATI_pixel_format_float", &glext_ATI_pixel_format_float, NULL},
	{"GL_ATI_pn_triangles", &glext_ATI_pn_triangles, LoadExt_ATI_pn_triangles},
	{"GL_ATI_separate_stencil", &glext_ATI_separate_stencil, LoadExt_ATI_separate_stencil},
	{"GL_ATI_text_fragment_shader", &glext_ATI_text_fragment_shader, NULL},
	{"GL_ATI_texture_env_combine3", &glext_ATI_texture_env_combine3, NULL},
	{"GL_ATI_texture_float", &glext_ATI_texture_float, NULL},
	{"GL_ATI_texture_mirror_once", &glext_ATI_texture_mirror_once, NULL},
	{"GL_ATI_vertex_array_object", &glext_ATI_vertex_array_object, LoadExt_ATI_vertex_array_object},
	{"GL_ATI_vertex_attrib_array_object", &glext_ATI_vertex_attrib_array_object, LoadExt_ATI_vertex_attrib_array_object},
	{"GL_ATI_vertex_streams", &glext_ATI_vertex_streams, LoadExt_ATI_vertex_streams},
	{"GL_EXT_422_pixels", &glext_EXT_422_pixels, NULL},
	{"GL_EXT_abgr", &glext_EXT_abgr, NULL},
	{"GL_EXT_bgra", &glext_EXT_bgra, NULL},
	{"GL_EXT_bindable_uniform", &glext_EXT_bindable_uniform, LoadExt_EXT_bindable_uniform},
	{"GL_EXT_blend_color", &glext_EXT_blend_color, LoadExt_EXT_blend_color},
	{"GL_EXT_blend_equation_separate", &glext_EXT_blend_equation_separate, LoadExt_EXT_blend_equation_separate},
	{"GL_EXT_blend_func_separate", &glext_EXT_blend_func_separate, LoadExt_EXT_blend_func_separate},
	{"GL_EXT_blend_logic_op", &glext_EXT_blend_logic_op, NULL},
	{"GL_EXT_blend_minmax", &glext_EXT_blend_minmax, LoadExt_EXT_blend_minmax},
	{"GL_EXT_blend_subtract", &glext_EXT_blend_subtract, NULL},
	{"GL_EXT_clip_volume_hint", &glext_EXT_clip_volume_hint, NULL},
	{"GL_EXT_cmyka", &glext_EXT_cmyka, NULL},
	{"GL_EXT_color_subtable", &glext_EXT_color_subtable, LoadExt_EXT_color_subtable},
	{"GL_EXT_compiled_vertex_array", &glext_EXT_compiled_vertex_array, LoadExt_EXT_compiled_vertex_array},
	{"GL_EXT_convolution", &glext_EXT_convolution, LoadExt_EXT_convolution},
	{"GL_EXT_coordinate_frame", &glext_EXT_coordinate_frame, LoadExt_EXT_coordinate_frame},
	{"GL_EXT_copy_texture", &glext_EXT_copy_texture, LoadExt_EXT_copy_texture},
	{"GL_EXT_cull_vertex", &glext_EXT_cull_vertex, LoadExt_EXT_cull_vertex},
	{"GL_EXT_depth_bounds_test", &glext_EXT_depth_bounds_test, LoadExt_EXT_depth_bounds_test},
	{"GL_EXT_direct_state_access", &glext_EXT_direct_state_access, LoadExt_EXT_direct_state_access},
	{"GL_EXT_draw_buffers2", &glext_EXT_draw_buffers2, LoadExt_EXT_draw_buffers2},
	{"GL_EXT_draw_instanced", &glext_EXT_draw_instanced, LoadExt_EXT_draw_instanced},
	{"GL_EXT_draw_range_elements", &glext_EXT_draw_range_elements, LoadExt_EXT_draw_range_elements},
	{"GL_EXT_fog_coord", &glext_EXT_fog_coord, LoadExt_EXT_fog_coord},
	{"GL_EXT_framebuffer_blit", &glext_EXT_framebuffer_blit, LoadExt_EXT_framebuffer_blit},
	{"GL_EXT_framebuffer_multisample", &glext_EXT_framebuffer_multisample, LoadExt_EXT_framebuffer_multisample},
	{"GL_EXT_framebuffer_multisample_blit_scaled", &glext_EXT_framebuffer_multisample_blit_scaled, NULL},
	{"GL_EXT_framebuffer_object", &glext_EXT_framebuffer_object, LoadExt_EXT_framebuffer_object},
	{"GL_EXT_framebuffer_sRGB", &glext_EXT_framebuffer_sRGB, NULL},
	{"GL_EXT_geometry_shader4", &glext_EXT_geometry_shader4, LoadExt_EXT_geometry_shader4},
	{"GL_EXT_gpu_program_parameters", &glext_EXT_gpu_program_parameters, LoadExt_EXT_gpu_program_parameters},
	{"GL_EXT_gpu_shader4", &glext_EXT_gpu_shader4, LoadExt_EXT_gpu_shader4},
	{"GL_EXT_histogram", &glext_EXT_histogram, LoadExt_EXT_histogram},
	{"GL_EXT_index_array_formats", &glext_EXT_index_array_formats, NULL},
	{"GL_EXT_index_func", &glext_EXT_index_func, LoadExt_EXT_index_func},
	{"GL_EXT_index_material", &glext_EXT_index_material, LoadExt_EXT_index_material},
	{"GL_EXT_index_texture", &glext_EXT_index_texture, NULL},
	{"GL_EXT_light_texture", &glext_EXT_light_texture, LoadExt_EXT_light_texture},
	{"GL_EXT_misc_attribute", &glext_EXT_misc_attribute, NULL},
	{"GL_EXT_multi_draw_arrays", &glext_EXT_multi_draw_arrays, LoadExt_EXT_multi_draw_arrays},
	{"GL_EXT_multisample", &glext_EXT_multisample, LoadExt_EXT_multisample},
	{"GL_EXT_packed_depth_stencil", &glext_EXT_packed_depth_stencil, NULL},
	{"GL_EXT_packed_float", &glext_EXT_packed_float, NULL},
	{"GL_EXT_packed_pixels", &glext_EXT_packed_pixels, NULL},
	{"GL_EXT_paletted_texture", &glext_EXT_paletted_texture, LoadExt_EXT_paletted_texture},
	{"GL_EXT_pixel_buffer_object", &glext_EXT_pixel_buffer_object, NULL},
	{"GL_EXT_pixel_transform", &glext_EXT_pixel_transform, LoadExt_EXT_pixel_transform},
	{"GL_EXT_pixel_transform_color_table", &glext_EXT_pixel_transform_color_table, NULL},
	{"GL_EXT_point_parameters", &glext_EXT_point_parameters, LoadExt_EXT_point_parameters},
	{"GL_EXT_polygon_offset", &glext_EXT_polygon_offset, LoadExt_EXT_polygon_offset},
	{"GL_EXT_provoking_vertex", &glext_EXT_provoking_vertex, LoadExt_EXT_provoking_vertex},
	{"GL_EXT_rescale_normal", &glext_EXT_rescale_normal, NULL},
	{"GL_EXT_secondary_color", &glext_EXT_secondary_color, LoadExt_EXT_secondary_color},
	{"GL_EXT_separate_shader_objects", &glext_EXT_separate_shader_objects, LoadExt_EXT_separate_shader_objects},
	{"GL_EXT_separate_specular_color", &glext_EXT_separate_specular_color, NULL},
	{"GL_EXT_shader_image_load_store", &glext_EXT_shader_image_load_store, LoadExt_EXT_shader_image_load_store},
	{"GL_EXT_shadow_funcs", &glext_EXT_shadow_funcs, NULL},
	{"GL_EXT_shared_texture_palette", &glext_EXT_shared_texture_palette, NULL},
	{"GL_EXT_stencil_clear_tag", &glext_EXT_stencil_clear_tag, LoadExt_EXT_stencil_clear_tag},
	{"GL_EXT_stencil_two_side", &glext_EXT_stencil_two_side, LoadExt_EXT_stencil_two_side},
	{"GL_EXT_stencil_wrap", &glext_EXT_stencil_wrap, NULL},
	{"GL_EXT_subtexture", &glext_EXT_subtexture, LoadExt_EXT_subtexture},
	{"GL_EXT_texture", &glext_EXT_texture, NULL},
	{"GL_EXT_texture3D", &glext_EXT_texture3D, LoadExt_EXT_texture3D},
	{"GL_EXT_texture_array", &glext_EXT_texture_array, NULL},
	{"GL_EXT_texture_buffer_object", &glext_EXT_texture_buffer_object, LoadExt_EXT_texture_buffer_object},
	{"GL_EXT_texture_compression_latc", &glext_EXT_texture_compression_latc, NULL},
	{"GL_EXT_texture_compression_rgtc", &glext_EXT_texture_compression_rgtc, NULL},
	{"GL_EXT_texture_compression_s3tc", &glext_EXT_texture_compression_s3tc, NULL},
	{"GL_EXT_texture_cube_map", &glext_EXT_texture_cube_map, NULL},
	{"GL_EXT_texture_env_add", &glext_EXT_texture_env_add, NULL},
	{"GL_EXT_texture_env_combine", &glext_EXT_texture_env_combine, NULL},
	{"GL_EXT_texture_env_dot3", &glext_EXT_texture_env_dot3, NULL},
	{"GL_EXT_texture_filter_anisotropic", &glext_EXT_texture_filter_anisotropic, NULL},
	{"GL_EXT_texture_integer", &glext_EXT_texture_integer, LoadExt_EXT_texture_integer},
	{"GL_EXT_texture_lod_bias", &glext_EXT_texture_lod_bias, NULL},
	{"GL_EXT_texture_mirror_clamp", &glext_EXT_texture_mirror_clamp, NULL},
	{"GL_EXT_texture_object", &glext_EXT_texture_object, LoadExt_EXT_texture_object},
	{"GL_EXT_texture_perturb_normal", &glext_EXT_texture_perturb_normal, LoadExt_EXT_texture_perturb_normal},
	{"GL_EXT_texture_sRGB", &glext_EXT_texture_sRGB, NULL},
	{"GL_EXT_texture_sRGB_decode", &glext_EXT_texture_sRGB_decode, NULL},
	{"GL_EXT_texture_shared_exponent", &glext_EXT_texture_shared_exponent, NULL},
	{"GL_EXT_texture_snorm", &glext_EXT_texture_snorm, NULL},
	{"GL_EXT_texture_swizzle", &glext_EXT_texture_swizzle, NULL},
	{"GL_EXT_timer_query", &glext_EXT_timer_query, LoadExt_EXT_timer_query},
	{"GL_EXT_transform_feedback", &glext_EXT_transform_feedback, LoadExt_EXT_transform_feedback},
	{"GL_EXT_vertex_array", &glext_EXT_vertex_array, LoadExt_EXT_vertex_array},
	{"GL_EXT_vertex_array_bgra", &glext_EXT_vertex_array_bgra, NULL},
	{"GL_EXT_vertex_attrib_64bit", &glext_EXT_vertex_attrib_64bit, LoadExt_EXT_vertex_attrib_64bit},
	{"GL_EXT_vertex_shader", &glext_EXT_vertex_shader, LoadExt_EXT_vertex_shader},
	{"GL_EXT_vertex_weighting", &glext_EXT_vertex_weighting, LoadExt_EXT_vertex_weighting},
	{"GL_EXT_x11_sync_object", &glext_EXT_x11_sync_object, LoadExt_EXT_x11_sync_object},
	{"GL_GREMEDY_frame_terminator", &glext_GREMEDY_frame_terminator, LoadExt_GREMEDY_frame_terminator},
	{"GL_GREMEDY_string_marker", &glext_GREMEDY_string_marker, LoadExt_GREMEDY_string_marker},
	{"GL_HP_convolution_border_modes", &glext_HP_convolution_border_modes, NULL},
	{"GL_HP_image_transform", &glext_HP_image_transform, LoadExt_HP_image_transform},
	{"GL_HP_occlusion_test", &glext_HP_occlusion_test, NULL},
	{"GL_HP_texture_lighting", &glext_HP_texture_lighting, NULL},
	{"GL_IBM_cull_vertex", &glext_IBM_cull_vertex, NULL},
	{"GL_IBM_multimode_draw_arrays", &glext_IBM_multimode_draw_arrays, LoadExt_IBM_multimode_draw_arrays},
	{"GL_IBM_rasterpos_clip", &glext_IBM_rasterpos_clip, NULL},
	{"GL_IBM_static_data", &glext_IBM_static_data, LoadExt_IBM_static_data},
	{"GL_IBM_texture_mirrored_repeat", &glext_IBM_texture_mirrored_repeat, NULL},
	{"GL_IBM_vertex_array_lists", &glext_IBM_vertex_array_lists, LoadExt_IBM_vertex_array_lists},
	{"GL_INGR_color_clamp", &glext_INGR_color_clamp, NULL},
	{"GL_INGR_interlace_read", &glext_INGR_interlace_read, NULL},
	{"GL_INGR_palette_buffer", &glext_INGR_palette_buffer, NULL},
	{"GL_INTEL_map_texture", &glext_INTEL_map_texture, LoadExt_INTEL_map_texture},
	{"GL_INTEL_parallel_arrays", &glext_INTEL_parallel_arrays, LoadExt_INTEL_parallel_arrays},
	{"GL_INTEL_texture_scissor", &glext_INTEL_texture_scissor, NULL},
	{"GL_KHR_texture_compression_astc_ldr", &glext_KHR_texture_compression_astc_ldr, NULL},
	{"GL_MESAX_texture_stack", &glext_MESAX_texture_stack, NULL},
	{"GL_MESA_pack_invert", &glext_MESA_pack_invert, NULL},
	{"GL_MESA_resize_buffers", &glext_MESA_resize_buffers, LoadExt_MESA_resize_buffers},
	{"GL_MESA_window_pos", &glext_MESA_window_pos, LoadExt_MESA_window_pos},
	{"GL_MESA_ycbcr_texture", &glext_MESA_ycbcr_texture, NULL},
	{"GL_NVX_conditional_render", &glext_NVX_conditional_render, LoadExt_NVX_conditional_render},
	{"GL_NV_bindless_texture", &glext_NV_bindless_texture, LoadExt_NV_bindless_texture},
	{"GL_NV_blend_square", &glext_NV_blend_square, NULL},
	{"GL_NV_compute_program5", &glext_NV_compute_program5, NULL},
	{"GL_NV_conditional_render", &glext_NV_conditional_render, LoadExt_NV_conditional_render},
	{"GL_NV_copy_depth_to_color", &glext_NV_copy_depth_to_color, NULL},
	{"GL_NV_copy_image", &glext_NV_copy_image, LoadExt_NV_copy_image},
	{"GL_NV_deep_texture3D", &glext_NV_deep_texture3D, NULL},
	{"GL_NV_depth_buffer_float", &glext_NV_depth_buffer_float, LoadExt_NV_depth_buffer_float},
	{"GL_NV_depth_clamp", &glext_NV_depth_clamp, NULL},
	{"GL_NV_draw_texture", &glext_NV_draw_texture, LoadExt_NV_draw_texture},
	{"GL_NV_evaluators", &glext_NV_evaluators, LoadExt_NV_evaluators},
	{"GL_NV_explicit_multisample", &glext_NV_explicit_multisample, LoadExt_NV_explicit_multisample},
	{"GL_NV_fence", &glext_NV_fence, LoadExt_NV_fence},
	{"GL_NV_float_buffer", &glext_NV_float_buffer, NULL},
	{"GL_NV_fog_distance", &glext_NV_fog_distance, NULL},
	{"GL_NV_fragment_program", &glext_NV_fragment_program, LoadExt_NV_fragment_program},
	{"GL_NV_fragment_program2", &glext_NV_fragment_program2, NULL},
	{"GL_NV_fragment_program4", &glext_NV_fragment_program4, NULL},
	{"GL_NV_fragment_program_option", &glext_NV_fragment_program_option, NULL},
	{"GL_NV_framebuffer_multisample_coverage", &glext_NV_framebuffer_multisample_coverage, LoadExt_NV_framebuffer_multisample_coverage},
	{"GL_NV_geometry_program4", &glext_NV_geometry_program4, LoadExt_NV_geometry_program4},
	{"GL_NV_geometry_shader4", &glext_NV_geometry_shader4, NULL},
	{"GL_NV_gpu_program4", &glext_NV_gpu_program4, LoadExt_NV_gpu_program4},
	{"GL_NV_gpu_program5", &glext_NV_gpu_program5, LoadExt_NV_gpu_program5},
	{"GL_NV_gpu_shader5", &glext_NV_gpu_shader5, LoadExt_NV_gpu_shader5},
	{"GL_NV_half_float", &glext_NV_half_float, LoadExt_NV_half_float},
	{"GL_NV_light_max_exponent", &glext_NV_light_max_exponent, NULL},
	{"GL_NV_multisample_coverage", &glext_NV_multisample_coverage, NULL},
	{"GL_NV_multisample_filter_hint", &glext_NV_multisample_filter_hint, NULL},
	{"GL_NV_occlusion_query", &glext_NV_occlusion_query, LoadExt_NV_occlusion_query},
	{"GL_NV_packed_depth_stencil", &glext_NV_packed_depth_stencil, NULL},
	{"GL_NV_parameter_buffer_object", &glext_NV_parameter_buffer_object, LoadExt_NV_parameter_buffer_object},
	{"GL_NV_parameter_buffer_object2", &glext_NV_parameter_buffer_object2, NULL},
	{"GL_NV_path_rendering", &glext_NV_path_rendering, LoadExt_NV_path_rendering},
	{"GL_NV_pixel_data_range", &glext_NV_pixel_data_range, LoadExt_NV_pixel_data_range},
	{"GL_NV_point_sprite", &glext_NV_point_sprite, LoadExt_NV_point_sprite},
	{"GL_NV_present_video", &glext_NV_present_video, LoadExt_NV_present_video},
	{"GL_NV_primitive_restart", &glext_NV_primitive_restart, LoadExt_NV_primitive_restart},
	{"GL_NV_register_combiners", &glext_NV_register_combiners, LoadExt_NV_register_combiners},
	{"GL_NV_register_combiners2", &glext_NV_register_combiners2, LoadExt_NV_register_combiners2},
	{"GL_NV_shader_atomic_counters", &glext_NV_shader_atomic_counters, NULL},
	{"GL_NV_shader_atomic_float", &glext_NV_shader_atomic_float, NULL},
	{"GL_NV_shader_buffer_load", &glext_NV_shader_buffer_load, LoadExt_NV_shader_buffer_load},
	{"GL_NV_shader_buffer_store", &glext_NV_shader_buffer_store, NULL},
	{"GL_NV_shader_storage_buffer_object", &glext_NV_shader_storage_buffer_object, NULL},
	{"GL_NV_tessellation_program5", &glext_NV_tessellation_program5, NULL},
	{"GL_NV_texgen_emboss", &glext_NV_texgen_emboss, NULL},
	{"GL_NV_texgen_reflection", &glext_NV_texgen_reflection, NULL},
	{"GL_NV_texture_barrier", &glext_NV_texture_barrier, LoadExt_NV_texture_barrier},
	{"GL_NV_texture_compression_vtc", &glext_NV_texture_compression_vtc, NULL},
	{"GL_NV_texture_env_combine4", &glext_NV_texture_env_combine4, NULL},
	{"GL_NV_texture_expand_normal", &glext_NV_texture_expand_normal, NULL},
	{"GL_NV_texture_multisample", &glext_NV_texture_multisample, LoadExt_NV_texture_multisample},
	{"GL_NV_texture_rectangle", &glext_NV_texture_rectangle, NULL},
	{"GL_NV_texture_shader", &glext_NV_texture_shader, NULL},
	{"GL_NV_texture_shader2", &glext_NV_texture_shader2, NULL},
	{"GL_NV_texture_shader3", &glext_NV_texture_shader3, NULL},
	{"GL_NV_transform_feedback", &glext_NV_transform_feedback, LoadExt_NV_transform_feedback},
	{"GL_NV_transform_feedback2", &glext_NV_transform_feedback2, LoadExt_NV_transform_feedback2},
	{"GL_NV_vdpau_interop", &glext_NV_vdpau_interop, LoadExt_NV_vdpau_interop},
	{"GL_NV_vertex_array_range", &glext_NV_vertex_array_range, LoadExt_NV_vertex_array_range},
	{"GL_NV_vertex_array_range2", &glext_NV_vertex_array_range2, NULL},
	{"GL_NV_vertex_attrib_integer_64bit", &glext_NV_vertex_attrib_integer_64bit, LoadExt_NV_vertex_attrib_integer_64bit},
	{"GL_NV_vertex_buffer_unified_memory", &glext_NV_vertex_buffer_unified_memory, LoadExt_NV_vertex_buffer_unified_memory},
	{"GL_NV_vertex_program", &glext_NV_vertex_program, LoadExt_NV_vertex_program},
	{"GL_NV_vertex_program1_1", &glext_NV_vertex_program1_1, NULL},
	{"GL_NV_vertex_program2", &glext_NV_vertex_program2, NULL},
	{"GL_NV_vertex_program2_option", &glext_NV_vertex_program2_option, NULL},
	{"GL_NV_vertex_program3", &glext_NV_vertex_program3, NULL},
	{"GL_NV_vertex_program4", &glext_NV_vertex_program4, LoadExt_NV_vertex_program4},
	{"GL_NV_video_capture", &glext_NV_video_capture, LoadExt_NV_video_capture},
	{"GL_OES_byte_coordinates", &glext_OES_byte_coordinates, LoadExt_OES_byte_coordinates},
	{"GL_OES_compressed_paletted_texture", &glext_OES_compressed_paletted_texture, NULL},
	{"GL_OES_fixed_point", &glext_OES_fixed_point, LoadExt_OES_fixed_point},
	{"GL_OES_query_matrix", &glext_OES_query_matrix, LoadExt_OES_query_matrix},
	{"GL_OES_read_format", &glext_OES_read_format, NULL},
	{"GL_OES_single_precision", &glext_OES_single_precision, LoadExt_OES_single_precision},
	{"GL_OML_interlace", &glext_OML_interlace, NULL},
	{"GL_OML_resample", &glext_OML_resample, NULL},
	{"GL_OML_subsample", &glext_OML_subsample, NULL},
	{"GL_PGI_misc_hints", &glext_PGI_misc_hints, LoadExt_PGI_misc_hints},
	{"GL_PGI_vertex_hints", &glext_PGI_vertex_hints, NULL},
	{"GL_REND_screen_coordinates", &glext_REND_screen_coordinates, NULL},
	{"GL_S3_s3tc", &glext_S3_s3tc, NULL},
	{"GL_SGIS_detail_texture", &glext_SGIS_detail_texture, LoadExt_SGIS_detail_texture},
	{"GL_SGIS_fog_function", &glext_SGIS_fog_function, LoadExt_SGIS_fog_function},
	{"GL_SGIS_generate_mipmap", &glext_SGIS_generate_mipmap, NULL},
	{"GL_SGIS_multisample", &glext_SGIS_multisample, LoadExt_SGIS_multisample},
	{"GL_SGIS_pixel_texture", &glext_SGIS_pixel_texture, LoadExt_SGIS_pixel_texture},
	{"GL_SGIS_point_line_texgen", &glext_SGIS_point_line_texgen, NULL},
	{"GL_SGIS_point_parameters", &glext_SGIS_point_parameters, LoadExt_SGIS_point_parameters},
	{"GL_SGIS_sharpen_texture", &glext_SGIS_sharpen_texture, LoadExt_SGIS_sharpen_texture},
	{"GL_SGIS_texture4D", &glext_SGIS_texture4D, LoadExt_SGIS_texture4D},
	{"GL_SGIS_texture_border_clamp", &glext_SGIS_texture_border_clamp, NULL},
	{"GL_SGIS_texture_color_mask", &glext_SGIS_texture_color_mask, LoadExt_SGIS_texture_color_mask},
	{"GL_SGIS_texture_edge_clamp", &glext_SGIS_texture_edge_clamp, NULL},
	{"GL_SGIS_texture_filter4", &glext_SGIS_texture_filter4, LoadExt_SGIS_texture_filter4},
	{"GL_SGIS_texture_lod", &glext_SGIS_texture_lod, NULL},
	{"GL_SGIS_texture_select", &glext_SGIS_texture_select, NULL},
	{"GL_SGIX_async", &glext_SGIX_async, LoadExt_SGIX_async},
	{"GL_SGIX_async_histogram", &glext_SGIX_async_histogram, NULL},
	{"GL_SGIX_async_pixel", &glext_SGIX_async_pixel, NULL},
	{"GL_SGIX_blend_alpha_minmax", &glext_SGIX_blend_alpha_minmax, NULL},
	{"GL_SGIX_calligraphic_fragment", &glext_SGIX_calligraphic_fragment, NULL},
	{"GL_SGIX_clipmap", &glext_SGIX_clipmap, NULL},
	{"GL_SGIX_convolution_accuracy", &glext_SGIX_convolution_accuracy, NULL},
	{"GL_SGIX_depth_texture", &glext_SGIX_depth_texture, NULL},
	{"GL_SGIX_flush_raster", &glext_SGIX_flush_raster, LoadExt_SGIX_flush_raster},
	{"GL_SGIX_fog_offset", &glext_SGIX_fog_offset, NULL},
	{"GL_SGIX_fragment_lighting", &glext_SGIX_fragment_lighting, LoadExt_SGIX_fragment_lighting},
	{"GL_SGIX_framezoom", &glext_SGIX_framezoom, LoadExt_SGIX_framezoom},
	{"GL_SGIX_instruments", &glext_SGIX_instruments, LoadExt_SGIX_instruments},
	{"GL_SGIX_interlace", &glext_SGIX_interlace, NULL},
	{"GL_SGIX_ir_instrument1", &glext_SGIX_ir_instrument1, NULL},
	{"GL_SGIX_list_priority", &glext_SGIX_list_priority, LoadExt_SGIX_list_priority},
	{"GL_SGIX_pixel_texture", &glext_SGIX_pixel_texture, LoadExt_SGIX_pixel_texture},
	{"GL_SGIX_pixel_tiles", &glext_SGIX_pixel_tiles, NULL},
	{"GL_SGIX_polynomial_ffd", &glext_SGIX_polynomial_ffd, LoadExt_SGIX_polynomial_ffd},
	{"GL_SGIX_reference_plane", &glext_SGIX_reference_plane, LoadExt_SGIX_reference_plane},
	{"GL_SGIX_resample", &glext_SGIX_resample, NULL},
	{"GL_SGIX_scalebias_hint", &glext_SGIX_scalebias_hint, NULL},
	{"GL_SGIX_shadow", &glext_SGIX_shadow, NULL},
	{"GL_SGIX_shadow_ambient", &glext_SGIX_shadow_ambient, NULL},
	{"GL_SGIX_sprite", &glext_SGIX_sprite, LoadExt_SGIX_sprite},
	{"GL_SGIX_subsample", &glext_SGIX_subsample, NULL},
	{"GL_SGIX_tag_sample_buffer", &glext_SGIX_tag_sample_buffer, LoadExt_SGIX_tag_sample_buffer},
	{"GL_SGIX_texture_add_env", &glext_SGIX_texture_add_env, NULL},
	{"GL_SGIX_texture_coordinate_clamp", &glext_SGIX_texture_coordinate_clamp, NULL},
	{"GL_SGIX_texture_lod_bias", &glext_SGIX_texture_lod_bias, NULL},
	{"GL_SGIX_texture_multi_buffer", &glext_SGIX_texture_multi_buffer, NULL},
	{"GL_SGIX_texture_scale_bias", &glext_SGIX_texture_scale_bias, NULL},
	{"GL_SGIX_vertex_preclip", &glext_SGIX_vertex_preclip, NULL},
	{"GL_SGIX_ycrcb", &glext_SGIX_ycrcb, NULL},
	{"GL_SGIX_ycrcb_subsample", &glext_SGIX_ycrcb_subsample, NULL},
	{"GL_SGIX_ycrcba", &glext_SGIX_ycrcba, NULL},
	{"GL_SGI_color_matrix", &glext_SGI_color_matrix, NULL},
	{"GL_SGI_color_table", &glext_SGI_color_table, LoadExt_SGI_color_table},
	{"GL_SGI_texture_color_table", &glext_SGI_texture_color_table, NULL},
	{"GL_SUNX_constant_data", &glext_SUNX_constant_data, LoadExt_SUNX_constant_data},
	{"GL_SUN_convolution_border_modes", &glext_SUN_convolution_border_modes, NULL},
	{"GL_SUN_global_alpha", &glext_SUN_global_alpha, LoadExt_SUN_global_alpha},
	{"GL_SUN_mesh_array", &glext_SUN_mesh_array, LoadExt_SUN_mesh_array},
	{"GL_SUN_slice_accum", &glext_SUN_slice_accum, NULL},
	{"GL_SUN_triangle_list", &glext_SUN_triangle_list, LoadExt_SUN_triangle_list},
	{"GL_SUN_vertex", &glext_SUN_vertex, LoadExt_SUN_vertex},
	{"GL_WIN_phong_shading", &glext_WIN_phong_shading, NULL},
	{"GL_WIN_specular_fog", &glext_WIN_specular_fog, NULL},
};

static int g_extensionMapSize = 448;

static ogl_StrToExtMap *FindExtEntry(const char *extensionName)
{
	int loop;
	ogl_StrToExtMap *currLoc = ExtensionTable;
	for(loop = 0; loop < g_extensionMapSize; ++loop, ++currLoc)
	{
		if(strcmp(extensionName, currLoc->extensionName) == 0)
			return currLoc;
	}
	
	return NULL;
}

static void ClearExtensionVars()
{
	glext_ARB_imaging = 0;
	glext_ARB_vertex_array_object = 0;
	glext_ARB_texture_rg = 0;
	glext_ARB_texture_compression_rgtc = 0;
	glext_ARB_map_buffer_range = 0;
	glext_ARB_half_float_vertex = 0;
	glext_ARB_framebuffer_sRGB = 0;
	glext_ARB_framebuffer_object = 0;
	glext_ARB_depth_buffer_float = 0;
	glext_ARB_uniform_buffer_object = 0;
	glext_ARB_copy_buffer = 0;
	glext_ARB_depth_clamp = 0;
	glext_ARB_draw_elements_base_vertex = 0;
	glext_ARB_fragment_coord_conventions = 0;
	glext_ARB_provoking_vertex = 0;
	glext_ARB_seamless_cube_map = 0;
	glext_ARB_sync = 0;
	glext_ARB_texture_multisample = 0;
	glext_ARB_vertex_array_bgra = 0;
	glext_ARB_texture_rgb10_a2ui = 0;
	glext_ARB_texture_swizzle = 0;
	glext_ARB_timer_query = 0;
	glext_ARB_vertex_type_2_10_10_10_rev = 0;
	glext_ARB_blend_func_extended = 0;
	glext_ARB_occlusion_query2 = 0;
	glext_ARB_sampler_objects = 0;
	glext_ARB_draw_indirect = 0;
	glext_ARB_gpu_shader5 = 0;
	glext_ARB_gpu_shader_fp64 = 0;
	glext_ARB_shader_subroutine = 0;
	glext_ARB_tessellation_shader = 0;
	glext_ARB_transform_feedback2 = 0;
	glext_ARB_transform_feedback3 = 0;
	glext_ARB_ES2_compatibility = 0;
	glext_ARB_get_program_binary = 0;
	glext_ARB_separate_shader_objects = 0;
	glext_ARB_vertex_attrib_64bit = 0;
	glext_ARB_viewport_array = 0;
	glext_ARB_base_instance = 0;
	glext_ARB_shading_language_420pack = 0;
	glext_ARB_transform_feedback_instanced = 0;
	glext_ARB_compressed_texture_pixel_storage = 0;
	glext_ARB_conservative_depth = 0;
	glext_ARB_internalformat_query = 0;
	glext_ARB_map_buffer_alignment = 0;
	glext_ARB_shader_atomic_counters = 0;
	glext_ARB_shader_image_load_store = 0;
	glext_ARB_shading_language_packing = 0;
	glext_ARB_texture_storage = 0;
	glext_KHR_debug = 0;
	glext_ARB_arrays_of_arrays = 0;
	glext_ARB_clear_buffer_object = 0;
	glext_ARB_compute_shader = 0;
	glext_ARB_copy_image = 0;
	glext_ARB_ES3_compatibility = 0;
	glext_ARB_explicit_uniform_location = 0;
	glext_ARB_fragment_layer_viewport = 0;
	glext_ARB_framebuffer_no_attachments = 0;
	glext_ARB_internalformat_query2 = 0;
	glext_ARB_invalidate_subdata = 0;
	glext_ARB_multi_draw_indirect = 0;
	glext_ARB_program_interface_query = 0;
	glext_ARB_shader_image_size = 0;
	glext_ARB_shader_storage_buffer_object = 0;
	glext_ARB_stencil_texturing = 0;
	glext_ARB_texture_buffer_range = 0;
	glext_ARB_texture_query_levels = 0;
	glext_ARB_texture_storage_multisample = 0;
	glext_ARB_texture_view = 0;
	glext_ARB_vertex_attrib_binding = 0;
	glext_3DFX_multisample = 0;
	glext_3DFX_tbuffer = 0;
	glext_3DFX_texture_compression_FXT1 = 0;
	glext_AMD_blend_minmax_factor = 0;
	glext_AMD_conservative_depth = 0;
	glext_AMD_debug_output = 0;
	glext_AMD_depth_clamp_separate = 0;
	glext_AMD_draw_buffers_blend = 0;
	glext_AMD_multi_draw_indirect = 0;
	glext_AMD_name_gen_delete = 0;
	glext_AMD_performance_monitor = 0;
	glext_AMD_pinned_memory = 0;
	glext_AMD_query_buffer_object = 0;
	glext_AMD_sample_positions = 0;
	glext_AMD_seamless_cubemap_per_texture = 0;
	glext_AMD_shader_stencil_export = 0;
	glext_AMD_shader_trinary_minmax = 0;
	glext_AMD_sparse_texture = 0;
	glext_AMD_stencil_operation_extended = 0;
	glext_AMD_texture_texture4 = 0;
	glext_AMD_transform_feedback3_lines_triangles = 0;
	glext_AMD_vertex_shader_layer = 0;
	glext_AMD_vertex_shader_tessellator = 0;
	glext_AMD_vertex_shader_viewport_index = 0;
	glext_APPLE_aux_depth_stencil = 0;
	glext_APPLE_client_storage = 0;
	glext_APPLE_element_array = 0;
	glext_APPLE_fence = 0;
	glext_APPLE_float_pixels = 0;
	glext_APPLE_flush_buffer_range = 0;
	glext_APPLE_object_purgeable = 0;
	glext_APPLE_rgb_422 = 0;
	glext_APPLE_row_bytes = 0;
	glext_APPLE_specular_vector = 0;
	glext_APPLE_texture_range = 0;
	glext_APPLE_transform_hint = 0;
	glext_APPLE_vertex_array_object = 0;
	glext_APPLE_vertex_array_range = 0;
	glext_APPLE_vertex_program_evaluators = 0;
	glext_APPLE_ycbcr_422 = 0;
	glext_ARB_cl_event = 0;
	glext_ARB_color_buffer_float = 0;
	glext_ARB_compatibility = 0;
	glext_ARB_debug_output = 0;
	glext_ARB_depth_texture = 0;
	glext_ARB_draw_buffers = 0;
	glext_ARB_draw_buffers_blend = 0;
	glext_ARB_draw_instanced = 0;
	glext_ARB_explicit_attrib_location = 0;
	glext_ARB_fragment_program = 0;
	glext_ARB_fragment_program_shadow = 0;
	glext_ARB_fragment_shader = 0;
	glext_ARB_geometry_shader4 = 0;
	glext_ARB_half_float_pixel = 0;
	glext_ARB_instanced_arrays = 0;
	glext_ARB_matrix_palette = 0;
	glext_ARB_multisample = 0;
	glext_ARB_multitexture = 0;
	glext_ARB_occlusion_query = 0;
	glext_ARB_pixel_buffer_object = 0;
	glext_ARB_point_parameters = 0;
	glext_ARB_point_sprite = 0;
	glext_ARB_robust_buffer_access_behavior = 0;
	glext_ARB_robustness = 0;
	glext_ARB_robustness_isolation = 0;
	glext_ARB_sample_shading = 0;
	glext_ARB_shader_bit_encoding = 0;
	glext_ARB_shader_objects = 0;
	glext_ARB_shader_precision = 0;
	glext_ARB_shader_stencil_export = 0;
	glext_ARB_shader_texture_lod = 0;
	glext_ARB_shading_language_100 = 0;
	glext_ARB_shading_language_include = 0;
	glext_ARB_shadow = 0;
	glext_ARB_shadow_ambient = 0;
	glext_ARB_texture_border_clamp = 0;
	glext_ARB_texture_buffer_object = 0;
	glext_ARB_texture_buffer_object_rgb32 = 0;
	glext_ARB_texture_compression = 0;
	glext_ARB_texture_compression_bptc = 0;
	glext_ARB_texture_cube_map = 0;
	glext_ARB_texture_cube_map_array = 0;
	glext_ARB_texture_env_add = 0;
	glext_ARB_texture_env_combine = 0;
	glext_ARB_texture_env_crossbar = 0;
	glext_ARB_texture_env_dot3 = 0;
	glext_ARB_texture_float = 0;
	glext_ARB_texture_gather = 0;
	glext_ARB_texture_mirrored_repeat = 0;
	glext_ARB_texture_non_power_of_two = 0;
	glext_ARB_texture_query_lod = 0;
	glext_ARB_texture_rectangle = 0;
	glext_ARB_transpose_matrix = 0;
	glext_ARB_vertex_blend = 0;
	glext_ARB_vertex_buffer_object = 0;
	glext_ARB_vertex_program = 0;
	glext_ARB_vertex_shader = 0;
	glext_ARB_window_pos = 0;
	glext_ATI_draw_buffers = 0;
	glext_ATI_element_array = 0;
	glext_ATI_envmap_bumpmap = 0;
	glext_ATI_fragment_shader = 0;
	glext_ATI_map_object_buffer = 0;
	glext_ATI_meminfo = 0;
	glext_ATI_pixel_format_float = 0;
	glext_ATI_pn_triangles = 0;
	glext_ATI_separate_stencil = 0;
	glext_ATI_text_fragment_shader = 0;
	glext_ATI_texture_env_combine3 = 0;
	glext_ATI_texture_float = 0;
	glext_ATI_texture_mirror_once = 0;
	glext_ATI_vertex_array_object = 0;
	glext_ATI_vertex_attrib_array_object = 0;
	glext_ATI_vertex_streams = 0;
	glext_EXT_422_pixels = 0;
	glext_EXT_abgr = 0;
	glext_EXT_bgra = 0;
	glext_EXT_bindable_uniform = 0;
	glext_EXT_blend_color = 0;
	glext_EXT_blend_equation_separate = 0;
	glext_EXT_blend_func_separate = 0;
	glext_EXT_blend_logic_op = 0;
	glext_EXT_blend_minmax = 0;
	glext_EXT_blend_subtract = 0;
	glext_EXT_clip_volume_hint = 0;
	glext_EXT_cmyka = 0;
	glext_EXT_color_subtable = 0;
	glext_EXT_compiled_vertex_array = 0;
	glext_EXT_convolution = 0;
	glext_EXT_coordinate_frame = 0;
	glext_EXT_copy_texture = 0;
	glext_EXT_cull_vertex = 0;
	glext_EXT_depth_bounds_test = 0;
	glext_EXT_direct_state_access = 0;
	glext_EXT_draw_buffers2 = 0;
	glext_EXT_draw_instanced = 0;
	glext_EXT_draw_range_elements = 0;
	glext_EXT_fog_coord = 0;
	glext_EXT_framebuffer_blit = 0;
	glext_EXT_framebuffer_multisample = 0;
	glext_EXT_framebuffer_multisample_blit_scaled = 0;
	glext_EXT_framebuffer_object = 0;
	glext_EXT_framebuffer_sRGB = 0;
	glext_EXT_geometry_shader4 = 0;
	glext_EXT_gpu_program_parameters = 0;
	glext_EXT_gpu_shader4 = 0;
	glext_EXT_histogram = 0;
	glext_EXT_index_array_formats = 0;
	glext_EXT_index_func = 0;
	glext_EXT_index_material = 0;
	glext_EXT_index_texture = 0;
	glext_EXT_light_texture = 0;
	glext_EXT_misc_attribute = 0;
	glext_EXT_multi_draw_arrays = 0;
	glext_EXT_multisample = 0;
	glext_EXT_packed_depth_stencil = 0;
	glext_EXT_packed_float = 0;
	glext_EXT_packed_pixels = 0;
	glext_EXT_paletted_texture = 0;
	glext_EXT_pixel_buffer_object = 0;
	glext_EXT_pixel_transform = 0;
	glext_EXT_pixel_transform_color_table = 0;
	glext_EXT_point_parameters = 0;
	glext_EXT_polygon_offset = 0;
	glext_EXT_provoking_vertex = 0;
	glext_EXT_rescale_normal = 0;
	glext_EXT_secondary_color = 0;
	glext_EXT_separate_shader_objects = 0;
	glext_EXT_separate_specular_color = 0;
	glext_EXT_shader_image_load_store = 0;
	glext_EXT_shadow_funcs = 0;
	glext_EXT_shared_texture_palette = 0;
	glext_EXT_stencil_clear_tag = 0;
	glext_EXT_stencil_two_side = 0;
	glext_EXT_stencil_wrap = 0;
	glext_EXT_subtexture = 0;
	glext_EXT_texture = 0;
	glext_EXT_texture3D = 0;
	glext_EXT_texture_array = 0;
	glext_EXT_texture_buffer_object = 0;
	glext_EXT_texture_compression_latc = 0;
	glext_EXT_texture_compression_rgtc = 0;
	glext_EXT_texture_compression_s3tc = 0;
	glext_EXT_texture_cube_map = 0;
	glext_EXT_texture_env_add = 0;
	glext_EXT_texture_env_combine = 0;
	glext_EXT_texture_env_dot3 = 0;
	glext_EXT_texture_filter_anisotropic = 0;
	glext_EXT_texture_integer = 0;
	glext_EXT_texture_lod_bias = 0;
	glext_EXT_texture_mirror_clamp = 0;
	glext_EXT_texture_object = 0;
	glext_EXT_texture_perturb_normal = 0;
	glext_EXT_texture_sRGB = 0;
	glext_EXT_texture_sRGB_decode = 0;
	glext_EXT_texture_shared_exponent = 0;
	glext_EXT_texture_snorm = 0;
	glext_EXT_texture_swizzle = 0;
	glext_EXT_timer_query = 0;
	glext_EXT_transform_feedback = 0;
	glext_EXT_vertex_array = 0;
	glext_EXT_vertex_array_bgra = 0;
	glext_EXT_vertex_attrib_64bit = 0;
	glext_EXT_vertex_shader = 0;
	glext_EXT_vertex_weighting = 0;
	glext_EXT_x11_sync_object = 0;
	glext_GREMEDY_frame_terminator = 0;
	glext_GREMEDY_string_marker = 0;
	glext_HP_convolution_border_modes = 0;
	glext_HP_image_transform = 0;
	glext_HP_occlusion_test = 0;
	glext_HP_texture_lighting = 0;
	glext_IBM_cull_vertex = 0;
	glext_IBM_multimode_draw_arrays = 0;
	glext_IBM_rasterpos_clip = 0;
	glext_IBM_static_data = 0;
	glext_IBM_texture_mirrored_repeat = 0;
	glext_IBM_vertex_array_lists = 0;
	glext_INGR_color_clamp = 0;
	glext_INGR_interlace_read = 0;
	glext_INGR_palette_buffer = 0;
	glext_INTEL_map_texture = 0;
	glext_INTEL_parallel_arrays = 0;
	glext_INTEL_texture_scissor = 0;
	glext_KHR_texture_compression_astc_ldr = 0;
	glext_MESAX_texture_stack = 0;
	glext_MESA_pack_invert = 0;
	glext_MESA_resize_buffers = 0;
	glext_MESA_window_pos = 0;
	glext_MESA_ycbcr_texture = 0;
	glext_NVX_conditional_render = 0;
	glext_NV_bindless_texture = 0;
	glext_NV_blend_square = 0;
	glext_NV_compute_program5 = 0;
	glext_NV_conditional_render = 0;
	glext_NV_copy_depth_to_color = 0;
	glext_NV_copy_image = 0;
	glext_NV_deep_texture3D = 0;
	glext_NV_depth_buffer_float = 0;
	glext_NV_depth_clamp = 0;
	glext_NV_draw_texture = 0;
	glext_NV_evaluators = 0;
	glext_NV_explicit_multisample = 0;
	glext_NV_fence = 0;
	glext_NV_float_buffer = 0;
	glext_NV_fog_distance = 0;
	glext_NV_fragment_program = 0;
	glext_NV_fragment_program2 = 0;
	glext_NV_fragment_program4 = 0;
	glext_NV_fragment_program_option = 0;
	glext_NV_framebuffer_multisample_coverage = 0;
	glext_NV_geometry_program4 = 0;
	glext_NV_geometry_shader4 = 0;
	glext_NV_gpu_program4 = 0;
	glext_NV_gpu_program5 = 0;
	glext_NV_gpu_shader5 = 0;
	glext_NV_half_float = 0;
	glext_NV_light_max_exponent = 0;
	glext_NV_multisample_coverage = 0;
	glext_NV_multisample_filter_hint = 0;
	glext_NV_occlusion_query = 0;
	glext_NV_packed_depth_stencil = 0;
	glext_NV_parameter_buffer_object = 0;
	glext_NV_parameter_buffer_object2 = 0;
	glext_NV_path_rendering = 0;
	glext_NV_pixel_data_range = 0;
	glext_NV_point_sprite = 0;
	glext_NV_present_video = 0;
	glext_NV_primitive_restart = 0;
	glext_NV_register_combiners = 0;
	glext_NV_register_combiners2 = 0;
	glext_NV_shader_atomic_counters = 0;
	glext_NV_shader_atomic_float = 0;
	glext_NV_shader_buffer_load = 0;
	glext_NV_shader_buffer_store = 0;
	glext_NV_shader_storage_buffer_object = 0;
	glext_NV_tessellation_program5 = 0;
	glext_NV_texgen_emboss = 0;
	glext_NV_texgen_reflection = 0;
	glext_NV_texture_barrier = 0;
	glext_NV_texture_compression_vtc = 0;
	glext_NV_texture_env_combine4 = 0;
	glext_NV_texture_expand_normal = 0;
	glext_NV_texture_multisample = 0;
	glext_NV_texture_rectangle = 0;
	glext_NV_texture_shader = 0;
	glext_NV_texture_shader2 = 0;
	glext_NV_texture_shader3 = 0;
	glext_NV_transform_feedback = 0;
	glext_NV_transform_feedback2 = 0;
	glext_NV_vdpau_interop = 0;
	glext_NV_vertex_array_range = 0;
	glext_NV_vertex_array_range2 = 0;
	glext_NV_vertex_attrib_integer_64bit = 0;
	glext_NV_vertex_buffer_unified_memory = 0;
	glext_NV_vertex_program = 0;
	glext_NV_vertex_program1_1 = 0;
	glext_NV_vertex_program2 = 0;
	glext_NV_vertex_program2_option = 0;
	glext_NV_vertex_program3 = 0;
	glext_NV_vertex_program4 = 0;
	glext_NV_video_capture = 0;
	glext_OES_byte_coordinates = 0;
	glext_OES_compressed_paletted_texture = 0;
	glext_OES_fixed_point = 0;
	glext_OES_query_matrix = 0;
	glext_OES_read_format = 0;
	glext_OES_single_precision = 0;
	glext_OML_interlace = 0;
	glext_OML_resample = 0;
	glext_OML_subsample = 0;
	glext_PGI_misc_hints = 0;
	glext_PGI_vertex_hints = 0;
	glext_REND_screen_coordinates = 0;
	glext_S3_s3tc = 0;
	glext_SGIS_detail_texture = 0;
	glext_SGIS_fog_function = 0;
	glext_SGIS_generate_mipmap = 0;
	glext_SGIS_multisample = 0;
	glext_SGIS_pixel_texture = 0;
	glext_SGIS_point_line_texgen = 0;
	glext_SGIS_point_parameters = 0;
	glext_SGIS_sharpen_texture = 0;
	glext_SGIS_texture4D = 0;
	glext_SGIS_texture_border_clamp = 0;
	glext_SGIS_texture_color_mask = 0;
	glext_SGIS_texture_edge_clamp = 0;
	glext_SGIS_texture_filter4 = 0;
	glext_SGIS_texture_lod = 0;
	glext_SGIS_texture_select = 0;
	glext_SGIX_async = 0;
	glext_SGIX_async_histogram = 0;
	glext_SGIX_async_pixel = 0;
	glext_SGIX_blend_alpha_minmax = 0;
	glext_SGIX_calligraphic_fragment = 0;
	glext_SGIX_clipmap = 0;
	glext_SGIX_convolution_accuracy = 0;
	glext_SGIX_depth_texture = 0;
	glext_SGIX_flush_raster = 0;
	glext_SGIX_fog_offset = 0;
	glext_SGIX_fragment_lighting = 0;
	glext_SGIX_framezoom = 0;
	glext_SGIX_instruments = 0;
	glext_SGIX_interlace = 0;
	glext_SGIX_ir_instrument1 = 0;
	glext_SGIX_list_priority = 0;
	glext_SGIX_pixel_texture = 0;
	glext_SGIX_pixel_tiles = 0;
	glext_SGIX_polynomial_ffd = 0;
	glext_SGIX_reference_plane = 0;
	glext_SGIX_resample = 0;
	glext_SGIX_scalebias_hint = 0;
	glext_SGIX_shadow = 0;
	glext_SGIX_shadow_ambient = 0;
	glext_SGIX_sprite = 0;
	glext_SGIX_subsample = 0;
	glext_SGIX_tag_sample_buffer = 0;
	glext_SGIX_texture_add_env = 0;
	glext_SGIX_texture_coordinate_clamp = 0;
	glext_SGIX_texture_lod_bias = 0;
	glext_SGIX_texture_multi_buffer = 0;
	glext_SGIX_texture_scale_bias = 0;
	glext_SGIX_vertex_preclip = 0;
	glext_SGIX_ycrcb = 0;
	glext_SGIX_ycrcb_subsample = 0;
	glext_SGIX_ycrcba = 0;
	glext_SGI_color_matrix = 0;
	glext_SGI_color_table = 0;
	glext_SGI_texture_color_table = 0;
	glext_SUNX_constant_data = 0;
	glext_SUN_convolution_border_modes = 0;
	glext_SUN_global_alpha = 0;
	glext_SUN_mesh_array = 0;
	glext_SUN_slice_accum = 0;
	glext_SUN_triangle_list = 0;
	glext_SUN_vertex = 0;
	glext_WIN_phong_shading = 0;
	glext_WIN_specular_fog = 0;
}


static void LoadExtByName(const char *extensionName)
{
	ogl_StrToExtMap *entry = NULL;
	entry = FindExtEntry(extensionName);
	if(entry)
	{
		if(entry->LoadExtension)
		{
			int numFailed = entry->LoadExtension();
			if(numFailed == 0)
			{
				*(entry->extensionVariable) = ogl_LOAD_SUCCEEDED;
			}
			else
			{
				*(entry->extensionVariable) = ogl_LOAD_SUCCEEDED + numFailed;
			}
		}
		else
		{
			*(entry->extensionVariable) = ogl_LOAD_SUCCEEDED;
		}
	}
}

typedef struct ogl_VersProfToLoaderMap_s
{
	int majorVersion;
	int minorVersion;
	int compatibilityProfile;
	PFN_LOADFUNCPOINTERS LoadVersion;
} VersionMapEntry;

static VersionMapEntry g_versionMapTable[] =
{
	{1, 1, 0, LoadVersion_1_1},
	{1, 2, 0, LoadVersion_1_2},
	{1, 3, 0, LoadVersion_1_3},
	{1, 4, 0, LoadVersion_1_4},
	{1, 5, 0, LoadVersion_1_5},
	{2, 0, 0, LoadVersion_2_0},
	{2, 1, 0, LoadVersion_2_1},
	{3, 0, 0, LoadVersion_3_0},
	{3, 1, 0, LoadVersion_3_1},
	{3, 1, 1, LoadVersion_3_1_Comp},
	{3, 2, 0, LoadVersion_3_2},
	{3, 2, 1, LoadVersion_3_2_Comp},
	{3, 3, 0, LoadVersion_3_3},
	{3, 3, 1, LoadVersion_3_3_Comp},
	{4, 0, 0, LoadVersion_4_0},
	{4, 0, 1, LoadVersion_4_0_Comp},
	{4, 1, 0, LoadVersion_4_1},
	{4, 1, 1, LoadVersion_4_1_Comp},
	{4, 2, 0, LoadVersion_4_2},
	{4, 2, 1, LoadVersion_4_2_Comp},
	{4, 3, 0, LoadVersion_4_3},
	{4, 3, 1, LoadVersion_4_3_Comp},
};

static int g_numVersionMapEntries = 22;

static int LoadVersionFromMap(int major, int minor, int compatibilityProfile)
{
	int loop = 0;
	for(; loop < g_numVersionMapEntries; ++loop)
	{
		if(
			(g_versionMapTable[loop].majorVersion == major) &&
			(g_versionMapTable[loop].minorVersion == minor) &&
			(g_versionMapTable[loop].compatibilityProfile == compatibilityProfile))
		{
			return g_versionMapTable[loop].LoadVersion();
		}
	}
	
	return -1;
}

static void ParseVersionFromString(int *pOutMajor, int *pOutMinor, const char *strVersion)
{
	const char *strDotPos = NULL;
	int iLength = 0;
	char strWorkBuff[10];
	*pOutMinor = 0;
	*pOutMajor = 0;

	strDotPos = strchr(strVersion, '.');
	if(!strDotPos)
		return;

	iLength = (int)((ptrdiff_t)strDotPos - (ptrdiff_t)strVersion);
	strncpy(strWorkBuff, strVersion, iLength);
	strWorkBuff[iLength] = '\0';

	*pOutMajor = atoi(strWorkBuff);
	strDotPos = strchr(strVersion + iLength + 1, ' ');
	if(!strDotPos)
	{
		/*No extra data. Take the whole rest of the string.*/
		strcpy(strWorkBuff, strVersion + iLength + 1);
	}
	else
	{
		/*Copy only up until the space.*/
		int iLengthMinor = (int)((ptrdiff_t)strDotPos - (ptrdiff_t)strVersion);
		iLengthMinor = iLengthMinor - (iLength + 1);
		strncpy(strWorkBuff, strVersion + iLength + 1, iLengthMinor);
		strWorkBuff[iLengthMinor] = '\0';
	}

	*pOutMinor = atoi(strWorkBuff);
}

static void ProcExtsFromExtList()
{
	GLint iLoop;
	GLint iNumExtensions = 0;
	_funcptr_glGetIntegerv(GL_NUM_EXTENSIONS, &iNumExtensions);

	for(iLoop = 0; iLoop < iNumExtensions; iLoop++)
	{
		const char *strExtensionName = (const char *)_funcptr_glGetStringi(GL_EXTENSIONS, iLoop);
		LoadExtByName(strExtensionName);
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

void ogl_CopyFromC();

static int g_majorVersion = 0;
static int g_minorVersion = 0;

static int InternalLoad()
{
	int numFailed = 0;
	int compProfile = 0;
	
	g_majorVersion = 0;
	g_minorVersion = 0;
	
	ClearExtensionVars();
	
	_funcptr_glGetString = (PFNGLGETSTRINGPROC)IntGetProcAddress("glGetString");
	if(!_funcptr_glGetString) return 0;
	
	ParseVersionFromString(&g_majorVersion, &g_minorVersion, (const char*)_funcptr_glGetString(GL_VERSION));
	
	if(g_majorVersion < 3)
	{
		ProcExtsFromExtString((const char*)_funcptr_glGetString(GL_EXTENSIONS));
	}
	else
	{
		_funcptr_glGetIntegerv = (PFNGLGETINTEGERVPROC)IntGetProcAddress("glGetIntegerv");
		if(!_funcptr_glGetIntegerv) return 0;
		_funcptr_glGetStringi = (PFNGLGETSTRINGIPROC)IntGetProcAddress("glGetStringi");
		if(!_funcptr_glGetStringi) return 0;
		
		ProcExtsFromExtList();
	}
	
	if(g_majorVersion >= 3)
	{
		if(g_majorVersion == 3 && g_minorVersion == 0)
		{ /*Deliberately empty. Core/compatibility didn't exist til 3.1.*/
		}
		else if(g_majorVersion == 3 && g_minorVersion == 1)
		{
			if(glext_ARB_compatibility)
				compProfile = 1;
		}
		else
		{
			GLint iProfileMask = 0;
			glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &iProfileMask);
			
			if(!iProfileMask)
			{
				if(glext_ARB_compatibility)
					compProfile = 1;
			}
			else
			{
				if(iProfileMask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT)
					compProfile = 1;
			}
		}
	}
	
	numFailed = LoadVersionFromMap(g_majorVersion, g_minorVersion, compProfile);
	if(numFailed == -1) /*Couldn't find something to load.*/
	{
		/*Unable to find a compatible one. Load max version+compatibility.*/
		numFailed = LoadVersionFromMap(4, 3, 1);
		if(numFailed == -1) /*Couldn't even load it.*/
			return ogl_LOAD_FAILED;
	}
	
	return ogl_LOAD_SUCCEEDED + numFailed;
}

int ogl_LoadFunctions()
{
	int numFailed = 0;
	numFailed = InternalLoad();
	ogl_CopyFromC();
	return numFailed;
}

int ogl_GetMajorVersion() { return g_majorVersion; }
int ogl_GetMinorVersion() { return g_minorVersion; }

int ogl_IsVersionGEQ( int testMajorVersion, int testMinorVersion )
{
	if(g_majorVersion > testMajorVersion) return 1;
	if(g_majorVersion < testMajorVersion) return 0;
	if(g_minorVersion >= testMinorVersion) return 1;
	return 0;
}
