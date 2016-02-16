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

/*
VULKAN_SAMPLE_DESCRIPTION
samples "init" utility functions
*/

#include <cstdlib>
#include <assert.h>
#include <string.h>
#include "util_init.hpp"
#include "cube_data.h"

#if defined( _WIN32 )
	#include "cinder/msw/CinderMsw.h"
#endif

using namespace std;

#if defined( _WIN32 )
	static const wchar_t *WINDOWED_WIN_CLASS_NAME = TEXT("CinderVulkanWinClass");
#endif

/*
 * TODO: function description here
 */
VkResult init_global_extension_properties(
        layer_properties &layer_props)
{
    VkExtensionProperties *instance_extensions;
    uint32_t instance_extension_count;
    VkResult res;
    char *layer_name = NULL;

    layer_name = layer_props.properties.layerName;

    do {
        res = vkEnumerateInstanceExtensionProperties(layer_name, &instance_extension_count, NULL);
        if (res)
            return res;

        if (instance_extension_count == 0) {
            return VK_SUCCESS;
        }

        layer_props.extensions.resize(instance_extension_count);
        instance_extensions = layer_props.extensions.data();
        res = vkEnumerateInstanceExtensionProperties(
                  layer_name,
                  &instance_extension_count,
                  instance_extensions);
    } while (res == VK_INCOMPLETE);

    return res;
}

/*
 * TODO: function description here
 */
VkResult init_global_layer_properties(struct sample_info &info)
{
    uint32_t instance_layer_count;
    VkLayerProperties *vk_props = NULL;
    VkResult res;

    /*
     * It's possible, though very rare, that the number of
     * instance layers could change. For example, installing something
     * could include new layers that the loader would pick up
     * between the initial query for the count and the
     * request for VkLayerProperties. The loader indicates that
     * by returning a VK_INCOMPLETE status and will update the
     * the count parameter.
     * The count parameter will be updated with the number of
     * entries loaded into the data pointer - in case the number
     * of layers went down or is smaller than the size given.
     */
    do {
        res = vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL);
        if (res)
            return res;

        if (instance_layer_count == 0) {
            return VK_SUCCESS;
        }

        vk_props = (VkLayerProperties *) realloc(vk_props, instance_layer_count * sizeof(VkLayerProperties));

        res = vkEnumerateInstanceLayerProperties(&instance_layer_count, vk_props);
    } while (res == VK_INCOMPLETE);

    /*
     * Now gather the extension list for each instance layer.
     */
    for (uint32_t i = 0; i < instance_layer_count; i++) {
        layer_properties layer_props;
        layer_props.properties = vk_props[i];
        res = init_global_extension_properties(layer_props);
        if (res)
            return res;
        info.instance_layer_properties.push_back(layer_props);
    }
    free(vk_props);

    return res;
}

VkResult init_device_extension_properties(
        struct sample_info &info,
        layer_properties &layer_props)
{
    VkExtensionProperties *device_extensions;
    uint32_t device_extension_count;
    VkResult res;
    char *layer_name = NULL;

    layer_name = layer_props.properties.layerName;

    do {
        res = vkEnumerateDeviceExtensionProperties(
                  info.gpus[0],
                  layer_name, &device_extension_count, NULL);
        if (res)
            return res;

        if (device_extension_count == 0) {
            return VK_SUCCESS;
        }

        layer_props.extensions.resize(device_extension_count);
        device_extensions = layer_props.extensions.data();
        res = vkEnumerateDeviceExtensionProperties(
                  info.gpus[0],
                  layer_name,
                  &device_extension_count,
                  device_extensions);
    } while (res == VK_INCOMPLETE);

    return res;
}

/*
 * TODO: function description here
 */
VkResult init_device_layer_properties(struct sample_info &info)
{
    uint32_t device_layer_count;
    VkLayerProperties *vk_props = NULL;
    VkResult res;

    /*
     * It's possible, though very rare, that the number of
     * instance layers could change. For example, installing something
     * could include new layers that the loader would pick up
     * between the initial query for the count and the
     * request for VkLayerProperties. The loader indicates that
     * by returning a VK_INCOMPLETE status and will update the
     * the count parameter.
     * The count parameter will be updated with the number of
     * entries loaded into the data pointer - in case the number
     * of layers went down or is smaller than the size given.
     */
    do {
        res = vkEnumerateDeviceLayerProperties(info.gpus[0], &device_layer_count, NULL);
        if (res)
            return res;

        if (device_layer_count == 0) {
            return VK_SUCCESS;
        }

        vk_props = (VkLayerProperties *) realloc(vk_props, device_layer_count * sizeof(VkLayerProperties));

        res = vkEnumerateDeviceLayerProperties(info.gpus[0], &device_layer_count, vk_props);
    } while (res == VK_INCOMPLETE);

    /*
     * Now gather the extension list for each device layer.
     */
    for (uint32_t i = 0; i < device_layer_count; i++) {
        layer_properties layer_props;
        layer_props.properties = vk_props[i];
        res = init_device_extension_properties(info, layer_props);
        if (res)
            return res;
        info.device_layer_properties.push_back(layer_props);
    }
    free(vk_props);

    return res;
}
/*
 * Return 1 (true) if all layer names specified in check_names
 * can be found in given layer properties.
 */
VkBool32 demo_check_layers(
        const std::vector<layer_properties> &layer_props,
        const std::vector<const char *> &layer_names)
{
    uint32_t check_count = layer_names.size();
    uint32_t layer_count = layer_props.size();
    for (uint32_t i = 0; i < check_count; i++) {
        VkBool32 found = 0;
        for (uint32_t j = 0; j < layer_count; j++) {
            if (!strcmp(layer_names[i], layer_props[j].properties.layerName)) {
                found = 1;
            }
        }
        if (!found) {
            std::cout << "Cannot find layer: " <<  layer_names[i] << std::endl;
            return 0;
        }
    }
    return 1;
}

void init_instance_extension_names(struct sample_info &info)
{
    info.instance_extension_names.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef _WIN32
    info.instance_extension_names.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
    info.instance_extension_names.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
}

VkResult init_instance(struct sample_info &info, char const*const app_short_name)
{
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = NULL;
    app_info.pApplicationName = app_short_name;
    app_info.applicationVersion = 1;
    app_info.pEngineName = app_short_name;
    app_info.engineVersion = 1;
    app_info.apiVersion = VK_API_VERSION;

    VkInstanceCreateInfo inst_info = {};
    inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    inst_info.pNext = NULL;
    inst_info.flags = 0;
    inst_info.pApplicationInfo = &app_info;
    inst_info.enabledLayerNameCount = info.instance_layer_names.size();
    inst_info.ppEnabledLayerNames = info.instance_layer_names.size() ? info.instance_layer_names.data() : NULL;
    inst_info.enabledExtensionNameCount = info.instance_extension_names.size();
    inst_info.ppEnabledExtensionNames = info.instance_extension_names.data();

    VkResult res = vkCreateInstance(&inst_info, NULL, &info.inst);
    assert(res == VK_SUCCESS);

    return res;
}

