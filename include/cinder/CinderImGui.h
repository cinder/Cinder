/*
 Copyright (c) 2010, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#if ! defined( IMGUI_USER_CONFIG )
#define IMGUI_USER_CONFIG "cinder/CinderImGuiConfig.h"
#endif
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

#include "cinder/Filesystem.h"
#include "cinder/CinderGlm.h"
#include "cinder/Noncopyable.h"

#include <vector>

namespace cinder {
	namespace app {
		class Window;
		typedef std::shared_ptr<Window> WindowRef;
	}
	namespace gl {
		class Texture2d;
		typedef std::shared_ptr<Texture2d> Texture2dRef;
	}
}

//! Additional convenience initializaiont and overloads for cinder types
namespace ImGui {
	struct CI_API Options {
		//! Defaults to using the current window, the basic ImGui font and the dark theme
		Options();

		//! Sets the window that will be used to connect the signals and render ImGui
		Options& window( const ci::app::WindowRef& window, int signalPriority = 1 );
		//! Returns the window that will be use to connect the signals and render ImGui
		ci::app::WindowRef getWindow() const { return mWindow; }

		//! Specify whether the block should call ImGui::NewFrame and ImGui::Render automatically. Default to true.
		Options& autoRender( bool autoRender );
		//! returns whether the block should call ImGui::NewFrame and ImGui::Render automatically
		bool isAutoRenderEnabled() const { return mAutoRender; }

		//! Sets imgui ini file path
		Options& iniPath( const ci::fs::path& path );
		//! Returns imgui ini file path
		const ci::fs::path& getIniPath() const { return mIniPath; }

		//! Enables keyboard input. Default to true.
		Options& enableKeyboard( bool enable );
		//! Returns whether the keyboard input is enabled
		bool isKeyboardEnabled() const { return mKeyboardEnabled; }

		//! Enables gamepad input. Default to true.
		Options& enableGamepad( bool enable );
		//! Returns whether the gamepad input is enabled
		bool isGamepadEnabled() const { return mGamepadEnabled; }

		//! Sets the signal priority that will be use to connect the signals and render ImGui
		Options& signalPriority( int signalPriority );
		//! Returns the signal priority that will be use to connect the signals and render ImGui
		int getSignalPriority() const { return mSignalPriority; }

		//! Sets the ImGuiStyle used during initialization.
		Options& style( const ImGuiStyle& style );
		//! Returns the ImGuiStyle used during initialization.
		const ImGuiStyle& getStyle() const { return mStyle; }
	protected:
		bool							mAutoRender;
		bool							mKeyboardEnabled;
		bool							mGamepadEnabled;
		ImGuiStyle						mStyle;
		ci::app::WindowRef				mWindow;
		ci::fs::path					mIniPath;
		int								mSignalPriority;
	};

	//! Convenience ImGui initialization for cinder applications.
	//! By default, automatic rendering into ci::app::getWindow() will be used.
	//! In a multi-window context, only call ImGui in App::draw() if the active window matches the one
	//! used here for initialization, or in App::update() only if the this window is still open.
	CI_API bool Initialize( const Options& options = Options() );

	CI_API bool DragFloat2( const char* label, glm::vec2* v2, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f );
	CI_API bool DragFloat3( const char* label, glm::vec3* v2, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f );
	CI_API bool DragFloat4( const char* label, glm::vec4* v2, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f );

	CI_API bool DragInt2( const char* label, glm::ivec2* v2, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%.3f" );
	CI_API bool DragInt3( const char* label, glm::ivec3* v2, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%.3f" );
	CI_API bool DragInt4( const char* label, glm::ivec4* v2, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%.3f" );

	CI_API bool SliderFloat2( const char* label, glm::vec2* v2, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f );
	CI_API bool SliderFloat3( const char* label, glm::vec3* v2, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f );
	CI_API bool SliderFloat4( const char* label, glm::vec4* v2, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f );

	CI_API bool SliderInt2( const char* label, glm::ivec2* v2, int v_min, int v_max, const char* format = "%.3f" );
	CI_API bool SliderInt3( const char* label, glm::ivec3* v2, int v_min, int v_max, const char* format = "%.3f" );
	CI_API bool SliderInt4( const char* label, glm::ivec4* v2, int v_min, int v_max, const char* format = "%.3f" );

	CI_API bool InputInt2( const char* label, glm::ivec2* v2, ImGuiInputTextFlags flags = 0 );
	CI_API bool InputInt3( const char* label, glm::ivec3* v2, ImGuiInputTextFlags flags = 0 );
	CI_API bool InputInt4( const char* label, glm::ivec4* v2, ImGuiInputTextFlags flags = 0 );

	CI_API bool ColorEdit3( const char* label, ci::Colorf* color, ImGuiColorEditFlags flags = 0 );
	CI_API bool ColorEdit4( const char* label, ci::ColorAf* color, ImGuiColorEditFlags flags = 0 );

	CI_API bool ColorPicker3( const char* label, ci::Colorf* color, ImGuiColorEditFlags flags = 0 );
	CI_API bool ColorPicker4( const char* label, ci::ColorAf* color, ImGuiColorEditFlags flags = 0 );

	CI_API bool Combo( const char* label, int* currIndex, const std::vector<std::string>& values, ImGuiComboFlags flags = 0 );
	CI_API bool ListBox( const char* label, int* currIndex, const std::vector<std::string>& values, int height_in_items = -1 );

	CI_API void	Image( const ci::gl::Texture2dRef& texture, const ci::vec2& size, const ci::vec2& uv0 = ci::vec2( 0, 0 ), const ci::vec2& uv1 = ci::vec2( 1, 1 ), const ci::vec4& tint_col = ci::vec4( 1, 1, 1, 1 ), const ci::vec4& border_col = ci::vec4( 0, 0, 0, 0 ) );

	struct CI_API ScopedWindow : public ci::Noncopyable {
		ScopedWindow( const char* label );
		~ScopedWindow();
	};

	struct CI_API ScopedGroup : public ci::Noncopyable {
		ScopedGroup();
		~ScopedGroup();
	};

	struct CI_API ScopedTreeNode : public ci::Noncopyable {
		ScopedTreeNode( const std::string& name );
		~ScopedTreeNode();
		//! Returns true when tree node is not collapsed
		explicit operator bool() const { return mOpened; }
	protected:
		bool mOpened;
	};

	struct CI_API ScopedId : public ci::Noncopyable {
		ScopedId( int int_id );
		ScopedId( const char* label );
		~ScopedId();
	};

	struct CI_API ScopedMenuBar : public ci::Noncopyable {
		ScopedMenuBar();
		~ScopedMenuBar();
	protected:
		bool mOpened;
	};

	struct CI_API ScopedMainMenuBar : public ci::Noncopyable {
		ScopedMainMenuBar();
		~ScopedMainMenuBar();
	protected:
		bool mOpened;
	};

	struct CI_API ScopedColumns : public ci::Noncopyable {
		ScopedColumns( int count, const char* id = NULL, bool border = true );
		~ScopedColumns();
	};
}
