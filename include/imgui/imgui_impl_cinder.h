// ImGui Platform Binding for: Cinder
// This needs to be used along with a Renderer (e.g. OpenGL3, Vulkan..)
// (Info: Cinder is a community-developed, free and open source library for professional-quality creative coding in C++. http://libcinder.org.)

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui
#pragma once

#include <memory>
#include "imgui.h"

namespace cinder { namespace app { typedef std::shared_ptr<class Window> WindowRef; } }

IMGUI_API bool		ImGui_ImplCinder_Init(const cinder::app::WindowRef &window, bool install_callbacks);
IMGUI_API void		ImGui_ImplCinder_Shutdown();
IMGUI_API void		ImGui_ImplCinder_NewFrame();