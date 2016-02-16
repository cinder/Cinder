/*
 * Vulkan Samples Kit
 *
 * Copyright (C) 2015 Valve Corporation
 * Copyright (C) 2015 Google, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef UTIL_INIT
#define UTIL_INIT

#include "util.hpp"

// Make sure functions start with init, execute, or destroy to assist codegen

VkResult init_global_extension_properties(
        layer_properties &layer_props);

VkResult init_global_layer_properties(sample_info &info);

VkResult init_device_extension_properties(
        struct sample_info &info,
        layer_properties &layer_props);
VkResult init_device_layer_properties(struct sample_info &info);

void init_instance_extension_names(struct sample_info &info);
VkResult init_instance(struct sample_info &info, char const*const app_short_name);
void init_device_extension_names(struct sample_info &info);
VkResult init_device(struct sample_info &info);
VkResult init_enumerate_device(struct sample_info &info, uint32_t gpu_count = 1);
VkBool32 demo_check_layers(
        const std::vector<layer_properties> &layer_props,
        const std::vector<const char *> &layer_names);
void init_connection(struct sample_info &info);
void init_window(struct sample_info &info);
void init_presentable_image(struct sample_info &info);
void execute_queue_cmdbuf(struct sample_info &info, const VkCommandBuffer *cmd_bufs);
void execute_pre_present_barrier(struct sample_info &info);
void execute_present_image(struct sample_info &info);
void init_swapchain_extension(struct sample_info &info);
void init_command_pool(struct sample_info &info);
void init_command_buffer(struct sample_info &info);
void execute_begin_command_buffer(struct sample_info &info);
void execute_end_command_buffer(struct sample_info &info);
void execute_queue_command_buffer(struct sample_info &info);
void init_device_queue(struct sample_info &info);
void init_swap_chain(struct sample_info &info);
void init_depth_buffer(struct sample_info &info);
void init_uniform_buffer(struct sample_info &info);
void init_descriptor_and_pipeline_layouts(struct sample_info &info, bool use_texture);
void init_renderpass(struct sample_info &info, bool include_depth, bool clear = true);
void init_vertex_buffer(struct sample_info &info, const void *vertexData, uint32_t dataSize, uint32_t dataStride, bool use_texture);
void init_framebuffers(struct sample_info &info, bool include_depth);
void init_descriptor_pool(struct sample_info &info, bool use_texture);
void init_descriptor_set(struct sample_info &info, bool use_texture);
void init_shaders(struct sample_info &info, const char *vertShaderText, const char *fragShaderText);
void init_pipeline_cache(struct sample_info &info);
void init_pipeline(struct sample_info &info, VkBool32 include_depth, VkBool32 include_vi = true);
void init_sampler(struct sample_info &info, VkSampler &sampler);
void init_image(struct sample_info &info, texture_object &texObj, const char* textureName);
void init_texture(struct sample_info &info,  const char* textureName = nullptr);
void init_viewports(struct sample_info &info);
void init_scissors(struct sample_info &info);
void init_fence(struct sample_info &info, VkFence &fence);
void init_submit_info(struct sample_info &info, VkSubmitInfo &submit_info);
void init_present_info(struct sample_info &info, VkPresentInfoKHR &present);
void init_clear_color_and_depth(struct sample_info &info, VkClearValue* clear_values);
void init_render_pass_begin_info(struct sample_info &info, VkRenderPassBeginInfo &rp_begin);

VkResult init_debug_msg_callback(struct sample_info &info, PFN_vkDbgMsgCallback dbgFunc);
void destroy_debug_msg_callback(struct sample_info &info);
void destroy_pipeline(struct sample_info &info);
void destroy_pipeline_cache(struct sample_info &info);
void destroy_descriptor_pool(struct sample_info &info);
void destroy_vertex_buffer(struct sample_info &info);
void destroy_texture(struct sample_info &info);
void destroy_framebuffers(struct sample_info &info);
void destroy_shaders(struct sample_info &info);
void destroy_renderpass(struct sample_info &info);
void destroy_descriptor_and_pipeline_layouts(struct sample_info &info);
void destroy_uniform_buffer(struct sample_info &info);
void destroy_depth_buffer(struct sample_info &info);
void destroy_swap_chain(struct sample_info &info);
void destroy_command_buffer(struct sample_info &info);
void destroy_command_pool(struct sample_info &info);
void destroy_device(struct sample_info &info);
void destroy_instance(struct sample_info &info);
void destroy_window(struct sample_info &info);

#endif // UTIL_INIT

