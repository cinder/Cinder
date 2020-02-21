#include "cinder/CinderImGui.h"
#include "imgui/imgui_impl_opengl3.h"

#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/app/Window.h"
#include "cinder/app/MouseEvent.h"
#include "cinder/app/KeyEvent.h"
#include "cinder/gl/Texture.h"

using namespace ci;
using namespace ci::app;

static bool sTriggerNewFrame = true;
std::vector<int> sAccelKeys;
std::unordered_map<Window*, signals::ConnectionList> sWindowConnections;

namespace {
	void newFrameGuard() {
		if( ! sTriggerNewFrame )
			return;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();
		sTriggerNewFrame = false;
	}

	void mouseDown( ci::app::MouseEvent& event )
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = app::toPixels( event.getPos() );
		if( event.isLeftDown() ) {
			io.MouseDown[0] = true;
			io.MouseDown[1] = false;
			io.MouseDown[2] = false;
		}
		else if( event.isRightDown() ) {
			io.MouseDown[0] = false;
			io.MouseDown[1] = true;
			io.MouseDown[2] = false;
		}
		else if( event.isMiddleDown() ) {
			io.MouseDown[0] = false;
			io.MouseDown[1] = false;
			io.MouseDown[2] = true;
		}

		event.setHandled( io.WantCaptureMouse );
	}

	void mouseMove( ci::app::MouseEvent& event )
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = toPixels( event.getPos() );

		event.setHandled( io.WantCaptureMouse );
	}

	void mouseDrag( ci::app::MouseEvent& event )
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = toPixels( event.getPos() );

		event.setHandled( io.WantCaptureMouse );
	}

	void mouseUp( ci::app::MouseEvent& event )
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[0] = false;
		io.MouseDown[1] = false;
		io.MouseDown[2] = false;

		event.setHandled( io.WantCaptureMouse );
	}

	void mouseWheel( ci::app::MouseEvent& event )
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel += event.getWheelIncrement();

		event.setHandled( io.WantCaptureMouse );
	}

	void keyDown( ci::app::KeyEvent& event )
	{
		ImGuiIO& io = ImGui::GetIO();

#if defined CINDER_LINUX
		auto character = event.getChar();
#else
		uint32_t character = event.getCharUtf32();
#endif

		io.KeysDown[event.getCode()] = true;

		if( !event.isAccelDown() && character > 0 && character <= 255 ) {
			io.AddInputCharacter( (char)character );
		}
		else if( event.getCode() != KeyEvent::KEY_LMETA
			&& event.getCode() != KeyEvent::KEY_RMETA
			&& event.isAccelDown()
			&& find( sAccelKeys.begin(), sAccelKeys.end(), event.getCode() ) == sAccelKeys.end() ) {
			sAccelKeys.push_back( event.getCode() );
		}

		io.KeyCtrl = io.KeysDown[KeyEvent::KEY_LCTRL] || io.KeysDown[KeyEvent::KEY_RCTRL];
		io.KeyShift = io.KeysDown[KeyEvent::KEY_LSHIFT] || io.KeysDown[KeyEvent::KEY_RSHIFT];
		io.KeyAlt = io.KeysDown[KeyEvent::KEY_LALT] || io.KeysDown[KeyEvent::KEY_RALT];
		io.KeySuper = io.KeysDown[KeyEvent::KEY_LMETA] || io.KeysDown[KeyEvent::KEY_RMETA] || io.KeysDown[KeyEvent::KEY_LSUPER] || io.KeysDown[KeyEvent::KEY_RSUPER];

		event.setHandled( io.WantCaptureKeyboard );
	}

	void keyUp( ci::app::KeyEvent& event )
	{
		ImGuiIO& io = ImGui::GetIO();

		io.KeysDown[event.getCode()] = false;

		for( auto key : sAccelKeys ) {
			io.KeysDown[key] = false;
		}
		sAccelKeys.clear();

		io.KeyCtrl = io.KeysDown[KeyEvent::KEY_LCTRL] || io.KeysDown[KeyEvent::KEY_RCTRL];
		io.KeyShift = io.KeysDown[KeyEvent::KEY_LSHIFT] || io.KeysDown[KeyEvent::KEY_RSHIFT];
		io.KeyAlt = io.KeysDown[KeyEvent::KEY_LALT] || io.KeysDown[KeyEvent::KEY_RALT];
		io.KeySuper = io.KeysDown[KeyEvent::KEY_LMETA] || io.KeysDown[KeyEvent::KEY_RMETA] || io.KeysDown[KeyEvent::KEY_LSUPER] || io.KeysDown[KeyEvent::KEY_RSUPER];

		event.setHandled( io.WantCaptureKeyboard );
	}

	void resize()
	{
		newFrameGuard();

		int w = ci::app::getWindow()->getWidth();
		int h = ci::app::getWindow()->getHeight();
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2( (float)w, (float)h );
	}
}

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

	void connectWindow( ci::app::WindowRef window )
	{
		ci::app::Window* windowPtr = window.get();
		if( sWindowConnections.count( windowPtr ) ) {
			CI_LOG_W( "Window is already connected." );
			return;
		}

		sWindowConnections[windowPtr] += window->getSignalMouseDown().connect( mouseDown );
		sWindowConnections[windowPtr] += window->getSignalMouseUp().connect( mouseUp );
		sWindowConnections[windowPtr] += window->getSignalMouseDrag().connect( mouseDrag );
		sWindowConnections[windowPtr] += window->getSignalMouseMove().connect( mouseMove );
		sWindowConnections[windowPtr] += window->getSignalMouseWheel().connect( mouseWheel );
		sWindowConnections[windowPtr] += window->getSignalKeyDown().connect( keyDown );
		sWindowConnections[windowPtr] += window->getSignalKeyUp().connect( keyUp );
		sWindowConnections[windowPtr] += window->getSignalResize().connect( resize );
		sWindowConnections[windowPtr] += window->getSignalClose().connect( [&] {
			sWindowConnections.erase( windowPtr );
		} );
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
}

