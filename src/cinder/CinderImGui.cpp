#include "cinder/CinderImGui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_cinder.h"

#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/app/Window.h"
#include "cinder/app/MouseEvent.h"
#include "cinder/app/KeyEvent.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Context.h"

using namespace ci;
using namespace ci::app;

namespace {

	static bool sTriggerNewFrame = true;
	std::vector<int> sAccelKeys;
	signals::ConnectionList sAppConnections;

	void newFrameGuard() {
		if( ! sTriggerNewFrame )
			return;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplCinder_NewFrame();
		ImGui::NewFrame();
		sTriggerNewFrame = false;
	}

	//! Used by Combo and ListBox below.
	static auto vector_getter = []( void* vec, int idx, const char** out_text )
	{
		auto& vector = *static_cast<std::vector<std::string>*>( vec );
		if( idx < 0 || idx >= static_cast<int>( vector.size() ) ) return false;
		*out_text = vector.at( idx ).c_str();
		return true;
	};
} // unnamed namespace

namespace ImGui {
	Options::Options()
		: mWindow( ci::app::getWindow() )
		, mAutoRender( true )
	{
	}

	Options& Options::window( const ci::app::WindowRef& window )
	{
		mWindow = window;
		return *this;
	}

	Options& Options::autoRender( bool autoRender )
	{
		mAutoRender = autoRender;
		return *this;
	}

	Options& Options::iniPath( const ci::fs::path& path )
	{
		mIniPath = path;
		return *this;
	}

	ScopedId::ScopedId( int int_id )
	{
		ImGui::PushID( int_id );
	}
	ScopedId::ScopedId( const char* label )
	{
		ImGui::PushID( label );
	}
	ScopedId::~ScopedId()
	{
		ImGui::PopID();
	}
	ScopedWindow::ScopedWindow( const char* label )
	{
		ImGui::Begin( label );
	}
	ScopedWindow::~ScopedWindow()
	{
		ImGui::End();
	}

	ScopedMainMenuBar::ScopedMainMenuBar() : mOpened{ ImGui::BeginMainMenuBar() } { }
	ScopedMainMenuBar::~ScopedMainMenuBar()
	{
		if( mOpened ) ImGui::EndMainMenuBar();
	}
	
	ScopedMenuBar::ScopedMenuBar() : mOpened{ ImGui::BeginMenuBar() } { }
	ScopedMenuBar::~ScopedMenuBar()
	{
		if( mOpened ) ImGui::EndMenuBar();
	}

	ScopedGroup::ScopedGroup()
	{
		ImGui::BeginGroup();
	}
	ScopedGroup::~ScopedGroup()
	{
		ImGui::EndGroup();
	}
	ScopedColumns::ScopedColumns( int count, const char* id, bool border )
	{
		ImGui::Columns( count, id, border );
	}
	ScopedColumns::~ScopedColumns()
	{
		ImGui::NextColumn();
	}
	ScopedTreeNode::ScopedTreeNode( const std::string& name )
		: mOpened( ImGui::TreeNode( name.c_str() ) )
	{
	}
	ScopedTreeNode::~ScopedTreeNode()
	{
		if( mOpened ) ImGui::TreePop();
	}

	bool DragFloat2( const char* label, glm::vec2* v2, float v_speed, float v_min, float v_max, const char* format, float power ) {
		return DragFloat2( label, glm::value_ptr( *v2 ), v_speed, v_min, v_max, format, power );
	}

	bool DragFloat3( const char* label, glm::vec3* v3, float v_speed, float v_min, float v_max, const char* format, float power ) {
		return DragFloat3( label, glm::value_ptr( *v3 ), v_speed, v_min, v_max, format, power );
	}

	bool DragFloat4( const char* label, glm::vec4* v4, float v_speed, float v_min, float v_max, const char* format, float power ) {
		return DragFloat4( label, glm::value_ptr( *v4 ), v_speed, v_min, v_max, format, power );
	}

	bool DragInt2( const char* label, glm::ivec2* v2, float v_speed, int v_min, int v_max, const char* format ) {
		return DragInt2( label, glm::value_ptr( *v2 ), v_speed, v_min, v_max, format );
	}

	bool DragInt3( const char* label, glm::ivec3* v3, float v_speed, int v_min, int v_max, const char* format ) {
		return DragInt3( label, glm::value_ptr( *v3 ), v_speed, v_min, v_max, format );
	}

	bool DragInt4( const char* label, glm::ivec4* v4, float v_speed, int v_min, int v_max, const char* format ) {
		return DragInt4( label, glm::value_ptr( *v4 ), v_speed, v_min, v_max, format );
	}

	bool SliderFloat2( const char* label, glm::vec2* v2, float v_min, float v_max, const char* format, float power ) {
		return SliderFloat2( label, glm::value_ptr( *v2 ), v_min, v_max, format, power );
	}