void init_device_extension_names(struct sample_info &info)
{
    info.device_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

VkResult init_device(struct sample_info &info)
{
    VkResult res;
    VkDeviceQueueCreateInfo queue_info = {};

    vkGetPhysicalDeviceQueueFamilyProperties(info.gpus[0], &info.queue_count, NULL);
    assert(info.queue_count >= 1);

    info.queue_props.resize(info.queue_count);
    vkGetPhysicalDeviceQueueFamilyProperties(info.gpus[0], &info.queue_count, info.queue_props.data());
    assert(info.queue_count >= 1);

    bool found = false;
    for (int i = 0; i < info.queue_count; i++)
    {
        if (info.queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queue_info.queueFamilyIndex = i;
            found = true;
            break;
        }
    }
    assert(found);
    assert(info.queue_count >= 1);

    /* This is as good a place as any to do this */
    vkGetPhysicalDeviceMemoryProperties(info.gpus[0], &info.memory_properties);
    vkGetPhysicalDeviceProperties(info.gpus[0], &info.gpu_props);

    float queue_priorities[1] = { 0.0 };
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info.pNext = NULL;
    queue_info.queueCount = 1;
    queue_info.pQueuePriorities = queue_priorities;

    VkDeviceCreateInfo device_info = {};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.pNext = NULL;
    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = &queue_info;
    device_info.enabledLayerNameCount = info.device_layer_names.size();
    device_info.ppEnabledLayerNames =
            device_info.enabledLayerNameCount ? info.device_layer_names.data() : NULL;
    device_info.enabledExtensionNameCount = info.device_extension_names.size();
    device_info.ppEnabledExtensionNames =
            device_info.enabledExtensionNameCount ? info.device_extension_names.data() : NULL;
    device_info.pEnabledFeatures = NULL;

    res = vkCreateDevice(info.gpus[0], &device_info, NULL, &info.device);
    assert(res == VK_SUCCESS);

    return res;
}

VkResult init_enumerate_device(struct sample_info &info, uint32_t gpu_count)
{
    uint32_t const U_ASSERT_ONLY req_count = gpu_count;
    VkResult res = vkEnumeratePhysicalDevices(info.inst, &gpu_count, NULL);
    assert(gpu_count);
    info.gpus.resize(gpu_count);
    res = vkEnumeratePhysicalDevices(info.inst, &gpu_count, info.gpus.data());
    assert(!res && gpu_count >= req_count);

    return res;
}

VkResult init_debug_msg_callback(struct sample_info &info, PFN_vkDbgMsgCallback dbgFunc)
{
    VkResult res;
    VkDbgMsgCallback msg_callback;

    info.dbgCreateMsgCallback = (PFN_vkDbgCreateMsgCallback) vkGetInstanceProcAddr(info.inst, "vkDbgCreateMsgCallback");
    if (!info.dbgCreateMsgCallback) {
        std::cout << "GetInstanceProcAddr: Unable to find vkDbgCreateMsgCallback function." << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    std::cout << "Got dbgCreateMsgCallback function\n";

    info.dbgDestroyMsgCallback = (PFN_vkDbgDestroyMsgCallback) vkGetInstanceProcAddr(info.inst, "vkDbgDestroyMsgCallback");
    if (!info.dbgDestroyMsgCallback) {
        std::cout << "GetInstanceProcAddr: Unable to find vkDbgDestroyMsgCallback function." << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    res = info.dbgCreateMsgCallback(
              info.inst,
              VK_DBG_REPORT_ERROR_BIT | VK_DBG_REPORT_WARN_BIT,
              dbgFunc, NULL,
              &msg_callback);
    switch (res) {
    case VK_SUCCESS:
        std::cout << "Successfully created message calback object\n";
        info.msg_callbacks.push_back(msg_callback);
        break;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        std::cout << "dbgCreateMsgCallback: out of host memory pointer\n" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
        break;
    default:
        std::cout << "dbgCreateMsgCallback: unknown failure\n" << std::endl;
        return VK_ERROR_INITIALIZATION_FAILED;
        break;
    }
    return res;
}

void destroy_debug_msg_callback(struct sample_info &info)
{
    while (info.msg_callbacks.size() > 0) {
        info.dbgDestroyMsgCallback(info.inst, info.msg_callbacks.back());
        info.msg_callbacks.pop_back();
    }
}

void init_connection(struct sample_info &info)
{
#ifndef _WIN32
    const xcb_setup_t *setup;
    xcb_screen_iterator_t iter;
    int scr;

    info.connection = xcb_connect(NULL, &scr);
    if (info.connection == NULL) {
        std::cout << "Cannot find a compatible Vulkan ICD.\n";
        exit(-1);
    }

    setup = xcb_get_setup(info.connection);
    iter = xcb_setup_roots_iterator(setup);
    while (scr-- > 0)
        xcb_screen_next(&iter);

    info.screen = iter.data;
#endif // _WIN32
}

#ifdef _WIN32
static void run(struct sample_info *info)
{
 /* Placeholder for samples that want to show dynamic content */
}

// MS-Windows event handling function:
LRESULT CALLBACK WndProc(HWND hWnd,
                         UINT uMsg,
                         WPARAM wParam,
                         LPARAM lParam)
{
    struct sample_info *info = reinterpret_cast<struct sample_info*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch(uMsg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        run(info);
        return 0;
    default:
        break;
    }
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void init_window(struct sample_info &info)
{
    WNDCLASSEX  win_class;
    assert(info.width > 0);
    assert(info.height > 0);

    info.connection = GetModuleHandle(NULL);
    sprintf(info.name, "Sample");

    // Initialize the window class structure:
    win_class.cbSize = sizeof(WNDCLASSEX);
    win_class.style = CS_HREDRAW | CS_VREDRAW;
    win_class.lpfnWndProc = WndProc;
    win_class.cbClsExtra = 0;
    win_class.cbWndExtra = 0;
    win_class.hInstance = info.connection; // hInstance
    win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    win_class.lpszMenuName = NULL;
    win_class.lpszClassName = WINDOWED_WIN_CLASS_NAME; //info.name;
    win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    // Register window class:
    if (!RegisterClassEx(&win_class)) {
        // It didn't work, so try to give a useful error:
        printf("Unexpected error trying to start the application!\n");
        fflush(stdout);
        exit(1);
    }
    // Create window with the registered class:
    RECT wr = { 0, 0, info.width, info.height };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	std::wstring wideTitle = cinder::msw::toWideString( info.name ); 	

    info.window = CreateWindowEx( 0,
                                  WINDOWED_WIN_CLASS_NAME,	//info.name,	// class name
                                  wideTitle.c_str(),		// info.name,	// app name
                                  WS_OVERLAPPEDWINDOW |			// window style
                                  WS_VISIBLE |
                                  WS_SYSMENU,
                                  100,100,						// x/y coords
                                  wr.right-wr.left,				// width
                                  wr.bottom-wr.top,				// height
                                  NULL,							// handle to parent
                                  NULL,							// handle to menu
                                  info.connection,				// hInstance
                                  NULL);						// no extra parameters
    if (!info.window) {
        // It didn't work, so try to give a useful error:
        printf("Cannot create a window in which to draw!\n");
        fflush(stdout);
        exit(1);
    }
    SetWindowLongPtr(info.window, GWLP_USERDATA, (LONG_PTR) &info);
}

void destroy_window(struct sample_info &info)
{
    DestroyWindow(info.window);
}
#else
void init_window(struct sample_info &info)
{
    assert(info.width > 0);
    assert(info.height > 0);

    uint32_t value_mask, value_list[32];

    info.window = xcb_generate_id(info.connection);

    value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    value_list[0] = info.screen->black_pixel;
    value_list[1] = XCB_EVENT_MASK_KEY_RELEASE |
                    XCB_EVENT_MASK_EXPOSURE;

    xcb_create_window(info.connection,
            XCB_COPY_FROM_PARENT,
            info.window, info.screen->root,
            0, 0, info.width, info.height, 0,
            XCB_WINDOW_CLASS_INPUT_OUTPUT,
            info.screen->root_visual,
            value_mask, value_list);

    /* Magic code that will send notification when window is destroyed */
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(info.connection, 1, 12,
                                                      "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(info.connection, cookie, 0);

    xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(info.connection, 0, 16, "WM_DELETE_WINDOW");
    info.atom_wm_delete_window = xcb_intern_atom_reply(info.connection, cookie2, 0);

    xcb_change_property(info.connection, XCB_PROP_MODE_REPLACE,
                        info.window, (*reply).atom, 4, 32, 1,
                        &(*info.atom_wm_delete_window).atom);
    free(reply);

    xcb_map_window(info.connection, info.window);

    // Force the x/y coordinates to 100,100 results are identical in consecutive runs
    const uint32_t coords[] = {100,  100};
    xcb_configure_window(info.connection, info.window,
                         XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);
    xcb_flush(info.connection);

    xcb_generic_event_t *e;
    while ((e = xcb_wait_for_event(info.connection))) {
       if  ((e->response_type & ~0x80) == XCB_EXPOSE)
           break;
    }

}

void destroy_window(struct sample_info &info)
{
    xcb_destroy_window(info.connection, info.window);
    xcb_disconnect(info.connection);
}

#endif // _WIN32

void init_depth_buffer(struct sample_info &info)
{
    VkResult U_ASSERT_ONLY res;
    bool U_ASSERT_ONLY pass;
    VkImageCreateInfo image_info = {};
    const VkFormat depth_format = VK_FORMAT_D16_UNORM;
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(info.gpus[0], depth_format, &props);
    if (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        image_info.tiling = VK_IMAGE_TILING_LINEAR;
    } else if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    } else {
        /* Try other depth formats? */
        std::cout << "VK_FORMAT_D16_UNORM Unsupported.\n";
        exit(-1);
    }

    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.pNext = NULL;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.format = depth_format;
    image_info.extent.width = info.width;
    image_info.extent.height = info.height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.samples = NUM_SAMPLES;
    image_info.queueFamilyIndexCount = 0;
    image_info.pQueueFamilyIndices = NULL;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    image_info.flags = 0;

    VkMemoryAllocateInfo mem_alloc = {};
    mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc.pNext = NULL;
    mem_alloc.allocationSize = 0;
    mem_alloc.memoryTypeIndex = 0;

    VkImageViewCreateInfo view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.pNext = NULL;
    view_info.image = VK_NULL_HANDLE;
    view_info.format = depth_format;
    view_info.components.r = VK_COMPONENT_SWIZZLE_R;
    view_info.components.g = VK_COMPONENT_SWIZZLE_G;
    view_info.components.b = VK_COMPONENT_SWIZZLE_B;
    view_info.components.a = VK_COMPONENT_SWIZZLE_A;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.flags = 0;

    VkMemoryRequirements mem_reqs;

    info.depth.format = depth_format;

    /* Create image */
    res = vkCreateImage(info.device, &image_info, NULL,
                        &info.depth.image);
    assert(res == VK_SUCCESS);

    vkGetImageMemoryRequirements(info.device,
                                       info.depth.image, &mem_reqs);

    mem_alloc.allocationSize = mem_reqs.size;
    /* Use the memory properties to determine the type of memory required */
    pass = memory_type_from_properties(info,
                                      mem_reqs.memoryTypeBits,
                                      0, /* No requirements */
                                      &mem_alloc.memoryTypeIndex);
    assert(pass);

    /* Allocate memory */
    res = vkAllocateMemory(info.device, &mem_alloc, NULL, &info.depth.mem);
    assert(res == VK_SUCCESS);

    /* Bind memory */
    res = vkBindImageMemory(info.device, info.depth.image,
                            info.depth.mem, 0);
    assert(res == VK_SUCCESS);

    /* Set the image layout to depth stencil optimal */
    set_image_layout(info, info.depth.image,
                          VK_IMAGE_ASPECT_DEPTH_BIT,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    /* Create image view */
   view_info.image = info.depth.image;
   res = vkCreateImageView(info.device, &view_info, NULL, &info.depth.view);
   assert(res == VK_SUCCESS);
}

void init_swapchain_extension(struct sample_info &info)
{
    /* DEPENDS on init_connection() and init_window() */

    VkResult U_ASSERT_ONLY res;

    GET_INSTANCE_PROC_ADDR(info.inst, GetPhysicalDeviceSurfaceSupportKHR);
    GET_INSTANCE_PROC_ADDR(info.inst, GetPhysicalDeviceSurfaceCapabilitiesKHR);
    GET_INSTANCE_PROC_ADDR(info.inst, GetPhysicalDeviceSurfaceFormatsKHR);
    GET_INSTANCE_PROC_ADDR(info.inst, GetPhysicalDeviceSurfacePresentModesKHR);

    GET_DEVICE_PROC_ADDR(info.device, CreateSwapchainKHR);
    GET_DEVICE_PROC_ADDR(info.device, DestroySwapchainKHR);
    GET_DEVICE_PROC_ADDR(info.device, GetSwapchainImagesKHR);
    GET_DEVICE_PROC_ADDR(info.device, AcquireNextImageKHR);
    GET_DEVICE_PROC_ADDR(info.device, QueuePresentKHR);

    // Construct the surface description:
#ifdef _WIN32
    res = vkCreateWin32SurfaceKHR(info.inst, info.connection, info.window,
                                  NULL, &info.surface);

#else  // _WIN32
    res = vkCreateXcbSurfaceKHR(info.inst, info.connection, info.window,
                                NULL, &info.surface);
#endif // _WIN32
    assert(res == VK_SUCCESS);

    // Iterate over each queue to learn whether it supports presenting:
    VkBool32* supportsPresent = (VkBool32 *)malloc(info.queue_count * sizeof(VkBool32));
    for (uint32_t i = 0; i < info.queue_count; i++) {
        info.fpGetPhysicalDeviceSurfaceSupportKHR(info.gpus[0], i,
                                                   info.surface,
                                                   &supportsPresent[i]);
    }

    // Search for a graphics queue and a present queue in the array of queue
    // families, try to find one that supports both
    uint32_t graphicsQueueNodeIndex = UINT32_MAX;
    uint32_t presentQueueNodeIndex  = UINT32_MAX;
    for (uint32_t i = 0; i < info.queue_count; i++) {
        if ((info.queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            if (graphicsQueueNodeIndex == UINT32_MAX) {
                graphicsQueueNodeIndex = i;
            }

            if (supportsPresent[i] == VK_TRUE) {
                graphicsQueueNodeIndex = i;
                presentQueueNodeIndex = i;
                break;
            }
        }
    }
    if (presentQueueNodeIndex == UINT32_MAX) {
        // If didn't find a queue that supports both graphics and present, then
        // find a separate present queue.
        for (uint32_t i = 0; i < info.queue_count; ++i) {
            if (supportsPresent[i] == VK_TRUE) {
                presentQueueNodeIndex = i;
                break;
            }
        }
    }
    free(supportsPresent);

    // Generate error if could not find both a graphics and a present queue
    if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX) {
        std::cout << "Could not find a graphics and a present queue\nCould not find a graphics and a present queue\n";
        exit(-1);
    }

    info.graphics_queue_family_index = graphicsQueueNodeIndex;

    // Get the list of VkFormats that are supported:
    uint32_t formatCount;
    res = info.fpGetPhysicalDeviceSurfaceFormatsKHR(info.gpus[0],
                                     info.surface,
                                     &formatCount, NULL);
    assert(res == VK_SUCCESS);
    VkSurfaceFormatKHR *surfFormats = (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));
    res = info.fpGetPhysicalDeviceSurfaceFormatsKHR(info.gpus[0],
                                     info.surface,
                                     &formatCount, surfFormats);
    assert(res == VK_SUCCESS);
    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
    // the surface has no preferred format.  Otherwise, at least one
    // supported format will be returned.
    if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        info.format = VK_FORMAT_B8G8R8A8_UNORM;
    }
    else
    {
        assert(formatCount >= 1);
        info.format = surfFormats[0].format;
    }
}

void init_presentable_image(struct sample_info &info)
{
    /* DEPENDS on init_swap_chain() */

    VkResult U_ASSERT_ONLY res;
    VkSemaphoreCreateInfo presentCompleteSemaphoreCreateInfo;
    presentCompleteSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    presentCompleteSemaphoreCreateInfo.pNext = NULL;
    presentCompleteSemaphoreCreateInfo.flags = 0;

    res = vkCreateSemaphore(info.device,
                            &presentCompleteSemaphoreCreateInfo,
                            NULL,
                            &info.presentCompleteSemaphore);
    assert(!res);

    // Get the index of the next available swapchain image:
    res = info.fpAcquireNextImageKHR(info.device, info.swap_chain,
                                      UINT64_MAX,
                                      info.presentCompleteSemaphore,
                                      NULL,
                                      &info.current_buffer);
    // TODO: Deal with the VK_SUBOPTIMAL_KHR and VK_ERROR_OUT_OF_DATE_KHR
    // return codes
    assert(!res);
}

void execute_queue_cmdbuf(struct sample_info &info, const VkCommandBuffer *cmd_bufs)
{
    VkResult U_ASSERT_ONLY res;
    VkFence nullFence = VK_NULL_HANDLE;

    VkSubmitInfo submit_info[1] = {};
    submit_info[0].pNext = NULL;
    submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info[0].waitSemaphoreCount = 1;
    submit_info[0].pWaitSemaphores = &info.presentCompleteSemaphore;
    submit_info[0].commandBufferCount = 1;
    submit_info[0].pCommandBuffers = cmd_bufs;
    submit_info[0].signalSemaphoreCount = 0;
    submit_info[0].pSignalSemaphores = NULL;

    /* Queue the command buffer for execution */
    res = vkQueueSubmit(info.queue, 1, submit_info, nullFence);
    assert(!res);

    res = vkQueueWaitIdle(info.queue);
    assert(!res);
}

void execute_pre_present_barrier(struct sample_info &info)
{
    /* DEPENDS on init_swap_chain() */
    /* Add mem barrier to change layout to present */

    VkImageMemoryBarrier prePresentBarrier = {};
    prePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    prePresentBarrier.pNext = NULL;
    prePresentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    prePresentBarrier.dstAccessMask = 0;
    prePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    prePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    prePresentBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    prePresentBarrier.subresourceRange.baseMipLevel = 0;
    prePresentBarrier.subresourceRange.levelCount = 1;
    prePresentBarrier.subresourceRange.baseArrayLayer = 0;
    prePresentBarrier.subresourceRange.layerCount = 1;
    prePresentBarrier.image = info.buffers[info.current_buffer].image;
    VkImageMemoryBarrier *pmemory_barrier = &prePresentBarrier;
    vkCmdPipelineBarrier(info.cmd, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         0, 1, (const void * const*)&pmemory_barrier);
}

void execute_present_image(struct sample_info &info)
{
    /* DEPENDS on init_presentable_image() and init_swap_chain()*/
    /* Present the image in the window */

    VkResult U_ASSERT_ONLY res;
    VkPresentInfoKHR present;
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pNext = NULL;
    present.swapchainCount = 1;
    present.pSwapchains = &info.swap_chain;
    present.pImageIndices = &info.current_buffer;
    present.pWaitSemaphores = NULL;
    present.waitSemaphoreCount = 0;
    present.pResults = NULL;

    res = info.fpQueuePresentKHR(info.queue, &present);
    // TODO: Deal with the VK_SUBOPTIMAL_WSI and VK_ERROR_OUT_OF_DATE_WSI
    // return codes
    assert(!res);
}

void init_swap_chain(struct sample_info &info)
{
    /* DEPENDS on info.cmd and info.queue initialized */

    VkResult U_ASSERT_ONLY res;
    VkSurfaceCapabilitiesKHR surfCapabilities;

    res = info.fpGetPhysicalDeviceSurfaceCapabilitiesKHR(info.gpus[0],
        info.surface,
        &surfCapabilities);
    assert(res == VK_SUCCESS);

    uint32_t presentModeCount;
    res = info.fpGetPhysicalDeviceSurfacePresentModesKHR(info.gpus[0],
        info.surface,
        &presentModeCount, NULL);
    assert(res == VK_SUCCESS);
    VkPresentModeKHR *presentModes =
        (VkPresentModeKHR *)malloc(presentModeCount * sizeof(VkPresentModeKHR));
    assert(presentModes);
    res = info.fpGetPhysicalDeviceSurfacePresentModesKHR(info.gpus[0],
        info.surface,
        &presentModeCount, presentModes);
    assert(res == VK_SUCCESS);

    VkExtent2D swapChainExtent;
    // width and height are either both -1, or both not -1.
    if (surfCapabilities.currentExtent.width == -1)
    {
        // If the surface size is undefined, the size is set to
        // the size of the images requested.
        swapChainExtent.width = info.width;
        swapChainExtent.height = info.height;
    }
    else
    {
        // If the surface size is defined, the swap chain size must match
        swapChainExtent = surfCapabilities.currentExtent;
    }

    // If mailbox mode is available, use it, as is the lowest-latency non-
    // tearing mode.  If not, try IMMEDIATE which will usually be available,
    // and is fastest (though it tears).  If not, fall back to FIFO which is
    // always available.
    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (size_t i = 0; i < presentModeCount; i++) {
        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
        if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) &&
            (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)) {
            swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }

    // Determine the number of VkImage's to use in the swap chain (we desire to
    // own only 1 image at a time, besides the images being displayed and
    // queued for display):
    uint32_t desiredNumberOfSwapChainImages = surfCapabilities.minImageCount + 1;
    if ((surfCapabilities.maxImageCount > 0) &&
        (desiredNumberOfSwapChainImages > surfCapabilities.maxImageCount))
    {
        // Application must settle for fewer images than desired:
        desiredNumberOfSwapChainImages = surfCapabilities.maxImageCount;
    }

    VkSurfaceTransformFlagBitsKHR preTransform;
    if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_NONE_BIT_KHR) {
        preTransform = VK_SURFACE_TRANSFORM_NONE_BIT_KHR;
    } else {
        preTransform = surfCapabilities.currentTransform;
    }

    VkSwapchainCreateInfoKHR swap_chain = {};
    swap_chain.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swap_chain.pNext = NULL;
    swap_chain.surface = info.surface;
    swap_chain.minImageCount = desiredNumberOfSwapChainImages;
    swap_chain.imageFormat = info.format;
    swap_chain.imageExtent.width = swapChainExtent.width;
    swap_chain.imageExtent.height = swapChainExtent.height;
    swap_chain.preTransform = preTransform;
    swap_chain.imageArrayLayers = 1;
    swap_chain.presentMode = swapchainPresentMode;
    swap_chain.oldSwapchain = VK_NULL_HANDLE;
    swap_chain.clipped = true;
    swap_chain.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swap_chain.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    swap_chain.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swap_chain.queueFamilyIndexCount = 0;
    swap_chain.pQueueFamilyIndices = NULL;

    res = info.fpCreateSwapchainKHR(info.device, &swap_chain, NULL, &info.swap_chain);
    assert(res == VK_SUCCESS);

    res = info.fpGetSwapchainImagesKHR(info.device, info.swap_chain,
                                      &info.swapchainImageCount, NULL);
    assert(res == VK_SUCCESS);

    VkImage* swapchainImages = (VkImage*)malloc(info.swapchainImageCount * sizeof(VkImage));
    assert(swapchainImages);
    res = info.fpGetSwapchainImagesKHR(info.device, info.swap_chain,
                                      &info.swapchainImageCount, swapchainImages);
    assert(res == VK_SUCCESS);

    for (uint32_t i = 0; i < info.swapchainImageCount; i++) {
        swap_chain_buffer sc_buffer;

        VkImageViewCreateInfo color_image_view = {};
        color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        color_image_view.pNext = NULL;
        color_image_view.format = info.format;
        color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;
        color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;
        color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;
        color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;
        color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        color_image_view.subresourceRange.baseMipLevel = 0;
        color_image_view.subresourceRange.levelCount = 1;
        color_image_view.subresourceRange.baseArrayLayer = 0;
        color_image_view.subresourceRange.layerCount = 1;
        color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
        color_image_view.flags = 0;


        sc_buffer.image = swapchainImages[i];

        set_image_layout(info, sc_buffer.image,
                               VK_IMAGE_ASPECT_COLOR_BIT,
                               VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        color_image_view.image = sc_buffer.image;

        res = vkCreateImageView(info.device,
                &color_image_view, NULL, &sc_buffer.view);
        info.buffers.push_back(sc_buffer);
        assert(res == VK_SUCCESS);
    }
    info.current_buffer = 0;
}

void init_uniform_buffer(struct sample_info &info)
{
    VkResult U_ASSERT_ONLY res;
    bool U_ASSERT_ONLY pass;
    info.Projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
    info.View       = glm::lookAt(
                          glm::vec3(0,3,10), // Camera is at (0,3,10), in World Space
                          glm::vec3(0,0,0), // and looks at the origin
                          glm::vec3(0,-1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                          );
    info.Model = glm::mat4(1.0f);
    info.MVP = info.Projection * info.View * info.Model;

    /* VULKAN_KEY_START */
    VkBufferCreateInfo buf_info = {};
    buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buf_info.pNext = NULL;
    buf_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    buf_info.size = sizeof(info.MVP);
    buf_info.queueFamilyIndexCount = 0;
    buf_info.pQueueFamilyIndices = NULL;
    buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buf_info.flags = 0;
    res = vkCreateBuffer(info.device, &buf_info, NULL, &info.uniform_data.buf);
    assert(res == VK_SUCCESS);

    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(info.device, info.uniform_data.buf, &mem_reqs);

    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.pNext = NULL;
    alloc_info.memoryTypeIndex = 0;

    alloc_info.allocationSize = mem_reqs.size;
    pass = memory_type_from_properties(info,
                                      mem_reqs.memoryTypeBits,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                      &alloc_info.memoryTypeIndex);
    assert(pass);

    res = vkAllocateMemory(info.device, &alloc_info, NULL, &(info.uniform_data.mem));
    assert(res == VK_SUCCESS);

    uint8_t *pData;
    res = vkMapMemory(info.device, info.uniform_data.mem, 0, mem_reqs.size, 0, (void **) &pData);
    assert(res == VK_SUCCESS);

    memcpy(pData, &info.MVP, sizeof(info.MVP));

    vkUnmapMemory(info.device, info.uniform_data.mem);

    res = vkBindBufferMemory(info.device,
            info.uniform_data.buf,
            info.uniform_data.mem, 0);
    assert(res == VK_SUCCESS);

    info.uniform_data.buffer_info.buffer = info.uniform_data.buf;
    info.uniform_data.buffer_info.offset = 0;
    info.uniform_data.buffer_info.range = sizeof(info.MVP);
}

void init_descriptor_and_pipeline_layouts(struct sample_info &info, bool use_texture)
{
    VkDescriptorSetLayoutBinding layout_bindings[2];
    layout_bindings[0].binding = 0;
    layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_bindings[0].descriptorCount = 1;
    layout_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layout_bindings[0].pImmutableSamplers = NULL;

    if (use_texture)
    {
        layout_bindings[1].binding = 1;
        layout_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        layout_bindings[1].descriptorCount = 1;
        layout_bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        layout_bindings[1].pImmutableSamplers = NULL;
    }

    /* Next take layout bindings and use them to create a descriptor set layout */
    VkDescriptorSetLayoutCreateInfo descriptor_layout = {};
    descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_layout.pNext = NULL;
    descriptor_layout.bindingCount = use_texture?2:1;
    descriptor_layout.pBinding = layout_bindings;

    VkResult U_ASSERT_ONLY res;

    info.desc_layout.resize(NUM_DESCRIPTOR_SETS);
    res = vkCreateDescriptorSetLayout(info.device,
            &descriptor_layout, NULL, info.desc_layout.data());
    assert(res == VK_SUCCESS);

    /* Now use the descriptor layout to create a pipeline layout */
    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
    pPipelineLayoutCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pPipelineLayoutCreateInfo.pNext                  = NULL;
    pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pPipelineLayoutCreateInfo.pPushConstantRanges    = NULL;
    pPipelineLayoutCreateInfo.setLayoutCount         = NUM_DESCRIPTOR_SETS;
    pPipelineLayoutCreateInfo.pSetLayouts            = info.desc_layout.data();

    res = vkCreatePipelineLayout(info.device,
                                 &pPipelineLayoutCreateInfo, NULL,
                                 &info.pipeline_layout);
    assert(res == VK_SUCCESS);
}

void init_renderpass(struct sample_info &info, bool include_depth, bool clear)
{
    /* DEPENDS on init_swap_chain() and init_depth_buffer() */

    VkResult U_ASSERT_ONLY res;
    /* Need attachments for render target and depth buffer */
    VkAttachmentDescription attachments[2];
    attachments[0].format = info.format;
    attachments[0].samples = NUM_SAMPLES;
    attachments[0].loadOp = clear?VK_ATTACHMENT_LOAD_OP_CLEAR:VK_ATTACHMENT_LOAD_OP_LOAD;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachments[0].flags = 0;

    if (include_depth)
    {
        attachments[1].format = info.depth.format;
        attachments[1].samples = NUM_SAMPLES;
        attachments[1].loadOp = clear?VK_ATTACHMENT_LOAD_OP_CLEAR:VK_ATTACHMENT_LOAD_OP_LOAD;
        attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachments[1].flags = 0;
    }

    VkAttachmentReference color_reference = {};
    color_reference.attachment = 0;
    color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_reference = {};
    depth_reference.attachment = 1;
    depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.flags = 0;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = NULL;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_reference;
    subpass.pResolveAttachments = NULL;
    subpass.pDepthStencilAttachment = include_depth?&depth_reference:NULL;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = NULL;

    VkRenderPassCreateInfo rp_info = {};
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rp_info.pNext = NULL;
    rp_info.attachmentCount = include_depth?2:1;
    rp_info.pAttachments = attachments;
    rp_info.subpassCount = 1;
    rp_info.pSubpasses = &subpass;
    rp_info.dependencyCount = 0;
    rp_info.pDependencies = NULL;

    res = vkCreateRenderPass(info.device, &rp_info, NULL, &info.render_pass);
    assert(res == VK_SUCCESS);
}

void init_framebuffers(struct sample_info &info, bool include_depth)
{
    /* DEPENDS on init_depth_buffer(), init_renderpass() and init_swapchain_extension() */

    VkResult U_ASSERT_ONLY res;
    VkImageView attachments[2];
    attachments[1] = info.depth.view;

    VkFramebufferCreateInfo fb_info = {};
    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.pNext = NULL;
    fb_info.renderPass = info.render_pass;
    fb_info.attachmentCount = include_depth?2:1;
    fb_info.pAttachments = attachments;
    fb_info.width  = info.width;
    fb_info.height = info.height;
    fb_info.layers = 1;

    uint32_t i;

    info.framebuffers = (VkFramebuffer *) malloc(info.swapchainImageCount * sizeof(VkFramebuffer));

    for (i = 0; i < info.swapchainImageCount; i++) {
        attachments[0] = info.buffers[i].view;
        res = vkCreateFramebuffer(info.device, &fb_info, NULL, &info.framebuffers[i]);
        assert(res == VK_SUCCESS);
    }
}
void init_command_pool(struct sample_info &info)
{
    /* DEPENDS on init_swapchain_extension() */
    VkResult U_ASSERT_ONLY res;

    VkCommandPoolCreateInfo cmd_pool_info = {};
    cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_info.pNext = NULL;
    cmd_pool_info.queueFamilyIndex = info.graphics_queue_family_index;
    cmd_pool_info.flags = 0;

    res = vkCreateCommandPool(info.device, &cmd_pool_info, NULL, &info.cmd_pool);
    assert(res == VK_SUCCESS);
}

void init_command_buffer(struct sample_info &info)
{
    /* DEPENDS on init_swapchain_extension() and init_command_pool() */
    VkResult U_ASSERT_ONLY res;

    VkCommandBufferAllocateInfo cmd = {};
    cmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd.pNext = NULL;
    cmd.commandPool = info.cmd_pool;
    cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd.bufferCount = 1;

    res = vkAllocateCommandBuffers(info.device, &cmd, &info.cmd);
    assert(res == VK_SUCCESS);
}
void execute_begin_command_buffer(struct sample_info &info)
{
    /* DEPENDS on init_command_buffer() */
    VkResult U_ASSERT_ONLY res;
    VkCommandBufferBeginInfo cmd_buf_info = {};
    cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_buf_info.pNext = NULL;
    cmd_buf_info.renderPass = VK_NULL_HANDLE;  /* May only set renderPass and framebuffer */
    cmd_buf_info.framebuffer = VK_NULL_HANDLE; /* for secondary command buffers           */
    cmd_buf_info.flags = 0;
    cmd_buf_info.subpass = 0;
    cmd_buf_info.occlusionQueryEnable = VK_FALSE;
    cmd_buf_info.queryFlags = 0;
    cmd_buf_info.pipelineStatistics = 0;
    res = vkBeginCommandBuffer(info.cmd, &cmd_buf_info);
    assert(res == VK_SUCCESS);
}

void execute_end_command_buffer(struct sample_info &info)
{
    VkResult U_ASSERT_ONLY res;

    res = vkEndCommandBuffer(info.cmd);
    assert(res == VK_SUCCESS);
}

void execute_queue_command_buffer(struct sample_info &info)
{
    VkResult U_ASSERT_ONLY res;

    /* Queue the command buffer for execution */
    const VkCommandBuffer cmd_bufs[] = { info.cmd };
    VkFenceCreateInfo fenceInfo;
    VkFence drawFence;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = NULL;
    fenceInfo.flags = 0;
    vkCreateFence(info.device, &fenceInfo, NULL, &drawFence);

    VkSubmitInfo submit_info[1] = {};
    submit_info[0].pNext = NULL;
    submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info[0].waitSemaphoreCount = 0;
    submit_info[0].pWaitSemaphores = NULL;
    submit_info[0].commandBufferCount = 1;
    submit_info[0].pCommandBuffers = cmd_bufs;
    submit_info[0].signalSemaphoreCount = 0;
    submit_info[0].pSignalSemaphores = NULL;

    res = vkQueueSubmit(info.queue, 1, submit_info, drawFence);
    assert(res == VK_SUCCESS);

    res = vkWaitForFences(info.device, 1, &drawFence, VK_TRUE, FENCE_TIMEOUT);
    assert(res == VK_SUCCESS);

    vkDestroyFence(info.device, drawFence, NULL);
}

void init_device_queue(struct sample_info &info)
{
    /* DEPENDS on init_swapchain_extension() */

    vkGetDeviceQueue(info.device, info.graphics_queue_family_index,
            0, &info.queue);
}

void init_vertex_buffer(struct sample_info &info, const void *vertexData, uint32_t dataSize, uint32_t dataStride, bool use_texture)
{
    VkResult U_ASSERT_ONLY res;
    bool U_ASSERT_ONLY pass;

    VkBufferCreateInfo buf_info = {};
    buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buf_info.pNext = NULL;
    buf_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    buf_info.size = dataSize;
    buf_info.queueFamilyIndexCount = 0;
    buf_info.pQueueFamilyIndices = NULL;
    buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buf_info.flags = 0;
    res = vkCreateBuffer(info.device, &buf_info, NULL, &info.vertex_buffer.buf);
    assert(res == VK_SUCCESS);

    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(info.device, info.vertex_buffer.buf, &mem_reqs);

    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.pNext = NULL;
    alloc_info.memoryTypeIndex = 0;

    alloc_info.allocationSize = mem_reqs.size;
    pass = memory_type_from_properties(info,
                                      mem_reqs.memoryTypeBits,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                      &alloc_info.memoryTypeIndex);
    assert(pass);

    res = vkAllocateMemory(info.device, &alloc_info, NULL, &(info.vertex_buffer.mem));
    assert(res == VK_SUCCESS);

    uint8_t *pData;
    res = vkMapMemory(info.device, info.vertex_buffer.mem, 0, mem_reqs.size, 0, (void **) &pData);
    assert(res == VK_SUCCESS);

    memcpy(pData, vertexData, dataSize);

    vkUnmapMemory(info.device, info.vertex_buffer.mem);

    res = vkBindBufferMemory(info.device,
            info.vertex_buffer.buf,
            info.vertex_buffer.mem, 0);
    assert(res == VK_SUCCESS);

    info.vi_binding.binding = 0;
    info.vi_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    info.vi_binding.stride = dataStride;

    info.vi_attribs[0].binding = 0;
    info.vi_attribs[0].location = 0;
    info.vi_attribs[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    info.vi_attribs[0].offset = 0;
    info.vi_attribs[1].binding = 0;
    info.vi_attribs[1].location = 1;
    info.vi_attribs[1].format = use_texture?VK_FORMAT_R32G32_SFLOAT:VK_FORMAT_R32G32B32A32_SFLOAT;
    info.vi_attribs[1].offset = 16;
}

void init_descriptor_pool(struct sample_info &info, bool use_texture)
{
    /* DEPENDS on init_uniform_buffer() and init_descriptor_and_pipeline_layouts() */

    VkResult U_ASSERT_ONLY res;
    VkDescriptorPoolSize type_count[2];
    type_count[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    type_count[0].descriptorCount = 1;
    if (use_texture)
    {
        type_count[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        type_count[1].descriptorCount = 1;
    }

    VkDescriptorPoolCreateInfo descriptor_pool = {};
    descriptor_pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool.pNext = NULL;
    descriptor_pool.maxSets = 1;
    descriptor_pool.poolSizeCount = use_texture?2:1;
    descriptor_pool.pPoolSizes = type_count;

    res = vkCreateDescriptorPool(info.device,
        &descriptor_pool, NULL, &info.desc_pool);
    assert(res == VK_SUCCESS);
}

void init_descriptor_set(struct sample_info &info, bool use_texture)
{
    /* DEPENDS on init_descriptor_pool() */

    VkResult U_ASSERT_ONLY res;

    VkDescriptorSetAllocateInfo alloc_info[1];
    alloc_info[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info[0].pNext = NULL;
    alloc_info[0].descriptorPool = info.desc_pool;
    alloc_info[0].setLayoutCount = NUM_DESCRIPTOR_SETS;
    alloc_info[0].pSetLayouts = info.desc_layout.data();

    info.desc_set.resize(NUM_DESCRIPTOR_SETS);
    res = vkAllocateDescriptorSets(info.device, alloc_info, info.desc_set.data());
    assert(res == VK_SUCCESS);

    VkWriteDescriptorSet writes[2];

    writes[0] = {};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext = NULL;
    writes[0].dstSet = info.desc_set[0];
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &info.uniform_data.buffer_info;
    writes[0].dstArrayElement = 0;
    writes[0].dstBinding = 0;

    if (use_texture)
    {
        writes[1] = {};
        writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[1].dstSet = info.desc_set[0];
        writes[1].dstBinding = 1;
        writes[1].descriptorCount = 1;
        writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes[1].pImageInfo = &info.texture_data.image_info;
        writes[1].dstArrayElement = 0;
    }

    vkUpdateDescriptorSets(info.device, use_texture?2:1, writes, 0, NULL);
}

void init_shaders(struct sample_info &info, const char *vertShaderText, const char *fragShaderText)
{
    VkResult U_ASSERT_ONLY res;
    bool U_ASSERT_ONLY retVal;

    std::vector<unsigned int> vtx_spv;
    info.shaderStages[0].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.shaderStages[0].pNext  = NULL;
    info.shaderStages[0].pSpecializationInfo = NULL;
    info.shaderStages[0].flags = 0;
    info.shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    info.shaderStages[0].pName = "main";

    init_glslang();
    retVal = GLSLtoSPV(VK_SHADER_STAGE_VERTEX_BIT, vertShaderText, vtx_spv);
    assert(retVal);

    VkShaderModuleCreateInfo moduleCreateInfo;
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = NULL;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = vtx_spv.size() * sizeof(unsigned int);
    moduleCreateInfo.pCode = vtx_spv.data();
    res = vkCreateShaderModule(info.device, &moduleCreateInfo, NULL, &info.shaderStages[0].module);
    assert(res == VK_SUCCESS);

    std::vector<unsigned int> frag_spv;
    info.shaderStages[1].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.shaderStages[1].pNext  = NULL;
    info.shaderStages[1].pSpecializationInfo = NULL;
    info.shaderStages[1].flags = 0;
    info.shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    info.shaderStages[1].pName = "main";

    retVal = GLSLtoSPV(VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderText, frag_spv);
    assert(retVal);

    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = NULL;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = frag_spv.size() * sizeof(unsigned int);
    moduleCreateInfo.pCode = frag_spv.data();
    res = vkCreateShaderModule(info.device, &moduleCreateInfo, NULL, &info.shaderStages[1].module);
    assert(res == VK_SUCCESS);

    finalize_glslang();
}

void init_pipeline_cache(struct sample_info &info)
{
    VkResult U_ASSERT_ONLY res;

    VkPipelineCacheCreateInfo pipelineCache;
    pipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    pipelineCache.pNext = NULL;
    pipelineCache.initialDataSize = 0;
    pipelineCache.pInitialData = NULL;
    pipelineCache.flags = 0;
    res = vkCreatePipelineCache(info.device, &pipelineCache, NULL, &info.pipelineCache);
    assert(res == VK_SUCCESS);
}

void init_pipeline(struct sample_info &info, VkBool32 include_depth, VkBool32 include_vi)
{
    VkResult U_ASSERT_ONLY res;

    VkDynamicState                         dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
    VkPipelineDynamicStateCreateInfo       dynamicState = {};
    memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = NULL;
    dynamicState.pDynamicStates = dynamicStateEnables;
    dynamicState.dynamicStateCount = 0;

    VkPipelineVertexInputStateCreateInfo vi;
    vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vi.pNext = NULL;
    vi.flags = 0;
    vi.vertexBindingDescriptionCount = 1;
    vi.pVertexBindingDescriptions = &info.vi_binding;
    vi.vertexAttributeDescriptionCount = 2;
    vi.pVertexAttributeDescriptions = info.vi_attribs;

    VkPipelineInputAssemblyStateCreateInfo ia;
    ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia.pNext = NULL;
    ia.flags = 0;
    ia.primitiveRestartEnable = VK_FALSE;
    ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineRasterizationStateCreateInfo rs;
    rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.pNext = NULL;
    rs.flags = 0;
    rs.polygonMode = VK_POLYGON_MODE_FILL;
    rs.cullMode = VK_CULL_MODE_BACK_BIT;
    rs.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rs.depthClampEnable = include_depth;
    rs.rasterizerDiscardEnable = VK_FALSE;
    rs.depthBiasEnable = VK_FALSE;
    rs.depthBiasConstantFactor = 0;
    rs.depthBiasClamp = 0;
    rs.depthBiasSlopeFactor = 0;
    rs.lineWidth = 0;

    VkPipelineColorBlendStateCreateInfo cb;
    cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    cb.flags = 0;
    cb.pNext = NULL;
    VkPipelineColorBlendAttachmentState att_state[1];
    att_state[0].colorWriteMask = 0xf;
    att_state[0].blendEnable = VK_FALSE;
    att_state[0].alphaBlendOp = VK_BLEND_OP_ADD;
    att_state[0].colorBlendOp = VK_BLEND_OP_ADD;
    att_state[0].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    att_state[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    att_state[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    att_state[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    cb.attachmentCount = 1;
    cb.pAttachments = att_state;
    cb.logicOpEnable = VK_FALSE;
    cb.logicOp = VK_LOGIC_OP_NO_OP;
    cb.blendConstants[0] = 1.0f;
    cb.blendConstants[1] = 1.0f;
    cb.blendConstants[2] = 1.0f;
    cb.blendConstants[3] = 1.0f;

    VkPipelineViewportStateCreateInfo vp = {};
    vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vp.pNext = NULL;
    vp.flags = 0;
    vp.viewportCount = NUM_VIEWPORTS;
    dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
    vp.scissorCount = NUM_SCISSORS;
    dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;
    vp.pScissors = NULL;
    vp.pViewports = NULL;

    VkPipelineDepthStencilStateCreateInfo ds;
    ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ds.pNext = NULL;
    ds.flags = 0;
    ds.depthTestEnable = include_depth;
    ds.depthWriteEnable = include_depth;
    ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    ds.depthBoundsTestEnable = VK_FALSE;
    ds.stencilTestEnable = VK_FALSE;
    ds.back.failOp = VK_STENCIL_OP_KEEP;
    ds.back.passOp = VK_STENCIL_OP_KEEP;
    ds.back.compareOp = VK_COMPARE_OP_ALWAYS;
    ds.back.compareMask = 0;
    ds.back.reference = 0;
    ds.back.depthFailOp = VK_STENCIL_OP_KEEP;
    ds.back.writeMask = 0;
    ds.minDepthBounds = 0;
    ds.maxDepthBounds = 0;
    ds.stencilTestEnable = VK_FALSE;
    ds.front = ds.back;

    VkPipelineMultisampleStateCreateInfo   ms;
    ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ms.pNext = NULL;
    ms.flags = 0;
    ms.pSampleMask = NULL;
    ms.rasterizationSamples = NUM_SAMPLES;
    ms.sampleShadingEnable = VK_FALSE;
    ms.alphaToCoverageEnable = VK_FALSE;
    ms.alphaToOneEnable = VK_FALSE;
    ms.minSampleShading = 0.0;

    VkGraphicsPipelineCreateInfo pipeline;
    pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline.pNext               = NULL;
    pipeline.layout              = info.pipeline_layout;
    pipeline.basePipelineHandle  = 0;
    pipeline.basePipelineIndex   = 0;
    pipeline.flags               = 0;
    pipeline.pVertexInputState   = include_vi?&vi:NULL;
    pipeline.pInputAssemblyState = &ia;
    pipeline.pRasterizationState = &rs;
    pipeline.pColorBlendState    = &cb;
    pipeline.pTessellationState  = NULL;
    pipeline.pMultisampleState   = &ms;
    pipeline.pDynamicState       = &dynamicState;
    pipeline.pViewportState      = &vp;
    pipeline.pDepthStencilState  = &ds;
    pipeline.pStages             = info.shaderStages;
    pipeline.stageCount          = 2;
    pipeline.renderPass          = info.render_pass;
    pipeline.subpass             = 0;

    res = vkCreateGraphicsPipelines(info.device, info.pipelineCache, 1, &pipeline, NULL, &info.pipeline);
    assert(res == VK_SUCCESS);
}

void init_sampler(struct sample_info &info, VkSampler &sampler)
{
    VkResult U_ASSERT_ONLY res;

    VkSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
    samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_BASE;
    samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCreateInfo.mipLodBias = 0.0;
    samplerCreateInfo.maxAnisotropy = 0;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerCreateInfo.minLod = 0.0;
    samplerCreateInfo.maxLod = 0.0;
    samplerCreateInfo.compareEnable = VK_FALSE;
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

    /* create sampler */
    res = vkCreateSampler(info.device, &samplerCreateInfo, NULL, &sampler);
    assert(res == VK_SUCCESS);
}

void init_image(struct sample_info &info, texture_object &texObj, const char* textureName)
{
    VkResult U_ASSERT_ONLY res;
    bool U_ASSERT_ONLY pass;
    std::string filename = get_base_data_dir();

    if (textureName == nullptr)
        filename.append("lunarg.ppm");
    else
        filename.append(textureName);

    if (!read_ppm(filename.c_str(), texObj.tex_width, texObj.tex_height, 0, NULL))
    {
        std::cout << "Could not read texture file lunarg.ppm\n";
        exit(-1);
    }

    VkFormatProperties formatProps;
    vkGetPhysicalDeviceFormatProperties(info.gpus[0], VK_FORMAT_R8G8B8A8_UNORM, &formatProps);

    /* See if we can use a linear tiled image for a texture, if not, we will need a staging image for the texture data */
    bool needStaging = (!(formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))?true:false;

    VkImageCreateInfo image_create_info = {};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.pNext = NULL;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
    image_create_info.extent.width = texObj.tex_width;
    image_create_info.extent.height = texObj.tex_height;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.samples = NUM_SAMPLES;
    image_create_info.tiling = VK_IMAGE_TILING_LINEAR;
    image_create_info.usage = needStaging?VK_IMAGE_USAGE_TRANSFER_SRC_BIT:
                                          VK_IMAGE_USAGE_SAMPLED_BIT;
    image_create_info.queueFamilyIndexCount = 0;
    image_create_info.pQueueFamilyIndices = NULL;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_create_info.flags = 0;


    VkMemoryAllocateInfo mem_alloc = {};
    mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc.pNext = NULL;
    mem_alloc.allocationSize = 0;
    mem_alloc.memoryTypeIndex = 0;

    VkImage mappableImage;
    VkDeviceMemory mappableMemory;

    VkMemoryRequirements mem_reqs;

    /* Create a mappable image.  It will be the texture if linear images are ok to be textures */
    /* or it will be the staging image if they are not.                                        */
    res = vkCreateImage(info.device, &image_create_info, NULL,
            &mappableImage);
    assert(res == VK_SUCCESS);

    vkGetImageMemoryRequirements(info.device, mappableImage, &mem_reqs);
    assert(res == VK_SUCCESS);

    mem_alloc.allocationSize = mem_reqs.size;

    /* Find the memory type that is host mappable */
    pass = memory_type_from_properties(info, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &mem_alloc.memoryTypeIndex);
    assert(pass);

    /* allocate memory */
    res = vkAllocateMemory(info.device, &mem_alloc, NULL,
                &(mappableMemory));
    assert(res == VK_SUCCESS);

    /* bind memory */
    res = vkBindImageMemory(info.device, mappableImage,
            mappableMemory, 0);
    assert(res == VK_SUCCESS);

    VkImageSubresource subres = {};
    subres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subres.mipLevel = 0;
    subres.arrayLayer = 0;

    VkSubresourceLayout layout;
    void *data;

    /* Get the subresource layout so we know what the row pitch is */
    vkGetImageSubresourceLayout(info.device, mappableImage, &subres, &layout);

    res = vkMapMemory(info.device, mappableMemory, 0, mem_reqs.size, 0, &data);
    assert(res == VK_SUCCESS);

    /* Read the ppm file into the mappable image's memory */
    if (!read_ppm(filename.c_str(), texObj.tex_width, texObj.tex_height, layout.rowPitch, (unsigned char *)data)) {
        std::cout << "Could not load texture file lunarg.ppm\n";
        exit(-1);
    }

    vkUnmapMemory(info.device, mappableMemory);

    if (!needStaging) {
        /* If we can use the linear tiled image as a texture, just do it */
        texObj.image = mappableImage;
        texObj.mem = mappableMemory;
        texObj.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        set_image_layout(info, texObj.image,
                               VK_IMAGE_ASPECT_COLOR_BIT,
                               VK_IMAGE_LAYOUT_UNDEFINED,
                               texObj.imageLayout);
    } else {
        /* The mappable image cannot be our texture, so create an optimally tiled image and blit to it */
        image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

        res = vkCreateImage(info.device, &image_create_info, NULL,
                &texObj.image);
        assert(res == VK_SUCCESS);

        vkGetImageMemoryRequirements(info.device, texObj.image, &mem_reqs);

        mem_alloc.allocationSize = mem_reqs.size;

        /* Find memory type - dont specify any mapping requirements */
        pass = memory_type_from_properties(info, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mem_alloc.memoryTypeIndex);
        assert(pass);

        /* allocate memory */
        res = vkAllocateMemory(info.device, &mem_alloc, NULL,
                    &texObj.mem);
        assert(res == VK_SUCCESS);

        /* bind memory */
        res = vkBindImageMemory(info.device, texObj.image,
                texObj.mem, 0);
        assert(res == VK_SUCCESS);

        /* Since we're going to blit from the mappable image, set its layout to SOURCE_OPTIMAL */
        /* Side effect is that this will create info.cmd                                       */
        set_image_layout(info, mappableImage,
                          VK_IMAGE_ASPECT_COLOR_BIT,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

        /* Since we're going to blit to the texture image, set its layout to DESTINATION_OPTIMAL */
        set_image_layout(info, texObj.image,
                          VK_IMAGE_ASPECT_COLOR_BIT,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        VkImageCopy copy_region;
        copy_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copy_region.srcSubresource.mipLevel = 0;
        copy_region.srcSubresource.baseArrayLayer = 0;
        copy_region.srcSubresource.layerCount = 1;
        copy_region.srcOffset.x = 0;
        copy_region.srcOffset.y = 0;
        copy_region.srcOffset.z = 0;
        copy_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copy_region.dstSubresource.mipLevel = 0;
        copy_region.dstSubresource.baseArrayLayer = 0;
        copy_region.dstSubresource.layerCount = 1;
        copy_region.dstOffset.x = 0;
        copy_region.dstOffset.y = 0;
        copy_region.dstOffset.z = 0;
        copy_region.extent.width = texObj.tex_width;
        copy_region.extent.height = texObj.tex_height;
        copy_region.extent.depth = 1;

        VkCommandBufferBeginInfo cmd_buf_info = {};
        cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmd_buf_info.pNext = NULL;
        cmd_buf_info.flags = 0;
        cmd_buf_info.renderPass = VK_NULL_HANDLE;
        cmd_buf_info.framebuffer = VK_NULL_HANDLE;
        cmd_buf_info.subpass = 0;
        cmd_buf_info.occlusionQueryEnable = VK_FALSE;
        cmd_buf_info.queryFlags = 0;
        cmd_buf_info.pipelineStatistics = 0;
        res = vkBeginCommandBuffer(info.cmd, &cmd_buf_info);

        res = vkBeginCommandBuffer(info.cmd, &cmd_buf_info);
        assert(res == VK_SUCCESS);

        /* Put the copy command into the command buffer */
        vkCmdCopyImage(info.cmd,
                        mappableImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                        texObj.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        1, &copy_region);

        res = vkEndCommandBuffer(info.cmd);
        assert(res == VK_SUCCESS);
        const VkCommandBuffer cmd_bufs[] = { info.cmd };
        VkFence nullFence = VK_NULL_HANDLE;

        VkSubmitInfo submit_info[1] = {};
        submit_info[0].pNext = NULL;
        submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info[0].waitSemaphoreCount = 0;
        submit_info[0].pWaitSemaphores = NULL;
        submit_info[0].commandBufferCount = 1;
        submit_info[0].pCommandBuffers = cmd_bufs;
        submit_info[0].signalSemaphoreCount = 0;
        submit_info[0].pSignalSemaphores = NULL;

        /* Queue the command buffer for execution */
        res = vkQueueSubmit(info.queue, 1, submit_info, nullFence);
        assert(res == VK_SUCCESS);

        /* Set the layout for the texture image from DESTINATION_OPTIMAL to SHADER_READ_ONLY */
        texObj.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        set_image_layout(info, texObj.image,
                               VK_IMAGE_ASPECT_COLOR_BIT,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               texObj.imageLayout);

        /* Release the resources for the staging image */
        vkFreeMemory(info.device, mappableMemory, NULL);
        vkDestroyImage(info.device, mappableImage, NULL);
    }



    VkImageViewCreateInfo view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.pNext = NULL;
    view_info.image = VK_NULL_HANDLE;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = VK_FORMAT_R8G8B8A8_UNORM;
    view_info.components.r = VK_COMPONENT_SWIZZLE_R;
    view_info.components.g = VK_COMPONENT_SWIZZLE_G;
    view_info.components.b = VK_COMPONENT_SWIZZLE_B;
    view_info.components.a = VK_COMPONENT_SWIZZLE_A;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    /* create image view */
    view_info.image = texObj.image;
    res = vkCreateImageView(info.device, &view_info, NULL,
            &texObj.view);
    assert(res == VK_SUCCESS);
}


void init_texture(struct sample_info &info, const char* textureName)
{
    struct texture_object texObj;

    /* create image */
    init_image(info, texObj, textureName);

    /* create sampler */
    init_sampler(info, texObj.sampler);

    info.textures.push_back(texObj);

    /* track a description of the texture */
    assert(info.textures.size() == 1);
    info.texture_data.image_info.imageView = info.textures[0].view;
    info.texture_data.image_info.sampler = info.textures[0].sampler;
    info.texture_data.image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
}

void init_viewports(struct sample_info &info)
{
    info.viewport.height = (float) info.height;
    info.viewport.width = (float) info.width;
    info.viewport.minDepth = (float) 0.0f;
    info.viewport.maxDepth = (float) 1.0f;
    info.viewport.x = 0;
    info.viewport.y = 0;
    vkCmdSetViewport(info.cmd, NUM_VIEWPORTS, &info.viewport);
}

void init_scissors(struct sample_info &info)
{
    info.scissor.extent.width = info.width;
    info.scissor.extent.height = info.height;
    info.scissor.offset.x = 0;
    info.scissor.offset.y = 0;
    vkCmdSetScissor(info.cmd, NUM_SCISSORS, &info.scissor);
}

void init_fence(struct sample_info &info, VkFence &fence)
{
    VkFenceCreateInfo fenceInfo;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = NULL;
    fenceInfo.flags = 0;
    vkCreateFence(info.device, &fenceInfo, NULL, &fence);
}

void init_submit_info(struct sample_info &info, VkSubmitInfo &submit_info)
{
    submit_info.pNext = NULL;
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &info.presentCompleteSemaphore;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &info.cmd;
    submit_info.signalSemaphoreCount = 0;
    submit_info.pSignalSemaphores = NULL;
}

void init_present_info(struct sample_info &info, VkPresentInfoKHR &present)
{
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pNext = NULL;
    present.swapchainCount = 1;
    present.pSwapchains = &info.swap_chain;
    present.pImageIndices = &info.current_buffer;
    present.pWaitSemaphores = NULL;
    present.waitSemaphoreCount = 0;
    present.pResults = NULL;
}

void init_clear_color_and_depth(struct sample_info &info, VkClearValue* clear_values)
{
    clear_values[0].color.float32[0] = 0.2f;
    clear_values[0].color.float32[1] = 0.2f;
    clear_values[0].color.float32[2] = 0.2f;
    clear_values[0].color.float32[3] = 0.2f;
    clear_values[1].depthStencil.depth = 1.0f;
    clear_values[1].depthStencil.stencil = 0;
}

void init_render_pass_begin_info(struct sample_info &info, VkRenderPassBeginInfo &rp_begin)
{
    rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin.pNext = NULL;
    rp_begin.renderPass = info.render_pass;
    rp_begin.framebuffer = info.framebuffers[info.current_buffer];
    rp_begin.renderArea.offset.x = 0;
    rp_begin.renderArea.offset.y = 0;
    rp_begin.renderArea.extent.width = info.width;
    rp_begin.renderArea.extent.height = info.height;
    rp_begin.clearValueCount = 0;
    rp_begin.pClearValues = nullptr;
}

void destroy_pipeline(struct sample_info &info)
{
    vkDestroyPipeline(info.device, info.pipeline, NULL);
}

void destroy_pipeline_cache(struct sample_info &info)
{
    vkDestroyPipelineCache(info.device, info.pipelineCache, NULL);
}

void destroy_uniform_buffer(struct sample_info &info)
{
    vkDestroyBuffer(info.device, info.uniform_data.buf, NULL);
    vkFreeMemory(info.device, info.uniform_data.mem, NULL);
}

void destroy_descriptor_and_pipeline_layouts(struct sample_info &info)
{
    for(int i = 0; i < NUM_DESCRIPTOR_SETS; i++)
        vkDestroyDescriptorSetLayout(info.device, info.desc_layout[i], NULL);
    vkDestroyPipelineLayout(info.device, info.pipeline_layout, NULL);
}

void destroy_descriptor_pool(struct sample_info &info)
{
   vkDestroyDescriptorPool(info.device, info.desc_pool, NULL);
}

void destroy_shaders(struct sample_info &info)
{
    vkDestroyShaderModule(info.device, info.shaderStages[0].module, NULL);
    vkDestroyShaderModule(info.device, info.shaderStages[1].module, NULL);
}

void destroy_command_buffer(struct sample_info &info)
{
    VkCommandBuffer cmd_bufs[1] = { info.cmd };
    vkFreeCommandBuffers(info.device, info.cmd_pool, 1, cmd_bufs);
}

void destroy_command_pool(struct sample_info &info)
{
    vkDestroyCommandPool(info.device, info.cmd_pool, NULL);
}

void destroy_depth_buffer(struct sample_info &info)
{
    vkDestroyImageView(info.device, info.depth.view, NULL);
    vkDestroyImage(info.device, info.depth.image, NULL);
    vkFreeMemory(info.device, info.depth.mem, NULL);
}

void destroy_vertex_buffer(struct sample_info &info)
{
    vkDestroyBuffer(info.device, info.vertex_buffer.buf, NULL);
    vkFreeMemory(info.device, info.vertex_buffer.mem, NULL);
}

void destroy_swap_chain(struct sample_info &info)
{
    for (uint32_t i = 0; i < info.swapchainImageCount; i++) {
        vkDestroyImageView(info.device, info.buffers[i].view, NULL);
    }
    info.fpDestroySwapchainKHR(info.device, info.swap_chain, NULL);
}

void destroy_framebuffers(struct sample_info &info)
{
    for (uint32_t i = 0; i < info.swapchainImageCount; i++) {
        vkDestroyFramebuffer(info.device, info.framebuffers[i], NULL);
    }
    free(info.framebuffers);

}

void destroy_renderpass(struct sample_info &info)
{
    vkDestroyRenderPass(info.device, info.render_pass, NULL);
}

void destroy_device(struct sample_info &info)
{
    vkDestroyDevice(info.device, NULL);
}

void destroy_instance(struct sample_info &info)
{
    vkDestroyInstance(info.inst, NULL);
}

void destroy_texture(struct sample_info &info)
{
    vkDestroySampler(info.device, info.textures[0].sampler, NULL);
    vkDestroyImageView(info.device, info.textures[0].view, NULL);
    vkDestroyImage(info.device, info.textures[0].image, NULL);
    vkFreeMemory(info.device, info.textures[0].mem, NULL);
}