void ImGui::Initialize( const ImGui::Options& options )
{
	IMGUI_CHECKVERSION();
	ImGuiContext* context = ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplOpenGL3_Init( "#version 150" );

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	//io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

	io.DisplaySize = ImVec2( static_cast<float>( app::getWindow()->getSize().x ), static_cast<float>( app::getWindow()->getSize().y ) );
	io.DeltaTime = 1.0f / 60.0f;
	io.IniFilename = ( getAssetPath( "" ) / "imgui.ini" ).string().c_str();
	io.WantCaptureMouse = true;
	io.KeyMap[ImGuiKey_Tab] = KeyEvent::KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = KeyEvent::KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = KeyEvent::KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = KeyEvent::KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = KeyEvent::KEY_DOWN;
	io.KeyMap[ImGuiKey_Home] = KeyEvent::KEY_HOME;
	io.KeyMap[ImGuiKey_End] = KeyEvent::KEY_END;
	io.KeyMap[ImGuiKey_Delete] = KeyEvent::KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = KeyEvent::KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = KeyEvent::KEY_RETURN;
	io.KeyMap[ImGuiKey_Escape] = KeyEvent::KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = KeyEvent::KEY_a;
	io.KeyMap[ImGuiKey_C] = KeyEvent::KEY_c;
	io.KeyMap[ImGuiKey_V] = KeyEvent::KEY_v;
	io.KeyMap[ImGuiKey_X] = KeyEvent::KEY_x;
	io.KeyMap[ImGuiKey_Y] = KeyEvent::KEY_y;
	io.KeyMap[ImGuiKey_Z] = KeyEvent::KEY_z;
	io.KeyMap[ImGuiKey_Insert] = KeyEvent::KEY_INSERT;
	io.KeyMap[ImGuiKey_Space] = KeyEvent::KEY_SPACE;

	// setup config file path
	static std::string path = ( getAssetPath( "" ) / "imgui.ini" ).string();
	if( ! options.getIniPath().empty() )
		path = options.getIniPath().string().c_str();
	io.IniFilename = path.c_str();

	auto window = options.getWindow();
	if( window ) {
		connectWindow( window );
		if( options.isAutoRenderEnabled() ) {
			sWindowConnections[window.get()] += window->getSignalPostDraw().connect( [] {
				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
				sTriggerNewFrame = true;
				App::get()->dispatchAsync( []() {
					newFrameGuard();
				} );
			} );
		}
		else {
			sTriggerNewFrame = false; //prevents resize() event from calling begin frame.
		}
	}
	
	app::App::get()->getSignalCleanup().connect( [context]() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext( context );
	} );
}

bool ImGui::DragFloat2( const char* label, glm::vec2* v2, float v_speed, float v_min, float v_max, const char* format, float power ) {
	return DragFloat2( label, glm::value_ptr( *v2 ), v_speed, v_min, v_max, format, power );
}

bool ImGui::DragFloat3( const char* label, glm::vec3* v3, float v_speed, float v_min, float v_max, const char* format, float power ) {
	return DragFloat3( label, glm::value_ptr( *v3 ), v_speed, v_min, v_max, format, power );
}

bool ImGui::DragFloat4( const char* label, glm::vec4* v4, float v_speed, float v_min, float v_max, const char* format, float power ) {
	return DragFloat4( label, glm::value_ptr( *v4 ), v_speed, v_min, v_max, format, power );
}

bool ImGui::DragInt2( const char* label, glm::ivec2* v2, float v_speed, int v_min, int v_max, const char* format ) {
	return DragInt2( label, glm::value_ptr( *v2 ), v_speed, v_min, v_max, format );
}

bool ImGui::DragInt3( const char* label, glm::ivec3* v3, float v_speed, int v_min, int v_max, const char* format ) {
	return DragInt3( label, glm::value_ptr( *v3 ), v_speed, v_min, v_max, format );
}