	bool SliderFloat3( const char* label, glm::vec3* v3, float v_min, float v_max, const char* format, float power ) {
		return SliderFloat3( label, glm::value_ptr( *v3 ), v_min, v_max, format, power );
	}

	bool SliderFloat4( const char* label, glm::vec4* v4, float v_min, float v_max, const char* format, float power ) {
		return SliderFloat4( label, glm::value_ptr( *v4 ), v_min, v_max, format, power );
	}

	bool SliderInt2( const char* label, glm::ivec2* v2, int v_min, int v_max, const char* format ) {
		return SliderInt2( label, glm::value_ptr( *v2 ), v_min, v_max, format );
	}

	bool SliderInt3( const char* label, glm::ivec3* v3, int v_min, int v_max, const char* format ) {
		return SliderInt3( label, glm::value_ptr( *v3 ), v_min, v_max, format );
	}

	bool SliderInt4( const char* label, glm::ivec4* v4, int v_min, int v_max, const char* format ) {
		return SliderInt4( label, glm::value_ptr( *v4 ), v_min, v_max, format );
	}

	bool InputInt2( const char* label, glm::ivec2* v2, ImGuiInputTextFlags flags ) {
		return InputInt2( label, glm::value_ptr( *v2 ), flags );
	}

	bool InputInt3( const char* label, glm::ivec3* v3, ImGuiInputTextFlags flags ) {
		return InputInt3( label, glm::value_ptr( *v3 ), flags );
	}

	bool InputInt4( const char* label, glm::ivec4* v4, ImGuiInputTextFlags flags ) {
		return InputInt4( label, glm::value_ptr( *v4 ), flags );
	}

	bool ColorEdit3( const char* label, ci::Colorf* color, ImGuiColorEditFlags flags ) {
		return ColorEdit3( label, color->ptr(), flags );
	}

	bool ColorEdit4( const char* label, ci::ColorAf* color, ImGuiColorEditFlags flags ) {
		return ColorEdit4( label, color->ptr(), flags );
	}

	bool ColorPicker3( const char* label, ci::Colorf* color, ImGuiColorEditFlags flags ) {
		return ColorPicker3( label, color->ptr(), flags );
	}

	bool ColorPicker4( const char* label, ci::ColorAf* color, ImGuiColorEditFlags flags ) {
		return ColorPicker4( label, color->ptr(), flags );
	}

	bool Combo( const char* label, int* currIndex, std::vector<std::string>& values )
	{
		if( values.empty() ) return false;
		return Combo( label, currIndex, vector_getter, static_cast<void*>( &values ), static_cast<int>( values.size() ) );
	}

	bool ListBox( const char* label, int* currIndex, std::vector<std::string>& values )
	{
		if( values.empty() ) return false;
		return ListBox( label, currIndex, vector_getter, static_cast<void*>( &values ), static_cast<int>( values.size() ) );
	}

	void Image( const ci::gl::Texture2dRef& texture, const ci::vec2& size, const ci::vec2& uv0, const ci::vec2& uv1, const ci::vec4& tint_col, const ci::vec4& border_col )
	{
		Image( (void*)(intptr_t)texture->getId(), size, uv0, uv1, tint_col, border_col );
	}
}

void ImGui::Initialize( const ImGui::Options& options )
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	WindowRef window = options.getWindow();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

	io.DisplaySize = ImVec2( static_cast<float>( ci::app::getWindow()->getSize().x ), static_cast<float>( ci::app::getWindow()->getSize().y ) );
	io.DeltaTime = 1.0f / 60.0f;
	io.WantCaptureMouse = true;

	// setup config file path
	static std::string path;
	if( options.getIniPath().empty() ) {
		path = ( getAssetPath( "" ) / "imgui.ini" ).string();
	}
	else {
		path = options.getIniPath().string().c_str();
	}
	io.IniFilename = path.c_str();

	ImGui_ImplCinder_Init( window, true );
	ImGui_ImplOpenGL3_Init( "#version 150" );

	if( options.isAutoRenderEnabled() ) {
		window->getSignalPostDraw().connect( [] {
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
			
			ImGuiIO& io = ImGui::GetIO();
			if( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
			{
				auto backup_current_context = gl::context()->getCurrent();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				gl::context()->makeCurrent( backup_current_context );
			}
			
			
			sTriggerNewFrame = true;
			App::get()->dispatchAsync( []() {
				newFrameGuard();
			} );
		} );
	}
	else {
		sTriggerNewFrame = false; //prevents resize() event from calling begin frame.
	}
	
	sAppConnections += app::App::get()->getSignalCleanup().connect( []() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplCinder_Shutdown();
		ImGui::DestroyContext();
	} );

	newFrameGuard();
}
