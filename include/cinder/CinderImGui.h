#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include "cinder/Filesystem.h"
#include "cinder/CinderGlm.h"
#include "cinder/Noncopyable.h"

#include <unordered_map>
#include <vector>

namespace cinder {
	namespace app { typedef std::shared_ptr<class Window> WindowRef; }
	namespace gl { typedef std::shared_ptr<class Texture2d> Texture2dRef; }
}

//! Additional convenience initializaiont and overloads for cinder types
namespace ImGui {
	struct CI_API Options {
		//! defaults to using the current window, the basic ImGui font and the dark theme
		Options();
		//! sets the window that will be used to connect the signals and render ImGui
		Options& window( const ci::app::WindowRef& window );
		//! species whether the block should call ImGui::NewFrame and ImGui::Render automatically. Default to true.
		Options& autoRender( bool autoRender );
		//! sets imgui ini file path
		Options& iniPath( const ci::fs::path& path );

		//! returns whether the block should call ImGui::NewFrame and ImGui::Render automatically
		bool isAutoRenderEnabled() const { return mAutoRender; }
		//! returns the window that will be use to connect the signals and render ImGui
		ci::app::WindowRef getWindow() const { return mWindow; }
		//! returns imgui ini file path
		const ci::fs::path& getIniPath() const { return mIniPath; }
	protected:
		bool							mAutoRender;
		ci::app::WindowRef				mWindow;
		ci::fs::path					mIniPath;
	};

	CI_API void Initialize( const Options& options = Options() );

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

	CI_API bool Combo( const char* label, int* currIndex, std::vector<std::string>& values );
	CI_API bool ListBox( const char* label, int* currIndex, std::vector<std::string>& values );

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