bool ImGui::DragInt4( const char* label, glm::ivec4* v4, float v_speed, int v_min, int v_max, const char* format ) {
	return DragInt4( label, glm::value_ptr( *v4 ), v_speed, v_min, v_max, format );
}

bool ImGui::SliderFloat2( const char* label, glm::vec2* v2, float v_min, float v_max, const char* format, float power ) {
	return SliderFloat2( label, glm::value_ptr( *v2 ), v_min, v_max, format, power );
}

bool ImGui::SliderFloat3( const char* label, glm::vec3* v3, float v_min, float v_max, const char* format, float power ) {
	return SliderFloat3( label, glm::value_ptr( *v3 ), v_min, v_max, format, power );
}

bool ImGui::SliderFloat4( const char* label, glm::vec4* v4, float v_min, float v_max, const char* format, float power ) {
	return SliderFloat4( label, glm::value_ptr( *v4 ), v_min, v_max, format, power );
}

bool ImGui::SliderInt2( const char* label, glm::ivec2* v2, int v_min, int v_max, const char* format ) {
	return SliderInt2( label, glm::value_ptr( *v2 ), v_min, v_max, format );
}

bool ImGui::SliderInt3( const char* label, glm::ivec3* v3, int v_min, int v_max, const char* format ) {
	return SliderInt3( label, glm::value_ptr( *v3 ), v_min, v_max, format );
}

bool ImGui::SliderInt4( const char* label, glm::ivec4* v4, int v_min, int v_max, const char* format ) {
	return SliderInt4( label, glm::value_ptr( *v4 ), v_min, v_max, format );
}

bool ImGui::InputInt2( const char* label, glm::ivec2* v2, ImGuiInputTextFlags flags ) {
	return InputInt2( label, glm::value_ptr( *v2 ), flags );
}

bool ImGui::InputInt3( const char* label, glm::ivec3* v3, ImGuiInputTextFlags flags ) {
	return InputInt3( label, glm::value_ptr( *v3 ), flags );
}

bool ImGui::InputInt4( const char* label, glm::ivec4* v4, ImGuiInputTextFlags flags ) {
	return InputInt4( label, glm::value_ptr( *v4 ), flags );
}

bool ImGui::ColorEdit3( const char* label, ci::Colorf* color, ImGuiColorEditFlags flags ) {
	return ColorEdit3( label, color->ptr(), flags );
}

bool ImGui::ColorEdit4( const char* label, ci::ColorAf* color, ImGuiColorEditFlags flags ) {
	return ColorEdit4( label, color->ptr(), flags );
}

bool ImGui::ColorPicker3( const char* label, ci::Colorf* color, ImGuiColorEditFlags flags ) {
	return ColorPicker3( label, color->ptr(), flags );
}

bool ImGui::ColorPicker4( const char* label, ci::ColorAf* color, ImGuiColorEditFlags flags ) {
	return ColorPicker4( label, color->ptr(), flags );
}

static auto vector_getter = []( void* vec, int idx, const char** out_text )
{
	auto& vector = *static_cast<std::vector<std::string>*>( vec );
	if( idx < 0 || idx >= static_cast<int>( vector.size() ) ) return false;
	*out_text = vector.at( idx ).c_str();
	return true;
};

bool ImGui::Combo( const char* label, int* currIndex, std::vector<std::string>& values )
{
	if( values.empty() ) return false;
	return Combo( label, currIndex, vector_getter, static_cast<void*>( &values ), static_cast<int>( values.size() ) );
}

bool ImGui::ListBox( const char* label, int* currIndex, std::vector<std::string>& values )
{
	if( values.empty() ) return false;
	return ListBox( label, currIndex, vector_getter, static_cast<void*>( &values ), static_cast<int>( values.size() ) );
}

bool ImGui::InputText( const char* label, std::string* buf, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data )
{
	// conversion
	char* buffer = new char[buf->size() + 128];
	std::strcpy( buffer, buf->c_str() );
	bool result = InputText( label, buffer, buf->size() + 128, flags, callback, user_data );
	if( result ) {
		*buf = std::string( buffer );
	}
	// cleanup
	delete[] buffer;
	return result;
}
bool ImGui::InputTextMultiline( const char* label, std::string* buf, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data )
{
	// conversion
	constexpr size_t extraSpace = 16384;
	char* buffer = new char[buf->size() + extraSpace];
	std::strcpy( buffer, buf->c_str() );
	bool result = InputTextMultiline( label, buffer, buf->size() + extraSpace, size, flags, callback, user_data );
	if( result ) {
		*buf = std::string( buffer );
	}
	// cleanup
	delete[] buffer;
	return result;
}

CI_API void ImGui::Image( const ci::gl::Texture2dRef& texture, const ci::vec2& size, const ci::vec2& uv0, const ci::vec2& uv1, const ci::vec4& tint_col, const ci::vec4& border_col )
{
	ImGui::Image( (ImTextureID)texture->getId(), size, uv0, uv1, tint_col, border_col );
}
